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

ಶಬ್ದ_t CONST Arch_decodeTransfer(ಶಬ್ದ_t flags)
{
    ಹಿಂತಿರುಗಿ 0;
}

exception_t CONST Arch_performTransfer(ಶಬ್ದ_t arch, tcb_t *tcb_src, tcb_t *tcb_dest)
{
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
ಶೂನ್ಯ Arch_leaveVMAsyncTransfer(tcb_t *tcb)
{
    vcpu_sysvmenter_ಉತ್ತರ_ಗಮ್ಯ_user(tcb);
}

ಸ್ಥಿರ exception_t perform_ಗಣ_EPTRoot(tcb_t *tcb, cap_t cap, cte_t *ಸ್ಥಾನ)
{
    cte_t *ಬೇರು_ಸ್ಥಾನ;
    exception_t e;

    ಬೇರು_ಸ್ಥಾನ = TCB_PTR_CTE_PTR(tcb, tcbArchEPTRoot);
    e = cteDelete(ಬೇರು_ಸ್ಥಾನ, true);
    ಆದರೆ (e != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ e;
    }

    cte_ಸೇರಿಸು(cap, ಸ್ಥಾನ, ಬೇರು_ಸ್ಥಾನ);

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decode_ಗಣ_EPTRoot(cap_t cap)
{
    cap_t ಬೇರು_Cap;
    cte_t *ಬೇರು_ಸ್ಥಾನ;
    deriveCap_ret_t dc_ret;

    ಬೇರು_ಸ್ಥಾನ = current_extra_caps.excaprefs[0];

    ಆದರೆ (ಬೇರು_ಸ್ಥಾನ == ಖಾಲಿ) {
        userError("TCB SetEPTRoot: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಬೇರು_Cap = ಬೇರು_ಸ್ಥಾನ->cap;

    ಆದರೆ (cap_get_capType(ಬೇರು_Cap) != cap_ept_pml4_cap) {
        userError("TCB SetEPTRoot: EPT PDPT is invalid.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    dc_ret = deriveCap(ಬೇರು_ಸ್ಥಾನ, ಬೇರು_Cap);
    ಆದರೆ (dc_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ dc_ret.ಸ್ಥಿತಿ_2;
    }

    ಆದರೆ (!cap_ept_pml4_cap_get_capPML4IsMapped(dc_ret.cap)) {
        userError("decodeSetEPTRoot: Invalid EPT cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಹಿಂತಿರುಗಿ perform_ಗಣ_EPTRoot(TCB_PTR(cap_thread_cap_get_capTCBPtr(cap)), dc_ret.cap, ಬೇರು_ಸ್ಥಾನ);
}
#ಆದರೆ_ಅಂತ್ಯ
