/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <mode/smp/ipi.h>
#अन्तर्भाव <smp/lock.h>
#अन्तर्भाव <util.h>

#ifdef ENABLE_SMP_SUPPORT

शून्य handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    सचे (clh_is_ipi_pending(getCurrentCPUIndex())) {
        विकल्प (call) {
        पक्ष IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            भङ्ग;

#ifdef CONFIG_HAVE_FPU
        पक्ष IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            भङ्ग;
#endif /* CONFIG_HAVE_FPU */

        पक्ष IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            भङ्ग;

        पक्ष IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            भङ्ग;

        पक्ष IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            भङ्ग;

        पक्ष IpiRemoteCall_MaskPrivateInterrupt:
            maskInterrupt(arg0, IDX_TO_IRQT(arg1));
            भङ्ग;

#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        पक्ष IpiRemoteCall_DeactivatePrivateInterrupt:
            deactivateInterrupt(IDX_TO_IRQT(arg1));
            भङ्ग;
#endif

#सचे defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        पक्ष IpiRemoteCall_VCPUInjectInterrupt: {
            virq_t virq;
            virq.शब्दाः[0] = arg2;
            handleVCPUInjectInterruptIPI((vcpu_t *) arg0, arg1, virq);
            भङ्ग;
        }
#endif

        स्वभाव:
            fail("Invalid remote call");
            भङ्ग;
        }

        big_kernel_lock.ग्रन्थिः[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

शून्य ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}
#endif /* ENABLE_SMP_SUPPORT */
