/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <types.h>
#অন্তর্ভাৱ <api/failures.h>
#অন্তর্ভাৱ <machine/registerset.h>
#অন্তর্ভাৱ <object/structures.h>
#অন্তর্ভাৱ <arch/object/tcb.h>
#অন্তর্ভাৱ <arch/machine.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    ঘূৰাই 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    ঘূৰাই EXCEPTION_NONE;
}

#ifdef CONFIG_VTX
শূন্য Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_reply_to_user(tcb);
}

স্থির exception_t performSetEPTRoot(tcb_t *tcb, cap_t cap, cte_t *slot)
{
    cte_t *মূল_Slot;
    exception_t e;

    মূল_Slot = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(মূল_Slot, true);
    যদি (e != EXCEPTION_NONE) {
        ঘূৰাই e;
    }

    cte_ঢোকাও(cap, slot, মূল_Slot);

    setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
    ঘূৰাই EXCEPTION_NONE;
}

exception_t decodeSetEPTRoot(cap_t cap)
{
    cap_t মূল_Cap;
    cte_t *মূল_Slot;
    deriveCap_ret_t dc_ret;

    মূল_Slot = current_extra_caps.excaprefs[0];

    যদি (মূল_Slot == রিক্ত) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    মূল_Cap = মূল_Slot->cap;

    যদি (cap_get_capType(মূল_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(মূল_Slot, মূল_Cap);
    যদি (dc_ret.status != EXCEPTION_NONE) {
        ঘূৰাই dc_ret.status;
    }

    যদি (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    ঘূৰাই performSetEPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, মূল_Slot);
}
#endif
