/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/preemption.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

/*
 * Possibly preempt the current thread to allow an interrupt to be handled.
 */
exception_t preemption_ਬਿੰਦੂ(ਖਾਲੀ)
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
    ਜੇ (ksWorkUnitsCompleted >= CONFIG_MAX_ਅੰਕ_WORK_UNITS_ਪ੍ਰਤੀ_PREEMPTION) {
        ksWorkUnitsCompleted = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        updateTimestamp();
        ਜੇ (isIRQPending() || isCurDomainExpired()
            || !(sc_ਸਰਗਰਮ(ਗੰਢ_STATE(ksCurSC)) && refill_sufficient(ਗੰਢ_STATE(ksCurSC), ਗੰਢ_STATE(ksConsumed)))) {
#ਨਹੀਂਤਾਂ
        ਜੇ (isIRQPending()) {
#ਜੇ_ਅੰਤ
            ਵਾਪਸ EXCEPTION_PREEMPTED;
        }
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

