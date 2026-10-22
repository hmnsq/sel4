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

#که_تعریف_شوی CONFIG_VTX
فراغ Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_ځواب_منزل_user(tcb);
}

ساكن exception_t perform_سېټ_EPTRoot(tcb_t *tcb, cap_t cap, cte_t *خونه)
{
    cte_t *ریښه_خونه;
    exception_t e;

    ریښه_خونه = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(ریښه_خونه, true);
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    cte_ورننباسه(cap, خونه, ریښه_خونه);

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

exception_t decode_سېټ_EPTRoot(cap_t cap)
{
    cap_t ریښه_Cap;
    cte_t *ریښه_خونه;
    deriveCap_ret_t dc_ret;

    ریښه_خونه = current_extra_caps.excaprefs[0];

    إذا (ریښه_خونه == عدم) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ریښه_Cap = ریښه_خونه->cap;

    إذا (cap_get_capType(ریښه_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(ریښه_خونه, ریښه_Cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        أرجع dc_ret.حالت;
    }

    إذا (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع perform_سېټ_EPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, ریښه_خونه);
}
#د_که_پای
