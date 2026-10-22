/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <model/preemption.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <config.h>

/*
 * Possibly preempt the current thread to allow an interrupt to be handled.
 */
exception_t preemptionPoint(ಶೂನ್ಯ)
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
    ಯದಿ (ksWorkUnitsCompleted >= CONFIG_MAX_ಅಂಕೆ_WORK_UNITS_PER_PREEMPTION) {
        ksWorkUnitsCompleted = 0;
#ifdef CONFIG_KERNEL_MCS
        updateTimestamp();
        ಯದಿ (isIRQPending() || isCurDomainExpired()
            || !(sc_active(ಗಂಟು_STATE(ksCurSC)) && refill_sufficient(ಗಂಟು_STATE(ksCurSC), ಗಂಟು_STATE(ksConsumed)))) {
#ಅನ್ಯಥಾ
        ಯದಿ (isIRQPending()) {
#endif
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_PREEMPTED;
        }
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

