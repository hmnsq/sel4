/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <model/preemption.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <plat/machine/hardware.h>
#அந்தர்பாவ <config.h>

/*
 * Possibly preempt the current thread to allow an interrupt to be handled.
 */
exception_t preemptionPoint(வெறுமை)
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
    எனில் (ksWorkUnitsCompleted >= CONFIG_MAX_இலக்கம்_WORK_UNITS_PER_PREEMPTION) {
        ksWorkUnitsCompleted = 0;
#ifdef CONFIG_KERNEL_MCS
        updateTimestamp();
        எனில் (isIRQPending() || isCurDomainExpired()
            || !(sc_active(முனை_STATE(ksCurSC)) && refill_sufficient(முனை_STATE(ksCurSC), முனை_STATE(ksConsumed)))) {
#இல்லையெனில்
        எனில் (isIRQPending()) {
#endif
            திரும்பு EXCEPTION_PREEMPTED;
        }
    }

    திரும்பு EXCEPTION_NONE;
}

