/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <arch/object/tcb.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    ಪ್ರತ್ಯಾಗಮ 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

#ifdef CONFIG_VTX
ಶೂನ್ಯ Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_reply_to_user(tcb);
}

ಸ್ಥಿರ exception_t performSetEPTRoot(tcb_t *tcb, cap_t cap, cte_t *slot)
{
    cte_t *ಬೇರು_Slot;
    exception_t e;

    ಬೇರು_Slot = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(ಬೇರು_Slot, true);
    ಯದಿ (e != EXCEPTION_NONE) {
        ಪ್ರತ್ಯಾಗಮ e;
    }

    cte_ಸೇರಿಸು(cap, slot, ಬೇರು_Slot);

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t decodeSetEPTRoot(cap_t cap)
{
    cap_t ಬೇರು_Cap;
    cte_t *ಬೇರು_Slot;
    deriveCap_ret_t dc_ret;

    ಬೇರು_Slot = current_extra_caps.excaprefs[0];

    ಯದಿ (ಬೇರು_Slot == ರಿಕ್ತ) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಬೇರು_Cap = ಬೇರು_Slot->cap;

    ಯದಿ (cap_get_capType(ಬೇರು_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(ಬೇರು_Slot, ಬೇರು_Cap);
    ಯದಿ (dc_ret.status != EXCEPTION_NONE) {
        ಪ್ರತ್ಯಾಗಮ dc_ret.status;
    }

    ಯದಿ (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಪ್ರತ್ಯಾಗಮ performSetEPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, ಬೇರು_Slot);
}
#endif
