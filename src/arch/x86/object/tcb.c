/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <types.h>
#include <api/failures.h>
#include <machine/registerset.h>
#include <object/structures.h>
#include <arch/object/tcb.h>
#include <arch/machine.h>

word_t CONST Arch_decodeTransfer(word_t flags)
{
    return 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    return EXCEPTION_NONE;
}

#ifdef CONFIG_VTX
void Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_reply_to_user(tcb);
}

static exception_t performSetEPTRoot(tcb_t *tcb, cap_t cap, cte_t *slot)
{
    cte_t *उगात_Slot;
    exception_t e;

    उगात_Slot = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(उगात_Slot, true);
    if (e != EXCEPTION_NONE) {
        return e;
    }

    cte_इसिंगित(cap, slot, उगात_Slot);

    setThreadState(बुहोल_STATE(ksCurThread), ThreadState_Restart);
    return EXCEPTION_NONE;
}

exception_t decodeSetEPTRoot(cap_t cap)
{
    cap_t उगात_Cap;
    cte_t *उगात_Slot;
    deriveCap_ret_t dc_ret;

    उगात_Slot = current_extra_caps.excaprefs[0];

    if (उगात_Slot == NULL) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        return EXCEPTION_SYSCALL_ERROR;
    }

    उगात_Cap = उगात_Slot->cap;

    if (cap_get_capType(उगात_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        return EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(उगात_Slot, उगात_Cap);
    if (dc_ret.status != EXCEPTION_NONE) {
        return dc_ret.status;
    }

    if (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        return EXCEPTION_SYSCALL_ERROR;
    }

    return performSetEPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, उगात_Slot);
}
#endif
