/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <mode/smp/ipi.h>
#அந்தர்பாவ <smp/lock.h>
#அந்தர்பாவ <util.h>

#ifdef ENABLE_SMP_SUPPORT

வெறுமை handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    எனில் (clh_is_ipi_pending(getCurrentCPUIndex())) {
        தேர்வு (call) {
        நிலை IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            நிறுத்து;

#ifdef CONFIG_HAVE_FPU
        நிலை IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            நிறுத்து;
#endif /* CONFIG_HAVE_FPU */

        நிலை IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            நிறுத்து;

        நிலை IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            நிறுத்து;

        நிலை IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            நிறுத்து;

        நிலை IpiRemoteCall_MaskPrivateInterrupt:
            maskInterrupt(arg0, IDX_TO_IRQT(arg1));
            நிறுத்து;

#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        நிலை IpiRemoteCall_DeactivatePrivateInterrupt:
            deactivateInterrupt(IDX_TO_IRQT(arg1));
            நிறுத்து;
#endif

#எனில் defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        நிலை IpiRemoteCall_VCPUInjectInterrupt: {
            virq_t virq;
            virq.சொற்கள்[0] = arg2;
            handleVCPUInjectInterruptIPI((vcpu_t *) arg0, arg1, virq);
            நிறுத்து;
        }
#endif

        இயல்பு:
            fail("Invalid remote call");
            நிறுத்து;
        }

        big_kernel_lock.முனை[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

வெறுமை ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}
#endif /* ENABLE_SMP_SUPPORT */
