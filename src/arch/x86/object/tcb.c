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

word_t CONST Arch_decodeTransfer(word_t flags)
{
    بازگشت 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    بازگشت EXCEPTION_NONE;
}

#ifdef CONFIG_VTX
تهی Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_reply_to_user(tcb);
}

ایستا exception_t performSetEPTRoot(tcb_t *tcb, cap_t cap, cte_t *slot)
{
    cte_t *ریشه_Slot;
    exception_t e;

    ریشه_Slot = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(ریشه_Slot, true);
    اگر (e != EXCEPTION_NONE) {
        بازگشت e;
    }

    cte_درج(cap, slot, ریشه_Slot);

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت EXCEPTION_NONE;
}

exception_t decodeSetEPTRoot(cap_t cap)
{
    cap_t ریشه_Cap;
    cte_t *ریشه_Slot;
    deriveCap_ret_t dc_ret;

    ریشه_Slot = current_extra_caps.excaprefs[0];

    اگر (ریشه_Slot == پوچ) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    ریشه_Cap = ریشه_Slot->cap;

    اگر (cap_get_capType(ریشه_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(ریشه_Slot, ریشه_Cap);
    اگر (dc_ret.status != EXCEPTION_NONE) {
        بازگشت dc_ret.status;
    }

    اگر (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    بازگشت performSetEPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, ریشه_Slot);
}
#endif
