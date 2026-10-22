/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <mode/smp/ipi.h>
#شامل <smp/lock.h>
#شامل <util.h>

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT

خالی handleRemoteCall(IpiRemoteCall_t call, لفظ_t arg0, لفظ_t arg1, لفظ_t arg2, bool_t irq_راستہ)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    اگر (clh_is_ipi_pending(getCurrentCPUIndex())) {
        انتخاب (call) {
        حالت IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_راستہ);
            توڑو;

#اگر_تعریف_شدہ CONFIG_HAVE_FPU
        حالت IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            توڑو;
#اگر_ختم /* CONFIG_HAVE_FPU */

        حالت IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            توڑو;

        حالت IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            توڑو;

        حالت IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            توڑو;

        حالت Ipi_Remote_Call_نقاب_نجی_مداخلت:
            نقاب_مداخلت(arg0, IDX_مقصد_IRQT(arg1));
            توڑو;

#اگر_تعریف_شدہ CONFIG_ARM_GIC_V3_SUPPORT
        حالت Ipi_Remote_Call_Deactivate_نجی_مداخلت:
            deactivate_مداخلت(IDX_مقصد_IRQT(arg1));
            توڑو;
#اگر_ختم

#اگر defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        حالت Ipi_Remote_Call_VCPUInject_مداخلت: {
            virq_t virq;
            virq.الفاظ[0] = arg2;
            handle_VCPUInject_مداخلت_IPI((vcpu_t *) arg0, arg1, virq);
            توڑو;
        }
#اگر_ختم

        طےشدہ:
            fail("Invalid remote call");
            توڑو;
        }

        big_kernel_تالا.گرہ[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

خالی ipi_send_نقاب(irq_t ipi, لفظ_t نقاب_2, bool_t isBlocking)
{
    generic_ipi_send_نقاب(ipi, نقاب_2, isBlocking);
}
#اگر_ختم /* ENABLE_SMP_SUPPORT */
