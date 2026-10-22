/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <mode/smp/ipi.h>
#ضمن <smp/ipi.h>
#ضمن <smp/lock.h>

#که_تعریف_شوی ENABLE_SMP_SUPPORT

فراغ handleRemoteCall(IpiRemoteCall_t call, کلمه_t arg0, کلمه_t arg1, کلمه_t arg2, bool_t irq_لار)
{
    /* we gets spurious irq_remote_call_ipi calls, e.g. when handling IPI
     * in lock while hardware IPI is pending. Guard against spurious IPIs! */
    إذا (clh_is_ipi_pending(getCurrentCPUIndex())) {
        بدل (call) {
        حالة IpiRemoteCall_Stall:
            ipiStallCoreCallback(irq_لار);
            اكسر;

        حالة Ipi_Remote_Call_Invalidate_پاڼه_Structure_Cache_ASID:
            invalidate_Local_پاڼه_Structure_Cache_ASID(arg0, arg1);
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

#که_تعریف_شوی CONFIG_VTX
        حالة IpiRemoteCall_ClearCurrentVCPU:
            clearCurrentVCPU();
            اكسر;
        حالة IpiRemoteCall_VMCheckBoundNotification: {
            tcb_t *tcb = (tcb_t *)arg0;

            /* For a running VM notifications are already checked by handleVmexit */
            إذا (tcb != غوټه_STATE(ksCurThread)) {
                VMCheckBoundNotification(tcb);
            }
            اكسر;
        }
#د_که_پای
        افتراضي:
            ډېرپېښ_handle_Remote_Call((Ipi_ډېرپېښ_Remote_Call_t)call, arg0, arg1, arg2);
            اكسر;
        }

        big_kernel_قلف.غوټه[getCurrentCPUIndex()].ipi = 0;
        ipi_wait();
    }
}

/* make sure all cpu IDs for number of core fit in bitwise word */
compile_assert(invalid_شمېره_د_supported_nodes, CONFIG_MAX_عدد_NODES <= کلمه_بټونه);

#که_تعریف_شوی CONFIG_USE_LOGICAL_IDS
ساكن فراغ x86_ipi_send_ماسک(مداخله_t ipi, کلمه_t ماسک_2, bool_t isBlocking)
{
    کلمه_t nr_موخه_clusters = 0;
    کلمه_t موخه_clusters[CONFIG_MAX_عدد_NODES];

    افعل {
        صحيح core = کلمه_بټونه - 1 - clzl(ماسک_2);
        موخه_clusters[nr_موخه_clusters] = 0;

        /* get mask of all cores in bitmask which are in same cluster as 'core' */
        کلمه_t sub_ماسک = ماسک_2 & cpu_mapping.other_indexes_دننه_cluster[core];
        موخه_clusters[nr_موخه_clusters] |= cpu_mapping.شاخص_منزل_logical_id[core];
        إذا (isBlocking) {
            big_kernel_قلف.غوټه[core].ipi = 1;
        }

        /* check if there is any other core in this cluster */
        طالما (sub_ماسک) {
            صحيح شاخص = کلمه_بټونه - 1 - clzl(sub_ماسک);
            موخه_clusters[nr_موخه_clusters] |= cpu_mapping.شاخص_منزل_logical_id[شاخص];
            إذا (isBlocking) {
                big_kernel_قلف.غوټه[شاخص].ipi = 1;
            }
            sub_ماسک &= ~بټ(شاخص);
        }

        ماسک_2 &= ~(cpu_mapping.other_indexes_دننه_cluster[core] | بټ(core));
        nr_موخه_clusters++;
    } طالما (ماسک_2 != 0);

    /* broadcast IPIs to clusters... */
    IPI_ICR_BARRIER;
    لكل (صحيح i = 0; i < nr_موخه_clusters; i++) {
        apic_send_ipi_cluster(ipi, موخه_clusters[i]);
    }
}
#د_که_پای /* CONFIG_USE_LOGICAL_IDS */

فراغ ipi_send_ماسک(irq_t ipi, کلمه_t ماسک_2, bool_t isBlocking)
{
    مداخله_t مداخله_ipi = ipi + IRQ_INT_دننه_واټن;

#که_تعریف_شوی CONFIG_USE_LOGICAL_IDS
    x86_ipi_send_ماسک(مداخله_ipi, ماسک_2, isBlocking);
#وإلا
    generic_ipi_send_ماسک(مداخله_ipi, ماسک_2, isBlocking);
#د_که_پای /* CONFIG_USE_LOGICAL_IDS */
}
#د_که_پای /* ENABLE_SMP_SUPPORT */
