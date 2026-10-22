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

word_t CONST Arch_decodeTransfer(word_t flags)
{
    أرجع 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    أرجع EXCEPTION_NONE;
}

#ifdef CONFIG_VTX
فراغ Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_reply_to_user(tcb);
}

ساكن exception_t performSetEPTRoot(tcb_t *tcb, cap_t cap, cte_t *slot)
{
    cte_t *جذر_Slot;
    exception_t e;

    جذر_Slot = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(جذر_Slot, true);
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    cte_أدرج(cap, slot, جذر_Slot);

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

exception_t decodeSetEPTRoot(cap_t cap)
{
    cap_t جذر_Cap;
    cte_t *جذر_Slot;
    deriveCap_ret_t dc_ret;

    جذر_Slot = current_extra_caps.excaprefs[0];

    إذا (جذر_Slot == عدم) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    جذر_Cap = جذر_Slot->cap;

    إذا (cap_get_capType(جذر_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(جذر_Slot, جذر_Cap);
    إذا (dc_ret.status != EXCEPTION_NONE) {
        أرجع dc_ret.status;
    }

    إذا (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع performSetEPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, جذر_Slot);
}
#endif
