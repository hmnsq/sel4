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

    𐤀𐤌 (invLabel < CNodeRevoke || invLabel > CNODE_LAST_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    𐤀𐤌 (length < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    w_bits = getSyscallArg(1, buffer);

    lu_ret = lookupTargetSlot(cap, index, w_bits);
    𐤀𐤌 (lu_ret.status != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        أرجع lu_ret.status;
    }
    destSlot = lu_ret.slot;

    𐤀𐤌 (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *srcSlot;
        word_t srcIndex, srcDepth, capData;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t srcRoot, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        𐤀𐤌 (length < 4 || current_extra_caps.excaprefs[0] == عدم) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        srcIndex = getSyscallArg(2, buffer);
        srcDepth = getSyscallArg(3, buffer);

        srcRoot = current_extra_caps.excaprefs[0]->cap;

        status = ensureEmptySlot(destSlot);
        𐤀𐤌 (status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            أرجع status;
        }

        lu_ret = lookupSourceSlot(srcRoot, srcIndex, srcDepth);
        𐤀𐤌 (lu_ret.status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            أرجع lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        𐤀𐤌 (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(srcDepth);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        بدل (invLabel) {
        حالة CNodeCopy:

            𐤀𐤌 (length < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = seL4_TruncatedMessage;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot, srcCap);
            𐤀𐤌 (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                أرجع dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            اكسر;

        حالة CNodeMint:
            𐤀𐤌 (length < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            capData = getSyscallArg(5, buffer);
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot,
                               updateCapData(false, capData, srcCap));
            𐤀𐤌 (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                أرجع dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            اكسر;

        حالة CNodeMove:
            newCap = srcSlot->cap;
            isMove = true;

            اكسر;

        حالة CNodeMutate:
            𐤀𐤌 (length < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            capData = getSyscallArg(4, buffer);
            newCap = updateCapData(true, capData, srcSlot->cap);
            isMove = true;

            اكسر;

        افتراضي:
            أكد(0);
            أرجع EXCEPTION_NONE;
        }

        𐤀𐤌 (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        𐤀𐤌 (isMove) {
            أرجع invokeCNodeMove(newCap, srcSlot, destSlot);
        } وإلا {
            أرجع invokeCNodeInsert(newCap, srcSlot, destSlot);
        }
    }

    𐤀𐤌 (invLabel == CNodeRevoke) {
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeRevoke(destSlot);
    }

    𐤀𐤌 (invLabel == CNodeDelete) {
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeDelete(destSlot);
    }

#ifndef CONFIG_KERNEL_MCS
    𐤀𐤌 (invLabel == CNodeSaveCaller) {
        status = ensureEmptySlot(destSlot);
        𐤀𐤌 (status != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            أرجع status;
        }

        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeSaveCaller(destSlot);
    }
#endif

    𐤀𐤌 (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = destSlot->cap;

        𐤀𐤌 (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeCancelBadgedSends(destCap);
    }

    𐤀𐤌 (invLabel == CNodeRotate) {
        word_t pivotNewData, pivotIndex, pivotDepth;
        word_t srcNewData, srcIndex, srcDepth;
        cte_t *pivotSlot, *srcSlot;
        cap_t pivotRoot, srcRoot, newSrcCap, newPivotCap;

        𐤀𐤌 (length < 8 || current_extra_caps.excaprefs[0] == عدم
            || current_extra_caps.excaprefs[1] == عدم) {
            current_syscall_error.type = seL4_TruncatedMessage;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        pivotNewData = getSyscallArg(2, buffer);
        pivotIndex   = getSyscallArg(3, buffer);
        pivotDepth   = getSyscallArg(4, buffer);
        srcNewData   = getSyscallArg(5, buffer);
        srcIndex     = getSyscallArg(6, buffer);
        srcDepth     = getSyscallArg(7, buffer);

        pivotRoot = current_extra_caps.excaprefs[0]->cap;
        srcRoot   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookupSourceSlot(srcRoot, srcIndex, srcDepth);
        𐤀𐤌 (lu_ret.status != EXCEPTION_NONE) {
            أرجع lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        lu_ret = lookupPivotSlot(pivotRoot, pivotIndex, pivotDepth);
        𐤀𐤌 (lu_ret.status != EXCEPTION_NONE) {
            أرجع lu_ret.status;
        }
        pivotSlot = lu_ret.slot;

        𐤀𐤌 (pivotSlot == srcSlot || pivotSlot == destSlot) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        𐤀𐤌 (srcSlot != destSlot) {
            status = ensureEmptySlot(destSlot);
            𐤀𐤌 (status != EXCEPTION_NONE) {
                أرجع status;
            }
        }

        𐤀𐤌 (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(srcDepth);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        𐤀𐤌 (cap_get_capType(pivotSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(pivotDepth);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = updateCapData(true, srcNewData, srcSlot->cap);
        newPivotCap = updateCapData(true, pivotNewData, pivotSlot->cap);

        𐤀𐤌 (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        𐤀𐤌 (cap_get_capType(newPivotCap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeRotate(newSrcCap, newPivotCap,
                                 srcSlot, pivotSlot, destSlot);
    }

    أرجع EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *destSlot)
{
    أرجع cteRevoke(destSlot);
}

exception_t invokeCNodeDelete(cte_t *destSlot)
{
    أرجع cteDelete(destSlot, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    word_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    𐤀𐤌 (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    أرجع EXCEPTION_NONE;
}

exception_t invokeCNodeInsert(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cteInsert(cap, srcSlot, destSlot);

    أرجع EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cteMove(cap, srcSlot, destSlot);

    أرجع EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    𐤀𐤌 (slot1 == slot3) {
        cteSwap(cap1, slot1, cap2, slot2);
    } وإلا {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    أرجع EXCEPTION_NONE;
}

#ifndef CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *destSlot)
{
    cap_t cap;
    cte_t *srcSlot;

    srcSlot = TCB_PTR_CTE_PTR(NODE_STATE(ksCurThread), tcbCaller);
    cap = srcSlot->cap;

    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        اكسر;

    حالة cap_reply_cap:
        𐤀𐤌 (!cap_reply_cap_get_capReplyMaster(cap)) {
            cteMove(cap, srcSlot, destSlot);
        }
        اكسر;

    افتراضي:
        fail("caller capability must be null or reply");
        اكسر;
    }

    أرجع EXCEPTION_NONE;
}
#endif

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
ساكن فراغ setUntypedCapAsFull(cap_t srcCap, cap_t newCap, cte_t *srcSlot)
{
    𐤀𐤌 ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        𐤀𐤌 ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_capBlockSize(newCap)
                == cap_untyped_cap_get_capBlockSize(srcCap))) {
            cap_untyped_cap_ptr_set_capFreeIndex(&(srcSlot->cap),
                                                 MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(srcCap)));
        }
    }
}

فراغ cteInsert(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_node_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = srcSlot->cteMDBNode;
    srcCap = srcSlot->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_node_set_mdbPrev(srcMDB, CTE_REF(srcSlot));
    newMDB = mdb_node_set_mdbRevocable(newMDB, newCapIsRevocable);
    newMDB = mdb_node_set_mdbFirstBadged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    أكد(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    أكد((cte_t *)mdb_node_get_mdbNext(destSlot->cteMDBNode) == عدم &&
           (cte_t *)mdb_node_get_mdbPrev(destSlot->cteMDBNode) == عدم);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    setUntypedCapAsFull(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSlot->cteMDBNode = newMDB;
    mdb_node_ptr_set_mdbNext(&srcSlot->cteMDBNode, CTE_REF(destSlot));
    𐤀𐤌 (mdb_node_get_mdbNext(newMDB)) {
        mdb_node_ptr_set_mdbPrev(
            &CTE_PTR(mdb_node_get_mdbNext(newMDB))->cteMDBNode,
            CTE_REF(destSlot));
    }
}

فراغ cteMove(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_node_t mdb;
    word_t prev_ptr, next_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    أكد(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    أكد((cte_t *)mdb_node_get_mdbNext(destSlot->cteMDBNode) == عدم &&
           (cte_t *)mdb_node_get_mdbPrev(destSlot->cteMDBNode) == عدم);

    mdb = srcSlot->cteMDBNode;
    destSlot->cap = newCap;
    srcSlot->cap = cap_null_cap_new();
    destSlot->cteMDBNode = mdb;
    srcSlot->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_node_get_mdbPrev(mdb);
    𐤀𐤌 (prev_ptr)
        mdb_node_ptr_set_mdbNext(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(destSlot));

    next_ptr = mdb_node_get_mdbNext(mdb);
    𐤀𐤌 (next_ptr)
        mdb_node_ptr_set_mdbPrev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(destSlot));
}

فراغ capSwapForDelete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    𐤀𐤌 (slot1 == slot2) {
        أرجع;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cteSwap(cap1, slot1, cap2, slot2);
}

فراغ cteSwap(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_node_t mdb1, mdb2;
    word_t next_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_node_get_mdbPrev(mdb1);
    𐤀𐤌 (prev_ptr)
        mdb_node_ptr_set_mdbNext(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    next_ptr = mdb_node_get_mdbNext(mdb1);
    𐤀𐤌 (next_ptr)
        mdb_node_ptr_set_mdbPrev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_node_get_mdbPrev(mdb2);
    𐤀𐤌 (prev_ptr)
        mdb_node_ptr_set_mdbNext(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    next_ptr = mdb_node_get_mdbNext(mdb2);
    𐤀𐤌 (next_ptr)
        mdb_node_ptr_set_mdbPrev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *slot)
{
    cte_t *nextPtr;
    exception_t status;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    لكل (nextPtr = CTE_PTR(mdb_node_get_mdbNext(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_node_get_mdbNext(slot->cteMDBNode))) {
        status = cteDelete(nextPtr, true);
        𐤀𐤌 (status != EXCEPTION_NONE) {
            أرجع status;
        }

        status = preemptionPoint();
        𐤀𐤌 (status != EXCEPTION_NONE) {
            أرجع status;
        }
    }

    أرجع EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *slot, bool_t exposed)
{
    finaliseSlot_ret_t fs_ret;

    fs_ret = finaliseSlot(slot, exposed);
    𐤀𐤌 (fs_ret.status != EXCEPTION_NONE) {
        أرجع fs_ret.status;
    }

    𐤀𐤌 (exposed || fs_ret.success) {
        emptySlot(slot, fs_ret.cleanupInfo);
    }
    أرجع EXCEPTION_NONE;
}

ساكن فراغ emptySlot(cte_t *slot, cap_t cleanupInfo)
{
    𐤀𐤌 (cap_get_capType(slot->cap) != cap_null_cap) {
        mdb_node_t mdbNode;
        cte_t *prev, *next;

        mdbNode = slot->cteMDBNode;
        prev = CTE_PTR(mdb_node_get_mdbPrev(mdbNode));
        next = CTE_PTR(mdb_node_get_mdbNext(mdbNode));

        𐤀𐤌 (prev) {
            mdb_node_ptr_set_mdbNext(&prev->cteMDBNode, CTE_REF(next));
        }
        𐤀𐤌 (next) {
            mdb_node_ptr_set_mdbPrev(&next->cteMDBNode, CTE_REF(prev));
        }
        𐤀𐤌 (next)
            mdb_node_ptr_set_mdbFirstBadged(&next->cteMDBNode,
                                            mdb_node_get_mdbFirstBadged(next->cteMDBNode) ||
                                            mdb_node_get_mdbFirstBadged(mdbNode));
        slot->cap = cap_null_cap_new();
        slot->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

ساكن مضمن bool_t CONST capRemovable(cap_t cap, cte_t *slot)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
        أرجع true;
    حالة cap_zombie_cap: {
        word_t n = cap_zombie_cap_get_capZombieNumber(cap);
        cte_t *z_slot = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        أرجع (n == 0 || (n == 1 && slot == z_slot));
    }
    افتراضي:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

ساكن مضمن bool_t CONST capCyclicZombie(cap_t cap, cte_t *slot)
{
    أرجع cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == slot;
}

ساكن finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t immediate)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t status;
    finaliseSlot_ret_t ret;

    طالما (cap_get_capType(slot->cap) != cap_null_cap) {
        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, false);

        𐤀𐤌 (capRemovable(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            أرجع ret;
        }

        slot->cap = fc_ret.remainder;

        𐤀𐤌 (!immediate && capCyclicZombie(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            أرجع ret;
        }

        status = reduceZombie(slot, immediate);
        𐤀𐤌 (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            أرجع ret;
        }

        status = preemptionPoint();
        𐤀𐤌 (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            أرجع ret;
        }
    }
    ret.status = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    أرجع ret;
}

ساكن exception_t reduceZombie(cte_t *slot, bool_t immediate)
{
    cte_t *ptr;
    word_t n, type;
    exception_t status;

    أكد(cap_get_capType(slot->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);
    n = cap_zombie_cap_get_capZombieNumber(slot->cap);
    type = cap_zombie_cap_get_capZombieType(slot->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    أكد(n > 0);

    𐤀𐤌 (immediate) {
        cte_t *endSlot = &ptr[n - 1];

        status = cteDelete(endSlot, false);
        𐤀𐤌 (status != EXCEPTION_NONE) {
            أرجع status;
        }

        بدل (cap_get_capType(slot->cap)) {
        حالة cap_null_cap:
            اكسر;

        حالة cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);

            𐤀𐤌 (ptr == ptr2 &&
                cap_zombie_cap_get_capZombieNumber(slot->cap) == n &&
                cap_zombie_cap_get_capZombieType(slot->cap) == type) {
                أكد(cap_get_capType(endSlot->cap) == cap_null_cap);
                slot->cap =
                    cap_zombie_cap_set_capZombieNumber(slot->cap, n - 1);
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

        𐤀𐤌 (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            أكد(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        capSwapForDelete(ptr, slot);
    }
    أرجع EXCEPTION_NONE;
}

فراغ cteDeleteOne(cte_t *slot)
{
    word_t cap_type = cap_get_capType(slot->cap);
    𐤀𐤌 (cap_type != cap_null_cap) {
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

فراغ insertNewCap(cte_t *parent, cte_t *slot, cap_t cap)
{
    cte_t *next;

    next = CTE_PTR(mdb_node_get_mdbNext(parent->cteMDBNode));
    slot->cap = cap;
    slot->cteMDBNode = mdb_node_new(CTE_REF(next), true, true, CTE_REF(parent));
    𐤀𐤌 (next) {
        mdb_node_ptr_set_mdbPrev(&next->cteMDBNode, CTE_REF(slot));
    }
    mdb_node_ptr_set_mdbNext(&parent->cteMDBNode, CTE_REF(slot));
}

#ifndef CONFIG_KERNEL_MCS
فراغ setupReplyMaster(tcb_t *thread)
{
    cte_t *slot;

    slot = TCB_PTR_CTE_PTR(thread, tcbReply);
    𐤀𐤌 (cap_get_capType(slot->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        slot->cap = cap_reply_cap_new(true, true, TCB_REF(thread));
        slot->cteMDBNode = nullMDBNode;
        mdb_node_ptr_set_mdbRevocable(&slot->cteMDBNode, true);
        mdb_node_ptr_set_mdbFirstBadged(&slot->cteMDBNode, true);
    }
}
#endif

bool_t PURE isMDBParentOf(cte_t *cte_a, cte_t *cte_b)
{
    𐤀𐤌 (!mdb_node_get_mdbRevocable(cte_a->cteMDBNode)) {
        أرجع false;
    }
    𐤀𐤌 (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        أرجع false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    𐤀𐤌 (!Arch_isMDBParentOf(cte_a->cap, cte_b->cap,
                            mdb_node_get_mdbFirstBadged(cte_b->cteMDBNode))) {
        أرجع false;
    }
    بدل (cap_get_capType(cte_a->cap)) {
    حالة cap_endpoint_cap: {
        word_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        𐤀𐤌 (badge == 0) {
            أرجع true;
        }
        أرجع (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_node_get_mdbFirstBadged(cte_b->cteMDBNode);
        اكسر;
    }

    حالة cap_notification_cap: {
        word_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        𐤀𐤌 (badge == 0) {
            أرجع true;
        }
        أرجع (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_node_get_mdbFirstBadged(cte_b->cteMDBNode);
        اكسر;
    }

    افتراضي:
        أرجع true;
        اكسر;
    }
}

exception_t ensureNoChildren(cte_t *slot)
{
    𐤀𐤌 (mdb_node_get_mdbNext(slot->cteMDBNode) != 0) {
        cte_t *next;

        next = CTE_PTR(mdb_node_get_mdbNext(slot->cteMDBNode));
        𐤀𐤌 (isMDBParentOf(slot, next)) {
            current_syscall_error.type = seL4_RevokeFirst;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    أرجع EXCEPTION_NONE;
}

exception_t ensureEmptySlot(cte_t *slot)
{
    𐤀𐤌 (cap_get_capType(slot->cap) != cap_null_cap) {
        current_syscall_error.type = seL4_DeleteFirst;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

bool_t PURE isFinalCapability(cte_t *cte)
{
    mdb_node_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    𐤀𐤌 (mdb_node_get_mdbPrev(mdb) == 0) {
        prevIsSameObject = false;
    } وإلا {
        cte_t *prev;

        prev = CTE_PTR(mdb_node_get_mdbPrev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    𐤀𐤌 (prevIsSameObject) {
        أرجع false;
    } وإلا {
        𐤀𐤌 (mdb_node_get_mdbNext(mdb) == 0) {
            أرجع true;
        } وإلا {
            cte_t *next;

            next = CTE_PTR(mdb_node_get_mdbNext(mdb));
            أرجع !sameObjectAs(cte->cap, next->cap);
        }
    }
}

bool_t PURE slotCapLongRunningDelete(cte_t *slot)
{
    𐤀𐤌 (cap_get_capType(slot->cap) == cap_null_cap) {
        أرجع false;
    } وإلا 𐤀𐤌 (! isFinalCapability(slot)) {
        أرجع false;
    }
    بدل (cap_get_capType(slot->cap)) {
    حالة cap_thread_cap:
    حالة cap_zombie_cap:
    حالة cap_cnode_cap:
        أرجع true;
    افتراضي:
        أرجع false;
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

    𐤀𐤌 (!buffer) {
        أرجع عدم;
    }

    ct = loadCapTransfer(buffer);
    cptr = ct.ctReceiveRoot;

    luc_ret = lookupCap(thread, cptr);
    𐤀𐤌 (luc_ret.status != EXCEPTION_NONE) {
        أرجع عدم;
    }
    cnode = luc_ret.cap;

    lus_ret = lookupTargetSlot(cnode, ct.ctReceiveIndex, ct.ctReceiveDepth);
    𐤀𐤌 (lus_ret.status != EXCEPTION_NONE) {
        أرجع عدم;
    }
    slot = lus_ret.slot;

    𐤀𐤌 (cap_get_capType(slot->cap) != cap_null_cap) {
        أرجع عدم;
    }

    أرجع slot;
}

cap_transfer_t PURE loadCapTransfer(word_t *buffer)
{
    ثابت صحيح offset = seL4_MsgMaxLength + seL4_MsgMaxExtraCaps + 2;
    أرجع capTransferFromWords(buffer + offset);
}
