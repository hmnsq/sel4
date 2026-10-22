/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/smp/ipi.h>
#ضمن <smp/lock.h>
#ضمن <util.h>

#که_تعریف_شوی ENABLE_SMP_SUPPORT

فراغ handleRemoteCall(IpiRemoteCall_t call, کلمه_t arg0, کلمه_t arg1, کلمه_t arg2, bool_t irq_لار)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    إذا (clh_is_ipi_pending(getCurrentCPUIndex())) {
        بدل (call) {
        حالة IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_لار);
            اكسر;

#که_تعریف_شوی CONFIG_HAVE_FPU
        حالة IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            اكسر;
#د_که_پای /* CONFIG_HAVE_FPU */

        حالة IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            اكسر;

        حالة IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            اكسر;

        حالة IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            اكسر;

        حالة Ipi_Remote_Call_ماسک_شخصي_مداخله:
            ماسک_مداخله(arg0, IDX_منزل_IRQT(arg1));
            اكسر;

#که_تعریف_شوی CONFIG_ARM_GIC_V3_SUPPORT
        حالة Ipi_Remote_Call_Deactivate_شخصي_مداخله:
            deactivate_مداخله(IDX_منزل_IRQT(arg1));
            اكسر;
#د_که_پای

#إذا defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        حالة Ipi_Remote_Call_VCPUInject_مداخله: {
            virq_t virq;
            virq.ټکي[0] = arg2;
            handle_VCPUInject_مداخله_IPI((vcpu_t *) arg0, arg1, virq);
            اكسر;
        }
#د_که_پای

        افتراضي:
            fail("Invalid remote call");
            اكسر;
        }

        big_kernel_قلف.غوټه[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

فراغ ipi_send_ماسک(irq_t ipi, کلمه_t ماسک_2, bool_t isBlocking)
{
    generic_ipi_send_ماسک(ipi, ماسک_2, isBlocking);
}
#د_که_پای /* ENABLE_SMP_SUPPORT */
