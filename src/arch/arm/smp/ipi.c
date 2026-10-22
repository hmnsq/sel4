/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <mode/smp/ipi.h>
#અન્તર્ભાવ <smp/lock.h>
#અન્તર્ભાવ <util.h>

#ifdef ENABLE_SMP_SUPPORT

શૂન્ય handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    જો (clh_is_ipi_pending(getCurrentCPUIndex())) {
        પસંદગી (call) {
        સ્થિતિ IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            તોડો;

#ifdef CONFIG_HAVE_FPU
        સ્થિતિ IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            તોડો;
#endif /* CONFIG_HAVE_FPU */

        સ્થિતિ IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            તોડો;

        સ્થિતિ IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            તોડો;

        સ્થિતિ IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            તોડો;

        સ્થિતિ IpiRemoteCall_MaskPrivateInterrupt:
            maskInterrupt(arg0, IDX_TO_IRQT(arg1));
            તોડો;

#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        સ્થિતિ IpiRemoteCall_DeactivatePrivateInterrupt:
            deactivateInterrupt(IDX_TO_IRQT(arg1));
            તોડો;
#endif

#જો defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        સ્થિતિ IpiRemoteCall_VCPUInjectInterrupt: {
            virq_t virq;
            virq.શબ્દો[0] = arg2;
            handleVCPUInjectInterruptIPI((vcpu_t *) arg0, arg1, virq);
            તોડો;
        }
#endif

        મૂળભૂત:
            fail("Invalid remote call");
            તોડો;
        }

        big_kernel_lock.ગાંઠ[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

શૂન્ય ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}
#endif /* ENABLE_SMP_SUPPORT */
