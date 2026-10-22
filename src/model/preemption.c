/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <model/preemption.h>
#અન્તર્ભાવ <model/statedata.h>
#અન્તર્ભાવ <plat/machine/hardware.h>
#અન્તર્ભાવ <config.h>

/*
 * Possibly preempt the current thread to allow an interrupt to be handled.
 */
exception_t preemptionPoint(શૂન્ય)
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
    જો (ksWorkUnitsCompleted >= CONFIG_MAX_અંક_WORK_UNITS_PER_PREEMPTION) {
        ksWorkUnitsCompleted = 0;
#ifdef CONFIG_KERNEL_MCS
        updateTimestamp();
        જો (isIRQPending() || isCurDomainExpired()
            || !(sc_active(ગાંઠ_STATE(ksCurSC)) && refill_sufficient(ગાંઠ_STATE(ksCurSC), ગાંઠ_STATE(ksConsumed)))) {
#નહીંતર
        જો (isIRQPending()) {
#endif
            પાછા EXCEPTION_PREEMPTED;
        }
    }

    પાછા EXCEPTION_NONE;
}

