/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/smp/ipi.h>
#ضمن <smp/lock.h>
#ضمن <util.h>

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT

فراغ handleRemoteCall(IpiRemoteCall_t call, لفظ_t arg0, لفظ_t arg1, لفظ_t arg2, bool_t irq_رستو)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    إذا (clh_is_ipi_pending(getCurrentCPUIndex())) {
        بدل (call) {
        حالة IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_رستو);
            اكسر;

#جيڪڏهن_وصف_ٿيل CONFIG_HAVE_FPU
        حالة IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            اكسر;
#جيڪڏهن_پورو /* CONFIG_HAVE_FPU */

        حالة IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            اكسر;

        حالة IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            اكسر;

        حالة IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            اكسر;

        حالة Ipi_Remote_Call_نقاب_ذاتي_مداخلت:
            نقاب_مداخلت(arg0, IDX_منزل_IRQT(arg1));
            اكسر;

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_GIC_V3_SUPPORT
        حالة Ipi_Remote_Call_Deactivate_ذاتي_مداخلت:
            deactivate_مداخلت(IDX_منزل_IRQT(arg1));
            اكسر;
#جيڪڏهن_پورو

#إذا defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        حالة Ipi_Remote_Call_VCPUInject_مداخلت: {
            virq_t virq;
            virq.لفظ[0] = arg2;
            handle_VCPUInject_مداخلت_IPI((vcpu_t *) arg0, arg1, virq);
            اكسر;
        }
#جيڪڏهن_پورو

        افتراضي:
            fail("Invalid remote call");
            اكسر;
        }

        big_kernel_تالو.ڳنڍ[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

فراغ ipi_send_نقاب(irq_t ipi, لفظ_t نقاب_2, bool_t isBlocking)
{
    generic_ipi_send_نقاب(ipi, نقاب_2, isBlocking);
}
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */
