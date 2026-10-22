/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/smp/ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <smp/ipi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <smp/lock.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT

ਖਾਲੀ handleRemoteCall(IpiRemoteCall_t call, ਸ਼ਬਦ_t arg0, ਸ਼ਬਦ_t arg1, ਸ਼ਬਦ_t arg2, bool_t irq_ਰਸਤਾ)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    ਜੇ (clh_is_ipi_pending(getCurrentCPUIndex())) {
        ਚੋਣ (call) {
        ਹਾਲਤ IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_ਰਸਤਾ);
            ਰੋਕੋ;

        ਹਾਲਤ Ipi_Remote_Call_Invalidate_ਪੰਨਾ_Structure_Cache_ASID:
            invalidate_Local_ਪੰਨਾ_Structure_Cache_ASID(arg0, arg1);
            ਰੋਕੋ;

        ਹਾਲਤ IpiRemoteCall_InvalidateTranslationSingle:
            invalidateLocalTranslationSingle(arg0);
            ਰੋਕੋ;

        ਹਾਲਤ IpiRemoteCall_InvalidateTranslationSingleASID:
            invalidateLocalTranslationSingleASID(arg0, arg1);
            ਰੋਕੋ;

        ਹਾਲਤ IpiRemoteCall_InvalidateTranslationAll:
            invalidateLocalTranslationAll();
            ਰੋਕੋ;

        ਹਾਲਤ IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
        ਹਾਲਤ IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            ਰੋਕੋ;
        ਹਾਲਤ IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            ਜੇ (tcb != ਗੰਢ_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            ਰੋਕੋ;
        }
#ਜੇ_ਅੰਤ
        ਮੂਲ:
            ਬਹੁਲਕ_handle_Remote_Call((Ipi_ਬਹੁਲਕ_Remote_Call_t)call, arg0, arg1, arg2);
            ਰੋਕੋ;
        }

        big_kernel_ਤਾਲਾ.ਗੰਢ[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_ਸੰਖਿਆ_ਦਾ_supported_nodes, CONFIG_MAX_ਅੰਕ_NODES <= ਸ਼ਬਦ_ਬਿੱਟ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_USE_LOGICAL_IDS
ਸਥਿਰ ਖਾਲੀ x86_ipi_send_ਨਕਾਬ(ਰੁਕਾਵਟ_t ipi, ਸ਼ਬਦ_t ਨਕਾਬ_2, bool_t isBlocking)
{
    ਸ਼ਬਦ_t nr_ਨਿਸ਼ਾਨਾ_clusters = 0;
    ਸ਼ਬਦ_t ਨਿਸ਼ਾਨਾ_clusters[CONFIG_MAX_ਅੰਕ_NODES];

    ਕਰੋ {
        ਪੂਰਨ core = ਸ਼ਬਦ_ਬਿੱਟ - 1 - clzl(ਨਕਾਬ_2);
        ਨਿਸ਼ਾਨਾ_clusters[nr_ਨਿਸ਼ਾਨਾ_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        ਸ਼ਬਦ_t sub_ਨਕਾਬ = ਨਕਾਬ_2 & cpu_mapping.other_indexes_ਵਿੱਚ_cluster[core];
        ਨਿਸ਼ਾਨਾ_clusters[nr_ਨਿਸ਼ਾਨਾ_clusters] |= cpu_mapping.ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_logical_id[core];
        ਜੇ (isBlocking) {
            big_kernel_ਤਾਲਾ.ਗੰਢ[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        ਜਦੋਂ (sub_ਨਕਾਬ) {
            ਪੂਰਨ ਸੂਚਕਾਂਕ = ਸ਼ਬਦ_ਬਿੱਟ - 1 - clzl(sub_ਨਕਾਬ);
            ਨਿਸ਼ਾਨਾ_clusters[nr_ਨਿਸ਼ਾਨਾ_clusters] |= cpu_mapping.ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_logical_id[ਸੂਚਕਾਂਕ];
            ਜੇ (isBlocking) {
                big_kernel_ਤਾਲਾ.ਗੰਢ[ਸੂਚਕਾਂਕ].ipi = 1;
            }
            sub_ਨਕਾਬ &= ~ਦੋਆਧਾਰੀ_ਅੰਕ(ਸੂਚਕਾਂਕ);
        }

        ਨਕਾਬ_2 &= ~(cpu_mapping.other_indexes_ਵਿੱਚ_cluster[core] | ਦੋਆਧਾਰੀ_ਅੰਕ(core));
        nr_ਨਿਸ਼ਾਨਾ_clusters++;
    } ਜਦੋਂ (ਨਕਾਬ_2 != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    ਲਈ (ਪੂਰਨ i = 0; i < nr_ਨਿਸ਼ਾਨਾ_clusters; i++) {
        apic_send_ipi_cluster(ipi, ਨਿਸ਼ਾਨਾ_clusters[i]);
    }
}
#ਜੇ_ਅੰਤ /* CONFIG_USE_LOGICAL_IDS */

ਖਾਲੀ ipi_send_ਨਕਾਬ(irq_t ipi, ਸ਼ਬਦ_t ਨਕਾਬ_2, bool_t isBlocking)
{
    ਰੁਕਾਵਟ_t ਰੁਕਾਵਟ_ipi = ipi + IRQ_INT_ਵਿੱਥ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_ਨਕਾਬ(ਰੁਕਾਵਟ_ipi, ਨਕਾਬ_2, isBlocking);
#ਨਹੀਂਤਾਂ
    generic_ipi_send_ਨਕਾਬ(ਰੁਕਾਵਟ_ipi, ਨਕਾਬ_2, isBlocking);
#ਜੇ_ਅੰਤ /* CONFIG_USE_LOGICAL_IDS */
}
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
