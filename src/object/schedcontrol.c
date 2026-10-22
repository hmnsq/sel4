/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <machine/timer.h>
#అన్తర్భావ <mode/api/ipc_buffer.h>
#అన్తర్భావ <object/schedcontext.h>
#అన్తర్భావ <object/schedcontrol.h>
#అన్తర్భావ <kernel/sporadic.h>

స్థిర exception_t invokeSchedControl_ConfigureFlags(sched_context_t *లక్ష్యం, మాట_t core, ticks_t budget,
                                                     ticks_t period, మాట_t max_refills, మాట_t badge, మాట_t flags)
{
    tcb_t *thread = లక్ష్యం->scTcb;

    /* don't modify parameters of thread while it is in a sorted queue */
    ఒకవేళ (thread) {
        /* possibly stall a remote core */
        SMP_COND_STATEMENT(remoteTCBStall(thread));
        /* remove from scheduler */
        tcbReleaseRemove(thread);
        tcb_Sched_వరుస_నుండి_తీయి(thread);
        /* bill the current consumed amount before adjusting the params */
        ఒకవేళ (కణుపు_STATE(ksCurSC) == లక్ష్యం) {
            /* This could potentially mutate state but if it returns
             * true no state was modified, thus removing it should
             * be the same. */
            ప్రతిజ్ఞా(checkBudget());
            commit_సమయం();
        }
    }

    ఒకవేళ (budget == period) {
        /* this is a cool hack: for round robin, we set the
         * period to 0, which means that the budget will always be ready to be refilled
         * and avoids some special casing.
         */
        refill_new(లక్ష్యం, MIN_REFILLS, budget, 0);
    } లేకపోతే ఒకవేళ (SMP_COND_STATEMENT(core == లక్ష్యం->scCore &&) sc_చురుకైన(లక్ష్యం) && thread
               && isRunnable(thread)) {
        /* the scheduling context is active - it can be used, so
         * we need to preserve the bandwidth */
        refill_update(లక్ష్యం, period, budget, max_refills);
    } లేకపోతే {
        /* the scheduling context isn't active - it's budget is not being used, so
         * we can just populate the parameters from now */
        refill_new(లక్ష్యం, max_refills, budget, period);
    }

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    లక్ష్యం->scCore = core;
    ఒకవేళ (thread) {
        migrateTCB(thread, లక్ష్యం->scCore);
    }
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

    ప్రతిజ్ఞా(sc_చురుకైన(లక్ష్యం));
    ఒకవేళ (thread) {
        schedContext_resume(లక్ష్యం);
        ఒకవేళ (SMP_TERNARY(core == CURRENT_CPU_సూచీ(), true)) {
            ఒకవేళ (isRunnable(thread) && thread != కణుపు_STATE(ksCurThread)) {
                possible_Switch_గమ్యం(thread);
            }
        } లేకపోతే ఒకవేళ (isRunnable(thread)) {
            SCHED_వరుసలో_చేర్చు(thread);
        }
        ఒకవేళ (thread == కణుపు_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    లక్ష్యం->scBadge = badge;
    లక్ష్యం->scSporadic = (flags & seL4_SchedContext_Sporadic) != 0;

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decodeSchedControl_ConfigureFlags(మాట_t నిడివి, cap_t cap, మాట_t *బఫరు)
{
    ఒకవేళ (current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("SchedControl_ConfigureFlags: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (నిడివి < (సమయం_ARG_కొలత * 2) + 3) {
        userError("SchedControl_configureFlags: truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    సమయం_t budget_us = బహుళకం_parse_సమయం_Arg(0, బఫరు);

    ఒకవేళ (budget_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t budget_ticks = us_గమ్యం_Ticks(budget_us);

    సమయం_t period_us = బహుళకం_parse_సమయం_Arg(సమయం_ARG_కొలత, బఫరు);

    ఒకవేళ (period_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: period out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t period_ticks = us_గమ్యం_Ticks(period_us);

    ఒకవేళ (budget_ticks < MIN_BUDGET) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (period_ticks < MIN_BUDGET) {
        userError("SchedControl_ConfigureFlags: period out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (budget_ticks > period_ticks) {
        userError("SchedControl_ConfigureFlags: budget must be <= period");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = period_us;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    cap_t లక్ష్యం_Cap = current_extra_caps.excaprefs[0]->cap;
    ఒకవేళ (unlikely(cap_get_capType(లక్ష్యం_Cap) != cap_sched_context_cap)) {
        userError("SchedControl_ConfigureFlags: target cap not a scheduling context cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    మాట_t extra_refills = getSyscallArg(సమయం_ARG_కొలత * 2, బఫరు);
    మాట_t max_refills = refill_absolute_max(లక్ష్యం_Cap);

    ఒకవేళ (extra_refills > max_refills - MIN_REFILLS) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = max_refills - MIN_REFILLS;
        userError("Max refills invalid, got %lu, max %lu",
                  extra_refills,
                  current_syscall_error.rangeErrorMax);
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    మాట_t badge = getSyscallArg(సమయం_ARG_కొలత * 2 + 1, బఫరు);
    మాట_t flags = getSyscallArg(సమయం_ARG_కొలత * 2 + 2, బఫరు);

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeSchedControl_ConfigureFlags(SC_PTR(cap_sched_context_cap_get_capSCPtr(లక్ష్యం_Cap)),
                                             cap_sched_control_cap_get_core(cap),
                                             budget_ticks,
                                             period_ticks,
                                             extra_refills + MIN_REFILLS,
                                             badge,
                                             flags);
}

exception_t decodeSchedControlInvocation(మాట_t label, cap_t cap, మాట_t నిడివి, మాట_t *బఫరు)
{
    ఎంపిక (label) {
    స్థితి SchedControlConfigureFlags:
        తిరిగి  decodeSchedControl_ConfigureFlags(నిడివి, cap, బఫరు);
    అప్రమేయ:
        userError("SchedControl invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}
