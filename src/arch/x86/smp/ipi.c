/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/smp/ipi.h>
#ضمن <smp/ipi.h>
#ضمن <smp/lock.h>

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT

فراغ handleRemoteCall(IpiRemoteCall_t call, لفظ_t arg0, لفظ_t arg1, لفظ_t arg2, bool_t irq_رستو)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    إذا (clh_is_ipi_pending(getCurrentCPUIndex())) {
        بدل (call) {
        حالة IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_رستو);
            اكسر;

        حالة Ipi_Remote_Call_Invalidate_صفحو_Structure_Cache_ASID:
            invalidate_Local_صفحو_Structure_Cache_ASID(arg0, arg1);
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

#جيڪڏهن_وصف_ٿيل CONFIG_VTX
        حالة IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            اكسر;
        حالة IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            إذا (tcb != ڳنڍ_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            اكسر;
        }
#جيڪڏهن_پورو
        افتراضي:
            گهڻائي_handle_Remote_Call((Ipi_گهڻائي_Remote_Call_t)call, arg0, arg1, arg2);
            اكسر;
        }

        big_kernel_تالو.ڳنڍ[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_عدد_جو_supported_nodes, CONFIG_MAX_انگ_NODES <= لفظ_بٽ);

#جيڪڏهن_وصف_ٿيل CONFIG_USE_LOGICAL_IDS
ساكن فراغ x86_ipi_send_نقاب(مداخلت_t ipi, لفظ_t نقاب_2, bool_t isBlocking)
{
    لفظ_t nr_هدف_clusters = 0;
    لفظ_t هدف_clusters[CONFIG_MAX_انگ_NODES];

    افعل {
        صحيح core = لفظ_بٽ - 1 - clzl(نقاب_2);
        هدف_clusters[nr_هدف_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        لفظ_t sub_نقاب = نقاب_2 & cpu_mapping.other_indexes_اندريون_cluster[core];
        هدف_clusters[nr_هدف_clusters] |= cpu_mapping.اشاريو_منزل_logical_id[core];
        إذا (isBlocking) {
            big_kernel_تالو.ڳنڍ[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        طالما (sub_نقاب) {
            صحيح اشاريو = لفظ_بٽ - 1 - clzl(sub_نقاب);
            هدف_clusters[nr_هدف_clusters] |= cpu_mapping.اشاريو_منزل_logical_id[اشاريو];
            إذا (isBlocking) {
                big_kernel_تالو.ڳنڍ[اشاريو].ipi = 1;
            }
            sub_نقاب &= ~ٻٽو_انگ(اشاريو);
        }

        نقاب_2 &= ~(cpu_mapping.other_indexes_اندريون_cluster[core] | ٻٽو_انگ(core));
        nr_هدف_clusters++;
    } طالما (نقاب_2 != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    لكل (صحيح i = 0; i < nr_هدف_clusters; i++) {
        apic_send_ipi_cluster(ipi, هدف_clusters[i]);
    }
}
#جيڪڏهن_پورو /* CONFIG_USE_LOGICAL_IDS */

فراغ ipi_send_نقاب(irq_t ipi, لفظ_t نقاب_2, bool_t isBlocking)
{
    مداخلت_t مداخلت_ipi = ipi + IRQ_INT_وٿي;

#جيڪڏهن_وصف_ٿيل CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_نقاب(مداخلت_ipi, نقاب_2, isBlocking);
#وإلا
    generic_ipi_send_نقاب(مداخلت_ipi, نقاب_2, isBlocking);
#جيڪڏهن_پورو /* CONFIG_USE_LOGICAL_IDS */
}
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */
