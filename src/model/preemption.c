/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <model/preemption.h>
#অন্তর্ভাব <model/statedata.h>
#অন্তর্ভাব <plat/machine/hardware.h>
#অন্তর্ভাব <config.h>

/*
 * Possibly preempt the current thread to allow an interrupt to be handled.
 */
exception_t preemptionPoint(শূন্য)
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
    যদি (ksWorkUnitsCompleted >= CONFIG_MAX_অঙ্ক_WORK_UNITS_PER_PREEMPTION) {
        ksWorkUnitsCompleted = 0;
#ifdef CONFIG_KERNEL_MCS
        updateTimestamp();
        যদি (isIRQPending() || isCurDomainExpired()
            || !(sc_active(গ্রন্থি_STATE(ksCurSC)) && refill_sufficient(গ্রন্থি_STATE(ksCurSC), গ্রন্থি_STATE(ksConsumed)))) {
#অন্যথা
        যদি (isIRQPending()) {
#endif
            প্রত্যাগম EXCEPTION_PREEMPTED;
        }
    }

    প্রত্যাগম EXCEPTION_NONE;
}

