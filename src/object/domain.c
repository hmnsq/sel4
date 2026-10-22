/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2025, Indan Zupancic
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#അന്തര്ഭാവ <util.h>
#അന്തര്ഭാവ <api/syscall.h>
#അന്തര്ഭാവ <kernel/thread.h>
#അന്തര്ഭാവ <mode/api/ipc_buffer.h>
#അന്തര്ഭാവ <model/statedata.h>
#അന്തര്ഭാവ <object/domain.h>
#അന്തര്ഭാവ <object/structures.h>

/* Domain schedules. The duration is in kernel ticks for non-MCS and timer ticks for MCS. */
dschedule_t ksDomSchedule[CONFIG_അക്കം_DOMAIN_SCHEDULES];

സ്ഥിതം ശൂന്യം invokeDomainSetSet(tcb_t *tcb, dom_t domain)
{
    prepareSetDomain(tcb, domain);
    setDomain(tcb, domain);
}

സ്ഥിതം exception_t decodeDomainSetSetInvocation(word_t length, word_t *buffer)
{
    dom_t domain;
    cap_t tcap;

    എങ്കിൽ (unlikely(length == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    } അല്ലെങ്കിൽ {
        domain = getSyscallArg(0, buffer);
        എങ്കിൽ (domain >= അക്കം_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, അക്കം_Domains);
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_സംഖ്യ = 0;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
    }

    എങ്കിൽ (unlikely(current_extra_caps.excaprefs[0] == ശൂന്യ)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    എങ്കിൽ (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_സംഖ്യ = 1;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainSetSet(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം ശൂന്യം invokeDomainScheduleConfigure(word_t index, dom_t domain, ticks_t duration)
{
    ksDomSchedule[index] = dschedule_make(domain, duration);
}

സ്ഥിതം exception_t decodeDomainScheduleConfigure(word_t length, word_t *buffer)
{
    word_t index;
    dom_t domain;
    ticks_t duration;

    എങ്കിൽ (unlikely(length < 2 + TIME_ARG_SIZE)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    domain = getSyscallArg(1, buffer);
    duration = mode_parseTimeArg(2, buffer);

    /* Last entry must stay an end marker, hence the -1: */
    എങ്കിൽ (index >= domScheduleLength - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 2;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    എങ്കിൽ (domain >= അക്കം_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = അക്കം_Domains - 1;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    എങ്കിൽ (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_Argument_സംഖ്യ = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    എങ്കിൽ (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_Argument_സംഖ്യ = 1;
        current_syscall_error.type = seL4_InvalidArgument;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    എങ്കിൽ (index == ksDomScheduleStart && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_Argument_സംഖ്യ = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleConfigure(index, domain, duration);
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം ശൂന്യം invokeDomainScheduleSetStart(word_t index)
{
    ksDomScheduleStart = index;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ksDomainTime = 0;
    ksDomScheduleIdx = domScheduleLength - 2;
    rescheduleRequired();
}

സ്ഥിതം exception_t decodeDomainScheduleSetStart(word_t length, word_t *buffer)
{
    word_t index;

    എങ്കിൽ (unlikely(length < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);

    എങ്കിൽ (index >= domScheduleLength) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 1;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    എങ്കിൽ (dschedule_is_end_marker(index)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_Argument_സംഖ്യ = 0;
        current_syscall_error.type = seL4_InvalidArgument;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleSetStart(index);
    തിരികെ EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(word_t invLabel, word_t length, word_t *buffer)
{
    തിരഞ്ഞെടുപ്പ് (invLabel) {
    അവസ്ഥ DomainSetSet:
        തിരികെ decodeDomainSetSetInvocation(length, buffer);
    അവസ്ഥ DomainScheduleConfigure:
        തിരികെ decodeDomainScheduleConfigure(length, buffer);
    അവസ്ഥ DomainScheduleSetStart:
        തിരികെ decodeDomainScheduleSetStart(length, buffer);
    സ്ഥിരസ്ഥിതി:
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
}
