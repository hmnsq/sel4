/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/cspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>

lookupCap_ret_t lookupCap(tcb_t *thread, cptr_t cPtr)
{
    lookup_ਥਾਂ_raw_ret_t lu_ret;
    lookupCap_ret_t ret;

    lu_ret = lookup_ਥਾਂ(thread, cPtr);
    ਜੇ (unlikely(lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
        ret.ਸਥਿਤੀ = lu_ret.ਸਥਿਤੀ;
        ret.cap = cap_null_cap_new();
        ਵਾਪਸ ret;
    }

    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ret.cap = lu_ret.ਥਾਂ->cap;
    ਵਾਪਸ ret;
}

lookup_Cap_And_ਥਾਂ_ret_t lookup_Cap_And_ਥਾਂ(tcb_t *thread, cptr_t cPtr)
{
    lookup_ਥਾਂ_raw_ret_t lu_ret;
    lookup_Cap_And_ਥਾਂ_ret_t ret;

    lu_ret = lookup_ਥਾਂ(thread, cPtr);
    ਜੇ (unlikely(lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
        ret.ਸਥਿਤੀ = lu_ret.ਸਥਿਤੀ;
        ret.ਥਾਂ = ਨਿਰਾ;
        ret.cap = cap_null_cap_new();
        ਵਾਪਸ ret;
    }

    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ret.ਥਾਂ = lu_ret.ਥਾਂ;
    ret.cap = lu_ret.ਥਾਂ->cap;
    ਵਾਪਸ ret;
}

lookup_ਥਾਂ_raw_ret_t lookup_ਥਾਂ(tcb_t *thread, cptr_t capptr)
{
    cap_t thread_ਜੜ੍ਹ;
    resolve_ਪਤਾ_ਬਿੱਟ_ret_t res_ret;
    lookup_ਥਾਂ_raw_ret_t ret;

    thread_ਜੜ੍ਹ = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;
    res_ret = resolve_ਪਤਾ_ਬਿੱਟ(thread_ਜੜ੍ਹ, capptr, ਸ਼ਬਦ_ਬਿੱਟ);

    ret.ਸਥਿਤੀ = res_ret.ਸਥਿਤੀ;
    ret.ਥਾਂ = res_ret.ਥਾਂ;
    ਵਾਪਸ ret;
}

lookup_ਥਾਂ_ret_t lookup_ਥਾਂ_For_CNode_Op(bool_t is_ਸਰੋਤ, cap_t ਜੜ੍ਹ, cptr_t capptr,
                                      ਸ਼ਬਦ_t ਡੂੰਘਾਈ)
{
    resolve_ਪਤਾ_ਬਿੱਟ_ret_t res_ret;
    lookup_ਥਾਂ_ret_t ret;

    ret.ਥਾਂ = ਨਿਰਾ;

    ਜੇ (unlikely(cap_get_capType(ਜੜ੍ਹ) != cap_cnode_cap)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = is_ਸਰੋਤ;
        current_lookup_fault = lookup_fault_invalid_ਜੜ੍ਹ_new();
        ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        ਵਾਪਸ ret;
    }

    ਜੇ (unlikely(ਡੂੰਘਾਈ < 1 || ਡੂੰਘਾਈ > ਸ਼ਬਦ_ਬਿੱਟ)) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = ਸ਼ਬਦ_ਬਿੱਟ;
        ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        ਵਾਪਸ ret;
    }
    res_ret = resolve_ਪਤਾ_ਬਿੱਟ(ਜੜ੍ਹ, capptr, ਡੂੰਘਾਈ);
    ਜੇ (unlikely(res_ret.ਸਥਿਤੀ != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = is_ਸਰੋਤ;
        /* current_lookup_fault will have been set by resolveAddressBits */
        ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        ਵਾਪਸ ret;
    }

    ਜੇ (unlikely(res_ret.ਬਿੱਟ_ਬਚਿਆ != 0)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = is_ਸਰੋਤ;
        current_lookup_fault =
            lookup_fault_ਡੂੰਘਾਈ_mismatch_new(0, res_ret.ਬਿੱਟ_ਬਚਿਆ);
        ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        ਵਾਪਸ ret;
    }

    ret.ਥਾਂ = res_ret.ਥਾਂ;
    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ਵਾਪਸ ret;
}

lookup_ਥਾਂ_ret_t lookup_ਸਰੋਤ_ਥਾਂ(cap_t ਜੜ੍ਹ, cptr_t capptr, ਸ਼ਬਦ_t ਡੂੰਘਾਈ)
{
    ਵਾਪਸ lookup_ਥਾਂ_For_CNode_Op(true, ਜੜ੍ਹ, capptr, ਡੂੰਘਾਈ);
}

lookup_ਥਾਂ_ret_t lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(cap_t ਜੜ੍ਹ, cptr_t capptr, ਸ਼ਬਦ_t ਡੂੰਘਾਈ)
{
    ਵਾਪਸ lookup_ਥਾਂ_For_CNode_Op(false, ਜੜ੍ਹ, capptr, ਡੂੰਘਾਈ);
}

lookup_ਥਾਂ_ret_t lookup_ਧੁਰਾ_ਥਾਂ(cap_t ਜੜ੍ਹ, cptr_t capptr, ਸ਼ਬਦ_t ਡੂੰਘਾਈ)
{
    ਵਾਪਸ lookup_ਥਾਂ_For_CNode_Op(true, ਜੜ੍ਹ, capptr, ਡੂੰਘਾਈ);
}

resolve_ਪਤਾ_ਬਿੱਟ_ret_t resolve_ਪਤਾ_ਬਿੱਟ(cap_t ਗੰਢ_Cap, cptr_t capptr, ਸ਼ਬਦ_t n_ਬਿੱਟ)
{
    resolve_ਪਤਾ_ਬਿੱਟ_ret_t ret;
    ਸ਼ਬਦ_t radix_ਬਿੱਟ, guard_ਬਿੱਟ, level_ਬਿੱਟ, guard;
    ਸ਼ਬਦ_t capGuard, ਵਿੱਥ;
    cte_t *ਥਾਂ;

    ret.ਬਿੱਟ_ਬਚਿਆ = n_ਬਿੱਟ;
    ret.ਥਾਂ = ਨਿਰਾ;

    ਜੇ (unlikely(cap_get_capType(ਗੰਢ_Cap) != cap_cnode_cap)) {
        current_lookup_fault = lookup_fault_invalid_ਜੜ੍ਹ_new();
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }

    ਜਦੋਂ (1) {
        radix_ਬਿੱਟ = cap_cnode_cap_get_capCNodeRadix(ਗੰਢ_Cap);
        guard_ਬਿੱਟ = cap_cnode_cap_get_cap_CNode_Guard_ਮਾਪ(ਗੰਢ_Cap);
        level_ਬਿੱਟ = radix_ਬਿੱਟ + guard_ਬਿੱਟ;

        /* Haskell error: "All CNodes must resolve bits" */
        ਪ੍ਰਤਿਜ੍ਞਾ(level_ਬਿੱਟ != 0);

        capGuard = cap_cnode_cap_get_capCNodeGuard(ਗੰਢ_Cap);

        /* The MASK(wordRadix) here is to avoid the case where
         * n_bits = wordBits (=2^wordRadix) and guardBits = 0, as it violates
         * the C spec to shift right by more than wordBits-1.
         */
        guard = (capptr >> ((n_ਬਿੱਟ - guard_ਬਿੱਟ) & ਨਕਾਬ(ਸ਼ਬਦ_Radix))) & ਨਕਾਬ(guard_ਬਿੱਟ);
        ਜੇ (unlikely(guard_ਬਿੱਟ > n_ਬਿੱਟ || guard != capGuard)) {
            current_lookup_fault =
                lookup_fault_guard_mismatch_new(capGuard, n_ਬਿੱਟ, guard_ਬਿੱਟ);
            ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
            ਵਾਪਸ ret;
        }

        ਜੇ (unlikely(level_ਬਿੱਟ > n_ਬਿੱਟ)) {
            current_lookup_fault =
                lookup_fault_ਡੂੰਘਾਈ_mismatch_new(level_ਬਿੱਟ, n_ਬਿੱਟ);
            ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
            ਵਾਪਸ ret;
        }

        ਵਿੱਥ = (capptr >> (n_ਬਿੱਟ - level_ਬਿੱਟ)) & ਨਕਾਬ(radix_ਬਿੱਟ);
        ਥਾਂ = CTE_PTR(cap_cnode_cap_get_capCNodePtr(ਗੰਢ_Cap)) + ਵਿੱਥ;

        ਜੇ (likely(n_ਬਿੱਟ == level_ਬਿੱਟ)) {
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
            ret.ਥਾਂ = ਥਾਂ;
            ret.ਬਿੱਟ_ਬਚਿਆ = 0;
            ਵਾਪਸ ret;
        }

        /** GHOSTUPD: "(\<acute>levelBits > 0, id)" */

        n_ਬਿੱਟ -= level_ਬਿੱਟ;
        ਗੰਢ_Cap = ਥਾਂ->cap;

        ਜੇ (unlikely(cap_get_capType(ਗੰਢ_Cap) != cap_cnode_cap)) {
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
            ret.ਥਾਂ = ਥਾਂ;
            ret.ਬਿੱਟ_ਬਚਿਆ = n_ਬਿੱਟ;
            ਵਾਪਸ ret;
        }
    }
}
