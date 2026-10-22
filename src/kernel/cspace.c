/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <object.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <kernel/cspace.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <arch/machine.h>

lookupCap_ret_t lookupCap(tcb_t *thread, cptr_t cPtr)
{
    lookupSlot_raw_ret_t lu_ret;
    lookupCap_ret_t ret;

    lu_ret = lookupSlot(thread, cPtr);
    जइ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        ret.status = lu_ret.status;
        ret.cap = cap_null_cap_new();
        प्रत्यागम ret;
    }

    ret.status = EXCEPTION_NONE;
    ret.cap = lu_ret.slot->cap;
    प्रत्यागम ret;
}

lookupCapAndSlot_ret_t lookupCapAndSlot(tcb_t *thread, cptr_t cPtr)
{
    lookupSlot_raw_ret_t lu_ret;
    lookupCapAndSlot_ret_t ret;

    lu_ret = lookupSlot(thread, cPtr);
    जइ (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        ret.status = lu_ret.status;
        ret.slot = रिक्त;
        ret.cap = cap_null_cap_new();
        प्रत्यागम ret;
    }

    ret.status = EXCEPTION_NONE;
    ret.slot = lu_ret.slot;
    ret.cap = lu_ret.slot->cap;
    प्रत्यागम ret;
}

lookupSlot_raw_ret_t lookupSlot(tcb_t *thread, cptr_t capptr)
{
    cap_t thread_मूलम्;
    resolveAddressBits_ret_t res_ret;
    lookupSlot_raw_ret_t ret;

    thread_मूलम् = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;
    res_ret = resolveAddressBits(thread_मूलम्, capptr, wordBits);

    ret.status = res_ret.status;
    ret.slot = res_ret.slot;
    प्रत्यागम ret;
}

lookupSlot_ret_t lookupSlotForCNodeOp(bool_t isSource, cap_t मूलम्, cptr_t capptr,
                                      word_t depth)
{
    resolveAddressBits_ret_t res_ret;
    lookupSlot_ret_t ret;

    ret.slot = रिक्त;

    जइ (unlikely(cap_get_capType(मूलम्) != cap_cnode_cap)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        current_lookup_fault = lookup_fault_invalid_मूलम्_new();
        ret.status = EXCEPTION_SYSCALL_ERROR;
        प्रत्यागम ret;
    }

    जइ (unlikely(depth < 1 || depth > wordBits)) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = wordBits;
        ret.status = EXCEPTION_SYSCALL_ERROR;
        प्रत्यागम ret;
    }
    res_ret = resolveAddressBits(मूलम्, capptr, depth);
    जइ (unlikely(res_ret.status != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        /* current_lookup_fault will have been set by resolveAddressBits */
        ret.status = EXCEPTION_SYSCALL_ERROR;
        प्रत्यागम ret;
    }

    जइ (unlikely(res_ret.bitsRemaining != 0)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        current_lookup_fault =
            lookup_fault_depth_mismatch_new(0, res_ret.bitsRemaining);
        ret.status = EXCEPTION_SYSCALL_ERROR;
        प्रत्यागम ret;
    }

    ret.slot = res_ret.slot;
    ret.status = EXCEPTION_NONE;
    प्रत्यागम ret;
}

lookupSlot_ret_t lookupSourceSlot(cap_t मूलम्, cptr_t capptr, word_t depth)
{
    प्रत्यागम lookupSlotForCNodeOp(true, मूलम्, capptr, depth);
}

lookupSlot_ret_t lookupTargetSlot(cap_t मूलम्, cptr_t capptr, word_t depth)
{
    प्रत्यागम lookupSlotForCNodeOp(false, मूलम्, capptr, depth);
}

lookupSlot_ret_t lookupPivotSlot(cap_t मूलम्, cptr_t capptr, word_t depth)
{
    प्रत्यागम lookupSlotForCNodeOp(true, मूलम्, capptr, depth);
}

resolveAddressBits_ret_t resolveAddressBits(cap_t ग्रन्थिः_Cap, cptr_t capptr, word_t n_bits)
{
    resolveAddressBits_ret_t ret;
    word_t radixBits, guardBits, levelBits, guard;
    word_t capGuard, offset;
    cte_t *slot;

    ret.bitsRemaining = n_bits;
    ret.slot = रिक्त;

    जइ (unlikely(cap_get_capType(ग्रन्थिः_Cap) != cap_cnode_cap)) {
        current_lookup_fault = lookup_fault_invalid_मूलम्_new();
        ret.status = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }

    जाव (1) {
        radixBits = cap_cnode_cap_get_capCNodeRadix(ग्रन्थिः_Cap);
        guardBits = cap_cnode_cap_get_capCNodeGuardSize(ग्रन्थिः_Cap);
        levelBits = radixBits + guardBits;

        /* Haskell error: "All CNodes must resolve bits" */
        प्रतिज्ञा(levelBits != 0);

        capGuard = cap_cnode_cap_get_capCNodeGuard(ग्रन्थिः_Cap);

        /* The MASK(wordRadix) here is to avoid the case where
         * n_bits = wordBits (=2^wordRadix) and guardBits = 0, as it violates
         * the C spec to shift right by more than wordBits-1.
         */
        guard = (capptr >> ((n_bits - guardBits) & MASK(wordRadix))) & MASK(guardBits);
        जइ (unlikely(guardBits > n_bits || guard != capGuard)) {
            current_lookup_fault =
                lookup_fault_guard_mismatch_new(capGuard, n_bits, guardBits);
            ret.status = EXCEPTION_LOOKUP_FAULT;
            प्रत्यागम ret;
        }

        जइ (unlikely(levelBits > n_bits)) {
            current_lookup_fault =
                lookup_fault_depth_mismatch_new(levelBits, n_bits);
            ret.status = EXCEPTION_LOOKUP_FAULT;
            प्रत्यागम ret;
        }

        offset = (capptr >> (n_bits - levelBits)) & MASK(radixBits);
        slot = CTE_PTR(cap_cnode_cap_get_capCNodePtr(ग्रन्थिः_Cap)) + offset;

        जइ (likely(n_bits == levelBits)) {
            ret.status = EXCEPTION_NONE;
            ret.slot = slot;
            ret.bitsRemaining = 0;
            प्रत्यागम ret;
        }

        /** GHOSTUPD: "(\<acute>levelBits > 0, id)" */

        n_bits -= levelBits;
        ग्रन्थिः_Cap = slot->cap;

        जइ (unlikely(cap_get_capType(ग्रन्थिः_Cap) != cap_cnode_cap)) {
            ret.status = EXCEPTION_NONE;
            ret.slot = slot;
            ret.bitsRemaining = n_bits;
            प्रत्यागम ret;
        }
    }
}
