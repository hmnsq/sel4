/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <machine/timer.h>
#अन्तर्भाव <mode/api/ipc_buffer.h>
#अन्तर्भाव <object/schedcontext.h>
#अन्तर्भाव <object/schedcontrol.h>
#अन्तर्भाव <kernel/sporadic.h>

स्थिर exception_t invokeSchedControl_ConfigureFlags(sched_context_t *target, word_t core, ticks_t budget,
                                                     ticks_t period, word_t max_refills, word_t badge, word_t flags)
{
    tcb_t *thread = target->scTcb;

    /* don't modify parameters of thread while it is in a sorted queue */
    यदि (thread) {
        /* possibly stall a remote core */
        SMP_COND_STATEMENT(remoteTCBStall(thread));
        /* remove from scheduler */
        tcbReleaseRemove(thread);
        tcbSchedDequeue(thread);
        /* bill the current consumed amount before adjusting the params */
        यदि (गाँठो_STATE(ksCurSC) == target) {
            /* This could potentially mutate state but if it returns
             * true no state was modified, thus removing it should
             * be the same. */
            प्रतिज्ञा(checkBudget());
            commitTime();
        }
    }

    यदि (budget == period) {
        /* this is a cool hack: for round robin, we set the
         * period to 0, which means that the budget will always be ready to be refilled
         * and avoids some special casing.
         */
        refill_new(target, MIN_REFILLS, budget, 0);
    } अन्यथा यदि (SMP_COND_STATEMENT(core == target->scCore &&) sc_active(target) && thread
               && isRunnable(thread)) {
        /* the scheduling context is active - it can be used, so
         * we need to preserve the bandwidth */
        refill_update(target, period, budget, max_refills);
    } अन्यथा {
        /* the scheduling context isn't active - it's budget is not being used, so
         * we can just populate the parameters from now */
        refill_new(target, max_refills, budget, period);
    }

#ifdef ENABLE_SMP_SUPPORT
    target->scCore = core;
    यदि (thread) {
        migrateTCB(thread, target->scCore);
    }
#endif /* ENABLE_SMP_SUPPORT */

    प्रतिज्ञा(sc_active(target));
    यदि (thread) {
        schedContext_resume(target);
        यदि (SMP_TERNARY(core == CURRENT_CPU_INDEX(), true)) {
            यदि (isRunnable(thread) && thread != गाँठो_STATE(ksCurThread)) {
                possibleSwitchTo(thread);
            }
        } अन्यथा यदि (isRunnable(thread)) {
            SCHED_ENQUEUE(thread);
        }
        यदि (thread == गाँठो_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    target->scBadge = badge;
    target->scSporadic = (flags & seL4_SchedContext_Sporadic) != 0;

    फर्काऊ EXCEPTION_NONE;
}

स्थिर exception_t decodeSchedControl_ConfigureFlags(word_t length, cap_t cap, word_t *buffer)
{
    यदि (current_extra_caps.excaprefs[0] == रिक्त) {
        userError("SchedControl_ConfigureFlags: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (length < (TIME_ARG_SIZE * 2) + 3) {
        userError("SchedControl_configureFlags: truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    समय_प्रकार budget_us = mode_parseTimeArg(0, buffer);

    यदि (budget_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t budget_ticks = usToTicks(budget_us);

    समय_प्रकार period_us = mode_parseTimeArg(TIME_ARG_SIZE, buffer);

    यदि (period_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: period out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t period_ticks = usToTicks(period_us);

    यदि (budget_ticks < MIN_BUDGET) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (period_ticks < MIN_BUDGET) {
        userError("SchedControl_ConfigureFlags: period out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (budget_ticks > period_ticks) {
        userError("SchedControl_ConfigureFlags: budget must be <= period");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = period_us;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    cap_t targetCap = current_extra_caps.excaprefs[0]->cap;
    यदि (unlikely(cap_get_capType(targetCap) != cap_sched_context_cap)) {
        userError("SchedControl_ConfigureFlags: target cap not a scheduling context cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_सङ्ख्या = 1;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    word_t extra_refills = getSyscallArg(TIME_ARG_SIZE * 2, buffer);
    word_t max_refills = refill_absolute_max(targetCap);

    यदि (extra_refills > max_refills - MIN_REFILLS) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = max_refills - MIN_REFILLS;
        userError("Max refills invalid, got %lu, max %lu",
                  extra_refills,
                  current_syscall_error.rangeErrorMax);
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    word_t badge = getSyscallArg(TIME_ARG_SIZE * 2 + 1, buffer);
    word_t flags = getSyscallArg(TIME_ARG_SIZE * 2 + 2, buffer);

    setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
    फर्काऊ invokeSchedControl_ConfigureFlags(SC_PTR(cap_sched_context_cap_get_capSCPtr(targetCap)),
                                             cap_sched_control_cap_get_core(cap),
                                             budget_ticks,
                                             period_ticks,
                                             extra_refills + MIN_REFILLS,
                                             badge,
                                             flags);
}

exception_t decodeSchedControlInvocation(word_t label, cap_t cap, word_t length, word_t *buffer)
{
    विकल्प (label) {
    पक्ष SchedControlConfigureFlags:
        फर्काऊ  decodeSchedControl_ConfigureFlags(length, cap, buffer);
    स्वभाव:
        userError("SchedControl invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }
}
