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

        స్థితి IpiRemoteCall_InvalidateTranslationSingle:
            invalidateTranslationSingleLocal(arg0);
            ఆపు;

        స్థితి IpiRemoteCall_InvalidateTranslationASID:
            invalidateTranslationASIDLocal(arg0);
            ఆపు;

        స్థితి IpiRemoteCall_InvalidateTranslationAll:
            invalidateTranslationAllLocal();
            ఆపు;

        స్థితి Ipi_Remote_Call_ముసుగు_వ్యక్తిగత_అంతరాయం:
            ముసుగు_అంతరాయం(arg0, IDX_గమ్యం_IRQT(arg1));
            ఆపు;

#నిర్వచితమైతే CONFIG_ARM_GIC_V3_SUPPORT
        స్థితి Ipi_Remote_Call_Deactivate_వ్యక్తిగత_అంతరాయం:
            deactivate_అంతరాయం(IDX_గమ్యం_IRQT(arg1));
            ఆపు;
#అయితే_ముగింపు

#ఒకవేళ defined CONFIG_ARM_HYPERVISOR_SUPPORT && defined ENABLE_SMP_SUPPORT
        స్థితి Ipi_Remote_Call_VCPUInject_అంతరాయం: {
            virq_t virq;
            virq.పదాలు[0] = arg2;
            handle_VCPUInject_అంతరాయం_IPI((vcpu_t *) arg0, arg1, virq);
            ఆపు;
        }
#అయితే_ముగింపు

        అప్రమేయ:
            fail("Invalid remote call");
            ఆపు;
        }

        big_kernel_తాళం.కణుపు[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

శూన్య ipi_send_ముసుగు(irq_t ipi, మాట_t ముసుగు_2, bool_t isBlocking)
{
    generic_ipi_send_ముసుగు(ipi, ముసుగు_2, isBlocking);
}
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
