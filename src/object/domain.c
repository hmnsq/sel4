/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2025, Indan Zupancic
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <mode/api/ipc_buffer.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <object/domain.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>

/* Domain schedules. The duration is in kernel ticks for non-MCS and timer ticks for MCS. */
dschedule_t ksDomSchedule[CONFIG_ಅಂಕೆ_DOMAIN_SCHEDULES];

ಸ್ಥಿರ ಶೂನ್ಯ invokeDomainSetSet(tcb_t *tcb, dom_t domain)
{
    prepareSetDomain(tcb, domain);
    setDomain(tcb, domain);
}

ಸ್ಥಿರ exception_t decodeDomainSetSetInvocation(word_t length, word_t *buffer)
{
    dom_t domain;
    cap_t tcap;

    ಯದಿ (unlikely(length == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    } ಅನ್ಯಥಾ {
        domain = getSyscallArg(0, buffer);
        ಯದಿ (domain >= ಅಂಕೆ_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, ಅಂಕೆ_Domains);
            current_syscall_error.type = seL4_InvalidArgument;
            current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 0;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
    }

    ಯದಿ (unlikely(current_extra_caps.excaprefs[0] == ರಿಕ್ತ)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    ಯದಿ (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainSetSet(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ ಶೂನ್ಯ invokeDomainScheduleConfigure(word_t index, dom_t domain, ticks_t duration)
{
    ksDomSchedule[index] = dschedule_make(domain, duration);
}

ಸ್ಥಿರ exception_t decodeDomainScheduleConfigure(word_t length, word_t *buffer)
{
    word_t index;
    dom_t domain;
    ticks_t duration;

    ಯದಿ (unlikely(length < 2 + TIME_ARG_SIZE)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);
    domain = getSyscallArg(1, buffer);
    duration = mode_parseTimeArg(2, buffer);

    /* Last entry must stay an end marker, hence the -1: */
    ಯದಿ (index >= domScheduleLength - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 2;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಯದಿ (domain >= ಅಂಕೆ_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ಅಂಕೆ_Domains - 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಯದಿ (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಯದಿ (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 1;
        current_syscall_error.type = seL4_InvalidArgument;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಯದಿ (index == ksDomScheduleStart && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 2;
        current_syscall_error.type = seL4_InvalidArgument;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleConfigure(index, domain, duration);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ ಶೂನ್ಯ invokeDomainScheduleSetStart(word_t index)
{
    ksDomScheduleStart = index;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ksDomainTime = 0;
    ksDomScheduleIdx = domScheduleLength - 2;
    rescheduleRequired();
}

ಸ್ಥಿರ exception_t decodeDomainScheduleSetStart(word_t length, word_t *buffer)
{
    word_t index;

    ಯದಿ (unlikely(length < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    index = getSyscallArg(0, buffer);

    ಯದಿ (index >= domScheduleLength) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = domScheduleLength - 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಯದಿ (dschedule_is_end_marker(index)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 0;
        current_syscall_error.type = seL4_InvalidArgument;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    invokeDomainScheduleSetStart(index);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(word_t invLabel, word_t length, word_t *buffer)
{
    ವಿಕಲ್ಪ (invLabel) {
    ಪಕ್ಷ DomainSetSet:
        ಪ್ರತ್ಯಾಗಮ decodeDomainSetSetInvocation(length, buffer);
    ಪಕ್ಷ DomainScheduleConfigure:
        ಪ್ರತ್ಯಾಗಮ decodeDomainScheduleConfigure(length, buffer);
    ಪಕ್ಷ DomainScheduleSetStart:
        ಪ್ರತ್ಯಾಗಮ decodeDomainScheduleSetStart(length, buffer);
    ಸ್ವಭಾವ:
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
}
