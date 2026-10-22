/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <mode/smp/ipi.h>
#অন্তর্ভাব <smp/lock.h>
#অন্তর্ভাব <util.h>

#ifdef ENABLE_SMP_SUPPORT

স্থির অস্থির irq_t ipiIrq[CONFIG_MAX_অঙ্ক_NODES];

শূন্য handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    যদি (clh_is_ipi_pending(getCurrentCPUIndex())) {
        নির্বাচন (call) {
        ক্ষেত্র IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            বিরতি;

#ifdef CONFIG_HAVE_FPU
        ক্ষেত্র IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            বিরতি;
#endif /* CONFIG_HAVE_FPU */

        পূর্বনির্ধারিত:
            fail("Invalid remote call");
            বিরতি;
        }

        big_kernel_lock.গ্রন্থি[getCurrentCPUIndex()].ipi = 0;
        ipiIrq[getCurrentCPUIndex()] = irqInvalid;
        ipi_wait();
    }
}

শূন্য ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}

irq_t ipi_get_irq(শূন্য)
{
    প্রতিজ্ঞা(!(ipiIrq[getCurrentCPUIndex()] == irqInvalid && clh_is_ipi_pending(getCurrentCPUIndex())));
    ফেরত ipiIrq[getCurrentCPUIndex()];
}

শূন্য ipi_clear_irq(irq_t irq)
{
    ipiIrq[getCurrentCPUIndex()] = irqInvalid;
    ফেরত;
}

/* this function is called with a single hart id. */
শূন্য ipi_send_target(irq_t irq, word_t hart_id)
{
    word_t hart_mask = BIT(hart_id);
    word_t core_id = hartIDToCoreID(hart_id);
    প্রতিজ্ঞা(core_id < CONFIG_MAX_অঙ্ক_NODES);

    প্রতিজ্ঞা((ipiIrq[core_id] == irqInvalid) || (ipiIrq[core_id] == irq_reschedule_ipi) ||
           (ipiIrq[core_id] == irq_remote_call_ipi && !clh_is_ipi_pending(core_id)));

    ipiIrq[core_id] = irq;
    fence_rw_rw();
    sbi_send_ipi(hart_mask);
}

#endif
