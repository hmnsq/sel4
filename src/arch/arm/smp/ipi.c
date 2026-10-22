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

        ସ୍ଥିତି IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            ଭାଙ୍ଗ;

        ସ୍ଥିତି IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            ଭାଙ୍ଗ;

        ସ୍ଥିତି IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            ଭାଙ୍ଗ;

        ସ୍ଥିତି IpiRemoteCall_MaskPrivateInterrupt:
            maskInterrupt(arg0, IDX_TO_IRQT(arg1));
            ଭାଙ୍ଗ;

#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        ସ୍ଥିତି IpiRemoteCall_DeactivatePrivateInterrupt:
            deactivateInterrupt(IDX_TO_IRQT(arg1));
            ଭାଙ୍ଗ;
#endif

#ଯଦି defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        ସ୍ଥିତି IpiRemoteCall_VCPUInjectInterrupt: {
            virq_t virq;
            virq.ଶବ୍ଦଗୁଡ଼ିକ[0] = arg2;
            handleVCPUInjectInterruptIPI((vcpu_t *) arg0, arg1, virq);
            ଭାଙ୍ଗ;
        }
#endif

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            fail("Invalid remote call");
            ଭାଙ୍ଗ;
        }

        big_kernel_lock.ଗଣ୍ଠି[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

ଶୂନ୍ୟ ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}
#endif /* ENABLE_SMP_SUPPORT */
