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

ساكن متطاير irq_t ipiIrq[CONFIG_MAX_رقم_NODES];

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

        افتراضي:
            fail("Invalid remote call");
            اكسر;
        }

        big_kernel_قفل.گره[getCurrentCPUIndex()].ipi = 0;
        ipiIrq[getCurrentCPUIndex()] = irqInvalid;
        ipi_wait();
    }
}

فراغ ipi_send_ماسک(irq_t ipi, کلمه_t ماسک_2, bool_t isBlocking)
{
    generic_ipi_send_ماسک(ipi, ماسک_2, isBlocking);
}

irq_t ipi_get_irq(فراغ)
{
    أكد(!(ipiIrq[getCurrentCPUIndex()] == irqInvalid && clh_is_ipi_pending(getCurrentCPUIndex())));
    أرجع ipiIrq[getCurrentCPUIndex()];
}

فراغ ipi_clear_irq(irq_t irq)
{
    ipiIrq[getCurrentCPUIndex()] = irqInvalid;
    أرجع;
}

/* this function is called with a single hart id. */
فراغ ipi_send_هدف(irq_t irq, کلمه_t hart_id)
{
    کلمه_t hart_ماسک = بیت(hart_id);
    کلمه_t core_id = hartIDToCoreID(hart_id);
    أكد(core_id < CONFIG_MAX_رقم_NODES);

    أكد((ipiIrq[core_id] == irqInvalid) || (ipiIrq[core_id] == irq_reschedule_ipi) ||
           (ipiIrq[core_id] == irq_remote_call_ipi && !clh_is_ipi_pending(core_id)));

    ipiIrq[core_id] = irq;
    fence_rw_rw();
    sbi_send_ipi(hart_ماسک);
}

#پایان_اگر
