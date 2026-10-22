/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/smp/ipi.h>
#ضمن <smp/ipi.h>
#ضمن <smp/lock.h>

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT

فراغ handleRemoteCall(IpiRemoteCall_t call, لفظ_t arg0, لفظ_t arg1, لفظ_t arg2, bool_t irq_رستہ)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    إذا (clh_is_ipi_pending(getCurrentCPUIndex())) {
        بدل (call) {
        حالة IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_رستہ);
            اكسر;

        حالة Ipi_Remote_Call_Invalidate_ورقہ_Structure_Cache_ASID:
            invalidate_Local_ورقہ_Structure_Cache_ASID(arg0, arg1);
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

#جے_تعریف_شدہ CONFIG_VTX
        حالة IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            اكسر;
        حالة IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            إذا (tcb != گرہ_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            اكسر;
        }
#جے_انت
        افتراضي:
            منوال_handle_Remote_Call((Ipi_منوال_Remote_Call_t)call, arg0, arg1, arg2);
            اكسر;
        }

        big_kernel_جندرا.گرہ[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_نمبر_دا_supported_nodes, CONFIG_MAX_عدد_NODES <= لفظ_بٹ);

#جے_تعریف_شدہ CONFIG_USE_LOGICAL_IDS
ساكن فراغ x86_ipi_send_نقاب(رکاوٹ_t ipi, لفظ_t نقاب_2, bool_t isBlocking)
{
    لفظ_t nr_نشانہ_clusters = 0;
    لفظ_t نشانہ_clusters[CONFIG_MAX_عدد_NODES];

    افعل {
        صحيح core = لفظ_بٹ - 1 - clzl(نقاب_2);
        نشانہ_clusters[nr_نشانہ_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        لفظ_t sub_نقاب = نقاب_2 & cpu_mapping.other_indexes_وچ_cluster[core];
        نشانہ_clusters[nr_نشانہ_clusters] |= cpu_mapping.اشاریہ_منزل_logical_id[core];
        إذا (isBlocking) {
            big_kernel_جندرا.گرہ[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        طالما (sub_نقاب) {
            صحيح اشاریہ = لفظ_بٹ - 1 - clzl(sub_نقاب);
            نشانہ_clusters[nr_نشانہ_clusters] |= cpu_mapping.اشاریہ_منزل_logical_id[اشاریہ];
            إذا (isBlocking) {
                big_kernel_جندرا.گرہ[اشاریہ].ipi = 1;
            }
            sub_نقاب &= ~ثنائی_ہندسہ(اشاریہ);
        }

        نقاب_2 &= ~(cpu_mapping.other_indexes_وچ_cluster[core] | ثنائی_ہندسہ(core));
        nr_نشانہ_clusters++;
    } طالما (نقاب_2 != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    لكل (صحيح i = 0; i < nr_نشانہ_clusters; i++) {
        apic_send_ipi_cluster(ipi, نشانہ_clusters[i]);
    }
}
#جے_انت /* CONFIG_USE_LOGICAL_IDS */

فراغ ipi_send_نقاب(irq_t ipi, لفظ_t نقاب_2, bool_t isBlocking)
{
    رکاوٹ_t رکاوٹ_ipi = ipi + IRQ_INT_وتھ;

#جے_تعریف_شدہ CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_نقاب(رکاوٹ_ipi, نقاب_2, isBlocking);
#وإلا
    generic_ipi_send_نقاب(رکاوٹ_ipi, نقاب_2, isBlocking);
#جے_انت /* CONFIG_USE_LOGICAL_IDS */
}
#جے_انت /* ENABLE_SMP_SUPPORT */
