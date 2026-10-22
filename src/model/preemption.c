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
exception_t preemptionPoint(تهی)
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
    اگر (ksWorkUnitsCompleted >= CONFIG_MAX_رقم_WORK_UNITS_PER_PREEMPTION) {
        ksWorkUnitsCompleted = 0;
#ifdef CONFIG_KERNEL_MCS
        updateTimestamp();
        اگر (isIRQPending() || isCurDomainExpired()
            || !(sc_active(گره_STATE(ksCurSC)) && refill_sufficient(گره_STATE(ksCurSC), گره_STATE(ksConsumed)))) {
#وگرنه
        اگر (isIRQPending()) {
#endif
            بازگشت EXCEPTION_PREEMPTED;
        }
    }

    بازگشت EXCEPTION_NONE;
}

