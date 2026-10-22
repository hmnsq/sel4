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

        पक्ष IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            भङ्ग;

        पक्ष IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            भङ्ग;

        पक्ष IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            भङ्ग;

        पक्ष Ipi_Remote_Call_आवरणम्_वैयक्तिकम्_व्यवधानम्:
            आवरणम्_व्यवधानम्(arg0, IDX_गन्तव्यम्_IRQT(arg1));
            भङ्ग;

#यदि_परिभाषितम् CONFIG_ARM_GIC_V3_SUPPORT
        पक्ष Ipi_Remote_Call_Deactivate_वैयक्तिकम्_व्यवधानम्:
            deactivate_व्यवधानम्(IDX_गन्तव्यम्_IRQT(arg1));
            भङ्ग;
#यदि_अन्तः

#यदि defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        पक्ष Ipi_Remote_Call_VCPUInject_व्यवधानम्: {
            virq_t virq;
            virq.शब्दाः[0] = arg2;
            handle_VCPUInject_व्यवधानम्_IPI((vcpu_t *) arg0, arg1, virq);
            भङ्ग;
        }
#यदि_अन्तः

        स्वभाव:
            fail("Invalid remote call");
            भङ्ग;
        }

        big_kernel_तालकम्.ग्रन्थिः[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

शून्य ipi_send_आवरणम्(irq_t ipi, word_t आवरणम्_2, bool_t isBlocking)
{
    generic_ipi_send_आवरणम्(ipi, आवरणम्_2, isBlocking);
}
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */
