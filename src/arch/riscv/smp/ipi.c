/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <mode/smp/ipi.h>
#ಅನ್ತರ್ಭಾವ <smp/lock.h>
#ಅನ್ತರ್ಭಾವ <util.h>

#ifdef ENABLE_SMP_SUPPORT

ಸ್ಥಿರ ಚಞ್ಚಲ irq_t ipiIrq[CONFIG_MAX_ಅಂಕೆ_NODES];

ಶೂನ್ಯ handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    ಯದಿ (clh_is_ipi_pending(getCurrentCPUIndex())) {
        ವಿಕಲ್ಪ (call) {
        ಪಕ್ಷ IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            ಭಙ್ಗ;

#ifdef CONFIG_HAVE_FPU
        ಪಕ್ಷ IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            ಭಙ್ಗ;
#endif /* CONFIG_HAVE_FPU */

        ಸ್ವಭಾವ:
            fail("Invalid remote call");
            ಭಙ್ಗ;
        }

        big_kernel_lock.ಗಂಟು[getCurrentCPUIndex()].ipi = 0;
        ipiIrq[getCurrentCPUIndex()] = irqInvalid;
        ipi_wait();
    }
}

ಶೂನ್ಯ ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}

irq_t ipi_get_irq(ಶೂನ್ಯ)
{
    ಪ್ರತಿಜ್ಞಾ(!(ipiIrq[getCurrentCPUIndex()] == irqInvalid && clh_is_ipi_pending(getCurrentCPUIndex())));
    ಪ್ರತ್ಯಾಗಮ ipiIrq[getCurrentCPUIndex()];
}

ಶೂನ್ಯ ipi_clear_irq(irq_t irq)
{
    ipiIrq[getCurrentCPUIndex()] = irqInvalid;
    ಪ್ರತ್ಯಾಗಮ;
}

/* this function is called with a single hart id. */
ಶೂನ್ಯ ipi_send_target(irq_t irq, word_t hart_id)
{
    word_t hart_mask = BIT(hart_id);
    word_t core_id = hartIDToCoreID(hart_id);
    ಪ್ರತಿಜ್ಞಾ(core_id < CONFIG_MAX_ಅಂಕೆ_NODES);

    ಪ್ರತಿಜ್ಞಾ((ipiIrq[core_id] == irqInvalid) || (ipiIrq[core_id] == irq_reschedule_ipi) ||
           (ipiIrq[core_id] == irq_remote_call_ipi && !clh_is_ipi_pending(core_id)));

    ipiIrq[core_id] = irq;
    fence_rw_rw();
    sbi_send_ipi(hart_mask);
}

#endif
