/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <mode/smp/ipi.h>
#अन्तर्भाव <smp/lock.h>
#अन्तर्भाव <util.h>

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT

स्थिर चञ्चल irq_t ipiIrq[CONFIG_MAX_अङ्कः_NODES];

शून्य handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irq_मार्गः)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    यदि (clh_is_ipi_pending(getCurrentCPUIndex())) {
        विकल्प (call) {
        पक्ष IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_मार्गः);
            भङ्ग;

#यदि_परिभाषितम् CONFIG_HAVE_FPU
        पक्ष IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            भङ्ग;
#यदि_अन्तः /* CONFIG_HAVE_FPU */

        स्वभाव:
            fail("Invalid remote call");
            भङ्ग;
        }

        big_kernel_तालकम्.ग्रन्थिः[getCurrentCPUIndex()].ipi = 0;
        ipiIrq[getCurrentCPUIndex()] = irqInvalid;
        ipi_wait();
    }
}

शून्य ipi_send_आवरणम्(irq_t ipi, word_t आवरणम्_2, bool_t isBlocking)
{
    generic_ipi_send_आवरणम्(ipi, आवरणम्_2, isBlocking);
}

irq_t ipi_get_irq(शून्य)
{
    प्रतिज्ञा(!(ipiIrq[getCurrentCPUIndex()] == irqInvalid && clh_is_ipi_pending(getCurrentCPUIndex())));
    प्रत्यागम ipiIrq[getCurrentCPUIndex()];
}

शून्य ipi_clear_irq(irq_t irq)
{
    ipiIrq[getCurrentCPUIndex()] = irqInvalid;
    प्रत्यागम;
}

/* this function is called with a single hart id. */
शून्य ipi_send_target(irq_t irq, word_t hart_id)
{
    word_t hart_आवरणम् = BIT(hart_id);
    word_t core_id = hartIDToCoreID(hart_id);
    प्रतिज्ञा(core_id < CONFIG_MAX_अङ्कः_NODES);

    प्रतिज्ञा((ipiIrq[core_id] == irqInvalid) || (ipiIrq[core_id] == irq_reschedule_ipi) ||
           (ipiIrq[core_id] == irq_remote_call_ipi && !clh_is_ipi_pending(core_id)));

    ipiIrq[core_id] = irq;
    fence_rw_rw();
    sbi_send_ipi(hart_आवरणम्);
}

#यदि_अन्तः
