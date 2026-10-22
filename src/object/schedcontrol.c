/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/api/ipc_buffer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/schedcontext.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/schedcontrol.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/sporadic.h>

ਸਥਿਰ exception_t invokeSchedControl_ConfigureFlags(sched_context_t *ਨਿਸ਼ਾਨਾ, ਸ਼ਬਦ_t core, ticks_t budget,
                                                     ticks_t period, ਸ਼ਬਦ_t max_refills, ਸ਼ਬਦ_t badge, ਸ਼ਬਦ_t flags)
{
    tcb_t *thread = ਨਿਸ਼ਾਨਾ->scTcb;

    /* don't modify parameters of thread while it is in a sorted queue */
    ਜੇ (thread) {
        /* possibly stall a remote core */
        SMP_COND_STATEMENT(remoteTCBStall(thread));
        /* remove from scheduler */
        tcbReleaseRemove(thread);
        tcb_Sched_ਕਤਾਰ_ਤੋਂ_ਕੱਢੋ(thread);
        /* bill the current consumed amount before adjusting the params */
        ਜੇ (ਗੰਢ_STATE(ksCurSC) == ਨਿਸ਼ਾਨਾ) {
            /* This could potentially mutate state but if it returns
             * true no state was modified, thus removing it should
             * be the same. */
            ਪ੍ਰਤਿਜ੍ਞਾ(checkBudget());
            commit_ਸਮਾਂ();
        }
    }

    ਜੇ (budget == period) {
        /* this is a cool hack: for round robin, we set the
         * period to 0, which means that the budget will always be ready to be refilled
         * and avoids some special casing.
         */
        refill_new(ਨਿਸ਼ਾਨਾ, MIN_REFILLS, budget, 0);
    } ਨਹੀਂਤਾਂ ਜੇ (SMP_COND_STATEMENT(core == ਨਿਸ਼ਾਨਾ->scCore &&) sc_ਸਰਗਰਮ(ਨਿਸ਼ਾਨਾ) && thread
               && isRunnable(thread)) {
        /* the scheduling context is active - it can be used, so
         * we need to preserve the bandwidth */
        refill_update(ਨਿਸ਼ਾਨਾ, period, budget, max_refills);
    } ਨਹੀਂਤਾਂ {
        /* the scheduling context isn't active - it's budget is not being used, so
         * we can just populate the parameters from now */
        refill_new(ਨਿਸ਼ਾਨਾ, max_refills, budget, period);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ਨਿਸ਼ਾਨਾ->scCore = core;
    ਜੇ (thread) {
        migrateTCB(thread, ਨਿਸ਼ਾਨਾ->scCore);
    }
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

    ਪ੍ਰਤਿਜ੍ਞਾ(sc_ਸਰਗਰਮ(ਨਿਸ਼ਾਨਾ));
    ਜੇ (thread) {
        schedContext_resume(ਨਿਸ਼ਾਨਾ);
        ਜੇ (SMP_TERNARY(core == CURRENT_CPU_ਸੂਚਕਾਂਕ(), true)) {
            ਜੇ (isRunnable(thread) && thread != ਗੰਢ_STATE(ksCurThread)) {
                possible_Switch_ਮੰਜ਼ਿਲ(thread);
            }
        } ਨਹੀਂਤਾਂ ਜੇ (isRunnable(thread)) {
            SCHED_ਕਤਾਰ_ਵਿੱਚ_ਪਾਓ(thread);
        }
        ਜੇ (thread == ਗੰਢ_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    ਨਿਸ਼ਾਨਾ->scBadge = badge;
    ਨਿਸ਼ਾਨਾ->scSporadic = (flags & seL4_SchedContext_Sporadic) != 0;

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decodeSchedControl_ConfigureFlags(ਸ਼ਬਦ_t ਲੰਮਾਈ, cap_t cap, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਜੇ (current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("SchedControl_ConfigureFlags: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ਲੰਮਾਈ < (ਸਮਾਂ_ARG_ਮਾਪ * 2) + 3) {
        userError("SchedControl_configureFlags: truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸਮਾਂ_t budget_us = ਬਹੁਲਕ_parse_ਸਮਾਂ_Arg(0, ਬਫਰ);

    ਜੇ (budget_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t budget_ticks = us_ਮੰਜ਼ਿਲ_Ticks(budget_us);

    ਸਮਾਂ_t period_us = ਬਹੁਲਕ_parse_ਸਮਾਂ_Arg(ਸਮਾਂ_ARG_ਮਾਪ, ਬਫਰ);

    ਜੇ (period_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: period out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t period_ticks = us_ਮੰਜ਼ਿਲ_Ticks(period_us);

    ਜੇ (budget_ticks < MIN_BUDGET) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (period_ticks < MIN_BUDGET) {
        userError("SchedControl_ConfigureFlags: period out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (budget_ticks > period_ticks) {
        userError("SchedControl_ConfigureFlags: budget must be <= period");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = period_us;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    cap_t ਨਿਸ਼ਾਨਾ_Cap = current_extra_caps.excaprefs[0]->cap;
    ਜੇ (unlikely(cap_get_capType(ਨਿਸ਼ਾਨਾ_Cap) != cap_sched_context_cap)) {
        userError("SchedControl_ConfigureFlags: target cap not a scheduling context cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸ਼ਬਦ_t extra_refills = getSyscallArg(ਸਮਾਂ_ARG_ਮਾਪ * 2, ਬਫਰ);
    ਸ਼ਬਦ_t max_refills = refill_absolute_max(ਨਿਸ਼ਾਨਾ_Cap);

    ਜੇ (extra_refills > max_refills - MIN_REFILLS) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = max_refills - MIN_REFILLS;
        userError("Max refills invalid, got %lu, max %lu",
                  extra_refills,
                  current_syscall_error.rangeErrorMax);
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਸ਼ਬਦ_t badge = getSyscallArg(ਸਮਾਂ_ARG_ਮਾਪ * 2 + 1, ਬਫਰ);
    ਸ਼ਬਦ_t flags = getSyscallArg(ਸਮਾਂ_ARG_ਮਾਪ * 2 + 2, ਬਫਰ);

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeSchedControl_ConfigureFlags(SC_PTR(cap_sched_context_cap_get_capSCPtr(ਨਿਸ਼ਾਨਾ_Cap)),
                                             cap_sched_control_cap_get_core(cap),
                                             budget_ticks,
                                             period_ticks,
                                             extra_refills + MIN_REFILLS,
                                             badge,
                                             flags);
}

exception_t decodeSchedControlInvocation(ਸ਼ਬਦ_t label, cap_t cap, ਸ਼ਬਦ_t ਲੰਮਾਈ, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਚੋਣ (label) {
    ਹਾਲਤ SchedControlConfigureFlags:
        ਵਾਪਸ  decodeSchedControl_ConfigureFlags(ਲੰਮਾਈ, cap, ਬਫਰ);
    ਮੂਲ:
        userError("SchedControl invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}
