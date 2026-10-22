/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <machine/timer.h>
#ಅನ್ತರ್ಭಾವ <mode/api/ipc_buffer.h>
#ಅನ್ತರ್ಭಾವ <object/schedcontext.h>
#ಅನ್ತರ್ಭಾವ <object/schedcontrol.h>
#ಅನ್ತರ್ಭಾವ <kernel/sporadic.h>

ಸ್ಥಿರ exception_t invokeSchedControl_ConfigureFlags(sched_context_t *ಗುರಿ, ಶಬ್ದ_t core, ticks_t budget,
                                                     ticks_t period, ಶಬ್ದ_t max_refills, ಶಬ್ದ_t badge, ಶಬ್ದ_t flags)
{
    tcb_t *thread = ಗುರಿ->scTcb;

    /* don't modify parameters of thread while it is in a sorted queue */
    ಆದರೆ (thread) {
        /* possibly stall a remote core */
        SMP_COND_STATEMENT(remoteTCBStall(thread));
        /* remove from scheduler */
        tcbReleaseRemove(thread);
        tcb_Sched_ಸರತಿಯಿಂದ_ತೆಗೆ(thread);
        /* bill the current consumed amount before adjusting the params */
        ಆದರೆ (ಗಂಟು_STATE(ksCurSC) == ಗುರಿ) {
            /* This could potentially mutate state but if it returns
             * true no state was modified, thus removing it should
             * be the same. */
            ಪ್ರತಿಜ್ಞಾ(checkBudget());
            commit_ಸಮಯ();
        }
    }

    ಆದರೆ (budget == period) {
        /* this is a cool hack: for round robin, we set the
         * period to 0, which means that the budget will always be ready to be refilled
         * and avoids some special casing.
         */
        refill_new(ಗುರಿ, MIN_REFILLS, budget, 0);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (SMP_COND_STATEMENT(core == ಗುರಿ->scCore &&) sc_ಸಕ್ರಿಯ(ಗುರಿ) && thread
               && isRunnable(thread)) {
        /* the scheduling context is active - it can be used, so
         * we need to preserve the bandwidth */
        refill_update(ಗುರಿ, period, budget, max_refills);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* the scheduling context isn't active - it's budget is not being used, so
         * we can just populate the parameters from now */
        refill_new(ಗುರಿ, max_refills, budget, period);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ಗುರಿ->scCore = core;
    ಆದರೆ (thread) {
        migrateTCB(thread, ಗುರಿ->scCore);
    }
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

    ಪ್ರತಿಜ್ಞಾ(sc_ಸಕ್ರಿಯ(ಗುರಿ));
    ಆದರೆ (thread) {
        schedContext_resume(ಗುರಿ);
        ಆದರೆ (SMP_TERNARY(core == CURRENT_CPU_ಸೂಚ್ಯಂಕ(), true)) {
            ಆದರೆ (isRunnable(thread) && thread != ಗಂಟು_STATE(ksCurThread)) {
                possible_Switch_ಗಮ್ಯ(thread);
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isRunnable(thread)) {
            SCHED_ಸರತಿಗೆ_ಸೇರಿಸು(thread);
        }
        ಆದರೆ (thread == ಗಂಟು_STATE(ksCurThread)) {
            rescheduleRequired();
        }
    }

    ಗುರಿ->scBadge = badge;
    ಗುರಿ->scSporadic = (flags & seL4_SchedContext_Sporadic) != 0;

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decodeSchedControl_ConfigureFlags(ಶಬ್ದ_t ಉದ್ದಳತೆ, cap_t cap, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆದರೆ (current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("SchedControl_ConfigureFlags: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ಉದ್ದಳತೆ < (ಸಮಯ_ARG_ಗಾತ್ರ * 2) + 3) {
        userError("SchedControl_configureFlags: truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಸಮಯ_t budget_us = ಬಹುಲಕ_parse_ಸಮಯ_Arg(0, ಬಫರ್);

    ಆದರೆ (budget_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t budget_ticks = us_ಗಮ್ಯ_Ticks(budget_us);

    ಸಮಯ_t period_us = ಬಹುಲಕ_parse_ಸಮಯ_Arg(ಸಮಯ_ARG_ಗಾತ್ರ, ಬಫರ್);

    ಆದರೆ (period_us > MAX_PERIOD_US) {
        userError("SchedControl_ConfigureFlags: period out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ticks_t period_ticks = us_ಗಮ್ಯ_Ticks(period_us);

    ಆದರೆ (budget_ticks < MIN_BUDGET) {
        userError("SchedControl_ConfigureFlags: budget out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (period_ticks < MIN_BUDGET) {
        userError("SchedControl_ConfigureFlags: period out of range.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = MAX_PERIOD_US;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (budget_ticks > period_ticks) {
        userError("SchedControl_ConfigureFlags: budget must be <= period");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = MIN_BUDGET_US;
        current_syscall_error.rangeErrorMax = period_us;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    cap_t ಗುರಿ_Cap = current_extra_caps.excaprefs[0]->cap;
    ಆದರೆ (unlikely(cap_get_capType(ಗುರಿ_Cap) != cap_sched_context_cap)) {
        userError("SchedControl_ConfigureFlags: target cap not a scheduling context cap");
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಶಬ್ದ_t extra_refills = getSyscallArg(ಸಮಯ_ARG_ಗಾತ್ರ * 2, ಬಫರ್);
    ಶಬ್ದ_t max_refills = refill_absolute_max(ಗುರಿ_Cap);

    ಆದರೆ (extra_refills > max_refills - MIN_REFILLS) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = max_refills - MIN_REFILLS;
        userError("Max refills invalid, got %lu, max %lu",
                  extra_refills,
                  current_syscall_error.rangeErrorMax);
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಶಬ್ದ_t badge = getSyscallArg(ಸಮಯ_ARG_ಗಾತ್ರ * 2 + 1, ಬಫರ್);
    ಶಬ್ದ_t flags = getSyscallArg(ಸಮಯ_ARG_ಗಾತ್ರ * 2 + 2, ಬಫರ್);

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeSchedControl_ConfigureFlags(SC_PTR(cap_sched_context_cap_get_capSCPtr(ಗುರಿ_Cap)),
                                             cap_sched_control_cap_get_core(cap),
                                             budget_ticks,
                                             period_ticks,
                                             extra_refills + MIN_REFILLS,
                                             badge,
                                             flags);
}

exception_t decodeSchedControlInvocation(ಶಬ್ದ_t label, cap_t cap, ಶಬ್ದ_t ಉದ್ದಳತೆ, ಶಬ್ದ_t *ಬಫರ್)
{
    ಆಯ್ಕೆ (label) {
    ಸ್ಥಿತಿ SchedControlConfigureFlags:
        ಹಿಂತಿರುಗಿ  decodeSchedControl_ConfigureFlags(ಉದ್ದಳತೆ, cap, ಬಫರ್);
    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("SchedControl invocation: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}
