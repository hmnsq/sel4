/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <object.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <kernel/cspace.h>
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine.h>

lookupCap_ret_t lookupCap(tcb_t *thread, cptr_t cPtr)
{
    lookupSlot_raw_ret_t lu_ret;
    lookupCap_ret_t ret;

    lu_ret = lookupSlot(thread, cPtr);
    ଯଦି (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        ret.status = lu_ret.status;
        ret.cap = cap_null_cap_new();
        ଫେରନ୍ତୁ ret;
    }

    ret.status = EXCEPTION_NONE;
    ret.cap = lu_ret.slot->cap;
    ଫେରନ୍ତୁ ret;
}

lookupCapAndSlot_ret_t lookupCapAndSlot(tcb_t *thread, cptr_t cPtr)
{
    lookupSlot_raw_ret_t lu_ret;
    lookupCapAndSlot_ret_t ret;

    lu_ret = lookupSlot(thread, cPtr);
    ଯଦି (unlikely(lu_ret.status != EXCEPTION_NONE)) {
        ret.status = lu_ret.status;
        ret.slot = ଖାଲି;
        ret.cap = cap_null_cap_new();
        ଫେରନ୍ତୁ ret;
    }

    ret.status = EXCEPTION_NONE;
    ret.slot = lu_ret.slot;
    ret.cap = lu_ret.slot->cap;
    ଫେରନ୍ତୁ ret;
}

lookupSlot_raw_ret_t lookupSlot(tcb_t *thread, cptr_t capptr)
{
    cap_t thread_ମୂଳ;
    resolveAddressBits_ret_t res_ret;
    lookupSlot_raw_ret_t ret;

    thread_ମୂଳ = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;
    res_ret = resolveAddressBits(thread_ମୂଳ, capptr, wordBits);

    ret.status = res_ret.status;
    ret.slot = res_ret.slot;
    ଫେରନ୍ତୁ ret;
}

lookupSlot_ret_t lookupSlotForCNodeOp(bool_t isSource, cap_t ମୂଳ, cptr_t capptr,
                                      word_t depth)
{
    resolveAddressBits_ret_t res_ret;
    lookupSlot_ret_t ret;

    ret.slot = ଖାଲି;

    ଯଦି (unlikely(cap_get_capType(ମୂଳ) != cap_cnode_cap)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        current_lookup_fault = lookup_fault_invalid_ମୂଳ_new();
        ret.status = EXCEPTION_SYSCALL_ERROR;
        ଫେରନ୍ତୁ ret;
    }

    ଯଦି (unlikely(depth < 1 || depth > wordBits)) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = wordBits;
        ret.status = EXCEPTION_SYSCALL_ERROR;
        ଫେରନ୍ତୁ ret;
    }
    res_ret = resolveAddressBits(ମୂଳ, capptr, depth);
    ଯଦି (unlikely(res_ret.status != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        /* current_lookup_fault will have been set by resolveAddressBits */
        ret.status = EXCEPTION_SYSCALL_ERROR;
        ଫେରନ୍ତୁ ret;
    }

    ଯଦି (unlikely(res_ret.bitsRemaining != 0)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        current_lookup_fault =
            lookup_fault_depth_mismatch_new(0, res_ret.bitsRemaining);
        ret.status = EXCEPTION_SYSCALL_ERROR;
        ଫେରନ୍ତୁ ret;
    }

    ret.slot = res_ret.slot;
    ret.status = EXCEPTION_NONE;
    ଫେରନ୍ତୁ ret;
}

lookupSlot_ret_t lookupSourceSlot(cap_t ମୂଳ, cptr_t capptr, word_t depth)
{
    ଫେରନ୍ତୁ lookupSlotForCNodeOp(true, ମୂଳ, capptr, depth);
}

lookupSlot_ret_t lookupTargetSlot(cap_t ମୂଳ, cptr_t capptr, word_t depth)
{
    ଫେରନ୍ତୁ lookupSlotForCNodeOp(false, ମୂଳ, capptr, depth);
}

lookupSlot_ret_t lookupPivotSlot(cap_t ମୂଳ, cptr_t capptr, word_t depth)
{
    ଫେରନ୍ତୁ lookupSlotForCNodeOp(true, ମୂଳ, capptr, depth);
}

resolveAddressBits_ret_t resolveAddressBits(cap_t ଗଣ୍ଠି_Cap, cptr_t capptr, word_t n_bits)
{
    resolveAddressBits_ret_t ret;
    word_t radixBits, guardBits, levelBits, guard;
    word_t capGuard, offset;
    cte_t *slot;

    ret.bitsRemaining = n_bits;
    ret.slot = ଖାଲି;

    ଯଦି (unlikely(cap_get_capType(ଗଣ୍ଠି_Cap) != cap_cnode_cap)) {
        current_lookup_fault = lookup_fault_invalid_ମୂଳ_new();
        ret.status = EXCEPTION_LOOKUP_FAULT;
        ଫେରନ୍ତୁ ret;
    }

    ଯେତେବେଳେ (1) {
        radixBits = cap_cnode_cap_get_capCNodeRadix(ଗଣ୍ଠି_Cap);
        guardBits = cap_cnode_cap_get_capCNodeGuardSize(ଗଣ୍ଠି_Cap);
        levelBits = radixBits + guardBits;

        /* Haskell error: "All CNodes must resolve bits" */
        ପ୍ରତିଜ୍ଞା(levelBits != 0);

        capGuard = cap_cnode_cap_get_capCNodeGuard(ଗଣ୍ଠି_Cap);

        /* The MASK(wordRadix) here is to avoid the case where
         * n_bits = wordBits (=2^wordRadix) and guardBits = 0, as it violates
         * the C spec to shift right by more than wordBits-1.
         */
        guard = (capptr >> ((n_bits - guardBits) & MASK(wordRadix))) & MASK(guardBits);
        ଯଦି (unlikely(guardBits > n_bits || guard != capGuard)) {
            current_lookup_fault =
                lookup_fault_guard_mismatch_new(capGuard, n_bits, guardBits);
            ret.status = EXCEPTION_LOOKUP_FAULT;
            ଫେରନ୍ତୁ ret;
        }

        ଯଦି (unlikely(levelBits > n_bits)) {
            current_lookup_fault =
                lookup_fault_depth_mismatch_new(levelBits, n_bits);
            ret.status = EXCEPTION_LOOKUP_FAULT;
            ଫେରନ୍ତୁ ret;
        }

        offset = (capptr >> (n_bits - levelBits)) & MASK(radixBits);
        slot = CTE_PTR(cap_cnode_cap_get_capCNodePtr(ଗଣ୍ଠି_Cap)) + offset;

        ଯଦି (likely(n_bits == levelBits)) {
            ret.status = EXCEPTION_NONE;
            ret.slot = slot;
            ret.bitsRemaining = 0;
            ଫେରନ୍ତୁ ret;
        }

        /** GHOSTUPD: "(\<acute>levelBits > 0, id)" */

        n_bits -= levelBits;
        ଗଣ୍ଠି_Cap = slot->cap;

        ଯଦି (unlikely(cap_get_capType(ଗଣ୍ଠି_Cap) != cap_cnode_cap)) {
            ret.status = EXCEPTION_NONE;
            ret.slot = slot;
            ret.bitsRemaining = n_bits;
            ଫେରନ୍ତୁ ret;
        }
    }
}
