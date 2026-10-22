/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <api/invocation.h>
#ضمن <api/syscall.h>
#ضمن <api/types.h>
#ضمن <machine/io.h>
#ضمن <object/structures.h>
#ضمن <object/objecttype.h>
#ضمن <object/cnode.h>
#ضمن <object/interrupt.h>
#ضمن <object/untyped.h>
#ضمن <kernel/cspace.h>
#ضمن <kernel/thread.h>
#ضمن <model/preemption.h>
#ضمن <model/statedata.h>
#ضمن <util.h>

بنية finaliseSlot_ret {
    exception_t status;
    bool_t success;
    cap_t cleanupInfo;
};
تعريفنوع بنية finaliseSlot_ret finaliseSlot_ret_t;

ساكن finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t exposed);
ساكن فراغ emptySlot(cte_t *slot, cap_t cleanupInfo);
ساكن exception_t reduceZombie(cte_t *slot, bool_t exposed);

#ifdef CONFIG_KERNEL_MCS
#define CNODE_LAST_INVOCATION CNodeRotate
#وإلا
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
    أكد(cap_get_capType(cap) == cap_cnode_cap);

    إذا (invLabel < CNodeRevoke || invLabel > CNODE_LAST_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        إرجاع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (length < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        إرجاع EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    w_bits = getSyscallArg(1, buffer);

    lu_ret = lookupTargetSlot(cap, index, w_bits);
    إذا (lu_ret.status != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        إرجاع lu_ret.status;
    }
    destSlot = lu_ret.slot;

    إذا (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *srcSlot;
        word_t srcIndex, srcDepth, capData;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_جذر, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        إذا (length < 4 || current_extra_caps.excaprefs[0] == عدم) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            إرجاع EXCEPTION_SYSCALL_ERROR;
        }
        srcIndex = getSyscallArg(2, buffer);
        srcDepth = getSyscallArg(3, buffer);

        src_جذر = current_extra_caps.excaprefs[0]->cap;

        status = ensureEmptySlot(destSlot);
        إذا (status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            إرجاع status;
        }

        lu_ret = lookupSourceSlot(src_جذر, srcIndex, srcDepth);
        إذا (lu_ret.status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            إرجاع lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        إذا (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(srcDepth);
            إرجاع EXCEPTION_SYSCALL_ERROR;
        }

        بدل (invLabel) {
        حالة CNodeCopy:

            إذا (length < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = seL4_TruncatedMessage;
                إرجاع EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot, srcCap);
            إذا (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                إرجاع dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            اكسر;

        حالة CNodeMint:
            إذا (length < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                إرجاع EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            capData = getSyscallArg(5, buffer);
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot,
                               updateCapData(false, capData, srcCap));
            إذا (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                إرجاع dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            اكسر;

        حالة CNodeMove:
            newCap = srcSlot->cap;
            isMove = true;

            اكسر;

        حالة CNodeMutate:
            إذا (length < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                إرجاع EXCEPTION_SYSCALL_ERROR;
            }

            capData = getSyscallArg(4, buffer);
            newCap = updateCapData(true, capData, srcSlot->cap);
            isMove = true;

            اكسر;

        افتراضي:
            أكد(0);
            إرجاع EXCEPTION_NONE;
        }

        إذا (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            إرجاع EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
        إذا (isMove) {
            إرجاع invokeCNodeMove(newCap, srcSlot, destSlot);
        } وإلا {
            إرجاع invoke_CNode_أدرج(newCap, srcSlot, destSlot);
        }
    }

    إذا (invLabel == CNodeRevoke) {
        setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
        إرجاع invokeCNodeRevoke(destSlot);
    }

    إذا (invLabel == CNodeDelete) {
        setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
        إرجاع invokeCNodeDelete(destSlot);
    }

#ifndef CONFIG_KERNEL_MCS
    إذا (invLabel == CNodeSaveCaller) {
        status = ensureEmptySlot(destSlot);
        إذا (status != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            إرجاع status;
        }

        setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
        إرجاع invokeCNodeSaveCaller(destSlot);
    }
#endif

    إذا (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = destSlot->cap;

        إذا (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            إرجاع EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
        إرجاع invokeCNodeCancelBadgedSends(destCap);
    }

    إذا (invLabel == CNodeRotate) {
        word_t pivotNewData, pivotIndex, pivotDepth;
        word_t srcNewData, srcIndex, srcDepth;
        cte_t *pivotSlot, *srcSlot;
        cap_t pivot_جذر, src_جذر, newSrcCap, newPivotCap;

        إذا (length < 8 || current_extra_caps.excaprefs[0] == عدم
            || current_extra_caps.excaprefs[1] == عدم) {
            current_syscall_error.type = seL4_TruncatedMessage;
            إرجاع EXCEPTION_SYSCALL_ERROR;
        }
        pivotNewData = getSyscallArg(2, buffer);
        pivotIndex   = getSyscallArg(3, buffer);
        pivotDepth   = getSyscallArg(4, buffer);
        srcNewData   = getSyscallArg(5, buffer);
        srcIndex     = getSyscallArg(6, buffer);
        srcDepth     = getSyscallArg(7, buffer);

        pivot_جذر = current_extra_caps.excaprefs[0]->cap;
        src_جذر   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookupSourceSlot(src_جذر, srcIndex, srcDepth);
        إذا (lu_ret.status != EXCEPTION_NONE) {
            إرجاع lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        lu_ret = lookupPivotSlot(pivot_جذر, pivotIndex, pivotDepth);
        إذا (lu_ret.status != EXCEPTION_NONE) {
            إرجاع lu_ret.status;
        }
        pivotSlot = lu_ret.slot;

        إذا (pivotSlot == srcSlot || pivotSlot == destSlot) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            إرجاع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (srcSlot != destSlot) {
            status = ensureEmptySlot(destSlot);
            إذا (status != EXCEPTION_NONE) {
                إرجاع status;
            }
        }

        إذا (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(srcDepth);
            إرجاع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (cap_get_capType(pivotSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(pivotDepth);
            إرجاع EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = updateCapData(true, srcNewData, srcSlot->cap);
        newPivotCap = updateCapData(true, pivotNewData, pivotSlot->cap);

        إذا (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            إرجاع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (cap_get_capType(newPivotCap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            إرجاع EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
        إرجاع invokeCNodeRotate(newSrcCap, newPivotCap,
                                 srcSlot, pivotSlot, destSlot);
    }

    إرجاع EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *destSlot)
{
    إرجاع cteRevoke(destSlot);
}

exception_t invokeCNodeDelete(cte_t *destSlot)
{
    إرجاع cteDelete(destSlot, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    word_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    إذا (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    إرجاع EXCEPTION_NONE;
}

exception_t invoke_CNode_أدرج(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cte_أدرج(cap, srcSlot, destSlot);

    إرجاع EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cteMove(cap, srcSlot, destSlot);

    إرجاع EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    إذا (slot1 == slot3) {
        cteSwap(cap1, slot1, cap2, slot2);
    } وإلا {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    إرجاع EXCEPTION_NONE;
}

#ifndef CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *destSlot)
{
    cap_t cap;
    cte_t *srcSlot;

    srcSlot = TCB_PTR_CTE_PTR(عقدة_STATE(ksCurThread), tcbCaller);
    cap = srcSlot->cap;

    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        اكسر;

    حالة cap_reply_cap:
        إذا (!cap_reply_cap_get_capReplyMaster(cap)) {
            cteMove(cap, srcSlot, destSlot);
        }
        اكسر;

    افتراضي:
        fail("caller capability must be null or reply");
        اكسر;
    }

    إرجاع EXCEPTION_NONE;
}
#endif

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
ساكن فراغ setUntypedCapAsFull(cap_t srcCap, cap_t newCap, cte_t *srcSlot)
{
    إذا ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        إذا ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_capBlockSize(newCap)
                == cap_untyped_cap_get_capBlockSize(srcCap))) {
            cap_untyped_cap_ptr_set_capFreeIndex(&(srcSlot->cap),
                                                 MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(srcCap)));
        }
    }
}

فراغ cte_أدرج(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_عقدة_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = srcSlot->cteMDBNode;
    srcCap = srcSlot->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_عقدة_set_mdb_Prev(srcMDB, CTE_REF(srcSlot));
    newMDB = mdb_عقدة_set_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_عقدة_set_mdb_First_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    أكد(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    أكد((cte_t *)mdb_عقدة_get_mdb_Next(destSlot->cteMDBNode) == عدم &&
           (cte_t *)mdb_عقدة_get_mdb_Prev(destSlot->cteMDBNode) == عدم);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    setUntypedCapAsFull(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSlot->cteMDBNode = newMDB;
    mdb_عقدة_ptr_set_mdb_Next(&srcSlot->cteMDBNode, CTE_REF(destSlot));
    إذا (mdb_عقدة_get_mdb_Next(newMDB)) {
        mdb_عقدة_ptr_set_mdb_Prev(
            &CTE_PTR(mdb_عقدة_get_mdb_Next(newMDB))->cteMDBNode,
            CTE_REF(destSlot));
    }
}

فراغ cteMove(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_عقدة_t mdb;
    word_t prev_ptr, next_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    أكد(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    أكد((cte_t *)mdb_عقدة_get_mdb_Next(destSlot->cteMDBNode) == عدم &&
           (cte_t *)mdb_عقدة_get_mdb_Prev(destSlot->cteMDBNode) == عدم);

    mdb = srcSlot->cteMDBNode;
    destSlot->cap = newCap;
    srcSlot->cap = cap_null_cap_new();
    destSlot->cteMDBNode = mdb;
    srcSlot->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_عقدة_get_mdb_Prev(mdb);
    إذا (prev_ptr)
        mdb_عقدة_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(destSlot));

    next_ptr = mdb_عقدة_get_mdb_Next(mdb);
    إذا (next_ptr)
        mdb_عقدة_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(destSlot));
}

فراغ capSwapForDelete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    إذا (slot1 == slot2) {
        إرجاع;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cteSwap(cap1, slot1, cap2, slot2);
}

فراغ cteSwap(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_عقدة_t mdb1, mdb2;
    word_t next_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_عقدة_get_mdb_Prev(mdb1);
    إذا (prev_ptr)
        mdb_عقدة_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    next_ptr = mdb_عقدة_get_mdb_Next(mdb1);
    إذا (next_ptr)
        mdb_عقدة_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_عقدة_get_mdb_Prev(mdb2);
    إذا (prev_ptr)
        mdb_عقدة_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    next_ptr = mdb_عقدة_get_mdb_Next(mdb2);
    إذا (next_ptr)
        mdb_عقدة_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *slot)
{
    cte_t *nextPtr;
    exception_t status;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    لكل (nextPtr = CTE_PTR(mdb_عقدة_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_عقدة_get_mdb_Next(slot->cteMDBNode))) {
        status = cteDelete(nextPtr, true);
        إذا (status != EXCEPTION_NONE) {
            إرجاع status;
        }

        status = preemptionPoint();
        إذا (status != EXCEPTION_NONE) {
            إرجاع status;
        }
    }

    إرجاع EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *slot, bool_t exposed)
{
    finaliseSlot_ret_t fs_ret;

    fs_ret = finaliseSlot(slot, exposed);
    إذا (fs_ret.status != EXCEPTION_NONE) {
        إرجاع fs_ret.status;
    }

    إذا (exposed || fs_ret.success) {
        emptySlot(slot, fs_ret.cleanupInfo);
    }
    إرجاع EXCEPTION_NONE;
}

ساكن فراغ emptySlot(cte_t *slot, cap_t cleanupInfo)
{
    إذا (cap_get_capType(slot->cap) != cap_null_cap) {
        mdb_عقدة_t mdb_عقدة;
        cte_t *prev, *next;

        mdb_عقدة = slot->cteMDBNode;
        prev = CTE_PTR(mdb_عقدة_get_mdb_Prev(mdb_عقدة));
        next = CTE_PTR(mdb_عقدة_get_mdb_Next(mdb_عقدة));

        إذا (prev) {
            mdb_عقدة_ptr_set_mdb_Next(&prev->cteMDBNode, CTE_REF(next));
        }
        إذا (next) {
            mdb_عقدة_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(prev));
        }
        إذا (next)
            mdb_عقدة_ptr_set_mdb_First_Badged(&next->cteMDBNode,
                                            mdb_عقدة_get_mdb_First_Badged(next->cteMDBNode) ||
                                            mdb_عقدة_get_mdb_First_Badged(mdb_عقدة));
        slot->cap = cap_null_cap_new();
        slot->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

ساكن مضمن bool_t CONST capRemovable(cap_t cap, cte_t *slot)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
        إرجاع true;
    حالة cap_zombie_cap: {
        word_t n = cap_zombie_cap_get_cap_Zombie_العدد(cap);
        cte_t *z_slot = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        إرجاع (n == 0 || (n == 1 && slot == z_slot));
    }
    افتراضي:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

ساكن مضمن bool_t CONST capCyclicZombie(cap_t cap, cte_t *slot)
{
    إرجاع cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == slot;
}

ساكن finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t immediate)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t status;
    finaliseSlot_ret_t ret;

    بينما (cap_get_capType(slot->cap) != cap_null_cap) {
        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, false);

        إذا (capRemovable(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            إرجاع ret;
        }

        slot->cap = fc_ret.remainder;

        إذا (!immediate && capCyclicZombie(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            إرجاع ret;
        }

        status = reduceZombie(slot, immediate);
        إذا (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            إرجاع ret;
        }

        status = preemptionPoint();
        إذا (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            إرجاع ret;
        }
    }
    ret.status = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    إرجاع ret;
}

ساكن exception_t reduceZombie(cte_t *slot, bool_t immediate)
{
    cte_t *ptr;
    word_t n, type;
    exception_t status;

    أكد(cap_get_capType(slot->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);
    n = cap_zombie_cap_get_cap_Zombie_العدد(slot->cap);
    type = cap_zombie_cap_get_capZombieType(slot->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    أكد(n > 0);

    إذا (immediate) {
        cte_t *endSlot = &ptr[n - 1];

        status = cteDelete(endSlot, false);
        إذا (status != EXCEPTION_NONE) {
            إرجاع status;
        }

        بدل (cap_get_capType(slot->cap)) {
        حالة cap_null_cap:
            اكسر;

        حالة cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);

            إذا (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_العدد(slot->cap) == n &&
                cap_zombie_cap_get_capZombieType(slot->cap) == type) {
                أكد(cap_get_capType(endSlot->cap) == cap_null_cap);
                slot->cap =
                    cap_zombie_cap_set_cap_Zombie_العدد(slot->cap, n - 1);
            } وإلا {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                أكد(ptr2 == slot && ptr != slot);
            }
            اكسر;
        }

        افتراضي:
            fail("Expected recursion to result in Zombie.");
        }
    } وإلا {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        أكد(ptr != slot);

        إذا (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            أكد(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        capSwapForDelete(ptr, slot);
    }
    إرجاع EXCEPTION_NONE;
}

فراغ cteDeleteOne(cte_t *slot)
{
    word_t cap_type = cap_get_capType(slot->cap);
    إذا (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret UNUSED;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        أكد(capRemovable(fc_ret.remainder, slot) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        emptySlot(slot, cap_null_cap_new());
    }
}

فراغ أدرج_New_Cap(cte_t *parent, cte_t *slot, cap_t cap)
{
    cte_t *next;

    next = CTE_PTR(mdb_عقدة_get_mdb_Next(parent->cteMDBNode));
    slot->cap = cap;
    slot->cteMDBNode = mdb_عقدة_new(CTE_REF(next), true, true, CTE_REF(parent));
    إذا (next) {
        mdb_عقدة_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(slot));
    }
    mdb_عقدة_ptr_set_mdb_Next(&parent->cteMDBNode, CTE_REF(slot));
}

#ifndef CONFIG_KERNEL_MCS
فراغ setupReplyMaster(tcb_t *thread)
{
    cte_t *slot;

    slot = TCB_PTR_CTE_PTR(thread, tcbReply);
    إذا (cap_get_capType(slot->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        slot->cap = cap_reply_cap_new(true, true, TCB_REF(thread));
        slot->cteMDBNode = nullMDBNode;
        mdb_عقدة_ptr_set_mdb_Revocable(&slot->cteMDBNode, true);
        mdb_عقدة_ptr_set_mdb_First_Badged(&slot->cteMDBNode, true);
    }
}
#endif

bool_t PURE isMDBParentOf(cte_t *cte_a, cte_t *cte_b)
{
    إذا (!mdb_عقدة_get_mdb_Revocable(cte_a->cteMDBNode)) {
        إرجاع false;
    }
    إذا (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        إرجاع false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    إذا (!Arch_isMDBParentOf(cte_a->cap, cte_b->cap,
                            mdb_عقدة_get_mdb_First_Badged(cte_b->cteMDBNode))) {
        إرجاع false;
    }
    بدل (cap_get_capType(cte_a->cap)) {
    حالة cap_endpoint_cap: {
        word_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        إذا (badge == 0) {
            إرجاع true;
        }
        إرجاع (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_عقدة_get_mdb_First_Badged(cte_b->cteMDBNode);
        اكسر;
    }

    حالة cap_notification_cap: {
        word_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        إذا (badge == 0) {
            إرجاع true;
        }
        إرجاع (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_عقدة_get_mdb_First_Badged(cte_b->cteMDBNode);
        اكسر;
    }

    افتراضي:
        إرجاع true;
        اكسر;
    }
}

exception_t ensureNoChildren(cte_t *slot)
{
    إذا (mdb_عقدة_get_mdb_Next(slot->cteMDBNode) != 0) {
        cte_t *next;

        next = CTE_PTR(mdb_عقدة_get_mdb_Next(slot->cteMDBNode));
        إذا (isMDBParentOf(slot, next)) {
            current_syscall_error.type = seL4_RevokeFirst;
            إرجاع EXCEPTION_SYSCALL_ERROR;
        }
    }

    إرجاع EXCEPTION_NONE;
}

exception_t ensureEmptySlot(cte_t *slot)
{
    إذا (cap_get_capType(slot->cap) != cap_null_cap) {
        current_syscall_error.type = seL4_DeleteFirst;
        إرجاع EXCEPTION_SYSCALL_ERROR;
    }

    إرجاع EXCEPTION_NONE;
}

bool_t PURE isFinalCapability(cte_t *cte)
{
    mdb_عقدة_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    إذا (mdb_عقدة_get_mdb_Prev(mdb) == 0) {
        prevIsSameObject = false;
    } وإلا {
        cte_t *prev;

        prev = CTE_PTR(mdb_عقدة_get_mdb_Prev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    إذا (prevIsSameObject) {
        إرجاع false;
    } وإلا {
        إذا (mdb_عقدة_get_mdb_Next(mdb) == 0) {
            إرجاع true;
        } وإلا {
            cte_t *next;

            next = CTE_PTR(mdb_عقدة_get_mdb_Next(mdb));
            إرجاع !sameObjectAs(cte->cap, next->cap);
        }
    }
}

bool_t PURE slotCapLongRunningDelete(cte_t *slot)
{
    إذا (cap_get_capType(slot->cap) == cap_null_cap) {
        إرجاع false;
    } وإلا إذا (! isFinalCapability(slot)) {
        إرجاع false;
    }
    بدل (cap_get_capType(slot->cap)) {
    حالة cap_thread_cap:
    حالة cap_zombie_cap:
    حالة cap_cnode_cap:
        إرجاع true;
    افتراضي:
        إرجاع false;
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

    إذا (!buffer) {
        إرجاع عدم;
    }

    ct = loadCapTransfer(buffer);
    cptr = ct.ct_Receive_جذر;

    luc_ret = lookupCap(thread, cptr);
    إذا (luc_ret.status != EXCEPTION_NONE) {
        إرجاع عدم;
    }
    cnode = luc_ret.cap;

    lus_ret = lookupTargetSlot(cnode, ct.ctReceiveIndex, ct.ctReceiveDepth);
    إذا (lus_ret.status != EXCEPTION_NONE) {
        إرجاع عدم;
    }
    slot = lus_ret.slot;

    إذا (cap_get_capType(slot->cap) != cap_null_cap) {
        إرجاع عدم;
    }

    إرجاع slot;
}

cap_transfer_t PURE loadCapTransfer(word_t *buffer)
{
    ثابت صحيح offset = se_L4_رسالة_Max_Length + se_L4_رسالة_Max_Extra_Caps + 2;
    إرجاع cap_Transfer_From_كلمات(buffer + offset);
}
