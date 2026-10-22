/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2025, Indan Zupancic
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#समावेश <util.h>
#समावेश <api/syscall.h>
#समावेश <kernel/thread.h>
#समावेश <mode/api/ipc_buffer.h>
#समावेश <model/statedata.h>
#समावेश <object/domain.h>
#समावेश <object/structures.h>

/* Domain schedules. The duration is in kernel ticks for non-MCS and timer ticks for MCS. */
dschedule_t ksDomSchedule[CONFIG_अंक_DOMAIN_SCHEDULES];

अटल व्योम invokeDomainSetSet(tcb_t *tcb, dom_t domain)
{
    prepareSetDomain(tcb, domain);
    setDomain(tcb, domain);
}

अटल exception_t decodeDomainSetSetInvocation(word_t लंबाई, word_t *बफर)
{
    dom_t domain;
    cap_t tcap;

    अगर (unlikely(लंबाई == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    } अन्यथा {
        domain = getSyscallArg(0, बफर);
        अगर (domain >= अंक_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, अंक_Domains);
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_संख्या = 0;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
    }

    अगर (unlikely(current_extra_caps.excaprefs[0] == शून्य)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    अगर (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainSetSet(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    वापस EXCEPTION_NONE;
}

अटल व्योम invokeDomainScheduleConfigure(word_t सूचकांक, dom_t domain, ticks_t duration)
{
    ksDomSchedule[सूचकांक] = dschedule_make(domain, duration);
}

अटल exception_t decodeDomainScheduleConfigure(word_t लंबाई, word_t *बफर)
{
    word_t सूचकांक;
    dom_t domain;
    ticks_t duration;

    अगर (unlikely(लंबाई < 2 + TIME_ARG_आकार)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    सूचकांक = getSyscallArg(0, बफर);
    domain = getSyscallArg(1, बफर);
    duration = mode_parseTimeArg(2, बफर);

    /* Last entry must stay an end marker, hence the -1: */
    अगर (सूचकांक >= dom_Schedule_लंबाई - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_Schedule_लंबाई - 2;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    अगर (domain >= अंक_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = अंक_Domains - 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    अगर (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_Argument_संख्या = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    अगर (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_Argument_संख्या = 1;
        current_syscall_error.type = seL4_InvalidArgument;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    अगर (सूचकांक == ksDomScheduleStart && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_Argument_संख्या = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleConfigure(सूचकांक, domain, duration);
    वापस EXCEPTION_NONE;
}

अटल व्योम invokeDomainScheduleSetStart(word_t सूचकांक)
{
    ksDomScheduleStart = सूचकांक;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ksDomainTime = 0;
    ksDomScheduleIdx = dom_Schedule_लंबाई - 2;
    rescheduleRequired();
}

अटल exception_t decodeDomainScheduleSetStart(word_t लंबाई, word_t *बफर)
{
    word_t सूचकांक;

    अगर (unlikely(लंबाई < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    सूचकांक = getSyscallArg(0, बफर);

    अगर (सूचकांक >= dom_Schedule_लंबाई) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_Schedule_लंबाई - 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    अगर (dschedule_is_end_marker(सूचकांक)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_Argument_संख्या = 0;
        current_syscall_error.type = seL4_InvalidArgument;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleSetStart(सूचकांक);
    वापस EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(word_t invLabel, word_t लंबाई, word_t *बफर)
{
    चयन (invLabel) {
    हाल DomainSetSet:
        वापस decodeDomainSetSetInvocation(लंबाई, बफर);
    हाल DomainScheduleConfigure:
        वापस decodeDomainScheduleConfigure(लंबाई, बफर);
    हाल DomainScheduleSetStart:
        वापस decodeDomainScheduleSetStart(लंबाई, बफर);
    शेष:
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
}
