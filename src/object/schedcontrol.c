/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <machine/timer.h>
#شامل <mode/api/ipc_buffer.h>
#شامل <object/schedcontext.h>
#شامل <object/schedcontrol.h>
#شامل <kernel/sporadic.h>

ایستا exception_t invokeSchedControl_ConfigureFlags(sched_context_t *target, word_t core, ticks_t budget,
                                                     ticks_t period, word_t max_refills, word_t badge, word_t flags)
{
    tcb_t *thread = target->scTcb;

    /* don't modify parameters of thread while it is in a sorted queue */
    اگر (thread) {
        /* possibly stall a remote core */
        SMP_COND_STATEMENT(remoteTCBStall(thread));
        /* remove from scheduler */
        tcbReleaseRemove(thread);
        tcbSchedDequeue(thread);
        /* bill the current consumed amount before adjusting the params */
        اگر (گره_STATE(ksCurSC) == target) {
            /* This could potentially mutate state but if it returns
             * true no state was modified, thus removing it should
             * be the same. */
            أكد(checkBudget());
            commitTime();
        }
    }

    اگر (budget == period) {
        /* this is a cool hack: for round robin, we set the
         * period to 0, which means that the budget will always be ready to be refilled
         * and avoids some special casing.
         */
        refill_new(target, MIN_REFILLS, budget, 0);
    } وگرنه اگر (SMP_COND_STATEMENT(core == target->scCore &&) sc_active(target) && thread
               && isRunnable(thread)) {
        /* the scheduling context is active - it can be used, so
         * we need to preserve the bandwidth */
        refill_update(target, period, budget, max_refills);
    } وگرنه {
        /* the scheduling context isn't active - it's budget is not being used, so
         * we can just populate the parameters from now */
        refill_new(target, max_refills, budget, period);
    }

#ifdef ENABLE_SMP_SUPPORT
    target->scCore = core;
    اگر (thread) {
        migrateTCB(thread, target->scCore);
    }
#endif /* ENABLE_SMP_SUPPORT */

    أكد(sc_active(target));
    اگر (thread) {
        schedContext_resume(target);
        اگر (SMP_TERNARY(core == CURRENT_CPU_INDEX(), true)) {
            اگر (isRunnable(thread) && thread != گره_STATE(ksCurThread)) {
                possibleSwitchTo(thread);
            }
        } وگرنه اگر (isRunnable(thread)) {
            SCHED_ENQUEUE(thread);
        }
        اگر (thread == گره_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    target->scBadge = badge;
    target->scSporadic = (flags & seL4_SchedContext_Sporadic) != 0;

    بازگشت EXCEPTION_NONE;
}

ایستا exception_t decodeSchedControl_ConfigureFlags(word_t length, cap_t cap, word_t *buffer)
{
    اگر (current_extra_caps.excaprefs[0] == پوچ) {
        userError("SchedControl_ConfigureFlags: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    اگر (length < (TIME_ARG_SIZE * 2) + 3) {
        userError("SchedControl_configureFlags: truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    time_t budget_us = mode_parseTimeArg(0, buffer);

    اگر (budget_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t budget_ticks = usToTicks(budget_us);

    time_t period_us = mode_parseTimeArg(TIME_ARG_SIZE, buffer);

    اگر (period_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: period out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t period_ticks = usToTicks(period_us);

    اگر (budget_ticks < MIN_BUDGET) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    اگر (period_ticks < MIN_BUDGET) {
        userError("SchedControl_ConfigureFlags: period out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    اگر (budget_ticks > period_ticks) {
        userError("SchedControl_ConfigureFlags: budget must be <= period");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = period_us;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    cap_t targetCap = current_extra_caps.excaprefs[0]->cap;
    اگر (unlikely(cap_get_capType(targetCap) != cap_sched_context_cap)) {
        userError("SchedControl_ConfigureFlags: target cap not a scheduling context cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    word_t extra_refills = getSyscallArg(TIME_ARG_SIZE * 2, buffer);
    word_t max_refills = refill_absolute_max(targetCap);

    اگر (extra_refills > max_refills - MIN_REFILLS) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = max_refills - MIN_REFILLS;
        userError("Max refills invalid, got %lu, max %lu",
                  extra_refills,
                  current_syscall_error.rangeErrorMax);
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    word_t badge = getSyscallArg(TIME_ARG_SIZE * 2 + 1, buffer);
    word_t flags = getSyscallArg(TIME_ARG_SIZE * 2 + 2, buffer);

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeSchedControl_ConfigureFlags(SC_PTR(cap_sched_context_cap_get_capSCPtr(targetCap)),
                                             cap_sched_control_cap_get_core(cap),
                                             budget_ticks,
                                             period_ticks,
                                             extra_refills + MIN_REFILLS,
                                             badge,
                                             flags);
}

exception_t decodeSchedControlInvocation(word_t label, cap_t cap, word_t length, word_t *buffer)
{
    انتخاب (label) {
    حالت SchedControlConfigureFlags:
        بازگشت  decodeSchedControl_ConfigureFlags(length, cap, buffer);
    پیشفرض:
        userError("SchedControl invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
}
