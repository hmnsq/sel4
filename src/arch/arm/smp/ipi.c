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

        حالت IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            بشکن;

        حالت IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            بشکن;

        حالت IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            بشکن;

        حالت IpiRemoteCall_MaskPrivateInterrupt:
            maskInterrupt(arg0, IDX_TO_IRQT(arg1));
            بشکن;

#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        حالت IpiRemoteCall_DeactivatePrivateInterrupt:
            deactivateInterrupt(IDX_TO_IRQT(arg1));
            بشکن;
#endif

#اگر defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        حالت IpiRemoteCall_VCPUInjectInterrupt: {
            virq_t virq;
            virq.واژگان[0] = arg2;
            handleVCPUInjectInterruptIPI((vcpu_t *) arg0, arg1, virq);
            بشکن;
        }
#endif

        پیشفرض:
            fail("Invalid remote call");
            بشکن;
        }

        big_kernel_lock.گره[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

تهی ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}
#endif /* ENABLE_SMP_SUPPORT */
