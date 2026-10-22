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
    जर (clh_is_ipi_pending(getCurrentCPUIndex())) {
        निवड (call) {
        स्थिती IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            थांबा;

        स्थिती IpiRemoteCall_InvalidatePageStructureCacheASID:
            invalidateLocalPageStructureCacheASID(arg0, arg1);
            थांबा;

        स्थिती IpiRemoteCall_InvalidateTranslationSingle:
            invalidateLocalTranslationSingle(arg0);
            थांबा;

        स्थिती IpiRemoteCall_InvalidateTranslationSingleASID:
            invalidateLocalTranslationSingleASID(arg0, arg1);
            थांबा;

        स्थिती IpiRemoteCall_InvalidateTranslationAll:
            invalidateLocalTranslationAll();
            थांबा;

        स्थिती IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            थांबा;

#ifdef CONFIG_VTX
        स्थिती IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            थांबा;
        स्थिती IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            जर (tcb != गाठ_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            थांबा;
        }
#endif
        पूर्वनिर्धारित:
            Mode_handleRemoteCall((IpiModeRemoteCall_t)call, arg0, arg1, arg2);
            थांबा;
        }

        big_kernel_lock.गाठ[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_संख्या_of_supported_nodes, CONFIG_MAX_अंक_NODES <= wordBits);

#ifdef CONFIG_USE_LOGICAL_IDS
स्थिर शून्य x86_ipi_send_mask(interrupt_t ipi, word_t mask, bool_t isBlocking)
{
    word_t nr_target_clusters = 0;
    word_t target_clusters[CONFIG_MAX_अंक_NODES];

    करा {
        पूर्णांक core = wordBits - 1 - clzl(mask);
        target_clusters[nr_target_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        word_t sub_mask = mask & cpu_mapping.other_indexes_in_cluster[core];
        target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[core];
        जर (isBlocking) {
            big_kernel_lock.गाठ[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        पर्यंत (sub_mask) {
            पूर्णांक index = wordBits - 1 - clzl(sub_mask);
            target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[index];
            जर (isBlocking) {
                big_kernel_lock.गाठ[index].ipi = 1;
            }
            sub_mask &= ~BIT(index);
        }

        mask &= ~(cpu_mapping.other_indexes_in_cluster[core] | BIT(core));
        nr_target_clusters++;
    } पर्यंत (mask != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    साठी (पूर्णांक i = 0; i < nr_target_clusters; i++) {
        apic_send_ipi_cluster(ipi, target_clusters[i]);
    }
}
#endif /* CONFIG_USE_LOGICAL_IDS */

शून्य ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    interrupt_t interrupt_ipi = ipi + IRQ_INT_OFFSET;

#ifdef CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#अन्यथा
    generic_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#endif /* CONFIG_USE_LOGICAL_IDS */
}
#endif /* ENABLE_SMP_SUPPORT */
