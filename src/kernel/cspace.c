/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <object.h>
#ضمن <api/failures.h>
#ضمن <kernel/thread.h>
#ضمن <kernel/cspace.h>
#ضمن <model/statedata.h>
#ضمن <arch/machine.h>

lookupCap_ret_t lookupCap(tcb_t *thread, cptr_t cPtr)
{
    lookup_تھاں_raw_ret_t lu_ret;
    lookupCap_ret_t ret;

    lu_ret = lookup_تھاں(thread, cPtr);
    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        ret.حالت = lu_ret.حالت;
        ret.cap = cap_null_cap_new();
        أرجع ret;
    }

    ret.حالت = EXCEPTION_NONE;
    ret.cap = lu_ret.تھاں->cap;
    أرجع ret;
}

lookup_Cap_And_تھاں_ret_t lookup_Cap_And_تھاں(tcb_t *thread, cptr_t cPtr)
{
    lookup_تھاں_raw_ret_t lu_ret;
    lookup_Cap_And_تھاں_ret_t ret;

    lu_ret = lookup_تھاں(thread, cPtr);
    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        ret.حالت = lu_ret.حالت;
        ret.تھاں = عدم;
        ret.cap = cap_null_cap_new();
        أرجع ret;
    }

    ret.حالت = EXCEPTION_NONE;
    ret.تھاں = lu_ret.تھاں;
    ret.cap = lu_ret.تھاں->cap;
    أرجع ret;
}

lookup_تھاں_raw_ret_t lookup_تھاں(tcb_t *thread, cptr_t capptr)
{
    cap_t thread_جڑ;
    resolve_پتہ_بٹ_ret_t res_ret;
    lookup_تھاں_raw_ret_t ret;

    thread_جڑ = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;
    res_ret = resolve_پتہ_بٹ(thread_جڑ, capptr, لفظ_بٹ);

    ret.حالت = res_ret.حالت;
    ret.تھاں = res_ret.تھاں;
    أرجع ret;
}

lookup_تھاں_ret_t lookup_تھاں_For_CNode_Op(bool_t is_سروت, cap_t جڑ, cptr_t capptr,
                                      لفظ_t ڈونگھائی)
{
    resolve_پتہ_بٹ_ret_t res_ret;
    lookup_تھاں_ret_t ret;

    ret.تھاں = عدم;

    إذا (unlikely(cap_get_capType(جڑ) != cap_cnode_cap)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سروت = is_سروت;
        current_lookup_fault = lookup_fault_invalid_جڑ_new();
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }

    إذا (unlikely(ڈونگھائی < 1 || ڈونگھائی > لفظ_بٹ)) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = لفظ_بٹ;
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }
    res_ret = resolve_پتہ_بٹ(جڑ, capptr, ڈونگھائی);
    إذا (unlikely(res_ret.حالت != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سروت = is_سروت;
        /* current_lookup_fault will have been set by resolveAddressBits */
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }

    إذا (unlikely(res_ret.بٹ_بچیا != 0)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سروت = is_سروت;
        current_lookup_fault =
            lookup_fault_ڈونگھائی_mismatch_new(0, res_ret.بٹ_بچیا);
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }

    ret.تھاں = res_ret.تھاں;
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

lookup_تھاں_ret_t lookup_سروت_تھاں(cap_t جڑ, cptr_t capptr, لفظ_t ڈونگھائی)
{
    أرجع lookup_تھاں_For_CNode_Op(true, جڑ, capptr, ڈونگھائی);
}

lookup_تھاں_ret_t lookup_نشانہ_تھاں(cap_t جڑ, cptr_t capptr, لفظ_t ڈونگھائی)
{
    أرجع lookup_تھاں_For_CNode_Op(false, جڑ, capptr, ڈونگھائی);
}

lookup_تھاں_ret_t lookup_دھرا_تھاں(cap_t جڑ, cptr_t capptr, لفظ_t ڈونگھائی)
{
    أرجع lookup_تھاں_For_CNode_Op(true, جڑ, capptr, ڈونگھائی);
}

resolve_پتہ_بٹ_ret_t resolve_پتہ_بٹ(cap_t گرہ_Cap, cptr_t capptr, لفظ_t n_بٹ)
{
    resolve_پتہ_بٹ_ret_t ret;
    لفظ_t radix_بٹ, guard_بٹ, level_بٹ, guard;
    لفظ_t capGuard, وتھ;
    cte_t *تھاں;

    ret.بٹ_بچیا = n_بٹ;
    ret.تھاں = عدم;

    إذا (unlikely(cap_get_capType(گرہ_Cap) != cap_cnode_cap)) {
        current_lookup_fault = lookup_fault_invalid_جڑ_new();
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    طالما (1) {
        radix_بٹ = cap_cnode_cap_get_capCNodeRadix(گرہ_Cap);
        guard_بٹ = cap_cnode_cap_get_cap_CNode_Guard_ناپ(گرہ_Cap);
        level_بٹ = radix_بٹ + guard_بٹ;

        /* Haskell error: "All CNodes must resolve bits" */
        أكد(level_بٹ != 0);

        capGuard = cap_cnode_cap_get_capCNodeGuard(گرہ_Cap);

        /* The MASK(wordRadix) here is to avoid the case where
         * n_bits = wordBits (=2^wordRadix) and guardBits = 0, as it violates
         * the C spec to shift right by more than wordBits-1.
         */
        guard = (capptr >> ((n_بٹ - guard_بٹ) & نقاب(لفظ_Radix))) & نقاب(guard_بٹ);
        إذا (unlikely(guard_بٹ > n_بٹ || guard != capGuard)) {
            current_lookup_fault =
                lookup_fault_guard_mismatch_new(capGuard, n_بٹ, guard_بٹ);
            ret.حالت = EXCEPTION_LOOKUP_FAULT;
            أرجع ret;
        }

        إذا (unlikely(level_بٹ > n_بٹ)) {
            current_lookup_fault =
                lookup_fault_ڈونگھائی_mismatch_new(level_بٹ, n_بٹ);
            ret.حالت = EXCEPTION_LOOKUP_FAULT;
            أرجع ret;
        }

        وتھ = (capptr >> (n_بٹ - level_بٹ)) & نقاب(radix_بٹ);
        تھاں = CTE_PTR(cap_cnode_cap_get_capCNodePtr(گرہ_Cap)) + وتھ;

        إذا (likely(n_بٹ == level_بٹ)) {
            ret.حالت = EXCEPTION_NONE;
            ret.تھاں = تھاں;
            ret.بٹ_بچیا = 0;
            أرجع ret;
        }

        /** GHOSTUPD: "(\<acute>levelBits > 0, id)" */

        n_بٹ -= level_بٹ;
        گرہ_Cap = تھاں->cap;

        إذا (unlikely(cap_get_capType(گرہ_Cap) != cap_cnode_cap)) {
            ret.حالت = EXCEPTION_NONE;
            ret.تھاں = تھاں;
            ret.بٹ_بچیا = n_بٹ;
            أرجع ret;
        }
    }
}
