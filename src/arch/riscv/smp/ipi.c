/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/smp/ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <smp/lock.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT

ਸਥਿਰ ਅਸਥਿਰ irq_t ipiIrq[CONFIG_MAX_ਅੰਕ_NODES];

ਖਾਲੀ handleRemoteCall(IpiRemoteCall_t call, ਸ਼ਬਦ_t arg0, ਸ਼ਬਦ_t arg1, ਸ਼ਬਦ_t arg2, bool_t irq_ਰਸਤਾ)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    ਜੇ (clh_is_ipi_pending(getCurrentCPUIndex())) {
        ਚੋਣ (call) {
        ਹਾਲਤ IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_ਰਸਤਾ);
            ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
        ਹਾਲਤ IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            ਰੋਕੋ;
#ਜੇ_ਅੰਤ /* CONFIG_HAVE_FPU */

        ਮੂਲ:
            fail("Invalid remote call");
            ਰੋਕੋ;
        }

        big_kernel_ਤਾਲਾ.ਗੰਢ[getCurrentCPUIndex()].ipi = 0;
        ipiIrq[getCurrentCPUIndex()] = irqInvalid;
        ipi_wait();
    }
}

ਖਾਲੀ ipi_send_ਨਕਾਬ(irq_t ipi, ਸ਼ਬਦ_t ਨਕਾਬ_2, bool_t isBlocking)
{
    generic_ipi_send_ਨਕਾਬ(ipi, ਨਕਾਬ_2, isBlocking);
}

irq_t ipi_get_irq(ਖਾਲੀ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(!(ipiIrq[getCurrentCPUIndex()] == irqInvalid && clh_is_ipi_pending(getCurrentCPUIndex())));
    ਵਾਪਸ ipiIrq[getCurrentCPUIndex()];
}

ਖਾਲੀ ipi_clear_irq(irq_t irq)
{
    ipiIrq[getCurrentCPUIndex()] = irqInvalid;
    ਵਾਪਸ;
}

/* this function is called with a single hart id. */
ਖਾਲੀ ipi_send_ਨਿਸ਼ਾਨਾ(irq_t irq, ਸ਼ਬਦ_t hart_id)
{
    ਸ਼ਬਦ_t hart_ਨਕਾਬ = ਦੋਆਧਾਰੀ_ਅੰਕ(hart_id);
    ਸ਼ਬਦ_t core_id = hartIDToCoreID(hart_id);
    ਪ੍ਰਤਿਜ੍ਞਾ(core_id < CONFIG_MAX_ਅੰਕ_NODES);

    ਪ੍ਰਤਿਜ੍ਞਾ((ipiIrq[core_id] == irqInvalid) || (ipiIrq[core_id] == irq_reschedule_ipi) ||
           (ipiIrq[core_id] == irq_remote_call_ipi && !clh_is_ipi_pending(core_id)));

    ipiIrq[core_id] = irq;
    fence_rw_rw();
    sbi_send_ipi(hart_ਨਕਾਬ);
}

#ਜੇ_ਅੰਤ
