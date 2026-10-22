/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/registerset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/tcb.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>

ਸ਼ਬਦ_t CONST Arch_decodeTransfer(ਸ਼ਬਦ_t flags)
{
    ਵਾਪਸ 0;
}

exception_t CONST Arch_performTransfer(ਸ਼ਬਦ_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    ਵਾਪਸ EXCEPTION_NONE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
ਖਾਲੀ Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_ਜਵਾਬ_ਮੰਜ਼ਿਲ_user(tcb);
}

ਸਥਿਰ exception_t perform_ਸੈੱਟ_EPTRoot(tcb_t *tcb, cap_t cap, cte_t *ਥਾਂ)
{
    cte_t *ਜੜ੍ਹ_ਥਾਂ;
    exception_t e;

    ਜੜ੍ਹ_ਥਾਂ = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(ਜੜ੍ਹ_ਥਾਂ, true);
    ਜੇ (e != EXCEPTION_NONE) {
        ਵਾਪਸ e;
    }

    cte_ਪਾਓ(cap, ਥਾਂ, ਜੜ੍ਹ_ਥਾਂ);

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decode_ਸੈੱਟ_EPTRoot(cap_t cap)
{
    cap_t ਜੜ੍ਹ_Cap;
    cte_t *ਜੜ੍ਹ_ਥਾਂ;
    deriveCap_ret_t dc_ret;

    ਜੜ੍ਹ_ਥਾਂ = current_extra_caps.excaprefs[0];

    ਜੇ (ਜੜ੍ਹ_ਥਾਂ == ਨਿਰਾ) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੜ੍ਹ_Cap = ਜੜ੍ਹ_ਥਾਂ->cap;

    ਜੇ (cap_get_capType(ਜੜ੍ਹ_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(ਜੜ੍ਹ_ਥਾਂ, ਜੜ੍ਹ_Cap);
    ਜੇ (dc_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ dc_ret.ਸਥਿਤੀ;
    }

    ਜੇ (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਵਾਪਸ perform_ਸੈੱਟ_EPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, ਜੜ੍ਹ_ਥਾਂ);
}
#ਜੇ_ਅੰਤ
