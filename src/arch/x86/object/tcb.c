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

لفظ_t CONST Arch_decodeTransfer(لفظ_t flags)
{
    أرجع 0;
}

exception_t CONST Arch_performTransfer(لفظ_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    أرجع EXCEPTION_NONE;
}

#جے_تعریف_شدہ CONFIG_VTX
فراغ Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_جواب_منزل_user(tcb);
}

ساكن exception_t perform_سیٹ_EPTRoot(tcb_t *tcb, cap_t cap, cte_t *تھاں)
{
    cte_t *جڑ_تھاں;
    exception_t e;

    جڑ_تھاں = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(جڑ_تھاں, true);
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    cte_داخل_کرو(cap, تھاں, جڑ_تھاں);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

exception_t decode_سیٹ_EPTRoot(cap_t cap)
{
    cap_t جڑ_Cap;
    cte_t *جڑ_تھاں;
    deriveCap_ret_t dc_ret;

    جڑ_تھاں = current_extra_caps.excaprefs[0];

    إذا (جڑ_تھاں == عدم) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    جڑ_Cap = جڑ_تھاں->cap;

    إذا (cap_get_capType(جڑ_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(جڑ_تھاں, جڑ_Cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        أرجع dc_ret.حالت;
    }

    إذا (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع perform_سیٹ_EPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, جڑ_تھاں);
}
#جے_انت
