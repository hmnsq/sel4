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

ساکن متلون irq_t ipiIrq[CONFIG_MAX_عدد_NODES];

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

        طےشدہ:
            fail("Invalid remote call");
            توڑو;
        }

        big_kernel_تالا.گرہ[getCurrentCPUIndex()].ipi = 0;
        ipiIrq[getCurrentCPUIndex()] = irqInvalid;
        ipi_wait();
    }
}

خالی ipi_send_نقاب(irq_t ipi, لفظ_t نقاب_2, bool_t isBlocking)
{
    generic_ipi_send_نقاب(ipi, نقاب_2, isBlocking);
}

irq_t ipi_get_irq(خالی)
{
    أكد(!(ipiIrq[getCurrentCPUIndex()] == irqInvalid && clh_is_ipi_pending(getCurrentCPUIndex())));
    واپس ipiIrq[getCurrentCPUIndex()];
}

خالی ipi_clear_irq(irq_t irq)
{
    ipiIrq[getCurrentCPUIndex()] = irqInvalid;
    واپس;
}

/* this function is called with a single hart id. */
خالی ipi_send_ہدف(irq_t irq, لفظ_t hart_id)
{
    لفظ_t hart_نقاب = ثنائی_ہندسہ(hart_id);
    لفظ_t core_id = hartIDToCoreID(hart_id);
    أكد(core_id < CONFIG_MAX_عدد_NODES);

    أكد((ipiIrq[core_id] == irqInvalid) || (ipiIrq[core_id] == irq_reschedule_ipi) ||
           (ipiIrq[core_id] == irq_remote_call_ipi && !clh_is_ipi_pending(core_id)));

    ipiIrq[core_id] = irq;
    fence_rw_rw();
    sbi_send_ipi(hart_نقاب);
}

#اگر_ختم
