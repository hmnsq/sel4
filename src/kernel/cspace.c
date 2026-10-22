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
    lookup_خونه_raw_ret_t lu_ret;
    lookupCap_ret_t ret;

    lu_ret = lookup_خونه(thread, cPtr);
    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        ret.حالت = lu_ret.حالت;
        ret.cap = cap_null_cap_new();
        أرجع ret;
    }

    ret.حالت = EXCEPTION_NONE;
    ret.cap = lu_ret.خونه->cap;
    أرجع ret;
}

lookup_Cap_And_خونه_ret_t lookup_Cap_And_خونه(tcb_t *thread, cptr_t cPtr)
{
    lookup_خونه_raw_ret_t lu_ret;
    lookup_Cap_And_خونه_ret_t ret;

    lu_ret = lookup_خونه(thread, cPtr);
    إذا (unlikely(lu_ret.حالت != EXCEPTION_NONE)) {
        ret.حالت = lu_ret.حالت;
        ret.خونه = عدم;
        ret.cap = cap_null_cap_new();
        أرجع ret;
    }

    ret.حالت = EXCEPTION_NONE;
    ret.خونه = lu_ret.خونه;
    ret.cap = lu_ret.خونه->cap;
    أرجع ret;
}

lookup_خونه_raw_ret_t lookup_خونه(tcb_t *thread, cptr_t capptr)
{
    cap_t thread_ریښه;
    resolve_پته_بټونه_ret_t res_ret;
    lookup_خونه_raw_ret_t ret;

    thread_ریښه = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;
    res_ret = resolve_پته_بټونه(thread_ریښه, capptr, کلمه_بټونه);

    ret.حالت = res_ret.حالت;
    ret.خونه = res_ret.خونه;
    أرجع ret;
}

lookup_خونه_ret_t lookup_خونه_For_CNode_Op(bool_t is_سرچینه, cap_t ریښه, cptr_t capptr,
                                      کلمه_t ژوروالی)
{
    resolve_پته_بټونه_ret_t res_ret;
    lookup_خونه_ret_t ret;

    ret.خونه = عدم;

    إذا (unlikely(cap_get_capType(ریښه) != cap_cnode_cap)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = is_سرچینه;
        current_lookup_fault = lookup_fault_invalid_ریښه_new();
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }

    إذا (unlikely(ژوروالی < 1 || ژوروالی > کلمه_بټونه)) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = کلمه_بټونه;
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }
    res_ret = resolve_پته_بټونه(ریښه, capptr, ژوروالی);
    إذا (unlikely(res_ret.حالت != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = is_سرچینه;
        /* current_lookup_fault will have been set by resolveAddressBits */
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }

    إذا (unlikely(res_ret.بټونه_پاتې_شوی != 0)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = is_سرچینه;
        current_lookup_fault =
            lookup_fault_ژوروالی_mismatch_new(0, res_ret.بټونه_پاتې_شوی);
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }

    ret.خونه = res_ret.خونه;
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

lookup_خونه_ret_t lookup_سرچینه_خونه(cap_t ریښه, cptr_t capptr, کلمه_t ژوروالی)
{
    أرجع lookup_خونه_For_CNode_Op(true, ریښه, capptr, ژوروالی);
}

lookup_خونه_ret_t lookup_موخه_خونه(cap_t ریښه, cptr_t capptr, کلمه_t ژوروالی)
{
    أرجع lookup_خونه_For_CNode_Op(false, ریښه, capptr, ژوروالی);
}

lookup_خونه_ret_t lookup_محور_خونه(cap_t ریښه, cptr_t capptr, کلمه_t ژوروالی)
{
    أرجع lookup_خونه_For_CNode_Op(true, ریښه, capptr, ژوروالی);
}

resolve_پته_بټونه_ret_t resolve_پته_بټونه(cap_t غوټه_Cap, cptr_t capptr, کلمه_t n_بټونه)
{
    resolve_پته_بټونه_ret_t ret;
    کلمه_t radix_بټونه, guard_بټونه, level_بټونه, guard;
    کلمه_t capGuard, دننه_واټن;
    cte_t *خونه;

    ret.بټونه_پاتې_شوی = n_بټونه;
    ret.خونه = عدم;

    إذا (unlikely(cap_get_capType(غوټه_Cap) != cap_cnode_cap)) {
        current_lookup_fault = lookup_fault_invalid_ریښه_new();
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    طالما (1) {
        radix_بټونه = cap_cnode_cap_get_capCNodeRadix(غوټه_Cap);
        guard_بټونه = cap_cnode_cap_get_cap_CNode_Guard_اندازه(غوټه_Cap);
        level_بټونه = radix_بټونه + guard_بټونه;

        /* Haskell error: "All CNodes must resolve bits" */
        أكد(level_بټونه != 0);

        capGuard = cap_cnode_cap_get_capCNodeGuard(غوټه_Cap);

        /* The MASK(wordRadix) here is to avoid the case where
         * n_bits = wordBits (=2^wordRadix) and guardBits = 0, as it violates
         * the C spec to shift right by more than wordBits-1.
         */
        guard = (capptr >> ((n_بټونه - guard_بټونه) & ماسک(کلمه_Radix))) & ماسک(guard_بټونه);
        إذا (unlikely(guard_بټونه > n_بټونه || guard != capGuard)) {
            current_lookup_fault =
                lookup_fault_guard_mismatch_new(capGuard, n_بټونه, guard_بټونه);
            ret.حالت = EXCEPTION_LOOKUP_FAULT;
            أرجع ret;
        }

        إذا (unlikely(level_بټونه > n_بټونه)) {
            current_lookup_fault =
                lookup_fault_ژوروالی_mismatch_new(level_بټونه, n_بټونه);
            ret.حالت = EXCEPTION_LOOKUP_FAULT;
            أرجع ret;
        }

        دننه_واټن = (capptr >> (n_بټونه - level_بټونه)) & ماسک(radix_بټونه);
        خونه = CTE_PTR(cap_cnode_cap_get_capCNodePtr(غوټه_Cap)) + دننه_واټن;

        إذا (likely(n_بټونه == level_بټونه)) {
            ret.حالت = EXCEPTION_NONE;
            ret.خونه = خونه;
            ret.بټونه_پاتې_شوی = 0;
            أرجع ret;
        }

        /** GHOSTUPD: "(\<acute>levelBits > 0, id)" */

        n_بټونه -= level_بټونه;
        غوټه_Cap = خونه->cap;

        إذا (unlikely(cap_get_capType(غوټه_Cap) != cap_cnode_cap)) {
            ret.حالت = EXCEPTION_NONE;
            ret.خونه = خونه;
            ret.بټونه_پاتې_شوی = n_بټونه;
            أرجع ret;
        }
    }
}
