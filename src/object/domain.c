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
dschedule_t ksDomSchedule[CONFIG_अङ्कः_DOMAIN_SCHEDULES];

स्थिर शून्य invokeDomainSetSet(tcb_t *tcb, dom_t domain)
{
    prepareSetDomain(tcb, domain);
    setDomain(tcb, domain);
}

स्थिर exception_t decodeDomainSetSetInvocation(word_t length, word_t *buffer)
{
    dom_t domain;
    cap_t tcap;

    सचे (unlikely(length == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    } अञ्ञथा {
        domain = getSyscallArg(0, buffer);
        सचे (domain >= अङ्कः_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, अङ्कः_Domains);
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_सङ्ख्या = 0;
            पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
        }
    }

    सचे (unlikely(current_extra_caps.excaprefs[0] == रिक्त)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    सचे (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainSetSet(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    पटिनिवत्त EXCEPTION_NONE;
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

    सचे (unlikely(length < 2 + TIME_ARG_SIZE)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    domain = getSyscallArg(1, buffer);
    duration = mode_parseTimeArg(2, buffer);

    /* Last entry must stay an end marker, hence the -1: */
    सचे (index >= domScheduleLength - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 2;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    सचे (domain >= अङ्कः_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = अङ्कः_Domains - 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    सचे (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_Argument_सङ्ख्या = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    सचे (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_Argument_सङ्ख्या = 1;
        current_syscall_error.type = seL4_InvalidArgument;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    सचे (index == ksDomScheduleStart && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_Argument_सङ्ख्या = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleConfigure(index, domain, duration);
    पटिनिवत्त EXCEPTION_NONE;
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

    सचे (unlikely(length < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);

    सचे (index >= domScheduleLength) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    सचे (dschedule_is_end_marker(index)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_Argument_सङ्ख्या = 0;
        current_syscall_error.type = seL4_InvalidArgument;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleSetStart(index);
    पटिनिवत्त EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(word_t invLabel, word_t length, word_t *buffer)
{
    विकल्प (invLabel) {
    पक्ष DomainSetSet:
        पटिनिवत्त decodeDomainSetSetInvocation(length, buffer);
    पक्ष DomainScheduleConfigure:
        पटिनिवत्त decodeDomainScheduleConfigure(length, buffer);
    पक्ष DomainScheduleSetStart:
        पटिनिवत्त decodeDomainScheduleSetStart(length, buffer);
    स्वभाव:
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
}
