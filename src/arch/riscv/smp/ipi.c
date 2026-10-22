/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <mode/smp/ipi.h>
#شامل <smp/lock.h>
#شامل <util.h>

#ifdef ENABLE_SMP_SUPPORT

ایستا فرار irq_t ipiIrq[CONFIG_MAX_رقم_NODES];

تهی handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    اگر (clh_is_ipi_pending(getCurrentCPUIndex())) {
        انتخاب (call) {
        حالت IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            بشکن;

#ifdef CONFIG_HAVE_FPU
        حالت IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            بشکن;
#endif /* CONFIG_HAVE_FPU */

        پیشفرض:
            fail("Invalid remote call");
            بشکن;
        }

        big_kernel_lock.گره[getCurrentCPUIndex()].ipi = 0;
        ipiIrq[getCurrentCPUIndex()] = irqInvalid;
        ipi_wait();
    }
}

تهی ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}

irq_t ipi_get_irq(تهی)
{
    أكد(!(ipiIrq[getCurrentCPUIndex()] == irqInvalid && clh_is_ipi_pending(getCurrentCPUIndex())));
    بازگشت ipiIrq[getCurrentCPUIndex()];
}

تهی ipi_clear_irq(irq_t irq)
{
    ipiIrq[getCurrentCPUIndex()] = irqInvalid;
    بازگشت;
}

/* this function is called with a single hart id. */
تهی ipi_send_target(irq_t irq, word_t hart_id)
{
    word_t hart_mask = BIT(hart_id);
    word_t core_id = hartIDToCoreID(hart_id);
    أكد(core_id < CONFIG_MAX_رقم_NODES);

    أكد((ipiIrq[core_id] == irqInvalid) || (ipiIrq[core_id] == irq_reschedule_ipi) ||
           (ipiIrq[core_id] == irq_remote_call_ipi && !clh_is_ipi_pending(core_id)));

    ipiIrq[core_id] = irq;
    fence_rw_rw();
    sbi_send_ipi(hart_mask);
}

#endif
