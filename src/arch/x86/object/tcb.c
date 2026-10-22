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
    返回 0;
}

exception_t CONST Arch_performTransfer(word_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    返回 EXCEPTION_NONE;
}

#ifdef CONFIG_VTX
void Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_reply_to_user(tcb);
}

static exception_t performSetEPTRoot(tcb_t *tcb, cap_t cap, cte_t *slot)
{
    cte_t *rootSlot;
    exception_t e;

    rootSlot = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(rootSlot, true);
    如果 (e != EXCEPTION_NONE) {
        返回 e;
    }

    cteInsert(cap, slot, rootSlot);

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    返回 EXCEPTION_NONE;
}

exception_t decodeSetEPTRoot(cap_t cap)
{
    cap_t rootCap;
    cte_t *rootSlot;
    deriveCap_ret_t dc_ret;

    rootSlot = current_extra_caps.excaprefs[0];

    如果 (rootSlot == NULL) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        返回 EXCEPTION_SYSCALL_ERROR;
    }

    rootCap = rootSlot->cap;

    如果 (cap_get_capType(rootCap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        返回 EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(rootSlot, rootCap);
    如果 (dc_ret.status != EXCEPTION_NONE) {
        返回 dc_ret.status;
    }

    如果 (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        返回 EXCEPTION_SYSCALL_ERROR;
    }

    返回 performSetEPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, rootSlot);
}
#endif
