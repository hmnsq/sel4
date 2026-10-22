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
    फर्काऊ 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    फर्काऊ EXCEPTION_NONE;
}

#ifdef CONFIG_VTX
शून्य Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_reply_to_user(tcb);
}

स्थिर exception_t performSetEPTRoot(tcb_t *tcb, cap_t cap, cte_t *slot)
{
    cte_t *जरा_Slot;
    exception_t e;

    जरा_Slot = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(जरा_Slot, true);
    यदि (e != EXCEPTION_NONE) {
        फर्काऊ e;
    }

    cte_घुसाउ(cap, slot, जरा_Slot);

    setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
    फर्काऊ EXCEPTION_NONE;
}

exception_t decodeSetEPTRoot(cap_t cap)
{
    cap_t जरा_Cap;
    cte_t *जरा_Slot;
    deriveCap_ret_t dc_ret;

    जरा_Slot = current_extra_caps.excaprefs[0];

    यदि (जरा_Slot == रिक्त) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    जरा_Cap = जरा_Slot->cap;

    यदि (cap_get_capType(जरा_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(जरा_Slot, जरा_Cap);
    यदि (dc_ret.status != EXCEPTION_NONE) {
        फर्काऊ dc_ret.status;
    }

    यदि (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    फर्काऊ performSetEPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, जरा_Slot);
}
#endif
