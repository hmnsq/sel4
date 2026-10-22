/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/machine/timer.h>
#अन्तर्भाव <arch/kernel/boot_sys.h>
#अन्तर्भाव <arch/kernel/smp_sys.h>
#अन्तर्भाव <smp/lock.h>

#ifdef ENABLE_SMP_SUPPORT

/* Index of next AP to boot, BSP has index zero */
BOOT_DATA VISIBLE
अस्थिर word_t smp_aps_index = 1;

#ifdef CONFIG_USE_LOGICAL_IDS
BOOT_CODE स्थिर शून्य update_logical_id_mappings(शून्य)
{
    cpu_mapping.index_to_logical_id[getCurrentCPUIndex()] = apic_get_logical_id();

    साठी (पूर्णांक i = 0; i < smp_aps_index; i++) {
        जर (apic_get_cluster(cpu_mapping.index_to_logical_id[getCurrentCPUIndex()]) ==
            apic_get_cluster(cpu_mapping.index_to_logical_id[i])) {

            cpu_mapping.other_indexes_in_cluster[getCurrentCPUIndex()] |= BIT(i);
            cpu_mapping.other_indexes_in_cluster[i] |= BIT(getCurrentCPUIndex());
        }
    }
}
#endif /* CONFIG_USE_LOGICAL_IDS */

BOOT_CODE स्थिर शून्य start_cpu(cpu_id_t cpu_id, paddr_t boot_fun_paddr)
{
    /* memory fence needed before starting the other CPU */
    x86_mfence();

    /* starting the other CPU */
    apic_send_init_ipi(cpu_id);
    apic_send_startup_ipi(cpu_id, boot_fun_paddr);
}

BOOT_CODE शून्य start_boot_aps(शून्य)
{
    /* update cpu mapping for BSP, cpus[0] is always assumed to be BSP */
    cpu_mapping.index_to_cpu_id[getCurrentCPUIndex()] = boot_state.cpus[0];
#ifdef CONFIG_USE_LOGICAL_IDS
    cpu_mapping.index_to_logical_id[getCurrentCPUIndex()] = apic_get_logical_id();
#endif /* CONFIG_USE_LOGICAL_IDS */

    /* startup APs one at a time as we use shared kernel boot stack */
    पर्यंत (smp_aps_index < boot_state.अंक_cpus) {
        word_t current_ap_index = smp_aps_index;

        म_लिखो("Starting node #%lu with APIC ID %lu \n",
               current_ap_index, boot_state.cpus[current_ap_index]);

        /* update cpu mapping for APs, store APIC ID of the next booting AP
         * as APIC ID are not continoius e.g. 0,2,1,3 for 4 cores with hyperthreading
         * we need to store a mapping to translate the index to real APIC ID */
        cpu_mapping.index_to_cpu_id[current_ap_index] = boot_state.cpus[current_ap_index];
        start_cpu(boot_state.cpus[current_ap_index], BOOT_गाठ_PADDR);

        /* wait for current AP to boot up */
        पर्यंत (smp_aps_index == current_ap_index) {
#ifdef ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
            गाठ_STATE(ksCurTime) = getCurrentTime();
            __atomic_thread_fence(__ATOMIC_ACQ_REL);
#endif
        }
    }
}

BOOT_CODE bool_t copy_boot_code_aps(uint32_t mem_lower)
{
    प्रतिज्ञा(boot_cpu_end - boot_cpu_start < 0x400);

    /* Ensure that our boot code fits in the memory hole we want to use, and check this region
     * is free according to multiboot. As boot_cpu_end and boot_cpu_start are link time
     * symbols (and not compile time) this cannot be a compile time check */
    word_t boot_size = (word_t)(boot_cpu_end - boot_cpu_start);
    word_t boot_गाठ_शीर्ष = BOOT_गाठ_PADDR + boot_size;
    word_t mem_lower_bytes = mem_lower << 10;
    जर (boot_गाठ_शीर्ष > BOOT_गाठ_MAX_PADDR) {
        म_लिखो("AP boot code does not fit in chosen memory hole. Can be at most %lu, is %lu\n",
               (word_t)(BOOT_गाठ_MAX_PADDR - BOOT_गाठ_PADDR), boot_size);
        परत false;
    }
    जर (mem_lower_bytes < boot_गाठ_शीर्ष) {
        म_लिखो("Need lower physical memory up to %lu to be free. Multiboot reports only up to %lu\n",
               boot_गाठ_शीर्ष, mem_lower_bytes);
        परत false;
    }

    /* copy CPU bootup code to lower memory */
    स_नकल((शून्य *)BOOT_गाठ_PADDR, boot_cpu_start, boot_size);
    परत true;
}

स्थिर BOOT_CODE bool_t try_boot_गाठ(शून्य)
{
    set_Current_VSpace_मूळ(kpptr_to_paddr(X86_KERNEL_VSPACE_मूळ), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    यंत्र अस्थिर("" ::: "memory");

    /* initialise the CPU, make sure legacy interrupts are disabled */
    जर (!init_cpu(1)) {
        परत false;
    }

#ifdef CONFIG_USE_LOGICAL_IDS
    update_logical_id_mappings();
#endif /* CONFIG_USE_LOGICAL_IDS */
    परत true;
}

/* This is the entry function for APs. However, it is not a BOOT_CODE as
 * there is a race between exiting this function and root task running on
 * node #0 to possibly reallocate this memory */
VISIBLE शून्य boot_गाठ(शून्य)
{
    bool_t result;

    mode_init_tls(smp_aps_index);
    result = try_boot_गाठ();

    जर (!result) {
        fail("boot_node failed for some reason :(\n");
    }

    clock_sync_test();
    smp_aps_index++;

    /* grab BKL before leaving the kernel */
    गाठ_LOCK_SYS;

    init_core_state(SchedulerAction_ChooseNewThread);
    ARCH_गाठ_STATE(x86KScurInterrupt) = int_invalid;
    ARCH_गाठ_STATE(x86KSPendingInterrupt) = int_invalid;

    schedule();
    activateThread();
}

#endif /* ENABLE_SMP_SUPPORT */
