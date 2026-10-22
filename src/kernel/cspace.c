/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <types.h>
#شامل <object.h>
#شامل <api/failures.h>
#شامل <kernel/thread.h>
#شامل <kernel/cspace.h>
#شامل <model/statedata.h>
#شامل <arch/machine.h>

lookupCap_ret_t lookupCap(tcb_t *thread, cptr_t cPtr)
{
    lookupSlot_raw_ret_t lu_ret;
    lookupCap_ret_t ret;

    lu_ret = lookupSlot(thread, cPtr);
    اگر (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        ret.status = lu_ret.status;
        ret.cap = cap_null_cap_new();
        بازگشت ret;
    }

    ret.status = EXCEPTION_NONE;
    ret.cap = lu_ret.slot->cap;
    بازگشت ret;
}

lookupCapAndSlot_ret_t lookupCapAndSlot(tcb_t *thread, cptr_t cPtr)
{
    lookupSlot_raw_ret_t lu_ret;
    lookupCapAndSlot_ret_t ret;

    lu_ret = lookupSlot(thread, cPtr);
    اگر (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        ret.status = lu_ret.status;
        ret.slot = پوچ;
        ret.cap = cap_null_cap_new();
        بازگشت ret;
    }

    ret.status = EXCEPTION_NONE;
    ret.slot = lu_ret.slot;
    ret.cap = lu_ret.slot->cap;
    بازگشت ret;
}

lookupSlot_raw_ret_t lookupSlot(tcb_t *thread, cptr_t capptr)
{
    cap_t thread_ریشه;
    resolveAddressBits_ret_t res_ret;
    lookupSlot_raw_ret_t ret;

    thread_ریشه = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;
    res_ret = resolveAddressBits(thread_ریشه, capptr, wordBits);

    ret.status = res_ret.status;
    ret.slot = res_ret.slot;
    بازگشت ret;
}

lookupSlot_ret_t lookupSlotForCNodeOp(bool_t isSource, cap_t ریشه, cptr_t capptr,
                                      word_t depth)
{
    resolveAddressBits_ret_t res_ret;
    lookupSlot_ret_t ret;

    ret.slot = پوچ;

    اگر (unlikely(cap_get_capType(ریشه) != cap_cnode_cap)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        current_lookup_fault = lookup_fault_invalid_ریشه_new();
        ret.status = EXCEPTION_SYSCALL_ERROR;
        بازگشت ret;
    }

    اگر (unlikely(depth < 1 || depth > wordBits)) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = wordBits;
        ret.status = EXCEPTION_SYSCALL_ERROR;
        بازگشت ret;
    }
    res_ret = resolveAddressBits(ریشه, capptr, depth);
    اگر (unlikely(res_ret.status != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        /* current_lookup_fault will have been set by resolveAddressBits */
        ret.status = EXCEPTION_SYSCALL_ERROR;
        بازگشت ret;
    }

    اگر (unlikely(res_ret.bitsRemaining != 0)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        current_lookup_fault =
            lookup_fault_depth_mismatch_new(0, res_ret.bitsRemaining);
        ret.status = EXCEPTION_SYSCALL_ERROR;
        بازگشت ret;
    }

    ret.slot = res_ret.slot;
    ret.status = EXCEPTION_NONE;
    بازگشت ret;
}

lookupSlot_ret_t lookupSourceSlot(cap_t ریشه, cptr_t capptr, word_t depth)
{
    بازگشت lookupSlotForCNodeOp(true, ریشه, capptr, depth);
}

lookupSlot_ret_t lookupTargetSlot(cap_t ریشه, cptr_t capptr, word_t depth)
{
    بازگشت lookupSlotForCNodeOp(false, ریشه, capptr, depth);
}

lookupSlot_ret_t lookupPivotSlot(cap_t ریشه, cptr_t capptr, word_t depth)
{
    بازگشت lookupSlotForCNodeOp(true, ریشه, capptr, depth);
}

resolveAddressBits_ret_t resolveAddressBits(cap_t گره_Cap, cptr_t capptr, word_t n_bits)
{
    resolveAddressBits_ret_t ret;
    word_t radixBits, guardBits, levelBits, guard;
    word_t capGuard, offset;
    cte_t *slot;

    ret.bitsRemaining = n_bits;
    ret.slot = پوچ;

    اگر (unlikely(cap_get_capType(گره_Cap) != cap_cnode_cap)) {
        current_lookup_fault = lookup_fault_invalid_ریشه_new();
        ret.status = EXCEPTION_LOOKUP_FAULT;
        بازگشت ret;
    }

    وقتی (1) {
        radixBits = cap_cnode_cap_get_capCNodeRadix(گره_Cap);
        guardBits = cap_cnode_cap_get_capCNodeGuardSize(گره_Cap);
        levelBits = radixBits + guardBits;

        /* Haskell error: "All CNodes must resolve bits" */
        أكد(levelBits != 0);

        capGuard = cap_cnode_cap_get_capCNodeGuard(گره_Cap);

        /* The MASK(wordRadix) here is to avoid the case where
         * n_bits = wordBits (=2^wordRadix) and guardBits = 0, as it violates
         * the C spec to shift right by more than wordBits-1.
         */
        guard = (capptr >> ((n_bits - guardBits) & MASK(wordRadix))) & MASK(guardBits);
        اگر (unlikely(guardBits > n_bits || guard != capGuard)) {
            current_lookup_fault =
                lookup_fault_guard_mismatch_new(capGuard, n_bits, guardBits);
            ret.status = EXCEPTION_LOOKUP_FAULT;
            بازگشت ret;
        }

        اگر (unlikely(levelBits > n_bits)) {
            current_lookup_fault =
                lookup_fault_depth_mismatch_new(levelBits, n_bits);
            ret.status = EXCEPTION_LOOKUP_FAULT;
            بازگشت ret;
        }

        offset = (capptr >> (n_bits - levelBits)) & MASK(radixBits);
        slot = CTE_PTR(cap_cnode_cap_get_capCNodePtr(گره_Cap)) + offset;

        اگر (likely(n_bits == levelBits)) {
            ret.status = EXCEPTION_NONE;
            ret.slot = slot;
            ret.bitsRemaining = 0;
            بازگشت ret;
        }

        /** GHOSTUPD: "(\<acute>levelBits > 0, id)" */

        n_bits -= levelBits;
        گره_Cap = slot->cap;

        اگر (unlikely(cap_get_capType(گره_Cap) != cap_cnode_cap)) {
            ret.status = EXCEPTION_NONE;
            ret.slot = slot;
            ret.bitsRemaining = n_bits;
            بازگشت ret;
        }
    }
}
