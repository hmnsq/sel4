/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <api/failures.h>
#അന്തര്ഭാവ <machine/registerset.h>
#അന്തര്ഭാവ <object/structures.h>
#അന്തര്ഭാവ <arch/object/tcb.h>
#അന്തര്ഭാവ <arch/machine.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    തിരികെ 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    തിരികെ EXCEPTION_NONE;
}

#ifdef CONFIG_VTX
ശൂന്യം Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_reply_to_user(tcb);
}

സ്ഥിതം exception_t performSetEPTRoot(tcb_t *tcb, cap_t cap, cte_t *slot)
{
    cte_t *വേര്_Slot;
    exception_t e;

    വേര്_Slot = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(വേര്_Slot, true);
    എങ്കിൽ (e != EXCEPTION_NONE) {
        തിരികെ e;
    }

    cte_തിരുകുക(cap, slot, വേര്_Slot);

    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ EXCEPTION_NONE;
}

exception_t decodeSetEPTRoot(cap_t cap)
{
    cap_t വേര്_Cap;
    cte_t *വേര്_Slot;
    deriveCap_ret_t dc_ret;

    വേര്_Slot = current_extra_caps.excaprefs[0];

    എങ്കിൽ (വേര്_Slot == ശൂന്യ) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    വേര്_Cap = വേര്_Slot->cap;

    എങ്കിൽ (cap_get_capType(വേര്_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(വേര്_Slot, വേര്_Cap);
    എങ്കിൽ (dc_ret.status != EXCEPTION_NONE) {
        തിരികെ dc_ret.status;
    }

    എങ്കിൽ (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    തിരികെ performSetEPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, വേര്_Slot);
}
#endif
