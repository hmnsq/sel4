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
    lookup_جڳهه_raw_ret_t lu_ret;
    lookupCap_ret_t ret;

    lu_ret = lookup_جڳهه(thread, cPtr);
    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        ret.حالت = lu_ret.حالت;
        ret.cap = cap_null_cap_new();
        أرجع ret;
    }

    ret.حالت = EXCEPTION_NONE;
    ret.cap = lu_ret.جڳهه->cap;
    أرجع ret;
}

lookup_Cap_And_جڳهه_ret_t lookup_Cap_And_جڳهه(tcb_t *thread, cptr_t cPtr)
{
    lookup_جڳهه_raw_ret_t lu_ret;
    lookup_Cap_And_جڳهه_ret_t ret;

    lu_ret = lookup_جڳهه(thread, cPtr);
    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        ret.حالت = lu_ret.حالت;
        ret.جڳهه = عدم;
        ret.cap = cap_null_cap_new();
        أرجع ret;
    }

    ret.حالت = EXCEPTION_NONE;
    ret.جڳهه = lu_ret.جڳهه;
    ret.cap = lu_ret.جڳهه->cap;
    أرجع ret;
}

lookup_جڳهه_raw_ret_t lookup_جڳهه(tcb_t *thread, cptr_t capptr)
{
    cap_t thread_پاڙ;
    resolve_پتو_بٽ_ret_t res_ret;
    lookup_جڳهه_raw_ret_t ret;

    thread_پاڙ = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;
    res_ret = resolve_پتو_بٽ(thread_پاڙ, capptr, لفظ_بٽ);

    ret.حالت = res_ret.حالت;
    ret.جڳهه = res_ret.جڳهه;
    أرجع ret;
}

lookup_جڳهه_ret_t lookup_جڳهه_For_CNode_Op(bool_t is_ذريعو, cap_t پاڙ, cptr_t capptr,
                                      لفظ_t اونهائي)
{
    resolve_پتو_بٽ_ret_t res_ret;
    lookup_جڳهه_ret_t ret;

    ret.جڳهه = عدم;

    إذا (unlikely(cap_get_capType(پاڙ) != cap_cnode_cap)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ذريعو = is_ذريعو;
        current_lookup_fault = lookup_fault_invalid_پاڙ_new();
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }

    إذا (unlikely(اونهائي < 1 || اونهائي > لفظ_بٽ)) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = لفظ_بٽ;
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }
    res_ret = resolve_پتو_بٽ(پاڙ, capptr, اونهائي);
    إذا (unlikely(res_ret.حالت != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ذريعو = is_ذريعو;
        /* current_lookup_fault will have been set by resolveAddressBits */
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }

    إذا (unlikely(res_ret.بٽ_بچيل != 0)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ذريعو = is_ذريعو;
        current_lookup_fault =
            lookup_fault_اونهائي_mismatch_new(0, res_ret.بٽ_بچيل);
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }

    ret.جڳهه = res_ret.جڳهه;
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

lookup_جڳهه_ret_t lookup_ذريعو_جڳهه(cap_t پاڙ, cptr_t capptr, لفظ_t اونهائي)
{
    أرجع lookup_جڳهه_For_CNode_Op(true, پاڙ, capptr, اونهائي);
}

lookup_جڳهه_ret_t lookup_هدف_جڳهه(cap_t پاڙ, cptr_t capptr, لفظ_t اونهائي)
{
    أرجع lookup_جڳهه_For_CNode_Op(false, پاڙ, capptr, اونهائي);
}

lookup_جڳهه_ret_t lookup_محور_جڳهه(cap_t پاڙ, cptr_t capptr, لفظ_t اونهائي)
{
    أرجع lookup_جڳهه_For_CNode_Op(true, پاڙ, capptr, اونهائي);
}

resolve_پتو_بٽ_ret_t resolve_پتو_بٽ(cap_t ڳنڍ_Cap, cptr_t capptr, لفظ_t n_بٽ)
{
    resolve_پتو_بٽ_ret_t ret;
    لفظ_t radix_بٽ, guard_بٽ, level_بٽ, guard;
    لفظ_t capGuard, وٿي;
    cte_t *جڳهه;

    ret.بٽ_بچيل = n_بٽ;
    ret.جڳهه = عدم;

    إذا (unlikely(cap_get_capType(ڳنڍ_Cap) != cap_cnode_cap)) {
        current_lookup_fault = lookup_fault_invalid_پاڙ_new();
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    طالما (1) {
        radix_بٽ = cap_cnode_cap_get_capCNodeRadix(ڳنڍ_Cap);
        guard_بٽ = cap_cnode_cap_get_cap_CNode_Guard_سائيز(ڳنڍ_Cap);
        level_بٽ = radix_بٽ + guard_بٽ;

        /* Haskell error: "All CNodes must resolve bits" */
        أكد(level_بٽ != 0);

        capGuard = cap_cnode_cap_get_capCNodeGuard(ڳنڍ_Cap);

        /* The MASK(wordRadix) here is to avoid the case where
         * n_bits = wordBits (=2^wordRadix) and guardBits = 0, as it violates
         * the C spec to shift right by more than wordBits-1.
         */
        guard = (capptr >> ((n_بٽ - guard_بٽ) & نقاب(لفظ_Radix))) & نقاب(guard_بٽ);
        إذا (unlikely(guard_بٽ > n_بٽ || guard != capGuard)) {
            current_lookup_fault =
                lookup_fault_guard_mismatch_new(capGuard, n_بٽ, guard_بٽ);
            ret.حالت = EXCEPTION_LOOKUP_FAULT;
            أرجع ret;
        }

        إذا (unlikely(level_بٽ > n_بٽ)) {
            current_lookup_fault =
                lookup_fault_اونهائي_mismatch_new(level_بٽ, n_بٽ);
            ret.حالت = EXCEPTION_LOOKUP_FAULT;
            أرجع ret;
        }

        وٿي = (capptr >> (n_بٽ - level_بٽ)) & نقاب(radix_بٽ);
        جڳهه = CTE_PTR(cap_cnode_cap_get_capCNodePtr(ڳنڍ_Cap)) + وٿي;

        إذا (likely(n_بٽ == level_بٽ)) {
            ret.حالت = EXCEPTION_NONE;
            ret.جڳهه = جڳهه;
            ret.بٽ_بچيل = 0;
            أرجع ret;
        }

        /** GHOSTUPD: "(\<acute>levelBits > 0, id)" */

        n_بٽ -= level_بٽ;
        ڳنڍ_Cap = جڳهه->cap;

        إذا (unlikely(cap_get_capType(ڳنڍ_Cap) != cap_cnode_cap)) {
            ret.حالت = EXCEPTION_NONE;
            ret.جڳهه = جڳهه;
            ret.بٽ_بچيل = n_بٽ;
            أرجع ret;
        }
    }
}
