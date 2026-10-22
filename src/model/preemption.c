/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <api/failures.h>
#شامل <model/preemption.h>
#شامل <model/statedata.h>
#شامل <plat/machine/hardware.h>
#شامل <config.h>

/*
 * Possibly preempt the current thread to allow an interrupt to be handled.
 */
exception_t preemption_نقطہ(خالی)
{
    /* Record that we have performed some work. */
    ksWorkUnitsCompleted++;

    /*
     * If we have performed a non-trivial amount of work since last time we
     * checked for preemption, and there is an interrupt pending, handle the
     * interrupt.
     *
     * We avoid checking for pending IRQs every call, as our callers tend to
     * call us in a tight loop and checking for pending IRQs can be quite slow.
     */
    اگر (ksWorkUnitsCompleted >= CONFIG_MAX_عدد_WORK_UNITS_فی_PREEMPTION) {
        ksWorkUnitsCompleted = 0;
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        updateTimestamp();
        اگر (isIRQPending() || isCurDomainExpired()
            || !(sc_فعال(گرہ_STATE(ksCurSC)) && refill_sufficient(گرہ_STATE(ksCurSC), گرہ_STATE(ksConsumed)))) {
#ورنہ
        اگر (isIRQPending()) {
#اگر_ختم
            واپس EXCEPTION_PREEMPTED;
        }
    }

    واپس EXCEPTION_NONE;
}

