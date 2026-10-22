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

#اگر_تعریف_شده ENABLE_SMP_SUPPORT

/* Index of next AP to boot, BSP has index zero */
BOOT_داده VISIBLE
متطاير کلمه_t smp_aps_شاخص = 1;

#اگر_تعریف_شده CONFIG_USE_LOGICAL_IDS
BOOT_کد ساكن فراغ update_logical_id_mappings(فراغ)
{
    cpu_mapping.شاخص_مقصد_logical_id[getCurrentCPUIndex()] = apic_get_logical_id();

    لكل (صحيح i = 0; i < smp_aps_شاخص; i++) {
        إذا (apic_get_cluster(cpu_mapping.شاخص_مقصد_logical_id[getCurrentCPUIndex()]) ==
            apic_get_cluster(cpu_mapping.شاخص_مقصد_logical_id[i])) {

            cpu_mapping.other_indexes_درون_cluster[getCurrentCPUIndex()] |= بیت(i);
            cpu_mapping.other_indexes_درون_cluster[i] |= بیت(getCurrentCPUIndex());
        }
    }
}
#پایان_اگر /* CONFIG_USE_LOGICAL_IDS */

BOOT_کد ساكن فراغ شروع_cpu(cpu_id_t cpu_id, paddr_t boot_fun_paddr)
{
    /* memory fence needed before starting the other CPU */
    x86_mfence();

    /* starting the other CPU */
    apic_send_init_ipi(cpu_id);
    apic_send_startup_ipi(cpu_id, boot_fun_paddr);
}

BOOT_کد فراغ شروع_boot_aps(فراغ)
{
    /* update cpu mapping for BSP, cpus[0] is always assumed to be BSP */
    cpu_mapping.شاخص_مقصد_cpu_id[getCurrentCPUIndex()] = boot_state.cpus[0];
#اگر_تعریف_شده CONFIG_USE_LOGICAL_IDS
    cpu_mapping.شاخص_مقصد_logical_id[getCurrentCPUIndex()] = apic_get_logical_id();
#پایان_اگر /* CONFIG_USE_LOGICAL_IDS */

    /* startup APs one at a time as we use shared kernel boot stack */
    طالما (smp_aps_شاخص < boot_state.رقم_cpus) {
        کلمه_t current_ap_شاخص = smp_aps_شاخص;

        چاپ_کن("Starting node #%lu with APIC ID %lu \n",
               current_ap_شاخص, boot_state.cpus[current_ap_شاخص]);

        /* update cpu mapping for APs, store APIC ID of the next booting AP
         * as APIC ID are not continoius e.g. 0,2,1,3 for 4 cores with hyperthreading
         * we need to store a mapping to translate the index to real APIC ID */
        cpu_mapping.شاخص_مقصد_cpu_id[current_ap_شاخص] = boot_state.cpus[current_ap_شاخص];
        شروع_cpu(boot_state.cpus[current_ap_شاخص], BOOT_گره_PADDR);

        /* wait for current AP to boot up */
        طالما (smp_aps_شاخص == current_ap_شاخص) {
#اگر_تعریف_شده ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
            گره_STATE(ks_Cur_زمان) = get_Current_زمان();
            __atomic_thread_fence(__ATOMIC_ACQ_REL);
#پایان_اگر
        }
    }
}

BOOT_کد bool_t copy_boot_کد_aps(uint32_t mem_پایینی)
{
    أكد(boot_cpu_انتها - boot_cpu_شروع < 0x400);

    /* Ensure that our boot code fits in the memory hole we want to use, and check this region
     * is free according to multiboot. As boot_cpu_end and boot_cpu_start are link time
     * symbols (and not compile time) this cannot be a compile time check */
    کلمه_t boot_اندازه = (کلمه_t)(boot_cpu_انتها - boot_cpu_شروع);
    کلمه_t boot_گره_بالاترین = BOOT_گره_PADDR + boot_اندازه;
    کلمه_t mem_پایینی_بایتها = mem_پایینی << 10;
    إذا (boot_گره_بالاترین > BOOT_گره_MAX_PADDR) {
        چاپ_کن("AP boot code does not fit in chosen memory hole. Can be at most %lu, is %lu\n",
               (کلمه_t)(BOOT_گره_MAX_PADDR - BOOT_گره_PADDR), boot_اندازه);
        أرجع false;
    }
    إذا (mem_پایینی_بایتها < boot_گره_بالاترین) {
        چاپ_کن("Need lower physical memory up to %lu to be free. Multiboot reports only up to %lu\n",
               boot_گره_بالاترین, mem_پایینی_بایتها);
        أرجع false;
    }

    /* copy CPU bootup code to lower memory */
    کپی_حافظه((فراغ *)BOOT_گره_PADDR, boot_cpu_شروع, boot_اندازه);
    أرجع true;
}

ساكن BOOT_کد bool_t try_boot_گره(فراغ)
{
    مجموعه_Current_VSpace_ریشه(kpptr_مقصد_paddr(X86_KERNEL_VSPACE_ریشه), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm متطاير("" ::: "memory");

    /* initialise the CPU, make sure legacy interrupts are disabled */
    إذا (!init_cpu(1)) {
        أرجع false;
    }

#اگر_تعریف_شده CONFIG_USE_LOGICAL_IDS
    update_logical_id_mappings();
#پایان_اگر /* CONFIG_USE_LOGICAL_IDS */
    أرجع true;
}

/* This is the entry function for APs. However, it is not a BOOT_CODE as
 * there is a race between exiting this function and root task running on
 * node #0 to possibly reallocate this memory */
VISIBLE فراغ boot_گره(فراغ)
{
    bool_t نتیجه;

    نما_init_tls(smp_aps_شاخص);
    نتیجه = try_boot_گره();

    إذا (!نتیجه) {
        fail("boot_node failed for some reason :(\n");
    }

    clock_sync_test();
    smp_aps_شاخص++;

    /* grab BKL before leaving the kernel */
    گره_قفل_SYS;

    init_core_state(Scheduler_Action_انتخاب_New_Thread);
    ARCH_گره_STATE(x86_KScur_وقفه) = int_invalid;
    ARCH_گره_STATE(x86_KSPending_وقفه) = int_invalid;

    زمانبندی();
    activateThread();
}

#پایان_اگر /* ENABLE_SMP_SUPPORT */
