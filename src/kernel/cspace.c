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
    यदि (unlikely(lu_ret.स्थितिः != EXCEPTION_NONE)) {
        ret.स्थितिः = lu_ret.स्थितिः;
        ret.cap = cap_null_cap_new();
        प्रत्यागम ret;
    }

    ret.स्थितिः = EXCEPTION_NONE;
    ret.cap = lu_ret.slot->cap;
    प्रत्यागम ret;
}

lookupCapAndSlot_ret_t lookupCapAndSlot(tcb_t *thread, cptr_t cPtr)
{
    lookupSlot_raw_ret_t lu_ret;
    lookupCapAndSlot_ret_t ret;

    lu_ret = lookupSlot(thread, cPtr);
    यदि (unlikely(lu_ret.स्थितिः != EXCEPTION_NONE)) {
        ret.स्थितिः = lu_ret.स्थितिः;
        ret.slot = रिक्त;
        ret.cap = cap_null_cap_new();
        प्रत्यागम ret;
    }

    ret.स्थितिः = EXCEPTION_NONE;
    ret.slot = lu_ret.slot;
    ret.cap = lu_ret.slot->cap;
    प्रत्यागम ret;
}

lookupSlot_raw_ret_t lookupSlot(tcb_t *thread, cptr_t capptr)
{
    cap_t thread_मूलम्;
    resolve_स्थानाङ्कः_द्वयङ्काः_ret_t res_ret;
    lookupSlot_raw_ret_t ret;

    thread_मूलम् = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;
    res_ret = resolve_स्थानाङ्कः_द्वयङ्काः(thread_मूलम्, capptr, word_द्वयङ्काः);

    ret.स्थितिः = res_ret.स्थितिः;
    ret.slot = res_ret.slot;
    प्रत्यागम ret;
}

lookupSlot_ret_t lookupSlotForCNodeOp(bool_t isSource, cap_t मूलम्, cptr_t capptr,
                                      word_t गभीरता)
{
    resolve_स्थानाङ्कः_द्वयङ्काः_ret_t res_ret;
    lookupSlot_ret_t ret;

    ret.slot = रिक्त;

    यदि (unlikely(cap_get_capType(मूलम्) != cap_cnode_cap)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        current_lookup_fault = lookup_fault_invalid_मूलम्_new();
        ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        प्रत्यागम ret;
    }

    यदि (unlikely(गभीरता < 1 || गभीरता > word_द्वयङ्काः)) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = word_द्वयङ्काः;
        ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        प्रत्यागम ret;
    }
    res_ret = resolve_स्थानाङ्कः_द्वयङ्काः(मूलम्, capptr, गभीरता);
    यदि (unlikely(res_ret.स्थितिः != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        /* current_lookup_fault will have been set by resolveAddressBits */
        ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        प्रत्यागम ret;
    }

    यदि (unlikely(res_ret.द्वयङ्काः_Remaining != 0)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = isSource;
        current_lookup_fault =
            lookup_fault_गभीरता_mismatch_new(0, res_ret.द्वयङ्काः_Remaining);
        ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        प्रत्यागम ret;
    }

    ret.slot = res_ret.slot;
    ret.स्थितिः = EXCEPTION_NONE;
    प्रत्यागम ret;
}

lookupSlot_ret_t lookupSourceSlot(cap_t मूलम्, cptr_t capptr, word_t गभीरता)
{
    प्रत्यागम lookupSlotForCNodeOp(true, मूलम्, capptr, गभीरता);
}

lookupSlot_ret_t lookupTargetSlot(cap_t मूलम्, cptr_t capptr, word_t गभीरता)
{
    प्रत्यागम lookupSlotForCNodeOp(false, मूलम्, capptr, गभीरता);
}

lookupSlot_ret_t lookupPivotSlot(cap_t मूलम्, cptr_t capptr, word_t गभीरता)
{
    प्रत्यागम lookupSlotForCNodeOp(true, मूलम्, capptr, गभीरता);
}

resolve_स्थानाङ्कः_द्वयङ्काः_ret_t resolve_स्थानाङ्कः_द्वयङ्काः(cap_t ग्रन्थिः_Cap, cptr_t capptr, word_t n_द्वयङ्काः)
{
    resolve_स्थानाङ्कः_द्वयङ्काः_ret_t ret;
    word_t radix_द्वयङ्काः, guard_द्वयङ्काः, level_द्वयङ्काः, guard;
    word_t capGuard, विस्थापनम्;
    cte_t *slot;

    ret.द्वयङ्काः_Remaining = n_द्वयङ्काः;
    ret.slot = रिक्त;

    यदि (unlikely(cap_get_capType(ग्रन्थिः_Cap) != cap_cnode_cap)) {
        current_lookup_fault = lookup_fault_invalid_मूलम्_new();
        ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
        प्रत्यागम ret;
    }

    यावत् (1) {
        radix_द्वयङ्काः = cap_cnode_cap_get_capCNodeRadix(ग्रन्थिः_Cap);
        guard_द्वयङ्काः = cap_cnode_cap_get_cap_CNode_Guard_आकार(ग्रन्थिः_Cap);
        level_द्वयङ्काः = radix_द्वयङ्काः + guard_द्वयङ्काः;

        /* Haskell error: "All CNodes must resolve bits" */
        प्रतिज्ञा(level_द्वयङ्काः != 0);

        capGuard = cap_cnode_cap_get_capCNodeGuard(ग्रन्थिः_Cap);

        /* The MASK(wordRadix) here is to avoid the case where
         * n_bits = wordBits (=2^wordRadix) and guardBits = 0, as it violates
         * the C spec to shift right by more than wordBits-1.
         */
        guard = (capptr >> ((n_द्वयङ्काः - guard_द्वयङ्काः) & आवरणम्(wordRadix))) & आवरणम्(guard_द्वयङ्काः);
        यदि (unlikely(guard_द्वयङ्काः > n_द्वयङ्काः || guard != capGuard)) {
            current_lookup_fault =
                lookup_fault_guard_mismatch_new(capGuard, n_द्वयङ्काः, guard_द्वयङ्काः);
            ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
            प्रत्यागम ret;
        }

        यदि (unlikely(level_द्वयङ्काः > n_द्वयङ्काः)) {
            current_lookup_fault =
                lookup_fault_गभीरता_mismatch_new(level_द्वयङ्काः, n_द्वयङ्काः);
            ret.स्थितिः = EXCEPTION_LOOKUP_FAULT;
            प्रत्यागम ret;
        }

        विस्थापनम् = (capptr >> (n_द्वयङ्काः - level_द्वयङ्काः)) & आवरणम्(radix_द्वयङ्काः);
        slot = CTE_PTR(cap_cnode_cap_get_capCNodePtr(ग्रन्थिः_Cap)) + विस्थापनम्;

        यदि (likely(n_द्वयङ्काः == level_द्वयङ्काः)) {
            ret.स्थितिः = EXCEPTION_NONE;
            ret.slot = slot;
            ret.द्वयङ्काः_Remaining = 0;
            प्रत्यागम ret;
        }

        /** GHOSTUPD: "(\<acute>levelBits > 0, id)" */

        n_द्वयङ्काः -= level_द्वयङ्काः;
        ग्रन्थिः_Cap = slot->cap;

        यदि (unlikely(cap_get_capType(ग्रन्थिः_Cap) != cap_cnode_cap)) {
            ret.स्थितिः = EXCEPTION_NONE;
            ret.slot = slot;
            ret.द्वयङ्काः_Remaining = n_द्वयङ्काः;
            प्रत्यागम ret;
        }
    }
}
