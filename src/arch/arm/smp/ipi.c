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
    जर (clh_is_ipi_pending(getCurrentCPUIndex())) {
        निवड (call) {
        स्थिती IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            थांबा;

#ifdef CONFIG_HAVE_FPU
        स्थिती IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            थांबा;
#endif /* CONFIG_HAVE_FPU */

        स्थिती IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            थांबा;

        स्थिती IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            थांबा;

        स्थिती IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            थांबा;

        स्थिती IpiRemoteCall_MaskPrivateInterrupt:
            maskInterrupt(arg0, IDX_TO_IRQT(arg1));
            थांबा;

#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        स्थिती IpiRemoteCall_DeactivatePrivateInterrupt:
            deactivateInterrupt(IDX_TO_IRQT(arg1));
            थांबा;
#endif

#जर defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        स्थिती IpiRemoteCall_VCPUInjectInterrupt: {
            virq_t virq;
            virq.शब्द[0] = arg2;
            handleVCPUInjectInterruptIPI((vcpu_t *) arg0, arg1, virq);
            थांबा;
        }
#endif

        पूर्वनिर्धारित:
            fail("Invalid remote call");
            थांबा;
        }

        big_kernel_lock.गाठ[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

शून्य ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}
#endif /* ENABLE_SMP_SUPPORT */
