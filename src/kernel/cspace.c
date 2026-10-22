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
    lookup_جایگاه_raw_ret_t lu_ret;
    lookupCap_ret_t ret;

    lu_ret = lookup_جایگاه(thread, cPtr);
    إذا (unlikely(lu_ret.وضعیت != EXCEPTION_NONE)) {
        ret.وضعیت = lu_ret.وضعیت;
        ret.cap = cap_null_cap_new();
        أرجع ret;
    }

    ret.وضعیت = EXCEPTION_NONE;
    ret.cap = lu_ret.جایگاه->cap;
    أرجع ret;
}

lookup_Cap_And_جایگاه_ret_t lookup_Cap_And_جایگاه(tcb_t *thread, cptr_t cPtr)
{
    lookup_جایگاه_raw_ret_t lu_ret;
    lookup_Cap_And_جایگاه_ret_t ret;

    lu_ret = lookup_جایگاه(thread, cPtr);
    إذا (unlikely(lu_ret.وضعیت != EXCEPTION_NONE)) {
        ret.وضعیت = lu_ret.وضعیت;
        ret.جایگاه = عدم;
        ret.cap = cap_null_cap_new();
        أرجع ret;
    }

    ret.وضعیت = EXCEPTION_NONE;
    ret.جایگاه = lu_ret.جایگاه;
    ret.cap = lu_ret.جایگاه->cap;
    أرجع ret;
}

lookup_جایگاه_raw_ret_t lookup_جایگاه(tcb_t *thread, cptr_t capptr)
{
    cap_t thread_ریشه;
    resolve_آدرس_بیتها_ret_t res_ret;
    lookup_جایگاه_raw_ret_t ret;

    thread_ریشه = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;
    res_ret = resolve_آدرس_بیتها(thread_ریشه, capptr, کلمه_بیتها);

    ret.وضعیت = res_ret.وضعیت;
    ret.جایگاه = res_ret.جایگاه;
    أرجع ret;
}

lookup_جایگاه_ret_t lookup_جایگاه_For_CNode_Op(bool_t is_منبع, cap_t ریشه, cptr_t capptr,
                                      کلمه_t عمق)
{
    resolve_آدرس_بیتها_ret_t res_ret;
    lookup_جایگاه_ret_t ret;

    ret.جایگاه = عدم;

    إذا (unlikely(cap_get_capType(ریشه) != cap_cnode_cap)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = is_منبع;
        current_lookup_fault = lookup_fault_invalid_ریشه_new();
        ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }

    إذا (unlikely(عمق < 1 || عمق > کلمه_بیتها)) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = کلمه_بیتها;
        ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }
    res_ret = resolve_آدرس_بیتها(ریشه, capptr, عمق);
    إذا (unlikely(res_ret.وضعیت != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = is_منبع;
        /* current_lookup_fault will have been set by resolveAddressBits */
        ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }

    إذا (unlikely(res_ret.بیتها_مانده != 0)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = is_منبع;
        current_lookup_fault =
            lookup_fault_عمق_mismatch_new(0, res_ret.بیتها_مانده);
        ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }

    ret.جایگاه = res_ret.جایگاه;
    ret.وضعیت = EXCEPTION_NONE;
    أرجع ret;
}

lookup_جایگاه_ret_t lookup_منبع_جایگاه(cap_t ریشه, cptr_t capptr, کلمه_t عمق)
{
    أرجع lookup_جایگاه_For_CNode_Op(true, ریشه, capptr, عمق);
}

lookup_جایگاه_ret_t lookup_هدف_جایگاه(cap_t ریشه, cptr_t capptr, کلمه_t عمق)
{
    أرجع lookup_جایگاه_For_CNode_Op(false, ریشه, capptr, عمق);
}

lookup_جایگاه_ret_t lookup_محور_جایگاه(cap_t ریشه, cptr_t capptr, کلمه_t عمق)
{
    أرجع lookup_جایگاه_For_CNode_Op(true, ریشه, capptr, عمق);
}

resolve_آدرس_بیتها_ret_t resolve_آدرس_بیتها(cap_t گره_Cap, cptr_t capptr, کلمه_t n_بیتها)
{
    resolve_آدرس_بیتها_ret_t ret;
    کلمه_t radix_بیتها, guard_بیتها, level_بیتها, guard;
    کلمه_t capGuard, جابه_جایی;
    cte_t *جایگاه;

    ret.بیتها_مانده = n_بیتها;
    ret.جایگاه = عدم;

    إذا (unlikely(cap_get_capType(گره_Cap) != cap_cnode_cap)) {
        current_lookup_fault = lookup_fault_invalid_ریشه_new();
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    طالما (1) {
        radix_بیتها = cap_cnode_cap_get_capCNodeRadix(گره_Cap);
        guard_بیتها = cap_cnode_cap_get_cap_CNode_Guard_اندازه(گره_Cap);
        level_بیتها = radix_بیتها + guard_بیتها;

        /* Haskell error: "All CNodes must resolve bits" */
        أكد(level_بیتها != 0);

        capGuard = cap_cnode_cap_get_capCNodeGuard(گره_Cap);

        /* The MASK(wordRadix) here is to avoid the case where
         * n_bits = wordBits (=2^wordRadix) and guardBits = 0, as it violates
         * the C spec to shift right by more than wordBits-1.
         */
        guard = (capptr >> ((n_بیتها - guard_بیتها) & ماسک(کلمه_Radix))) & ماسک(guard_بیتها);
        إذا (unlikely(guard_بیتها > n_بیتها || guard != capGuard)) {
            current_lookup_fault =
                lookup_fault_guard_mismatch_new(capGuard, n_بیتها, guard_بیتها);
            ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
            أرجع ret;
        }

        إذا (unlikely(level_بیتها > n_بیتها)) {
            current_lookup_fault =
                lookup_fault_عمق_mismatch_new(level_بیتها, n_بیتها);
            ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
            أرجع ret;
        }

        جابه_جایی = (capptr >> (n_بیتها - level_بیتها)) & ماسک(radix_بیتها);
        جایگاه = CTE_PTR(cap_cnode_cap_get_capCNodePtr(گره_Cap)) + جابه_جایی;

        إذا (likely(n_بیتها == level_بیتها)) {
            ret.وضعیت = EXCEPTION_NONE;
            ret.جایگاه = جایگاه;
            ret.بیتها_مانده = 0;
            أرجع ret;
        }

        /** GHOSTUPD: "(\<acute>levelBits > 0, id)" */

        n_بیتها -= level_بیتها;
        گره_Cap = جایگاه->cap;

        إذا (unlikely(cap_get_capType(گره_Cap) != cap_cnode_cap)) {
            ret.وضعیت = EXCEPTION_NONE;
            ret.جایگاه = جایگاه;
            ret.بیتها_مانده = n_بیتها;
            أرجع ret;
        }
    }
}
