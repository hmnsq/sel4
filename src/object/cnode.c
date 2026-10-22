/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <api/invocation.h>
#અન્તર્ભાવ <api/syscall.h>
#અન્તર્ભાવ <api/types.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <object/structures.h>
#અન્તર્ભાવ <object/objecttype.h>
#અન્તર્ભાવ <object/cnode.h>
#અન્તર્ભાવ <object/interrupt.h>
#અન્તર્ભાવ <object/untyped.h>
#અન્તર્ભાવ <kernel/cspace.h>
#અન્તર્ભાવ <kernel/thread.h>
#અન્તર્ભાવ <model/preemption.h>
#અન્તર્ભાવ <model/statedata.h>
#અન્તર્ભાવ <util.h>

સંરચના finaliseSlot_ret {
    exception_t status;
    bool_t success;
    cap_t cleanupInfo;
};
પ્રકારવ્યાખ્યા સંરચના finaliseSlot_ret finaliseSlot_ret_t;

સ્થિર finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t exposed);
સ્થિર શૂન્ય emptySlot(cte_t *slot, cap_t cleanupInfo);
સ્થિર exception_t reduceZombie(cte_t *slot, bool_t exposed);

#ifdef CONFIG_KERNEL_MCS
#define CNODE_LAST_INVOCATION CNodeRotate
#નહીંતર
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
    પ્રતિજ્ઞા(cap_get_capType(cap) == cap_cnode_cap);

    જો (invLabel < CNodeRevoke || invLabel > CNODE_LAST_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (length < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    w_bits = getSyscallArg(1, buffer);

    lu_ret = lookupTargetSlot(cap, index, w_bits);
    જો (lu_ret.status != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        પાછા lu_ret.status;
    }
    destSlot = lu_ret.slot;

    જો (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *srcSlot;
        word_t srcIndex, srcDepth, capData;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_મૂળ, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        જો (length < 4 || current_extra_caps.excaprefs[0] == ખાલી) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        srcIndex = getSyscallArg(2, buffer);
        srcDepth = getSyscallArg(3, buffer);

        src_મૂળ = current_extra_caps.excaprefs[0]->cap;

        status = ensureEmptySlot(destSlot);
        જો (status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            પાછા status;
        }

        lu_ret = lookupSourceSlot(src_મૂળ, srcIndex, srcDepth);
        જો (lu_ret.status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            પાછા lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        જો (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(srcDepth);
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        પસંદગી (invLabel) {
        સ્થિતિ CNodeCopy:

            જો (length < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = seL4_TruncatedMessage;
                પાછા EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot, srcCap);
            જો (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                પાછા dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            તોડો;

        સ્થિતિ CNodeMint:
            જો (length < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                પાછા EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            capData = getSyscallArg(5, buffer);
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot,
                               updateCapData(false, capData, srcCap));
            જો (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                પાછા dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            તોડો;

        સ્થિતિ CNodeMove:
            newCap = srcSlot->cap;
            isMove = true;

            તોડો;

        સ્થિતિ CNodeMutate:
            જો (length < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                પાછા EXCEPTION_SYSCALL_ERROR;
            }

            capData = getSyscallArg(4, buffer);
            newCap = updateCapData(true, capData, srcSlot->cap);
            isMove = true;

            તોડો;

        મૂળભૂત:
            પ્રતિજ્ઞા(0);
            પાછા EXCEPTION_NONE;
        }

        જો (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        જો (isMove) {
            પાછા invokeCNodeMove(newCap, srcSlot, destSlot);
        } નહીંતર {
            પાછા invoke_CNode_ઉમેરો(newCap, srcSlot, destSlot);
        }
    }

    જો (invLabel == CNodeRevoke) {
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા invokeCNodeRevoke(destSlot);
    }

    જો (invLabel == CNodeDelete) {
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા invokeCNodeDelete(destSlot);
    }

#ifndef CONFIG_KERNEL_MCS
    જો (invLabel == CNodeSaveCaller) {
        status = ensureEmptySlot(destSlot);
        જો (status != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            પાછા status;
        }

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા invokeCNodeSaveCaller(destSlot);
    }
#endif

    જો (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = destSlot->cap;

        જો (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા invokeCNodeCancelBadgedSends(destCap);
    }

    જો (invLabel == CNodeRotate) {
        word_t pivotNewData, pivotIndex, pivotDepth;
        word_t srcNewData, srcIndex, srcDepth;
        cte_t *pivotSlot, *srcSlot;
        cap_t pivot_મૂળ, src_મૂળ, newSrcCap, newPivotCap;

        જો (length < 8 || current_extra_caps.excaprefs[0] == ખાલી
            || current_extra_caps.excaprefs[1] == ખાલી) {
            current_syscall_error.type = seL4_TruncatedMessage;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        pivotNewData = getSyscallArg(2, buffer);
        pivotIndex   = getSyscallArg(3, buffer);
        pivotDepth   = getSyscallArg(4, buffer);
        srcNewData   = getSyscallArg(5, buffer);
        srcIndex     = getSyscallArg(6, buffer);
        srcDepth     = getSyscallArg(7, buffer);

        pivot_મૂળ = current_extra_caps.excaprefs[0]->cap;
        src_મૂળ   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookupSourceSlot(src_મૂળ, srcIndex, srcDepth);
        જો (lu_ret.status != EXCEPTION_NONE) {
            પાછા lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        lu_ret = lookupPivotSlot(pivot_મૂળ, pivotIndex, pivotDepth);
        જો (lu_ret.status != EXCEPTION_NONE) {
            પાછા lu_ret.status;
        }
        pivotSlot = lu_ret.slot;

        જો (pivotSlot == srcSlot || pivotSlot == destSlot) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (srcSlot != destSlot) {
            status = ensureEmptySlot(destSlot);
            જો (status != EXCEPTION_NONE) {
                પાછા status;
            }
        }

        જો (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(srcDepth);
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (cap_get_capType(pivotSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(pivotDepth);
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = updateCapData(true, srcNewData, srcSlot->cap);
        newPivotCap = updateCapData(true, pivotNewData, pivotSlot->cap);

        જો (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        જો (cap_get_capType(newPivotCap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા invokeCNodeRotate(newSrcCap, newPivotCap,
                                 srcSlot, pivotSlot, destSlot);
    }

    પાછા EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *destSlot)
{
    પાછા cteRevoke(destSlot);
}

exception_t invokeCNodeDelete(cte_t *destSlot)
{
    પાછા cteDelete(destSlot, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    word_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    જો (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    પાછા EXCEPTION_NONE;
}

exception_t invoke_CNode_ઉમેરો(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cte_ઉમેરો(cap, srcSlot, destSlot);

    પાછા EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cteMove(cap, srcSlot, destSlot);

    પાછા EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    જો (slot1 == slot3) {
        cteSwap(cap1, slot1, cap2, slot2);
    } નહીંતર {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    પાછા EXCEPTION_NONE;
}

#ifndef CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *destSlot)
{
    cap_t cap;
    cte_t *srcSlot;

    srcSlot = TCB_PTR_CTE_PTR(ગાંઠ_STATE(ksCurThread), tcbCaller);
    cap = srcSlot->cap;

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        તોડો;

    સ્થિતિ cap_reply_cap:
        જો (!cap_reply_cap_get_capReplyMaster(cap)) {
            cteMove(cap, srcSlot, destSlot);
        }
        તોડો;

    મૂળભૂત:
        fail("caller capability must be null or reply");
        તોડો;
    }

    પાછા EXCEPTION_NONE;
}
#endif

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
સ્થિર શૂન્ય setUntypedCapAsFull(cap_t srcCap, cap_t newCap, cte_t *srcSlot)
{
    જો ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        જો ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_capBlockSize(newCap)
                == cap_untyped_cap_get_capBlockSize(srcCap))) {
            cap_untyped_cap_ptr_set_capFreeIndex(&(srcSlot->cap),
                                                 MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(srcCap)));
        }
    }
}

શૂન્ય cte_ઉમેરો(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_ગાંઠ_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = srcSlot->cteMDBNode;
    srcCap = srcSlot->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_ગાંઠ_set_mdb_Prev(srcMDB, CTE_REF(srcSlot));
    newMDB = mdb_ગાંઠ_set_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_ગાંઠ_set_mdb_First_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    પ્રતિજ્ઞા(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    પ્રતિજ્ઞા((cte_t *)mdb_ગાંઠ_get_mdb_Next(destSlot->cteMDBNode) == ખાલી &&
           (cte_t *)mdb_ગાંઠ_get_mdb_Prev(destSlot->cteMDBNode) == ખાલી);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    setUntypedCapAsFull(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSlot->cteMDBNode = newMDB;
    mdb_ગાંઠ_ptr_set_mdb_Next(&srcSlot->cteMDBNode, CTE_REF(destSlot));
    જો (mdb_ગાંઠ_get_mdb_Next(newMDB)) {
        mdb_ગાંઠ_ptr_set_mdb_Prev(
            &CTE_PTR(mdb_ગાંઠ_get_mdb_Next(newMDB))->cteMDBNode,
            CTE_REF(destSlot));
    }
}

શૂન્ય cteMove(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_ગાંઠ_t mdb;
    word_t prev_ptr, next_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    પ્રતિજ્ઞા(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    પ્રતિજ્ઞા((cte_t *)mdb_ગાંઠ_get_mdb_Next(destSlot->cteMDBNode) == ખાલી &&
           (cte_t *)mdb_ગાંઠ_get_mdb_Prev(destSlot->cteMDBNode) == ખાલી);

    mdb = srcSlot->cteMDBNode;
    destSlot->cap = newCap;
    srcSlot->cap = cap_null_cap_new();
    destSlot->cteMDBNode = mdb;
    srcSlot->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_ગાંઠ_get_mdb_Prev(mdb);
    જો (prev_ptr)
        mdb_ગાંઠ_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(destSlot));

    next_ptr = mdb_ગાંઠ_get_mdb_Next(mdb);
    જો (next_ptr)
        mdb_ગાંઠ_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(destSlot));
}

શૂન્ય capSwapForDelete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    જો (slot1 == slot2) {
        પાછા;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cteSwap(cap1, slot1, cap2, slot2);
}

શૂન્ય cteSwap(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_ગાંઠ_t mdb1, mdb2;
    word_t next_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_ગાંઠ_get_mdb_Prev(mdb1);
    જો (prev_ptr)
        mdb_ગાંઠ_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    next_ptr = mdb_ગાંઠ_get_mdb_Next(mdb1);
    જો (next_ptr)
        mdb_ગાંઠ_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_ગાંઠ_get_mdb_Prev(mdb2);
    જો (prev_ptr)
        mdb_ગાંઠ_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    next_ptr = mdb_ગાંઠ_get_mdb_Next(mdb2);
    જો (next_ptr)
        mdb_ગાંઠ_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *slot)
{
    cte_t *nextPtr;
    exception_t status;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    માટે (nextPtr = CTE_PTR(mdb_ગાંઠ_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_ગાંઠ_get_mdb_Next(slot->cteMDBNode))) {
        status = cteDelete(nextPtr, true);
        જો (status != EXCEPTION_NONE) {
            પાછા status;
        }

        status = preemptionPoint();
        જો (status != EXCEPTION_NONE) {
            પાછા status;
        }
    }

    પાછા EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *slot, bool_t exposed)
{
    finaliseSlot_ret_t fs_ret;

    fs_ret = finaliseSlot(slot, exposed);
    જો (fs_ret.status != EXCEPTION_NONE) {
        પાછા fs_ret.status;
    }

    જો (exposed || fs_ret.success) {
        emptySlot(slot, fs_ret.cleanupInfo);
    }
    પાછા EXCEPTION_NONE;
}

સ્થિર શૂન્ય emptySlot(cte_t *slot, cap_t cleanupInfo)
{
    જો (cap_get_capType(slot->cap) != cap_null_cap) {
        mdb_ગાંઠ_t mdb_ગાંઠ;
        cte_t *prev, *next;

        mdb_ગાંઠ = slot->cteMDBNode;
        prev = CTE_PTR(mdb_ગાંઠ_get_mdb_Prev(mdb_ગાંઠ));
        next = CTE_PTR(mdb_ગાંઠ_get_mdb_Next(mdb_ગાંઠ));

        જો (prev) {
            mdb_ગાંઠ_ptr_set_mdb_Next(&prev->cteMDBNode, CTE_REF(next));
        }
        જો (next) {
            mdb_ગાંઠ_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(prev));
        }
        જો (next)
            mdb_ગાંઠ_ptr_set_mdb_First_Badged(&next->cteMDBNode,
                                            mdb_ગાંઠ_get_mdb_First_Badged(next->cteMDBNode) ||
                                            mdb_ગાંઠ_get_mdb_First_Badged(mdb_ગાંઠ));
        slot->cap = cap_null_cap_new();
        slot->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

સ્થિર અંતઃરેખ bool_t CONST capRemovable(cap_t cap, cte_t *slot)
{
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_null_cap:
        પાછા true;
    સ્થિતિ cap_zombie_cap: {
        word_t n = cap_zombie_cap_get_cap_Zombie_સંખ્યા(cap);
        cte_t *z_slot = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        પાછા (n == 0 || (n == 1 && slot == z_slot));
    }
    મૂળભૂત:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

સ્થિર અંતઃરેખ bool_t CONST capCyclicZombie(cap_t cap, cte_t *slot)
{
    પાછા cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == slot;
}

સ્થિર finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t immediate)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t status;
    finaliseSlot_ret_t ret;

    જ્યારે (cap_get_capType(slot->cap) != cap_null_cap) {
        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, false);

        જો (capRemovable(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            પાછા ret;
        }

        slot->cap = fc_ret.remainder;

        જો (!immediate && capCyclicZombie(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            પાછા ret;
        }

        status = reduceZombie(slot, immediate);
        જો (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            પાછા ret;
        }

        status = preemptionPoint();
        જો (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            પાછા ret;
        }
    }
    ret.status = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    પાછા ret;
}

સ્થિર exception_t reduceZombie(cte_t *slot, bool_t immediate)
{
    cte_t *ptr;
    word_t n, type;
    exception_t status;

    પ્રતિજ્ઞા(cap_get_capType(slot->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);
    n = cap_zombie_cap_get_cap_Zombie_સંખ્યા(slot->cap);
    type = cap_zombie_cap_get_capZombieType(slot->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    પ્રતિજ્ઞા(n > 0);

    જો (immediate) {
        cte_t *endSlot = &ptr[n - 1];

        status = cteDelete(endSlot, false);
        જો (status != EXCEPTION_NONE) {
            પાછા status;
        }

        પસંદગી (cap_get_capType(slot->cap)) {
        સ્થિતિ cap_null_cap:
            તોડો;

        સ્થિતિ cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);

            જો (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_સંખ્યા(slot->cap) == n &&
                cap_zombie_cap_get_capZombieType(slot->cap) == type) {
                પ્રતિજ્ઞા(cap_get_capType(endSlot->cap) == cap_null_cap);
                slot->cap =
                    cap_zombie_cap_set_cap_Zombie_સંખ્યા(slot->cap, n - 1);
            } નહીંતર {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                પ્રતિજ્ઞા(ptr2 == slot && ptr != slot);
            }
            તોડો;
        }

        મૂળભૂત:
            fail("Expected recursion to result in Zombie.");
        }
    } નહીંતર {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        પ્રતિજ્ઞા(ptr != slot);

        જો (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            પ્રતિજ્ઞા(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        capSwapForDelete(ptr, slot);
    }
    પાછા EXCEPTION_NONE;
}

શૂન્ય cteDeleteOne(cte_t *slot)
{
    word_t cap_type = cap_get_capType(slot->cap);
    જો (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret UNUSED;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        પ્રતિજ્ઞા(capRemovable(fc_ret.remainder, slot) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        emptySlot(slot, cap_null_cap_new());
    }
}

શૂન્ય ઉમેરો_New_Cap(cte_t *parent, cte_t *slot, cap_t cap)
{
    cte_t *next;

    next = CTE_PTR(mdb_ગાંઠ_get_mdb_Next(parent->cteMDBNode));
    slot->cap = cap;
    slot->cteMDBNode = mdb_ગાંઠ_new(CTE_REF(next), true, true, CTE_REF(parent));
    જો (next) {
        mdb_ગાંઠ_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(slot));
    }
    mdb_ગાંઠ_ptr_set_mdb_Next(&parent->cteMDBNode, CTE_REF(slot));
}

#ifndef CONFIG_KERNEL_MCS
શૂન્ય setupReplyMaster(tcb_t *thread)
{
    cte_t *slot;

    slot = TCB_PTR_CTE_PTR(thread, tcbReply);
    જો (cap_get_capType(slot->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        slot->cap = cap_reply_cap_new(true, true, TCB_REF(thread));
        slot->cteMDBNode = nullMDBNode;
        mdb_ગાંઠ_ptr_set_mdb_Revocable(&slot->cteMDBNode, true);
        mdb_ગાંઠ_ptr_set_mdb_First_Badged(&slot->cteMDBNode, true);
    }
}
#endif

bool_t PURE isMDBParentOf(cte_t *cte_a, cte_t *cte_b)
{
    જો (!mdb_ગાંઠ_get_mdb_Revocable(cte_a->cteMDBNode)) {
        પાછા false;
    }
    જો (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        પાછા false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    જો (!Arch_isMDBParentOf(cte_a->cap, cte_b->cap,
                            mdb_ગાંઠ_get_mdb_First_Badged(cte_b->cteMDBNode))) {
        પાછા false;
    }
    પસંદગી (cap_get_capType(cte_a->cap)) {
    સ્થિતિ cap_endpoint_cap: {
        word_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        જો (badge == 0) {
            પાછા true;
        }
        પાછા (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_ગાંઠ_get_mdb_First_Badged(cte_b->cteMDBNode);
        તોડો;
    }

    સ્થિતિ cap_notification_cap: {
        word_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        જો (badge == 0) {
            પાછા true;
        }
        પાછા (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_ગાંઠ_get_mdb_First_Badged(cte_b->cteMDBNode);
        તોડો;
    }

    મૂળભૂત:
        પાછા true;
        તોડો;
    }
}

exception_t ensureNoChildren(cte_t *slot)
{
    જો (mdb_ગાંઠ_get_mdb_Next(slot->cteMDBNode) != 0) {
        cte_t *next;

        next = CTE_PTR(mdb_ગાંઠ_get_mdb_Next(slot->cteMDBNode));
        જો (isMDBParentOf(slot, next)) {
            current_syscall_error.type = seL4_RevokeFirst;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
    }

    પાછા EXCEPTION_NONE;
}

exception_t ensureEmptySlot(cte_t *slot)
{
    જો (cap_get_capType(slot->cap) != cap_null_cap) {
        current_syscall_error.type = seL4_DeleteFirst;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    પાછા EXCEPTION_NONE;
}

bool_t PURE isFinalCapability(cte_t *cte)
{
    mdb_ગાંઠ_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    જો (mdb_ગાંઠ_get_mdb_Prev(mdb) == 0) {
        prevIsSameObject = false;
    } નહીંતર {
        cte_t *prev;

        prev = CTE_PTR(mdb_ગાંઠ_get_mdb_Prev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    જો (prevIsSameObject) {
        પાછા false;
    } નહીંતર {
        જો (mdb_ગાંઠ_get_mdb_Next(mdb) == 0) {
            પાછા true;
        } નહીંતર {
            cte_t *next;

            next = CTE_PTR(mdb_ગાંઠ_get_mdb_Next(mdb));
            પાછા !sameObjectAs(cte->cap, next->cap);
        }
    }
}

bool_t PURE slotCapLongRunningDelete(cte_t *slot)
{
    જો (cap_get_capType(slot->cap) == cap_null_cap) {
        પાછા false;
    } નહીંતર જો (! isFinalCapability(slot)) {
        પાછા false;
    }
    પસંદગી (cap_get_capType(slot->cap)) {
    સ્થિતિ cap_thread_cap:
    સ્થિતિ cap_zombie_cap:
    સ્થિતિ cap_cnode_cap:
        પાછા true;
    મૂળભૂત:
        પાછા false;
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

    જો (!buffer) {
        પાછા ખાલી;
    }

    ct = loadCapTransfer(buffer);
    cptr = ct.ct_Receive_મૂળ;

    luc_ret = lookupCap(thread, cptr);
    જો (luc_ret.status != EXCEPTION_NONE) {
        પાછા ખાલી;
    }
    cnode = luc_ret.cap;

    lus_ret = lookupTargetSlot(cnode, ct.ctReceiveIndex, ct.ctReceiveDepth);
    જો (lus_ret.status != EXCEPTION_NONE) {
        પાછા ખાલી;
    }
    slot = lus_ret.slot;

    જો (cap_get_capType(slot->cap) != cap_null_cap) {
        પાછા ખાલી;
    }

    પાછા slot;
}

cap_transfer_t PURE loadCapTransfer(word_t *buffer)
{
    અચલ પૂર્ણાંક offset = se_L4_સંદેશ_Max_Length + se_L4_સંદેશ_Max_Extra_Caps + 2;
    પાછા cap_Transfer_From_શબ્દો(buffer + offset);
}
