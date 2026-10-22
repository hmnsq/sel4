/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <api/invocation.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <api/types.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <object/objecttype.h>
#अन्तर्भाव <object/cnode.h>
#अन्तर्भाव <object/interrupt.h>
#अन्तर्भाव <object/untyped.h>
#अन्तर्भाव <kernel/cspace.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <model/preemption.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <util.h>

संरचना finaliseSlot_ret {
    exception_t status;
    bool_t success;
    cap_t cleanupInfo;
};
प्रकारव्याख्या संरचना finaliseSlot_ret finaliseSlot_ret_t;

स्थिर finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t exposed);
स्थिर शून्य emptySlot(cte_t *slot, cap_t cleanupInfo);
स्थिर exception_t reduceZombie(cte_t *slot, bool_t exposed);

#ifdef CONFIG_KERNEL_MCS
#define CNODE_LAST_INVOCATION CNodeRotate
#अन्यथा
#define CNODE_LAST_INVOCATION CNodeSaveCaller
#endif

exception_t decodeCNodeInvocation(word_t invLabel, word_t length, cap_t cap,
                                  word_t *buffer)
{
    lookupSlot_ret_t lu_ret;
    cte_t *destSlot;
    word_t index, w_bits;
    exception_t status;

    /* Haskell error: "decodeCNodeInvocation: invalid cap" */
    प्रतिज्ञा(cap_get_capType(cap) == cap_cnode_cap);

    जर (invLabel < CNodeRevoke || invLabel > CNODE_LAST_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        परत EXCEPTION_SYSCALL_ERROR;
    }

    जर (length < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    w_bits = getSyscallArg(1, buffer);

    lu_ret = lookupTargetSlot(cap, index, w_bits);
    जर (lu_ret.status != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        परत lu_ret.status;
    }
    destSlot = lu_ret.slot;

    जर (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *srcSlot;
        word_t srcIndex, srcDepth, capData;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_मूळ, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        जर (length < 4 || current_extra_caps.excaprefs[0] == रिक्त) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            परत EXCEPTION_SYSCALL_ERROR;
        }
        srcIndex = getSyscallArg(2, buffer);
        srcDepth = getSyscallArg(3, buffer);

        src_मूळ = current_extra_caps.excaprefs[0]->cap;

        status = ensureEmptySlot(destSlot);
        जर (status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            परत status;
        }

        lu_ret = lookupSourceSlot(src_मूळ, srcIndex, srcDepth);
        जर (lu_ret.status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            परत lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        जर (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(srcDepth);
            परत EXCEPTION_SYSCALL_ERROR;
        }

        निवड (invLabel) {
        स्थिती CNodeCopy:

            जर (length < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = seL4_TruncatedMessage;
                परत EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot, srcCap);
            जर (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                परत dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            थांबा;

        स्थिती CNodeMint:
            जर (length < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                परत EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            capData = getSyscallArg(5, buffer);
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot,
                               updateCapData(false, capData, srcCap));
            जर (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                परत dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            थांबा;

        स्थिती CNodeMove:
            newCap = srcSlot->cap;
            isMove = true;

            थांबा;

        स्थिती CNodeMutate:
            जर (length < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                परत EXCEPTION_SYSCALL_ERROR;
            }

            capData = getSyscallArg(4, buffer);
            newCap = updateCapData(true, capData, srcSlot->cap);
            isMove = true;

            थांबा;

        पूर्वनिर्धारित:
            प्रतिज्ञा(0);
            परत EXCEPTION_NONE;
        }

        जर (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            परत EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
        जर (isMove) {
            परत invokeCNodeMove(newCap, srcSlot, destSlot);
        } अन्यथा {
            परत invoke_CNode_घाला(newCap, srcSlot, destSlot);
        }
    }

    जर (invLabel == CNodeRevoke) {
        setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
        परत invokeCNodeRevoke(destSlot);
    }

    जर (invLabel == CNodeDelete) {
        setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
        परत invokeCNodeDelete(destSlot);
    }

#ifndef CONFIG_KERNEL_MCS
    जर (invLabel == CNodeSaveCaller) {
        status = ensureEmptySlot(destSlot);
        जर (status != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            परत status;
        }

        setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
        परत invokeCNodeSaveCaller(destSlot);
    }
#endif

    जर (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = destSlot->cap;

        जर (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            परत EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
        परत invokeCNodeCancelBadgedSends(destCap);
    }

    जर (invLabel == CNodeRotate) {
        word_t pivotNewData, pivotIndex, pivotDepth;
        word_t srcNewData, srcIndex, srcDepth;
        cte_t *pivotSlot, *srcSlot;
        cap_t pivot_मूळ, src_मूळ, newSrcCap, newPivotCap;

        जर (length < 8 || current_extra_caps.excaprefs[0] == रिक्त
            || current_extra_caps.excaprefs[1] == रिक्त) {
            current_syscall_error.type = seL4_TruncatedMessage;
            परत EXCEPTION_SYSCALL_ERROR;
        }
        pivotNewData = getSyscallArg(2, buffer);
        pivotIndex   = getSyscallArg(3, buffer);
        pivotDepth   = getSyscallArg(4, buffer);
        srcNewData   = getSyscallArg(5, buffer);
        srcIndex     = getSyscallArg(6, buffer);
        srcDepth     = getSyscallArg(7, buffer);

        pivot_मूळ = current_extra_caps.excaprefs[0]->cap;
        src_मूळ   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookupSourceSlot(src_मूळ, srcIndex, srcDepth);
        जर (lu_ret.status != EXCEPTION_NONE) {
            परत lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        lu_ret = lookupPivotSlot(pivot_मूळ, pivotIndex, pivotDepth);
        जर (lu_ret.status != EXCEPTION_NONE) {
            परत lu_ret.status;
        }
        pivotSlot = lu_ret.slot;

        जर (pivotSlot == srcSlot || pivotSlot == destSlot) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            परत EXCEPTION_SYSCALL_ERROR;
        }

        जर (srcSlot != destSlot) {
            status = ensureEmptySlot(destSlot);
            जर (status != EXCEPTION_NONE) {
                परत status;
            }
        }

        जर (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(srcDepth);
            परत EXCEPTION_SYSCALL_ERROR;
        }

        जर (cap_get_capType(pivotSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(pivotDepth);
            परत EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = updateCapData(true, srcNewData, srcSlot->cap);
        newPivotCap = updateCapData(true, pivotNewData, pivotSlot->cap);

        जर (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            परत EXCEPTION_SYSCALL_ERROR;
        }

        जर (cap_get_capType(newPivotCap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            परत EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
        परत invokeCNodeRotate(newSrcCap, newPivotCap,
                                 srcSlot, pivotSlot, destSlot);
    }

    परत EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *destSlot)
{
    परत cteRevoke(destSlot);
}

exception_t invokeCNodeDelete(cte_t *destSlot)
{
    परत cteDelete(destSlot, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    word_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    जर (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    परत EXCEPTION_NONE;
}

exception_t invoke_CNode_घाला(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cte_घाला(cap, srcSlot, destSlot);

    परत EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cteMove(cap, srcSlot, destSlot);

    परत EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    जर (slot1 == slot3) {
        cteSwap(cap1, slot1, cap2, slot2);
    } अन्यथा {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    परत EXCEPTION_NONE;
}

#ifndef CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *destSlot)
{
    cap_t cap;
    cte_t *srcSlot;

    srcSlot = TCB_PTR_CTE_PTR(गाठ_STATE(ksCurThread), tcbCaller);
    cap = srcSlot->cap;

    निवड (cap_get_capType(cap)) {
    स्थिती cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        थांबा;

    स्थिती cap_reply_cap:
        जर (!cap_reply_cap_get_capReplyMaster(cap)) {
            cteMove(cap, srcSlot, destSlot);
        }
        थांबा;

    पूर्वनिर्धारित:
        fail("caller capability must be null or reply");
        थांबा;
    }

    परत EXCEPTION_NONE;
}
#endif

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
स्थिर शून्य setUntypedCapAsFull(cap_t srcCap, cap_t newCap, cte_t *srcSlot)
{
    जर ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        जर ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_capBlockSize(newCap)
                == cap_untyped_cap_get_capBlockSize(srcCap))) {
            cap_untyped_cap_ptr_set_capFreeIndex(&(srcSlot->cap),
                                                 MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(srcCap)));
        }
    }
}

शून्य cte_घाला(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_गाठ_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = srcSlot->cteMDBNode;
    srcCap = srcSlot->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_गाठ_set_mdb_Prev(srcMDB, CTE_REF(srcSlot));
    newMDB = mdb_गाठ_set_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_गाठ_set_mdb_First_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    प्रतिज्ञा(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    प्रतिज्ञा((cte_t *)mdb_गाठ_get_mdb_Next(destSlot->cteMDBNode) == रिक्त &&
           (cte_t *)mdb_गाठ_get_mdb_Prev(destSlot->cteMDBNode) == रिक्त);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    setUntypedCapAsFull(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSlot->cteMDBNode = newMDB;
    mdb_गाठ_ptr_set_mdb_Next(&srcSlot->cteMDBNode, CTE_REF(destSlot));
    जर (mdb_गाठ_get_mdb_Next(newMDB)) {
        mdb_गाठ_ptr_set_mdb_Prev(
            &CTE_PTR(mdb_गाठ_get_mdb_Next(newMDB))->cteMDBNode,
            CTE_REF(destSlot));
    }
}

शून्य cteMove(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_गाठ_t mdb;
    word_t prev_ptr, next_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    प्रतिज्ञा(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    प्रतिज्ञा((cte_t *)mdb_गाठ_get_mdb_Next(destSlot->cteMDBNode) == रिक्त &&
           (cte_t *)mdb_गाठ_get_mdb_Prev(destSlot->cteMDBNode) == रिक्त);

    mdb = srcSlot->cteMDBNode;
    destSlot->cap = newCap;
    srcSlot->cap = cap_null_cap_new();
    destSlot->cteMDBNode = mdb;
    srcSlot->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_गाठ_get_mdb_Prev(mdb);
    जर (prev_ptr)
        mdb_गाठ_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(destSlot));

    next_ptr = mdb_गाठ_get_mdb_Next(mdb);
    जर (next_ptr)
        mdb_गाठ_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(destSlot));
}

शून्य capSwapForDelete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    जर (slot1 == slot2) {
        परत;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cteSwap(cap1, slot1, cap2, slot2);
}

शून्य cteSwap(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_गाठ_t mdb1, mdb2;
    word_t next_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_गाठ_get_mdb_Prev(mdb1);
    जर (prev_ptr)
        mdb_गाठ_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    next_ptr = mdb_गाठ_get_mdb_Next(mdb1);
    जर (next_ptr)
        mdb_गाठ_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_गाठ_get_mdb_Prev(mdb2);
    जर (prev_ptr)
        mdb_गाठ_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    next_ptr = mdb_गाठ_get_mdb_Next(mdb2);
    जर (next_ptr)
        mdb_गाठ_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *slot)
{
    cte_t *nextPtr;
    exception_t status;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    साठी (nextPtr = CTE_PTR(mdb_गाठ_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_गाठ_get_mdb_Next(slot->cteMDBNode))) {
        status = cteDelete(nextPtr, true);
        जर (status != EXCEPTION_NONE) {
            परत status;
        }

        status = preemptionPoint();
        जर (status != EXCEPTION_NONE) {
            परत status;
        }
    }

    परत EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *slot, bool_t exposed)
{
    finaliseSlot_ret_t fs_ret;

    fs_ret = finaliseSlot(slot, exposed);
    जर (fs_ret.status != EXCEPTION_NONE) {
        परत fs_ret.status;
    }

    जर (exposed || fs_ret.success) {
        emptySlot(slot, fs_ret.cleanupInfo);
    }
    परत EXCEPTION_NONE;
}

स्थिर शून्य emptySlot(cte_t *slot, cap_t cleanupInfo)
{
    जर (cap_get_capType(slot->cap) != cap_null_cap) {
        mdb_गाठ_t mdb_गाठ;
        cte_t *prev, *next;

        mdb_गाठ = slot->cteMDBNode;
        prev = CTE_PTR(mdb_गाठ_get_mdb_Prev(mdb_गाठ));
        next = CTE_PTR(mdb_गाठ_get_mdb_Next(mdb_गाठ));

        जर (prev) {
            mdb_गाठ_ptr_set_mdb_Next(&prev->cteMDBNode, CTE_REF(next));
        }
        जर (next) {
            mdb_गाठ_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(prev));
        }
        जर (next)
            mdb_गाठ_ptr_set_mdb_First_Badged(&next->cteMDBNode,
                                            mdb_गाठ_get_mdb_First_Badged(next->cteMDBNode) ||
                                            mdb_गाठ_get_mdb_First_Badged(mdb_गाठ));
        slot->cap = cap_null_cap_new();
        slot->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

स्थिर ओळीत bool_t CONST capRemovable(cap_t cap, cte_t *slot)
{
    निवड (cap_get_capType(cap)) {
    स्थिती cap_null_cap:
        परत true;
    स्थिती cap_zombie_cap: {
        word_t n = cap_zombie_cap_get_cap_Zombie_संख्या(cap);
        cte_t *z_slot = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        परत (n == 0 || (n == 1 && slot == z_slot));
    }
    पूर्वनिर्धारित:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

स्थिर ओळीत bool_t CONST capCyclicZombie(cap_t cap, cte_t *slot)
{
    परत cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == slot;
}

स्थिर finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t immediate)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t status;
    finaliseSlot_ret_t ret;

    पर्यंत (cap_get_capType(slot->cap) != cap_null_cap) {
        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, false);

        जर (capRemovable(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            परत ret;
        }

        slot->cap = fc_ret.remainder;

        जर (!immediate && capCyclicZombie(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            परत ret;
        }

        status = reduceZombie(slot, immediate);
        जर (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            परत ret;
        }

        status = preemptionPoint();
        जर (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            परत ret;
        }
    }
    ret.status = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    परत ret;
}

स्थिर exception_t reduceZombie(cte_t *slot, bool_t immediate)
{
    cte_t *ptr;
    word_t n, type;
    exception_t status;

    प्रतिज्ञा(cap_get_capType(slot->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);
    n = cap_zombie_cap_get_cap_Zombie_संख्या(slot->cap);
    type = cap_zombie_cap_get_capZombieType(slot->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    प्रतिज्ञा(n > 0);

    जर (immediate) {
        cte_t *endSlot = &ptr[n - 1];

        status = cteDelete(endSlot, false);
        जर (status != EXCEPTION_NONE) {
            परत status;
        }

        निवड (cap_get_capType(slot->cap)) {
        स्थिती cap_null_cap:
            थांबा;

        स्थिती cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);

            जर (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_संख्या(slot->cap) == n &&
                cap_zombie_cap_get_capZombieType(slot->cap) == type) {
                प्रतिज्ञा(cap_get_capType(endSlot->cap) == cap_null_cap);
                slot->cap =
                    cap_zombie_cap_set_cap_Zombie_संख्या(slot->cap, n - 1);
            } अन्यथा {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                प्रतिज्ञा(ptr2 == slot && ptr != slot);
            }
            थांबा;
        }

        पूर्वनिर्धारित:
            fail("Expected recursion to result in Zombie.");
        }
    } अन्यथा {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        प्रतिज्ञा(ptr != slot);

        जर (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            प्रतिज्ञा(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        capSwapForDelete(ptr, slot);
    }
    परत EXCEPTION_NONE;
}

शून्य cteDeleteOne(cte_t *slot)
{
    word_t cap_type = cap_get_capType(slot->cap);
    जर (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret UNUSED;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        प्रतिज्ञा(capRemovable(fc_ret.remainder, slot) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        emptySlot(slot, cap_null_cap_new());
    }
}

शून्य घाला_New_Cap(cte_t *parent, cte_t *slot, cap_t cap)
{
    cte_t *next;

    next = CTE_PTR(mdb_गाठ_get_mdb_Next(parent->cteMDBNode));
    slot->cap = cap;
    slot->cteMDBNode = mdb_गाठ_new(CTE_REF(next), true, true, CTE_REF(parent));
    जर (next) {
        mdb_गाठ_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(slot));
    }
    mdb_गाठ_ptr_set_mdb_Next(&parent->cteMDBNode, CTE_REF(slot));
}

#ifndef CONFIG_KERNEL_MCS
शून्य setupReplyMaster(tcb_t *thread)
{
    cte_t *slot;

    slot = TCB_PTR_CTE_PTR(thread, tcbReply);
    जर (cap_get_capType(slot->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        slot->cap = cap_reply_cap_new(true, true, TCB_REF(thread));
        slot->cteMDBNode = nullMDBNode;
        mdb_गाठ_ptr_set_mdb_Revocable(&slot->cteMDBNode, true);
        mdb_गाठ_ptr_set_mdb_First_Badged(&slot->cteMDBNode, true);
    }
}
#endif

bool_t PURE isMDBParentOf(cte_t *cte_a, cte_t *cte_b)
{
    जर (!mdb_गाठ_get_mdb_Revocable(cte_a->cteMDBNode)) {
        परत false;
    }
    जर (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        परत false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    जर (!Arch_isMDBParentOf(cte_a->cap, cte_b->cap,
                            mdb_गाठ_get_mdb_First_Badged(cte_b->cteMDBNode))) {
        परत false;
    }
    निवड (cap_get_capType(cte_a->cap)) {
    स्थिती cap_endpoint_cap: {
        word_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        जर (badge == 0) {
            परत true;
        }
        परत (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_गाठ_get_mdb_First_Badged(cte_b->cteMDBNode);
        थांबा;
    }

    स्थिती cap_notification_cap: {
        word_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        जर (badge == 0) {
            परत true;
        }
        परत (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_गाठ_get_mdb_First_Badged(cte_b->cteMDBNode);
        थांबा;
    }

    पूर्वनिर्धारित:
        परत true;
        थांबा;
    }
}

exception_t ensureNoChildren(cte_t *slot)
{
    जर (mdb_गाठ_get_mdb_Next(slot->cteMDBNode) != 0) {
        cte_t *next;

        next = CTE_PTR(mdb_गाठ_get_mdb_Next(slot->cteMDBNode));
        जर (isMDBParentOf(slot, next)) {
            current_syscall_error.type = seL4_RevokeFirst;
            परत EXCEPTION_SYSCALL_ERROR;
        }
    }

    परत EXCEPTION_NONE;
}

exception_t ensureEmptySlot(cte_t *slot)
{
    जर (cap_get_capType(slot->cap) != cap_null_cap) {
        current_syscall_error.type = seL4_DeleteFirst;
        परत EXCEPTION_SYSCALL_ERROR;
    }

    परत EXCEPTION_NONE;
}

bool_t PURE isFinalCapability(cte_t *cte)
{
    mdb_गाठ_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    जर (mdb_गाठ_get_mdb_Prev(mdb) == 0) {
        prevIsSameObject = false;
    } अन्यथा {
        cte_t *prev;

        prev = CTE_PTR(mdb_गाठ_get_mdb_Prev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    जर (prevIsSameObject) {
        परत false;
    } अन्यथा {
        जर (mdb_गाठ_get_mdb_Next(mdb) == 0) {
            परत true;
        } अन्यथा {
            cte_t *next;

            next = CTE_PTR(mdb_गाठ_get_mdb_Next(mdb));
            परत !sameObjectAs(cte->cap, next->cap);
        }
    }
}

bool_t PURE slotCapLongRunningDelete(cte_t *slot)
{
    जर (cap_get_capType(slot->cap) == cap_null_cap) {
        परत false;
    } अन्यथा जर (! isFinalCapability(slot)) {
        परत false;
    }
    निवड (cap_get_capType(slot->cap)) {
    स्थिती cap_thread_cap:
    स्थिती cap_zombie_cap:
    स्थिती cap_cnode_cap:
        परत true;
    पूर्वनिर्धारित:
        परत false;
    }
}

/* This implementation is specialised to the (current) limit
 * of one cap receive slot. */
cte_t *getReceiveSlots(tcb_t *thread, word_t *buffer)
{
    cap_transfer_t ct;
    cptr_t cptr;
    lookupCap_ret_t luc_ret;
    lookupSlot_ret_t lus_ret;
    cte_t *slot;
    cap_t cnode;

    जर (!buffer) {
        परत रिक्त;
    }

    ct = loadCapTransfer(buffer);
    cptr = ct.ct_Receive_मूळ;

    luc_ret = lookupCap(thread, cptr);
    जर (luc_ret.status != EXCEPTION_NONE) {
        परत रिक्त;
    }
    cnode = luc_ret.cap;

    lus_ret = lookupTargetSlot(cnode, ct.ctReceiveIndex, ct.ctReceiveDepth);
    जर (lus_ret.status != EXCEPTION_NONE) {
        परत रिक्त;
    }
    slot = lus_ret.slot;

    जर (cap_get_capType(slot->cap) != cap_null_cap) {
        परत रिक्त;
    }

    परत slot;
}

cap_transfer_t PURE loadCapTransfer(word_t *buffer)
{
    स्थिरांक पूर्णांक offset = se_L4_संदेश_Max_Length + se_L4_संदेश_Max_Extra_Caps + 2;
    परत cap_Transfer_From_शब्द(buffer + offset);
}
