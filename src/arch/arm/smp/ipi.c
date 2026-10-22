/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/smp/ipi.h>
#ضمن <smp/lock.h>
#ضمن <util.h>

#اگر_تعریف_شده ENABLE_SMP_SUPPORT

فراغ handleRemoteCall(IpiRemoteCall_t call, کلمه_t arg0, کلمه_t arg1, کلمه_t arg2, bool_t irq_مسیر)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    إذا (clh_is_ipi_pending(getCurrentCPUIndex())) {
        بدل (call) {
        حالة IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_مسیر);
            اكسر;

#اگر_تعریف_شده CONFIG_HAVE_FPU
        حالة IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            اكسر;
#پایان_اگر /* CONFIG_HAVE_FPU */

        حالة IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            اكسر;

        حالة IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            اكسر;

        حالة IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            اكسر;

        حالة Ipi_Remote_Call_ماسک_خصوصی_وقفه:
            ماسک_وقفه(arg0, IDX_مقصد_IRQT(arg1));
            اكسر;

#اگر_تعریف_شده CONFIG_ARM_GIC_V3_SUPPORT
        حالة Ipi_Remote_Call_Deactivate_خصوصی_وقفه:
            deactivate_وقفه(IDX_مقصد_IRQT(arg1));
            اكسر;
#پایان_اگر

#إذا defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        حالة Ipi_Remote_Call_VCPUInject_وقفه: {
            virq_t virq;
            virq.واژگان[0] = arg2;
            handle_VCPUInject_وقفه_IPI((vcpu_t *) arg0, arg1, virq);
            اكسر;
        }
#پایان_اگر

        افتراضي:
            fail("Invalid remote call");
            اكسر;
        }

        big_kernel_قفل.گره[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

فراغ ipi_send_ماسک(irq_t ipi, کلمه_t ماسک_2, bool_t isBlocking)
{
    generic_ipi_send_ماسک(ipi, ماسک_2, isBlocking);
}
#پایان_اگر /* ENABLE_SMP_SUPPORT */
