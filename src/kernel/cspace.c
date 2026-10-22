/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <types.h>
#அந்தர்பாவ <object.h>
#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <kernel/cspace.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <arch/machine.h>

lookupCap_ret_t lookupCap(tcb_t *thread, cptr_t cPtr)
{
    lookupSlot_raw_ret_t lu_ret;
    lookupCap_ret_t ret;

    lu_ret = lookupSlot(thread, cPtr);
    எனில் (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        ret.status = lu_ret.status;
        ret.cap = cap_null_cap_new();
        திரும்பு ret;
    }

    ret.status = EXCEPTION_NONE;
    ret.cap = lu_ret.slot->cap;
    திரும்பு ret;
}

lookupCapAndSlot_ret_t lookupCapAndSlot(tcb_t *thread, cptr_t cPtr)
{
    lookupSlot_raw_ret_t lu_ret;
    lookupCapAndSlot_ret_t ret;

    lu_ret = lookupSlot(thread, cPtr);
    எனில் (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        ret.status = lu_ret.status;
        ret.slot = வெற்று;
        ret.cap = cap_null_cap_new();
        திரும்பு ret;
    }

    ret.status = EXCEPTION_NONE;
    ret.slot = lu_ret.slot;
    ret.cap = lu_ret.slot->cap;
    திரும்பு ret;
}

lookupSlot_raw_ret_t lookupSlot(tcb_t *thread, cptr_t capptr)
{
    cap_t thread_வேர்;
    resolveAddressBits_ret_t res_ret;
    lookupSlot_raw_ret_t ret;

    thread_வேர் = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;
    res_ret = resolveAddressBits(thread_வேர், capptr, wordBits);

    ret.status = res_ret.status;
    ret.slot = res_ret.slot;
    திரும்பு ret;
}

lookupSlot_ret_t lookupSlotForCNodeOp(bool_t isSource, cap_t வேர், cptr_t capptr,
                                      word_t depth)
{
    resolveAddressBits_ret_t res_ret;
    lookupSlot_ret_t ret;

    ret.slot = வெற்று;

    எனில் (unlikely(cap_get_capType(வேர்) != cap_cnode_cap)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        current_lookup_fault = lookup_fault_invalid_வேர்_new();
        ret.status = EXCEPTION_SYSCALL_ERROR;
        திரும்பு ret;
    }

    எனில் (unlikely(depth < 1 || depth > wordBits)) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = wordBits;
        ret.status = EXCEPTION_SYSCALL_ERROR;
        திரும்பு ret;
    }
    res_ret = resolveAddressBits(வேர், capptr, depth);
    எனில் (unlikely(res_ret.status != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        /* current_lookup_fault will have been set by resolveAddressBits */
        ret.status = EXCEPTION_SYSCALL_ERROR;
        திரும்பு ret;
    }

    எனில் (unlikely(res_ret.bitsRemaining != 0)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        current_lookup_fault =
            lookup_fault_depth_mismatch_new(0, res_ret.bitsRemaining);
        ret.status = EXCEPTION_SYSCALL_ERROR;
        திரும்பு ret;
    }

    ret.slot = res_ret.slot;
    ret.status = EXCEPTION_NONE;
    திரும்பு ret;
}

lookupSlot_ret_t lookupSourceSlot(cap_t வேர், cptr_t capptr, word_t depth)
{
    திரும்பு lookupSlotForCNodeOp(true, வேர், capptr, depth);
}

lookupSlot_ret_t lookupTargetSlot(cap_t வேர், cptr_t capptr, word_t depth)
{
    திரும்பு lookupSlotForCNodeOp(false, வேர், capptr, depth);
}

lookupSlot_ret_t lookupPivotSlot(cap_t வேர், cptr_t capptr, word_t depth)
{
    திரும்பு lookupSlotForCNodeOp(true, வேர், capptr, depth);
}

resolveAddressBits_ret_t resolveAddressBits(cap_t முனை_Cap, cptr_t capptr, word_t n_bits)
{
    resolveAddressBits_ret_t ret;
    word_t radixBits, guardBits, levelBits, guard;
    word_t capGuard, offset;
    cte_t *slot;

    ret.bitsRemaining = n_bits;
    ret.slot = வெற்று;

    எனில் (unlikely(cap_get_capType(முனை_Cap) != cap_cnode_cap)) {
        current_lookup_fault = lookup_fault_invalid_வேர்_new();
        ret.status = EXCEPTION_LOOKUP_FAULT;
        திரும்பு ret;
    }

    போது (1) {
        radixBits = cap_cnode_cap_get_capCNodeRadix(முனை_Cap);
        guardBits = cap_cnode_cap_get_capCNodeGuardSize(முனை_Cap);
        levelBits = radixBits + guardBits;

        /* Haskell error: "All CNodes must resolve bits" */
        ப்ரதிஜ்ஞா(levelBits != 0);

        capGuard = cap_cnode_cap_get_capCNodeGuard(முனை_Cap);

        /* The MASK(wordRadix) here is to avoid the case where
         * n_bits = wordBits (=2^wordRadix) and guardBits = 0, as it violates
         * the C spec to shift right by more than wordBits-1.
         */
        guard = (capptr >> ((n_bits - guardBits) & MASK(wordRadix))) & MASK(guardBits);
        எனில் (unlikely(guardBits > n_bits || guard != capGuard)) {
            current_lookup_fault =
                lookup_fault_guard_mismatch_new(capGuard, n_bits, guardBits);
            ret.status = EXCEPTION_LOOKUP_FAULT;
            திரும்பு ret;
        }

        எனில் (unlikely(levelBits > n_bits)) {
            current_lookup_fault =
                lookup_fault_depth_mismatch_new(levelBits, n_bits);
            ret.status = EXCEPTION_LOOKUP_FAULT;
            திரும்பு ret;
        }

        offset = (capptr >> (n_bits - levelBits)) & MASK(radixBits);
        slot = CTE_PTR(cap_cnode_cap_get_capCNodePtr(முனை_Cap)) + offset;

        எனில் (likely(n_bits == levelBits)) {
            ret.status = EXCEPTION_NONE;
            ret.slot = slot;
            ret.bitsRemaining = 0;
            திரும்பு ret;
        }

        /** GHOSTUPD: "(\<acute>levelBits > 0, id)" */

        n_bits -= levelBits;
        முனை_Cap = slot->cap;

        எனில் (unlikely(cap_get_capType(முனை_Cap) != cap_cnode_cap)) {
            ret.status = EXCEPTION_NONE;
            ret.slot = slot;
            ret.bitsRemaining = n_bits;
            திரும்பு ret;
        }
    }
}
