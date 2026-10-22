/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <object.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>

lookupCap_ret_t lookupCap(tcb_t *thread, cptr_t cPtr)
{
    lookupSlot_raw_ret_t lu_ret;
    lookupCap_ret_t ret;

    lu_ret = lookupSlot(thread, cPtr);
    ಯದಿ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        ret.status = lu_ret.status;
        ret.cap = cap_null_cap_new();
        ಪ್ರತ್ಯಾಗಮ ret;
    }

    ret.status = EXCEPTION_NONE;
    ret.cap = lu_ret.slot->cap;
    ಪ್ರತ್ಯಾಗಮ ret;
}

lookupCapAndSlot_ret_t lookupCapAndSlot(tcb_t *thread, cptr_t cPtr)
{
    lookupSlot_raw_ret_t lu_ret;
    lookupCapAndSlot_ret_t ret;

    lu_ret = lookupSlot(thread, cPtr);
    ಯದಿ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        ret.status = lu_ret.status;
        ret.slot = ರಿಕ್ತ;
        ret.cap = cap_null_cap_new();
        ಪ್ರತ್ಯಾಗಮ ret;
    }

    ret.status = EXCEPTION_NONE;
    ret.slot = lu_ret.slot;
    ret.cap = lu_ret.slot->cap;
    ಪ್ರತ್ಯಾಗಮ ret;
}

lookupSlot_raw_ret_t lookupSlot(tcb_t *thread, cptr_t capptr)
{
    cap_t thread_ಬೇರು;
    resolveAddressBits_ret_t res_ret;
    lookupSlot_raw_ret_t ret;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;
    res_ret = resolveAddressBits(thread_ಬೇರು, capptr, wordBits);

    ret.status = res_ret.status;
    ret.slot = res_ret.slot;
    ಪ್ರತ್ಯಾಗಮ ret;
}

lookupSlot_ret_t lookupSlotForCNodeOp(bool_t isSource, cap_t ಬೇರು, cptr_t capptr,
                                      word_t depth)
{
    resolveAddressBits_ret_t res_ret;
    lookupSlot_ret_t ret;

    ret.slot = ರಿಕ್ತ;

    ಯದಿ (unlikely(cap_get_capType(ಬೇರು) != cap_cnode_cap)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();
        ret.status = EXCEPTION_SYSCALL_ERROR;
        ಪ್ರತ್ಯಾಗಮ ret;
    }

    ಯದಿ (unlikely(depth < 1 || depth > wordBits)) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = wordBits;
        ret.status = EXCEPTION_SYSCALL_ERROR;
        ಪ್ರತ್ಯಾಗಮ ret;
    }
    res_ret = resolveAddressBits(ಬೇರು, capptr, depth);
    ಯದಿ (unlikely(res_ret.status != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        /* current_lookup_fault will have been set by resolveAddressBits */
        ret.status = EXCEPTION_SYSCALL_ERROR;
        ಪ್ರತ್ಯಾಗಮ ret;
    }

    ಯದಿ (unlikely(res_ret.bitsRemaining != 0)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        current_lookup_fault =
            lookup_fault_depth_mismatch_new(0, res_ret.bitsRemaining);
        ret.status = EXCEPTION_SYSCALL_ERROR;
        ಪ್ರತ್ಯಾಗಮ ret;
    }

    ret.slot = res_ret.slot;
    ret.status = EXCEPTION_NONE;
    ಪ್ರತ್ಯಾಗಮ ret;
}

lookupSlot_ret_t lookupSourceSlot(cap_t ಬೇರು, cptr_t capptr, word_t depth)
{
    ಪ್ರತ್ಯಾಗಮ lookupSlotForCNodeOp(true, ಬೇರು, capptr, depth);
}

lookupSlot_ret_t lookupTargetSlot(cap_t ಬೇರು, cptr_t capptr, word_t depth)
{
    ಪ್ರತ್ಯಾಗಮ lookupSlotForCNodeOp(false, ಬೇರು, capptr, depth);
}

lookupSlot_ret_t lookupPivotSlot(cap_t ಬೇರು, cptr_t capptr, word_t depth)
{
    ಪ್ರತ್ಯಾಗಮ lookupSlotForCNodeOp(true, ಬೇರು, capptr, depth);
}

resolveAddressBits_ret_t resolveAddressBits(cap_t ಗಂಟು_Cap, cptr_t capptr, word_t n_bits)
{
    resolveAddressBits_ret_t ret;
    word_t radixBits, guardBits, levelBits, guard;
    word_t capGuard, offset;
    cte_t *slot;

    ret.bitsRemaining = n_bits;
    ret.slot = ರಿಕ್ತ;

    ಯದಿ (unlikely(cap_get_capType(ಗಂಟು_Cap) != cap_cnode_cap)) {
        current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ಪ್ರತ್ಯಾಗಮ ret;
    }

    ಯಾವತ್ (1) {
        radixBits = cap_cnode_cap_get_capCNodeRadix(ಗಂಟು_Cap);
        guardBits = cap_cnode_cap_get_capCNodeGuardSize(ಗಂಟು_Cap);
        levelBits = radixBits + guardBits;

        /* Haskell error: "All CNodes must resolve bits" */
        ಪ್ರತಿಜ್ಞಾ(levelBits != 0);

        capGuard = cap_cnode_cap_get_capCNodeGuard(ಗಂಟು_Cap);

        /* The MASK(wordRadix) here is to avoid the case where
         * n_bits = wordBits (=2^wordRadix) and guardBits = 0, as it violates
         * the C spec to shift right by more than wordBits-1.
         */
        guard = (capptr >> ((n_bits - guardBits) & MASK(wordRadix))) & MASK(guardBits);
        ಯದಿ (unlikely(guardBits > n_bits || guard != capGuard)) {
            current_lookup_fault =
                lookup_fault_guard_mismatch_new(capGuard, n_bits, guardBits);
            ret.status = EXCEPTION_LOOKUP_FAULT;
            ಪ್ರತ್ಯಾಗಮ ret;
        }

        ಯದಿ (unlikely(levelBits > n_bits)) {
            current_lookup_fault =
                lookup_fault_depth_mismatch_new(levelBits, n_bits);
            ret.status = EXCEPTION_LOOKUP_FAULT;
            ಪ್ರತ್ಯಾಗಮ ret;
        }

        offset = (capptr >> (n_bits - levelBits)) & MASK(radixBits);
        slot = CTE_PTR(cap_cnode_cap_get_capCNodePtr(ಗಂಟು_Cap)) + offset;

        ಯದಿ (likely(n_bits == levelBits)) {
            ret.status = EXCEPTION_NONE;
            ret.slot = slot;
            ret.bitsRemaining = 0;
            ಪ್ರತ್ಯಾಗಮ ret;
        }

        /** GHOSTUPD: "(\<acute>levelBits > 0, id)" */

        n_bits -= levelBits;
        ಗಂಟು_Cap = slot->cap;

        ಯದಿ (unlikely(cap_get_capType(ಗಂಟು_Cap) != cap_cnode_cap)) {
            ret.status = EXCEPTION_NONE;
            ret.slot = slot;
            ret.bitsRemaining = n_bits;
            ಪ್ರತ್ಯಾಗಮ ret;
        }
    }
}
