/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2025, Indan Zupancic
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#அந்தர்பாவ <util.h>
#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <mode/api/ipc_buffer.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <object/domain.h>
#அந்தர்பாவ <object/structures.h>

/* Domain schedules. The duration is in kernel ticks for non-MCS and timer ticks for MCS. */
dschedule_t ksDomSchedule[CONFIG_இலக்கம்_DOMAIN_SCHEDULES];

நிலையான வெறுமை invokeDomainSetSet(tcb_t *tcb, dom_t domain)
{
    prepareSetDomain(tcb, domain);
    setDomain(tcb, domain);
}

நிலையான exception_t decodeDomainSetSetInvocation(word_t length, word_t *buffer)
{
    dom_t domain;
    cap_t tcap;

    எனில் (unlikely(length == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    } இல்லையெனில் {
        domain = getSyscallArg(0, buffer);
        எனில் (domain >= இலக்கம்_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, இலக்கம்_Domains);
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_எண் = 0;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
    }

    எனில் (unlikely(current_extra_caps.excaprefs[0] == வெற்று)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    எனில் (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainSetSet(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    திரும்பு EXCEPTION_NONE;
}

நிலையான வெறுமை invokeDomainScheduleConfigure(word_t index, dom_t domain, ticks_t duration)
{
    ksDomSchedule[index] = dschedule_make(domain, duration);
}

நிலையான exception_t decodeDomainScheduleConfigure(word_t length, word_t *buffer)
{
    word_t index;
    dom_t domain;
    ticks_t duration;

    எனில் (unlikely(length < 2 + TIME_ARG_SIZE)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    domain = getSyscallArg(1, buffer);
    duration = mode_parseTimeArg(2, buffer);

    /* Last entry must stay an end marker, hence the -1: */
    எனில் (index >= domScheduleLength - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 2;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    எனில் (domain >= இலக்கம்_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = இலக்கம்_Domains - 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    எனில் (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_Argument_எண் = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    எனில் (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_Argument_எண் = 1;
        current_syscall_error.type = seL4_InvalidArgument;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    எனில் (index == ksDomScheduleStart && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_Argument_எண் = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleConfigure(index, domain, duration);
    திரும்பு EXCEPTION_NONE;
}

நிலையான வெறுமை invokeDomainScheduleSetStart(word_t index)
{
    ksDomScheduleStart = index;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ksDomainTime = 0;
    ksDomScheduleIdx = domScheduleLength - 2;
    rescheduleRequired();
}

நிலையான exception_t decodeDomainScheduleSetStart(word_t length, word_t *buffer)
{
    word_t index;

    எனில் (unlikely(length < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);

    எனில் (index >= domScheduleLength) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    எனில் (dschedule_is_end_marker(index)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_Argument_எண் = 0;
        current_syscall_error.type = seL4_InvalidArgument;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleSetStart(index);
    திரும்பு EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(word_t invLabel, word_t length, word_t *buffer)
{
    தேர்வு (invLabel) {
    நிலை DomainSetSet:
        திரும்பு decodeDomainSetSetInvocation(length, buffer);
    நிலை DomainScheduleConfigure:
        திரும்பு decodeDomainScheduleConfigure(length, buffer);
    நிலை DomainScheduleSetStart:
        திரும்பு decodeDomainScheduleSetStart(length, buffer);
    இயல்பு:
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
}
