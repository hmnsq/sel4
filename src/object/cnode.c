/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <api/failures.h>
#അന്തര്ഭാവ <api/invocation.h>
#അന്തര്ഭാവ <api/syscall.h>
#അന്തര്ഭാവ <api/types.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <object/structures.h>
#അന്തര്ഭാവ <object/objecttype.h>
#അന്തര്ഭാവ <object/cnode.h>
#അന്തര്ഭാവ <object/interrupt.h>
#അന്തര്ഭാവ <object/untyped.h>
#അന്തര്ഭാവ <kernel/cspace.h>
#അന്തര്ഭാവ <kernel/thread.h>
#അന്തര്ഭാവ <model/preemption.h>
#അന്തര്ഭാവ <model/statedata.h>
#അന്തര്ഭാവ <util.h>

ഘടന finaliseSlot_ret {
    exception_t status;
    bool_t success;
    cap_t cleanupInfo;
};
തരനിർവചനം ഘടന finaliseSlot_ret finaliseSlot_ret_t;

സ്ഥിതം finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t exposed);
സ്ഥിതം ശൂന്യം emptySlot(cte_t *slot, cap_t cleanupInfo);
സ്ഥിതം exception_t reduceZombie(cte_t *slot, bool_t exposed);

#ifdef CONFIG_KERNEL_MCS
#define CNODE_LAST_INVOCATION CNodeRotate
#അല്ലെങ്കിൽ
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
    പ്രതിജ്ഞാ(cap_get_capType(cap) == cap_cnode_cap);

    എങ്കിൽ (invLabel < CNodeRevoke || invLabel > CNODE_LAST_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    എങ്കിൽ (length < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    w_bits = getSyscallArg(1, buffer);

    lu_ret = lookupTargetSlot(cap, index, w_bits);
    എങ്കിൽ (lu_ret.status != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        തിരികെ lu_ret.status;
    }
    destSlot = lu_ret.slot;

    എങ്കിൽ (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *srcSlot;
        word_t srcIndex, srcDepth, capData;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_വേര്, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        എങ്കിൽ (length < 4 || current_extra_caps.excaprefs[0] == ശൂന്യ) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
        srcIndex = getSyscallArg(2, buffer);
        srcDepth = getSyscallArg(3, buffer);

        src_വേര് = current_extra_caps.excaprefs[0]->cap;

        status = ensureEmptySlot(destSlot);
        എങ്കിൽ (status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            തിരികെ status;
        }

        lu_ret = lookupSourceSlot(src_വേര്, srcIndex, srcDepth);
        എങ്കിൽ (lu_ret.status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            തിരികെ lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        എങ്കിൽ (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(srcDepth);
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        തിരഞ്ഞെടുപ്പ് (invLabel) {
        അവസ്ഥ CNodeCopy:

            എങ്കിൽ (length < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = seL4_TruncatedMessage;
                തിരികെ EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot, srcCap);
            എങ്കിൽ (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                തിരികെ dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            നിർത്തുക;

        അവസ്ഥ CNodeMint:
            എങ്കിൽ (length < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                തിരികെ EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            capData = getSyscallArg(5, buffer);
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot,
                               updateCapData(false, capData, srcCap));
            എങ്കിൽ (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                തിരികെ dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            നിർത്തുക;

        അവസ്ഥ CNodeMove:
            newCap = srcSlot->cap;
            isMove = true;

            നിർത്തുക;

        അവസ്ഥ CNodeMutate:
            എങ്കിൽ (length < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                തിരികെ EXCEPTION_SYSCALL_ERROR;
            }

            capData = getSyscallArg(4, buffer);
            newCap = updateCapData(true, capData, srcSlot->cap);
            isMove = true;

            നിർത്തുക;

        സ്ഥിരസ്ഥിതി:
            പ്രതിജ്ഞാ(0);
            തിരികെ EXCEPTION_NONE;
        }

        എങ്കിൽ (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        എങ്കിൽ (isMove) {
            തിരികെ invokeCNodeMove(newCap, srcSlot, destSlot);
        } അല്ലെങ്കിൽ {
            തിരികെ invoke_CNode_തിരുകുക(newCap, srcSlot, destSlot);
        }
    }

    എങ്കിൽ (invLabel == CNodeRevoke) {
        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ invokeCNodeRevoke(destSlot);
    }

    എങ്കിൽ (invLabel == CNodeDelete) {
        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ invokeCNodeDelete(destSlot);
    }

#ifndef CONFIG_KERNEL_MCS
    എങ്കിൽ (invLabel == CNodeSaveCaller) {
        status = ensureEmptySlot(destSlot);
        എങ്കിൽ (status != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            തിരികെ status;
        }

        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ invokeCNodeSaveCaller(destSlot);
    }
#endif

    എങ്കിൽ (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = destSlot->cap;

        എങ്കിൽ (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ invokeCNodeCancelBadgedSends(destCap);
    }

    എങ്കിൽ (invLabel == CNodeRotate) {
        word_t pivotNewData, pivotIndex, pivotDepth;
        word_t srcNewData, srcIndex, srcDepth;
        cte_t *pivotSlot, *srcSlot;
        cap_t pivot_വേര്, src_വേര്, newSrcCap, newPivotCap;

        എങ്കിൽ (length < 8 || current_extra_caps.excaprefs[0] == ശൂന്യ
            || current_extra_caps.excaprefs[1] == ശൂന്യ) {
            current_syscall_error.type = seL4_TruncatedMessage;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
        pivotNewData = getSyscallArg(2, buffer);
        pivotIndex   = getSyscallArg(3, buffer);
        pivotDepth   = getSyscallArg(4, buffer);
        srcNewData   = getSyscallArg(5, buffer);
        srcIndex     = getSyscallArg(6, buffer);
        srcDepth     = getSyscallArg(7, buffer);

        pivot_വേര് = current_extra_caps.excaprefs[0]->cap;
        src_വേര്   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookupSourceSlot(src_വേര്, srcIndex, srcDepth);
        എങ്കിൽ (lu_ret.status != EXCEPTION_NONE) {
            തിരികെ lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        lu_ret = lookupPivotSlot(pivot_വേര്, pivotIndex, pivotDepth);
        എങ്കിൽ (lu_ret.status != EXCEPTION_NONE) {
            തിരികെ lu_ret.status;
        }
        pivotSlot = lu_ret.slot;

        എങ്കിൽ (pivotSlot == srcSlot || pivotSlot == destSlot) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        എങ്കിൽ (srcSlot != destSlot) {
            status = ensureEmptySlot(destSlot);
            എങ്കിൽ (status != EXCEPTION_NONE) {
                തിരികെ status;
            }
        }

        എങ്കിൽ (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(srcDepth);
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        എങ്കിൽ (cap_get_capType(pivotSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(pivotDepth);
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = updateCapData(true, srcNewData, srcSlot->cap);
        newPivotCap = updateCapData(true, pivotNewData, pivotSlot->cap);

        എങ്കിൽ (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        എങ്കിൽ (cap_get_capType(newPivotCap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ invokeCNodeRotate(newSrcCap, newPivotCap,
                                 srcSlot, pivotSlot, destSlot);
    }

    തിരികെ EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *destSlot)
{
    തിരികെ cteRevoke(destSlot);
}

exception_t invokeCNodeDelete(cte_t *destSlot)
{
    തിരികെ cteDelete(destSlot, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    word_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    എങ്കിൽ (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    തിരികെ EXCEPTION_NONE;
}

exception_t invoke_CNode_തിരുകുക(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cte_തിരുകുക(cap, srcSlot, destSlot);

    തിരികെ EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cteMove(cap, srcSlot, destSlot);

    തിരികെ EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    എങ്കിൽ (slot1 == slot3) {
        cteSwap(cap1, slot1, cap2, slot2);
    } അല്ലെങ്കിൽ {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    തിരികെ EXCEPTION_NONE;
}

#ifndef CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *destSlot)
{
    cap_t cap;
    cte_t *srcSlot;

    srcSlot = TCB_PTR_CTE_PTR(കണ്ണി_STATE(ksCurThread), tcbCaller);
    cap = srcSlot->cap;

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        നിർത്തുക;

    അവസ്ഥ cap_reply_cap:
        എങ്കിൽ (!cap_reply_cap_get_capReplyMaster(cap)) {
            cteMove(cap, srcSlot, destSlot);
        }
        നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
        fail("caller capability must be null or reply");
        നിർത്തുക;
    }

    തിരികെ EXCEPTION_NONE;
}
#endif

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
സ്ഥിതം ശൂന്യം setUntypedCapAsFull(cap_t srcCap, cap_t newCap, cte_t *srcSlot)
{
    എങ്കിൽ ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        എങ്കിൽ ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_capBlockSize(newCap)
                == cap_untyped_cap_get_capBlockSize(srcCap))) {
            cap_untyped_cap_ptr_set_capFreeIndex(&(srcSlot->cap),
                                                 MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(srcCap)));
        }
    }
}

ശൂന്യം cte_തിരുകുക(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_കണ്ണി_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = srcSlot->cteMDBNode;
    srcCap = srcSlot->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_കണ്ണി_set_mdb_Prev(srcMDB, CTE_REF(srcSlot));
    newMDB = mdb_കണ്ണി_set_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_കണ്ണി_set_mdb_First_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    പ്രതിജ്ഞാ(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    പ്രതിജ്ഞാ((cte_t *)mdb_കണ്ണി_get_mdb_Next(destSlot->cteMDBNode) == ശൂന്യ &&
           (cte_t *)mdb_കണ്ണി_get_mdb_Prev(destSlot->cteMDBNode) == ശൂന്യ);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    setUntypedCapAsFull(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSlot->cteMDBNode = newMDB;
    mdb_കണ്ണി_ptr_set_mdb_Next(&srcSlot->cteMDBNode, CTE_REF(destSlot));
    എങ്കിൽ (mdb_കണ്ണി_get_mdb_Next(newMDB)) {
        mdb_കണ്ണി_ptr_set_mdb_Prev(
            &CTE_PTR(mdb_കണ്ണി_get_mdb_Next(newMDB))->cteMDBNode,
            CTE_REF(destSlot));
    }
}

ശൂന്യം cteMove(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_കണ്ണി_t mdb;
    word_t prev_ptr, next_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    പ്രതിജ്ഞാ(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    പ്രതിജ്ഞാ((cte_t *)mdb_കണ്ണി_get_mdb_Next(destSlot->cteMDBNode) == ശൂന്യ &&
           (cte_t *)mdb_കണ്ണി_get_mdb_Prev(destSlot->cteMDBNode) == ശൂന്യ);

    mdb = srcSlot->cteMDBNode;
    destSlot->cap = newCap;
    srcSlot->cap = cap_null_cap_new();
    destSlot->cteMDBNode = mdb;
    srcSlot->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_കണ്ണി_get_mdb_Prev(mdb);
    എങ്കിൽ (prev_ptr)
        mdb_കണ്ണി_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(destSlot));

    next_ptr = mdb_കണ്ണി_get_mdb_Next(mdb);
    എങ്കിൽ (next_ptr)
        mdb_കണ്ണി_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(destSlot));
}

ശൂന്യം capSwapForDelete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    എങ്കിൽ (slot1 == slot2) {
        തിരികെ;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cteSwap(cap1, slot1, cap2, slot2);
}

ശൂന്യം cteSwap(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_കണ്ണി_t mdb1, mdb2;
    word_t next_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_കണ്ണി_get_mdb_Prev(mdb1);
    എങ്കിൽ (prev_ptr)
        mdb_കണ്ണി_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    next_ptr = mdb_കണ്ണി_get_mdb_Next(mdb1);
    എങ്കിൽ (next_ptr)
        mdb_കണ്ണി_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_കണ്ണി_get_mdb_Prev(mdb2);
    എങ്കിൽ (prev_ptr)
        mdb_കണ്ണി_ptr_set_mdb_Next(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    next_ptr = mdb_കണ്ണി_get_mdb_Next(mdb2);
    എങ്കിൽ (next_ptr)
        mdb_കണ്ണി_ptr_set_mdb_Prev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *slot)
{
    cte_t *nextPtr;
    exception_t status;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    വേണ്ടി (nextPtr = CTE_PTR(mdb_കണ്ണി_get_mdb_Next(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_കണ്ണി_get_mdb_Next(slot->cteMDBNode))) {
        status = cteDelete(nextPtr, true);
        എങ്കിൽ (status != EXCEPTION_NONE) {
            തിരികെ status;
        }

        status = preemptionPoint();
        എങ്കിൽ (status != EXCEPTION_NONE) {
            തിരികെ status;
        }
    }

    തിരികെ EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *slot, bool_t exposed)
{
    finaliseSlot_ret_t fs_ret;

    fs_ret = finaliseSlot(slot, exposed);
    എങ്കിൽ (fs_ret.status != EXCEPTION_NONE) {
        തിരികെ fs_ret.status;
    }

    എങ്കിൽ (exposed || fs_ret.success) {
        emptySlot(slot, fs_ret.cleanupInfo);
    }
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം ശൂന്യം emptySlot(cte_t *slot, cap_t cleanupInfo)
{
    എങ്കിൽ (cap_get_capType(slot->cap) != cap_null_cap) {
        mdb_കണ്ണി_t mdb_കണ്ണി;
        cte_t *prev, *next;

        mdb_കണ്ണി = slot->cteMDBNode;
        prev = CTE_PTR(mdb_കണ്ണി_get_mdb_Prev(mdb_കണ്ണി));
        next = CTE_PTR(mdb_കണ്ണി_get_mdb_Next(mdb_കണ്ണി));

        എങ്കിൽ (prev) {
            mdb_കണ്ണി_ptr_set_mdb_Next(&prev->cteMDBNode, CTE_REF(next));
        }
        എങ്കിൽ (next) {
            mdb_കണ്ണി_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(prev));
        }
        എങ്കിൽ (next)
            mdb_കണ്ണി_ptr_set_mdb_First_Badged(&next->cteMDBNode,
                                            mdb_കണ്ണി_get_mdb_First_Badged(next->cteMDBNode) ||
                                            mdb_കണ്ണി_get_mdb_First_Badged(mdb_കണ്ണി));
        slot->cap = cap_null_cap_new();
        slot->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

സ്ഥിതം വരിയിൽ bool_t CONST capRemovable(cap_t cap, cte_t *slot)
{
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_null_cap:
        തിരികെ true;
    അവസ്ഥ cap_zombie_cap: {
        word_t n = cap_zombie_cap_get_cap_Zombie_സംഖ്യ(cap);
        cte_t *z_slot = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        തിരികെ (n == 0 || (n == 1 && slot == z_slot));
    }
    സ്ഥിരസ്ഥിതി:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

സ്ഥിതം വരിയിൽ bool_t CONST capCyclicZombie(cap_t cap, cte_t *slot)
{
    തിരികെ cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == slot;
}

സ്ഥിതം finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t immediate)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t status;
    finaliseSlot_ret_t ret;

    ആകുമ്പോൾ (cap_get_capType(slot->cap) != cap_null_cap) {
        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, false);

        എങ്കിൽ (capRemovable(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            തിരികെ ret;
        }

        slot->cap = fc_ret.remainder;

        എങ്കിൽ (!immediate && capCyclicZombie(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            തിരികെ ret;
        }

        status = reduceZombie(slot, immediate);
        എങ്കിൽ (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            തിരികെ ret;
        }

        status = preemptionPoint();
        എങ്കിൽ (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            തിരികെ ret;
        }
    }
    ret.status = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    തിരികെ ret;
}

സ്ഥിതം exception_t reduceZombie(cte_t *slot, bool_t immediate)
{
    cte_t *ptr;
    word_t n, type;
    exception_t status;

    പ്രതിജ്ഞാ(cap_get_capType(slot->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);
    n = cap_zombie_cap_get_cap_Zombie_സംഖ്യ(slot->cap);
    type = cap_zombie_cap_get_capZombieType(slot->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    പ്രതിജ്ഞാ(n > 0);

    എങ്കിൽ (immediate) {
        cte_t *endSlot = &ptr[n - 1];

        status = cteDelete(endSlot, false);
        എങ്കിൽ (status != EXCEPTION_NONE) {
            തിരികെ status;
        }

        തിരഞ്ഞെടുപ്പ് (cap_get_capType(slot->cap)) {
        അവസ്ഥ cap_null_cap:
            നിർത്തുക;

        അവസ്ഥ cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);

            എങ്കിൽ (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_സംഖ്യ(slot->cap) == n &&
                cap_zombie_cap_get_capZombieType(slot->cap) == type) {
                പ്രതിജ്ഞാ(cap_get_capType(endSlot->cap) == cap_null_cap);
                slot->cap =
                    cap_zombie_cap_set_cap_Zombie_സംഖ്യ(slot->cap, n - 1);
            } അല്ലെങ്കിൽ {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                പ്രതിജ്ഞാ(ptr2 == slot && ptr != slot);
            }
            നിർത്തുക;
        }

        സ്ഥിരസ്ഥിതി:
            fail("Expected recursion to result in Zombie.");
        }
    } അല്ലെങ്കിൽ {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        പ്രതിജ്ഞാ(ptr != slot);

        എങ്കിൽ (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            പ്രതിജ്ഞാ(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        capSwapForDelete(ptr, slot);
    }
    തിരികെ EXCEPTION_NONE;
}

ശൂന്യം cteDeleteOne(cte_t *slot)
{
    word_t cap_type = cap_get_capType(slot->cap);
    എങ്കിൽ (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret UNUSED;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        പ്രതിജ്ഞാ(capRemovable(fc_ret.remainder, slot) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        emptySlot(slot, cap_null_cap_new());
    }
}

ശൂന്യം തിരുകുക_New_Cap(cte_t *parent, cte_t *slot, cap_t cap)
{
    cte_t *next;

    next = CTE_PTR(mdb_കണ്ണി_get_mdb_Next(parent->cteMDBNode));
    slot->cap = cap;
    slot->cteMDBNode = mdb_കണ്ണി_new(CTE_REF(next), true, true, CTE_REF(parent));
    എങ്കിൽ (next) {
        mdb_കണ്ണി_ptr_set_mdb_Prev(&next->cteMDBNode, CTE_REF(slot));
    }
    mdb_കണ്ണി_ptr_set_mdb_Next(&parent->cteMDBNode, CTE_REF(slot));
}

#ifndef CONFIG_KERNEL_MCS
ശൂന്യം setupReplyMaster(tcb_t *thread)
{
    cte_t *slot;

    slot = TCB_PTR_CTE_PTR(thread, tcbReply);
    എങ്കിൽ (cap_get_capType(slot->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        slot->cap = cap_reply_cap_new(true, true, TCB_REF(thread));
        slot->cteMDBNode = nullMDBNode;
        mdb_കണ്ണി_ptr_set_mdb_Revocable(&slot->cteMDBNode, true);
        mdb_കണ്ണി_ptr_set_mdb_First_Badged(&slot->cteMDBNode, true);
    }
}
#endif

bool_t PURE isMDBParentOf(cte_t *cte_a, cte_t *cte_b)
{
    എങ്കിൽ (!mdb_കണ്ണി_get_mdb_Revocable(cte_a->cteMDBNode)) {
        തിരികെ false;
    }
    എങ്കിൽ (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        തിരികെ false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    എങ്കിൽ (!Arch_isMDBParentOf(cte_a->cap, cte_b->cap,
                            mdb_കണ്ണി_get_mdb_First_Badged(cte_b->cteMDBNode))) {
        തിരികെ false;
    }
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cte_a->cap)) {
    അവസ്ഥ cap_endpoint_cap: {
        word_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        എങ്കിൽ (badge == 0) {
            തിരികെ true;
        }
        തിരികെ (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_കണ്ണി_get_mdb_First_Badged(cte_b->cteMDBNode);
        നിർത്തുക;
    }

    അവസ്ഥ cap_notification_cap: {
        word_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        എങ്കിൽ (badge == 0) {
            തിരികെ true;
        }
        തിരികെ (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_കണ്ണി_get_mdb_First_Badged(cte_b->cteMDBNode);
        നിർത്തുക;
    }

    സ്ഥിരസ്ഥിതി:
        തിരികെ true;
        നിർത്തുക;
    }
}

exception_t ensureNoChildren(cte_t *slot)
{
    എങ്കിൽ (mdb_കണ്ണി_get_mdb_Next(slot->cteMDBNode) != 0) {
        cte_t *next;

        next = CTE_PTR(mdb_കണ്ണി_get_mdb_Next(slot->cteMDBNode));
        എങ്കിൽ (isMDBParentOf(slot, next)) {
            current_syscall_error.type = seL4_RevokeFirst;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
    }

    തിരികെ EXCEPTION_NONE;
}

exception_t ensureEmptySlot(cte_t *slot)
{
    എങ്കിൽ (cap_get_capType(slot->cap) != cap_null_cap) {
        current_syscall_error.type = seL4_DeleteFirst;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    തിരികെ EXCEPTION_NONE;
}

bool_t PURE isFinalCapability(cte_t *cte)
{
    mdb_കണ്ണി_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    എങ്കിൽ (mdb_കണ്ണി_get_mdb_Prev(mdb) == 0) {
        prevIsSameObject = false;
    } അല്ലെങ്കിൽ {
        cte_t *prev;

        prev = CTE_PTR(mdb_കണ്ണി_get_mdb_Prev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    എങ്കിൽ (prevIsSameObject) {
        തിരികെ false;
    } അല്ലെങ്കിൽ {
        എങ്കിൽ (mdb_കണ്ണി_get_mdb_Next(mdb) == 0) {
            തിരികെ true;
        } അല്ലെങ്കിൽ {
            cte_t *next;

            next = CTE_PTR(mdb_കണ്ണി_get_mdb_Next(mdb));
            തിരികെ !sameObjectAs(cte->cap, next->cap);
        }
    }
}

bool_t PURE slotCapLongRunningDelete(cte_t *slot)
{
    എങ്കിൽ (cap_get_capType(slot->cap) == cap_null_cap) {
        തിരികെ false;
    } അല്ലെങ്കിൽ എങ്കിൽ (! isFinalCapability(slot)) {
        തിരികെ false;
    }
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(slot->cap)) {
    അവസ്ഥ cap_thread_cap:
    അവസ്ഥ cap_zombie_cap:
    അവസ്ഥ cap_cnode_cap:
        തിരികെ true;
    സ്ഥിരസ്ഥിതി:
        തിരികെ false;
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

    എങ്കിൽ (!buffer) {
        തിരികെ ശൂന്യ;
    }

    ct = loadCapTransfer(buffer);
    cptr = ct.ct_Receive_വേര്;

    luc_ret = lookupCap(thread, cptr);
    എങ്കിൽ (luc_ret.status != EXCEPTION_NONE) {
        തിരികെ ശൂന്യ;
    }
    cnode = luc_ret.cap;

    lus_ret = lookupTargetSlot(cnode, ct.ctReceiveIndex, ct.ctReceiveDepth);
    എങ്കിൽ (lus_ret.status != EXCEPTION_NONE) {
        തിരികെ ശൂന്യ;
    }
    slot = lus_ret.slot;

    എങ്കിൽ (cap_get_capType(slot->cap) != cap_null_cap) {
        തിരികെ ശൂന്യ;
    }

    തിരികെ slot;
}

cap_transfer_t PURE loadCapTransfer(word_t *buffer)
{
    സ്ഥിരം പൂർണ്ണസംഖ്യ offset = se_L4_സന്ദേശം_Max_Length + se_L4_സന്ദേശം_Max_Extra_Caps + 2;
    തിരികെ cap_Transfer_From_വാക്കുകൾ(buffer + offset);
}
