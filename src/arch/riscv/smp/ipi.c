/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <mode/smp/ipi.h>
#അന്തര്ഭാവ <smp/lock.h>
#അന്തര്ഭാവ <util.h>

#ifdef ENABLE_SMP_SUPPORT

സ്ഥിതം അസ്ഥിര irq_t ipiIrq[CONFIG_MAX_അക്കം_NODES];

ശൂന്യം handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    എങ്കിൽ (clh_is_ipi_pending(getCurrentCPUIndex())) {
        തിരഞ്ഞെടുപ്പ് (call) {
        അവസ്ഥ IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            നിർത്തുക;

#ifdef CONFIG_HAVE_FPU
        അവസ്ഥ IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            നിർത്തുക;
#endif /* CONFIG_HAVE_FPU */

        സ്ഥിരസ്ഥിതി:
            fail("Invalid remote call");
            നിർത്തുക;
        }

        big_kernel_lock.കണ്ണി[getCurrentCPUIndex()].ipi = 0;
        ipiIrq[getCurrentCPUIndex()] = irqInvalid;
        ipi_wait();
    }
}

ശൂന്യം ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}

irq_t ipi_get_irq(ശൂന്യം)
{
    പ്രതിജ്ഞാ(!(ipiIrq[getCurrentCPUIndex()] == irqInvalid && clh_is_ipi_pending(getCurrentCPUIndex())));
    തിരികെ ipiIrq[getCurrentCPUIndex()];
}

ശൂന്യം ipi_clear_irq(irq_t irq)
{
    ipiIrq[getCurrentCPUIndex()] = irqInvalid;
    തിരികെ;
}

/* this function is called with a single hart id. */
ശൂന്യം ipi_send_target(irq_t irq, word_t hart_id)
{
    word_t hart_mask = BIT(hart_id);
    word_t core_id = hartIDToCoreID(hart_id);
    പ്രതിജ്ഞാ(core_id < CONFIG_MAX_അക്കം_NODES);

    പ്രതിജ്ഞാ((ipiIrq[core_id] == irqInvalid) || (ipiIrq[core_id] == irq_reschedule_ipi) ||
           (ipiIrq[core_id] == irq_remote_call_ipi && !clh_is_ipi_pending(core_id)));

    ipiIrq[core_id] = irq;
    fence_rw_rw();
    sbi_send_ipi(hart_mask);
}

#endif
