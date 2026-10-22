/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <mode/smp/ipi.h>
#അന്തര്ഭാവ <smp/ipi.h>
#അന്തര്ഭാവ <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT

ശൂന്യം handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    എങ്കിൽ (clh_is_ipi_pending(getCurrentCPUIndex())) {
        തിരഞ്ഞെടുപ്പ് (call) {
        അവസ്ഥ IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            നിർത്തുക;

        അവസ്ഥ IpiRemoteCall_InvalidatePageStructureCacheASID:
            invalidateLocalPageStructureCacheASID(arg0, arg1);
            നിർത്തുക;

        അവസ്ഥ IpiRemoteCall_InvalidateTranslationSingle:
            invalidateLocalTranslationSingle(arg0);
            നിർത്തുക;

        അവസ്ഥ IpiRemoteCall_InvalidateTranslationSingleASID:
            invalidateLocalTranslationSingleASID(arg0, arg1);
            നിർത്തുക;

        അവസ്ഥ IpiRemoteCall_InvalidateTranslationAll:
            invalidateLocalTranslationAll();
            നിർത്തുക;

        അവസ്ഥ IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            നിർത്തുക;

#ifdef CONFIG_VTX
        അവസ്ഥ IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            നിർത്തുക;
        അവസ്ഥ IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            എങ്കിൽ (tcb != കണ്ണി_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            നിർത്തുക;
        }
#endif
        സ്ഥിരസ്ഥിതി:
            Mode_handleRemoteCall((IpiModeRemoteCall_t)call, arg0, arg1, arg2);
            നിർത്തുക;
        }

        big_kernel_lock.കണ്ണി[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_സംഖ്യ_of_supported_nodes, CONFIG_MAX_അക്കം_NODES <= wordBits);

#ifdef CONFIG_USE_LOGICAL_IDS
സ്ഥിതം ശൂന്യം x86_ipi_send_mask(interrupt_t ipi, word_t mask, bool_t isBlocking)
{
    word_t nr_target_clusters = 0;
    word_t target_clusters[CONFIG_MAX_അക്കം_NODES];

    ചെയ്യുക {
        പൂർണ്ണസംഖ്യ core = wordBits - 1 - clzl(mask);
        target_clusters[nr_target_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        word_t sub_mask = mask & cpu_mapping.other_indexes_in_cluster[core];
        target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[core];
        എങ്കിൽ (isBlocking) {
            big_kernel_lock.കണ്ണി[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        ആകുമ്പോൾ (sub_mask) {
            പൂർണ്ണസംഖ്യ index = wordBits - 1 - clzl(sub_mask);
            target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[index];
            എങ്കിൽ (isBlocking) {
                big_kernel_lock.കണ്ണി[index].ipi = 1;
            }
            sub_mask &= ~BIT(index);
        }

        mask &= ~(cpu_mapping.other_indexes_in_cluster[core] | BIT(core));
        nr_target_clusters++;
    } ആകുമ്പോൾ (mask != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < nr_target_clusters; i++) {
        apic_send_ipi_cluster(ipi, target_clusters[i]);
    }
}
#endif /* CONFIG_USE_LOGICAL_IDS */

ശൂന്യം ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    interrupt_t interrupt_ipi = ipi + IRQ_INT_OFFSET;

#ifdef CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#അല്ലെങ്കിൽ
    generic_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#endif /* CONFIG_USE_LOGICAL_IDS */
}
#endif /* ENABLE_SMP_SUPPORT */
