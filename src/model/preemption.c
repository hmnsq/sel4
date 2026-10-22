/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <model/preemption.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <plat/machine/hardware.h>
#अन्तर्भाव <config.h>

/*
 * Possibly preempt the current thread to allow an interrupt to be handled.
 */
exception_t preemptionPoint(शून्य)
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
    यदि (ksWorkUnitsCompleted >= CONFIG_MAX_अङ्कः_WORK_UNITS_प्रत्येकम्_PREEMPTION) {
        ksWorkUnitsCompleted = 0;
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        updateTimestamp();
        यदि (isIRQPending() || isCurDomainExpired()
            || !(sc_सक्रिय(ग्रन्थिः_STATE(ksCurSC)) && refill_sufficient(ग्रन्थिः_STATE(ksCurSC), ग्रन्थिः_STATE(ksConsumed)))) {
#अन्यथा
        यदि (isIRQPending()) {
#यदि_अन्तः
            प्रत्यागम EXCEPTION_PREEMPTED;
        }
    }

    प्रत्यागम EXCEPTION_NONE;
}

