/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <api/failures.h>
#समावेश <model/preemption.h>
#समावेश <model/statedata.h>
#समावेश <plat/machine/hardware.h>
#समावेश <config.h>

/*
 * Possibly preempt the current thread to allow an interrupt to be handled.
 */
exception_t preemptionPoint(व्योम)
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
    अगर (ksWorkUnitsCompleted >= CONFIG_MAX_अंक_WORK_UNITS_PER_PREEMPTION) {
        ksWorkUnitsCompleted = 0;
#अगर_घोषित CONFIG_KERNEL_MCS
        updateTimestamp();
        अगर (isIRQPending() || isCurDomainExpired()
            || !(sc_active(गाँठ_STATE(ksCurSC)) && refill_sufficient(गाँठ_STATE(ksCurSC), गाँठ_STATE(ksConsumed)))) {
#अन्यथा
        अगर (isIRQPending()) {
#पूर्ण_अगर
            वापस EXCEPTION_PREEMPTED;
        }
    }

    वापस EXCEPTION_NONE;
}

