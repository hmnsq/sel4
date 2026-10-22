/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <types.h>
#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <machine/registerset.h>
#அந்தர்பாவ <object/structures.h>
#அந்தர்பாவ <arch/object/tcb.h>
#அந்தர்பாவ <arch/machine.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    திரும்பு 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    திரும்பு EXCEPTION_NONE;
}

#ifdef CONFIG_VTX
வெறுமை Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_reply_to_user(tcb);
}

நிலையான exception_t performSetEPTRoot(tcb_t *tcb, cap_t cap, cte_t *slot)
{
    cte_t *வேர்_Slot;
    exception_t e;

    வேர்_Slot = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(வேர்_Slot, true);
    எனில் (e != EXCEPTION_NONE) {
        திரும்பு e;
    }

    cte_செருகு(cap, slot, வேர்_Slot);

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு EXCEPTION_NONE;
}

exception_t decodeSetEPTRoot(cap_t cap)
{
    cap_t வேர்_Cap;
    cte_t *வேர்_Slot;
    deriveCap_ret_t dc_ret;

    வேர்_Slot = current_extra_caps.excaprefs[0];

    எனில் (வேர்_Slot == வெற்று) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    வேர்_Cap = வேர்_Slot->cap;

    எனில் (cap_get_capType(வேர்_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(வேர்_Slot, வேர்_Cap);
    எனில் (dc_ret.status != EXCEPTION_NONE) {
        திரும்பு dc_ret.status;
    }

    எனில் (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    திரும்பு performSetEPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, வேர்_Slot);
}
#endif
