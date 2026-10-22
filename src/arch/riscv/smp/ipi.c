/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <mode/smp/ipi.h>
#அந்தர்பாவ <smp/lock.h>
#அந்தர்பாவ <util.h>

#ifdef ENABLE_SMP_SUPPORT

நிலையான நிலையற்ற irq_t ipiIrq[CONFIG_MAX_இலக்கம்_NODES];

வெறுமை handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    எனில் (clh_is_ipi_pending(getCurrentCPUIndex())) {
        தேர்வு (call) {
        நிலை IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            நிறுத்து;

#ifdef CONFIG_HAVE_FPU
        நிலை IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            நிறுத்து;
#endif /* CONFIG_HAVE_FPU */

        இயல்பு:
            fail("Invalid remote call");
            நிறுத்து;
        }

        big_kernel_lock.முனை[getCurrentCPUIndex()].ipi = 0;
        ipiIrq[getCurrentCPUIndex()] = irqInvalid;
        ipi_wait();
    }
}

வெறுமை ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}

irq_t ipi_get_irq(வெறுமை)
{
    ப்ரதிஜ்ஞா(!(ipiIrq[getCurrentCPUIndex()] == irqInvalid && clh_is_ipi_pending(getCurrentCPUIndex())));
    திரும்பு ipiIrq[getCurrentCPUIndex()];
}

வெறுமை ipi_clear_irq(irq_t irq)
{
    ipiIrq[getCurrentCPUIndex()] = irqInvalid;
    திரும்பு;
}

/* this function is called with a single hart id. */
வெறுமை ipi_send_target(irq_t irq, word_t hart_id)
{
    word_t hart_mask = BIT(hart_id);
    word_t core_id = hartIDToCoreID(hart_id);
    ப்ரதிஜ்ஞா(core_id < CONFIG_MAX_இலக்கம்_NODES);

    ப்ரதிஜ்ஞா((ipiIrq[core_id] == irqInvalid) || (ipiIrq[core_id] == irq_reschedule_ipi) ||
           (ipiIrq[core_id] == irq_remote_call_ipi && !clh_is_ipi_pending(core_id)));

    ipiIrq[core_id] = irq;
    fence_rw_rw();
    sbi_send_ipi(hart_mask);
}

#endif
