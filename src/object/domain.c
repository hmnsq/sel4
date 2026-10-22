/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2025, Indan Zupancic
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#અન્તર્ભાવ <util.h>
#અન્તર્ભાવ <api/syscall.h>
#અન્તર્ભાવ <kernel/thread.h>
#અન્તર્ભાવ <mode/api/ipc_buffer.h>
#અન્તર્ભાવ <model/statedata.h>
#અન્તર્ભાવ <object/domain.h>
#અન્તર્ભાવ <object/structures.h>

/* Domain schedules. The duration is in kernel ticks for non-MCS and timer ticks for MCS. */
dschedule_t ksDomSchedule[CONFIG_અંક_DOMAIN_SCHEDULES];

સ્થિર શૂન્ય invokeDomainSetSet(tcb_t *tcb, dom_t domain)
{
    prepareSetDomain(tcb, domain);
    setDomain(tcb, domain);
}

સ્થિર exception_t decodeDomainSetSetInvocation(word_t length, word_t *buffer)
{
    dom_t domain;
    cap_t tcap;

    જો (unlikely(length == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    } નહીંતર {
        domain = getSyscallArg(0, buffer);
        જો (domain >= અંક_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, અંક_Domains);
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_સંખ્યા = 0;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
    }

    જો (unlikely(current_extra_caps.excaprefs[0] == ખાલી)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    જો (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainSetSet(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    પાછા EXCEPTION_NONE;
}

સ્થિર શૂન્ય invokeDomainScheduleConfigure(word_t index, dom_t domain, ticks_t duration)
{
    ksDomSchedule[index] = dschedule_make(domain, duration);
}

સ્થિર exception_t decodeDomainScheduleConfigure(word_t length, word_t *buffer)
{
    word_t index;
    dom_t domain;
    ticks_t duration;

    જો (unlikely(length < 2 + TIME_ARG_SIZE)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    domain = getSyscallArg(1, buffer);
    duration = mode_parseTimeArg(2, buffer);

    /* Last entry must stay an end marker, hence the -1: */
    જો (index >= domScheduleLength - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 2;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    જો (domain >= અંક_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = અંક_Domains - 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    જો (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_Argument_સંખ્યા = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    જો (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_Argument_સંખ્યા = 1;
        current_syscall_error.type = seL4_InvalidArgument;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    જો (index == ksDomScheduleStart && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_Argument_સંખ્યા = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleConfigure(index, domain, duration);
    પાછા EXCEPTION_NONE;
}

સ્થિર શૂન્ય invokeDomainScheduleSetStart(word_t index)
{
    ksDomScheduleStart = index;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ksDomainTime = 0;
    ksDomScheduleIdx = domScheduleLength - 2;
    rescheduleRequired();
}

સ્થિર exception_t decodeDomainScheduleSetStart(word_t length, word_t *buffer)
{
    word_t index;

    જો (unlikely(length < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);

    જો (index >= domScheduleLength) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    જો (dschedule_is_end_marker(index)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_Argument_સંખ્યા = 0;
        current_syscall_error.type = seL4_InvalidArgument;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleSetStart(index);
    પાછા EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(word_t invLabel, word_t length, word_t *buffer)
{
    પસંદગી (invLabel) {
    સ્થિતિ DomainSetSet:
        પાછા decodeDomainSetSetInvocation(length, buffer);
    સ્થિતિ DomainScheduleConfigure:
        પાછા decodeDomainScheduleConfigure(length, buffer);
    સ્થિતિ DomainScheduleSetStart:
        પાછા decodeDomainScheduleSetStart(length, buffer);
    મૂળભૂત:
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
}
