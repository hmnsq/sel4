/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <mode/smp/ipi.h>
#અન્તર્ભાવ <smp/ipi.h>
#અન્તર્ભાવ <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT

શૂન્ય handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    જો (clh_is_ipi_pending(getCurrentCPUIndex())) {
        પસંદગી (call) {
        સ્થિતિ IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            તોડો;

        સ્થિતિ IpiRemoteCall_InvalidatePageStructureCacheASID:
            invalidateLocalPageStructureCacheASID(arg0, arg1);
            તોડો;

        સ્થિતિ IpiRemoteCall_InvalidateTranslationSingle:
            invalidateLocalTranslationSingle(arg0);
            તોડો;

        સ્થિતિ IpiRemoteCall_InvalidateTranslationSingleASID:
            invalidateLocalTranslationSingleASID(arg0, arg1);
            તોડો;

        સ્થિતિ IpiRemoteCall_InvalidateTranslationAll:
            invalidateLocalTranslationAll();
            તોડો;

        સ્થિતિ IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            તોડો;

#ifdef CONFIG_VTX
        સ્થિતિ IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            તોડો;
        સ્થિતિ IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            જો (tcb != ગાંઠ_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            તોડો;
        }
#endif
        મૂળભૂત:
            Mode_handleRemoteCall((IpiModeRemoteCall_t)call, arg0, arg1, arg2);
            તોડો;
        }

        big_kernel_lock.ગાંઠ[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_સંખ્યા_of_supported_nodes, CONFIG_MAX_અંક_NODES <= wordBits);

#ifdef CONFIG_USE_LOGICAL_IDS
સ્થિર શૂન્ય x86_ipi_send_mask(interrupt_t ipi, word_t mask, bool_t isBlocking)
{
    word_t nr_target_clusters = 0;
    word_t target_clusters[CONFIG_MAX_અંક_NODES];

    કરો {
        પૂર્ણાંક core = wordBits - 1 - clzl(mask);
        target_clusters[nr_target_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        word_t sub_mask = mask & cpu_mapping.other_indexes_in_cluster[core];
        target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[core];
        જો (isBlocking) {
            big_kernel_lock.ગાંઠ[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        જ્યારે (sub_mask) {
            પૂર્ણાંક index = wordBits - 1 - clzl(sub_mask);
            target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[index];
            જો (isBlocking) {
                big_kernel_lock.ગાંઠ[index].ipi = 1;
            }
            sub_mask &= ~BIT(index);
        }

        mask &= ~(cpu_mapping.other_indexes_in_cluster[core] | BIT(core));
        nr_target_clusters++;
    } જ્યારે (mask != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    માટે (પૂર્ણાંક i = 0; i < nr_target_clusters; i++) {
        apic_send_ipi_cluster(ipi, target_clusters[i]);
    }
}
#endif /* CONFIG_USE_LOGICAL_IDS */

શૂન્ય ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    interrupt_t interrupt_ipi = ipi + IRQ_INT_OFFSET;

#ifdef CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#નહીંતર
    generic_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#endif /* CONFIG_USE_LOGICAL_IDS */
}
#endif /* ENABLE_SMP_SUPPORT */
