/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2025, Indan Zupancic
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#شامل <util.h>
#شامل <api/syscall.h>
#شامل <kernel/thread.h>
#شامل <mode/api/ipc_buffer.h>
#شامل <model/statedata.h>
#شامل <object/domain.h>
#شامل <object/structures.h>

/* Domain schedules. The duration is in kernel ticks for non-MCS and timer ticks for MCS. */
dschedule_t ksDomSchedule[CONFIG_رقم_DOMAIN_SCHEDULES];

ایستا تهی invokeDomainSetSet(tcb_t *tcb, dom_t domain)
{
    prepareSetDomain(tcb, domain);
    setDomain(tcb, domain);
}

ایستا exception_t decodeDomainSetSetInvocation(word_t length, word_t *buffer)
{
    dom_t domain;
    cap_t tcap;

    اگر (unlikely(length == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    } وگرنه {
        domain = getSyscallArg(0, buffer);
        اگر (domain >= رقم_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, رقم_Domains);
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_عدد = 0;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
    }

    اگر (unlikely(current_extra_caps.excaprefs[0] == پوچ)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    اگر (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_عدد = 1;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainSetSet(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    بازگشت EXCEPTION_NONE;
}

ایستا تهی invokeDomainScheduleConfigure(word_t index, dom_t domain, ticks_t duration)
{
    ksDomSchedule[index] = dschedule_make(domain, duration);
}

ایستا exception_t decodeDomainScheduleConfigure(word_t length, word_t *buffer)
{
    word_t index;
    dom_t domain;
    ticks_t duration;

    اگر (unlikely(length < 2 + TIME_ARG_SIZE)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    domain = getSyscallArg(1, buffer);
    duration = mode_parseTimeArg(2, buffer);

    /* Last entry must stay an end marker, hence the -1: */
    اگر (index >= domScheduleLength - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 2;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    اگر (domain >= رقم_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = رقم_Domains - 1;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    اگر (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_Argument_عدد = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    اگر (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_Argument_عدد = 1;
        current_syscall_error.type = seL4_InvalidArgument;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    اگر (index == ksDomScheduleStart && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_Argument_عدد = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleConfigure(index, domain, duration);
    بازگشت EXCEPTION_NONE;
}

ایستا تهی invokeDomainScheduleSetStart(word_t index)
{
    ksDomScheduleStart = index;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ksDomainTime = 0;
    ksDomScheduleIdx = domScheduleLength - 2;
    rescheduleRequired();
}

ایستا exception_t decodeDomainScheduleSetStart(word_t length, word_t *buffer)
{
    word_t index;

    اگر (unlikely(length < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);

    اگر (index >= domScheduleLength) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 1;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    اگر (dschedule_is_end_marker(index)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_Argument_عدد = 0;
        current_syscall_error.type = seL4_InvalidArgument;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleSetStart(index);
    بازگشت EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(word_t invLabel, word_t length, word_t *buffer)
{
    انتخاب (invLabel) {
    حالت DomainSetSet:
        بازگشت decodeDomainSetSetInvocation(length, buffer);
    حالت DomainScheduleConfigure:
        بازگشت decodeDomainScheduleConfigure(length, buffer);
    حالت DomainScheduleSetStart:
        بازگشت decodeDomainScheduleSetStart(length, buffer);
    پیشفرض:
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
}
