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

ساكن exception_t invokeSchedControl_ConfigureFlags(sched_context_t *نشانہ, لفظ_t core, ticks_t budget,
                                                     ticks_t period, لفظ_t max_refills, لفظ_t badge, لفظ_t flags)
{
    tcb_t *thread = نشانہ->scTcb;

    /* don't modify parameters of thread while it is in a sorted queue */
    إذا (thread) {
        /* possibly stall a remote core */
        SMP_COND_STATEMENT(remoteTCBStall(thread));
        /* remove from scheduler */
        tcbReleaseRemove(thread);
        tcb_Sched_قطار_توں_کڈھو(thread);
        /* bill the current consumed amount before adjusting the params */
        إذا (گرہ_STATE(ksCurSC) == نشانہ) {
            /* This could potentially mutate state but if it returns
             * true no state was modified, thus removing it should
             * be the same. */
            أكد(checkBudget());
            commit_ویلا();
        }
    }

    إذا (budget == period) {
        /* this is a cool hack: for round robin, we set the
         * period to 0, which means that the budget will always be ready to be refilled
         * and avoids some special casing.
         */
        refill_new(نشانہ, MIN_REFILLS, budget, 0);
    } وإلا إذا (SMP_COND_STATEMENT(core == نشانہ->scCore &&) sc_سرگرم(نشانہ) && thread
               && isRunnable(thread)) {
        /* the scheduling context is active - it can be used, so
         * we need to preserve the bandwidth */
        refill_update(نشانہ, period, budget, max_refills);
    } وإلا {
        /* the scheduling context isn't active - it's budget is not being used, so
         * we can just populate the parameters from now */
        refill_new(نشانہ, max_refills, budget, period);
    }

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    نشانہ->scCore = core;
    إذا (thread) {
        migrateTCB(thread, نشانہ->scCore);
    }
#جے_انت /* ENABLE_SMP_SUPPORT */

    أكد(sc_سرگرم(نشانہ));
    إذا (thread) {
        schedContext_resume(نشانہ);
        إذا (SMP_TERNARY(core == CURRENT_CPU_اشاریہ(), true)) {
            إذا (isRunnable(thread) && thread != گرہ_STATE(ksCurThread)) {
                possible_Switch_منزل(thread);
            }
        } وإلا إذا (isRunnable(thread)) {
            SCHED_قطار_وچ_پاؤ(thread);
        }
        إذا (thread == گرہ_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    نشانہ->scBadge = badge;
    نشانہ->scSporadic = (flags & seL4_SchedContext_Sporadic) != 0;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeSchedControl_ConfigureFlags(لفظ_t لمائی, cap_t cap, لفظ_t *بفر)
{
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("SchedControl_ConfigureFlags: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (لمائی < (ویلا_ARG_ناپ * 2) + 3) {
        userError("SchedControl_configureFlags: truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ویلا_t budget_us = منوال_parse_ویلا_Arg(0, بفر);

    إذا (budget_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t budget_ticks = us_منزل_Ticks(budget_us);

    ویلا_t period_us = منوال_parse_ویلا_Arg(ویلا_ARG_ناپ, بفر);

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

    cap_t نشانہ_Cap = current_extra_caps.excaprefs[0]->cap;
    إذا (unlikely(cap_get_capType(نشانہ_Cap) != cap_sched_context_cap)) {
        userError("SchedControl_ConfigureFlags: target cap not a scheduling context cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    لفظ_t extra_refills = getSyscallArg(ویلا_ARG_ناپ * 2, بفر);
    لفظ_t max_refills = refill_absolute_max(نشانہ_Cap);

    إذا (extra_refills > max_refills - MIN_REFILLS) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = max_refills - MIN_REFILLS;
        userError("Max refills invalid, got %lu, max %lu",
                  extra_refills,
                  current_syscall_error.rangeErrorMax);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    لفظ_t badge = getSyscallArg(ویلا_ARG_ناپ * 2 + 1, بفر);
    لفظ_t flags = getSyscallArg(ویلا_ARG_ناپ * 2 + 2, بفر);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSchedControl_ConfigureFlags(SC_PTR(cap_sched_context_cap_get_capSCPtr(نشانہ_Cap)),
                                             cap_sched_control_cap_get_core(cap),
                                             budget_ticks,
                                             period_ticks,
                                             extra_refills + MIN_REFILLS,
                                             badge,
                                             flags);
}

exception_t decodeSchedControlInvocation(لفظ_t label, cap_t cap, لفظ_t لمائی, لفظ_t *بفر)
{
    بدل (label) {
    حالة SchedControlConfigureFlags:
        أرجع  decodeSchedControl_ConfigureFlags(لمائی, cap, بفر);
    افتراضي:
        userError("SchedControl invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
