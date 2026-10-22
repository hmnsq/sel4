/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <api/invocation.h>
#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <api/types.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <object/structures.h>
#அந்தர்பாவ <object/objecttype.h>
#அந்தர்பாவ <object/cnode.h>
#அந்தர்பாவ <object/interrupt.h>
#அந்தர்பாவ <object/untyped.h>
#அந்தர்பாவ <kernel/cspace.h>
#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <model/preemption.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <util.h>

கட்டமைப்பு finaliseSlot_ret {
    exception_t status;
    bool_t success;
    cap_t cleanupInfo;
};
வகைவரை கட்டமைப்பு finaliseSlot_ret finaliseSlot_ret_t;

நிலையான finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t exposed);
நிலையான வெறுமை emptySlot(cte_t *slot, cap_t cleanupInfo);
நிலையான exception_t reduceZombie(cte_t *slot, bool_t exposed);

#ifdef CONFIG_KERNEL_MCS
#define CNODE_LAST_INVOCATION CNodeRotate
#இல்லையெனில்
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
    ப்ரதிஜ்ஞா(cap_get_capType(cap) == cap_cnode_cap);

    எனில் (invLabel < CNodeRevoke || invLabel > CNODE_LAST_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (length < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    w_bits = getSyscallArg(1, buffer);

    lu_ret = lookupTargetSlot(cap, index, w_bits);
    எனில் (lu_ret.status != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        திரும்பு lu_ret.status;
    }
    destSlot = lu_ret.slot;

    எனில் (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *srcSlot;
        word_t srcIndex, srcDepth, capData;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_வேர், newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        எனில் (length < 4 || current_extra_caps.excaprefs[0] == வெற்று) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        srcIndex = getSyscallArg(2, buffer);
        srcDepth = getSyscallArg(3, buffer);

        src_வேர் = current_extra_caps.excaprefs[0]->cap;

        status = ensureEmptySlot(destSlot);
        எனில் (status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            திரும்பு status;
        }

        lu_ret = lookupSourceSlot(src_வேர், srcIndex, srcDepth);
        எனில் (lu_ret.status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            திரும்பு lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        எனில் (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(srcDepth);
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        தேர்வு (invLabel) {
        நிலை CNodeCopy:

            எனில் (length < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = seL4_TruncatedMessage;
                திரும்பு EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot, srcCap);
            எனில் (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                திரும்பு dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            நிறுத்து;

        நிலை CNodeMint:
            எனில் (length < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                திரும்பு EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            capData = getSyscallArg(5, buffer);
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot,
                               updateCapData(false, capData, srcCap));
            எனில் (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                திரும்பு dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            நிறுத்து;

        நிலை CNodeMove:
            newCap = srcSlot->cap;
            isMove = true;

            நிறுத்து;

        நிலை CNodeMutate:
            எனில் (length < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                திரும்பு EXCEPTION_SYSCALL_ERROR;
            }

            capData = getSyscallArg(4, buffer);
            newCap = updateCapData(true, capData, srcSlot->cap);
            isMove = true;

            நிறுத்து;

        இயல்பு:
            ப்ரதிஜ்ஞா(0);
            திரும்பு EXCEPTION_NONE;
        }

        எனில் (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        எனில் (isMove) {
            திரும்பு invokeCNodeMove(newCap, srcSlot, destSlot);
        } இல்லையெனில் {
            திரும்பு invoke_CNode_செருகு(newCap, srcSlot, destSlot);
        }
    }

    எனில் (invLabel == CNodeRevoke) {
        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு invokeCNodeRevoke(destSlot);
    }

    எனில் (invLabel == CNodeDelete) {
        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு invokeCNodeDelete(destSlot);
    }

#ifndef CONFIG_KERNEL_MCS
    எனில் (invLabel == CNodeSaveCaller) {
        status = ensureEmptySlot(destSlot);
        எனில் (status != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            திரும்பு status;
        }

        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு invokeCNodeSaveCaller(destSlot);
    }
#endif

    எனில் (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = destSlot->cap;

        எனில் (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு invokeCNodeCancelBadgedSends(destCap);
    }

    எனில் (invLabel == CNodeRotate) {
        word_t pivotNewData, pivotIndex, pivotDepth;
        word_t srcNewData, srcIndex, srcDepth;
        cte_t *pivotSlot, *srcSlot;
        cap_t pivot_வேர், src_வேர், newSrcCap, newPivotCap;

        எனில் (length < 8 || current_extra_caps.excaprefs[0] == வெற்று
            || current_extra_caps.excaprefs[1] == வெற்று) {
            current_syscall_error.type = seL4_TruncatedMessage;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        pivotNewData = getSyscallArg(2, buffer);
        pivotIndex   = getSyscallArg(3, buffer);
        pivotDepth   = getSyscallArg(4, buffer);
        srcNewData   = getSyscallArg(5, buffer);
        srcIndex     = getSyscallArg(6, buffer);
        srcDepth     = getSyscallArg(7, buffer);

        pivot_வேர் = current_extra_caps.excaprefs[0]->cap;
        src_வேர்   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookupSourceSlot(src_வேர், srcIndex, srcDepth);
        எனில் (lu_ret.status != EXCEPTION_NONE) {
            திரும்பு lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        lu_ret = lookupPivotSlot(pivot_வேர், pivotIndex, pivotDepth);
        எனில் (lu_ret.status != EXCEPTION_NONE) {
            திரும்பு lu_ret.status;
        }
        pivotSlot = lu_ret.slot;

        எனில் (pivotSlot == srcSlot || pivotSlot == destSlot) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        எனில் (srcSlot != destSlot) {
            status = ensureEmptySlot(destSlot);
            எனில் (status != EXCEPTION_NONE) {
                திரும்பு status;
            }
        }

        எனில் (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(srcDepth);
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        எனில் (cap_get_capType(pivotSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(pivotDepth);
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = updateCapData(true, srcNewData, srcSlot->cap);
        newPivotCap = updateCapData(true, pivotNewData, pivotSlot->cap);

        எனில் (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        எனில் (cap_get_capType(newPivotCap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு invokeCNodeRotate(newSrcCap, newPivotCap,
                                 srcSlot, pivotSlot, destSlot);
    }

    திரும்பு EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *destSlot)
{
    திரும்பு cteRevoke(destSlot);
}

exception_t invokeCNodeDelete(cte_t *destSlot)
{
    திரும்பு cteDelete(destSlot, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    word_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    எனில் (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    திரும்பு EXCEPTION_NONE;
}

exception_t invoke_CNode_செருகு(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cte_செருகு(cap, srcSlot, destSlot);

    திரும்பு EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cteMove(cap, srcSlot, destSlot);

    திரும்பு EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    எனில் (slot1 == slot3) {
        cteSwap(cap1, slot1, cap2, slot2);
    } இல்லையெனில் {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    திரும்பு EXCEPTION_NONE;
}

#ifndef CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *destSlot)
{
    cap_t cap;
    cte_t *srcSlot;

    srcSlot = TCB_PTR_CTE_PTR(முனை_STATE(ksCurThread), tcbCaller);
    cap = srcSlot->cap;

    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        நிறுத்து;

    நிலை cap_reply_cap:
        எனில் (!cap_reply_cap_get_capReplyMaster(cap)) {
            cteMove(cap, srcSlot, destSlot);
        }
        நிறுத்து;

    இயல்பு:
        fail("caller capability must be null or reply");
        நிறுத்து;
    }

    திரும்பு EXCEPTION_NONE;
}
#endif

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
நிலையான வெறுமை setUntypedCapAsFull(cap_t srcCap, cap_t newCap, cte_t *srcSlot)
{
    எனில் ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        எனில் ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_capBlockSize(newCap)
                == cap_untyped_cap_get_capBlockSize(srcCap))) {
            cap_untyped_cap_ptr_set_capFreeIndex(&(srcSlot->cap),
                                                 MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(srcCap)));
        }
    }
}

வெறுமை cte_செருகு(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_முனை_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = srcSlot->cteMDBNode;
    srcCap = srcSlot->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_முனை_set_mdb_Prev(srcMDB, CTE_REF(srcSlot));
    newMDB = mdb_முனை_set_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_முனை_set_mdb_First_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    ப்ரதிஜ்ஞா(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    ப்ரதிஜ்ஞா((cte_t *)mdb_முனை_get_mdb_Next(destSlot->cteMDBNode) == வெற்று &&
           (cte_t *)mdb_முனை_get_mdb_Prev(destSlot->cteMDBNode) == வெற்று);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    setUntypedCapAsFull(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSlot->cteMDBNode = newMDB;
    mdb_முனை_ptr_set_mdb_Next(&srcSlot->cteMDBNode, CTE_REF(destSlot));
    எனில் (mdb_முனை_get_mdb_Next(newMDB)) {
        mdb_முனை_ptr_set_mdb_Prev(
            &CTE_PTR(mdb_முனை_get_mdb_Next(newMDB))->cteMDBNode,
            CTE_REF(destSlot));
    }
}

வெறுமை cteMove(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_முனை_t mdb;
    word_t prev_ptr, next_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    ப்ரதிஜ்ஞா(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    ப்ரதிஜ்ஞா((cte_t *)mdb_முனை_get_mdb_Next(destSlot->cteMDBNode) == வெற்று &&
           (cte_t *)mdb_முனை_get_mdb_Prev(destSlot->cteMDBNode) == வெற்று);

    mdb = srcSlot->cteMDBNode;
    destSlot->cap = newCap;
    srcSlot->cap = cap_null_cap_new();
    destSlot->cteMDBNode = mdb;
    srcSlot->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_முனை_get_mdb_Prev(mdb);
    எனில் (prev_ptr)
        mdb_முனை_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(destSlot));

    next_ptr = mdb_முனை_get_mdb_Next(mdb);
    எனில் (next_ptr)
        mdb_முனை_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(destSlot));
}

வெறுமை capSwapForDelete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    எனில் (slot1 == slot2) {
        திரும்பு;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cteSwap(cap1, slot1, cap2, slot2);
}

வெறுமை cteSwap(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_முனை_t mdb1, mdb2;
    word_t next_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_முனை_get_mdb_Prev(mdb1);
    எனில் (prev_ptr)
        mdb_முனை_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    next_ptr = mdb_முனை_get_mdb_Next(mdb1);
    எனில் (next_ptr)
        mdb_முனை_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_முனை_get_mdb_Prev(mdb2);
    எனில் (prev_ptr)
        mdb_முனை_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    next_ptr = mdb_முனை_get_mdb_Next(mdb2);
    எனில் (next_ptr)
        mdb_முனை_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *slot)
{
    cte_t *nextPtr;
    exception_t status;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    க்கு (nextPtr = CTE_PTR(mdb_முனை_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_முனை_get_mdb_Next(slot->cteMDBNode))) {
        status = cteDelete(nextPtr, true);
        எனில் (status != EXCEPTION_NONE) {
            திரும்பு status;
        }

        status = preemptionPoint();
        எனில் (status != EXCEPTION_NONE) {
            திரும்பு status;
        }
    }

    திரும்பு EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *slot, bool_t exposed)
{
    finaliseSlot_ret_t fs_ret;

    fs_ret = finaliseSlot(slot, exposed);
    எனில் (fs_ret.status != EXCEPTION_NONE) {
        திரும்பு fs_ret.status;
    }

    எனில் (exposed || fs_ret.success) {
        emptySlot(slot, fs_ret.cleanupInfo);
    }
    திரும்பு EXCEPTION_NONE;
}

நிலையான வெறுமை emptySlot(cte_t *slot, cap_t cleanupInfo)
{
    எனில் (cap_get_capType(slot->cap) != cap_null_cap) {
        mdb_முனை_t mdb_முனை;
        cte_t *prev, *next;

        mdb_முனை = slot->cteMDBNode;
        prev = CTE_PTR(mdb_முனை_get_mdb_Prev(mdb_முனை));
        next = CTE_PTR(mdb_முனை_get_mdb_Next(mdb_முனை));

        எனில் (prev) {
            mdb_முனை_ptr_set_mdb_Next(&prev->cteMDBNode, CTE_REF(next));
        }
        எனில் (next) {
            mdb_முனை_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(prev));
        }
        எனில் (next)
            mdb_முனை_ptr_set_mdb_First_Badged(&next->cteMDBNode,
                                            mdb_முனை_get_mdb_First_Badged(next->cteMDBNode) ||
                                            mdb_முனை_get_mdb_First_Badged(mdb_முனை));
        slot->cap = cap_null_cap_new();
        slot->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

நிலையான உள்வரி bool_t CONST capRemovable(cap_t cap, cte_t *slot)
{
    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_null_cap:
        திரும்பு true;
    நிலை cap_zombie_cap: {
        word_t n = cap_zombie_cap_get_cap_Zombie_எண்(cap);
        cte_t *z_slot = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        திரும்பு (n == 0 || (n == 1 && slot == z_slot));
    }
    இயல்பு:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

நிலையான உள்வரி bool_t CONST capCyclicZombie(cap_t cap, cte_t *slot)
{
    திரும்பு cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == slot;
}

நிலையான finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t immediate)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t status;
    finaliseSlot_ret_t ret;

    போது (cap_get_capType(slot->cap) != cap_null_cap) {
        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, false);

        எனில் (capRemovable(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            திரும்பு ret;
        }

        slot->cap = fc_ret.remainder;

        எனில் (!immediate && capCyclicZombie(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            திரும்பு ret;
        }

        status = reduceZombie(slot, immediate);
        எனில் (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            திரும்பு ret;
        }

        status = preemptionPoint();
        எனில் (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            திரும்பு ret;
        }
    }
    ret.status = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    திரும்பு ret;
}

நிலையான exception_t reduceZombie(cte_t *slot, bool_t immediate)
{
    cte_t *ptr;
    word_t n, type;
    exception_t status;

    ப்ரதிஜ்ஞா(cap_get_capType(slot->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);
    n = cap_zombie_cap_get_cap_Zombie_எண்(slot->cap);
    type = cap_zombie_cap_get_capZombieType(slot->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    ப்ரதிஜ்ஞா(n > 0);

    எனில் (immediate) {
        cte_t *endSlot = &ptr[n - 1];

        status = cteDelete(endSlot, false);
        எனில் (status != EXCEPTION_NONE) {
            திரும்பு status;
        }

        தேர்வு (cap_get_capType(slot->cap)) {
        நிலை cap_null_cap:
            நிறுத்து;

        நிலை cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);

            எனில் (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_எண்(slot->cap) == n &&
                cap_zombie_cap_get_capZombieType(slot->cap) == type) {
                ப்ரதிஜ்ஞா(cap_get_capType(endSlot->cap) == cap_null_cap);
                slot->cap =
                    cap_zombie_cap_set_cap_Zombie_எண்(slot->cap, n - 1);
            } இல்லையெனில் {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                ப்ரதிஜ்ஞா(ptr2 == slot && ptr != slot);
            }
            நிறுத்து;
        }

        இயல்பு:
            fail("Expected recursion to result in Zombie.");
        }
    } இல்லையெனில் {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        ப்ரதிஜ்ஞா(ptr != slot);

        எனில் (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            ப்ரதிஜ்ஞா(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        capSwapForDelete(ptr, slot);
    }
    திரும்பு EXCEPTION_NONE;
}

வெறுமை cteDeleteOne(cte_t *slot)
{
    word_t cap_type = cap_get_capType(slot->cap);
    எனில் (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret UNUSED;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        ப்ரதிஜ்ஞா(capRemovable(fc_ret.remainder, slot) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        emptySlot(slot, cap_null_cap_new());
    }
}

வெறுமை செருகு_New_Cap(cte_t *parent, cte_t *slot, cap_t cap)
{
    cte_t *next;

    next = CTE_PTR(mdb_முனை_get_mdb_Next(parent->cteMDBNode));
    slot->cap = cap;
    slot->cteMDBNode = mdb_முனை_new(CTE_REF(next), true, true, CTE_REF(parent));
    எனில் (next) {
        mdb_முனை_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(slot));
    }
    mdb_முனை_ptr_set_mdb_Next(&parent->cteMDBNode, CTE_REF(slot));
}

#ifndef CONFIG_KERNEL_MCS
வெறுமை setupReplyMaster(tcb_t *thread)
{
    cte_t *slot;

    slot = TCB_PTR_CTE_PTR(thread, tcbReply);
    எனில் (cap_get_capType(slot->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        slot->cap = cap_reply_cap_new(true, true, TCB_REF(thread));
        slot->cteMDBNode = nullMDBNode;
        mdb_முனை_ptr_set_mdb_Revocable(&slot->cteMDBNode, true);
        mdb_முனை_ptr_set_mdb_First_Badged(&slot->cteMDBNode, true);
    }
}
#endif

bool_t PURE isMDBParentOf(cte_t *cte_a, cte_t *cte_b)
{
    எனில் (!mdb_முனை_get_mdb_Revocable(cte_a->cteMDBNode)) {
        திரும்பு false;
    }
    எனில் (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        திரும்பு false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    எனில் (!Arch_isMDBParentOf(cte_a->cap, cte_b->cap,
                            mdb_முனை_get_mdb_First_Badged(cte_b->cteMDBNode))) {
        திரும்பு false;
    }
    தேர்வு (cap_get_capType(cte_a->cap)) {
    நிலை cap_endpoint_cap: {
        word_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        எனில் (badge == 0) {
            திரும்பு true;
        }
        திரும்பு (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_முனை_get_mdb_First_Badged(cte_b->cteMDBNode);
        நிறுத்து;
    }

    நிலை cap_notification_cap: {
        word_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        எனில் (badge == 0) {
            திரும்பு true;
        }
        திரும்பு (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_முனை_get_mdb_First_Badged(cte_b->cteMDBNode);
        நிறுத்து;
    }

    இயல்பு:
        திரும்பு true;
        நிறுத்து;
    }
}

exception_t ensureNoChildren(cte_t *slot)
{
    எனில் (mdb_முனை_get_mdb_Next(slot->cteMDBNode) != 0) {
        cte_t *next;

        next = CTE_PTR(mdb_முனை_get_mdb_Next(slot->cteMDBNode));
        எனில் (isMDBParentOf(slot, next)) {
            current_syscall_error.type = seL4_RevokeFirst;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
    }

    திரும்பு EXCEPTION_NONE;
}

exception_t ensureEmptySlot(cte_t *slot)
{
    எனில் (cap_get_capType(slot->cap) != cap_null_cap) {
        current_syscall_error.type = seL4_DeleteFirst;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    திரும்பு EXCEPTION_NONE;
}

bool_t PURE isFinalCapability(cte_t *cte)
{
    mdb_முனை_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    எனில் (mdb_முனை_get_mdb_Prev(mdb) == 0) {
        prevIsSameObject = false;
    } இல்லையெனில் {
        cte_t *prev;

        prev = CTE_PTR(mdb_முனை_get_mdb_Prev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    எனில் (prevIsSameObject) {
        திரும்பு false;
    } இல்லையெனில் {
        எனில் (mdb_முனை_get_mdb_Next(mdb) == 0) {
            திரும்பு true;
        } இல்லையெனில் {
            cte_t *next;

            next = CTE_PTR(mdb_முனை_get_mdb_Next(mdb));
            திரும்பு !sameObjectAs(cte->cap, next->cap);
        }
    }
}

bool_t PURE slotCapLongRunningDelete(cte_t *slot)
{
    எனில் (cap_get_capType(slot->cap) == cap_null_cap) {
        திரும்பு false;
    } இல்லையெனில் எனில் (! isFinalCapability(slot)) {
        திரும்பு false;
    }
    தேர்வு (cap_get_capType(slot->cap)) {
    நிலை cap_thread_cap:
    நிலை cap_zombie_cap:
    நிலை cap_cnode_cap:
        திரும்பு true;
    இயல்பு:
        திரும்பு false;
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

    எனில் (!buffer) {
        திரும்பு வெற்று;
    }

    ct = loadCapTransfer(buffer);
    cptr = ct.ct_Receive_வேர்;

    luc_ret = lookupCap(thread, cptr);
    எனில் (luc_ret.status != EXCEPTION_NONE) {
        திரும்பு வெற்று;
    }
    cnode = luc_ret.cap;

    lus_ret = lookupTargetSlot(cnode, ct.ctReceiveIndex, ct.ctReceiveDepth);
    எனில் (lus_ret.status != EXCEPTION_NONE) {
        திரும்பு வெற்று;
    }
    slot = lus_ret.slot;

    எனில் (cap_get_capType(slot->cap) != cap_null_cap) {
        திரும்பு வெற்று;
    }

    திரும்பு slot;
}

cap_transfer_t PURE loadCapTransfer(word_t *buffer)
{
    மாறிலி முழு offset = se_L4_செய்தி_Max_Length + se_L4_செய்தி_Max_Extra_Caps + 2;
    திரும்பு cap_Transfer_From_சொற்கள்(buffer + offset);
}
