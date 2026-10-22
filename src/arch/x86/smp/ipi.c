/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <mode/smp/ipi.h>
#অন্তর্ভাব <smp/ipi.h>
#অন্তর্ভাব <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT

শূন্য handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    যদি (clh_is_ipi_pending(getCurrentCPUIndex())) {
        বিকল্প (call) {
        পক্ষ IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            ভঙ্গ;

        পক্ষ IpiRemoteCall_InvalidatePageStructureCacheASID:
            invalidateLocalPageStructureCacheASID(arg0, arg1);
            ভঙ্গ;

        পক্ষ IpiRemoteCall_InvalidateTranslationSingle:
            invalidateLocalTranslationSingle(arg0);
            ভঙ্গ;

        পক্ষ IpiRemoteCall_InvalidateTranslationSingleASID:
            invalidateLocalTranslationSingleASID(arg0, arg1);
            ভঙ্গ;

        পক্ষ IpiRemoteCall_InvalidateTranslationAll:
            invalidateLocalTranslationAll();
            ভঙ্গ;

        পক্ষ IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            ভঙ্গ;

#ifdef CONFIG_VTX
        পক্ষ IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            ভঙ্গ;
        পক্ষ IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            যদি (tcb != গ্রন্থি_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            ভঙ্গ;
        }
#endif
        স্বভাব:
            Mode_handleRemoteCall((IpiModeRemoteCall_t)call, arg0, arg1, arg2);
            ভঙ্গ;
        }

        big_kernel_lock.গ্রন্থি[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_সংখ্যা_of_supported_nodes, CONFIG_MAX_অঙ্ক_NODES <= wordBits);

#ifdef CONFIG_USE_LOGICAL_IDS
স্থির শূন্য x86_ipi_send_mask(interrupt_t ipi, word_t mask, bool_t isBlocking)
{
    word_t nr_target_clusters = 0;
    word_t target_clusters[CONFIG_MAX_অঙ্ক_NODES];

    কুরু {
        পূর্ণাঙ্ক core = wordBits - 1 - clzl(mask);
        target_clusters[nr_target_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        word_t sub_mask = mask & cpu_mapping.other_indexes_in_cluster[core];
        target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[core];
        যদি (isBlocking) {
            big_kernel_lock.গ্রন্থি[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        যাবত্ (sub_mask) {
            পূর্ণাঙ্ক index = wordBits - 1 - clzl(sub_mask);
            target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[index];
            যদি (isBlocking) {
                big_kernel_lock.গ্রন্থি[index].ipi = 1;
            }
            sub_mask &= ~BIT(index);
        }

        mask &= ~(cpu_mapping.other_indexes_in_cluster[core] | BIT(core));
        nr_target_clusters++;
    } যাবত্ (mask != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    প্রতি (পূর্ণাঙ্ক i = 0; i < nr_target_clusters; i++) {
        apic_send_ipi_cluster(ipi, target_clusters[i]);
    }
}
#endif /* CONFIG_USE_LOGICAL_IDS */

শূন্য ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    interrupt_t interrupt_ipi = ipi + IRQ_INT_OFFSET;

#ifdef CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#অন্যথা
    generic_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#endif /* CONFIG_USE_LOGICAL_IDS */
}
#endif /* ENABLE_SMP_SUPPORT */
