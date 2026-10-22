/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <mode/smp/ipi.h>
#شامل <smp/ipi.h>
#شامل <smp/lock.h>

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT

خالی handleRemoteCall(IpiRemoteCall_t call, لفظ_t arg0, لفظ_t arg1, لفظ_t arg2, bool_t irq_راستہ)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    اگر (clh_is_ipi_pending(getCurrentCPUIndex())) {
        انتخاب (call) {
        حالت IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_راستہ);
            توڑو;

        حالت Ipi_Remote_Call_Invalidate_صفحہ_Structure_Cache_ASID:
            invalidate_Local_صفحہ_Structure_Cache_ASID(arg0, arg1);
            توڑو;

        حالت IpiRemoteCall_InvalidateTranslationSingle:
            invalidateLocalTranslationSingle(arg0);
            توڑو;

        حالت IpiRemoteCall_InvalidateTranslationSingleASID:
            invalidateLocalTranslationSingleASID(arg0, arg1);
            توڑو;

        حالت IpiRemoteCall_InvalidateTranslationAll:
            invalidateLocalTranslationAll();
            توڑو;

        حالت IpiRemoteCall_switchFpuOwner:
            switchLocalFpuOwner((tcb_t *)arg0);
            توڑو;

#اگر_تعریف_شدہ CONFIG_VTX
        حالت IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            توڑو;
        حالت IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            اگر (tcb != گرہ_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            توڑو;
        }
#اگر_ختم
        طےشدہ:
            منوال_handle_Remote_Call((Ipi_منوال_Remote_Call_t)call, arg0, arg1, arg2);
            توڑو;
        }

        big_kernel_تالا.گرہ[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_نمبر_کا_supported_nodes, CONFIG_MAX_عدد_NODES <= لفظ_بٹ);

#اگر_تعریف_شدہ CONFIG_USE_LOGICAL_IDS
ساکن خالی x86_ipi_send_نقاب(مداخلت_t ipi, لفظ_t نقاب_2, bool_t isBlocking)
{
    لفظ_t nr_ہدف_clusters = 0;
    لفظ_t ہدف_clusters[CONFIG_MAX_عدد_NODES];

    کرو {
        صحیح core = لفظ_بٹ - 1 - clzl(نقاب_2);
        ہدف_clusters[nr_ہدف_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        لفظ_t sub_نقاب = نقاب_2 & cpu_mapping.other_indexes_اندر_cluster[core];
        ہدف_clusters[nr_ہدف_clusters] |= cpu_mapping.اشاریہ_مقصد_logical_id[core];
        اگر (isBlocking) {
            big_kernel_تالا.گرہ[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        جب (sub_نقاب) {
            صحیح اشاریہ = لفظ_بٹ - 1 - clzl(sub_نقاب);
            ہدف_clusters[nr_ہدف_clusters] |= cpu_mapping.اشاریہ_مقصد_logical_id[اشاریہ];
            اگر (isBlocking) {
                big_kernel_تالا.گرہ[اشاریہ].ipi = 1;
            }
            sub_نقاب &= ~ثنائی_ہندسہ(اشاریہ);
        }

        نقاب_2 &= ~(cpu_mapping.other_indexes_اندر_cluster[core] | ثنائی_ہندسہ(core));
        nr_ہدف_clusters++;
    } جب (نقاب_2 != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    برائے (صحیح i = 0; i < nr_ہدف_clusters; i++) {
        apic_send_ipi_cluster(ipi, ہدف_clusters[i]);
    }
}
#اگر_ختم /* CONFIG_USE_LOGICAL_IDS */

خالی ipi_send_نقاب(irq_t ipi, لفظ_t نقاب_2, bool_t isBlocking)
{
    مداخلت_t مداخلت_ipi = ipi + IRQ_INT_انحراف;

#اگر_تعریف_شدہ CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_نقاب(مداخلت_ipi, نقاب_2, isBlocking);
#ورنہ
    generic_ipi_send_نقاب(مداخلت_ipi, نقاب_2, isBlocking);
#اگر_ختم /* CONFIG_USE_LOGICAL_IDS */
}
#اگر_ختم /* ENABLE_SMP_SUPPORT */
