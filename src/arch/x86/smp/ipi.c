/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <mode/smp/ipi.h>
#شامل <smp/ipi.h>
#شامل <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT

تهی handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    اگر (clh_is_ipi_pending(getCurrentCPUIndex())) {
        انتخاب (call) {
        حالت IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            بشکن;

        حالت IpiRemoteCall_InvalidatePageStructureCacheASID:
            invalidateLocalPageStructureCacheASID(arg0, arg1);
            بشکن;

        حالت IpiRemoteCall_InvalidateTranslationSingle:
            invalidateLocalTranslationSingle(arg0);
            بشکن;

        حالت IpiRemoteCall_InvalidateTranslationSingleASID:
            invalidateLocalTranslationSingleASID(arg0, arg1);
            بشکن;

        حالت IpiRemoteCall_InvalidateTranslationAll:
            invalidateLocalTranslationAll();
            بشکن;

        حالت IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            بشکن;

#ifdef CONFIG_VTX
        حالت IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            بشکن;
        حالت IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            اگر (tcb != گره_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            بشکن;
        }
#endif
        پیشفرض:
            Mode_handleRemoteCall((IpiModeRemoteCall_t)call, arg0, arg1, arg2);
            بشکن;
        }

        big_kernel_lock.گره[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_عدد_of_supported_nodes, CONFIG_MAX_رقم_NODES <= wordBits);

#ifdef CONFIG_USE_LOGICAL_IDS
ایستا تهی x86_ipi_send_mask(interrupt_t ipi, word_t mask, bool_t isBlocking)
{
    word_t nr_target_clusters = 0;
    word_t target_clusters[CONFIG_MAX_رقم_NODES];

    انجامبده {
        صحیح core = wordBits - 1 - clzl(mask);
        target_clusters[nr_target_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        word_t sub_mask = mask & cpu_mapping.other_indexes_in_cluster[core];
        target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[core];
        اگر (isBlocking) {
            big_kernel_lock.گره[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        وقتی (sub_mask) {
            صحیح index = wordBits - 1 - clzl(sub_mask);
            target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[index];
            اگر (isBlocking) {
                big_kernel_lock.گره[index].ipi = 1;
            }
            sub_mask &= ~BIT(index);
        }

        mask &= ~(cpu_mapping.other_indexes_in_cluster[core] | BIT(core));
        nr_target_clusters++;
    } وقتی (mask != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    برای (صحیح i = 0; i < nr_target_clusters; i++) {
        apic_send_ipi_cluster(ipi, target_clusters[i]);
    }
}
#endif /* CONFIG_USE_LOGICAL_IDS */

تهی ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    interrupt_t interrupt_ipi = ipi + IRQ_INT_OFFSET;

#ifdef CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#وگرنه
    generic_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#endif /* CONFIG_USE_LOGICAL_IDS */
}
#endif /* ENABLE_SMP_SUPPORT */
