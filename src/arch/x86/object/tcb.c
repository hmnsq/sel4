/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <machine/registerset.h>
#અન્તર્ભાવ <object/structures.h>
#અન્તર્ભાવ <arch/object/tcb.h>
#અન્તર્ભાવ <arch/machine.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    પાછા 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    પાછા EXCEPTION_NONE;
}

#ifdef CONFIG_VTX
શૂન્ય Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_reply_to_user(tcb);
}

સ્થિર exception_t performSetEPTRoot(tcb_t *tcb, cap_t cap, cte_t *slot)
{
    cte_t *મૂળ_Slot;
    exception_t e;

    મૂળ_Slot = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(મૂળ_Slot, true);
    જો (e != EXCEPTION_NONE) {
        પાછા e;
    }

    cte_ઉમેરો(cap, slot, મૂળ_Slot);

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા EXCEPTION_NONE;
}

exception_t decodeSetEPTRoot(cap_t cap)
{
    cap_t મૂળ_Cap;
    cte_t *મૂળ_Slot;
    deriveCap_ret_t dc_ret;

    મૂળ_Slot = current_extra_caps.excaprefs[0];

    જો (મૂળ_Slot == ખાલી) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    મૂળ_Cap = મૂળ_Slot->cap;

    જો (cap_get_capType(મૂળ_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(મૂળ_Slot, મૂળ_Cap);
    જો (dc_ret.status != EXCEPTION_NONE) {
        પાછા dc_ret.status;
    }

    જો (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    પાછા performSetEPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, મૂળ_Slot);
}
#endif
