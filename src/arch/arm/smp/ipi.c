/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <mode/smp/ipi.h>
#അന്തര്ഭാവ <smp/lock.h>
#അന്തര്ഭാവ <util.h>

#ifdef ENABLE_SMP_SUPPORT

ശൂന്യം handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    എങ്കിൽ (clh_is_ipi_pending(getCurrentCPUIndex())) {
        തിരഞ്ഞെടുപ്പ് (call) {
        അവസ്ഥ IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            നിർത്തുക;

#ifdef CONFIG_HAVE_FPU
        അവസ്ഥ IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            നിർത്തുക;
#endif /* CONFIG_HAVE_FPU */

        അവസ്ഥ IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            നിർത്തുക;

        അവസ്ഥ IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            നിർത്തുക;

        അവസ്ഥ IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            നിർത്തുക;

        അവസ്ഥ IpiRemoteCall_MaskPrivateInterrupt:
            maskInterrupt(arg0, IDX_TO_IRQT(arg1));
            നിർത്തുക;

#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        അവസ്ഥ IpiRemoteCall_DeactivatePrivateInterrupt:
            deactivateInterrupt(IDX_TO_IRQT(arg1));
            നിർത്തുക;
#endif

#എങ്കിൽ defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        അവസ്ഥ IpiRemoteCall_VCPUInjectInterrupt: {
            virq_t virq;
            virq.വാക്കുകൾ[0] = arg2;
            handleVCPUInjectInterruptIPI((vcpu_t *) arg0, arg1, virq);
            നിർത്തുക;
        }
#endif

        സ്ഥിരസ്ഥിതി:
            fail("Invalid remote call");
            നിർത്തുക;
        }

        big_kernel_lock.കണ്ണി[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

ശൂന്യം ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}
#endif /* ENABLE_SMP_SUPPORT */
