/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <api/invocation.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <api/types.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <object/objecttype.h>
#ಅನ್ತರ್ಭಾವ <object/cnode.h>
#ಅನ್ತರ್ಭಾವ <object/interrupt.h>
#ಅನ್ತರ್ಭಾವ <object/untyped.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <model/preemption.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <util.h>

ಸಂರಚನಾ finaliseSlot_ret {
    exception_t status;
    bool_t success;
    cap_t cleanupInfo;
};
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ finaliseSlot_ret finaliseSlot_ret_t;

ಸ್ಥಿರ finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t exposed);
ಸ್ಥಿರ ಶೂನ್ಯ emptySlot(cte_t *slot, cap_t cleanupInfo);
ಸ್ಥಿರ exception_t reduceZombie(cte_t *slot, bool_t exposed);

#ifdef CONFIG_KERNEL_MCS
#define CNODE_LAST_INVOCATION CNodeRotate
#ಅನ್ಯಥಾ
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
    ಪ್ರತಿಜ್ಞಾ(cap_get_capType(cap) == cap_cnode_cap);

    ಯದಿ (invLabel < CNodeRevoke || invLabel > CNODE_LAST_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (length < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    w_bits = getSyscallArg(1, buffer);

    lu_ret = lookupTargetSlot(cap, index, w_bits);
    ಯದಿ (lu_ret.status != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        ಪ್ರತ್ಯಾಗಮ lu_ret.status;
    }
    destSlot = lu_ret.slot;

    ಯದಿ (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *srcSlot;
        word_t srcIndex, srcDepth, capData;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_ಬೇರು, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        ಯದಿ (length < 4 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        srcIndex = getSyscallArg(2, buffer);
        srcDepth = getSyscallArg(3, buffer);

        src_ಬೇರು = current_extra_caps.excaprefs[0]->cap;

        status = ensureEmptySlot(destSlot);
        ಯದಿ (status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            ಪ್ರತ್ಯಾಗಮ status;
        }

        lu_ret = lookupSourceSlot(src_ಬೇರು, srcIndex, srcDepth);
        ಯದಿ (lu_ret.status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            ಪ್ರತ್ಯಾಗಮ lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        ಯದಿ (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(srcDepth);
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ವಿಕಲ್ಪ (invLabel) {
        ಪಕ್ಷ CNodeCopy:

            ಯದಿ (length < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = seL4_TruncatedMessage;
                ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot, srcCap);
            ಯದಿ (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                ಪ್ರತ್ಯಾಗಮ dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            ಭಙ್ಗ;

        ಪಕ್ಷ CNodeMint:
            ಯದಿ (length < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            capData = getSyscallArg(5, buffer);
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot,
                               updateCapData(false, capData, srcCap));
            ಯದಿ (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                ಪ್ರತ್ಯಾಗಮ dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            ಭಙ್ಗ;

        ಪಕ್ಷ CNodeMove:
            newCap = srcSlot->cap;
            isMove = true;

            ಭಙ್ಗ;

        ಪಕ್ಷ CNodeMutate:
            ಯದಿ (length < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
            }

            capData = getSyscallArg(4, buffer);
            newCap = updateCapData(true, capData, srcSlot->cap);
            isMove = true;

            ಭಙ್ಗ;

        ಸ್ವಭಾವ:
            ಪ್ರತಿಜ್ಞಾ(0);
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
        }

        ಯದಿ (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಯದಿ (isMove) {
            ಪ್ರತ್ಯಾಗಮ invokeCNodeMove(newCap, srcSlot, destSlot);
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ invoke_CNode_ಸೇರಿಸು(newCap, srcSlot, destSlot);
        }
    }

    ಯದಿ (invLabel == CNodeRevoke) {
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ invokeCNodeRevoke(destSlot);
    }

    ಯದಿ (invLabel == CNodeDelete) {
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ invokeCNodeDelete(destSlot);
    }

#ifndef CONFIG_KERNEL_MCS
    ಯದಿ (invLabel == CNodeSaveCaller) {
        status = ensureEmptySlot(destSlot);
        ಯದಿ (status != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            ಪ್ರತ್ಯಾಗಮ status;
        }

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ invokeCNodeSaveCaller(destSlot);
    }
#endif

    ಯದಿ (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = destSlot->cap;

        ಯದಿ (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ invokeCNodeCancelBadgedSends(destCap);
    }

    ಯದಿ (invLabel == CNodeRotate) {
        word_t pivotNewData, pivotIndex, pivotDepth;
        word_t srcNewData, srcIndex, srcDepth;
        cte_t *pivotSlot, *srcSlot;
        cap_t pivot_ಬೇರು, src_ಬೇರು, newSrcCap, newPivotCap;

        ಯದಿ (length < 8 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ
            || current_extra_caps.excaprefs[1] == ರಿಕ್ತ) {
            current_syscall_error.type = seL4_TruncatedMessage;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        pivotNewData = getSyscallArg(2, buffer);
        pivotIndex   = getSyscallArg(3, buffer);
        pivotDepth   = getSyscallArg(4, buffer);
        srcNewData   = getSyscallArg(5, buffer);
        srcIndex     = getSyscallArg(6, buffer);
        srcDepth     = getSyscallArg(7, buffer);

        pivot_ಬೇರು = current_extra_caps.excaprefs[0]->cap;
        src_ಬೇರು   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookupSourceSlot(src_ಬೇರು, srcIndex, srcDepth);
        ಯದಿ (lu_ret.status != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        lu_ret = lookupPivotSlot(pivot_ಬೇರು, pivotIndex, pivotDepth);
        ಯದಿ (lu_ret.status != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ lu_ret.status;
        }
        pivotSlot = lu_ret.slot;

        ಯದಿ (pivotSlot == srcSlot || pivotSlot == destSlot) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (srcSlot != destSlot) {
            status = ensureEmptySlot(destSlot);
            ಯದಿ (status != EXCEPTION_NONE) {
                ಪ್ರತ್ಯಾಗಮ status;
            }
        }

        ಯದಿ (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(srcDepth);
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (cap_get_capType(pivotSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(pivotDepth);
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = updateCapData(true, srcNewData, srcSlot->cap);
        newPivotCap = updateCapData(true, pivotNewData, pivotSlot->cap);

        ಯದಿ (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        ಯದಿ (cap_get_capType(newPivotCap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ invokeCNodeRotate(newSrcCap, newPivotCap,
                                 srcSlot, pivotSlot, destSlot);
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *destSlot)
{
    ಪ್ರತ್ಯಾಗಮ cteRevoke(destSlot);
}

exception_t invokeCNodeDelete(cte_t *destSlot)
{
    ಪ್ರತ್ಯಾಗಮ cteDelete(destSlot, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    word_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    ಯದಿ (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t invoke_CNode_ಸೇರಿಸು(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cte_ಸೇರಿಸು(cap, srcSlot, destSlot);

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cteMove(cap, srcSlot, destSlot);

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    ಯದಿ (slot1 == slot3) {
        cteSwap(cap1, slot1, cap2, slot2);
    } ಅನ್ಯಥಾ {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

#ifndef CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *destSlot)
{
    cap_t cap;
    cte_t *srcSlot;

    srcSlot = TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcbCaller);
    cap = srcSlot->cap;

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_reply_cap:
        ಯದಿ (!cap_reply_cap_get_capReplyMaster(cap)) {
            cteMove(cap, srcSlot, destSlot);
        }
        ಭಙ್ಗ;

    ಸ್ವಭಾವ:
        fail("caller capability must be null or reply");
        ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}
#endif

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
ಸ್ಥಿರ ಶೂನ್ಯ setUntypedCapAsFull(cap_t srcCap, cap_t newCap, cte_t *srcSlot)
{
    ಯದಿ ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        ಯದಿ ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_capBlockSize(newCap)
                == cap_untyped_cap_get_capBlockSize(srcCap))) {
            cap_untyped_cap_ptr_set_capFreeIndex(&(srcSlot->cap),
                                                 MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(srcCap)));
        }
    }
}

ಶೂನ್ಯ cte_ಸೇರಿಸು(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_ಗಂಟು_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = srcSlot->cteMDBNode;
    srcCap = srcSlot->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_ಗಂಟು_set_mdb_Prev(srcMDB, CTE_REF(srcSlot));
    newMDB = mdb_ಗಂಟು_set_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_ಗಂಟು_set_mdb_First_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    ಪ್ರತಿಜ್ಞಾ(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    ಪ್ರತಿಜ್ಞಾ((cte_t *)mdb_ಗಂಟು_get_mdb_Next(destSlot->cteMDBNode) == ರಿಕ್ತ &&
           (cte_t *)mdb_ಗಂಟು_get_mdb_Prev(destSlot->cteMDBNode) == ರಿಕ್ತ);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    setUntypedCapAsFull(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSlot->cteMDBNode = newMDB;
    mdb_ಗಂಟು_ptr_set_mdb_Next(&srcSlot->cteMDBNode, CTE_REF(destSlot));
    ಯದಿ (mdb_ಗಂಟು_get_mdb_Next(newMDB)) {
        mdb_ಗಂಟು_ptr_set_mdb_Prev(
            &CTE_PTR(mdb_ಗಂಟು_get_mdb_Next(newMDB))->cteMDBNode,
            CTE_REF(destSlot));
    }
}

ಶೂನ್ಯ cteMove(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_ಗಂಟು_t mdb;
    word_t prev_ptr, next_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    ಪ್ರತಿಜ್ಞಾ(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    ಪ್ರತಿಜ್ಞಾ((cte_t *)mdb_ಗಂಟು_get_mdb_Next(destSlot->cteMDBNode) == ರಿಕ್ತ &&
           (cte_t *)mdb_ಗಂಟು_get_mdb_Prev(destSlot->cteMDBNode) == ರಿಕ್ತ);

    mdb = srcSlot->cteMDBNode;
    destSlot->cap = newCap;
    srcSlot->cap = cap_null_cap_new();
    destSlot->cteMDBNode = mdb;
    srcSlot->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_ಗಂಟು_get_mdb_Prev(mdb);
    ಯದಿ (prev_ptr)
        mdb_ಗಂಟು_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(destSlot));

    next_ptr = mdb_ಗಂಟು_get_mdb_Next(mdb);
    ಯದಿ (next_ptr)
        mdb_ಗಂಟು_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(destSlot));
}

ಶೂನ್ಯ capSwapForDelete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    ಯದಿ (slot1 == slot2) {
        ಪ್ರತ್ಯಾಗಮ;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cteSwap(cap1, slot1, cap2, slot2);
}

ಶೂನ್ಯ cteSwap(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_ಗಂಟು_t mdb1, mdb2;
    word_t next_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_ಗಂಟು_get_mdb_Prev(mdb1);
    ಯದಿ (prev_ptr)
        mdb_ಗಂಟು_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    next_ptr = mdb_ಗಂಟು_get_mdb_Next(mdb1);
    ಯದಿ (next_ptr)
        mdb_ಗಂಟು_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_ಗಂಟು_get_mdb_Prev(mdb2);
    ಯದಿ (prev_ptr)
        mdb_ಗಂಟು_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    next_ptr = mdb_ಗಂಟು_get_mdb_Next(mdb2);
    ಯದಿ (next_ptr)
        mdb_ಗಂಟು_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *slot)
{
    cte_t *nextPtr;
    exception_t status;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    ಪ್ರತಿ (nextPtr = CTE_PTR(mdb_ಗಂಟು_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_ಗಂಟು_get_mdb_Next(slot->cteMDBNode))) {
        status = cteDelete(nextPtr, true);
        ಯದಿ (status != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ status;
        }

        status = preemptionPoint();
        ಯದಿ (status != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ status;
        }
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *slot, bool_t exposed)
{
    finaliseSlot_ret_t fs_ret;

    fs_ret = finaliseSlot(slot, exposed);
    ಯದಿ (fs_ret.status != EXCEPTION_NONE) {
        ಪ್ರತ್ಯಾಗಮ fs_ret.status;
    }

    ಯದಿ (exposed || fs_ret.success) {
        emptySlot(slot, fs_ret.cleanupInfo);
    }
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ ಶೂನ್ಯ emptySlot(cte_t *slot, cap_t cleanupInfo)
{
    ಯದಿ (cap_get_capType(slot->cap) != cap_null_cap) {
        mdb_ಗಂಟು_t mdb_ಗಂಟು;
        cte_t *prev, *next;

        mdb_ಗಂಟು = slot->cteMDBNode;
        prev = CTE_PTR(mdb_ಗಂಟು_get_mdb_Prev(mdb_ಗಂಟು));
        next = CTE_PTR(mdb_ಗಂಟು_get_mdb_Next(mdb_ಗಂಟು));

        ಯದಿ (prev) {
            mdb_ಗಂಟು_ptr_set_mdb_Next(&prev->cteMDBNode, CTE_REF(next));
        }
        ಯದಿ (next) {
            mdb_ಗಂಟು_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(prev));
        }
        ಯದಿ (next)
            mdb_ಗಂಟು_ptr_set_mdb_First_Badged(&next->cteMDBNode,
                                            mdb_ಗಂಟು_get_mdb_First_Badged(next->cteMDBNode) ||
                                            mdb_ಗಂಟು_get_mdb_First_Badged(mdb_ಗಂಟು));
        slot->cap = cap_null_cap_new();
        slot->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool_t CONST capRemovable(cap_t cap, cte_t *slot)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_null_cap:
        ಪ್ರತ್ಯಾಗಮ true;
    ಪಕ್ಷ cap_zombie_cap: {
        word_t n = cap_zombie_cap_get_cap_Zombie_ಸಂಖ್ಯೆ(cap);
        cte_t *z_slot = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        ಪ್ರತ್ಯಾಗಮ (n == 0 || (n == 1 && slot == z_slot));
    }
    ಸ್ವಭಾವ:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool_t CONST capCyclicZombie(cap_t cap, cte_t *slot)
{
    ಪ್ರತ್ಯಾಗಮ cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == slot;
}

ಸ್ಥಿರ finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t immediate)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t status;
    finaliseSlot_ret_t ret;

    ಯಾವತ್ (cap_get_capType(slot->cap) != cap_null_cap) {
        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, false);

        ಯದಿ (capRemovable(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            ಪ್ರತ್ಯಾಗಮ ret;
        }

        slot->cap = fc_ret.remainder;

        ಯದಿ (!immediate && capCyclicZombie(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            ಪ್ರತ್ಯಾಗಮ ret;
        }

        status = reduceZombie(slot, immediate);
        ಯದಿ (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            ಪ್ರತ್ಯಾಗಮ ret;
        }

        status = preemptionPoint();
        ಯದಿ (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            ಪ್ರತ್ಯಾಗಮ ret;
        }
    }
    ret.status = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ exception_t reduceZombie(cte_t *slot, bool_t immediate)
{
    cte_t *ptr;
    word_t n, type;
    exception_t status;

    ಪ್ರತಿಜ್ಞಾ(cap_get_capType(slot->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);
    n = cap_zombie_cap_get_cap_Zombie_ಸಂಖ್ಯೆ(slot->cap);
    type = cap_zombie_cap_get_capZombieType(slot->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    ಪ್ರತಿಜ್ಞಾ(n > 0);

    ಯದಿ (immediate) {
        cte_t *endSlot = &ptr[n - 1];

        status = cteDelete(endSlot, false);
        ಯದಿ (status != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ status;
        }

        ವಿಕಲ್ಪ (cap_get_capType(slot->cap)) {
        ಪಕ್ಷ cap_null_cap:
            ಭಙ್ಗ;

        ಪಕ್ಷ cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);

            ಯದಿ (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_ಸಂಖ್ಯೆ(slot->cap) == n &&
                cap_zombie_cap_get_capZombieType(slot->cap) == type) {
                ಪ್ರತಿಜ್ಞಾ(cap_get_capType(endSlot->cap) == cap_null_cap);
                slot->cap =
                    cap_zombie_cap_set_cap_Zombie_ಸಂಖ್ಯೆ(slot->cap, n - 1);
            } ಅನ್ಯಥಾ {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                ಪ್ರತಿಜ್ಞಾ(ptr2 == slot && ptr != slot);
            }
            ಭಙ್ಗ;
        }

        ಸ್ವಭಾವ:
            fail("Expected recursion to result in Zombie.");
        }
    } ಅನ್ಯಥಾ {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        ಪ್ರತಿಜ್ಞಾ(ptr != slot);

        ಯದಿ (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            ಪ್ರತಿಜ್ಞಾ(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        capSwapForDelete(ptr, slot);
    }
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಶೂನ್ಯ cteDeleteOne(cte_t *slot)
{
    word_t cap_type = cap_get_capType(slot->cap);
    ಯದಿ (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret UNUSED;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        ಪ್ರತಿಜ್ಞಾ(capRemovable(fc_ret.remainder, slot) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        emptySlot(slot, cap_null_cap_new());
    }
}

ಶೂನ್ಯ ಸೇರಿಸು_New_Cap(cte_t *parent, cte_t *slot, cap_t cap)
{
    cte_t *next;

    next = CTE_PTR(mdb_ಗಂಟು_get_mdb_Next(parent->cteMDBNode));
    slot->cap = cap;
    slot->cteMDBNode = mdb_ಗಂಟು_new(CTE_REF(next), true, true, CTE_REF(parent));
    ಯದಿ (next) {
        mdb_ಗಂಟು_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(slot));
    }
    mdb_ಗಂಟು_ptr_set_mdb_Next(&parent->cteMDBNode, CTE_REF(slot));
}

#ifndef CONFIG_KERNEL_MCS
ಶೂನ್ಯ setupReplyMaster(tcb_t *thread)
{
    cte_t *slot;

    slot = TCB_PTR_CTE_PTR(thread, tcbReply);
    ಯದಿ (cap_get_capType(slot->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        slot->cap = cap_reply_cap_new(true, true, TCB_REF(thread));
        slot->cteMDBNode = nullMDBNode;
        mdb_ಗಂಟು_ptr_set_mdb_Revocable(&slot->cteMDBNode, true);
        mdb_ಗಂಟು_ptr_set_mdb_First_Badged(&slot->cteMDBNode, true);
    }
}
#endif

bool_t PURE isMDBParentOf(cte_t *cte_a, cte_t *cte_b)
{
    ಯದಿ (!mdb_ಗಂಟು_get_mdb_Revocable(cte_a->cteMDBNode)) {
        ಪ್ರತ್ಯಾಗಮ false;
    }
    ಯದಿ (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        ಪ್ರತ್ಯಾಗಮ false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    ಯದಿ (!Arch_isMDBParentOf(cte_a->cap, cte_b->cap,
                            mdb_ಗಂಟು_get_mdb_First_Badged(cte_b->cteMDBNode))) {
        ಪ್ರತ್ಯಾಗಮ false;
    }
    ವಿಕಲ್ಪ (cap_get_capType(cte_a->cap)) {
    ಪಕ್ಷ cap_endpoint_cap: {
        word_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        ಯದಿ (badge == 0) {
            ಪ್ರತ್ಯಾಗಮ true;
        }
        ಪ್ರತ್ಯಾಗಮ (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_ಗಂಟು_get_mdb_First_Badged(cte_b->cteMDBNode);
        ಭಙ್ಗ;
    }

    ಪಕ್ಷ cap_notification_cap: {
        word_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        ಯದಿ (badge == 0) {
            ಪ್ರತ್ಯಾಗಮ true;
        }
        ಪ್ರತ್ಯಾಗಮ (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_ಗಂಟು_get_mdb_First_Badged(cte_b->cteMDBNode);
        ಭಙ್ಗ;
    }

    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ true;
        ಭಙ್ಗ;
    }
}

exception_t ensureNoChildren(cte_t *slot)
{
    ಯದಿ (mdb_ಗಂಟು_get_mdb_Next(slot->cteMDBNode) != 0) {
        cte_t *next;

        next = CTE_PTR(mdb_ಗಂಟು_get_mdb_Next(slot->cteMDBNode));
        ಯದಿ (isMDBParentOf(slot, next)) {
            current_syscall_error.type = seL4_RevokeFirst;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t ensureEmptySlot(cte_t *slot)
{
    ಯದಿ (cap_get_capType(slot->cap) != cap_null_cap) {
        current_syscall_error.type = seL4_DeleteFirst;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

bool_t PURE isFinalCapability(cte_t *cte)
{
    mdb_ಗಂಟು_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    ಯದಿ (mdb_ಗಂಟು_get_mdb_Prev(mdb) == 0) {
        prevIsSameObject = false;
    } ಅನ್ಯಥಾ {
        cte_t *prev;

        prev = CTE_PTR(mdb_ಗಂಟು_get_mdb_Prev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    ಯದಿ (prevIsSameObject) {
        ಪ್ರತ್ಯಾಗಮ false;
    } ಅನ್ಯಥಾ {
        ಯದಿ (mdb_ಗಂಟು_get_mdb_Next(mdb) == 0) {
            ಪ್ರತ್ಯಾಗಮ true;
        } ಅನ್ಯಥಾ {
            cte_t *next;

            next = CTE_PTR(mdb_ಗಂಟು_get_mdb_Next(mdb));
            ಪ್ರತ್ಯಾಗಮ !sameObjectAs(cte->cap, next->cap);
        }
    }
}

bool_t PURE slotCapLongRunningDelete(cte_t *slot)
{
    ಯದಿ (cap_get_capType(slot->cap) == cap_null_cap) {
        ಪ್ರತ್ಯಾಗಮ false;
    } ಅನ್ಯಥಾ ಯದಿ (! isFinalCapability(slot)) {
        ಪ್ರತ್ಯಾಗಮ false;
    }
    ವಿಕಲ್ಪ (cap_get_capType(slot->cap)) {
    ಪಕ್ಷ cap_thread_cap:
    ಪಕ್ಷ cap_zombie_cap:
    ಪಕ್ಷ cap_cnode_cap:
        ಪ್ರತ್ಯಾಗಮ true;
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ false;
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

    ಯದಿ (!buffer) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ct = loadCapTransfer(buffer);
    cptr = ct.ct_Receive_ಬೇರು;

    luc_ret = lookupCap(thread, cptr);
    ಯದಿ (luc_ret.status != EXCEPTION_NONE) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    cnode = luc_ret.cap;

    lus_ret = lookupTargetSlot(cnode, ct.ctReceiveIndex, ct.ctReceiveDepth);
    ಯದಿ (lus_ret.status != EXCEPTION_NONE) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    slot = lus_ret.slot;

    ಯದಿ (cap_get_capType(slot->cap) != cap_null_cap) {
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ slot;
}

cap_transfer_t PURE loadCapTransfer(word_t *buffer)
{
    ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ offset = se_L4_ಸಂದೇಶ_Max_Length + se_L4_ಸಂದೇಶ_Max_Extra_Caps + 2;
    ಪ್ರತ್ಯಾಗಮ cap_Transfer_From_ಪದಗಳು(buffer + offset);
}
