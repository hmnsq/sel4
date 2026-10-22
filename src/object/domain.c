/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2025, Indan Zupancic
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <mode/api/ipc_buffer.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <object/domain.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>

/* Domain schedules. The duration is in kernel ticks for non-MCS and timer ticks for MCS. */
dschedule_t ks_Dom_ವೇಳಾಪಟ್ಟಿ[CONFIG_ಅಂಕೆ_DOMAIN_SCHEDULES];

ಸ್ಥಿರ ಶೂನ್ಯ invoke_Domain_ಗಣ_ಗಣ(tcb_t *tcb, dom_t domain)
{
    prepare_ಗಣ_Domain(tcb, domain);
    ಗಣ_Domain(tcb, domain);
}

ಸ್ಥಿರ exception_t decode_Domain_ಗಣ_ಗಣ_Invocation(ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    dom_t domain;
    cap_t tcap;

    ಆದರೆ (unlikely(ಉದ್ದಳತೆ == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    } ಇಲ್ಲದಿದ್ದರೆ {
        domain = getSyscallArg(0, ಬಫರ್);
        ಆದರೆ (domain >= ಅಂಕೆ_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, ಅಂಕೆ_Domains);
            current_syscall_error.type = se_L4_Invalid_ವಾದ;
            current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
    }

    ಆದರೆ (unlikely(current_extra_caps.excaprefs[0] == ಖಾಲಿ)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    ಆದರೆ (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_ಗಣ_ಗಣ(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ ಶೂನ್ಯ invoke_Domain_ವೇಳಾಪಟ್ಟಿ_Configure(ಶಬ್ದ_t ಸೂಚ್ಯಂಕ, dom_t domain, ticks_t duration)
{
    ks_Dom_ವೇಳಾಪಟ್ಟಿ[ಸೂಚ್ಯಂಕ] = dschedule_make(domain, duration);
}

ಸ್ಥಿರ exception_t decode_Domain_ವೇಳಾಪಟ್ಟಿ_Configure(ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ;
    dom_t domain;
    ticks_t duration;

    ಆದರೆ (unlikely(ಉದ್ದಳತೆ < 2 + ಸಮಯ_ARG_ಗಾತ್ರ)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಸೂಚ್ಯಂಕ = getSyscallArg(0, ಬಫರ್);
    domain = getSyscallArg(1, ಬಫರ್);
    duration = ಬಹುಲಕ_parse_ಸಮಯ_Arg(2, ಬಫರ್);

    /* Last entry must stay an end marker, hence the -1: */
    ಆದರೆ (ಸೂಚ್ಯಂಕ >= dom_ವೇಳಾಪಟ್ಟಿ_ಉದ್ದಳತೆ - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_ವೇಳಾಪಟ್ಟಿ_ಉದ್ದಳತೆ - 2;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (domain >= ಅಂಕೆ_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ಅಂಕೆ_Domains - 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 2;
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 1;
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (ಸೂಚ್ಯಂಕ == ks_Dom_ವೇಳಾಪಟ್ಟಿ_ಆರಂಭ && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 2;
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_ವೇಳಾಪಟ್ಟಿ_Configure(ಸೂಚ್ಯಂಕ, domain, duration);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ ಶೂನ್ಯ invoke_Domain_ವೇಳಾಪಟ್ಟಿ_ಗಣ_ಆರಂಭ(ಶಬ್ದ_t ಸೂಚ್ಯಂಕ)
{
    ks_Dom_ವೇಳಾಪಟ್ಟಿ_ಆರಂಭ = ಸೂಚ್ಯಂಕ;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ks_Domain_ಸಮಯ = 0;
    ks_Dom_ವೇಳಾಪಟ್ಟಿ_Idx = dom_ವೇಳಾಪಟ್ಟಿ_ಉದ್ದಳತೆ - 2;
    rescheduleRequired();
}

ಸ್ಥಿರ exception_t decode_Domain_ವೇಳಾಪಟ್ಟಿ_ಗಣ_ಆರಂಭ(ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ;

    ಆದರೆ (unlikely(ಉದ್ದಳತೆ < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಸೂಚ್ಯಂಕ = getSyscallArg(0, ಬಫರ್);

    ಆದರೆ (ಸೂಚ್ಯಂಕ >= dom_ವೇಳಾಪಟ್ಟಿ_ಉದ್ದಳತೆ) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_ವೇಳಾಪಟ್ಟಿ_ಉದ್ದಳತೆ - 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (dschedule_is_ಕೊನೆ_marker(ಸೂಚ್ಯಂಕ)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_ವೇಳಾಪಟ್ಟಿ_ಗಣ_ಆರಂಭ(ಸೂಚ್ಯಂಕ);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆಯ್ಕೆ (invLabel) {
    ಸ್ಥಿತಿ Domain_ಗಣ_ಗಣ:
        ಹಿಂತಿರುಗಿ decode_Domain_ಗಣ_ಗಣ_Invocation(ಉದ್ದಳತೆ, ಬಫರ್);
    ಸ್ಥಿತಿ Domain_ವೇಳಾಪಟ್ಟಿ_Configure:
        ಹಿಂತಿರುಗಿ decode_Domain_ವೇಳಾಪಟ್ಟಿ_Configure(ಉದ್ದಳತೆ, ಬಫರ್);
    ಸ್ಥಿತಿ Domain_ವೇಳಾಪಟ್ಟಿ_ಗಣ_ಆರಂಭ:
        ಹಿಂತಿರುಗಿ decode_Domain_ವೇಳಾಪಟ್ಟಿ_ಗಣ_ಆರಂಭ(ಉದ್ದಳತೆ, ಬಫರ್);
    ಪೂರ್ವನಿಯೋಜಿತ:
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}
