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
exception_t preemption_نقطو(فراغ)
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
    إذا (ksWorkUnitsCompleted >= CONFIG_MAX_انگ_WORK_UNITS_في_PREEMPTION) {
        ksWorkUnitsCompleted = 0;
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        updateTimestamp();
        إذا (isIRQPending() || isCurDomainExpired()
            || !(sc_سرگرم(ڳنڍ_STATE(ksCurSC)) && refill_sufficient(ڳنڍ_STATE(ksCurSC), ڳنڍ_STATE(ksConsumed)))) {
#وإلا
        إذا (isIRQPending()) {
#جيڪڏهن_پورو
            أرجع EXCEPTION_PREEMPTED;
        }
    }

    أرجع EXCEPTION_NONE;
}

