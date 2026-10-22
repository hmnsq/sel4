/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <mode/smp/ipi.h>
#ଅନ୍ତର୍ଭାଵ <smp/lock.h>
#ଅନ୍ତର୍ଭାଵ <util.h>

#ifdef ENABLE_SMP_SUPPORT

ସ୍ଥିର ଅସ୍ଥିର irq_t ipiIrq[CONFIG_MAX_ଅଙ୍କ_NODES];

ଶୂନ୍ୟ handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    ଯଦି (clh_is_ipi_pending(getCurrentCPUIndex())) {
        ବାଛ (call) {
        ସ୍ଥିତି IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            ଭାଙ୍ଗ;

#ifdef CONFIG_HAVE_FPU
        ସ୍ଥିତି IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            ଭାଙ୍ଗ;
#endif /* CONFIG_HAVE_FPU */

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            fail("Invalid remote call");
            ଭାଙ୍ଗ;
        }

        big_kernel_lock.ଗଣ୍ଠି[getCurrentCPUIndex()].ipi = 0;
        ipiIrq[getCurrentCPUIndex()] = irqInvalid;
        ipi_wait();
    }
}

ଶୂନ୍ୟ ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}

irq_t ipi_get_irq(ଶୂନ୍ୟ)
{
    ପ୍ରତିଜ୍ଞା(!(ipiIrq[getCurrentCPUIndex()] == irqInvalid && clh_is_ipi_pending(getCurrentCPUIndex())));
    ଫେରନ୍ତୁ ipiIrq[getCurrentCPUIndex()];
}

ଶୂନ୍ୟ ipi_clear_irq(irq_t irq)
{
    ipiIrq[getCurrentCPUIndex()] = irqInvalid;
    ଫେରନ୍ତୁ;
}

/* this function is called with a single hart id. */
ଶୂନ୍ୟ ipi_send_target(irq_t irq, word_t hart_id)
{
    word_t hart_mask = BIT(hart_id);
    word_t core_id = hartIDToCoreID(hart_id);
    ପ୍ରତିଜ୍ଞା(core_id < CONFIG_MAX_ଅଙ୍କ_NODES);

    ପ୍ରତିଜ୍ଞା((ipiIrq[core_id] == irqInvalid) || (ipiIrq[core_id] == irq_reschedule_ipi) ||
           (ipiIrq[core_id] == irq_remote_call_ipi && !clh_is_ipi_pending(core_id)));

    ipiIrq[core_id] = irq;
    fence_rw_rw();
    sbi_send_ipi(hart_mask);
}

#endif
