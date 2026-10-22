/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <machine/registerset.h>
#ଅନ୍ତର୍ଭାଵ <object/structures.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/tcb.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    ଫେରନ୍ତୁ 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

#ifdef CONFIG_VTX
ଶୂନ୍ୟ Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_reply_to_user(tcb);
}

ସ୍ଥିର exception_t performSetEPTRoot(tcb_t *tcb, cap_t cap, cte_t *slot)
{
    cte_t *ମୂଳ_Slot;
    exception_t e;

    ମୂଳ_Slot = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(ମୂଳ_Slot, true);
    ଯଦି (e != EXCEPTION_NONE) {
        ଫେରନ୍ତୁ e;
    }

    cte_ଭର୍ତ୍ତି(cap, slot, ମୂଳ_Slot);

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t decodeSetEPTRoot(cap_t cap)
{
    cap_t ମୂଳ_Cap;
    cte_t *ମୂଳ_Slot;
    deriveCap_ret_t dc_ret;

    ମୂଳ_Slot = current_extra_caps.excaprefs[0];

    ଯଦି (ମୂଳ_Slot == ଖାଲି) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ମୂଳ_Cap = ମୂଳ_Slot->cap;

    ଯଦି (cap_get_capType(ମୂଳ_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(ମୂଳ_Slot, ମୂଳ_Cap);
    ଯଦି (dc_ret.status != EXCEPTION_NONE) {
        ଫେରନ୍ତୁ dc_ret.status;
    }

    ଯଦି (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଫେରନ୍ତୁ performSetEPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, ମୂଳ_Slot);
}
#endif
