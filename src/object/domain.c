/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2025, Indan Zupancic
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#अन्तर्भाव <util.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <mode/api/ipc_buffer.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <object/domain.h>
#अन्तर्भाव <object/structures.h>

/* Domain schedules. The duration is in kernel ticks for non-MCS and timer ticks for MCS. */
dschedule_t ksDomSchedule[CONFIG_अंक_DOMAIN_SCHEDULES];

स्थिर शून्य invokeDomainSetSet(tcb_t *tcb, dom_t domain)
{
    prepareSetDomain(tcb, domain);
    setDomain(tcb, domain);
}

स्थिर exception_t decodeDomainSetSetInvocation(word_t length, word_t *buffer)
{
    dom_t domain;
    cap_t tcap;

    जर (unlikely(length == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        परत EXCEPTION_SYSCALL_ERROR;
    } अन्यथा {
        domain = getSyscallArg(0, buffer);
        जर (domain >= अंक_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, अंक_Domains);
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_संख्या = 0;
            परत EXCEPTION_SYSCALL_ERROR;
        }
    }

    जर (unlikely(current_extra_caps.excaprefs[0] == रिक्त)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        परत EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    जर (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 1;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainSetSet(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    परत EXCEPTION_NONE;
}

स्थिर शून्य invokeDomainScheduleConfigure(word_t index, dom_t domain, ticks_t duration)
{
    ksDomSchedule[index] = dschedule_make(domain, duration);
}

स्थिर exception_t decodeDomainScheduleConfigure(word_t length, word_t *buffer)
{
    word_t index;
    dom_t domain;
    ticks_t duration;

    जर (unlikely(length < 2 + TIME_ARG_SIZE)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    domain = getSyscallArg(1, buffer);
    duration = mode_parseTimeArg(2, buffer);

    /* Last entry must stay an end marker, hence the -1: */
    जर (index >= domScheduleLength - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 2;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    जर (domain >= अंक_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = अंक_Domains - 1;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    जर (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_Argument_संख्या = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    जर (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_Argument_संख्या = 1;
        current_syscall_error.type = seL4_InvalidArgument;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    जर (index == ksDomScheduleStart && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_Argument_संख्या = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleConfigure(index, domain, duration);
    परत EXCEPTION_NONE;
}

स्थिर शून्य invokeDomainScheduleSetStart(word_t index)
{
    ksDomScheduleStart = index;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ksDomainTime = 0;
    ksDomScheduleIdx = domScheduleLength - 2;
    rescheduleRequired();
}

स्थिर exception_t decodeDomainScheduleSetStart(word_t length, word_t *buffer)
{
    word_t index;

    जर (unlikely(length < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);

    जर (index >= domScheduleLength) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 1;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    जर (dschedule_is_end_marker(index)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_Argument_संख्या = 0;
        current_syscall_error.type = seL4_InvalidArgument;
        परत EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(गाठ_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleSetStart(index);
    परत EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(word_t invLabel, word_t length, word_t *buffer)
{
    निवड (invLabel) {
    स्थिती DomainSetSet:
        परत decodeDomainSetSetInvocation(length, buffer);
    स्थिती DomainScheduleConfigure:
        परत decodeDomainScheduleConfigure(length, buffer);
    स्थिती DomainScheduleSetStart:
        परत decodeDomainScheduleSetStart(length, buffer);
    पूर्वनिर्धारित:
        current_syscall_error.type = seL4_IllegalOperation;
        परत EXCEPTION_SYSCALL_ERROR;
    }
}
