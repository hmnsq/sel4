/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <model/preemption.h>
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/hardware.h>
#ଅନ୍ତର୍ଭାଵ <config.h>

/*
 * Possibly preempt the current thread to allow an interrupt to be handled.
 */
exception_t preemptionPoint(ଶୂନ୍ୟ)
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
    ଯଦି (ksWorkUnitsCompleted >= CONFIG_MAX_ଅଙ୍କ_WORK_UNITS_PER_PREEMPTION) {
        ksWorkUnitsCompleted = 0;
#ifdef CONFIG_KERNEL_MCS
        updateTimestamp();
        ଯଦି (isIRQPending() || isCurDomainExpired()
            || !(sc_active(ଗଣ୍ଠି_STATE(ksCurSC)) && refill_sufficient(ଗଣ୍ଠି_STATE(ksCurSC), ଗଣ୍ଠି_STATE(ksConsumed)))) {
#ଅନ୍ୟଥା
        ଯଦି (isIRQPending()) {
#endif
            ଫେରନ୍ତୁ EXCEPTION_PREEMPTED;
        }
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

