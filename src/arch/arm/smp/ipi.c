/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <mode/smp/ipi.h>
#অন্তর্ভাৱ <smp/lock.h>
#অন্তর্ভাৱ <util.h>

#ifdef ENABLE_SMP_SUPPORT

শূন্য handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    যদি (clh_is_ipi_pending(getCurrentCPUIndex())) {
        ৱিকল্প (call) {
        পক্ষ IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            ভঙ্গ;

#ifdef CONFIG_HAVE_FPU
        পক্ষ IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            ভঙ্গ;
#endif /* CONFIG_HAVE_FPU */

        পক্ষ IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            ভঙ্গ;

        পক্ষ IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            ভঙ্গ;

        পক্ষ IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            ভঙ্গ;

        পক্ষ IpiRemoteCall_MaskPrivateInterrupt:
            maskInterrupt(arg0, IDX_TO_IRQT(arg1));
            ভঙ্গ;

#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        পক্ষ IpiRemoteCall_DeactivatePrivateInterrupt:
            deactivateInterrupt(IDX_TO_IRQT(arg1));
            ভঙ্গ;
#endif

#যদি defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        পক্ষ IpiRemoteCall_VCPUInjectInterrupt: {
            virq_t virq;
            virq.শব্দগুলি[0] = arg2;
            handleVCPUInjectInterruptIPI((vcpu_t *) arg0, arg1, virq);
            ভঙ্গ;
        }
#endif

        স্ৱভাৱ:
            fail("Invalid remote call");
            ভঙ্গ;
        }

        big_kernel_lock.গ্ৰন্থি[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

শূন্য ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}
#endif /* ENABLE_SMP_SUPPORT */
