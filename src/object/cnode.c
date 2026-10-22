/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <assert.h>
#شامل <types.h>
#شامل <api/failures.h>
#شامل <api/invocation.h>
#شامل <api/syscall.h>
#شامل <api/types.h>
#شامل <machine/io.h>
#شامل <object/structures.h>
#شامل <object/objecttype.h>
#شامل <object/cnode.h>
#شامل <object/interrupt.h>
#شامل <object/untyped.h>
#شامل <kernel/cspace.h>
#شامل <kernel/thread.h>
#شامل <model/preemption.h>
#شامل <model/statedata.h>
#شامل <util.h>

ساختار finaliseSlot_ret {
    exception_t status;
    bool_t success;
    cap_t cleanupInfo;
};
تعریفنوع ساختار finaliseSlot_ret finaliseSlot_ret_t;

ایستا finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t exposed);
ایستا تهی emptySlot(cte_t *slot, cap_t cleanupInfo);
ایستا exception_t reduceZombie(cte_t *slot, bool_t exposed);

#ifdef CONFIG_KERNEL_MCS
#define CNODE_LAST_INVOCATION CNodeRotate
#وگرنه
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

    اگر (invLabel < CNodeRevoke || invLabel > CNODE_LAST_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    اگر (length < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    w_bits = getSyscallArg(1, buffer);

    lu_ret = lookupTargetSlot(cap, index, w_bits);
    اگر (lu_ret.status != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        بازگشت lu_ret.status;
    }
    destSlot = lu_ret.slot;

    اگر (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *srcSlot;
        word_t srcIndex, srcDepth, capData;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_ریشه, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        اگر (length < 4 || current_extra_caps.excaprefs[0] == پوچ) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        srcIndex = getSyscallArg(2, buffer);
        srcDepth = getSyscallArg(3, buffer);

        src_ریشه = current_extra_caps.excaprefs[0]->cap;

        status = ensureEmptySlot(destSlot);
        اگر (status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            بازگشت status;
        }

        lu_ret = lookupSourceSlot(src_ریشه, srcIndex, srcDepth);
        اگر (lu_ret.status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            بازگشت lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        اگر (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(srcDepth);
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }

        انتخاب (invLabel) {
        حالت CNodeCopy:

            اگر (length < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = seL4_TruncatedMessage;
                بازگشت EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot, srcCap);
            اگر (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                بازگشت dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            بشکن;

        حالت CNodeMint:
            اگر (length < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                بازگشت EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            capData = getSyscallArg(5, buffer);
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot,
                               updateCapData(false, capData, srcCap));
            اگر (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                بازگشت dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            بشکن;

        حالت CNodeMove:
            newCap = srcSlot->cap;
            isMove = true;

            بشکن;

        حالت CNodeMutate:
            اگر (length < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                بازگشت EXCEPTION_SYSCALL_ERROR;
            }

            capData = getSyscallArg(4, buffer);
            newCap = updateCapData(true, capData, srcSlot->cap);
            isMove = true;

            بشکن;

        پیشفرض:
            أكد(0);
            بازگشت EXCEPTION_NONE;
        }

        اگر (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        اگر (isMove) {
            بازگشت invokeCNodeMove(newCap, srcSlot, destSlot);
        } وگرنه {
            بازگشت invoke_CNode_درج(newCap, srcSlot, destSlot);
        }
    }

    اگر (invLabel == CNodeRevoke) {
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        بازگشت invokeCNodeRevoke(destSlot);
    }

    اگر (invLabel == CNodeDelete) {
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        بازگشت invokeCNodeDelete(destSlot);
    }

#ifndef CONFIG_KERNEL_MCS
    اگر (invLabel == CNodeSaveCaller) {
        status = ensureEmptySlot(destSlot);
        اگر (status != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            بازگشت status;
        }

        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        بازگشت invokeCNodeSaveCaller(destSlot);
    }
#endif

    اگر (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = destSlot->cap;

        اگر (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        بازگشت invokeCNodeCancelBadgedSends(destCap);
    }

    اگر (invLabel == CNodeRotate) {
        word_t pivotNewData, pivotIndex, pivotDepth;
        word_t srcNewData, srcIndex, srcDepth;
        cte_t *pivotSlot, *srcSlot;
        cap_t pivot_ریشه, src_ریشه, newSrcCap, newPivotCap;

        اگر (length < 8 || current_extra_caps.excaprefs[0] == پوچ
            || current_extra_caps.excaprefs[1] == پوچ) {
            current_syscall_error.type = seL4_TruncatedMessage;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        pivotNewData = getSyscallArg(2, buffer);
        pivotIndex   = getSyscallArg(3, buffer);
        pivotDepth   = getSyscallArg(4, buffer);
        srcNewData   = getSyscallArg(5, buffer);
        srcIndex     = getSyscallArg(6, buffer);
        srcDepth     = getSyscallArg(7, buffer);

        pivot_ریشه = current_extra_caps.excaprefs[0]->cap;
        src_ریشه   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookupSourceSlot(src_ریشه, srcIndex, srcDepth);
        اگر (lu_ret.status != EXCEPTION_NONE) {
            بازگشت lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        lu_ret = lookupPivotSlot(pivot_ریشه, pivotIndex, pivotDepth);
        اگر (lu_ret.status != EXCEPTION_NONE) {
            بازگشت lu_ret.status;
        }
        pivotSlot = lu_ret.slot;

        اگر (pivotSlot == srcSlot || pivotSlot == destSlot) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }

        اگر (srcSlot != destSlot) {
            status = ensureEmptySlot(destSlot);
            اگر (status != EXCEPTION_NONE) {
                بازگشت status;
            }
        }

        اگر (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(srcDepth);
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }

        اگر (cap_get_capType(pivotSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(pivotDepth);
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = updateCapData(true, srcNewData, srcSlot->cap);
        newPivotCap = updateCapData(true, pivotNewData, pivotSlot->cap);

        اگر (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }

        اگر (cap_get_capType(newPivotCap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        بازگشت invokeCNodeRotate(newSrcCap, newPivotCap,
                                 srcSlot, pivotSlot, destSlot);
    }

    بازگشت EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *destSlot)
{
    بازگشت cteRevoke(destSlot);
}

exception_t invokeCNodeDelete(cte_t *destSlot)
{
    بازگشت cteDelete(destSlot, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    word_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    اگر (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    بازگشت EXCEPTION_NONE;
}

exception_t invoke_CNode_درج(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cte_درج(cap, srcSlot, destSlot);

    بازگشت EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cteMove(cap, srcSlot, destSlot);

    بازگشت EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    اگر (slot1 == slot3) {
        cteSwap(cap1, slot1, cap2, slot2);
    } وگرنه {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    بازگشت EXCEPTION_NONE;
}

#ifndef CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *destSlot)
{
    cap_t cap;
    cte_t *srcSlot;

    srcSlot = TCB_PTR_CTE_PTR(گره_STATE(ksCurThread), tcbCaller);
    cap = srcSlot->cap;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        بشکن;

    حالت cap_reply_cap:
        اگر (!cap_reply_cap_get_capReplyMaster(cap)) {
            cteMove(cap, srcSlot, destSlot);
        }
        بشکن;

    پیشفرض:
        fail("caller capability must be null or reply");
        بشکن;
    }

    بازگشت EXCEPTION_NONE;
}
#endif

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
ایستا تهی setUntypedCapAsFull(cap_t srcCap, cap_t newCap, cte_t *srcSlot)
{
    اگر ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        اگر ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_capBlockSize(newCap)
                == cap_untyped_cap_get_capBlockSize(srcCap))) {
            cap_untyped_cap_ptr_set_capFreeIndex(&(srcSlot->cap),
                                                 MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(srcCap)));
        }
    }
}

تهی cte_درج(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_گره_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = srcSlot->cteMDBNode;
    srcCap = srcSlot->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_گره_set_mdb_Prev(srcMDB, CTE_REF(srcSlot));
    newMDB = mdb_گره_set_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_گره_set_mdb_First_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    أكد(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    أكد((cte_t *)mdb_گره_get_mdb_Next(destSlot->cteMDBNode) == پوچ &&
           (cte_t *)mdb_گره_get_mdb_Prev(destSlot->cteMDBNode) == پوچ);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    setUntypedCapAsFull(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSlot->cteMDBNode = newMDB;
    mdb_گره_ptr_set_mdb_Next(&srcSlot->cteMDBNode, CTE_REF(destSlot));
    اگر (mdb_گره_get_mdb_Next(newMDB)) {
        mdb_گره_ptr_set_mdb_Prev(
            &CTE_PTR(mdb_گره_get_mdb_Next(newMDB))->cteMDBNode,
            CTE_REF(destSlot));
    }
}

تهی cteMove(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_گره_t mdb;
    word_t prev_ptr, next_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    أكد(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    أكد((cte_t *)mdb_گره_get_mdb_Next(destSlot->cteMDBNode) == پوچ &&
           (cte_t *)mdb_گره_get_mdb_Prev(destSlot->cteMDBNode) == پوچ);

    mdb = srcSlot->cteMDBNode;
    destSlot->cap = newCap;
    srcSlot->cap = cap_null_cap_new();
    destSlot->cteMDBNode = mdb;
    srcSlot->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_گره_get_mdb_Prev(mdb);
    اگر (prev_ptr)
        mdb_گره_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(destSlot));

    next_ptr = mdb_گره_get_mdb_Next(mdb);
    اگر (next_ptr)
        mdb_گره_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(destSlot));
}

تهی capSwapForDelete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    اگر (slot1 == slot2) {
        بازگشت;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cteSwap(cap1, slot1, cap2, slot2);
}

تهی cteSwap(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_گره_t mdb1, mdb2;
    word_t next_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_گره_get_mdb_Prev(mdb1);
    اگر (prev_ptr)
        mdb_گره_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    next_ptr = mdb_گره_get_mdb_Next(mdb1);
    اگر (next_ptr)
        mdb_گره_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_گره_get_mdb_Prev(mdb2);
    اگر (prev_ptr)
        mdb_گره_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    next_ptr = mdb_گره_get_mdb_Next(mdb2);
    اگر (next_ptr)
        mdb_گره_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *slot)
{
    cte_t *nextPtr;
    exception_t status;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    برای (nextPtr = CTE_PTR(mdb_گره_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_گره_get_mdb_Next(slot->cteMDBNode))) {
        status = cteDelete(nextPtr, true);
        اگر (status != EXCEPTION_NONE) {
            بازگشت status;
        }

        status = preemptionPoint();
        اگر (status != EXCEPTION_NONE) {
            بازگشت status;
        }
    }

    بازگشت EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *slot, bool_t exposed)
{
    finaliseSlot_ret_t fs_ret;

    fs_ret = finaliseSlot(slot, exposed);
    اگر (fs_ret.status != EXCEPTION_NONE) {
        بازگشت fs_ret.status;
    }

    اگر (exposed || fs_ret.success) {
        emptySlot(slot, fs_ret.cleanupInfo);
    }
    بازگشت EXCEPTION_NONE;
}

ایستا تهی emptySlot(cte_t *slot, cap_t cleanupInfo)
{
    اگر (cap_get_capType(slot->cap) != cap_null_cap) {
        mdb_گره_t mdb_گره;
        cte_t *prev, *next;

        mdb_گره = slot->cteMDBNode;
        prev = CTE_PTR(mdb_گره_get_mdb_Prev(mdb_گره));
        next = CTE_PTR(mdb_گره_get_mdb_Next(mdb_گره));

        اگر (prev) {
            mdb_گره_ptr_set_mdb_Next(&prev->cteMDBNode, CTE_REF(next));
        }
        اگر (next) {
            mdb_گره_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(prev));
        }
        اگر (next)
            mdb_گره_ptr_set_mdb_First_Badged(&next->cteMDBNode,
                                            mdb_گره_get_mdb_First_Badged(next->cteMDBNode) ||
                                            mdb_گره_get_mdb_First_Badged(mdb_گره));
        slot->cap = cap_null_cap_new();
        slot->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

ایستا درونخطی bool_t CONST capRemovable(cap_t cap, cte_t *slot)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_null_cap:
        بازگشت true;
    حالت cap_zombie_cap: {
        word_t n = cap_zombie_cap_get_cap_Zombie_عدد(cap);
        cte_t *z_slot = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        بازگشت (n == 0 || (n == 1 && slot == z_slot));
    }
    پیشفرض:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

ایستا درونخطی bool_t CONST capCyclicZombie(cap_t cap, cte_t *slot)
{
    بازگشت cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == slot;
}

ایستا finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t immediate)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t status;
    finaliseSlot_ret_t ret;

    وقتی (cap_get_capType(slot->cap) != cap_null_cap) {
        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, false);

        اگر (capRemovable(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            بازگشت ret;
        }

        slot->cap = fc_ret.remainder;

        اگر (!immediate && capCyclicZombie(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            بازگشت ret;
        }

        status = reduceZombie(slot, immediate);
        اگر (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            بازگشت ret;
        }

        status = preemptionPoint();
        اگر (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            بازگشت ret;
        }
    }
    ret.status = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    بازگشت ret;
}

ایستا exception_t reduceZombie(cte_t *slot, bool_t immediate)
{
    cte_t *ptr;
    word_t n, type;
    exception_t status;

    أكد(cap_get_capType(slot->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);
    n = cap_zombie_cap_get_cap_Zombie_عدد(slot->cap);
    type = cap_zombie_cap_get_capZombieType(slot->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    أكد(n > 0);

    اگر (immediate) {
        cte_t *endSlot = &ptr[n - 1];

        status = cteDelete(endSlot, false);
        اگر (status != EXCEPTION_NONE) {
            بازگشت status;
        }

        انتخاب (cap_get_capType(slot->cap)) {
        حالت cap_null_cap:
            بشکن;

        حالت cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);

            اگر (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_عدد(slot->cap) == n &&
                cap_zombie_cap_get_capZombieType(slot->cap) == type) {
                أكد(cap_get_capType(endSlot->cap) == cap_null_cap);
                slot->cap =
                    cap_zombie_cap_set_cap_Zombie_عدد(slot->cap, n - 1);
            } وگرنه {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                أكد(ptr2 == slot && ptr != slot);
            }
            بشکن;
        }

        پیشفرض:
            fail("Expected recursion to result in Zombie.");
        }
    } وگرنه {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        أكد(ptr != slot);

        اگر (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            أكد(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        capSwapForDelete(ptr, slot);
    }
    بازگشت EXCEPTION_NONE;
}

تهی cteDeleteOne(cte_t *slot)
{
    word_t cap_type = cap_get_capType(slot->cap);
    اگر (cap_type != cap_null_cap) {
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

تهی درج_New_Cap(cte_t *parent, cte_t *slot, cap_t cap)
{
    cte_t *next;

    next = CTE_PTR(mdb_گره_get_mdb_Next(parent->cteMDBNode));
    slot->cap = cap;
    slot->cteMDBNode = mdb_گره_new(CTE_REF(next), true, true, CTE_REF(parent));
    اگر (next) {
        mdb_گره_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(slot));
    }
    mdb_گره_ptr_set_mdb_Next(&parent->cteMDBNode, CTE_REF(slot));
}

#ifndef CONFIG_KERNEL_MCS
تهی setupReplyMaster(tcb_t *thread)
{
    cte_t *slot;

    slot = TCB_PTR_CTE_PTR(thread, tcbReply);
    اگر (cap_get_capType(slot->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        slot->cap = cap_reply_cap_new(true, true, TCB_REF(thread));
        slot->cteMDBNode = nullMDBNode;
        mdb_گره_ptr_set_mdb_Revocable(&slot->cteMDBNode, true);
        mdb_گره_ptr_set_mdb_First_Badged(&slot->cteMDBNode, true);
    }
}
#endif

bool_t PURE isMDBParentOf(cte_t *cte_a, cte_t *cte_b)
{
    اگر (!mdb_گره_get_mdb_Revocable(cte_a->cteMDBNode)) {
        بازگشت false;
    }
    اگر (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        بازگشت false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    اگر (!Arch_isMDBParentOf(cte_a->cap, cte_b->cap,
                            mdb_گره_get_mdb_First_Badged(cte_b->cteMDBNode))) {
        بازگشت false;
    }
    انتخاب (cap_get_capType(cte_a->cap)) {
    حالت cap_endpoint_cap: {
        word_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        اگر (badge == 0) {
            بازگشت true;
        }
        بازگشت (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_گره_get_mdb_First_Badged(cte_b->cteMDBNode);
        بشکن;
    }

    حالت cap_notification_cap: {
        word_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        اگر (badge == 0) {
            بازگشت true;
        }
        بازگشت (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_گره_get_mdb_First_Badged(cte_b->cteMDBNode);
        بشکن;
    }

    پیشفرض:
        بازگشت true;
        بشکن;
    }
}

exception_t ensureNoChildren(cte_t *slot)
{
    اگر (mdb_گره_get_mdb_Next(slot->cteMDBNode) != 0) {
        cte_t *next;

        next = CTE_PTR(mdb_گره_get_mdb_Next(slot->cteMDBNode));
        اگر (isMDBParentOf(slot, next)) {
            current_syscall_error.type = seL4_RevokeFirst;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
    }

    بازگشت EXCEPTION_NONE;
}

exception_t ensureEmptySlot(cte_t *slot)
{
    اگر (cap_get_capType(slot->cap) != cap_null_cap) {
        current_syscall_error.type = seL4_DeleteFirst;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    بازگشت EXCEPTION_NONE;
}

bool_t PURE isFinalCapability(cte_t *cte)
{
    mdb_گره_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    اگر (mdb_گره_get_mdb_Prev(mdb) == 0) {
        prevIsSameObject = false;
    } وگرنه {
        cte_t *prev;

        prev = CTE_PTR(mdb_گره_get_mdb_Prev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    اگر (prevIsSameObject) {
        بازگشت false;
    } وگرنه {
        اگر (mdb_گره_get_mdb_Next(mdb) == 0) {
            بازگشت true;
        } وگرنه {
            cte_t *next;

            next = CTE_PTR(mdb_گره_get_mdb_Next(mdb));
            بازگشت !sameObjectAs(cte->cap, next->cap);
        }
    }
}

bool_t PURE slotCapLongRunningDelete(cte_t *slot)
{
    اگر (cap_get_capType(slot->cap) == cap_null_cap) {
        بازگشت false;
    } وگرنه اگر (! isFinalCapability(slot)) {
        بازگشت false;
    }
    انتخاب (cap_get_capType(slot->cap)) {
    حالت cap_thread_cap:
    حالت cap_zombie_cap:
    حالت cap_cnode_cap:
        بازگشت true;
    پیشفرض:
        بازگشت false;
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

    اگر (!buffer) {
        بازگشت پوچ;
    }

    ct = loadCapTransfer(buffer);
    cptr = ct.ct_Receive_ریشه;

    luc_ret = lookupCap(thread, cptr);
    اگر (luc_ret.status != EXCEPTION_NONE) {
        بازگشت پوچ;
    }
    cnode = luc_ret.cap;

    lus_ret = lookupTargetSlot(cnode, ct.ctReceiveIndex, ct.ctReceiveDepth);
    اگر (lus_ret.status != EXCEPTION_NONE) {
        بازگشت پوچ;
    }
    slot = lus_ret.slot;

    اگر (cap_get_capType(slot->cap) != cap_null_cap) {
        بازگشت پوچ;
    }

    بازگشت slot;
}

cap_transfer_t PURE loadCapTransfer(word_t *buffer)
{
    ثابت صحیح offset = se_L4_پیام_Max_Length + se_L4_پیام_Max_Extra_Caps + 2;
    بازگشت cap_Transfer_From_واژگان(buffer + offset);
}
