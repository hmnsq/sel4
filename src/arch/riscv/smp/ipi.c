/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <mode/smp/ipi.h>
#అన్తర్భావ <smp/lock.h>
#అన్తర్భావ <util.h>

#నిర్వచితమైతే ENABLE_SMP_SUPPORT

స్థిర అస్థిర irq_t ipiIrq[CONFIG_MAX_అంకె_NODES];

శూన్య handleRemoteCall(IpiRemoteCall_t call, మాట_t arg0, మాట_t arg1, మాట_t arg2, bool_t irq_మార్గం)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    ఒకవేళ (clh_is_ipi_pending(getCurrentCPUIndex())) {
        ఎంపిక (call) {
        స్థితి IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_మార్గం);
            ఆపు;

#నిర్వచితమైతే CONFIG_HAVE_FPU
        స్థితి IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            ఆపు;
#అయితే_ముగింపు /* CONFIG_HAVE_FPU */

        అప్రమేయ:
            fail("Invalid remote call");
            ఆపు;
        }

        big_kernel_తాళం.కణుపు[getCurrentCPUIndex()].ipi = 0;
        ipiIrq[getCurrentCPUIndex()] = irqInvalid;
        ipi_wait();
    }
}

శూన్య ipi_send_ముసుగు(irq_t ipi, మాట_t ముసుగు_2, bool_t isBlocking)
{
    generic_ipi_send_ముసుగు(ipi, ముసుగు_2, isBlocking);
}

irq_t ipi_get_irq(శూన్య)
{
    ప్రతిజ్ఞా(!(ipiIrq[getCurrentCPUIndex()] == irqInvalid && clh_is_ipi_pending(getCurrentCPUIndex())));
    తిరిగి ipiIrq[getCurrentCPUIndex()];
}

శూన్య ipi_clear_irq(irq_t irq)
{
    ipiIrq[getCurrentCPUIndex()] = irqInvalid;
    తిరిగి;
}

/* this function is called with a single hart id. */
శూన్య ipi_send_లక్ష్యం(irq_t irq, మాట_t hart_id)
{
    మాట_t hart_ముసుగు = బిట్టు(hart_id);
    మాట_t core_id = hartIDToCoreID(hart_id);
    ప్రతిజ్ఞా(core_id < CONFIG_MAX_అంకె_NODES);

    ప్రతిజ్ఞా((ipiIrq[core_id] == irqInvalid) || (ipiIrq[core_id] == irq_reschedule_ipi) ||
           (ipiIrq[core_id] == irq_remote_call_ipi && !clh_is_ipi_pending(core_id)));

    ipiIrq[core_id] = irq;
    fence_rw_rw();
    sbi_send_ipi(hart_ముసుగు);
}

#అయితే_ముగింపు
