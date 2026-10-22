/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <mode/smp/ipi.h>
#include <smp/lock.h>
#include <util.h>

#ifdef ENABLE_SMP_SUPPORT

void handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    གལ་སྲིད (clh_is_ipi_pending(getCurrentCPUIndex())) {
        switch (call) {
        pak_sa IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            bha_nga;

#ifdef CONFIG_HAVE_FPU
        pak_sa IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            bha_nga;
#endif /* CONFIG_HAVE_FPU */

        pak_sa IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            bha_nga;

        pak_sa IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            bha_nga;

        pak_sa IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            bha_nga;

        pak_sa IpiRemoteCall_MaskPrivateInterrupt:
            maskInterrupt(arg0, IDX_TO_IRQT(arg1));
            bha_nga;

#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        pak_sa IpiRemoteCall_DeactivatePrivateInterrupt:
            deactivateInterrupt(IDX_TO_IRQT(arg1));
            bha_nga;
#endif

#གལ་སྲིད defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        pak_sa IpiRemoteCall_VCPUInjectInterrupt: {
            virq_t virq;
            virq.words[0] = arg2;
            handleVCPUInjectInterruptIPI((vcpu_t *) arg0, arg1, virq);
            bha_nga;
        }
#endif

        default:
            fail("Invalid remote call");
            bha_nga;
        }

        big_kernel_lock.node[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

void ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}
#endif /* ENABLE_SMP_SUPPORT */
