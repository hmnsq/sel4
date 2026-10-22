/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <types.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <api/invocation.h>
#অন্তর্ভাব <api/syscall.h>
#অন্তর্ভাব <api/types.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <object/structures.h>
#অন্তর্ভাব <object/objecttype.h>
#অন্তর্ভাব <object/cnode.h>
#অন্তর্ভাব <object/interrupt.h>
#অন্তর্ভাব <object/untyped.h>
#অন্তর্ভাব <kernel/cspace.h>
#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <model/preemption.h>
#অন্তর্ভাব <model/statedata.h>
#অন্তর্ভাব <util.h>

কাঠামো finaliseSlot_ret {
    exception_t status;
    bool_t success;
    cap_t cleanupInfo;
};
প্রকারলেখ কাঠামো finaliseSlot_ret finaliseSlot_ret_t;

স্থির finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t exposed);
স্থির শূন্য emptySlot(cte_t *slot, cap_t cleanupInfo);
স্থির exception_t reduceZombie(cte_t *slot, bool_t exposed);

#ifdef CONFIG_KERNEL_MCS
#define CNODE_LAST_INVOCATION CNodeRotate
#অন্যথা
#define CNODE_LAST_INVOCATION CNodeSaveCaller
#endif

exception_t decodeCNodeInvocation(word_t invLabel, word_t দৈর্ঘ্য, cap_t cap,
                                  word_t *বাফার)
{
    lookupSlot_ret_t lu_ret;
    cte_t *destSlot;
    word_t সূচক, w_bits;
    exception_t status;

    /* Haskell error: "decodeCNodeInvocation: invalid cap" */
    প্রতিজ্ঞা(cap_get_capType(cap) == cap_cnode_cap);

    যদি (invLabel < CNodeRevoke || invLabel > CNODE_LAST_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (দৈর্ঘ্য < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    সূচক = getSyscallArg(0, বাফার);
    w_bits = getSyscallArg(1, বাফার);

    lu_ret = lookupTargetSlot(cap, সূচক, w_bits);
    যদি (lu_ret.status != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        ফেরত lu_ret.status;
    }
    destSlot = lu_ret.slot;

    যদি (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *srcSlot;
        word_t src_সূচক, srcDepth, cap_তথ্য;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_মূল, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        যদি (দৈর্ঘ্য < 4 || current_extra_caps.excaprefs[0] == ফাঁকা) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
        src_সূচক = getSyscallArg(2, বাফার);
        srcDepth = getSyscallArg(3, বাফার);

        src_মূল = current_extra_caps.excaprefs[0]->cap;

        status = ensureEmptySlot(destSlot);
        যদি (status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            ফেরত status;
        }

        lu_ret = lookupSourceSlot(src_মূল, src_সূচক, srcDepth);
        যদি (lu_ret.status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            ফেরত lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        যদি (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(srcDepth);
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        নির্বাচন (invLabel) {
        ক্ষেত্র CNodeCopy:

            যদি (দৈর্ঘ্য < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = seL4_TruncatedMessage;
                ফেরত EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, বাফার));
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot, srcCap);
            যদি (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                ফেরত dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            বিরতি;

        ক্ষেত্র CNodeMint:
            যদি (দৈর্ঘ্য < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                ফেরত EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, বাফার));
            cap_তথ্য = getSyscallArg(5, বাফার);
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot,
                               update_Cap_তথ্য(false, cap_তথ্য, srcCap));
            যদি (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                ফেরত dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            বিরতি;

        ক্ষেত্র CNodeMove:
            newCap = srcSlot->cap;
            isMove = true;

            বিরতি;

        ক্ষেত্র CNodeMutate:
            যদি (দৈর্ঘ্য < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                ফেরত EXCEPTION_SYSCALL_ERROR;
            }

            cap_তথ্য = getSyscallArg(4, বাফার);
            newCap = update_Cap_তথ্য(true, cap_তথ্য, srcSlot->cap);
            isMove = true;

            বিরতি;

        পূর্বনির্ধারিত:
            প্রতিজ্ঞা(0);
            ফেরত EXCEPTION_NONE;
        }

        যদি (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        যদি (isMove) {
            ফেরত invokeCNodeMove(newCap, srcSlot, destSlot);
        } অন্যথা {
            ফেরত invoke_CNode_ঢোকাও(newCap, srcSlot, destSlot);
        }
    }

    যদি (invLabel == CNodeRevoke) {
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত invokeCNodeRevoke(destSlot);
    }

    যদি (invLabel == CNodeDelete) {
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত invokeCNodeDelete(destSlot);
    }

#ifndef CONFIG_KERNEL_MCS
    যদি (invLabel == CNodeSaveCaller) {
        status = ensureEmptySlot(destSlot);
        যদি (status != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            ফেরত status;
        }

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত invokeCNodeSaveCaller(destSlot);
    }
#endif

    যদি (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = destSlot->cap;

        যদি (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত invokeCNodeCancelBadgedSends(destCap);
    }

    যদি (invLabel == CNodeRotate) {
        word_t pivot_New_তথ্য, pivot_সূচক, pivotDepth;
        word_t src_New_তথ্য, src_সূচক, srcDepth;
        cte_t *pivotSlot, *srcSlot;
        cap_t pivot_মূল, src_মূল, newSrcCap, newPivotCap;

        যদি (দৈর্ঘ্য < 8 || current_extra_caps.excaprefs[0] == ফাঁকা
            || current_extra_caps.excaprefs[1] == ফাঁকা) {
            current_syscall_error.type = seL4_TruncatedMessage;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
        pivot_New_তথ্য = getSyscallArg(2, বাফার);
        pivot_সূচক   = getSyscallArg(3, বাফার);
        pivotDepth   = getSyscallArg(4, বাফার);
        src_New_তথ্য   = getSyscallArg(5, বাফার);
        src_সূচক     = getSyscallArg(6, বাফার);
        srcDepth     = getSyscallArg(7, বাফার);

        pivot_মূল = current_extra_caps.excaprefs[0]->cap;
        src_মূল   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookupSourceSlot(src_মূল, src_সূচক, srcDepth);
        যদি (lu_ret.status != EXCEPTION_NONE) {
            ফেরত lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        lu_ret = lookupPivotSlot(pivot_মূল, pivot_সূচক, pivotDepth);
        যদি (lu_ret.status != EXCEPTION_NONE) {
            ফেরত lu_ret.status;
        }
        pivotSlot = lu_ret.slot;

        যদি (pivotSlot == srcSlot || pivotSlot == destSlot) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        যদি (srcSlot != destSlot) {
            status = ensureEmptySlot(destSlot);
            যদি (status != EXCEPTION_NONE) {
                ফেরত status;
            }
        }

        যদি (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(srcDepth);
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        যদি (cap_get_capType(pivotSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(pivotDepth);
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = update_Cap_তথ্য(true, src_New_তথ্য, srcSlot->cap);
        newPivotCap = update_Cap_তথ্য(true, pivot_New_তথ্য, pivotSlot->cap);

        যদি (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        যদি (cap_get_capType(newPivotCap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত invokeCNodeRotate(newSrcCap, newPivotCap,
                                 srcSlot, pivotSlot, destSlot);
    }

    ফেরত EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *destSlot)
{
    ফেরত cteRevoke(destSlot);
}

exception_t invokeCNodeDelete(cte_t *destSlot)
{
    ফেরত cteDelete(destSlot, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    word_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    যদি (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    ফেরত EXCEPTION_NONE;
}

exception_t invoke_CNode_ঢোকাও(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cte_ঢোকাও(cap, srcSlot, destSlot);

    ফেরত EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cteMove(cap, srcSlot, destSlot);

    ফেরত EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    যদি (slot1 == slot3) {
        cteSwap(cap1, slot1, cap2, slot2);
    } অন্যথা {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    ফেরত EXCEPTION_NONE;
}

#ifndef CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *destSlot)
{
    cap_t cap;
    cte_t *srcSlot;

    srcSlot = TCB_PTR_CTE_PTR(গ্রন্থি_STATE(ksCurThread), tcbCaller);
    cap = srcSlot->cap;

    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        বিরতি;

    ক্ষেত্র cap_reply_cap:
        যদি (!cap_reply_cap_get_capReplyMaster(cap)) {
            cteMove(cap, srcSlot, destSlot);
        }
        বিরতি;

    পূর্বনির্ধারিত:
        fail("caller capability must be null or reply");
        বিরতি;
    }

    ফেরত EXCEPTION_NONE;
}
#endif

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
স্থির শূন্য setUntypedCapAsFull(cap_t srcCap, cap_t newCap, cte_t *srcSlot)
{
    যদি ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        যদি ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_cap_Block_আকার(newCap)
                == cap_untyped_cap_get_cap_Block_আকার(srcCap))) {
            cap_untyped_cap_ptr_set_cap_Free_সূচক(&(srcSlot->cap),
                                                 MAX_FREE_সূচক(cap_untyped_cap_get_cap_Block_আকার(srcCap)));
        }
    }
}

শূন্য cte_ঢোকাও(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_গ্রন্থি_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = srcSlot->cteMDBNode;
    srcCap = srcSlot->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_গ্রন্থি_set_mdb_পূর্ব(srcMDB, CTE_REF(srcSlot));
    newMDB = mdb_গ্রন্থি_set_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_গ্রন্থি_set_mdb_First_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    প্রতিজ্ঞা(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    প্রতিজ্ঞা((cte_t *)mdb_গ্রন্থি_get_mdb_পরবর্তী(destSlot->cteMDBNode) == ফাঁকা &&
           (cte_t *)mdb_গ্রন্থি_get_mdb_পূর্ব(destSlot->cteMDBNode) == ফাঁকা);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    setUntypedCapAsFull(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSlot->cteMDBNode = newMDB;
    mdb_গ্রন্থি_ptr_set_mdb_পরবর্তী(&srcSlot->cteMDBNode, CTE_REF(destSlot));
    যদি (mdb_গ্রন্থি_get_mdb_পরবর্তী(newMDB)) {
        mdb_গ্রন্থি_ptr_set_mdb_পূর্ব(
            &CTE_PTR(mdb_গ্রন্থি_get_mdb_পরবর্তী(newMDB))->cteMDBNode,
            CTE_REF(destSlot));
    }
}

শূন্য cteMove(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_গ্রন্থি_t mdb;
    word_t পূর্ব_ptr, পরবর্তী_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    প্রতিজ্ঞা(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    প্রতিজ্ঞা((cte_t *)mdb_গ্রন্থি_get_mdb_পরবর্তী(destSlot->cteMDBNode) == ফাঁকা &&
           (cte_t *)mdb_গ্রন্থি_get_mdb_পূর্ব(destSlot->cteMDBNode) == ফাঁকা);

    mdb = srcSlot->cteMDBNode;
    destSlot->cap = newCap;
    srcSlot->cap = cap_null_cap_new();
    destSlot->cteMDBNode = mdb;
    srcSlot->cteMDBNode = nullMDBNode;

    পূর্ব_ptr = mdb_গ্রন্থি_get_mdb_পূর্ব(mdb);
    যদি (পূর্ব_ptr)
        mdb_গ্রন্থি_ptr_set_mdb_পরবর্তী(
            &CTE_PTR(পূর্ব_ptr)->cteMDBNode,
            CTE_REF(destSlot));

    পরবর্তী_ptr = mdb_গ্রন্থি_get_mdb_পরবর্তী(mdb);
    যদি (পরবর্তী_ptr)
        mdb_গ্রন্থি_ptr_set_mdb_পূর্ব(
            &CTE_PTR(পরবর্তী_ptr)->cteMDBNode,
            CTE_REF(destSlot));
}

শূন্য capSwapForDelete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    যদি (slot1 == slot2) {
        ফেরত;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cteSwap(cap1, slot1, cap2, slot2);
}

শূন্য cteSwap(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_গ্রন্থি_t mdb1, mdb2;
    word_t পরবর্তী_ptr, পূর্ব_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    পূর্ব_ptr = mdb_গ্রন্থি_get_mdb_পূর্ব(mdb1);
    যদি (পূর্ব_ptr)
        mdb_গ্রন্থি_ptr_set_mdb_পরবর্তী(
            &CTE_PTR(পূর্ব_ptr)->cteMDBNode,
            CTE_REF(slot2));

    পরবর্তী_ptr = mdb_গ্রন্থি_get_mdb_পরবর্তী(mdb1);
    যদি (পরবর্তী_ptr)
        mdb_গ্রন্থি_ptr_set_mdb_পূর্ব(
            &CTE_PTR(পরবর্তী_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    পূর্ব_ptr = mdb_গ্রন্থি_get_mdb_পূর্ব(mdb2);
    যদি (পূর্ব_ptr)
        mdb_গ্রন্থি_ptr_set_mdb_পরবর্তী(
            &CTE_PTR(পূর্ব_ptr)->cteMDBNode,
            CTE_REF(slot1));

    পরবর্তী_ptr = mdb_গ্রন্থি_get_mdb_পরবর্তী(mdb2);
    যদি (পরবর্তী_ptr)
        mdb_গ্রন্থি_ptr_set_mdb_পূর্ব(
            &CTE_PTR(পরবর্তী_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *slot)
{
    cte_t *পরবর্তী_Ptr;
    exception_t status;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    জন্য (পরবর্তী_Ptr = CTE_PTR(mdb_গ্রন্থি_get_mdb_পরবর্তী(slot->cteMDBNode));
         পরবর্তী_Ptr && isMDBParentOf(slot, পরবর্তী_Ptr);
         পরবর্তী_Ptr = CTE_PTR(mdb_গ্রন্থি_get_mdb_পরবর্তী(slot->cteMDBNode))) {
        status = cteDelete(পরবর্তী_Ptr, true);
        যদি (status != EXCEPTION_NONE) {
            ফেরত status;
        }

        status = preemptionPoint();
        যদি (status != EXCEPTION_NONE) {
            ফেরত status;
        }
    }

    ফেরত EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *slot, bool_t exposed)
{
    finaliseSlot_ret_t fs_ret;

    fs_ret = finaliseSlot(slot, exposed);
    যদি (fs_ret.status != EXCEPTION_NONE) {
        ফেরত fs_ret.status;
    }

    যদি (exposed || fs_ret.success) {
        emptySlot(slot, fs_ret.cleanupInfo);
    }
    ফেরত EXCEPTION_NONE;
}

স্থির শূন্য emptySlot(cte_t *slot, cap_t cleanupInfo)
{
    যদি (cap_get_capType(slot->cap) != cap_null_cap) {
        mdb_গ্রন্থি_t mdb_গ্রন্থি;
        cte_t *পূর্ব, *পরবর্তী;

        mdb_গ্রন্থি = slot->cteMDBNode;
        পূর্ব = CTE_PTR(mdb_গ্রন্থি_get_mdb_পূর্ব(mdb_গ্রন্থি));
        পরবর্তী = CTE_PTR(mdb_গ্রন্থি_get_mdb_পরবর্তী(mdb_গ্রন্থি));

        যদি (পূর্ব) {
            mdb_গ্রন্থি_ptr_set_mdb_পরবর্তী(&পূর্ব->cteMDBNode, CTE_REF(পরবর্তী));
        }
        যদি (পরবর্তী) {
            mdb_গ্রন্থি_ptr_set_mdb_পূর্ব(&পরবর্তী->cteMDBNode, CTE_REF(পূর্ব));
        }
        যদি (পরবর্তী)
            mdb_গ্রন্থি_ptr_set_mdb_First_Badged(&পরবর্তী->cteMDBNode,
                                            mdb_গ্রন্থি_get_mdb_First_Badged(পরবর্তী->cteMDBNode) ||
                                            mdb_গ্রন্থি_get_mdb_First_Badged(mdb_গ্রন্থি));
        slot->cap = cap_null_cap_new();
        slot->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

স্থির অন্তঃরেখ bool_t CONST capRemovable(cap_t cap, cte_t *slot)
{
    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_null_cap:
        ফেরত true;
    ক্ষেত্র cap_zombie_cap: {
        word_t n = cap_zombie_cap_get_cap_Zombie_সংখ্যা(cap);
        cte_t *z_slot = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        ফেরত (n == 0 || (n == 1 && slot == z_slot));
    }
    পূর্বনির্ধারিত:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

স্থির অন্তঃরেখ bool_t CONST capCyclicZombie(cap_t cap, cte_t *slot)
{
    ফেরত cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == slot;
}

স্থির finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t immediate)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t status;
    finaliseSlot_ret_t ret;

    যখন (cap_get_capType(slot->cap) != cap_null_cap) {
        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, false);

        যদি (capRemovable(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            ফেরত ret;
        }

        slot->cap = fc_ret.remainder;

        যদি (!immediate && capCyclicZombie(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            ফেরত ret;
        }

        status = reduceZombie(slot, immediate);
        যদি (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            ফেরত ret;
        }

        status = preemptionPoint();
        যদি (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            ফেরত ret;
        }
    }
    ret.status = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    ফেরত ret;
}

স্থির exception_t reduceZombie(cte_t *slot, bool_t immediate)
{
    cte_t *ptr;
    word_t n, type;
    exception_t status;

    প্রতিজ্ঞা(cap_get_capType(slot->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);
    n = cap_zombie_cap_get_cap_Zombie_সংখ্যা(slot->cap);
    type = cap_zombie_cap_get_capZombieType(slot->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    প্রতিজ্ঞা(n > 0);

    যদি (immediate) {
        cte_t *endSlot = &ptr[n - 1];

        status = cteDelete(endSlot, false);
        যদি (status != EXCEPTION_NONE) {
            ফেরত status;
        }

        নির্বাচন (cap_get_capType(slot->cap)) {
        ক্ষেত্র cap_null_cap:
            বিরতি;

        ক্ষেত্র cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);

            যদি (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_সংখ্যা(slot->cap) == n &&
                cap_zombie_cap_get_capZombieType(slot->cap) == type) {
                প্রতিজ্ঞা(cap_get_capType(endSlot->cap) == cap_null_cap);
                slot->cap =
                    cap_zombie_cap_set_cap_Zombie_সংখ্যা(slot->cap, n - 1);
            } অন্যথা {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                প্রতিজ্ঞা(ptr2 == slot && ptr != slot);
            }
            বিরতি;
        }

        পূর্বনির্ধারিত:
            fail("Expected recursion to result in Zombie.");
        }
    } অন্যথা {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        প্রতিজ্ঞা(ptr != slot);

        যদি (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            প্রতিজ্ঞা(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        capSwapForDelete(ptr, slot);
    }
    ফেরত EXCEPTION_NONE;
}

শূন্য cteDeleteOne(cte_t *slot)
{
    word_t cap_type = cap_get_capType(slot->cap);
    যদি (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret UNUSED;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        প্রতিজ্ঞা(capRemovable(fc_ret.remainder, slot) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        emptySlot(slot, cap_null_cap_new());
    }
}

শূন্য ঢোকাও_New_Cap(cte_t *parent, cte_t *slot, cap_t cap)
{
    cte_t *পরবর্তী;

    পরবর্তী = CTE_PTR(mdb_গ্রন্থি_get_mdb_পরবর্তী(parent->cteMDBNode));
    slot->cap = cap;
    slot->cteMDBNode = mdb_গ্রন্থি_new(CTE_REF(পরবর্তী), true, true, CTE_REF(parent));
    যদি (পরবর্তী) {
        mdb_গ্রন্থি_ptr_set_mdb_পূর্ব(&পরবর্তী->cteMDBNode, CTE_REF(slot));
    }
    mdb_গ্রন্থি_ptr_set_mdb_পরবর্তী(&parent->cteMDBNode, CTE_REF(slot));
}

#ifndef CONFIG_KERNEL_MCS
শূন্য setupReplyMaster(tcb_t *thread)
{
    cte_t *slot;

    slot = TCB_PTR_CTE_PTR(thread, tcbReply);
    যদি (cap_get_capType(slot->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        slot->cap = cap_reply_cap_new(true, true, TCB_REF(thread));
        slot->cteMDBNode = nullMDBNode;
        mdb_গ্রন্থি_ptr_set_mdb_Revocable(&slot->cteMDBNode, true);
        mdb_গ্রন্থি_ptr_set_mdb_First_Badged(&slot->cteMDBNode, true);
    }
}
#endif

bool_t PURE isMDBParentOf(cte_t *cte_a, cte_t *cte_b)
{
    যদি (!mdb_গ্রন্থি_get_mdb_Revocable(cte_a->cteMDBNode)) {
        ফেরত false;
    }
    যদি (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        ফেরত false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    যদি (!Arch_isMDBParentOf(cte_a->cap, cte_b->cap,
                            mdb_গ্রন্থি_get_mdb_First_Badged(cte_b->cteMDBNode))) {
        ফেরত false;
    }
    নির্বাচন (cap_get_capType(cte_a->cap)) {
    ক্ষেত্র cap_endpoint_cap: {
        word_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        যদি (badge == 0) {
            ফেরত true;
        }
        ফেরত (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_গ্রন্থি_get_mdb_First_Badged(cte_b->cteMDBNode);
        বিরতি;
    }

    ক্ষেত্র cap_notification_cap: {
        word_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        যদি (badge == 0) {
            ফেরত true;
        }
        ফেরত (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_গ্রন্থি_get_mdb_First_Badged(cte_b->cteMDBNode);
        বিরতি;
    }

    পূর্বনির্ধারিত:
        ফেরত true;
        বিরতি;
    }
}

exception_t ensureNoChildren(cte_t *slot)
{
    যদি (mdb_গ্রন্থি_get_mdb_পরবর্তী(slot->cteMDBNode) != 0) {
        cte_t *পরবর্তী;

        পরবর্তী = CTE_PTR(mdb_গ্রন্থি_get_mdb_পরবর্তী(slot->cteMDBNode));
        যদি (isMDBParentOf(slot, পরবর্তী)) {
            current_syscall_error.type = seL4_RevokeFirst;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
    }

    ফেরত EXCEPTION_NONE;
}

exception_t ensureEmptySlot(cte_t *slot)
{
    যদি (cap_get_capType(slot->cap) != cap_null_cap) {
        current_syscall_error.type = seL4_DeleteFirst;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    ফেরত EXCEPTION_NONE;
}

bool_t PURE isFinalCapability(cte_t *cte)
{
    mdb_গ্রন্থি_t mdb;
    bool_t পূর্ব_Is_Same_Object;

    mdb = cte->cteMDBNode;

    যদি (mdb_গ্রন্থি_get_mdb_পূর্ব(mdb) == 0) {
        পূর্ব_Is_Same_Object = false;
    } অন্যথা {
        cte_t *পূর্ব;

        পূর্ব = CTE_PTR(mdb_গ্রন্থি_get_mdb_পূর্ব(mdb));
        পূর্ব_Is_Same_Object = sameObjectAs(পূর্ব->cap, cte->cap);
    }

    যদি (পূর্ব_Is_Same_Object) {
        ফেরত false;
    } অন্যথা {
        যদি (mdb_গ্রন্থি_get_mdb_পরবর্তী(mdb) == 0) {
            ফেরত true;
        } অন্যথা {
            cte_t *পরবর্তী;

            পরবর্তী = CTE_PTR(mdb_গ্রন্থি_get_mdb_পরবর্তী(mdb));
            ফেরত !sameObjectAs(cte->cap, পরবর্তী->cap);
        }
    }
}

bool_t PURE slotCapLongRunningDelete(cte_t *slot)
{
    যদি (cap_get_capType(slot->cap) == cap_null_cap) {
        ফেরত false;
    } অন্যথা যদি (! isFinalCapability(slot)) {
        ফেরত false;
    }
    নির্বাচন (cap_get_capType(slot->cap)) {
    ক্ষেত্র cap_thread_cap:
    ক্ষেত্র cap_zombie_cap:
    ক্ষেত্র cap_cnode_cap:
        ফেরত true;
    পূর্বনির্ধারিত:
        ফেরত false;
    }
}

/* This implementation is specialised to the (current) limit
 * of one cap receive slot. */
cte_t *getReceiveSlots(tcb_t *thread, word_t *বাফার)
{
    cap_transfer_t ct;
    cptr_t cptr;
    lookupCap_ret_t luc_ret;
    lookupSlot_ret_t lus_ret;
    cte_t *slot;
    cap_t cnode;

    যদি (!বাফার) {
        ফেরত ফাঁকা;
    }

    ct = loadCapTransfer(বাফার);
    cptr = ct.ct_Receive_মূল;

    luc_ret = lookupCap(thread, cptr);
    যদি (luc_ret.status != EXCEPTION_NONE) {
        ফেরত ফাঁকা;
    }
    cnode = luc_ret.cap;

    lus_ret = lookupTargetSlot(cnode, ct.ct_Receive_সূচক, ct.ctReceiveDepth);
    যদি (lus_ret.status != EXCEPTION_NONE) {
        ফেরত ফাঁকা;
    }
    slot = lus_ret.slot;

    যদি (cap_get_capType(slot->cap) != cap_null_cap) {
        ফেরত ফাঁকা;
    }

    ফেরত slot;
}

cap_transfer_t PURE loadCapTransfer(word_t *বাফার)
{
    ধ্রুবক পূর্ণসংখ্যা offset = se_L4_বার্তা_Max_দৈর্ঘ্য + se_L4_বার্তা_Max_Extra_Caps + 2;
    ফেরত cap_Transfer_From_শব্দগুলি(বাফার + offset);
}
