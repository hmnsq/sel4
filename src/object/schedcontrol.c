/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <machine/timer.h>
#ضمن <mode/api/ipc_buffer.h>
#ضمن <object/schedcontext.h>
#ضمن <object/schedcontrol.h>
#ضمن <kernel/sporadic.h>

ساكن exception_t invokeSchedControl_ConfigureFlags(sched_context_t *موخه, کلمه_t core, ticks_t budget,
                                                     ticks_t period, کلمه_t max_refills, کلمه_t badge, کلمه_t flags)
{
    tcb_t *thread = موخه->scTcb;

    /* don't modify parameters of thread while it is in a sorted queue */
    إذا (thread) {
        /* possibly stall a remote core */
        SMP_COND_STATEMENT(remoteTCBStall(thread));
        /* remove from scheduler */
        tcbReleaseRemove(thread);
        tcb_Sched_له_کتار_ایستل(thread);
        /* bill the current consumed amount before adjusting the params */
        إذا (غوټه_STATE(ksCurSC) == موخه) {
            /* This could potentially mutate state but if it returns
             * true no state was modified, thus removing it should
             * be the same. */
            أكد(checkBudget());
            commit_وخت();
        }
    }

    إذا (budget == period) {
        /* this is a cool hack: for round robin, we set the
         * period to 0, which means that the budget will always be ready to be refilled
         * and avoids some special casing.
         */
        refill_new(موخه, MIN_REFILLS, budget, 0);
    } وإلا إذا (SMP_COND_STATEMENT(core == موخه->scCore &&) sc_فعال(موخه) && thread
               && isRunnable(thread)) {
        /* the scheduling context is active - it can be used, so
         * we need to preserve the bandwidth */
        refill_update(موخه, period, budget, max_refills);
    } وإلا {
        /* the scheduling context isn't active - it's budget is not being used, so
         * we can just populate the parameters from now */
        refill_new(موخه, max_refills, budget, period);
    }

#که_تعریف_شوی ENABLE_SMP_SUPPORT
    موخه->scCore = core;
    إذا (thread) {
        migrateTCB(thread, موخه->scCore);
    }
#د_که_پای /* ENABLE_SMP_SUPPORT */

    أكد(sc_فعال(موخه));
    إذا (thread) {
        schedContext_resume(موخه);
        إذا (SMP_TERNARY(core == CURRENT_CPU_شاخص(), true)) {
            إذا (isRunnable(thread) && thread != غوټه_STATE(ksCurThread)) {
                possible_Switch_منزل(thread);
            }
        } وإلا إذا (isRunnable(thread)) {
            SCHED_کتار_ته_ورزیاتول(thread);
        }
        إذا (thread == غوټه_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    موخه->scBadge = badge;
    موخه->scSporadic = (flags & seL4_SchedContext_Sporadic) != 0;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeSchedControl_ConfigureFlags(کلمه_t اوږدتیا, cap_t cap, کلمه_t *بفر)
{
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("SchedControl_ConfigureFlags: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (اوږدتیا < (وخت_ARG_اندازه * 2) + 3) {
        userError("SchedControl_configureFlags: truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    وخت_t budget_us = ډېرپېښ_parse_وخت_Arg(0, بفر);

    إذا (budget_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t budget_ticks = us_منزل_Ticks(budget_us);

    وخت_t period_us = ډېرپېښ_parse_وخت_Arg(وخت_ARG_اندازه, بفر);

    إذا (period_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: period out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t period_ticks = us_منزل_Ticks(period_us);

    إذا (budget_ticks < MIN_BUDGET) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (period_ticks < MIN_BUDGET) {
        userError("SchedControl_ConfigureFlags: period out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (budget_ticks > period_ticks) {
        userError("SchedControl_ConfigureFlags: budget must be <= period");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = period_us;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cap_t موخه_Cap = current_extra_caps.excaprefs[0]->cap;
    إذا (unlikely(cap_get_capType(موخه_Cap) != cap_sched_context_cap)) {
        userError("SchedControl_ConfigureFlags: target cap not a scheduling context cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    کلمه_t extra_refills = getSyscallArg(وخت_ARG_اندازه * 2, بفر);
    کلمه_t max_refills = refill_absolute_max(موخه_Cap);

    إذا (extra_refills > max_refills - MIN_REFILLS) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = max_refills - MIN_REFILLS;
        userError("Max refills invalid, got %lu, max %lu",
                  extra_refills,
                  current_syscall_error.rangeErrorMax);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    کلمه_t badge = getSyscallArg(وخت_ARG_اندازه * 2 + 1, بفر);
    کلمه_t flags = getSyscallArg(وخت_ARG_اندازه * 2 + 2, بفر);

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSchedControl_ConfigureFlags(SC_PTR(cap_sched_context_cap_get_capSCPtr(موخه_Cap)),
                                             cap_sched_control_cap_get_core(cap),
                                             budget_ticks,
                                             period_ticks,
                                             extra_refills + MIN_REFILLS,
                                             badge,
                                             flags);
}

exception_t decodeSchedControlInvocation(کلمه_t label, cap_t cap, کلمه_t اوږدتیا, کلمه_t *بفر)
{
    بدل (label) {
    حالة SchedControlConfigureFlags:
        أرجع  decodeSchedControl_ConfigureFlags(اوږدتیا, cap, بفر);
    افتراضي:
        userError("SchedControl invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
