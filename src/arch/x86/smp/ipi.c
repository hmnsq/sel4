/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <mode/smp/ipi.h>
#ଅନ୍ତର୍ଭାଵ <smp/ipi.h>
#ଅନ୍ତର୍ଭାଵ <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT

ଶୂନ୍ୟ handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    ଯଦି (clh_is_ipi_pending(getCurrentCPUIndex())) {
        ବାଛ (call) {
        ସ୍ଥିତି IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            ଭାଙ୍ଗ;

        ସ୍ଥିତି IpiRemoteCall_InvalidatePageStructureCacheASID:
            invalidateLocalPageStructureCacheASID(arg0, arg1);
            ଭାଙ୍ଗ;

        ସ୍ଥିତି IpiRemoteCall_InvalidateTranslationSingle:
            invalidateLocalTranslationSingle(arg0);
            ଭାଙ୍ଗ;

        ସ୍ଥିତି IpiRemoteCall_InvalidateTranslationSingleASID:
            invalidateLocalTranslationSingleASID(arg0, arg1);
            ଭାଙ୍ଗ;

        ସ୍ଥିତି IpiRemoteCall_InvalidateTranslationAll:
            invalidateLocalTranslationAll();
            ଭାଙ୍ଗ;

        ସ୍ଥିତି IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            ଭାଙ୍ଗ;

#ifdef CONFIG_VTX
        ସ୍ଥିତି IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            ଭାଙ୍ଗ;
        ସ୍ଥିତି IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            ଯଦି (tcb != ଗଣ୍ଠି_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            ଭାଙ୍ଗ;
        }
#endif
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
            Mode_handleRemoteCall((IpiModeRemoteCall_t)call, arg0, arg1, arg2);
            ଭାଙ୍ଗ;
        }

        big_kernel_lock.ଗଣ୍ଠି[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_ସଂଖ୍ୟା_of_supported_nodes, CONFIG_MAX_ଅଙ୍କ_NODES <= wordBits);

#ifdef CONFIG_USE_LOGICAL_IDS
ସ୍ଥିର ଶୂନ୍ୟ x86_ipi_send_mask(interrupt_t ipi, word_t mask, bool_t isBlocking)
{
    word_t nr_target_clusters = 0;
    word_t target_clusters[CONFIG_MAX_ଅଙ୍କ_NODES];

    କର {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା core = wordBits - 1 - clzl(mask);
        target_clusters[nr_target_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        word_t sub_mask = mask & cpu_mapping.other_indexes_in_cluster[core];
        target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[core];
        ଯଦି (isBlocking) {
            big_kernel_lock.ଗଣ୍ଠି[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        ଯେତେବେଳେ (sub_mask) {
            ପୂର୍ଣ୍ଣସଂଖ୍ୟା index = wordBits - 1 - clzl(sub_mask);
            target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[index];
            ଯଦି (isBlocking) {
                big_kernel_lock.ଗଣ୍ଠି[index].ipi = 1;
            }
            sub_mask &= ~BIT(index);
        }

        mask &= ~(cpu_mapping.other_indexes_in_cluster[core] | BIT(core));
        nr_target_clusters++;
    } ଯେତେବେଳେ (mask != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < nr_target_clusters; i++) {
        apic_send_ipi_cluster(ipi, target_clusters[i]);
    }
}
#endif /* CONFIG_USE_LOGICAL_IDS */

ଶୂନ୍ୟ ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    interrupt_t interrupt_ipi = ipi + IRQ_INT_OFFSET;

#ifdef CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#ଅନ୍ୟଥା
    generic_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#endif /* CONFIG_USE_LOGICAL_IDS */
}
#endif /* ENABLE_SMP_SUPPORT */
