/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <mode/smp/ipi.h>
#ಅನ್ತರ್ಭಾವ <smp/ipi.h>
#ಅನ್ತರ್ಭಾವ <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT

ಶೂನ್ಯ handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    ಯದಿ (clh_is_ipi_pending(getCurrentCPUIndex())) {
        ವಿಕಲ್ಪ (call) {
        ಪಕ್ಷ IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            ಭಙ್ಗ;

        ಪಕ್ಷ IpiRemoteCall_InvalidatePageStructureCacheASID:
            invalidateLocalPageStructureCacheASID(arg0, arg1);
            ಭಙ್ಗ;

        ಪಕ್ಷ IpiRemoteCall_InvalidateTranslationSingle:
            invalidateLocalTranslationSingle(arg0);
            ಭಙ್ಗ;

        ಪಕ್ಷ IpiRemoteCall_InvalidateTranslationSingleASID:
            invalidateLocalTranslationSingleASID(arg0, arg1);
            ಭಙ್ಗ;

        ಪಕ್ಷ IpiRemoteCall_InvalidateTranslationAll:
            invalidateLocalTranslationAll();
            ಭಙ್ಗ;

        ಪಕ್ಷ IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            ಭಙ್ಗ;

#ifdef CONFIG_VTX
        ಪಕ್ಷ IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            ಭಙ್ಗ;
        ಪಕ್ಷ IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            ಯದಿ (tcb != ಗಂಟು_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            ಭಙ್ಗ;
        }
#endif
        ಸ್ವಭಾವ:
            Mode_handleRemoteCall((IpiModeRemoteCall_t)call, arg0, arg1, arg2);
            ಭಙ್ಗ;
        }

        big_kernel_lock.ಗಂಟು[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_ಸಂಖ್ಯೆ_of_supported_nodes, CONFIG_MAX_ಅಂಕೆ_NODES <= wordBits);

#ifdef CONFIG_USE_LOGICAL_IDS
ಸ್ಥಿರ ಶೂನ್ಯ x86_ipi_send_mask(interrupt_t ipi, word_t mask, bool_t isBlocking)
{
    word_t nr_target_clusters = 0;
    word_t target_clusters[CONFIG_MAX_ಅಂಕೆ_NODES];

    ಕುರು {
        ಪೂರ್ಣಾಙ್ಕ core = wordBits - 1 - clzl(mask);
        target_clusters[nr_target_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        word_t sub_mask = mask & cpu_mapping.other_indexes_in_cluster[core];
        target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[core];
        ಯದಿ (isBlocking) {
            big_kernel_lock.ಗಂಟು[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        ಯಾವತ್ (sub_mask) {
            ಪೂರ್ಣಾಙ್ಕ index = wordBits - 1 - clzl(sub_mask);
            target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[index];
            ಯದಿ (isBlocking) {
                big_kernel_lock.ಗಂಟು[index].ipi = 1;
            }
            sub_mask &= ~BIT(index);
        }

        mask &= ~(cpu_mapping.other_indexes_in_cluster[core] | BIT(core));
        nr_target_clusters++;
    } ಯಾವತ್ (mask != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < nr_target_clusters; i++) {
        apic_send_ipi_cluster(ipi, target_clusters[i]);
    }
}
#endif /* CONFIG_USE_LOGICAL_IDS */

ಶೂನ್ಯ ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    interrupt_t interrupt_ipi = ipi + IRQ_INT_OFFSET;

#ifdef CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#ಅನ್ಯಥಾ
    generic_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#endif /* CONFIG_USE_LOGICAL_IDS */
}
#endif /* ENABLE_SMP_SUPPORT */
