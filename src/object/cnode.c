/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <api/invocation.h>
#ଅନ୍ତର୍ଭାଵ <api/syscall.h>
#ଅନ୍ତର୍ଭାଵ <api/types.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <object/structures.h>
#ଅନ୍ତର୍ଭାଵ <object/objecttype.h>
#ଅନ୍ତର୍ଭାଵ <object/cnode.h>
#ଅନ୍ତର୍ଭାଵ <object/interrupt.h>
#ଅନ୍ତର୍ଭାଵ <object/untyped.h>
#ଅନ୍ତର୍ଭାଵ <kernel/cspace.h>
#ଅନ୍ତର୍ଭାଵ <kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <model/preemption.h>
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <util.h>

ସଂରଚନା finaliseSlot_ret {
    exception_t status;
    bool_t success;
    cap_t cleanupInfo;
};
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା finaliseSlot_ret finaliseSlot_ret_t;

ସ୍ଥିର finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t exposed);
ସ୍ଥିର ଶୂନ୍ୟ emptySlot(cte_t *slot, cap_t cleanupInfo);
ସ୍ଥିର exception_t reduceZombie(cte_t *slot, bool_t exposed);

#ifdef CONFIG_KERNEL_MCS
#define CNODE_LAST_INVOCATION CNodeRotate
#ଅନ୍ୟଥା
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
    ପ୍ରତିଜ୍ଞା(cap_get_capType(cap) == cap_cnode_cap);

    ଯଦି (invLabel < CNodeRevoke || invLabel > CNODE_LAST_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଯଦି (length < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    w_bits = getSyscallArg(1, buffer);

    lu_ret = lookupTargetSlot(cap, index, w_bits);
    ଯଦି (lu_ret.status != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        ଫେରନ୍ତୁ lu_ret.status;
    }
    destSlot = lu_ret.slot;

    ଯଦି (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *srcSlot;
        word_t srcIndex, srcDepth, capData;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_ମୂଳ, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        ଯଦି (length < 4 || current_extra_caps.excaprefs[0] == ଖାଲି) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        srcIndex = getSyscallArg(2, buffer);
        srcDepth = getSyscallArg(3, buffer);

        src_ମୂଳ = current_extra_caps.excaprefs[0]->cap;

        status = ensureEmptySlot(destSlot);
        ଯଦି (status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            ଫେରନ୍ତୁ status;
        }

        lu_ret = lookupSourceSlot(src_ମୂଳ, srcIndex, srcDepth);
        ଯଦି (lu_ret.status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            ଫେରନ୍ତୁ lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        ଯଦି (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(srcDepth);
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        ବାଛ (invLabel) {
        ସ୍ଥିତି CNodeCopy:

            ଯଦି (length < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = seL4_TruncatedMessage;
                ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot, srcCap);
            ଯଦି (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                ଫେରନ୍ତୁ dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            ଭାଙ୍ଗ;

        ସ୍ଥିତି CNodeMint:
            ଯଦି (length < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            capData = getSyscallArg(5, buffer);
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot,
                               updateCapData(false, capData, srcCap));
            ଯଦି (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                ଫେରନ୍ତୁ dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            ଭାଙ୍ଗ;

        ସ୍ଥିତି CNodeMove:
            newCap = srcSlot->cap;
            isMove = true;

            ଭାଙ୍ଗ;

        ସ୍ଥିତି CNodeMutate:
            ଯଦି (length < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
            }

            capData = getSyscallArg(4, buffer);
            newCap = updateCapData(true, capData, srcSlot->cap);
            isMove = true;

            ଭାଙ୍ଗ;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            ପ୍ରତିଜ୍ଞା(0);
            ଫେରନ୍ତୁ EXCEPTION_NONE;
        }

        ଯଦି (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଯଦି (isMove) {
            ଫେରନ୍ତୁ invokeCNodeMove(newCap, srcSlot, destSlot);
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ invoke_CNode_ଭର୍ତ୍ତି(newCap, srcSlot, destSlot);
        }
    }

    ଯଦି (invLabel == CNodeRevoke) {
        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ invokeCNodeRevoke(destSlot);
    }

    ଯଦି (invLabel == CNodeDelete) {
        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ invokeCNodeDelete(destSlot);
    }

#ifndef CONFIG_KERNEL_MCS
    ଯଦି (invLabel == CNodeSaveCaller) {
        status = ensureEmptySlot(destSlot);
        ଯଦି (status != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            ଫେରନ୍ତୁ status;
        }

        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ invokeCNodeSaveCaller(destSlot);
    }
#endif

    ଯଦି (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = destSlot->cap;

        ଯଦି (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ invokeCNodeCancelBadgedSends(destCap);
    }

    ଯଦି (invLabel == CNodeRotate) {
        word_t pivotNewData, pivotIndex, pivotDepth;
        word_t srcNewData, srcIndex, srcDepth;
        cte_t *pivotSlot, *srcSlot;
        cap_t pivot_ମୂଳ, src_ମୂଳ, newSrcCap, newPivotCap;

        ଯଦି (length < 8 || current_extra_caps.excaprefs[0] == ଖାଲି
            || current_extra_caps.excaprefs[1] == ଖାଲି) {
            current_syscall_error.type = seL4_TruncatedMessage;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        pivotNewData = getSyscallArg(2, buffer);
        pivotIndex   = getSyscallArg(3, buffer);
        pivotDepth   = getSyscallArg(4, buffer);
        srcNewData   = getSyscallArg(5, buffer);
        srcIndex     = getSyscallArg(6, buffer);
        srcDepth     = getSyscallArg(7, buffer);

        pivot_ମୂଳ = current_extra_caps.excaprefs[0]->cap;
        src_ମୂଳ   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookupSourceSlot(src_ମୂଳ, srcIndex, srcDepth);
        ଯଦି (lu_ret.status != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        lu_ret = lookupPivotSlot(pivot_ମୂଳ, pivotIndex, pivotDepth);
        ଯଦି (lu_ret.status != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ lu_ret.status;
        }
        pivotSlot = lu_ret.slot;

        ଯଦି (pivotSlot == srcSlot || pivotSlot == destSlot) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        ଯଦି (srcSlot != destSlot) {
            status = ensureEmptySlot(destSlot);
            ଯଦି (status != EXCEPTION_NONE) {
                ଫେରନ୍ତୁ status;
            }
        }

        ଯଦି (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(srcDepth);
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        ଯଦି (cap_get_capType(pivotSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(pivotDepth);
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = updateCapData(true, srcNewData, srcSlot->cap);
        newPivotCap = updateCapData(true, pivotNewData, pivotSlot->cap);

        ଯଦି (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        ଯଦି (cap_get_capType(newPivotCap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ invokeCNodeRotate(newSrcCap, newPivotCap,
                                 srcSlot, pivotSlot, destSlot);
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *destSlot)
{
    ଫେରନ୍ତୁ cteRevoke(destSlot);
}

exception_t invokeCNodeDelete(cte_t *destSlot)
{
    ଫେରନ୍ତୁ cteDelete(destSlot, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    word_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    ଯଦି (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t invoke_CNode_ଭର୍ତ୍ତି(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cte_ଭର୍ତ୍ତି(cap, srcSlot, destSlot);

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cteMove(cap, srcSlot, destSlot);

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    ଯଦି (slot1 == slot3) {
        cteSwap(cap1, slot1, cap2, slot2);
    } ଅନ୍ୟଥା {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

#ifndef CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *destSlot)
{
    cap_t cap;
    cte_t *srcSlot;

    srcSlot = TCB_PTR_CTE_PTR(ଗଣ୍ଠି_STATE(ksCurThread), tcbCaller);
    cap = srcSlot->cap;

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_reply_cap:
        ଯଦି (!cap_reply_cap_get_capReplyMaster(cap)) {
            cteMove(cap, srcSlot, destSlot);
        }
        ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("caller capability must be null or reply");
        ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}
#endif

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
ସ୍ଥିର ଶୂନ୍ୟ setUntypedCapAsFull(cap_t srcCap, cap_t newCap, cte_t *srcSlot)
{
    ଯଦି ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        ଯଦି ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_capBlockSize(newCap)
                == cap_untyped_cap_get_capBlockSize(srcCap))) {
            cap_untyped_cap_ptr_set_capFreeIndex(&(srcSlot->cap),
                                                 MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(srcCap)));
        }
    }
}

ଶୂନ୍ୟ cte_ଭର୍ତ୍ତି(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_ଗଣ୍ଠି_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = srcSlot->cteMDBNode;
    srcCap = srcSlot->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_ଗଣ୍ଠି_set_mdb_Prev(srcMDB, CTE_REF(srcSlot));
    newMDB = mdb_ଗଣ୍ଠି_set_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_ଗଣ୍ଠି_set_mdb_First_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    ପ୍ରତିଜ୍ଞା(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    ପ୍ରତିଜ୍ଞା((cte_t *)mdb_ଗଣ୍ଠି_get_mdb_Next(destSlot->cteMDBNode) == ଖାଲି &&
           (cte_t *)mdb_ଗଣ୍ଠି_get_mdb_Prev(destSlot->cteMDBNode) == ଖାଲି);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    setUntypedCapAsFull(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSlot->cteMDBNode = newMDB;
    mdb_ଗଣ୍ଠି_ptr_set_mdb_Next(&srcSlot->cteMDBNode, CTE_REF(destSlot));
    ଯଦି (mdb_ଗଣ୍ଠି_get_mdb_Next(newMDB)) {
        mdb_ଗଣ୍ଠି_ptr_set_mdb_Prev(
            &CTE_PTR(mdb_ଗଣ୍ଠି_get_mdb_Next(newMDB))->cteMDBNode,
            CTE_REF(destSlot));
    }
}

ଶୂନ୍ୟ cteMove(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_ଗଣ୍ଠି_t mdb;
    word_t prev_ptr, next_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    ପ୍ରତିଜ୍ଞା(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    ପ୍ରତିଜ୍ଞା((cte_t *)mdb_ଗଣ୍ଠି_get_mdb_Next(destSlot->cteMDBNode) == ଖାଲି &&
           (cte_t *)mdb_ଗଣ୍ଠି_get_mdb_Prev(destSlot->cteMDBNode) == ଖାଲି);

    mdb = srcSlot->cteMDBNode;
    destSlot->cap = newCap;
    srcSlot->cap = cap_null_cap_new();
    destSlot->cteMDBNode = mdb;
    srcSlot->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_ଗଣ୍ଠି_get_mdb_Prev(mdb);
    ଯଦି (prev_ptr)
        mdb_ଗଣ୍ଠି_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(destSlot));

    next_ptr = mdb_ଗଣ୍ଠି_get_mdb_Next(mdb);
    ଯଦି (next_ptr)
        mdb_ଗଣ୍ଠି_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(destSlot));
}

ଶୂନ୍ୟ capSwapForDelete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    ଯଦି (slot1 == slot2) {
        ଫେରନ୍ତୁ;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cteSwap(cap1, slot1, cap2, slot2);
}

ଶୂନ୍ୟ cteSwap(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_ଗଣ୍ଠି_t mdb1, mdb2;
    word_t next_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_ଗଣ୍ଠି_get_mdb_Prev(mdb1);
    ଯଦି (prev_ptr)
        mdb_ଗଣ୍ଠି_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    next_ptr = mdb_ଗଣ୍ଠି_get_mdb_Next(mdb1);
    ଯଦି (next_ptr)
        mdb_ଗଣ୍ଠି_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_ଗଣ୍ଠି_get_mdb_Prev(mdb2);
    ଯଦି (prev_ptr)
        mdb_ଗଣ୍ଠି_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    next_ptr = mdb_ଗଣ୍ଠି_get_mdb_Next(mdb2);
    ଯଦି (next_ptr)
        mdb_ଗଣ୍ଠି_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *slot)
{
    cte_t *nextPtr;
    exception_t status;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    ପାଇଁ (nextPtr = CTE_PTR(mdb_ଗଣ୍ଠି_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_ଗଣ୍ଠି_get_mdb_Next(slot->cteMDBNode))) {
        status = cteDelete(nextPtr, true);
        ଯଦି (status != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ status;
        }

        status = preemptionPoint();
        ଯଦି (status != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ status;
        }
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *slot, bool_t exposed)
{
    finaliseSlot_ret_t fs_ret;

    fs_ret = finaliseSlot(slot, exposed);
    ଯଦି (fs_ret.status != EXCEPTION_NONE) {
        ଫେରନ୍ତୁ fs_ret.status;
    }

    ଯଦି (exposed || fs_ret.success) {
        emptySlot(slot, fs_ret.cleanupInfo);
    }
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର ଶୂନ୍ୟ emptySlot(cte_t *slot, cap_t cleanupInfo)
{
    ଯଦି (cap_get_capType(slot->cap) != cap_null_cap) {
        mdb_ଗଣ୍ଠି_t mdb_ଗଣ୍ଠି;
        cte_t *prev, *next;

        mdb_ଗଣ୍ଠି = slot->cteMDBNode;
        prev = CTE_PTR(mdb_ଗଣ୍ଠି_get_mdb_Prev(mdb_ଗଣ୍ଠି));
        next = CTE_PTR(mdb_ଗଣ୍ଠି_get_mdb_Next(mdb_ଗଣ୍ଠି));

        ଯଦି (prev) {
            mdb_ଗଣ୍ଠି_ptr_set_mdb_Next(&prev->cteMDBNode, CTE_REF(next));
        }
        ଯଦି (next) {
            mdb_ଗଣ୍ଠି_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(prev));
        }
        ଯଦି (next)
            mdb_ଗଣ୍ଠି_ptr_set_mdb_First_Badged(&next->cteMDBNode,
                                            mdb_ଗଣ୍ଠି_get_mdb_First_Badged(next->cteMDBNode) ||
                                            mdb_ଗଣ୍ଠି_get_mdb_First_Badged(mdb_ଗଣ୍ଠି));
        slot->cap = cap_null_cap_new();
        slot->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool_t CONST capRemovable(cap_t cap, cte_t *slot)
{
    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_null_cap:
        ଫେରନ୍ତୁ true;
    ସ୍ଥିତି cap_zombie_cap: {
        word_t n = cap_zombie_cap_get_cap_Zombie_ସଂଖ୍ୟା(cap);
        cte_t *z_slot = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        ଫେରନ୍ତୁ (n == 0 || (n == 1 && slot == z_slot));
    }
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool_t CONST capCyclicZombie(cap_t cap, cte_t *slot)
{
    ଫେରନ୍ତୁ cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == slot;
}

ସ୍ଥିର finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t immediate)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t status;
    finaliseSlot_ret_t ret;

    ଯେତେବେଳେ (cap_get_capType(slot->cap) != cap_null_cap) {
        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, false);

        ଯଦି (capRemovable(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            ଫେରନ୍ତୁ ret;
        }

        slot->cap = fc_ret.remainder;

        ଯଦି (!immediate && capCyclicZombie(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            ଫେରନ୍ତୁ ret;
        }

        status = reduceZombie(slot, immediate);
        ଯଦି (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            ଫେରନ୍ତୁ ret;
        }

        status = preemptionPoint();
        ଯଦି (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            ଫେରନ୍ତୁ ret;
        }
    }
    ret.status = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର exception_t reduceZombie(cte_t *slot, bool_t immediate)
{
    cte_t *ptr;
    word_t n, type;
    exception_t status;

    ପ୍ରତିଜ୍ଞା(cap_get_capType(slot->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);
    n = cap_zombie_cap_get_cap_Zombie_ସଂଖ୍ୟା(slot->cap);
    type = cap_zombie_cap_get_capZombieType(slot->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    ପ୍ରତିଜ୍ଞା(n > 0);

    ଯଦି (immediate) {
        cte_t *endSlot = &ptr[n - 1];

        status = cteDelete(endSlot, false);
        ଯଦି (status != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ status;
        }

        ବାଛ (cap_get_capType(slot->cap)) {
        ସ୍ଥିତି cap_null_cap:
            ଭାଙ୍ଗ;

        ସ୍ଥିତି cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);

            ଯଦି (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_ସଂଖ୍ୟା(slot->cap) == n &&
                cap_zombie_cap_get_capZombieType(slot->cap) == type) {
                ପ୍ରତିଜ୍ଞା(cap_get_capType(endSlot->cap) == cap_null_cap);
                slot->cap =
                    cap_zombie_cap_set_cap_Zombie_ସଂଖ୍ୟା(slot->cap, n - 1);
            } ଅନ୍ୟଥା {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                ପ୍ରତିଜ୍ଞା(ptr2 == slot && ptr != slot);
            }
            ଭାଙ୍ଗ;
        }

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            fail("Expected recursion to result in Zombie.");
        }
    } ଅନ୍ୟଥା {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        ପ୍ରତିଜ୍ଞା(ptr != slot);

        ଯଦି (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            ପ୍ରତିଜ୍ଞା(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        capSwapForDelete(ptr, slot);
    }
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ଶୂନ୍ୟ cteDeleteOne(cte_t *slot)
{
    word_t cap_type = cap_get_capType(slot->cap);
    ଯଦି (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret UNUSED;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        ପ୍ରତିଜ୍ଞା(capRemovable(fc_ret.remainder, slot) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        emptySlot(slot, cap_null_cap_new());
    }
}

ଶୂନ୍ୟ ଭର୍ତ୍ତି_New_Cap(cte_t *parent, cte_t *slot, cap_t cap)
{
    cte_t *next;

    next = CTE_PTR(mdb_ଗଣ୍ଠି_get_mdb_Next(parent->cteMDBNode));
    slot->cap = cap;
    slot->cteMDBNode = mdb_ଗଣ୍ଠି_new(CTE_REF(next), true, true, CTE_REF(parent));
    ଯଦି (next) {
        mdb_ଗଣ୍ଠି_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(slot));
    }
    mdb_ଗଣ୍ଠି_ptr_set_mdb_Next(&parent->cteMDBNode, CTE_REF(slot));
}

#ifndef CONFIG_KERNEL_MCS
ଶୂନ୍ୟ setupReplyMaster(tcb_t *thread)
{
    cte_t *slot;

    slot = TCB_PTR_CTE_PTR(thread, tcbReply);
    ଯଦି (cap_get_capType(slot->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        slot->cap = cap_reply_cap_new(true, true, TCB_REF(thread));
        slot->cteMDBNode = nullMDBNode;
        mdb_ଗଣ୍ଠି_ptr_set_mdb_Revocable(&slot->cteMDBNode, true);
        mdb_ଗଣ୍ଠି_ptr_set_mdb_First_Badged(&slot->cteMDBNode, true);
    }
}
#endif

bool_t PURE isMDBParentOf(cte_t *cte_a, cte_t *cte_b)
{
    ଯଦି (!mdb_ଗଣ୍ଠି_get_mdb_Revocable(cte_a->cteMDBNode)) {
        ଫେରନ୍ତୁ false;
    }
    ଯଦି (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        ଫେରନ୍ତୁ false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    ଯଦି (!Arch_isMDBParentOf(cte_a->cap, cte_b->cap,
                            mdb_ଗଣ୍ଠି_get_mdb_First_Badged(cte_b->cteMDBNode))) {
        ଫେରନ୍ତୁ false;
    }
    ବାଛ (cap_get_capType(cte_a->cap)) {
    ସ୍ଥିତି cap_endpoint_cap: {
        word_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        ଯଦି (badge == 0) {
            ଫେରନ୍ତୁ true;
        }
        ଫେରନ୍ତୁ (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_ଗଣ୍ଠି_get_mdb_First_Badged(cte_b->cteMDBNode);
        ଭାଙ୍ଗ;
    }

    ସ୍ଥିତି cap_notification_cap: {
        word_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        ଯଦି (badge == 0) {
            ଫେରନ୍ତୁ true;
        }
        ଫେରନ୍ତୁ (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_ଗଣ୍ଠି_get_mdb_First_Badged(cte_b->cteMDBNode);
        ଭାଙ୍ଗ;
    }

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ true;
        ଭାଙ୍ଗ;
    }
}

exception_t ensureNoChildren(cte_t *slot)
{
    ଯଦି (mdb_ଗଣ୍ଠି_get_mdb_Next(slot->cteMDBNode) != 0) {
        cte_t *next;

        next = CTE_PTR(mdb_ଗଣ୍ଠି_get_mdb_Next(slot->cteMDBNode));
        ଯଦି (isMDBParentOf(slot, next)) {
            current_syscall_error.type = seL4_RevokeFirst;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t ensureEmptySlot(cte_t *slot)
{
    ଯଦି (cap_get_capType(slot->cap) != cap_null_cap) {
        current_syscall_error.type = seL4_DeleteFirst;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

bool_t PURE isFinalCapability(cte_t *cte)
{
    mdb_ଗଣ୍ଠି_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    ଯଦି (mdb_ଗଣ୍ଠି_get_mdb_Prev(mdb) == 0) {
        prevIsSameObject = false;
    } ଅନ୍ୟଥା {
        cte_t *prev;

        prev = CTE_PTR(mdb_ଗଣ୍ଠି_get_mdb_Prev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    ଯଦି (prevIsSameObject) {
        ଫେରନ୍ତୁ false;
    } ଅନ୍ୟଥା {
        ଯଦି (mdb_ଗଣ୍ଠି_get_mdb_Next(mdb) == 0) {
            ଫେରନ୍ତୁ true;
        } ଅନ୍ୟଥା {
            cte_t *next;

            next = CTE_PTR(mdb_ଗଣ୍ଠି_get_mdb_Next(mdb));
            ଫେରନ୍ତୁ !sameObjectAs(cte->cap, next->cap);
        }
    }
}

bool_t PURE slotCapLongRunningDelete(cte_t *slot)
{
    ଯଦି (cap_get_capType(slot->cap) == cap_null_cap) {
        ଫେରନ୍ତୁ false;
    } ଅନ୍ୟଥା ଯଦି (! isFinalCapability(slot)) {
        ଫେରନ୍ତୁ false;
    }
    ବାଛ (cap_get_capType(slot->cap)) {
    ସ୍ଥିତି cap_thread_cap:
    ସ୍ଥିତି cap_zombie_cap:
    ସ୍ଥିତି cap_cnode_cap:
        ଫେରନ୍ତୁ true;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ false;
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

    ଯଦି (!buffer) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ct = loadCapTransfer(buffer);
    cptr = ct.ct_Receive_ମୂଳ;

    luc_ret = lookupCap(thread, cptr);
    ଯଦି (luc_ret.status != EXCEPTION_NONE) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    cnode = luc_ret.cap;

    lus_ret = lookupTargetSlot(cnode, ct.ctReceiveIndex, ct.ctReceiveDepth);
    ଯଦି (lus_ret.status != EXCEPTION_NONE) {
        ଫେରନ୍ତୁ ଖାଲି;
    }
    slot = lus_ret.slot;

    ଯଦି (cap_get_capType(slot->cap) != cap_null_cap) {
        ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ slot;
}

cap_transfer_t PURE loadCapTransfer(word_t *buffer)
{
    ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset = se_L4_ସନ୍ଦେଶ_Max_Length + se_L4_ସନ୍ଦେଶ_Max_Extra_Caps + 2;
    ଫେରନ୍ତୁ cap_Transfer_From_ଶବ୍ଦଗୁଡ଼ିକ(buffer + offset);
}
