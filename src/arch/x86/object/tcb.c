/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <types.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <machine/registerset.h>
#అన్తర్భావ <object/structures.h>
#అన్తర్భావ <arch/object/tcb.h>
#అన్తర్భావ <arch/machine.h>

మాట_t CONST Arch_decodeTransfer(మాట_t flags)
{
    తిరిగి 0;
}

exception_t CONST Arch_performTransfer(మాట_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    తిరిగి EXCEPTION_NONE;
}

#నిర్వచితమైతే CONFIG_VTX
శూన్య Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_జవాబు_గమ్యం_user(tcb);
}

స్థిర exception_t perform_సమితి_EPTRoot(tcb_t *tcb, cap_t cap, cte_t *స్థానం)
{
    cte_t *మూలం_స్థానం;
    exception_t e;

    మూలం_స్థానం = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(మూలం_స్థానం, true);
    ఒకవేళ (e != EXCEPTION_NONE) {
        తిరిగి e;
    }

    cte_చొప్పించు(cap, స్థానం, మూలం_స్థానం);

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి EXCEPTION_NONE;
}

exception_t decode_సమితి_EPTRoot(cap_t cap)
{
    cap_t మూలం_Cap;
    cte_t *మూలం_స్థానం;
    deriveCap_ret_t dc_ret;

    మూలం_స్థానం = current_extra_caps.excaprefs[0];

    ఒకవేళ (మూలం_స్థానం == ఖాళీ) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    మూలం_Cap = మూలం_స్థానం->cap;

    ఒకవేళ (cap_get_capType(మూలం_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(మూలం_స్థానం, మూలం_Cap);
    ఒకవేళ (dc_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి dc_ret.స్థితి_2;
    }

    ఒకవేళ (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    తిరిగి perform_సమితి_EPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, మూలం_స్థానం);
}
#అయితే_ముగింపు
