/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <types.h>
#समावेश <api/failures.h>
#समावेश <machine/registerset.h>
#समावेश <object/structures.h>
#समावेश <arch/object/tcb.h>
#समावेश <arch/machine.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    वापस 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    वापस EXCEPTION_NONE;
}

#अगर_घोषित CONFIG_VTX
व्योम Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_reply_to_user(tcb);
}

अटल exception_t performSetEPTRoot(tcb_t *tcb, cap_t cap, cte_t *slot)
{
    cte_t *मूल_Slot;
    exception_t e;

    मूल_Slot = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(मूल_Slot, true);
    अगर (e != EXCEPTION_NONE) {
        वापस e;
    }

    cte_जोड़ो(cap, slot, मूल_Slot);

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस EXCEPTION_NONE;
}

exception_t decodeSetEPTRoot(cap_t cap)
{
    cap_t मूल_Cap;
    cte_t *मूल_Slot;
    deriveCap_ret_t dc_ret;

    मूल_Slot = current_extra_caps.excaprefs[0];

    अगर (मूल_Slot == शून्य) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    मूल_Cap = मूल_Slot->cap;

    अगर (cap_get_capType(मूल_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(मूल_Slot, मूल_Cap);
    अगर (dc_ret.status != EXCEPTION_NONE) {
        वापस dc_ret.status;
    }

    अगर (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    वापस performSetEPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, मूल_Slot);
}
#पूर्ण_अगर
