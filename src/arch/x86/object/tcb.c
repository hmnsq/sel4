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
    पटिनिवत्त 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    पटिनिवत्त EXCEPTION_NONE;
}

#ifdef CONFIG_VTX
शून्य Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_reply_to_user(tcb);
}

स्थिर exception_t performSetEPTRoot(tcb_t *tcb, cap_t cap, cte_t *slot)
{
    cte_t *मूलम्_Slot;
    exception_t e;

    मूलम्_Slot = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(मूलम्_Slot, true);
    सचे (e != EXCEPTION_NONE) {
        पटिनिवत्त e;
    }

    cte_निवेशय(cap, slot, मूलम्_Slot);

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त EXCEPTION_NONE;
}

exception_t decodeSetEPTRoot(cap_t cap)
{
    cap_t मूलम्_Cap;
    cte_t *मूलम्_Slot;
    deriveCap_ret_t dc_ret;

    मूलम्_Slot = current_extra_caps.excaprefs[0];

    सचे (मूलम्_Slot == रिक्त) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    मूलम्_Cap = मूलम्_Slot->cap;

    सचे (cap_get_capType(मूलम्_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(मूलम्_Slot, मूलम्_Cap);
    सचे (dc_ret.status != EXCEPTION_NONE) {
        पटिनिवत्त dc_ret.status;
    }

    सचे (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    पटिनिवत्त performSetEPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, मूलम्_Slot);
}
#endif
