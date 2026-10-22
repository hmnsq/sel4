/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <assert.h>
#include <types.h>
#include <api/failures.h>
#include <api/invocation.h>
#include <api/syscall.h>
#include <api/types.h>
#include <machine/io.h>
#include <object/structures.h>
#include <object/objecttype.h>
#include <object/cnode.h>
#include <object/interrupt.h>
#include <object/untyped.h>
#include <kernel/cspace.h>
#include <kernel/thread.h>
#include <model/preemption.h>
#include <model/statedata.h>
#include <util.h>

struct finaliseSlot_ret {
    exception_t status;
    bool_t success;
    cap_t cleanupInfo;
};
typedef struct finaliseSlot_ret finaliseSlot_ret_t;

static finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t exposed);
static void emptySlot(cte_t *slot, cap_t cleanupInfo);
static exception_t reduceZombie(cte_t *slot, bool_t exposed);

#ifdef CONFIG_KERNEL_MCS
#define CNODE_LAST_INVOCATION CNodeRotate
#không_thì
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
    assert(cap_get_capType(cap) == cap_cnode_cap);

    nếu (invLabel < CNodeRevoke || invLabel > CNODE_LAST_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        trả_về EXCEPTION_SYSCALL_ERROR;
    }

    nếu (length < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        trả_về EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    w_bits = getSyscallArg(1, buffer);

    lu_ret = lookupTargetSlot(cap, index, w_bits);
    nếu (lu_ret.status != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        trả_về lu_ret.status;
    }
    destSlot = lu_ret.slot;

    nếu (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *srcSlot;
        word_t srcIndex, srcDepth, capData;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t srcRoot, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        nếu (length < 4 || current_extra_caps.excaprefs[0] == NULL) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            trả_về EXCEPTION_SYSCALL_ERROR;
        }
        srcIndex = getSyscallArg(2, buffer);
        srcDepth = getSyscallArg(3, buffer);

        srcRoot = current_extra_caps.excaprefs[0]->cap;

        status = ensureEmptySlot(destSlot);
        nếu (status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            trả_về status;
        }

        lu_ret = lookupSourceSlot(srcRoot, srcIndex, srcDepth);
        nếu (lu_ret.status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            trả_về lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        nếu (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(srcDepth);
            trả_về EXCEPTION_SYSCALL_ERROR;
        }

        switch (invLabel) {
        case CNodeCopy:

            nếu (length < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = seL4_TruncatedMessage;
                trả_về EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot, srcCap);
            nếu (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                trả_về dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            break;

        case CNodeMint:
            nếu (length < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                trả_về EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, buffer));
            capData = getSyscallArg(5, buffer);
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot,
                               updateCapData(false, capData, srcCap));
            nếu (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                trả_về dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            break;

        case CNodeMove:
            newCap = srcSlot->cap;
            isMove = true;

            break;

        case CNodeMutate:
            nếu (length < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                trả_về EXCEPTION_SYSCALL_ERROR;
            }

            capData = getSyscallArg(4, buffer);
            newCap = updateCapData(true, capData, srcSlot->cap);
            isMove = true;

            break;

        default:
            assert(0);
            trả_về EXCEPTION_NONE;
        }

        nếu (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            trả_về EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        nếu (isMove) {
            trả_về invokeCNodeMove(newCap, srcSlot, destSlot);
        } không_thì {
            trả_về invokeCNodeInsert(newCap, srcSlot, destSlot);
        }
    }

    nếu (invLabel == CNodeRevoke) {
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        trả_về invokeCNodeRevoke(destSlot);
    }

    nếu (invLabel == CNodeDelete) {
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        trả_về invokeCNodeDelete(destSlot);
    }

#ifndef CONFIG_KERNEL_MCS
    nếu (invLabel == CNodeSaveCaller) {
        status = ensureEmptySlot(destSlot);
        nếu (status != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            trả_về status;
        }

        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        trả_về invokeCNodeSaveCaller(destSlot);
    }
#endif

    nếu (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = destSlot->cap;

        nếu (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            trả_về EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        trả_về invokeCNodeCancelBadgedSends(destCap);
    }

    nếu (invLabel == CNodeRotate) {
        word_t pivotNewData, pivotIndex, pivotDepth;
        word_t srcNewData, srcIndex, srcDepth;
        cte_t *pivotSlot, *srcSlot;
        cap_t pivotRoot, srcRoot, newSrcCap, newPivotCap;

        nếu (length < 8 || current_extra_caps.excaprefs[0] == NULL
            || current_extra_caps.excaprefs[1] == NULL) {
            current_syscall_error.type = seL4_TruncatedMessage;
            trả_về EXCEPTION_SYSCALL_ERROR;
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
        nếu (lu_ret.status != EXCEPTION_NONE) {
            trả_về lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        lu_ret = lookupPivotSlot(pivotRoot, pivotIndex, pivotDepth);
        nếu (lu_ret.status != EXCEPTION_NONE) {
            trả_về lu_ret.status;
        }
        pivotSlot = lu_ret.slot;

        nếu (pivotSlot == srcSlot || pivotSlot == destSlot) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            trả_về EXCEPTION_SYSCALL_ERROR;
        }

        nếu (srcSlot != destSlot) {
            status = ensureEmptySlot(destSlot);
            nếu (status != EXCEPTION_NONE) {
                trả_về status;
            }
        }

        nếu (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(srcDepth);
            trả_về EXCEPTION_SYSCALL_ERROR;
        }

        nếu (cap_get_capType(pivotSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(pivotDepth);
            trả_về EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = updateCapData(true, srcNewData, srcSlot->cap);
        newPivotCap = updateCapData(true, pivotNewData, pivotSlot->cap);

        nếu (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            trả_về EXCEPTION_SYSCALL_ERROR;
        }

        nếu (cap_get_capType(newPivotCap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            trả_về EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        trả_về invokeCNodeRotate(newSrcCap, newPivotCap,
                                 srcSlot, pivotSlot, destSlot);
    }

    trả_về EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *destSlot)
{
    trả_về cteRevoke(destSlot);
}

exception_t invokeCNodeDelete(cte_t *destSlot)
{
    trả_về cteDelete(destSlot, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    word_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    nếu (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    trả_về EXCEPTION_NONE;
}

exception_t invokeCNodeInsert(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cteInsert(cap, srcSlot, destSlot);

    trả_về EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cteMove(cap, srcSlot, destSlot);

    trả_về EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    nếu (slot1 == slot3) {
        cteSwap(cap1, slot1, cap2, slot2);
    } không_thì {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    trả_về EXCEPTION_NONE;
}

#ifndef CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *destSlot)
{
    cap_t cap;
    cte_t *srcSlot;

    srcSlot = TCB_PTR_CTE_PTR(NODE_STATE(ksCurThread), tcbCaller);
    cap = srcSlot->cap;

    switch (cap_get_capType(cap)) {
    case cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        break;

    case cap_reply_cap:
        nếu (!cap_reply_cap_get_capReplyMaster(cap)) {
            cteMove(cap, srcSlot, destSlot);
        }
        break;

    default:
        fail("caller capability must be null or reply");
        break;
    }

    trả_về EXCEPTION_NONE;
}
#endif

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
static void setUntypedCapAsFull(cap_t srcCap, cap_t newCap, cte_t *srcSlot)
{
    nếu ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        nếu ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_capBlockSize(newCap)
                == cap_untyped_cap_get_capBlockSize(srcCap))) {
            cap_untyped_cap_ptr_set_capFreeIndex(&(srcSlot->cap),
                                                 MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(srcCap)));
        }
    }
}

void cteInsert(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
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
    assert(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    assert((cte_t *)mdb_node_get_mdbNext(destSlot->cteMDBNode) == NULL &&
           (cte_t *)mdb_node_get_mdbPrev(destSlot->cteMDBNode) == NULL);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    setUntypedCapAsFull(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSlot->cteMDBNode = newMDB;
    mdb_node_ptr_set_mdbNext(&srcSlot->cteMDBNode, CTE_REF(destSlot));
    nếu (mdb_node_get_mdbNext(newMDB)) {
        mdb_node_ptr_set_mdbPrev(
            &CTE_PTR(mdb_node_get_mdbNext(newMDB))->cteMDBNode,
            CTE_REF(destSlot));
    }
}

void cteMove(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_node_t mdb;
    word_t prev_ptr, next_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    assert(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    assert((cte_t *)mdb_node_get_mdbNext(destSlot->cteMDBNode) == NULL &&
           (cte_t *)mdb_node_get_mdbPrev(destSlot->cteMDBNode) == NULL);

    mdb = srcSlot->cteMDBNode;
    destSlot->cap = newCap;
    srcSlot->cap = cap_null_cap_new();
    destSlot->cteMDBNode = mdb;
    srcSlot->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_node_get_mdbPrev(mdb);
    nếu (prev_ptr)
        mdb_node_ptr_set_mdbNext(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(destSlot));

    next_ptr = mdb_node_get_mdbNext(mdb);
    nếu (next_ptr)
        mdb_node_ptr_set_mdbPrev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(destSlot));
}

void capSwapForDelete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    nếu (slot1 == slot2) {
        trả_về;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cteSwap(cap1, slot1, cap2, slot2);
}

void cteSwap(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_node_t mdb1, mdb2;
    word_t next_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_node_get_mdbPrev(mdb1);
    nếu (prev_ptr)
        mdb_node_ptr_set_mdbNext(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    next_ptr = mdb_node_get_mdbNext(mdb1);
    nếu (next_ptr)
        mdb_node_ptr_set_mdbPrev(
            &CTE_PTR(next_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_node_get_mdbPrev(mdb2);
    nếu (prev_ptr)
        mdb_node_ptr_set_mdbNext(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    next_ptr = mdb_node_get_mdbNext(mdb2);
    nếu (next_ptr)
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
    cho (nextPtr = CTE_PTR(mdb_node_get_mdbNext(slot->cteMDBNode));
         nextPtr && isMDBParentOf(slot, nextPtr);
         nextPtr = CTE_PTR(mdb_node_get_mdbNext(slot->cteMDBNode))) {
        status = cteDelete(nextPtr, true);
        nếu (status != EXCEPTION_NONE) {
            trả_về status;
        }

        status = preemptionPoint();
        nếu (status != EXCEPTION_NONE) {
            trả_về status;
        }
    }

    trả_về EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *slot, bool_t exposed)
{
    finaliseSlot_ret_t fs_ret;

    fs_ret = finaliseSlot(slot, exposed);
    nếu (fs_ret.status != EXCEPTION_NONE) {
        trả_về fs_ret.status;
    }

    nếu (exposed || fs_ret.success) {
        emptySlot(slot, fs_ret.cleanupInfo);
    }
    trả_về EXCEPTION_NONE;
}

static void emptySlot(cte_t *slot, cap_t cleanupInfo)
{
    nếu (cap_get_capType(slot->cap) != cap_null_cap) {
        mdb_node_t mdbNode;
        cte_t *prev, *next;

        mdbNode = slot->cteMDBNode;
        prev = CTE_PTR(mdb_node_get_mdbPrev(mdbNode));
        next = CTE_PTR(mdb_node_get_mdbNext(mdbNode));

        nếu (prev) {
            mdb_node_ptr_set_mdbNext(&prev->cteMDBNode, CTE_REF(next));
        }
        nếu (next) {
            mdb_node_ptr_set_mdbPrev(&next->cteMDBNode, CTE_REF(prev));
        }
        nếu (next)
            mdb_node_ptr_set_mdbFirstBadged(&next->cteMDBNode,
                                            mdb_node_get_mdbFirstBadged(next->cteMDBNode) ||
                                            mdb_node_get_mdbFirstBadged(mdbNode));
        slot->cap = cap_null_cap_new();
        slot->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

static inline bool_t CONST capRemovable(cap_t cap, cte_t *slot)
{
    switch (cap_get_capType(cap)) {
    case cap_null_cap:
        trả_về true;
    case cap_zombie_cap: {
        word_t n = cap_zombie_cap_get_capZombieNumber(cap);
        cte_t *z_slot = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        trả_về (n == 0 || (n == 1 && slot == z_slot));
    }
    default:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

static inline bool_t CONST capCyclicZombie(cap_t cap, cte_t *slot)
{
    trả_về cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == slot;
}

static finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t immediate)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t status;
    finaliseSlot_ret_t ret;

    trong_khi (cap_get_capType(slot->cap) != cap_null_cap) {
        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, false);

        nếu (capRemovable(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            trả_về ret;
        }

        slot->cap = fc_ret.remainder;

        nếu (!immediate && capCyclicZombie(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            trả_về ret;
        }

        status = reduceZombie(slot, immediate);
        nếu (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            trả_về ret;
        }

        status = preemptionPoint();
        nếu (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            trả_về ret;
        }
    }
    ret.status = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    trả_về ret;
}

static exception_t reduceZombie(cte_t *slot, bool_t immediate)
{
    cte_t *ptr;
    word_t n, type;
    exception_t status;

    assert(cap_get_capType(slot->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);
    n = cap_zombie_cap_get_capZombieNumber(slot->cap);
    type = cap_zombie_cap_get_capZombieType(slot->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    assert(n > 0);

    nếu (immediate) {
        cte_t *endSlot = &ptr[n - 1];

        status = cteDelete(endSlot, false);
        nếu (status != EXCEPTION_NONE) {
            trả_về status;
        }

        switch (cap_get_capType(slot->cap)) {
        case cap_null_cap:
            break;

        case cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);

            nếu (ptr == ptr2 &&
                cap_zombie_cap_get_capZombieNumber(slot->cap) == n &&
                cap_zombie_cap_get_capZombieType(slot->cap) == type) {
                assert(cap_get_capType(endSlot->cap) == cap_null_cap);
                slot->cap =
                    cap_zombie_cap_set_capZombieNumber(slot->cap, n - 1);
            } không_thì {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                assert(ptr2 == slot && ptr != slot);
            }
            break;
        }

        default:
            fail("Expected recursion to result in Zombie.");
        }
    } không_thì {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        assert(ptr != slot);

        nếu (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            assert(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        capSwapForDelete(ptr, slot);
    }
    trả_về EXCEPTION_NONE;
}

void cteDeleteOne(cte_t *slot)
{
    word_t cap_type = cap_get_capType(slot->cap);
    nếu (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret UNUSED;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        assert(capRemovable(fc_ret.remainder, slot) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        emptySlot(slot, cap_null_cap_new());
    }
}

void insertNewCap(cte_t *parent, cte_t *slot, cap_t cap)
{
    cte_t *next;

    next = CTE_PTR(mdb_node_get_mdbNext(parent->cteMDBNode));
    slot->cap = cap;
    slot->cteMDBNode = mdb_node_new(CTE_REF(next), true, true, CTE_REF(parent));
    nếu (next) {
        mdb_node_ptr_set_mdbPrev(&next->cteMDBNode, CTE_REF(slot));
    }
    mdb_node_ptr_set_mdbNext(&parent->cteMDBNode, CTE_REF(slot));
}

#ifndef CONFIG_KERNEL_MCS
void setupReplyMaster(tcb_t *thread)
{
    cte_t *slot;

    slot = TCB_PTR_CTE_PTR(thread, tcbReply);
    nếu (cap_get_capType(slot->cap) == cap_null_cap) {
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
    nếu (!mdb_node_get_mdbRevocable(cte_a->cteMDBNode)) {
        trả_về false;
    }
    nếu (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        trả_về false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    nếu (!Arch_isMDBParentOf(cte_a->cap, cte_b->cap,
                            mdb_node_get_mdbFirstBadged(cte_b->cteMDBNode))) {
        trả_về false;
    }
    switch (cap_get_capType(cte_a->cap)) {
    case cap_endpoint_cap: {
        word_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        nếu (badge == 0) {
            trả_về true;
        }
        trả_về (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_node_get_mdbFirstBadged(cte_b->cteMDBNode);
        break;
    }

    case cap_notification_cap: {
        word_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        nếu (badge == 0) {
            trả_về true;
        }
        trả_về (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_node_get_mdbFirstBadged(cte_b->cteMDBNode);
        break;
    }

    default:
        trả_về true;
        break;
    }
}

exception_t ensureNoChildren(cte_t *slot)
{
    nếu (mdb_node_get_mdbNext(slot->cteMDBNode) != 0) {
        cte_t *next;

        next = CTE_PTR(mdb_node_get_mdbNext(slot->cteMDBNode));
        nếu (isMDBParentOf(slot, next)) {
            current_syscall_error.type = seL4_RevokeFirst;
            trả_về EXCEPTION_SYSCALL_ERROR;
        }
    }

    trả_về EXCEPTION_NONE;
}

exception_t ensureEmptySlot(cte_t *slot)
{
    nếu (cap_get_capType(slot->cap) != cap_null_cap) {
        current_syscall_error.type = seL4_DeleteFirst;
        trả_về EXCEPTION_SYSCALL_ERROR;
    }

    trả_về EXCEPTION_NONE;
}

bool_t PURE isFinalCapability(cte_t *cte)
{
    mdb_node_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    nếu (mdb_node_get_mdbPrev(mdb) == 0) {
        prevIsSameObject = false;
    } không_thì {
        cte_t *prev;

        prev = CTE_PTR(mdb_node_get_mdbPrev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    nếu (prevIsSameObject) {
        trả_về false;
    } không_thì {
        nếu (mdb_node_get_mdbNext(mdb) == 0) {
            trả_về true;
        } không_thì {
            cte_t *next;

            next = CTE_PTR(mdb_node_get_mdbNext(mdb));
            trả_về !sameObjectAs(cte->cap, next->cap);
        }
    }
}

bool_t PURE slotCapLongRunningDelete(cte_t *slot)
{
    nếu (cap_get_capType(slot->cap) == cap_null_cap) {
        trả_về false;
    } không_thì nếu (! isFinalCapability(slot)) {
        trả_về false;
    }
    switch (cap_get_capType(slot->cap)) {
    case cap_thread_cap:
    case cap_zombie_cap:
    case cap_cnode_cap:
        trả_về true;
    default:
        trả_về false;
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

    nếu (!buffer) {
        trả_về NULL;
    }

    ct = loadCapTransfer(buffer);
    cptr = ct.ctReceiveRoot;

    luc_ret = lookupCap(thread, cptr);
    nếu (luc_ret.status != EXCEPTION_NONE) {
        trả_về NULL;
    }
    cnode = luc_ret.cap;

    lus_ret = lookupTargetSlot(cnode, ct.ctReceiveIndex, ct.ctReceiveDepth);
    nếu (lus_ret.status != EXCEPTION_NONE) {
        trả_về NULL;
    }
    slot = lus_ret.slot;

    nếu (cap_get_capType(slot->cap) != cap_null_cap) {
        trả_về NULL;
    }

    trả_về slot;
}

cap_transfer_t PURE loadCapTransfer(word_t *buffer)
{
    const số_nguyên offset = seL4_MsgMaxLength + seL4_MsgMaxExtraCaps + 2;
    trả_về capTransferFromWords(buffer + offset);
}
