/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <mode/smp/ipi.h>
#ಅನ್ತರ್ಭಾವ <smp/lock.h>
#ಅನ್ತರ್ಭಾವ <util.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT

ಶೂನ್ಯ handleRemoteCall(IpiRemoteCall_t call, ಶಬ್ದ_t arg0, ಶಬ್ದ_t arg1, ಶಬ್ದ_t arg2, bool_t irq_ಮಾರ್ಗ)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    ಆದರೆ (clh_is_ipi_pending(getCurrentCPUIndex())) {
        ಆಯ್ಕೆ (call) {
        ಸ್ಥಿತಿ IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_ಮಾರ್ಗ);
            ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
        ಸ್ಥಿತಿ IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_HAVE_FPU */

        ಸ್ಥಿತಿ IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ Ipi_Remote_Call_ಮುಖವಾಡ_ಖಾಸಗಿ_ಅಡಚಣೆ:
            ಮುಖವಾಡ_ಅಡಚಣೆ(arg0, IDX_ಗಮ್ಯ_IRQT(arg1));
            ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_GIC_V3_SUPPORT
        ಸ್ಥಿತಿ Ipi_Remote_Call_Deactivate_ಖಾಸಗಿ_ಅಡಚಣೆ:
            deactivate_ಅಡಚಣೆ(IDX_ಗಮ್ಯ_IRQT(arg1));
            ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        ಸ್ಥಿತಿ Ipi_Remote_Call_VCPUInject_ಅಡಚಣೆ: {
            virq_t virq;
            virq.ಪದಗಳು[0] = arg2;
            handle_VCPUInject_ಅಡಚಣೆ_IPI((vcpu_t *) arg0, arg1, virq);
            ನಿಲ್ಲಿಸು;
        }
#ಆದರೆ_ಅಂತ್ಯ

        ಪೂರ್ವನಿಯೋಜಿತ:
            fail("Invalid remote call");
            ನಿಲ್ಲಿಸು;
        }

        big_kernel_ಬೀಗ.ಗಂಟು[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

ಶೂನ್ಯ ipi_send_ಮುಖವಾಡ(irq_t ipi, ಶಬ್ದ_t ಮುಖವಾಡ_2, bool_t isBlocking)
{
    generic_ipi_send_ಮುಖವಾಡ(ipi, ಮುಖವಾಡ_2, isBlocking);
}
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
