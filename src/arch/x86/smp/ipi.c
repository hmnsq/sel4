/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <mode/smp/ipi.h>
#समावेश <smp/ipi.h>
#समावेश <smp/lock.h>

#अगर_घोषित ENABLE_SMP_SUPPORT

व्योम handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    अगर (clh_is_ipi_pending(getCurrentCPUIndex())) {
        चयन (call) {
        हाल IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            अवरोध;

        हाल IpiRemoteCall_InvalidatePageStructureCacheASID:
            invalidateLocalPageStructureCacheASID(arg0, arg1);
            अवरोध;

        हाल IpiRemoteCall_InvalidateTranslationSingle:
            invalidateLocalTranslationSingle(arg0);
            अवरोध;

        हाल IpiRemoteCall_InvalidateTranslationSingleASID:
            invalidateLocalTranslationSingleASID(arg0, arg1);
            अवरोध;

        हाल IpiRemoteCall_InvalidateTranslationAll:
            invalidateLocalTranslationAll();
            अवरोध;

        हाल IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            अवरोध;

#अगर_घोषित CONFIG_VTX
        हाल IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            अवरोध;
        हाल IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            अगर (tcb != गाँठ_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            अवरोध;
        }
#पूर्ण_अगर
        शेष:
            Mode_handleRemoteCall((IpiModeRemoteCall_t)call, arg0, arg1, arg2);
            अवरोध;
        }

        big_kernel_lock.गाँठ[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_संख्या_of_supported_nodes, CONFIG_MAX_अंक_NODES <= wordBits);

#अगर_घोषित CONFIG_USE_LOGICAL_IDS
अटल व्योम x86_ipi_send_mask(interrupt_t ipi, word_t mask, bool_t isBlocking)
{
    word_t nr_target_clusters = 0;
    word_t target_clusters[CONFIG_MAX_अंक_NODES];

    करो {
        पूर्णांक core = wordBits - 1 - clzl(mask);
        target_clusters[nr_target_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        word_t sub_mask = mask & cpu_mapping.other_indexes_in_cluster[core];
        target_clusters[nr_target_clusters] |= cpu_mapping.सूचकांक_to_logical_id[core];
        अगर (isBlocking) {
            big_kernel_lock.गाँठ[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        जबतक (sub_mask) {
            पूर्णांक सूचकांक = wordBits - 1 - clzl(sub_mask);
            target_clusters[nr_target_clusters] |= cpu_mapping.सूचकांक_to_logical_id[सूचकांक];
            अगर (isBlocking) {
                big_kernel_lock.गाँठ[सूचकांक].ipi = 1;
            }
            sub_mask &= ~BIT(सूचकांक);
        }

        mask &= ~(cpu_mapping.other_indexes_in_cluster[core] | BIT(core));
        nr_target_clusters++;
    } जबतक (mask != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    क्रम (पूर्णांक i = 0; i < nr_target_clusters; i++) {
        apic_send_ipi_cluster(ipi, target_clusters[i]);
    }
}
#पूर्ण_अगर /* CONFIG_USE_LOGICAL_IDS */

व्योम ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    interrupt_t interrupt_ipi = ipi + IRQ_INT_OFFSET;

#अगर_घोषित CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#अन्यथा
    generic_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#पूर्ण_अगर /* CONFIG_USE_LOGICAL_IDS */
}
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */
