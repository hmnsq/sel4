/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <api/failures.h>
#ضمن <model/preemption.h>
#ضمن <model/statedata.h>
#ضمن <plat/machine/hardware.h>
#ضمن <config.h>

/*
 * Possibly preempt the current thread to allow an interrupt to be handled.
 */
exception_t preemptionPoint(فراغ)
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
    אם (ksWorkUnitsCompleted >= CONFIG_MAX_NUM_WORK_UNITS_PER_PREEMPTION) {
        ksWorkUnitsCompleted = 0;
#ifdef CONFIG_KERNEL_MCS
        updateTimestamp();
        אם (isIRQPending() || isCurDomainExpired()
            || !(sc_active(NODE_STATE(ksCurSC)) && refill_sufficient(NODE_STATE(ksCurSC), NODE_STATE(ksConsumed)))) {
#אחרת
        אם (isIRQPending()) {
#endif
            החזר EXCEPTION_PREEMPTED;
        }
    }

    החזר EXCEPTION_NONE;
}

