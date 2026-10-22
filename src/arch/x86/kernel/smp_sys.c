/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <arch/machine.h>
#شامل <arch/machine/timer.h>
#شامل <arch/kernel/boot_sys.h>
#شامل <arch/kernel/smp_sys.h>
#شامل <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT

/* Index of next AP to boot, BSP has index zero */
BOOT_DATA VISIBLE
فرار word_t smp_aps_index = 1;

#ifdef CONFIG_USE_LOGICAL_IDS
BOOT_CODE ایستا تهی update_logical_id_mappings(تهی)
{
    cpu_mapping.index_to_logical_id[getCurrentCPUIndex()] = apic_get_logical_id();

    برای (صحیح i = 0; i < smp_aps_index; i++) {
        اگر (apic_get_cluster(cpu_mapping.index_to_logical_id[getCurrentCPUIndex()]) ==
            apic_get_cluster(cpu_mapping.index_to_logical_id[i])) {

            cpu_mapping.other_indexes_in_cluster[getCurrentCPUIndex()] |= BIT(i);
            cpu_mapping.other_indexes_in_cluster[i] |= BIT(getCurrentCPUIndex());
        }
    }
}
#endif /* CONFIG_USE_LOGICAL_IDS */

BOOT_CODE ایستا تهی start_cpu(cpu_id_t cpu_id, paddr_t boot_fun_paddr)
{
    /* memory fence needed before starting the other CPU */
    x86_mfence();

    /* starting the other CPU */
    apic_send_init_ipi(cpu_id);
    apic_send_startup_ipi(cpu_id, boot_fun_paddr);
}

BOOT_CODE تهی start_boot_aps(تهی)
{
    /* update cpu mapping for BSP, cpus[0] is always assumed to be BSP */
    cpu_mapping.index_to_cpu_id[getCurrentCPUIndex()] = boot_state.cpus[0];
#ifdef CONFIG_USE_LOGICAL_IDS
    cpu_mapping.index_to_logical_id[getCurrentCPUIndex()] = apic_get_logical_id();
#endif /* CONFIG_USE_LOGICAL_IDS */

    /* startup APs one at a time as we use shared kernel boot stack */
    وقتی (smp_aps_index < boot_state.رقم_cpus) {
        word_t current_ap_index = smp_aps_index;

        printf("Starting node #%lu with APIC ID %lu \n",
               current_ap_index, boot_state.cpus[current_ap_index]);

        /* update cpu mapping for APs, store APIC ID of the next booting AP
         * as APIC ID are not continoius e.g. 0,2,1,3 for 4 cores with hyperthreading
         * we need to store a mapping to translate the index to real APIC ID */
        cpu_mapping.index_to_cpu_id[current_ap_index] = boot_state.cpus[current_ap_index];
        start_cpu(boot_state.cpus[current_ap_index], BOOT_گره_PADDR);

        /* wait for current AP to boot up */
        وقتی (smp_aps_index == current_ap_index) {
#ifdef ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
            گره_STATE(ksCurTime) = getCurrentTime();
            __atomic_thread_fence(__ATOMIC_ACQ_REL);
#endif
        }
    }
}

BOOT_CODE bool_t copy_boot_code_aps(uint32_t mem_lower)
{
    أكد(boot_cpu_end - boot_cpu_start < 0x400);

    /* Ensure that our boot code fits in the memory hole we want to use, and check this region
     * is free according to multiboot. As boot_cpu_end and boot_cpu_start are link time
     * symbols (and not compile time) this cannot be a compile time check */
    word_t boot_size = (word_t)(boot_cpu_end - boot_cpu_start);
    word_t boot_گره_بالاترین = BOOT_گره_PADDR + boot_size;
    word_t mem_lower_bytes = mem_lower << 10;
    اگر (boot_گره_بالاترین > BOOT_گره_MAX_PADDR) {
        printf("AP boot code does not fit in chosen memory hole. Can be at most %lu, is %lu\n",
               (word_t)(BOOT_گره_MAX_PADDR - BOOT_گره_PADDR), boot_size);
        بازگشت false;
    }
    اگر (mem_lower_bytes < boot_گره_بالاترین) {
        printf("Need lower physical memory up to %lu to be free. Multiboot reports only up to %lu\n",
               boot_گره_بالاترین, mem_lower_bytes);
        بازگشت false;
    }

    /* copy CPU bootup code to lower memory */
    memcpy((تهی *)BOOT_گره_PADDR, boot_cpu_start, boot_size);
    بازگشت true;
}

ایستا BOOT_CODE bool_t try_boot_گره(تهی)
{
    set_Current_VSpace_ریشه(kpptr_to_paddr(X86_KERNEL_VSPACE_ریشه), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm فرار("" ::: "memory");

    /* initialise the CPU, make sure legacy interrupts are disabled */
    اگر (!init_cpu(1)) {
        بازگشت false;
    }

#ifdef CONFIG_USE_LOGICAL_IDS
    update_logical_id_mappings();
#endif /* CONFIG_USE_LOGICAL_IDS */
    بازگشت true;
}

/* This is the entry function for APs. However, it is not a BOOT_CODE as
 * there is a race between exiting this function and root task running on
 * node #0 to possibly reallocate this memory */
VISIBLE تهی boot_گره(تهی)
{
    bool_t result;

    mode_init_tls(smp_aps_index);
    result = try_boot_گره();

    اگر (!result) {
        fail("boot_node failed for some reason :(\n");
    }

    clock_sync_test();
    smp_aps_index++;

    /* grab BKL before leaving the kernel */
    گره_LOCK_SYS;

    init_core_state(SchedulerAction_ChooseNewThread);
    ARCH_گره_STATE(x86KScurInterrupt) = int_invalid;
    ARCH_گره_STATE(x86KSPendingInterrupt) = int_invalid;

    schedule();
    activateThread();
}

#endif /* ENABLE_SMP_SUPPORT */
