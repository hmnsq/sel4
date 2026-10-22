/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2025, Indan Zupancic
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#అన్తర్భావ <util.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <mode/api/ipc_buffer.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <object/domain.h>
#అన్తర్భావ <object/structures.h>

/* Domain schedules. The duration is in kernel ticks for non-MCS and timer ticks for MCS. */
dschedule_t ks_Dom_కాలపట్టిక[CONFIG_అంకె_DOMAIN_SCHEDULES];

స్థిర శూన్య invoke_Domain_సమితి_సమితి(tcb_t *tcb, dom_t domain)
{
    prepare_సమితి_Domain(tcb, domain);
    సమితి_Domain(tcb, domain);
}

స్థిర exception_t decode_Domain_సమితి_సమితి_Invocation(మాట_t నిడివి, మాట_t *బఫరు)
{
    dom_t domain;
    cap_t tcap;

    ఒకవేళ (unlikely(నిడివి == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    } లేకపోతే {
        domain = getSyscallArg(0, బఫరు);
        ఒకవేళ (domain >= అంకె_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, అంకె_Domains);
            current_syscall_error.type = se_L4_Invalid_వాదం;
            current_syscall_error.invalid_వాదం_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
    }

    ఒకవేళ (unlikely(current_extra_caps.excaprefs[0] == ఖాళీ)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    ఒకవేళ (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_సమితి_సమితి(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    తిరిగి EXCEPTION_NONE;
}

స్థిర శూన్య invoke_Domain_కాలపట్టిక_Configure(మాట_t సూచీ, dom_t domain, ticks_t duration)
{
    ks_Dom_కాలపట్టిక[సూచీ] = dschedule_make(domain, duration);
}

స్థిర exception_t decode_Domain_కాలపట్టిక_Configure(మాట_t నిడివి, మాట_t *బఫరు)
{
    మాట_t సూచీ;
    dom_t domain;
    ticks_t duration;

    ఒకవేళ (unlikely(నిడివి < 2 + సమయం_ARG_కొలత)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    సూచీ = getSyscallArg(0, బఫరు);
    domain = getSyscallArg(1, బఫరు);
    duration = బహుళకం_parse_సమయం_Arg(2, బఫరు);

    /* Last entry must stay an end marker, hence the -1: */
    ఒకవేళ (సూచీ >= dom_కాలపట్టిక_నిడివి - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_కాలపట్టిక_నిడివి - 2;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (domain >= అంకె_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = అంకె_Domains - 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_వాదం_సంఖ్య = 2;
        current_syscall_error.type = se_L4_Invalid_వాదం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_వాదం_సంఖ్య = 1;
        current_syscall_error.type = se_L4_Invalid_వాదం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (సూచీ == ks_Dom_కాలపట్టిక_ప్రారంభం && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_వాదం_సంఖ్య = 2;
        current_syscall_error.type = se_L4_Invalid_వాదం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_కాలపట్టిక_Configure(సూచీ, domain, duration);
    తిరిగి EXCEPTION_NONE;
}

స్థిర శూన్య invoke_Domain_కాలపట్టిక_సమితి_ప్రారంభం(మాట_t సూచీ)
{
    ks_Dom_కాలపట్టిక_ప్రారంభం = సూచీ;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ks_Domain_సమయం = 0;
    ks_Dom_కాలపట్టిక_Idx = dom_కాలపట్టిక_నిడివి - 2;
    rescheduleRequired();
}

స్థిర exception_t decode_Domain_కాలపట్టిక_సమితి_ప్రారంభం(మాట_t నిడివి, మాట_t *బఫరు)
{
    మాట_t సూచీ;

    ఒకవేళ (unlikely(నిడివి < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    సూచీ = getSyscallArg(0, బఫరు);

    ఒకవేళ (సూచీ >= dom_కాలపట్టిక_నిడివి) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_కాలపట్టిక_నిడివి - 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (dschedule_is_చివర_marker(సూచీ)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_వాదం_సంఖ్య = 0;
        current_syscall_error.type = se_L4_Invalid_వాదం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_కాలపట్టిక_సమితి_ప్రారంభం(సూచీ);
    తిరిగి EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(మాట_t invLabel, మాట_t నిడివి, మాట_t *బఫరు)
{
    ఎంపిక (invLabel) {
    స్థితి Domain_సమితి_సమితి:
        తిరిగి decode_Domain_సమితి_సమితి_Invocation(నిడివి, బఫరు);
    స్థితి Domain_కాలపట్టిక_Configure:
        తిరిగి decode_Domain_కాలపట్టిక_Configure(నిడివి, బఫరు);
    స్థితి Domain_కాలపట్టిక_సమితి_ప్రారంభం:
        తిరిగి decode_Domain_కాలపట్టిక_సమితి_ప్రారంభం(నిడివి, బఫరు);
    అప్రమేయ:
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}
