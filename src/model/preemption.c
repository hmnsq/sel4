/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <api/failures.h>
#അന്തര്ഭാവ <model/preemption.h>
#അന്തര്ഭാവ <model/statedata.h>
#അന്തര്ഭാവ <plat/machine/hardware.h>
#അന്തര്ഭാവ <config.h>

/*
 * Possibly preempt the current thread to allow an interrupt to be handled.
 */
exception_t preemptionPoint(ശൂന്യം)
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
    എങ്കിൽ (ksWorkUnitsCompleted >= CONFIG_MAX_അക്കം_WORK_UNITS_PER_PREEMPTION) {
        ksWorkUnitsCompleted = 0;
#ifdef CONFIG_KERNEL_MCS
        updateTimestamp();
        എങ്കിൽ (isIRQPending() || isCurDomainExpired()
            || !(sc_active(കണ്ണി_STATE(ksCurSC)) && refill_sufficient(കണ്ണി_STATE(ksCurSC), കണ്ണി_STATE(ksConsumed)))) {
#അല്ലെങ്കിൽ
        എങ്കിൽ (isIRQPending()) {
#endif
            തിരികെ EXCEPTION_PREEMPTED;
        }
    }

    തിരികെ EXCEPTION_NONE;
}

