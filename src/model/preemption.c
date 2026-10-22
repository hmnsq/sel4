/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <model/preemption.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <plat/machine/hardware.h>
#అన్తర్భావ <config.h>

/*
 * Possibly preempt the current thread to allow an interrupt to be handled.
 */
exception_t preemption_బిందువు(శూన్య)
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
    ఒకవేళ (ksWorkUnitsCompleted >= CONFIG_MAX_అంకె_WORK_UNITS_చొప్పున_PREEMPTION) {
        ksWorkUnitsCompleted = 0;
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        updateTimestamp();
        ఒకవేళ (isIRQPending() || isCurDomainExpired()
            || !(sc_చురుకైన(కణుపు_STATE(ksCurSC)) && refill_sufficient(కణుపు_STATE(ksCurSC), కణుపు_STATE(ksConsumed)))) {
#లేకపోతే
        ఒకవేళ (isIRQPending()) {
#అయితే_ముగింపు
            తిరిగి EXCEPTION_PREEMPTED;
        }
    }

    తిరిగి EXCEPTION_NONE;
}

