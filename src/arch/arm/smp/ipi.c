/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <mode/smp/ipi.h>
#ಅನ್ತರ್ಭಾವ <smp/lock.h>
#ಅನ್ತರ್ಭಾವ <util.h>

#ifdef ENABLE_SMP_SUPPORT

ಶೂನ್ಯ handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    ಯದಿ (clh_is_ipi_pending(getCurrentCPUIndex())) {
        ವಿಕಲ್ಪ (call) {
        ಪಕ್ಷ IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            ಭಙ್ಗ;

#ifdef CONFIG_HAVE_FPU
        ಪಕ್ಷ IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            ಭಙ್ಗ;
#endif /* CONFIG_HAVE_FPU */

        ಪಕ್ಷ IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            ಭಙ್ಗ;

        ಪಕ್ಷ IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            ಭಙ್ಗ;

        ಪಕ್ಷ IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            ಭಙ್ಗ;

        ಪಕ್ಷ IpiRemoteCall_MaskPrivateInterrupt:
            maskInterrupt(arg0, IDX_TO_IRQT(arg1));
            ಭಙ್ಗ;

#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        ಪಕ್ಷ IpiRemoteCall_DeactivatePrivateInterrupt:
            deactivateInterrupt(IDX_TO_IRQT(arg1));
            ಭಙ್ಗ;
#endif

#ಯದಿ defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        ಪಕ್ಷ IpiRemoteCall_VCPUInjectInterrupt: {
            virq_t virq;
            virq.ಪದಗಳು[0] = arg2;
            handleVCPUInjectInterruptIPI((vcpu_t *) arg0, arg1, virq);
            ಭಙ್ಗ;
        }
#endif

        ಸ್ವಭಾವ:
            fail("Invalid remote call");
            ಭಙ್ಗ;
        }

        big_kernel_lock.ಗಂಟು[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

ಶೂನ್ಯ ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}
#endif /* ENABLE_SMP_SUPPORT */
