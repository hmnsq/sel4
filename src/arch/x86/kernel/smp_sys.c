/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <arch/machine.h>
#ضمن <arch/machine/timer.h>
#ضمن <arch/kernel/boot_sys.h>
#ضمن <arch/kernel/smp_sys.h>
#ضمن <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT

/* Index of next AP to boot, BSP has index zero */
BOOT_DATA VISIBLE
متطاير word_t smp_aps_index = 1;

#ifdef CONFIG_USE_LOGICAL_IDS
BOOT_CODE ساكن فراغ update_logical_id_mappings(فراغ)
{
    cpu_mapping.index_to_logical_id[getCurrentCPUIndex()] = apic_get_logical_id();

    لكل (صحيح i = 0; i < smp_aps_index; i++) {
        إذا (apic_get_cluster(cpu_mapping.index_to_logical_id[getCurrentCPUIndex()]) ==
            apic_get_cluster(cpu_mapping.index_to_logical_id[i])) {

            cpu_mapping.other_indexes_in_cluster[getCurrentCPUIndex()] |= BIT(i);
            cpu_mapping.other_indexes_in_cluster[i] |= BIT(getCurrentCPUIndex());
        }
    }
}
#endif /* CONFIG_USE_LOGICAL_IDS */

BOOT_CODE ساكن فراغ start_cpu(cpu_id_t cpu_id, paddr_t boot_fun_paddr)
{
    /* memory fence needed before starting the other CPU */
    x86_mfence();

    /* starting the other CPU */
    apic_send_init_ipi(cpu_id);
    apic_send_startup_ipi(cpu_id, boot_fun_paddr);
}

BOOT_CODE فراغ start_boot_aps(فراغ)
{
    /* update cpu mapping for BSP, cpus[0] is always assumed to be BSP */
    cpu_mapping.index_to_cpu_id[getCurrentCPUIndex()] = boot_state.cpus[0];
#ifdef CONFIG_USE_LOGICAL_IDS
    cpu_mapping.index_to_logical_id[getCurrentCPUIndex()] = apic_get_logical_id();
#endif /* CONFIG_USE_LOGICAL_IDS */

    /* startup APs one at a time as we use shared kernel boot stack */
    طالما (smp_aps_index < boot_state.ژمارە_cpus) {
        word_t current_ap_index = smp_aps_index;

        printf("Starting node #%lu with APIC ID %lu \n",
               current_ap_index, boot_state.cpus[current_ap_index]);

        /* update cpu mapping for APs, store APIC ID of the next booting AP
         * as APIC ID are not continoius e.g. 0,2,1,3 for 4 cores with hyperthreading
         * we need to store a mapping to translate the index to real APIC ID */
        cpu_mapping.index_to_cpu_id[current_ap_index] = boot_state.cpus[current_ap_index];
        start_cpu(boot_state.cpus[current_ap_index], BOOT_گرێ_PADDR);

        /* wait for current AP to boot up */
        طالما (smp_aps_index == current_ap_index) {
#ifdef ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
            گرێ_STATE(ksCurTime) = getCurrentTime();
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
    word_t boot_گرێ_لووتکە = BOOT_گرێ_PADDR + boot_size;
    word_t mem_lower_bytes = mem_lower << 10;
    إذا (boot_گرێ_لووتکە > BOOT_گرێ_MAX_PADDR) {
        printf("AP boot code does not fit in chosen memory hole. Can be at most %lu, is %lu\n",
               (word_t)(BOOT_گرێ_MAX_PADDR - BOOT_گرێ_PADDR), boot_size);
        أرجع false;
    }
    إذا (mem_lower_bytes < boot_گرێ_لووتکە) {
        printf("Need lower physical memory up to %lu to be free. Multiboot reports only up to %lu\n",
               boot_گرێ_لووتکە, mem_lower_bytes);
        أرجع false;
    }

    /* copy CPU bootup code to lower memory */
    memcpy((فراغ *)BOOT_گرێ_PADDR, boot_cpu_start, boot_size);
    أرجع true;
}

ساكن BOOT_CODE bool_t try_boot_گرێ(فراغ)
{
    set_Current_VSpace_ڕەگ(kpptr_to_paddr(X86_KERNEL_VSPACE_ڕەگ), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm متطاير("" ::: "memory");

    /* initialise the CPU, make sure legacy interrupts are disabled */
    إذا (!init_cpu(1)) {
        أرجع false;
    }

#ifdef CONFIG_USE_LOGICAL_IDS
    update_logical_id_mappings();
#endif /* CONFIG_USE_LOGICAL_IDS */
    أرجع true;
}

/* This is the entry function for APs. However, it is not a BOOT_CODE as
 * there is a race between exiting this function and root task running on
 * node #0 to possibly reallocate this memory */
VISIBLE فراغ boot_گرێ(فراغ)
{
    bool_t result;

    mode_init_tls(smp_aps_index);
    result = try_boot_گرێ();

    إذا (!result) {
        fail("boot_node failed for some reason :(\n");
    }

    clock_sync_test();
    smp_aps_index++;

    /* grab BKL before leaving the kernel */
    گرێ_LOCK_SYS;

    init_core_state(SchedulerAction_ChooseNewThread);
    ARCH_گرێ_STATE(x86KScurInterrupt) = int_invalid;
    ARCH_گرێ_STATE(x86KSPendingInterrupt) = int_invalid;

    schedule();
    activateThread();
}

#endif /* ENABLE_SMP_SUPPORT */
