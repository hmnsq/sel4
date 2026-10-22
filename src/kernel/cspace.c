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
    lookup_ಸ್ಥಾನ_raw_ret_t lu_ret;
    lookupCap_ret_t ret;

    lu_ret = lookup_ಸ್ಥಾನ(thread, cPtr);
    ಆದರೆ (unlikely(lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
        ret.ಸ್ಥಿತಿ_2 = lu_ret.ಸ್ಥಿತಿ_2;
        ret.cap = cap_null_cap_new();
        ಹಿಂತಿರುಗಿ ret;
    }

    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ret.cap = lu_ret.ಸ್ಥಾನ->cap;
    ಹಿಂತಿರುಗಿ ret;
}

lookup_Cap_And_ಸ್ಥಾನ_ret_t lookup_Cap_And_ಸ್ಥಾನ(tcb_t *thread, cptr_t cPtr)
{
    lookup_ಸ್ಥಾನ_raw_ret_t lu_ret;
    lookup_Cap_And_ಸ್ಥಾನ_ret_t ret;

    lu_ret = lookup_ಸ್ಥಾನ(thread, cPtr);
    ಆದರೆ (unlikely(lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
        ret.ಸ್ಥಿತಿ_2 = lu_ret.ಸ್ಥಿತಿ_2;
        ret.ಸ್ಥಾನ = ಖಾಲಿ;
        ret.cap = cap_null_cap_new();
        ಹಿಂತಿರುಗಿ ret;
    }

    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ret.ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;
    ret.cap = lu_ret.ಸ್ಥಾನ->cap;
    ಹಿಂತಿರುಗಿ ret;
}

lookup_ಸ್ಥಾನ_raw_ret_t lookup_ಸ್ಥಾನ(tcb_t *thread, cptr_t capptr)
{
    cap_t thread_ಬೇರು;
    resolve_ವಿಳಾಸ_ಬಿಟ್ಟುಗಳು_ret_t res_ret;
    lookup_ಸ್ಥಾನ_raw_ret_t ret;

    thread_ಬೇರು = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;
    res_ret = resolve_ವಿಳಾಸ_ಬಿಟ್ಟುಗಳು(thread_ಬೇರು, capptr, ಶಬ್ದ_ಬಿಟ್ಟುಗಳು);

    ret.ಸ್ಥಿತಿ_2 = res_ret.ಸ್ಥಿತಿ_2;
    ret.ಸ್ಥಾನ = res_ret.ಸ್ಥಾನ;
    ಹಿಂತಿರುಗಿ ret;
}

lookup_ಸ್ಥಾನ_ret_t lookup_ಸ್ಥಾನ_For_CNode_Op(bool_t is_ಮೂಲ, cap_t ಬೇರು, cptr_t capptr,
                                      ಶಬ್ದ_t ಆಳ)
{
    resolve_ವಿಳಾಸ_ಬಿಟ್ಟುಗಳು_ret_t res_ret;
    lookup_ಸ್ಥಾನ_ret_t ret;

    ret.ಸ್ಥಾನ = ಖಾಲಿ;

    ಆದರೆ (unlikely(cap_get_capType(ಬೇರು) != cap_cnode_cap)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = is_ಮೂಲ;
        current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        ಹಿಂತಿರುಗಿ ret;
    }

    ಆದರೆ (unlikely(ಆಳ < 1 || ಆಳ > ಶಬ್ದ_ಬಿಟ್ಟುಗಳು)) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = ಶಬ್ದ_ಬಿಟ್ಟುಗಳು;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        ಹಿಂತಿರುಗಿ ret;
    }
    res_ret = resolve_ವಿಳಾಸ_ಬಿಟ್ಟುಗಳು(ಬೇರು, capptr, ಆಳ);
    ಆದರೆ (unlikely(res_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = is_ಮೂಲ;
        /* current_lookup_fault will have been set by resolveAddressBits */
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        ಹಿಂತಿರುಗಿ ret;
    }

    ಆದರೆ (unlikely(res_ret.ಬಿಟ್ಟುಗಳು_ಉಳಿದ != 0)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = is_ಮೂಲ;
        current_lookup_fault =
            lookup_fault_ಆಳ_mismatch_new(0, res_ret.ಬಿಟ್ಟುಗಳು_ಉಳಿದ);
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        ಹಿಂತಿರುಗಿ ret;
    }

    ret.ಸ್ಥಾನ = res_ret.ಸ್ಥಾನ;
    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ಹಿಂತಿರುಗಿ ret;
}

lookup_ಸ್ಥಾನ_ret_t lookup_ಮೂಲ_ಸ್ಥಾನ(cap_t ಬೇರು, cptr_t capptr, ಶಬ್ದ_t ಆಳ)
{
    ಹಿಂತಿರುಗಿ lookup_ಸ್ಥಾನ_For_CNode_Op(true, ಬೇರು, capptr, ಆಳ);
}

lookup_ಸ್ಥಾನ_ret_t lookup_ಗುರಿ_ಸ್ಥಾನ(cap_t ಬೇರು, cptr_t capptr, ಶಬ್ದ_t ಆಳ)
{
    ಹಿಂತಿರುಗಿ lookup_ಸ್ಥಾನ_For_CNode_Op(false, ಬೇರು, capptr, ಆಳ);
}

lookup_ಸ್ಥಾನ_ret_t lookup_ಅಚ್ಚು_ಸ್ಥಾನ(cap_t ಬೇರು, cptr_t capptr, ಶಬ್ದ_t ಆಳ)
{
    ಹಿಂತಿರುಗಿ lookup_ಸ್ಥಾನ_For_CNode_Op(true, ಬೇರು, capptr, ಆಳ);
}

resolve_ವಿಳಾಸ_ಬಿಟ್ಟುಗಳು_ret_t resolve_ವಿಳಾಸ_ಬಿಟ್ಟುಗಳು(cap_t ಗಂಟು_Cap, cptr_t capptr, ಶಬ್ದ_t n_ಬಿಟ್ಟುಗಳು)
{
    resolve_ವಿಳಾಸ_ಬಿಟ್ಟುಗಳು_ret_t ret;
    ಶಬ್ದ_t radix_ಬಿಟ್ಟುಗಳು, guard_ಬಿಟ್ಟುಗಳು, level_ಬಿಟ್ಟುಗಳು, guard;
    ಶಬ್ದ_t capGuard, ಸ್ಥಾನಾಂತರ;
    cte_t *ಸ್ಥಾನ;

    ret.ಬಿಟ್ಟುಗಳು_ಉಳಿದ = n_ಬಿಟ್ಟುಗಳು;
    ret.ಸ್ಥಾನ = ಖಾಲಿ;

    ಆದರೆ (unlikely(cap_get_capType(ಗಂಟು_Cap) != cap_cnode_cap)) {
        current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }

    ಆಗ (1) {
        radix_ಬಿಟ್ಟುಗಳು = cap_cnode_cap_get_capCNodeRadix(ಗಂಟು_Cap);
        guard_ಬಿಟ್ಟುಗಳು = cap_cnode_cap_get_cap_CNode_Guard_ಗಾತ್ರ(ಗಂಟು_Cap);
        level_ಬಿಟ್ಟುಗಳು = radix_ಬಿಟ್ಟುಗಳು + guard_ಬಿಟ್ಟುಗಳು;

        /* Haskell error: "All CNodes must resolve bits" */
        ಪ್ರತಿಜ್ಞಾ(level_ಬಿಟ್ಟುಗಳು != 0);

        capGuard = cap_cnode_cap_get_capCNodeGuard(ಗಂಟು_Cap);

        /* The MASK(wordRadix) here is to avoid the case where
         * n_bits = wordBits (=2^wordRadix) and guardBits = 0, as it violates
         * the C spec to shift right by more than wordBits-1.
         */
        guard = (capptr >> ((n_ಬಿಟ್ಟುಗಳು - guard_ಬಿಟ್ಟುಗಳು) & ಮುಖವಾಡ(ಶಬ್ದ_Radix))) & ಮುಖವಾಡ(guard_ಬಿಟ್ಟುಗಳು);
        ಆದರೆ (unlikely(guard_ಬಿಟ್ಟುಗಳು > n_ಬಿಟ್ಟುಗಳು || guard != capGuard)) {
            current_lookup_fault =
                lookup_fault_guard_mismatch_new(capGuard, n_ಬಿಟ್ಟುಗಳು, guard_ಬಿಟ್ಟುಗಳು);
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
            ಹಿಂತಿರುಗಿ ret;
        }

        ಆದರೆ (unlikely(level_ಬಿಟ್ಟುಗಳು > n_ಬಿಟ್ಟುಗಳು)) {
            current_lookup_fault =
                lookup_fault_ಆಳ_mismatch_new(level_ಬಿಟ್ಟುಗಳು, n_ಬಿಟ್ಟುಗಳು);
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
            ಹಿಂತಿರುಗಿ ret;
        }

        ಸ್ಥಾನಾಂತರ = (capptr >> (n_ಬಿಟ್ಟುಗಳು - level_ಬಿಟ್ಟುಗಳು)) & ಮುಖವಾಡ(radix_ಬಿಟ್ಟುಗಳು);
        ಸ್ಥಾನ = CTE_PTR(cap_cnode_cap_get_capCNodePtr(ಗಂಟು_Cap)) + ಸ್ಥಾನಾಂತರ;

        ಆದರೆ (likely(n_ಬಿಟ್ಟುಗಳು == level_ಬಿಟ್ಟುಗಳು)) {
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
            ret.ಸ್ಥಾನ = ಸ್ಥಾನ;
            ret.ಬಿಟ್ಟುಗಳು_ಉಳಿದ = 0;
            ಹಿಂತಿರುಗಿ ret;
        }

        /** GHOSTUPD: "(\<acute>levelBits > 0, id)" */

        n_ಬಿಟ್ಟುಗಳು -= level_ಬಿಟ್ಟುಗಳು;
        ಗಂಟು_Cap = ಸ್ಥಾನ->cap;

        ಆದರೆ (unlikely(cap_get_capType(ಗಂಟು_Cap) != cap_cnode_cap)) {
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
            ret.ಸ್ಥಾನ = ಸ್ಥಾನ;
            ret.ಬಿಟ್ಟುಗಳು_ಉಳಿದ = n_ಬಿಟ್ಟುಗಳು;
            ಹಿಂತಿರುಗಿ ret;
        }
    }
}
