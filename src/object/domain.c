/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2025, Indan Zupancic
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#অন্তর্ভাব <util.h>
#অন্তর্ভাব <api/syscall.h>
#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <mode/api/ipc_buffer.h>
#অন্তর্ভাব <model/statedata.h>
#অন্তর্ভাব <object/domain.h>
#অন্তর্ভাব <object/structures.h>

/* Domain schedules. The duration is in kernel ticks for non-MCS and timer ticks for MCS. */
dschedule_t ksDomSchedule[CONFIG_অঙ্ক_DOMAIN_SCHEDULES];

স্থির শূন্য invokeDomainSetSet(tcb_t *tcb, dom_t domain)
{
    prepareSetDomain(tcb, domain);
    setDomain(tcb, domain);
}

স্থির exception_t decodeDomainSetSetInvocation(word_t দৈর্ঘ্য, word_t *বাফার)
{
    dom_t domain;
    cap_t tcap;

    যদি (unlikely(দৈর্ঘ্য == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    } অন্যথা {
        domain = getSyscallArg(0, বাফার);
        যদি (domain >= অঙ্ক_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, অঙ্ক_Domains);
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_সংখ্যা = 0;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
    }

    যদি (unlikely(current_extra_caps.excaprefs[0] == ফাঁকা)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    যদি (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 1;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainSetSet(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    ফেরত EXCEPTION_NONE;
}

স্থির শূন্য invokeDomainScheduleConfigure(word_t সূচক, dom_t domain, ticks_t duration)
{
    ksDomSchedule[সূচক] = dschedule_make(domain, duration);
}

স্থির exception_t decodeDomainScheduleConfigure(word_t দৈর্ঘ্য, word_t *বাফার)
{
    word_t সূচক;
    dom_t domain;
    ticks_t duration;

    যদি (unlikely(দৈর্ঘ্য < 2 + TIME_ARG_আকার)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    সূচক = getSyscallArg(0, বাফার);
    domain = getSyscallArg(1, বাফার);
    duration = mode_parseTimeArg(2, বাফার);

    /* Last entry must stay an end marker, hence the -1: */
    যদি (সূচক >= dom_Schedule_দৈর্ঘ্য - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_Schedule_দৈর্ঘ্য - 2;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    যদি (domain >= অঙ্ক_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = অঙ্ক_Domains - 1;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    যদি (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_Argument_সংখ্যা = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    যদি (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_Argument_সংখ্যা = 1;
        current_syscall_error.type = seL4_InvalidArgument;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    যদি (সূচক == ksDomScheduleStart && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_Argument_সংখ্যা = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleConfigure(সূচক, domain, duration);
    ফেরত EXCEPTION_NONE;
}

স্থির শূন্য invokeDomainScheduleSetStart(word_t সূচক)
{
    ksDomScheduleStart = সূচক;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ksDomainTime = 0;
    ksDomScheduleIdx = dom_Schedule_দৈর্ঘ্য - 2;
    rescheduleRequired();
}

স্থির exception_t decodeDomainScheduleSetStart(word_t দৈর্ঘ্য, word_t *বাফার)
{
    word_t সূচক;

    যদি (unlikely(দৈর্ঘ্য < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    সূচক = getSyscallArg(0, বাফার);

    যদি (সূচক >= dom_Schedule_দৈর্ঘ্য) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_Schedule_দৈর্ঘ্য - 1;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    যদি (dschedule_is_end_marker(সূচক)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_Argument_সংখ্যা = 0;
        current_syscall_error.type = seL4_InvalidArgument;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleSetStart(সূচক);
    ফেরত EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(word_t invLabel, word_t দৈর্ঘ্য, word_t *বাফার)
{
    নির্বাচন (invLabel) {
    ক্ষেত্র DomainSetSet:
        ফেরত decodeDomainSetSetInvocation(দৈর্ঘ্য, বাফার);
    ক্ষেত্র DomainScheduleConfigure:
        ফেরত decodeDomainScheduleConfigure(দৈর্ঘ্য, বাফার);
    ক্ষেত্র DomainScheduleSetStart:
        ফেরত decodeDomainScheduleSetStart(দৈর্ঘ্য, বাফার);
    পূর্বনির্ধারিত:
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
}
