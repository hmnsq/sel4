/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2025, Indan Zupancic
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ضمن <util.h>
#ضمن <api/syscall.h>
#ضمن <kernel/thread.h>
#ضمن <mode/api/ipc_buffer.h>
#ضمن <model/statedata.h>
#ضمن <object/domain.h>
#ضمن <object/structures.h>

/* Domain schedules. The duration is in kernel ticks for non-MCS and timer ticks for MCS. */
dschedule_t ksDomSchedule[CONFIG_عدد_DOMAIN_SCHEDULES];

ساكن فراغ invokeDomainSetSet(tcb_t *tcb, dom_t domain)
{
    prepareSetDomain(tcb, domain);
    setDomain(tcb, domain);
}

ساكن exception_t decodeDomainSetSetInvocation(word_t length, word_t *buffer)
{
    dom_t domain;
    cap_t tcap;

    إذا (unlikely(length == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    } وإلا {
        domain = getSyscallArg(0, buffer);
        إذا (domain >= عدد_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, عدد_Domains);
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_نمبر = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    إذا (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(گرہ_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainSetSet(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    أرجع EXCEPTION_NONE;
}

ساكن فراغ invokeDomainScheduleConfigure(word_t index, dom_t domain, ticks_t duration)
{
    ksDomSchedule[index] = dschedule_make(domain, duration);
}

ساكن exception_t decodeDomainScheduleConfigure(word_t length, word_t *buffer)
{
    word_t index;
    dom_t domain;
    ticks_t duration;

    إذا (unlikely(length < 2 + TIME_ARG_SIZE)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    domain = getSyscallArg(1, buffer);
    duration = mode_parseTimeArg(2, buffer);

    /* Last entry must stay an end marker, hence the -1: */
    إذا (index >= domScheduleLength - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (domain >= عدد_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = عدد_Domains - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_Argument_نمبر = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_Argument_نمبر = 1;
        current_syscall_error.type = seL4_InvalidArgument;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (index == ksDomScheduleStart && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_Argument_نمبر = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(گرہ_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleConfigure(index, domain, duration);
    أرجع EXCEPTION_NONE;
}

ساكن فراغ invokeDomainScheduleSetStart(word_t index)
{
    ksDomScheduleStart = index;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ksDomainTime = 0;
    ksDomScheduleIdx = domScheduleLength - 2;
    rescheduleRequired();
}

ساكن exception_t decodeDomainScheduleSetStart(word_t length, word_t *buffer)
{
    word_t index;

    إذا (unlikely(length < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);

    إذا (index >= domScheduleLength) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (dschedule_is_end_marker(index)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_Argument_نمبر = 0;
        current_syscall_error.type = seL4_InvalidArgument;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(گرہ_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleSetStart(index);
    أرجع EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(word_t invLabel, word_t length, word_t *buffer)
{
    بدل (invLabel) {
    حالة DomainSetSet:
        أرجع decodeDomainSetSetInvocation(length, buffer);
    حالة DomainScheduleConfigure:
        أرجع decodeDomainScheduleConfigure(length, buffer);
    حالة DomainScheduleSetStart:
        أرجع decodeDomainScheduleSetStart(length, buffer);
    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
