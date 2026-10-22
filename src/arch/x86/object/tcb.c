/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <machine/registerset.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <arch/object/tcb.h>
#अन्तर्भाव <arch/machine.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    प्रत्यागम 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    प्रत्यागम EXCEPTION_NONE;
}

#ifdef CONFIG_VTX
शून्य Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_reply_to_user(tcb);
}

स्थिर exception_t performSetEPTRoot(tcb_t *tcb, cap_t cap, cte_t *slot)
{
    cte_t *मूळ_Slot;
    exception_t e;

    मूळ_Slot = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(मूळ_Slot, true);
    यदि (e != EXCEPTION_NONE) {
        प्रत्यागम e;
    }

    cte_घाला(cap, slot, मूळ_Slot);

    setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeSetEPTRoot(cap_t cap)
{
    cap_t मूळ_Cap;
    cte_t *मूळ_Slot;
    deriveCap_ret_t dc_ret;

    मूळ_Slot = current_extra_caps.excaprefs[0];

    यदि (मूळ_Slot == रिक्त) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    मूळ_Cap = मूळ_Slot->cap;

    यदि (cap_get_capType(मूळ_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(मूळ_Slot, मूळ_Cap);
    यदि (dc_ret.status != EXCEPTION_NONE) {
        प्रत्यागम dc_ret.status;
    }

    यदि (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    प्रत्यागम performSetEPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, मूळ_Slot);
}
#endif
