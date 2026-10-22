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
प्रकारनिर्देश संरचना finaliseSlot_ret finaliseSlot_ret_t;

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

    यदि (invLabel < CNodeRevoke || invLabel > CNODE_LAST_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (length < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    w_bits = getSyscallArg(1, buffer);

    lu_ret = lookupTargetSlot(cap, index, w_bits);
    यदि (lu_ret.status != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        फर्काऊ lu_ret.status;
    }
    destSlot = lu_ret.slot;

    यदि (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *srcSlot;
        word_t srcIndex, srcDepth, capData;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_जरा, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        यदि (length < 4 || current_extra_caps.excaprefs[0] == रिक्त) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }
        srcIndex = getSyscallArg(2, buffer);
        srcDepth = getSyscallArg(3, buffer);

        src_जरा = current_extra_caps.excaprefs[0]->cap;

        status = ensureEmptySlot(destSlot);
        यदि (status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            फर्काऊ status;
        }

        lu_ret = lookupSourceSlot(src_जरा, srcIndex, srcDepth);
        यदि (lu_ret.status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            फर्काऊ lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        यदि (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(srcDepth);
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        विकल्प (invLabel) {
        पक्ष CNodeCopy:

            यदि (length < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = seL4_TruncatedMessage;
                फर्काऊ EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot, srcCap);
            यदि (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                फर्काऊ dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            रोक;

        पक्ष CNodeMint:
            यदि (length < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                फर्काऊ EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            capData = getSyscallArg(5, buffer);
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot,
                               updateCapData(false, capData, srcCap));
            यदि (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                फर्काऊ dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            रोक;

        पक्ष CNodeMove:
            newCap = srcSlot->cap;
            isMove = true;

            रोक;

        पक्ष CNodeMutate:
            यदि (length < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                फर्काऊ EXCEPTION_SYSCALL_ERROR;
            }

            capData = getSyscallArg(4, buffer);
            newCap = updateCapData(true, capData, srcSlot->cap);
            isMove = true;

            रोक;

        स्वभाव:
            प्रतिज्ञा(0);
            फर्काऊ EXCEPTION_NONE;
        }

        यदि (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        यदि (isMove) {
            फर्काऊ invokeCNodeMove(newCap, srcSlot, destSlot);
        } अन्यथा {
            फर्काऊ invoke_CNode_घुसाउ(newCap, srcSlot, destSlot);
        }
    }

    यदि (invLabel == CNodeRevoke) {
        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        फर्काऊ invokeCNodeRevoke(destSlot);
    }

    यदि (invLabel == CNodeDelete) {
        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        फर्काऊ invokeCNodeDelete(destSlot);
    }

#ifndef CONFIG_KERNEL_MCS
    यदि (invLabel == CNodeSaveCaller) {
        status = ensureEmptySlot(destSlot);
        यदि (status != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            फर्काऊ status;
        }

        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        फर्काऊ invokeCNodeSaveCaller(destSlot);
    }
#endif

    यदि (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = destSlot->cap;

        यदि (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        फर्काऊ invokeCNodeCancelBadgedSends(destCap);
    }

    यदि (invLabel == CNodeRotate) {
        word_t pivotNewData, pivotIndex, pivotDepth;
        word_t srcNewData, srcIndex, srcDepth;
        cte_t *pivotSlot, *srcSlot;
        cap_t pivot_जरा, src_जरा, newSrcCap, newPivotCap;

        यदि (length < 8 || current_extra_caps.excaprefs[0] == रिक्त
            || current_extra_caps.excaprefs[1] == रिक्त) {
            current_syscall_error.type = seL4_TruncatedMessage;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }
        pivotNewData = getSyscallArg(2, buffer);
        pivotIndex   = getSyscallArg(3, buffer);
        pivotDepth   = getSyscallArg(4, buffer);
        srcNewData   = getSyscallArg(5, buffer);
        srcIndex     = getSyscallArg(6, buffer);
        srcDepth     = getSyscallArg(7, buffer);

        pivot_जरा = current_extra_caps.excaprefs[0]->cap;
        src_जरा   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookupSourceSlot(src_जरा, srcIndex, srcDepth);
        यदि (lu_ret.status != EXCEPTION_NONE) {
            फर्काऊ lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        lu_ret = lookupPivotSlot(pivot_जरा, pivotIndex, pivotDepth);
        यदि (lu_ret.status != EXCEPTION_NONE) {
            फर्काऊ lu_ret.status;
        }
        pivotSlot = lu_ret.slot;

        यदि (pivotSlot == srcSlot || pivotSlot == destSlot) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        यदि (srcSlot != destSlot) {
            status = ensureEmptySlot(destSlot);
            यदि (status != EXCEPTION_NONE) {
                फर्काऊ status;
            }
        }

        यदि (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(srcDepth);
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        यदि (cap_get_capType(pivotSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(pivotDepth);
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = updateCapData(true, srcNewData, srcSlot->cap);
        newPivotCap = updateCapData(true, pivotNewData, pivotSlot->cap);

        यदि (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        यदि (cap_get_capType(newPivotCap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        फर्काऊ invokeCNodeRotate(newSrcCap, newPivotCap,
                                 srcSlot, pivotSlot, destSlot);
    }

    फर्काऊ EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *destSlot)
{
    फर्काऊ cteRevoke(destSlot);
}

exception_t invokeCNodeDelete(cte_t *destSlot)
{
    फर्काऊ cteDelete(destSlot, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    word_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    यदि (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    फर्काऊ EXCEPTION_NONE;
}

exception_t invoke_CNode_घुसाउ(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cte_घुसाउ(cap, srcSlot, destSlot);

    फर्काऊ EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cteMove(cap, srcSlot, destSlot);

    फर्काऊ EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    यदि (slot1 == slot3) {
        cteSwap(cap1, slot1, cap2, slot2);
    } अन्यथा {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    फर्काऊ EXCEPTION_NONE;
}

#ifndef CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *destSlot)
{
    cap_t cap;
    cte_t *srcSlot;

    srcSlot = TCB_PTR_CTE_PTR(गाँठो_STATE(ksCurThread), tcbCaller);
    cap = srcSlot->cap;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        रोक;

    पक्ष cap_reply_cap:
        यदि (!cap_reply_cap_get_capReplyMaster(cap)) {
            cteMove(cap, srcSlot, destSlot);
        }
        रोक;

    स्वभाव:
        fail("caller capability must be null or reply");
        रोक;
    }

    फर्काऊ EXCEPTION_NONE;
}
#endif

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
स्थिर शून्य setUntypedCapAsFull(cap_t srcCap, cap_t newCap, cte_t *srcSlot)
{
    यदि ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        यदि ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_capBlockSize(newCap)
                == cap_untyped_cap_get_capBlockSize(srcCap))) {
            cap_untyped_cap_ptr_set_capFreeIndex(&(srcSlot->cap),
                                                 MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(srcCap)));
        }
    }
}

शून्य cte_घुसाउ(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_गाँठो_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = srcSlot->cteMDBNode;
    srcCap = srcSlot->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_गाँठो_set_mdb_Prev(srcMDB, CTE_REF(srcSlot));
    newMDB = mdb_गाँठो_set_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_गाँठो_set_mdb_First_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    प्रतिज्ञा(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    प्रतिज्ञा((cte_t *)mdb_गाँठो_get_mdb_Next(destSlot->cteMDBNode) == रिक्त &&
           (cte_t *)mdb_गाँठो_get_mdb_Prev(destSlot->cteMDBNode) == रिक्त);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    setUntypedCapAsFull(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSlot->cteMDBNode = newMDB;
    mdb_गाँठो_ptr_set_mdb_Next(&srcSlot->cteMDBNode, CTE_REF(destSlot));
    यदि (mdb_गाँठो_get_mdb_Next(newMDB)) {
        mdb_गाँठो_ptr_set_mdb_Prev(
            &CTE_PTR(mdb_गाँठो_get_mdb_Next(newMDB))->cteMDBNode,
            CTE_REF(destSlot));
    }
}

शून्य cteMove(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_गाँठो_t mdb;
    word_t prev_ptr, next_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    प्रतिज्ञा(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    प्रतिज्ञा((cte_t *)mdb_गाँठो_get_mdb_Next(destSlot->cteMDBNode) == रिक्त &&
           (cte_t *)mdb_गाँठो_get_mdb_Prev(destSlot->cteMDBNode) == रिक्त);

    mdb = srcSlot->cteMDBNode;
    destSlot->cap = newCap;
    srcSlot->cap = cap_null_cap_new();
    destSlot->cteMDBNode = mdb;
    srcSlot->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_गाँठो_get_mdb_Prev(mdb);
    यदि (prev_ptr)
        mdb_गाँठो_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(destSlot));

    next_ptr = mdb_गाँठो_get_mdb_Next(mdb);
    यदि (next_ptr)
        mdb_गाँठो_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(destSlot));
}

शून्य capSwapForDelete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    यदि (slot1 == slot2) {
        फर्काऊ;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cteSwap(cap1, slot1, cap2, slot2);
}

शून्य cteSwap(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_गाँठो_t mdb1, mdb2;
    word_t next_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_गाँठो_get_mdb_Prev(mdb1);
    यदि (prev_ptr)
        mdb_गाँठो_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    next_ptr = mdb_गाँठो_get_mdb_Next(mdb1);
    यदि (next_ptr)
        mdb_गाँठो_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_गाँठो_get_mdb_Prev(mdb2);
    यदि (prev_ptr)
        mdb_गाँठो_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    next_ptr = mdb_गाँठो_get_mdb_Next(mdb2);
    यदि (next_ptr)
        mdb_गाँठो_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *slot)
{
    cte_t *nextPtr;
    exception_t status;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    लागि (nextPtr = CTE_PTR(mdb_गाँठो_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_गाँठो_get_mdb_Next(slot->cteMDBNode))) {
        status = cteDelete(nextPtr, true);
        यदि (status != EXCEPTION_NONE) {
            फर्काऊ status;
        }

        status = preemptionPoint();
        यदि (status != EXCEPTION_NONE) {
            फर्काऊ status;
        }
    }

    फर्काऊ EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *slot, bool_t exposed)
{
    finaliseSlot_ret_t fs_ret;

    fs_ret = finaliseSlot(slot, exposed);
    यदि (fs_ret.status != EXCEPTION_NONE) {
        फर्काऊ fs_ret.status;
    }

    यदि (exposed || fs_ret.success) {
        emptySlot(slot, fs_ret.cleanupInfo);
    }
    फर्काऊ EXCEPTION_NONE;
}

स्थिर शून्य emptySlot(cte_t *slot, cap_t cleanupInfo)
{
    यदि (cap_get_capType(slot->cap) != cap_null_cap) {
        mdb_गाँठो_t mdb_गाँठो;
        cte_t *prev, *next;

        mdb_गाँठो = slot->cteMDBNode;
        prev = CTE_PTR(mdb_गाँठो_get_mdb_Prev(mdb_गाँठो));
        next = CTE_PTR(mdb_गाँठो_get_mdb_Next(mdb_गाँठो));

        यदि (prev) {
            mdb_गाँठो_ptr_set_mdb_Next(&prev->cteMDBNode, CTE_REF(next));
        }
        यदि (next) {
            mdb_गाँठो_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(prev));
        }
        यदि (next)
            mdb_गाँठो_ptr_set_mdb_First_Badged(&next->cteMDBNode,
                                            mdb_गाँठो_get_mdb_First_Badged(next->cteMDBNode) ||
                                            mdb_गाँठो_get_mdb_First_Badged(mdb_गाँठो));
        slot->cap = cap_null_cap_new();
        slot->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

स्थिर अन्तःस्थ bool_t CONST capRemovable(cap_t cap, cte_t *slot)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_null_cap:
        फर्काऊ true;
    पक्ष cap_zombie_cap: {
        word_t n = cap_zombie_cap_get_cap_Zombie_सङ्ख्या(cap);
        cte_t *z_slot = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        फर्काऊ (n == 0 || (n == 1 && slot == z_slot));
    }
    स्वभाव:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

स्थिर अन्तःस्थ bool_t CONST capCyclicZombie(cap_t cap, cte_t *slot)
{
    फर्काऊ cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == slot;
}

स्थिर finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t immediate)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t status;
    finaliseSlot_ret_t ret;

    जबसम्म (cap_get_capType(slot->cap) != cap_null_cap) {
        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, false);

        यदि (capRemovable(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            फर्काऊ ret;
        }

        slot->cap = fc_ret.remainder;

        यदि (!immediate && capCyclicZombie(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            फर्काऊ ret;
        }

        status = reduceZombie(slot, immediate);
        यदि (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            फर्काऊ ret;
        }

        status = preemptionPoint();
        यदि (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            फर्काऊ ret;
        }
    }
    ret.status = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    फर्काऊ ret;
}

स्थिर exception_t reduceZombie(cte_t *slot, bool_t immediate)
{
    cte_t *ptr;
    word_t n, type;
    exception_t status;

    प्रतिज्ञा(cap_get_capType(slot->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);
    n = cap_zombie_cap_get_cap_Zombie_सङ्ख्या(slot->cap);
    type = cap_zombie_cap_get_capZombieType(slot->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    प्रतिज्ञा(n > 0);

    यदि (immediate) {
        cte_t *endSlot = &ptr[n - 1];

        status = cteDelete(endSlot, false);
        यदि (status != EXCEPTION_NONE) {
            फर्काऊ status;
        }

        विकल्प (cap_get_capType(slot->cap)) {
        पक्ष cap_null_cap:
            रोक;

        पक्ष cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);

            यदि (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_सङ्ख्या(slot->cap) == n &&
                cap_zombie_cap_get_capZombieType(slot->cap) == type) {
                प्रतिज्ञा(cap_get_capType(endSlot->cap) == cap_null_cap);
                slot->cap =
                    cap_zombie_cap_set_cap_Zombie_सङ्ख्या(slot->cap, n - 1);
            } अन्यथा {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                प्रतिज्ञा(ptr2 == slot && ptr != slot);
            }
            रोक;
        }

        स्वभाव:
            fail("Expected recursion to result in Zombie.");
        }
    } अन्यथा {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        प्रतिज्ञा(ptr != slot);

        यदि (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            प्रतिज्ञा(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        capSwapForDelete(ptr, slot);
    }
    फर्काऊ EXCEPTION_NONE;
}

शून्य cteDeleteOne(cte_t *slot)
{
    word_t cap_type = cap_get_capType(slot->cap);
    यदि (cap_type != cap_null_cap) {
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

शून्य घुसाउ_New_Cap(cte_t *parent, cte_t *slot, cap_t cap)
{
    cte_t *next;

    next = CTE_PTR(mdb_गाँठो_get_mdb_Next(parent->cteMDBNode));
    slot->cap = cap;
    slot->cteMDBNode = mdb_गाँठो_new(CTE_REF(next), true, true, CTE_REF(parent));
    यदि (next) {
        mdb_गाँठो_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(slot));
    }
    mdb_गाँठो_ptr_set_mdb_Next(&parent->cteMDBNode, CTE_REF(slot));
}

#ifndef CONFIG_KERNEL_MCS
शून्य setupReplyMaster(tcb_t *thread)
{
    cte_t *slot;

    slot = TCB_PTR_CTE_PTR(thread, tcbReply);
    यदि (cap_get_capType(slot->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        slot->cap = cap_reply_cap_new(true, true, TCB_REF(thread));
        slot->cteMDBNode = nullMDBNode;
        mdb_गाँठो_ptr_set_mdb_Revocable(&slot->cteMDBNode, true);
        mdb_गाँठो_ptr_set_mdb_First_Badged(&slot->cteMDBNode, true);
    }
}
#endif

bool_t PURE isMDBParentOf(cte_t *cte_a, cte_t *cte_b)
{
    यदि (!mdb_गाँठो_get_mdb_Revocable(cte_a->cteMDBNode)) {
        फर्काऊ false;
    }
    यदि (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        फर्काऊ false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    यदि (!Arch_isMDBParentOf(cte_a->cap, cte_b->cap,
                            mdb_गाँठो_get_mdb_First_Badged(cte_b->cteMDBNode))) {
        फर्काऊ false;
    }
    विकल्प (cap_get_capType(cte_a->cap)) {
    पक्ष cap_endpoint_cap: {
        word_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        यदि (badge == 0) {
            फर्काऊ true;
        }
        फर्काऊ (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_गाँठो_get_mdb_First_Badged(cte_b->cteMDBNode);
        रोक;
    }

    पक्ष cap_notification_cap: {
        word_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        यदि (badge == 0) {
            फर्काऊ true;
        }
        फर्काऊ (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_गाँठो_get_mdb_First_Badged(cte_b->cteMDBNode);
        रोक;
    }

    स्वभाव:
        फर्काऊ true;
        रोक;
    }
}

exception_t ensureNoChildren(cte_t *slot)
{
    यदि (mdb_गाँठो_get_mdb_Next(slot->cteMDBNode) != 0) {
        cte_t *next;

        next = CTE_PTR(mdb_गाँठो_get_mdb_Next(slot->cteMDBNode));
        यदि (isMDBParentOf(slot, next)) {
            current_syscall_error.type = seL4_RevokeFirst;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }
    }

    फर्काऊ EXCEPTION_NONE;
}

exception_t ensureEmptySlot(cte_t *slot)
{
    यदि (cap_get_capType(slot->cap) != cap_null_cap) {
        current_syscall_error.type = seL4_DeleteFirst;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    फर्काऊ EXCEPTION_NONE;
}

bool_t PURE isFinalCapability(cte_t *cte)
{
    mdb_गाँठो_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    यदि (mdb_गाँठो_get_mdb_Prev(mdb) == 0) {
        prevIsSameObject = false;
    } अन्यथा {
        cte_t *prev;

        prev = CTE_PTR(mdb_गाँठो_get_mdb_Prev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    यदि (prevIsSameObject) {
        फर्काऊ false;
    } अन्यथा {
        यदि (mdb_गाँठो_get_mdb_Next(mdb) == 0) {
            फर्काऊ true;
        } अन्यथा {
            cte_t *next;

            next = CTE_PTR(mdb_गाँठो_get_mdb_Next(mdb));
            फर्काऊ !sameObjectAs(cte->cap, next->cap);
        }
    }
}

bool_t PURE slotCapLongRunningDelete(cte_t *slot)
{
    यदि (cap_get_capType(slot->cap) == cap_null_cap) {
        फर्काऊ false;
    } अन्यथा यदि (! isFinalCapability(slot)) {
        फर्काऊ false;
    }
    विकल्प (cap_get_capType(slot->cap)) {
    पक्ष cap_thread_cap:
    पक्ष cap_zombie_cap:
    पक्ष cap_cnode_cap:
        फर्काऊ true;
    स्वभाव:
        फर्काऊ false;
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

    यदि (!buffer) {
        फर्काऊ रिक्त;
    }

    ct = loadCapTransfer(buffer);
    cptr = ct.ct_Receive_जरा;

    luc_ret = lookupCap(thread, cptr);
    यदि (luc_ret.status != EXCEPTION_NONE) {
        फर्काऊ रिक्त;
    }
    cnode = luc_ret.cap;

    lus_ret = lookupTargetSlot(cnode, ct.ctReceiveIndex, ct.ctReceiveDepth);
    यदि (lus_ret.status != EXCEPTION_NONE) {
        फर्काऊ रिक्त;
    }
    slot = lus_ret.slot;

    यदि (cap_get_capType(slot->cap) != cap_null_cap) {
        फर्काऊ रिक्त;
    }

    फर्काऊ slot;
}

cap_transfer_t PURE loadCapTransfer(word_t *buffer)
{
    अचर पूर्णाङ्क offset = se_L4_सन्देश_Max_Length + se_L4_सन्देश_Max_Extra_Caps + 2;
    फर्काऊ cap_Transfer_From_शब्दहरू(buffer + offset);
}
