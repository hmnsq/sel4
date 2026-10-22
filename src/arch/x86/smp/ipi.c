/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <mode/smp/ipi.h>
#अन्तर्भाव <smp/ipi.h>
#अन्तर्भाव <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT

शून्य handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    जइ (clh_is_ipi_pending(getCurrentCPUIndex())) {
        विकल्प (call) {
        पक्ष IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            भङ्ग;

        पक्ष IpiRemoteCall_InvalidatePageStructureCacheASID:
            invalidateLocalPageStructureCacheASID(arg0, arg1);
            भङ्ग;

        पक्ष IpiRemoteCall_InvalidateTranslationSingle:
            invalidateLocalTranslationSingle(arg0);
            भङ्ग;

        पक्ष IpiRemoteCall_InvalidateTranslationSingleASID:
            invalidateLocalTranslationSingleASID(arg0, arg1);
            भङ्ग;

        पक्ष IpiRemoteCall_InvalidateTranslationAll:
            invalidateLocalTranslationAll();
            भङ्ग;

        पक्ष IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            भङ्ग;

#ifdef CONFIG_VTX
        पक्ष IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            भङ्ग;
        पक्ष IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            जइ (tcb != ग्रन्थिः_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            भङ्ग;
        }
#endif
        स्वभाव:
            Mode_handleRemoteCall((IpiModeRemoteCall_t)call, arg0, arg1, arg2);
            भङ्ग;
        }

        big_kernel_lock.ग्रन्थिः[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_सङ्ख्या_of_supported_nodes, CONFIG_MAX_अङ्कः_NODES <= wordBits);

#ifdef CONFIG_USE_LOGICAL_IDS
स्थिर शून्य x86_ipi_send_mask(interrupt_t ipi, word_t mask, bool_t isBlocking)
{
    word_t nr_target_clusters = 0;
    word_t target_clusters[CONFIG_MAX_अङ्कः_NODES];

    करेहि {
        पूर्णाङ्क core = wordBits - 1 - clzl(mask);
        target_clusters[nr_target_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        word_t sub_mask = mask & cpu_mapping.other_indexes_in_cluster[core];
        target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[core];
        जइ (isBlocking) {
            big_kernel_lock.ग्रन्थिः[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        जाव (sub_mask) {
            पूर्णाङ्क index = wordBits - 1 - clzl(sub_mask);
            target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[index];
            जइ (isBlocking) {
                big_kernel_lock.ग्रन्थिः[index].ipi = 1;
            }
            sub_mask &= ~BIT(index);
        }

        mask &= ~(cpu_mapping.other_indexes_in_cluster[core] | BIT(core));
        nr_target_clusters++;
    } जाव (mask != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    प्रति (पूर्णाङ्क i = 0; i < nr_target_clusters; i++) {
        apic_send_ipi_cluster(ipi, target_clusters[i]);
    }
}
#endif /* CONFIG_USE_LOGICAL_IDS */

शून्य ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    interrupt_t interrupt_ipi = ipi + IRQ_INT_OFFSET;

#ifdef CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#अण्णहा
    generic_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#endif /* CONFIG_USE_LOGICAL_IDS */
}
#endif /* ENABLE_SMP_SUPPORT */
