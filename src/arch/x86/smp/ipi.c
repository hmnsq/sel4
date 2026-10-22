/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <mode/smp/ipi.h>
#அந்தர்பாவ <smp/ipi.h>
#அந்தர்பாவ <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT

வெறுமை handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    எனில் (clh_is_ipi_pending(getCurrentCPUIndex())) {
        தேர்வு (call) {
        நிலை IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            நிறுத்து;

        நிலை IpiRemoteCall_InvalidatePageStructureCacheASID:
            invalidateLocalPageStructureCacheASID(arg0, arg1);
            நிறுத்து;

        நிலை IpiRemoteCall_InvalidateTranslationSingle:
            invalidateLocalTranslationSingle(arg0);
            நிறுத்து;

        நிலை IpiRemoteCall_InvalidateTranslationSingleASID:
            invalidateLocalTranslationSingleASID(arg0, arg1);
            நிறுத்து;

        நிலை IpiRemoteCall_InvalidateTranslationAll:
            invalidateLocalTranslationAll();
            நிறுத்து;

        நிலை IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            நிறுத்து;

#ifdef CONFIG_VTX
        நிலை IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            நிறுத்து;
        நிலை IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            எனில் (tcb != முனை_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            நிறுத்து;
        }
#endif
        இயல்பு:
            Mode_handleRemoteCall((IpiModeRemoteCall_t)call, arg0, arg1, arg2);
            நிறுத்து;
        }

        big_kernel_lock.முனை[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_எண்_of_supported_nodes, CONFIG_MAX_இலக்கம்_NODES <= wordBits);

#ifdef CONFIG_USE_LOGICAL_IDS
நிலையான வெறுமை x86_ipi_send_mask(interrupt_t ipi, word_t mask, bool_t isBlocking)
{
    word_t nr_target_clusters = 0;
    word_t target_clusters[CONFIG_MAX_இலக்கம்_NODES];

    செய் {
        முழு core = wordBits - 1 - clzl(mask);
        target_clusters[nr_target_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        word_t sub_mask = mask & cpu_mapping.other_indexes_in_cluster[core];
        target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[core];
        எனில் (isBlocking) {
            big_kernel_lock.முனை[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        போது (sub_mask) {
            முழு index = wordBits - 1 - clzl(sub_mask);
            target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[index];
            எனில் (isBlocking) {
                big_kernel_lock.முனை[index].ipi = 1;
            }
            sub_mask &= ~BIT(index);
        }

        mask &= ~(cpu_mapping.other_indexes_in_cluster[core] | BIT(core));
        nr_target_clusters++;
    } போது (mask != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    க்கு (முழு i = 0; i < nr_target_clusters; i++) {
        apic_send_ipi_cluster(ipi, target_clusters[i]);
    }
}
#endif /* CONFIG_USE_LOGICAL_IDS */

வெறுமை ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    interrupt_t interrupt_ipi = ipi + IRQ_INT_OFFSET;

#ifdef CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#இல்லையெனில்
    generic_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#endif /* CONFIG_USE_LOGICAL_IDS */
}
#endif /* ENABLE_SMP_SUPPORT */
