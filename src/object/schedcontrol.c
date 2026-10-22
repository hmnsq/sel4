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

ساکن exception_t invokeSchedControl_ConfigureFlags(sched_context_t *ہدف, لفظ_t core, ticks_t budget,
                                                     ticks_t period, لفظ_t max_refills, لفظ_t badge, لفظ_t flags)
{
    tcb_t *thread = ہدف->scTcb;

    /* don't modify parameters of thread while it is in a sorted queue */
    اگر (thread) {
        /* possibly stall a remote core */
        SMP_COND_STATEMENT(remoteTCBStall(thread));
        /* remove from scheduler */
        tcbReleaseRemove(thread);
        tcb_Sched_قطار_سے_نکالو(thread);
        /* bill the current consumed amount before adjusting the params */
        اگر (گرہ_STATE(ksCurSC) == ہدف) {
            /* This could potentially mutate state but if it returns
             * true no state was modified, thus removing it should
             * be the same. */
            أكد(checkBudget());
            commit_وقت();
        }
    }

    اگر (budget == period) {
        /* this is a cool hack: for round robin, we set the
         * period to 0, which means that the budget will always be ready to be refilled
         * and avoids some special casing.
         */
        refill_new(ہدف, MIN_REFILLS, budget, 0);
    } ورنہ اگر (SMP_COND_STATEMENT(core == ہدف->scCore &&) sc_فعال(ہدف) && thread
               && isRunnable(thread)) {
        /* the scheduling context is active - it can be used, so
         * we need to preserve the bandwidth */
        refill_update(ہدف, period, budget, max_refills);
    } ورنہ {
        /* the scheduling context isn't active - it's budget is not being used, so
         * we can just populate the parameters from now */
        refill_new(ہدف, max_refills, budget, period);
    }

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    ہدف->scCore = core;
    اگر (thread) {
        migrateTCB(thread, ہدف->scCore);
    }
#اگر_ختم /* ENABLE_SMP_SUPPORT */

    أكد(sc_فعال(ہدف));
    اگر (thread) {
        schedContext_resume(ہدف);
        اگر (SMP_TERNARY(core == CURRENT_CPU_اشاریہ(), true)) {
            اگر (isRunnable(thread) && thread != گرہ_STATE(ksCurThread)) {
                possible_Switch_مقصد(thread);
            }
        } ورنہ اگر (isRunnable(thread)) {
            SCHED_قطار_میں_ڈالو(thread);
        }
        اگر (thread == گرہ_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    ہدف->scBadge = badge;
    ہدف->scSporadic = (flags & seL4_SchedContext_Sporadic) != 0;

    واپس EXCEPTION_NONE;
}

ساکن exception_t decodeSchedControl_ConfigureFlags(لفظ_t لمبائی, cap_t cap, لفظ_t *بفر)
{
    اگر (current_extra_caps.excaprefs[0] == تہی) {
        userError("SchedControl_ConfigureFlags: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (لمبائی < (وقت_ARG_سائز * 2) + 3) {
        userError("SchedControl_configureFlags: truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    وقت_t budget_us = منوال_parse_وقت_Arg(0, بفر);

    اگر (budget_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t budget_ticks = us_مقصد_Ticks(budget_us);

    وقت_t period_us = منوال_parse_وقت_Arg(وقت_ARG_سائز, بفر);

    اگر (period_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: period out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t period_ticks = us_مقصد_Ticks(period_us);

    اگر (budget_ticks < MIN_BUDGET) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (period_ticks < MIN_BUDGET) {
        userError("SchedControl_ConfigureFlags: period out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (budget_ticks > period_ticks) {
        userError("SchedControl_ConfigureFlags: budget must be <= period");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = period_us;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    cap_t ہدف_Cap = current_extra_caps.excaprefs[0]->cap;
    اگر (unlikely(cap_get_capType(ہدف_Cap) != cap_sched_context_cap)) {
        userError("SchedControl_ConfigureFlags: target cap not a scheduling context cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    لفظ_t extra_refills = getSyscallArg(وقت_ARG_سائز * 2, بفر);
    لفظ_t max_refills = refill_absolute_max(ہدف_Cap);

    اگر (extra_refills > max_refills - MIN_REFILLS) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = max_refills - MIN_REFILLS;
        userError("Max refills invalid, got %lu, max %lu",
                  extra_refills,
                  current_syscall_error.rangeErrorMax);
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    لفظ_t badge = getSyscallArg(وقت_ARG_سائز * 2 + 1, بفر);
    لفظ_t flags = getSyscallArg(وقت_ARG_سائز * 2 + 2, بفر);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeSchedControl_ConfigureFlags(SC_PTR(cap_sched_context_cap_get_capSCPtr(ہدف_Cap)),
                                             cap_sched_control_cap_get_core(cap),
                                             budget_ticks,
                                             period_ticks,
                                             extra_refills + MIN_REFILLS,
                                             badge,
                                             flags);
}

exception_t decodeSchedControlInvocation(لفظ_t label, cap_t cap, لفظ_t لمبائی, لفظ_t *بفر)
{
    انتخاب (label) {
    حالت SchedControlConfigureFlags:
        واپس  decodeSchedControl_ConfigureFlags(لمبائی, cap, بفر);
    طےشدہ:
        userError("SchedControl invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
}
