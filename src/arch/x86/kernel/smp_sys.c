/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/timer.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/boot_sys.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/smp_sys.h>
#ଅନ୍ତର୍ଭାଵ <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT

/* Index of next AP to boot, BSP has index zero */
BOOT_DATA VISIBLE
ଅସ୍ଥିର word_t smp_aps_index = 1;

#ifdef CONFIG_USE_LOGICAL_IDS
BOOT_CODE ସ୍ଥିର ଶୂନ୍ୟ update_logical_id_mappings(ଶୂନ୍ୟ)
{
    cpu_mapping.index_to_logical_id[getCurrentCPUIndex()] = apic_get_logical_id();

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < smp_aps_index; i++) {
        ଯଦି (apic_get_cluster(cpu_mapping.index_to_logical_id[getCurrentCPUIndex()]) ==
            apic_get_cluster(cpu_mapping.index_to_logical_id[i])) {

            cpu_mapping.other_indexes_in_cluster[getCurrentCPUIndex()] |= BIT(i);
            cpu_mapping.other_indexes_in_cluster[i] |= BIT(getCurrentCPUIndex());
        }
    }
}
#endif /* CONFIG_USE_LOGICAL_IDS */

BOOT_CODE ସ୍ଥିର ଶୂନ୍ୟ start_cpu(cpu_id_t cpu_id, paddr_t boot_fun_paddr)
{
    /* memory fence needed before starting the other CPU */
    x86_mfence();

    /* starting the other CPU */
    apic_send_init_ipi(cpu_id);
    apic_send_startup_ipi(cpu_id, boot_fun_paddr);
}

BOOT_CODE ଶୂନ୍ୟ start_boot_aps(ଶୂନ୍ୟ)
{
    /* update cpu mapping for BSP, cpus[0] is always assumed to be BSP */
    cpu_mapping.index_to_cpu_id[getCurrentCPUIndex()] = boot_state.cpus[0];
#ifdef CONFIG_USE_LOGICAL_IDS
    cpu_mapping.index_to_logical_id[getCurrentCPUIndex()] = apic_get_logical_id();
#endif /* CONFIG_USE_LOGICAL_IDS */

    /* startup APs one at a time as we use shared kernel boot stack */
    ଯେତେବେଳେ (smp_aps_index < boot_state.ଅଙ୍କ_cpus) {
        word_t current_ap_index = smp_aps_index;

        printf("Starting node #%lu with APIC ID %lu \n",
               current_ap_index, boot_state.cpus[current_ap_index]);

        /* update cpu mapping for APs, store APIC ID of the next booting AP
         * as APIC ID are not continoius e.g. 0,2,1,3 for 4 cores with hyperthreading
         * we need to store a mapping to translate the index to real APIC ID */
        cpu_mapping.index_to_cpu_id[current_ap_index] = boot_state.cpus[current_ap_index];
        start_cpu(boot_state.cpus[current_ap_index], BOOT_ଗଣ୍ଠି_PADDR);

        /* wait for current AP to boot up */
        ଯେତେବେଳେ (smp_aps_index == current_ap_index) {
#ifdef ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
            ଗଣ୍ଠି_STATE(ksCurTime) = getCurrentTime();
            __atomic_thread_fence(__ATOMIC_ACQ_REL);
#endif
        }
    }
}

BOOT_CODE bool_t copy_boot_code_aps(uint32_t mem_lower)
{
    ପ୍ରତିଜ୍ଞା(boot_cpu_end - boot_cpu_start < 0x400);

    /* Ensure that our boot code fits in the memory hole we want to use, and check this region
     * is free according to multiboot. As boot_cpu_end and boot_cpu_start are link time
     * symbols (and not compile time) this cannot be a compile time check */
    word_t boot_size = (word_t)(boot_cpu_end - boot_cpu_start);
    word_t boot_ଗଣ୍ଠି_ଶୀର୍ଷ = BOOT_ଗଣ୍ଠି_PADDR + boot_size;
    word_t mem_lower_bytes = mem_lower << 10;
    ଯଦି (boot_ଗଣ୍ଠି_ଶୀର୍ଷ > BOOT_ଗଣ୍ଠି_MAX_PADDR) {
        printf("AP boot code does not fit in chosen memory hole. Can be at most %lu, is %lu\n",
               (word_t)(BOOT_ଗଣ୍ଠି_MAX_PADDR - BOOT_ଗଣ୍ଠି_PADDR), boot_size);
        ଫେରନ୍ତୁ false;
    }
    ଯଦି (mem_lower_bytes < boot_ଗଣ୍ଠି_ଶୀର୍ଷ) {
        printf("Need lower physical memory up to %lu to be free. Multiboot reports only up to %lu\n",
               boot_ଗଣ୍ଠି_ଶୀର୍ଷ, mem_lower_bytes);
        ଫେରନ୍ତୁ false;
    }

    /* copy CPU bootup code to lower memory */
    memcpy((ଶୂନ୍ୟ *)BOOT_ଗଣ୍ଠି_PADDR, boot_cpu_start, boot_size);
    ଫେରନ୍ତୁ true;
}

ସ୍ଥିର BOOT_CODE bool_t try_boot_ଗଣ୍ଠି(ଶୂନ୍ୟ)
{
    set_Current_VSpace_ମୂଳ(kpptr_to_paddr(X86_KERNEL_VSPACE_ମୂଳ), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm ଅସ୍ଥିର("" ::: "memory");

    /* initialise the CPU, make sure legacy interrupts are disabled */
    ଯଦି (!init_cpu(1)) {
        ଫେରନ୍ତୁ false;
    }

#ifdef CONFIG_USE_LOGICAL_IDS
    update_logical_id_mappings();
#endif /* CONFIG_USE_LOGICAL_IDS */
    ଫେରନ୍ତୁ true;
}

/* This is the entry function for APs. However, it is not a BOOT_CODE as
 * there is a race between exiting this function and root task running on
 * node #0 to possibly reallocate this memory */
VISIBLE ଶୂନ୍ୟ boot_ଗଣ୍ଠି(ଶୂନ୍ୟ)
{
    bool_t result;

    mode_init_tls(smp_aps_index);
    result = try_boot_ଗଣ୍ଠି();

    ଯଦି (!result) {
        fail("boot_node failed for some reason :(\n");
    }

    clock_sync_test();
    smp_aps_index++;

    /* grab BKL before leaving the kernel */
    ଗଣ୍ଠି_LOCK_SYS;

    init_core_state(SchedulerAction_ChooseNewThread);
    ARCH_ଗଣ୍ଠି_STATE(x86KScurInterrupt) = int_invalid;
    ARCH_ଗଣ୍ଠି_STATE(x86KSPendingInterrupt) = int_invalid;

    schedule();
    activateThread();
}

#endif /* ENABLE_SMP_SUPPORT */
