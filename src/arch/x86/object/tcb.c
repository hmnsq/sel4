/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <machine/registerset.h>
#ضمن <object/structures.h>
#ضمن <arch/object/tcb.h>
#ضمن <arch/machine.h>

لفظ_t CONST Arch_decodeTransfer(لفظ_t flags)
{
    أرجع 0;
}

exception_t CONST Arch_performTransfer(لفظ_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    أرجع EXCEPTION_NONE;
}

#جيڪڏهن_وصف_ٿيل CONFIG_VTX
فراغ Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_جواب_منزل_user(tcb);
}

ساكن exception_t perform_سيٽ_EPTRoot(tcb_t *tcb, cap_t cap, cte_t *جڳهه)
{
    cte_t *پاڙ_جڳهه;
    exception_t e;

    پاڙ_جڳهه = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(پاڙ_جڳهه, true);
    إذا (e != EXCEPTION_NONE) {
        أرجع e;
    }

    cte_وجهو(cap, جڳهه, پاڙ_جڳهه);

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع EXCEPTION_NONE;
}

exception_t decode_سيٽ_EPTRoot(cap_t cap)
{
    cap_t پاڙ_Cap;
    cte_t *پاڙ_جڳهه;
    deriveCap_ret_t dc_ret;

    پاڙ_جڳهه = current_extra_caps.excaprefs[0];

    إذا (پاڙ_جڳهه == عدم) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    پاڙ_Cap = پاڙ_جڳهه->cap;

    إذا (cap_get_capType(پاڙ_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(پاڙ_جڳهه, پاڙ_Cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        أرجع dc_ret.حالت;
    }

    إذا (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع perform_سيٽ_EPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, پاڙ_جڳهه);
}
#جيڪڏهن_پورو
