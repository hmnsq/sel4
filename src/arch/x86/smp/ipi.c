/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/smp/ipi.h>
#ضمن <smp/ipi.h>
#ضمن <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT

فراغ handleRemoteCall(IpiRemoteCall_t call, word_t arg0, word_t arg1, word_t arg2, bool_t irqPath)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    𐡄𐡍 (clh_is_ipi_pending(getCurrentCPUIndex())) {
        بدل (call) {
        حالة IpiRemoteCall_Stall:
            ipiStallCoreCallback(irqPath);
            اكسر;

        حالة IpiRemoteCall_InvalidatePageStructureCacheASID:
            invalidateLocalPageStructureCacheASID(arg0, arg1);
            اكسر;

        حالة IpiRemoteCall_InvalidateTranslationSingle:
            invalidateLocalTranslationSingle(arg0);
            اكسر;

        حالة IpiRemoteCall_InvalidateTranslationSingleASID:
            invalidateLocalTranslationSingleASID(arg0, arg1);
            اكسر;

        حالة IpiRemoteCall_InvalidateTranslationAll:
            invalidateLocalTranslationAll();
            اكسر;

        حالة IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            اكسر;

#ifdef CONFIG_VTX
        حالة IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            اكسر;
        حالة IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            𐡄𐡍 (tcb != NODE_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            اكسر;
        }
#endif
        افتراضي:
            Mode_handleRemoteCall((IpiModeRemoteCall_t)call, arg0, arg1, arg2);
            اكسر;
        }

        big_kernel_lock.node[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_number_of_supported_nodes, CONFIG_MAX_NUM_NODES <= wordBits);

#ifdef CONFIG_USE_LOGICAL_IDS
ساكن فراغ x86_ipi_send_mask(interrupt_t ipi, word_t mask, bool_t isBlocking)
{
    word_t nr_target_clusters = 0;
    word_t target_clusters[CONFIG_MAX_NUM_NODES];

    𐡏𐡁𐡃 {
        صحيح core = wordBits - 1 - clzl(mask);
        target_clusters[nr_target_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        word_t sub_mask = mask & cpu_mapping.other_indexes_in_cluster[core];
        target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[core];
        𐡄𐡍 (isBlocking) {
            big_kernel_lock.node[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        طالما (sub_mask) {
            صحيح index = wordBits - 1 - clzl(sub_mask);
            target_clusters[nr_target_clusters] |= cpu_mapping.index_to_logical_id[index];
            𐡄𐡍 (isBlocking) {
                big_kernel_lock.node[index].ipi = 1;
            }
            sub_mask &= ~BIT(index);
        }

        mask &= ~(cpu_mapping.other_indexes_in_cluster[core] | BIT(core));
        nr_target_clusters++;
    } طالما (mask != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    لكل (صحيح i = 0; i < nr_target_clusters; i++) {
        apic_send_ipi_cluster(ipi, target_clusters[i]);
    }
}
#endif /* CONFIG_USE_LOGICAL_IDS */

فراغ ipi_send_mask(irq_t ipi, word_t mask, bool_t isBlocking)
{
    interrupt_t interrupt_ipi = ipi + IRQ_INT_OFFSET;

#ifdef CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#وإلا
    generic_ipi_send_mask(interrupt_ipi, mask, isBlocking);
#endif /* CONFIG_USE_LOGICAL_IDS */
}
#endif /* ENABLE_SMP_SUPPORT */
