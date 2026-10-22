/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2025, Indan Zupancic
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ଅନ୍ତର୍ଭାଵ <util.h>
#ଅନ୍ତର୍ଭାଵ <api/syscall.h>
#ଅନ୍ତର୍ଭାଵ <kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <mode/api/ipc_buffer.h>
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <object/domain.h>
#ଅନ୍ତର୍ଭାଵ <object/structures.h>

/* Domain schedules. The duration is in kernel ticks for non-MCS and timer ticks for MCS. */
dschedule_t ksDomSchedule[CONFIG_ଅଙ୍କ_DOMAIN_SCHEDULES];

ସ୍ଥିର ଶୂନ୍ୟ invokeDomainSetSet(tcb_t *tcb, dom_t domain)
{
    prepareSetDomain(tcb, domain);
    setDomain(tcb, domain);
}

ସ୍ଥିର exception_t decodeDomainSetSetInvocation(word_t length, word_t *buffer)
{
    dom_t domain;
    cap_t tcap;

    ଯଦି (unlikely(length == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    } ଅନ୍ୟଥା {
        domain = getSyscallArg(0, buffer);
        ଯଦି (domain >= ଅଙ୍କ_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, ଅଙ୍କ_Domains);
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 0;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
    }

    ଯଦି (unlikely(current_extra_caps.excaprefs[0] == ଖାଲି)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    ଯଦି (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainSetSet(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର ଶୂନ୍ୟ invokeDomainScheduleConfigure(word_t index, dom_t domain, ticks_t duration)
{
    ksDomSchedule[index] = dschedule_make(domain, duration);
}

ସ୍ଥିର exception_t decodeDomainScheduleConfigure(word_t length, word_t *buffer)
{
    word_t index;
    dom_t domain;
    ticks_t duration;

    ଯଦି (unlikely(length < 2 + TIME_ARG_SIZE)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    domain = getSyscallArg(1, buffer);
    duration = mode_parseTimeArg(2, buffer);

    /* Last entry must stay an end marker, hence the -1: */
    ଯଦି (index >= domScheduleLength - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 2;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଯଦି (domain >= ଅଙ୍କ_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ଅଙ୍କ_Domains - 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଯଦି (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଯଦି (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 1;
        current_syscall_error.type = seL4_InvalidArgument;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଯଦି (index == ksDomScheduleStart && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleConfigure(index, domain, duration);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର ଶୂନ୍ୟ invokeDomainScheduleSetStart(word_t index)
{
    ksDomScheduleStart = index;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ksDomainTime = 0;
    ksDomScheduleIdx = domScheduleLength - 2;
    rescheduleRequired();
}

ସ୍ଥିର exception_t decodeDomainScheduleSetStart(word_t length, word_t *buffer)
{
    word_t index;

    ଯଦି (unlikely(length < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);

    ଯଦି (index >= domScheduleLength) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଯଦି (dschedule_is_end_marker(index)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 0;
        current_syscall_error.type = seL4_InvalidArgument;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleSetStart(index);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(word_t invLabel, word_t length, word_t *buffer)
{
    ବାଛ (invLabel) {
    ସ୍ଥିତି DomainSetSet:
        ଫେରନ୍ତୁ decodeDomainSetSetInvocation(length, buffer);
    ସ୍ଥିତି DomainScheduleConfigure:
        ଫେରନ୍ତୁ decodeDomainScheduleConfigure(length, buffer);
    ସ୍ଥିତି DomainScheduleSetStart:
        ଫେରନ୍ତୁ decodeDomainScheduleSetStart(length, buffer);
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
}
