/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2025, Indan Zupancic
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/api/ipc_buffer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>

/* Domain schedules. The duration is in kernel ticks for non-MCS and timer ticks for MCS. */
dschedule_t ks_Dom_ਸਮਾਂ_ਸਾਰਣੀ[CONFIG_ਅੰਕ_DOMAIN_SCHEDULES];

ਸਥਿਰ ਖਾਲੀ invoke_Domain_ਸੈੱਟ_ਸੈੱਟ(tcb_t *tcb, dom_t domain)
{
    prepare_ਸੈੱਟ_Domain(tcb, domain);
    ਸੈੱਟ_Domain(tcb, domain);
}

ਸਥਿਰ exception_t decode_Domain_ਸੈੱਟ_ਸੈੱਟ_Invocation(ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    dom_t domain;
    cap_t tcap;

    ਜੇ (unlikely(ਲੰਮਾਈ == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    } ਨਹੀਂਤਾਂ {
        domain = getSyscallArg(0, ਬਫਰ);
        ਜੇ (domain >= ਅੰਕ_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, ਅੰਕ_Domains);
            current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
            current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
    }

    ਜੇ (unlikely(current_extra_caps.excaprefs[0] == ਨਿਰਾ)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    ਜੇ (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_ਸੈੱਟ_ਸੈੱਟ(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ ਖਾਲੀ invoke_Domain_ਸਮਾਂ_ਸਾਰਣੀ_Configure(ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ, dom_t domain, ticks_t duration)
{
    ks_Dom_ਸਮਾਂ_ਸਾਰਣੀ[ਸੂਚਕਾਂਕ] = dschedule_make(domain, duration);
}

ਸਥਿਰ exception_t decode_Domain_ਸਮਾਂ_ਸਾਰਣੀ_Configure(ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ;
    dom_t domain;
    ticks_t duration;

    ਜੇ (unlikely(ਲੰਮਾਈ < 2 + ਸਮਾਂ_ARG_ਮਾਪ)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਸੂਚਕਾਂਕ = getSyscallArg(0, ਬਫਰ);
    domain = getSyscallArg(1, ਬਫਰ);
    duration = ਬਹੁਲਕ_parse_ਸਮਾਂ_Arg(2, ਬਫਰ);

    /* Last entry must stay an end marker, hence the -1: */
    ਜੇ (ਸੂਚਕਾਂਕ >= dom_ਸਮਾਂ_ਸਾਰਣੀ_ਲੰਮਾਈ - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_ਸਮਾਂ_ਸਾਰਣੀ_ਲੰਮਾਈ - 2;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (domain >= ਅੰਕ_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ਅੰਕ_Domains - 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 2;
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 1;
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (ਸੂਚਕਾਂਕ == ks_Dom_ਸਮਾਂ_ਸਾਰਣੀ_ਸ਼ੁਰੂ && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 2;
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_ਸਮਾਂ_ਸਾਰਣੀ_Configure(ਸੂਚਕਾਂਕ, domain, duration);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ ਖਾਲੀ invoke_Domain_ਸਮਾਂ_ਸਾਰਣੀ_ਸੈੱਟ_ਸ਼ੁਰੂ(ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ)
{
    ks_Dom_ਸਮਾਂ_ਸਾਰਣੀ_ਸ਼ੁਰੂ = ਸੂਚਕਾਂਕ;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ks_Domain_ਸਮਾਂ = 0;
    ks_Dom_ਸਮਾਂ_ਸਾਰਣੀ_Idx = dom_ਸਮਾਂ_ਸਾਰਣੀ_ਲੰਮਾਈ - 2;
    rescheduleRequired();
}

ਸਥਿਰ exception_t decode_Domain_ਸਮਾਂ_ਸਾਰਣੀ_ਸੈੱਟ_ਸ਼ੁਰੂ(ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ;

    ਜੇ (unlikely(ਲੰਮਾਈ < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਸੂਚਕਾਂਕ = getSyscallArg(0, ਬਫਰ);

    ਜੇ (ਸੂਚਕਾਂਕ >= dom_ਸਮਾਂ_ਸਾਰਣੀ_ਲੰਮਾਈ) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_ਸਮਾਂ_ਸਾਰਣੀ_ਲੰਮਾਈ - 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (dschedule_is_ਸਿਰਾ_marker(ਸੂਚਕਾਂਕ)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_ਸਮਾਂ_ਸਾਰਣੀ_ਸੈੱਟ_ਸ਼ੁਰੂ(ਸੂਚਕਾਂਕ);
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਚੋਣ (invLabel) {
    ਹਾਲਤ Domain_ਸੈੱਟ_ਸੈੱਟ:
        ਵਾਪਸ decode_Domain_ਸੈੱਟ_ਸੈੱਟ_Invocation(ਲੰਮਾਈ, ਬਫਰ);
    ਹਾਲਤ Domain_ਸਮਾਂ_ਸਾਰਣੀ_Configure:
        ਵਾਪਸ decode_Domain_ਸਮਾਂ_ਸਾਰਣੀ_Configure(ਲੰਮਾਈ, ਬਫਰ);
    ਹਾਲਤ Domain_ਸਮਾਂ_ਸਾਰਣੀ_ਸੈੱਟ_ਸ਼ੁਰੂ:
        ਵਾਪਸ decode_Domain_ਸਮਾਂ_ਸਾਰਣੀ_ਸੈੱਟ_ਸ਼ੁਰੂ(ਲੰਮਾਈ, ਬਫਰ);
    ਮੂਲ:
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}
