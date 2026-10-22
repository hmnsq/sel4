/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/smp/ipi.h>
#ضمن <smp/lock.h>
#ضمن <util.h>

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT

فراغ handleRemoteCall(IpiRemoteCall_t call, لفظ_t arg0, لفظ_t arg1, لفظ_t arg2, bool_t irq_رستہ)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    إذا (clh_is_ipi_pending(getCurrentCPUIndex())) {
        بدل (call) {
        حالة IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_رستہ);
            اكسر;

#جے_تعریف_شدہ CONFIG_HAVE_FPU
        حالة IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            اكسر;
#جے_انت /* CONFIG_HAVE_FPU */

        حالة IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            اكسر;

        حالة IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            اكسر;

        حالة IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            اكسر;

        حالة Ipi_Remote_Call_نقاب_نجی_رکاوٹ:
            نقاب_رکاوٹ(arg0, IDX_منزل_IRQT(arg1));
            اكسر;

#جے_تعریف_شدہ CONFIG_ARM_GIC_V3_SUPPORT
        حالة Ipi_Remote_Call_Deactivate_نجی_رکاوٹ:
            deactivate_رکاوٹ(IDX_منزل_IRQT(arg1));
            اكسر;
#جے_انت

#إذا defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        حالة Ipi_Remote_Call_VCPUInject_رکاوٹ: {
            virq_t virq;
            virq.الفاظ[0] = arg2;
            handle_VCPUInject_رکاوٹ_IPI((vcpu_t *) arg0, arg1, virq);
            اكسر;
        }
#جے_انت

        افتراضي:
            fail("Invalid remote call");
            اكسر;
        }

        big_kernel_جندرا.گرہ[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

فراغ ipi_send_نقاب(irq_t ipi, لفظ_t نقاب_2, bool_t isBlocking)
{
    generic_ipi_send_نقاب(ipi, نقاب_2, isBlocking);
}
#جے_انت /* ENABLE_SMP_SUPPORT */
