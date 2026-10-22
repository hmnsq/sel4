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

        ক্ষেত্র IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            বিরতি;

        ক্ষেত্র IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            বিরতি;

        ক্ষেত্র IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            বিরতি;

        ক্ষেত্র IpiRemoteCall_MaskPrivateInterrupt:
            maskInterrupt(arg0, IDX_TO_IRQT(arg1));
            বিরতি;

#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        ক্ষেত্র IpiRemoteCall_DeactivatePrivateInterrupt:
            deactivateInterrupt(IDX_TO_IRQT(arg1));
            বিরতি;
#endif

#যদি defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        ক্ষেত্র IpiRemoteCall_VCPUInjectInterrupt: {
            virq_t virq;
            virq.শব্দগুলি[0] = arg2;
            handleVCPUInjectInterruptIPI((vcpu_t *) arg0, arg1, virq);
            বিরতি;
        }
#endif

        পূর্বনির্ধারিত:
            fail("Invalid remote call");
            বিরতি;
        }

        big_kernel_lock.গ্রন্থি[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

শূন্য ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}
#endif /* ENABLE_SMP_SUPPORT */
