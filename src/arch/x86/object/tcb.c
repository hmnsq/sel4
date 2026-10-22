/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <machine/registerset.h>
#ضمن <object/structures.h>
#ضمن <arch/object/tcb.h>
#ضمن <arch/machine.h>

کلمه_t CONST Arch_decodeTransfer(کلمه_t flags)
{
    أرجع 0;
}

exception_t CONST Arch_performTransfer(کلمه_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    أرجع EXCEPTION_NONE;
}

#اگر_تعریف_شده CONFIG_VTX
فراغ Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_پاسخ_مقصد_user(tcb);
}

ساكن exception_t perform_مجموعه_EPTRoot(tcb_t *tcb, cap_t cap, cte_t *جایگاه)
{
    cte_t *ریشه_جایگاه;
    exception_t e;

    ریشه_جایگاه = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(ریشه_جایگاه, true);
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    cte_درج(cap, جایگاه, ریشه_جایگاه);

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

exception_t decode_مجموعه_EPTRoot(cap_t cap)
{
    cap_t ریشه_Cap;
    cte_t *ریشه_جایگاه;
    deriveCap_ret_t dc_ret;

    ریشه_جایگاه = current_extra_caps.excaprefs[0];

    إذا (ریشه_جایگاه == عدم) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ریشه_Cap = ریشه_جایگاه->cap;

    إذا (cap_get_capType(ریشه_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(ریشه_جایگاه, ریشه_Cap);
    إذا (dc_ret.وضعیت != EXCEPTION_NONE) {
        أرجع dc_ret.وضعیت;
    }

    إذا (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع perform_مجموعه_EPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, ریشه_جایگاه);
}
#پایان_اگر
