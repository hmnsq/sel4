/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <mode/smp/ipi.h>
#अन्तर्भाव <smp/ipi.h>
#अन्तर्भाव <smp/lock.h>

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT

शून्य handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irq_मार्गः)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    यदि (clh_is_ipi_pending(getCurrentCPUIndex())) {
        विकल्प (call) {
        पक्ष IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_मार्गः);
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

#यदि_परिभाषितम् CONFIG_VTX
        पक्ष IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            भङ्ग;
        पक्ष IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            यदि (tcb != ग्रन्थिः_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            भङ्ग;
        }
#यदि_अन्तः
        स्वभाव:
            बहुलकम्_handle_Remote_Call((Ipi_बहुलकम्_Remote_Call_t)call, arg0, arg1, arg2);
            भङ्ग;
        }

        big_kernel_तालकम्.ग्रन्थिः[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_सङ्ख्या_सम्बन्धि_supported_nodes, CONFIG_MAX_अङ्कः_NODES <= word_द्वयङ्काः);

#यदि_परिभाषितम् CONFIG_USE_LOGICAL_IDS
स्थिर शून्य x86_ipi_send_आवरणम्(व्यवधानम्_t ipi, word_t आवरणम्_2, bool_t isBlocking)
{
    word_t nr_target_clusters = 0;
    word_t target_clusters[CONFIG_MAX_अङ्कः_NODES];

    कुरु {
        पूर्णाङ्क core = word_द्वयङ्काः - 1 - clzl(आवरणम्_2);
        target_clusters[nr_target_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        word_t sub_आवरणम् = आवरणम्_2 & cpu_mapping.other_indexes_अन्तर्_cluster[core];
        target_clusters[nr_target_clusters] |= cpu_mapping.सूचकांक_गन्तव्यम्_logical_id[core];
        यदि (isBlocking) {
            big_kernel_तालकम्.ग्रन्थिः[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        यावत् (sub_आवरणम्) {
            पूर्णाङ्क सूचकांक = word_द्वयङ्काः - 1 - clzl(sub_आवरणम्);
            target_clusters[nr_target_clusters] |= cpu_mapping.सूचकांक_गन्तव्यम्_logical_id[सूचकांक];
            यदि (isBlocking) {
                big_kernel_तालकम्.ग्रन्थिः[सूचकांक].ipi = 1;
            }
            sub_आवरणम् &= ~BIT(सूचकांक);
        }

        आवरणम्_2 &= ~(cpu_mapping.other_indexes_अन्तर्_cluster[core] | BIT(core));
        nr_target_clusters++;
    } यावत् (आवरणम्_2 != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    प्रति (पूर्णाङ्क i = 0; i < nr_target_clusters; i++) {
        apic_send_ipi_cluster(ipi, target_clusters[i]);
    }
}
#यदि_अन्तः /* CONFIG_USE_LOGICAL_IDS */

शून्य ipi_send_आवरणम्(irq_t ipi, word_t आवरणम्_2, bool_t isBlocking)
{
    व्यवधानम्_t व्यवधानम्_ipi = ipi + IRQ_INT_विस्थापनम्;

#यदि_परिभाषितम् CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_आवरणम्(व्यवधानम्_ipi, आवरणम्_2, isBlocking);
#अन्यथा
    generic_ipi_send_आवरणम्(व्यवधानम्_ipi, आवरणम्_2, isBlocking);
#यदि_अन्तः /* CONFIG_USE_LOGICAL_IDS */
}
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */
