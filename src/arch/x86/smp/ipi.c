/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <mode/smp/ipi.h>
#అన్తర్భావ <smp/ipi.h>
#అన్తర్భావ <smp/lock.h>

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

        స్థితి Ipi_Remote_Call_Invalidate_పుట_Structure_Cache_ASID:
            invalidate_Local_పుట_Structure_Cache_ASID(arg0, arg1);
            ఆపు;

        స్థితి IpiRemoteCall_InvalidateTranslationSingle:
            invalidateLocalTranslationSingle(arg0);
            ఆపు;

        స్థితి IpiRemoteCall_InvalidateTranslationSingleASID:
            invalidateLocalTranslationSingleASID(arg0, arg1);
            ఆపు;

        స్థితి IpiRemoteCall_InvalidateTranslationAll:
            invalidateLocalTranslationAll();
            ఆపు;

        స్థితి IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            ఆపు;

#నిర్వచితమైతే CONFIG_VTX
        స్థితి IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            ఆపు;
        స్థితి IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            ఒకవేళ (tcb != కణుపు_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            ఆపు;
        }
#అయితే_ముగింపు
        అప్రమేయ:
            బహుళకం_handle_Remote_Call((Ipi_బహుళకం_Remote_Call_t)call, arg0, arg1, arg2);
            ఆపు;
        }

        big_kernel_తాళం.కణుపు[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_సంఖ్య_యొక్క_supported_nodes, CONFIG_MAX_అంకె_NODES <= మాట_బిట్లు);

#నిర్వచితమైతే CONFIG_USE_LOGICAL_IDS
స్థిర శూన్య x86_ipi_send_ముసుగు(అంతరాయం_t ipi, మాట_t ముసుగు_2, bool_t isBlocking)
{
    మాట_t nr_లక్ష్యం_clusters = 0;
    మాట_t లక్ష్యం_clusters[CONFIG_MAX_అంకె_NODES];

    చేయి {
        పూర్ణాంకం core = మాట_బిట్లు - 1 - clzl(ముసుగు_2);
        లక్ష్యం_clusters[nr_లక్ష్యం_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        మాట_t sub_ముసుగు = ముసుగు_2 & cpu_mapping.other_indexes_లోపల_cluster[core];
        లక్ష్యం_clusters[nr_లక్ష్యం_clusters] |= cpu_mapping.సూచీ_గమ్యం_logical_id[core];
        ఒకవేళ (isBlocking) {
            big_kernel_తాళం.కణుపు[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        అయితే (sub_ముసుగు) {
            పూర్ణాంకం సూచీ = మాట_బిట్లు - 1 - clzl(sub_ముసుగు);
            లక్ష్యం_clusters[nr_లక్ష్యం_clusters] |= cpu_mapping.సూచీ_గమ్యం_logical_id[సూచీ];
            ఒకవేళ (isBlocking) {
                big_kernel_తాళం.కణుపు[సూచీ].ipi = 1;
            }
            sub_ముసుగు &= ~బిట్టు(సూచీ);
        }

        ముసుగు_2 &= ~(cpu_mapping.other_indexes_లోపల_cluster[core] | బిట్టు(core));
        nr_లక్ష్యం_clusters++;
    } అయితే (ముసుగు_2 != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    కొరకు (పూర్ణాంకం i = 0; i < nr_లక్ష్యం_clusters; i++) {
        apic_send_ipi_cluster(ipi, లక్ష్యం_clusters[i]);
    }
}
#అయితే_ముగింపు /* CONFIG_USE_LOGICAL_IDS */

శూన్య ipi_send_ముసుగు(irq_t ipi, మాట_t ముసుగు_2, bool_t isBlocking)
{
    అంతరాయం_t అంతరాయం_ipi = ipi + IRQ_INT_స్థానభ్రంశం;

#నిర్వచితమైతే CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_ముసుగు(అంతరాయం_ipi, ముసుగు_2, isBlocking);
#లేకపోతే
    generic_ipi_send_ముసుగు(అంతరాయం_ipi, ముసుగు_2, isBlocking);
#అయితే_ముగింపు /* CONFIG_USE_LOGICAL_IDS */
}
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
