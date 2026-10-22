/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/smp/ipi.h>
#ضمن <smp/lock.h>
#ضمن <util.h>

#ifdef ENABLE_SMP_SUPPORT

فراغ handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    𐡄𐡍 (clh_is_ipi_pending(getCurrentCPUIndex())) {
        بدل (call) {
        حالة IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            اكسر;

#ifdef CONFIG_HAVE_FPU
        حالة IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            اكسر;
#endif /* CONFIG_HAVE_FPU */

        حالة IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            اكسر;

        حالة IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            اكسر;

        حالة IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            اكسر;

        حالة IpiRemoteCall_MaskPrivateInterrupt:
            maskInterrupt(arg0, IDX_TO_IRQT(arg1));
            اكسر;

#ifdef CONFIG_ARM_GIC_V3_SUPPORT
        حالة IpiRemoteCall_DeactivatePrivateInterrupt:
            deactivateInterrupt(IDX_TO_IRQT(arg1));
            اكسر;
#endif

#𐡄𐡍 defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        حالة IpiRemoteCall_VCPUInjectInterrupt: {
            virq_t virq;
            virq.words[0] = arg2;
            handleVCPUInjectInterruptIPI((vcpu_t *) arg0, arg1, virq);
            اكسر;
        }
#endif

        افتراضي:
            fail("Invalid remote call");
            اكسر;
        }

        big_kernel_lock.node[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

فراغ ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    generic_ipi_send_mask(ipi, mask, isBlocking);
}
#endif /* ENABLE_SMP_SUPPORT */
