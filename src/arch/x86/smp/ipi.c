/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <mode/smp/ipi.h>
#ಅನ್ತರ್ಭಾವ <smp/ipi.h>
#ಅನ್ತರ್ಭಾವ <smp/lock.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT

ಶೂನ್ಯ handleRemoteCall(IpiRemoteCall_t call, ಶಬ್ದ_t arg0, ಶಬ್ದ_t arg1, ಶಬ್ದ_t arg2, bool_t irq_ಮಾರ್ಗ)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    ಆದರೆ (clh_is_ipi_pending(getCurrentCPUIndex())) {
        ಆಯ್ಕೆ (call) {
        ಸ್ಥಿತಿ IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_ಮಾರ್ಗ);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ Ipi_Remote_Call_Invalidate_ಪುಟ_Structure_Cache_ASID:
            invalidate_Local_ಪುಟ_Structure_Cache_ASID(arg0, arg1);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ IpiRemoteCall_InvalidateTranslationSingle:
            invalidateLocalTranslationSingle(arg0);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ IpiRemoteCall_InvalidateTranslationSingleASID:
            invalidateLocalTranslationSingleASID(arg0, arg1);
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ IpiRemoteCall_InvalidateTranslationAll:
            invalidateLocalTranslationAll();
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
        ಸ್ಥಿತಿ IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            ಆದರೆ (tcb != ಗಂಟು_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            ನಿಲ್ಲಿಸು;
        }
#ಆದರೆ_ಅಂತ್ಯ
        ಪೂರ್ವನಿಯೋಜಿತ:
            ಬಹುಲಕ_handle_Remote_Call((Ipi_ಬಹುಲಕ_Remote_Call_t)call, arg0, arg1, arg2);
            ನಿಲ್ಲಿಸು;
        }

        big_kernel_ಬೀಗ.ಗಂಟು[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_ಸಂಖ್ಯೆ_ದ_supported_nodes, CONFIG_MAX_ಅಂಕೆ_NODES <= ಶಬ್ದ_ಬಿಟ್ಟುಗಳು);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_USE_LOGICAL_IDS
ಸ್ಥಿರ ಶೂನ್ಯ x86_ipi_send_ಮುಖವಾಡ(ಅಡಚಣೆ_t ipi, ಶಬ್ದ_t ಮುಖವಾಡ_2, bool_t isBlocking)
{
    ಶಬ್ದ_t nr_ಗುರಿ_clusters = 0;
    ಶಬ್ದ_t ಗುರಿ_clusters[CONFIG_MAX_ಅಂಕೆ_NODES];

    ಮಾಡು {
        ಪೂರ್ಣಾಂಕ core = ಶಬ್ದ_ಬಿಟ್ಟುಗಳು - 1 - clzl(ಮುಖವಾಡ_2);
        ಗುರಿ_clusters[nr_ಗುರಿ_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        ಶಬ್ದ_t sub_ಮುಖವಾಡ = ಮುಖವಾಡ_2 & cpu_mapping.other_indexes_ಒಳ_cluster[core];
        ಗುರಿ_clusters[nr_ಗುರಿ_clusters] |= cpu_mapping.ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_logical_id[core];
        ಆದರೆ (isBlocking) {
            big_kernel_ಬೀಗ.ಗಂಟು[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        ಆಗ (sub_ಮುಖವಾಡ) {
            ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = ಶಬ್ದ_ಬಿಟ್ಟುಗಳು - 1 - clzl(sub_ಮುಖವಾಡ);
            ಗುರಿ_clusters[nr_ಗುರಿ_clusters] |= cpu_mapping.ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_logical_id[ಸೂಚ್ಯಂಕ];
            ಆದರೆ (isBlocking) {
                big_kernel_ಬೀಗ.ಗಂಟು[ಸೂಚ್ಯಂಕ].ipi = 1;
            }
            sub_ಮುಖವಾಡ &= ~ಬಿಟ್(ಸೂಚ್ಯಂಕ);
        }

        ಮುಖವಾಡ_2 &= ~(cpu_mapping.other_indexes_ಒಳ_cluster[core] | ಬಿಟ್(core));
        nr_ಗುರಿ_clusters++;
    } ಆಗ (ಮುಖವಾಡ_2 != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < nr_ಗುರಿ_clusters; i++) {
        apic_send_ipi_cluster(ipi, ಗುರಿ_clusters[i]);
    }
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_USE_LOGICAL_IDS */

ಶೂನ್ಯ ipi_send_ಮುಖವಾಡ(irq_t ipi, ಶಬ್ದ_t ಮುಖವಾಡ_2, bool_t isBlocking)
{
    ಅಡಚಣೆ_t ಅಡಚಣೆ_ipi = ipi + IRQ_INT_ಸ್ಥಾನಾಂತರ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_ಮುಖವಾಡ(ಅಡಚಣೆ_ipi, ಮುಖವಾಡ_2, isBlocking);
#ಇಲ್ಲದಿದ್ದರೆ
    generic_ipi_send_ಮುಖವಾಡ(ಅಡಚಣೆ_ipi, ಮುಖವಾಡ_2, isBlocking);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_USE_LOGICAL_IDS */
}
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
