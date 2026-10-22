/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <mode/smp/ipi.h>
#समावेश <smp/lock.h>
#समावेश <util.h>

#अगर_घोषित ENABLE_SMP_SUPPORT

व्योम handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    अगर (clh_is_ipi_pending(getCurrentCPUIndex())) {
        चयन (call) {
        हाल IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            अवरोध;

#अगर_घोषित CONFIG_HAVE_FPU
        हाल IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            अवरोध;
#पूर्ण_अगर /* CONFIG_HAVE_FPU */

        हाल IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            अवरोध;

        हाल IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            अवरोध;

        हाल IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            अवरोध;

        हाल IpiRemoteCall_MaskPrivateInterrupt:
            maskInterrupt(arg0, IDX_TO_IRQT(arg1));
            अवरोध;

#अगर_घोषित CONFIG_ARM_GIC_V3_SUPPORT
        हाल IpiRemoteCall_DeactivatePrivateInterrupt:
            deactivateInterrupt(IDX_TO_IRQT(arg1));
            अवरोध;
#पूर्ण_अगर

#अगर defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        हाल IpiRemoteCall_VCPUInjectInterrupt: {
            virq_t virq;
            virq.शब्दगण[0] = arg2;
            handleVCPUInjectInterruptIPI((vcpu_t *) arg0, arg1, virq);
            अवरोध;
        }
#पूर्ण_अगर

        शेष:
            fail("Invalid remote call");
            अवरोध;
        }

        big_kernel_lock.गाँठ[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

व्योम ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */
