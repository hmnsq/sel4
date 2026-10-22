/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <types.h>
#অন্তর্ভাব <object.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <kernel/cspace.h>
#অন্তর্ভাব <model/statedata.h>
#অন্তর্ভাব <arch/machine.h>

lookupCap_ret_t lookupCap(tcb_t *thread, cptr_t cPtr)
{
    lookupSlot_raw_ret_t lu_ret;
    lookupCap_ret_t ret;

    lu_ret = lookupSlot(thread, cPtr);
    যদি (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        ret.status = lu_ret.status;
        ret.cap = cap_null_cap_new();
        প্রত্যাগম ret;
    }

    ret.status = EXCEPTION_NONE;
    ret.cap = lu_ret.slot->cap;
    প্রত্যাগম ret;
}

lookupCapAndSlot_ret_t lookupCapAndSlot(tcb_t *thread, cptr_t cPtr)
{
    lookupSlot_raw_ret_t lu_ret;
    lookupCapAndSlot_ret_t ret;

    lu_ret = lookupSlot(thread, cPtr);
    যদি (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        ret.status = lu_ret.status;
        ret.slot = রিক্ত;
        ret.cap = cap_null_cap_new();
        প্রত্যাগম ret;
    }

    ret.status = EXCEPTION_NONE;
    ret.slot = lu_ret.slot;
    ret.cap = lu_ret.slot->cap;
    প্রত্যাগম ret;
}

lookupSlot_raw_ret_t lookupSlot(tcb_t *thread, cptr_t capptr)
{
    cap_t thread_মূল;
    resolveAddressBits_ret_t res_ret;
    lookupSlot_raw_ret_t ret;

    thread_মূল = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;
    res_ret = resolveAddressBits(thread_মূল, capptr, wordBits);

    ret.status = res_ret.status;
    ret.slot = res_ret.slot;
    প্রত্যাগম ret;
}

lookupSlot_ret_t lookupSlotForCNodeOp(bool_t isSource, cap_t মূল, cptr_t capptr,
                                      word_t depth)
{
    resolveAddressBits_ret_t res_ret;
    lookupSlot_ret_t ret;

    ret.slot = রিক্ত;

    যদি (unlikely(cap_get_capType(মূল) != cap_cnode_cap)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        current_lookup_fault = lookup_fault_invalid_মূল_new();
        ret.status = EXCEPTION_SYSCALL_ERROR;
        প্রত্যাগম ret;
    }

    যদি (unlikely(depth < 1 || depth > wordBits)) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = wordBits;
        ret.status = EXCEPTION_SYSCALL_ERROR;
        প্রত্যাগম ret;
    }
    res_ret = resolveAddressBits(মূল, capptr, depth);
    যদি (unlikely(res_ret.status != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        /* current_lookup_fault will have been set by resolveAddressBits */
        ret.status = EXCEPTION_SYSCALL_ERROR;
        প্রত্যাগম ret;
    }

    যদি (unlikely(res_ret.bitsRemaining != 0)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        current_lookup_fault =
            lookup_fault_depth_mismatch_new(0, res_ret.bitsRemaining);
        ret.status = EXCEPTION_SYSCALL_ERROR;
        প্রত্যাগম ret;
    }

    ret.slot = res_ret.slot;
    ret.status = EXCEPTION_NONE;
    প্রত্যাগম ret;
}

lookupSlot_ret_t lookupSourceSlot(cap_t মূল, cptr_t capptr, word_t depth)
{
    প্রত্যাগম lookupSlotForCNodeOp(true, মূল, capptr, depth);
}

lookupSlot_ret_t lookupTargetSlot(cap_t মূল, cptr_t capptr, word_t depth)
{
    প্রত্যাগম lookupSlotForCNodeOp(false, মূল, capptr, depth);
}

lookupSlot_ret_t lookupPivotSlot(cap_t মূল, cptr_t capptr, word_t depth)
{
    প্রত্যাগম lookupSlotForCNodeOp(true, মূল, capptr, depth);
}

resolveAddressBits_ret_t resolveAddressBits(cap_t গ্রন্থি_Cap, cptr_t capptr, word_t n_bits)
{
    resolveAddressBits_ret_t ret;
    word_t radixBits, guardBits, levelBits, guard;
    word_t capGuard, offset;
    cte_t *slot;

    ret.bitsRemaining = n_bits;
    ret.slot = রিক্ত;

    যদি (unlikely(cap_get_capType(গ্রন্থি_Cap) != cap_cnode_cap)) {
        current_lookup_fault = lookup_fault_invalid_মূল_new();
        ret.status = EXCEPTION_LOOKUP_FAULT;
        প্রত্যাগম ret;
    }

    যাবত্ (1) {
        radixBits = cap_cnode_cap_get_capCNodeRadix(গ্রন্থি_Cap);
        guardBits = cap_cnode_cap_get_capCNodeGuardSize(গ্রন্থি_Cap);
        levelBits = radixBits + guardBits;

        /* Haskell error: "All CNodes must resolve bits" */
        প্রতিজ্ঞা(levelBits != 0);

        capGuard = cap_cnode_cap_get_capCNodeGuard(গ্রন্থি_Cap);

        /* The MASK(wordRadix) here is to avoid the case where
         * n_bits = wordBits (=2^wordRadix) and guardBits = 0, as it violates
         * the C spec to shift right by more than wordBits-1.
         */
        guard = (capptr >> ((n_bits - guardBits) & MASK(wordRadix))) & MASK(guardBits);
        যদি (unlikely(guardBits > n_bits || guard != capGuard)) {
            current_lookup_fault =
                lookup_fault_guard_mismatch_new(capGuard, n_bits, guardBits);
            ret.status = EXCEPTION_LOOKUP_FAULT;
            প্রত্যাগম ret;
        }

        যদি (unlikely(levelBits > n_bits)) {
            current_lookup_fault =
                lookup_fault_depth_mismatch_new(levelBits, n_bits);
            ret.status = EXCEPTION_LOOKUP_FAULT;
            প্রত্যাগম ret;
        }

        offset = (capptr >> (n_bits - levelBits)) & MASK(radixBits);
        slot = CTE_PTR(cap_cnode_cap_get_capCNodePtr(গ্রন্থি_Cap)) + offset;

        যদি (likely(n_bits == levelBits)) {
            ret.status = EXCEPTION_NONE;
            ret.slot = slot;
            ret.bitsRemaining = 0;
            প্রত্যাগম ret;
        }

        /** GHOSTUPD: "(\<acute>levelBits > 0, id)" */

        n_bits -= levelBits;
        গ্রন্থি_Cap = slot->cap;

        যদি (unlikely(cap_get_capType(গ্রন্থি_Cap) != cap_cnode_cap)) {
            ret.status = EXCEPTION_NONE;
            ret.slot = slot;
            ret.bitsRemaining = n_bits;
            প্রত্যাগম ret;
        }
    }
}
