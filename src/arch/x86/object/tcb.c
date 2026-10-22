/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <types.h>
#شامل <api/failures.h>
#شامل <machine/registerset.h>
#شامل <object/structures.h>
#شامل <arch/object/tcb.h>
#شامل <arch/machine.h>

لفظ_t CONST Arch_decodeTransfer(لفظ_t flags)
{
    واپس 0;
}

exception_t CONST Arch_performTransfer(لفظ_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    واپس EXCEPTION_NONE;
}

#اگر_تعریف_شدہ CONFIG_VTX
خالی Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_جواب_مقصد_user(tcb);
}

ساکن exception_t perform_سیٹ_EPTRoot(tcb_t *tcb, cap_t cap, cte_t *خانہ)
{
    cte_t *جڑ_خانہ;
    exception_t e;

    جڑ_خانہ = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(جڑ_خانہ, true);
    اگر (e != EXCEPTION_NONE) {
        واپس e;
    }

    cte_داخل_کرو(cap, خانہ, جڑ_خانہ);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس EXCEPTION_NONE;
}

exception_t decode_سیٹ_EPTRoot(cap_t cap)
{
    cap_t جڑ_Cap;
    cte_t *جڑ_خانہ;
    deriveCap_ret_t dc_ret;

    جڑ_خانہ = current_extra_caps.excaprefs[0];

    اگر (جڑ_خانہ == تہی) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    جڑ_Cap = جڑ_خانہ->cap;

    اگر (cap_get_capType(جڑ_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(جڑ_خانہ, جڑ_Cap);
    اگر (dc_ret.حالت_2 != EXCEPTION_NONE) {
        واپس dc_ret.حالت_2;
    }

    اگر (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    واپس perform_سیٹ_EPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, جڑ_خانہ);
}
#اگر_ختم
