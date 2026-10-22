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

ساكن exception_t invokeSchedControl_ConfigureFlags(sched_context_t *هدف, لفظ_t core, ticks_t budget,
                                                     ticks_t period, لفظ_t max_refills, لفظ_t badge, لفظ_t flags)
{
    tcb_t *thread = هدف->scTcb;

    /* don't modify parameters of thread while it is in a sorted queue */
    إذا (thread) {
        /* possibly stall a remote core */
        SMP_COND_STATEMENT(remoteTCBStall(thread));
        /* remove from scheduler */
        tcbReleaseRemove(thread);
        tcb_Sched_قطار_مان_ڪڍو(thread);
        /* bill the current consumed amount before adjusting the params */
        إذا (ڳنڍ_STATE(ksCurSC) == هدف) {
            /* This could potentially mutate state but if it returns
             * true no state was modified, thus removing it should
             * be the same. */
            أكد(checkBudget());
            commit_وقت();
        }
    }

    إذا (budget == period) {
        /* this is a cool hack: for round robin, we set the
         * period to 0, which means that the budget will always be ready to be refilled
         * and avoids some special casing.
         */
        refill_new(هدف, MIN_REFILLS, budget, 0);
    } وإلا إذا (SMP_COND_STATEMENT(core == هدف->scCore &&) sc_سرگرم(هدف) && thread
               && isRunnable(thread)) {
        /* the scheduling context is active - it can be used, so
         * we need to preserve the bandwidth */
        refill_update(هدف, period, budget, max_refills);
    } وإلا {
        /* the scheduling context isn't active - it's budget is not being used, so
         * we can just populate the parameters from now */
        refill_new(هدف, max_refills, budget, period);
    }

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    هدف->scCore = core;
    إذا (thread) {
        migrateTCB(thread, هدف->scCore);
    }
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

    أكد(sc_سرگرم(هدف));
    إذا (thread) {
        schedContext_resume(هدف);
        إذا (SMP_TERNARY(core == CURRENT_CPU_اشاريو(), true)) {
            إذا (isRunnable(thread) && thread != ڳنڍ_STATE(ksCurThread)) {
                possible_Switch_منزل(thread);
            }
        } وإلا إذا (isRunnable(thread)) {
            SCHED_قطار_اندر_وجهو(thread);
        }
        إذا (thread == ڳنڍ_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    هدف->scBadge = badge;
    هدف->scSporadic = (flags & seL4_SchedContext_Sporadic) != 0;

    أرجع EXCEPTION_NONE;
}

ساكن exception_t decodeSchedControl_ConfigureFlags(لفظ_t ڊيگهائي, cap_t cap, لفظ_t *بفر)
{
    إذا (current_extra_caps.excaprefs[0] == عدم) {
        userError("SchedControl_ConfigureFlags: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (ڊيگهائي < (وقت_ARG_سائيز * 2) + 3) {
        userError("SchedControl_configureFlags: truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    وقت_t budget_us = گهڻائي_parse_وقت_Arg(0, بفر);

    إذا (budget_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t budget_ticks = us_منزل_Ticks(budget_us);

    وقت_t period_us = گهڻائي_parse_وقت_Arg(وقت_ARG_سائيز, بفر);

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

    cap_t هدف_Cap = current_extra_caps.excaprefs[0]->cap;
    إذا (unlikely(cap_get_capType(هدف_Cap) != cap_sched_context_cap)) {
        userError("SchedControl_ConfigureFlags: target cap not a scheduling context cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    لفظ_t extra_refills = getSyscallArg(وقت_ARG_سائيز * 2, بفر);
    لفظ_t max_refills = refill_absolute_max(هدف_Cap);

    إذا (extra_refills > max_refills - MIN_REFILLS) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = max_refills - MIN_REFILLS;
        userError("Max refills invalid, got %lu, max %lu",
                  extra_refills,
                  current_syscall_error.rangeErrorMax);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    لفظ_t badge = getSyscallArg(وقت_ARG_سائيز * 2 + 1, بفر);
    لفظ_t flags = getSyscallArg(وقت_ARG_سائيز * 2 + 2, بفر);

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeSchedControl_ConfigureFlags(SC_PTR(cap_sched_context_cap_get_capSCPtr(هدف_Cap)),
                                             cap_sched_control_cap_get_core(cap),
                                             budget_ticks,
                                             period_ticks,
                                             extra_refills + MIN_REFILLS,
                                             badge,
                                             flags);
}

exception_t decodeSchedControlInvocation(لفظ_t label, cap_t cap, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    بدل (label) {
    حالة SchedControlConfigureFlags:
        أرجع  decodeSchedControl_ConfigureFlags(ڊيگهائي, cap, بفر);
    افتراضي:
        userError("SchedControl invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
