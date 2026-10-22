/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <machine/timer.h>
#அந்தர்பாவ <mode/api/ipc_buffer.h>
#அந்தர்பாவ <object/schedcontext.h>
#அந்தர்பாவ <object/schedcontrol.h>
#அந்தர்பாவ <kernel/sporadic.h>

நிலையான exception_t invokeSchedControl_ConfigureFlags(sched_context_t *target, word_t core, ticks_t budget,
                                                     ticks_t period, word_t max_refills, word_t badge, word_t flags)
{
    tcb_t *thread = target->scTcb;

    /* don't modify parameters of thread while it is in a sorted queue */
    எனில் (thread) {
        /* possibly stall a remote core */
        SMP_COND_STATEMENT(remoteTCBStall(thread));
        /* remove from scheduler */
        tcbReleaseRemove(thread);
        tcbSchedDequeue(thread);
        /* bill the current consumed amount before adjusting the params */
        எனில் (முனை_STATE(ksCurSC) == target) {
            /* This could potentially mutate state but if it returns
             * true no state was modified, thus removing it should
             * be the same. */
            ப்ரதிஜ்ஞா(checkBudget());
            commitTime();
        }
    }

    எனில் (budget == period) {
        /* this is a cool hack: for round robin, we set the
         * period to 0, which means that the budget will always be ready to be refilled
         * and avoids some special casing.
         */
        refill_new(target, MIN_REFILLS, budget, 0);
    } இல்லையெனில் எனில் (SMP_COND_STATEMENT(core == target->scCore &&) sc_active(target) && thread
               && isRunnable(thread)) {
        /* the scheduling context is active - it can be used, so
         * we need to preserve the bandwidth */
        refill_update(target, period, budget, max_refills);
    } இல்லையெனில் {
        /* the scheduling context isn't active - it's budget is not being used, so
         * we can just populate the parameters from now */
        refill_new(target, max_refills, budget, period);
    }

#ifdef ENABLE_SMP_SUPPORT
    target->scCore = core;
    எனில் (thread) {
        migrateTCB(thread, target->scCore);
    }
#endif /* ENABLE_SMP_SUPPORT */

    ப்ரதிஜ்ஞா(sc_active(target));
    எனில் (thread) {
        schedContext_resume(target);
        எனில் (SMP_TERNARY(core == CURRENT_CPU_INDEX(), true)) {
            எனில் (isRunnable(thread) && thread != முனை_STATE(ksCurThread)) {
                possibleSwitchTo(thread);
            }
        } இல்லையெனில் எனில் (isRunnable(thread)) {
            SCHED_ENQUEUE(thread);
        }
        எனில் (thread == முனை_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    target->scBadge = badge;
    target->scSporadic = (flags & seL4_SchedContext_Sporadic) != 0;

    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeSchedControl_ConfigureFlags(word_t length, cap_t cap, word_t *buffer)
{
    எனில் (current_extra_caps.excaprefs[0] == வெற்று) {
        userError("SchedControl_ConfigureFlags: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (length < (TIME_ARG_SIZE * 2) + 3) {
        userError("SchedControl_configureFlags: truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    time_t budget_us = mode_parseTimeArg(0, buffer);

    எனில் (budget_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t budget_ticks = usToTicks(budget_us);

    time_t period_us = mode_parseTimeArg(TIME_ARG_SIZE, buffer);

    எனில் (period_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: period out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t period_ticks = usToTicks(period_us);

    எனில் (budget_ticks < MIN_BUDGET) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (period_ticks < MIN_BUDGET) {
        userError("SchedControl_ConfigureFlags: period out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (budget_ticks > period_ticks) {
        userError("SchedControl_ConfigureFlags: budget must be <= period");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = period_us;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    cap_t targetCap = current_extra_caps.excaprefs[0]->cap;
    எனில் (unlikely(cap_get_capType(targetCap) != cap_sched_context_cap)) {
        userError("SchedControl_ConfigureFlags: target cap not a scheduling context cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    word_t extra_refills = getSyscallArg(TIME_ARG_SIZE * 2, buffer);
    word_t max_refills = refill_absolute_max(targetCap);

    எனில் (extra_refills > max_refills - MIN_REFILLS) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = max_refills - MIN_REFILLS;
        userError("Max refills invalid, got %lu, max %lu",
                  extra_refills,
                  current_syscall_error.rangeErrorMax);
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    word_t badge = getSyscallArg(TIME_ARG_SIZE * 2 + 1, buffer);
    word_t flags = getSyscallArg(TIME_ARG_SIZE * 2 + 2, buffer);

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeSchedControl_ConfigureFlags(SC_PTR(cap_sched_context_cap_get_capSCPtr(targetCap)),
                                             cap_sched_control_cap_get_core(cap),
                                             budget_ticks,
                                             period_ticks,
                                             extra_refills + MIN_REFILLS,
                                             badge,
                                             flags);
}

exception_t decodeSchedControlInvocation(word_t label, cap_t cap, word_t length, word_t *buffer)
{
    தேர்வு (label) {
    நிலை SchedControlConfigureFlags:
        திரும்பு  decodeSchedControl_ConfigureFlags(length, cap, buffer);
    இயல்பு:
        userError("SchedControl invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
}
