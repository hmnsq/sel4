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
    lookup_خانہ_raw_ret_t lu_ret;
    lookupCap_ret_t ret;

    lu_ret = lookup_خانہ(thread, cPtr);
    اگر (unlikely(lu_ret.حالت_2 != EXCEPTION_NONE)) {
        ret.حالت_2 = lu_ret.حالت_2;
        ret.cap = cap_null_cap_new();
        واپس ret;
    }

    ret.حالت_2 = EXCEPTION_NONE;
    ret.cap = lu_ret.خانہ->cap;
    واپس ret;
}

lookup_Cap_And_خانہ_ret_t lookup_Cap_And_خانہ(tcb_t *thread, cptr_t cPtr)
{
    lookup_خانہ_raw_ret_t lu_ret;
    lookup_Cap_And_خانہ_ret_t ret;

    lu_ret = lookup_خانہ(thread, cPtr);
    اگر (unlikely(lu_ret.حالت_2 != EXCEPTION_NONE)) {
        ret.حالت_2 = lu_ret.حالت_2;
        ret.خانہ = تہی;
        ret.cap = cap_null_cap_new();
        واپس ret;
    }

    ret.حالت_2 = EXCEPTION_NONE;
    ret.خانہ = lu_ret.خانہ;
    ret.cap = lu_ret.خانہ->cap;
    واپس ret;
}

lookup_خانہ_raw_ret_t lookup_خانہ(tcb_t *thread, cptr_t capptr)
{
    cap_t thread_جڑ;
    resolve_پتہ_بٹ_ret_t res_ret;
    lookup_خانہ_raw_ret_t ret;

    thread_جڑ = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;
    res_ret = resolve_پتہ_بٹ(thread_جڑ, capptr, لفظ_بٹ);

    ret.حالت_2 = res_ret.حالت_2;
    ret.خانہ = res_ret.خانہ;
    واپس ret;
}

lookup_خانہ_ret_t lookup_خانہ_For_CNode_Op(bool_t is_ماخذ, cap_t جڑ, cptr_t capptr,
                                      لفظ_t گہرائی)
{
    resolve_پتہ_بٹ_ret_t res_ret;
    lookup_خانہ_ret_t ret;

    ret.خانہ = تہی;

    اگر (unlikely(cap_get_capType(جڑ) != cap_cnode_cap)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = is_ماخذ;
        current_lookup_fault = lookup_fault_invalid_جڑ_new();
        ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        واپس ret;
    }

    اگر (unlikely(گہرائی < 1 || گہرائی > لفظ_بٹ)) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = لفظ_بٹ;
        ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        واپس ret;
    }
    res_ret = resolve_پتہ_بٹ(جڑ, capptr, گہرائی);
    اگر (unlikely(res_ret.حالت_2 != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = is_ماخذ;
        /* current_lookup_fault will have been set by resolveAddressBits */
        ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        واپس ret;
    }

    اگر (unlikely(res_ret.بٹ_بقایا != 0)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = is_ماخذ;
        current_lookup_fault =
            lookup_fault_گہرائی_mismatch_new(0, res_ret.بٹ_بقایا);
        ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        واپس ret;
    }

    ret.خانہ = res_ret.خانہ;
    ret.حالت_2 = EXCEPTION_NONE;
    واپس ret;
}

lookup_خانہ_ret_t lookup_ماخذ_خانہ(cap_t جڑ, cptr_t capptr, لفظ_t گہرائی)
{
    واپس lookup_خانہ_For_CNode_Op(true, جڑ, capptr, گہرائی);
}

lookup_خانہ_ret_t lookup_ہدف_خانہ(cap_t جڑ, cptr_t capptr, لفظ_t گہرائی)
{
    واپس lookup_خانہ_For_CNode_Op(false, جڑ, capptr, گہرائی);
}

lookup_خانہ_ret_t lookup_محور_خانہ(cap_t جڑ, cptr_t capptr, لفظ_t گہرائی)
{
    واپس lookup_خانہ_For_CNode_Op(true, جڑ, capptr, گہرائی);
}

resolve_پتہ_بٹ_ret_t resolve_پتہ_بٹ(cap_t گرہ_Cap, cptr_t capptr, لفظ_t n_بٹ)
{
    resolve_پتہ_بٹ_ret_t ret;
    لفظ_t radix_بٹ, guard_بٹ, level_بٹ, guard;
    لفظ_t capGuard, انحراف;
    cte_t *خانہ;

    ret.بٹ_بقایا = n_بٹ;
    ret.خانہ = تہی;

    اگر (unlikely(cap_get_capType(گرہ_Cap) != cap_cnode_cap)) {
        current_lookup_fault = lookup_fault_invalid_جڑ_new();
        ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
        واپس ret;
    }

    جب (1) {
        radix_بٹ = cap_cnode_cap_get_capCNodeRadix(گرہ_Cap);
        guard_بٹ = cap_cnode_cap_get_cap_CNode_Guard_سائز(گرہ_Cap);
        level_بٹ = radix_بٹ + guard_بٹ;

        /* Haskell error: "All CNodes must resolve bits" */
        أكد(level_بٹ != 0);

        capGuard = cap_cnode_cap_get_capCNodeGuard(گرہ_Cap);

        /* The MASK(wordRadix) here is to avoid the case where
         * n_bits = wordBits (=2^wordRadix) and guardBits = 0, as it violates
         * the C spec to shift right by more than wordBits-1.
         */
        guard = (capptr >> ((n_بٹ - guard_بٹ) & نقاب(لفظ_Radix))) & نقاب(guard_بٹ);
        اگر (unlikely(guard_بٹ > n_بٹ || guard != capGuard)) {
            current_lookup_fault =
                lookup_fault_guard_mismatch_new(capGuard, n_بٹ, guard_بٹ);
            ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
            واپس ret;
        }

        اگر (unlikely(level_بٹ > n_بٹ)) {
            current_lookup_fault =
                lookup_fault_گہرائی_mismatch_new(level_بٹ, n_بٹ);
            ret.حالت_2 = EXCEPTION_LOOKUP_FAULT;
            واپس ret;
        }

        انحراف = (capptr >> (n_بٹ - level_بٹ)) & نقاب(radix_بٹ);
        خانہ = CTE_PTR(cap_cnode_cap_get_capCNodePtr(گرہ_Cap)) + انحراف;

        اگر (likely(n_بٹ == level_بٹ)) {
            ret.حالت_2 = EXCEPTION_NONE;
            ret.خانہ = خانہ;
            ret.بٹ_بقایا = 0;
            واپس ret;
        }

        /** GHOSTUPD: "(\<acute>levelBits > 0, id)" */

        n_بٹ -= level_بٹ;
        گرہ_Cap = خانہ->cap;

        اگر (unlikely(cap_get_capType(گرہ_Cap) != cap_cnode_cap)) {
            ret.حالت_2 = EXCEPTION_NONE;
            ret.خانہ = خانہ;
            ret.بٹ_بقایا = n_بٹ;
            واپس ret;
        }
    }
}
