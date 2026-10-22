/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <util.h>
#অন্তর্ভাব <hardware.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <arch/machine.h>
#অন্তর্ভাব <arch/kernel/apic.h>
#অন্তর্ভাব <arch/kernel/cmdline.h>
#অন্তর্ভাব <arch/kernel/boot.h>
#অন্তর্ভাব <arch/kernel/boot_sys.h>
#অন্তর্ভাব <arch/kernel/smp_sys.h>
#অন্তর্ভাব <arch/kernel/vspace.h>
#অন্তর্ভাব <arch/kernel/elf.h>
#অন্তর্ভাব <smp/lock.h>
#অন্তর্ভাব <linker.h>
#অন্তর্ভাব <plat/machine/acpi.h>
#অন্তর্ভাব <plat/machine/devices.h>
#অন্তর্ভাব <plat/machine/pic.h>
#অন্তর্ভাব <plat/machine/ioapic.h>
#অন্তর্ভাব <sel4/arch/bootinfo_types.h>

/* addresses defined in linker script */
/* need a fake array to get the pointer from the linker script */

/* start/end of CPU boot code */
বাহ্য বর্ণ boot_cpu_start[1];
বাহ্য বর্ণ boot_cpu_end[1];

/* start/end of boot stack */
বাহ্য বর্ণ boot_স্তূপ_bottom[1];
বাহ্য বর্ণ boot_স্তূপ_শীর্ষ[1];

/* locations in kernel image */
বাহ্য বর্ণ ki_skim_start[1];
বাহ্য বর্ণ ki_skim_end[1];

#ifdef CONFIG_PRINTING
/* kernel entry point */
বাহ্য বর্ণ _start[1];
#endif

/* constants */

#define HIGHMEM_PADDR 0x100000

BOOT_BSS
boot_state_t boot_state;

/* global variables (not covered by abstract specification) */

BOOT_BSS
cmdline_opt_t cmdline_opt;

/* functions not modeled in abstract specification */

BOOT_CODE স্থির paddr_t find_load_paddr(paddr_t min_paddr, word_t image_size)
{
    পূর্ণাঙ্ক i;

    প্রতি (i = 0; i < boot_state.mem_p_regs.গুনতি; i++) {
        paddr_t start = MAX(min_paddr, boot_state.mem_p_regs.list[i].start);
        paddr_t end = boot_state.mem_p_regs.list[i].end;
        word_t region_size = end - start;

        যদি (region_size >= image_size) {
            প্রত্যাগম start;
        }
    }

    প্রত্যাগম 0;
}

BOOT_CODE স্থির paddr_t load_boot_module(word_t boot_module_start, word_t boot_module_end, paddr_t load_paddr)
{
    v_region_t v_reg;
    word_t entry;
    Elf_Header_t *elf_file = (Elf_Header_t *)boot_module_start;

    যদি (!elf_checkFile(elf_file, boot_module_end - boot_module_start)) {
        printf("Boot module does not contain a valid ELF image\n");
        প্রত্যাগম 0;
    }

    v_reg = elf_getMemoryBounds(elf_file);
    entry = elf_file->e_entry;

    যদি (v_reg.end == 0) {
        printf("ELF image in boot module does not contain any segments\n");
        প্রত্যাগম 0;
    }
    v_reg.end = ROUND_UP(v_reg.end, PAGE_BITS);

    printf("size=0x%lx v_entry=%p v_start=%p v_end=%p ",
           v_reg.end - v_reg.start,
           (শূন্য *)entry,
           (শূন্য *)v_reg.start,
           (শূন্য *)v_reg.end
          );

    যদি (!IS_ALIGNED(v_reg.start, PAGE_BITS)) {
        printf("Userland image virtual start address must be 4KB-aligned\n");
        প্রত্যাগম 0;
    }
    যদি (v_reg.end + 2 * BIT(PAGE_BITS) > USER_শীর্ষ) {
        /* for IPC buffer frame and bootinfo frame, need 2*4K of additional userland virtual memory */
        printf("Userland image virtual end address too high\n");
        প্রত্যাগম 0;
    }
    যদি ((entry < v_reg.start) || (entry >= v_reg.end)) {
        printf("Userland image entry point does not lie within userland image\n");
        প্রত্যাগম 0;
    }

    load_paddr = find_load_paddr(load_paddr, v_reg.end - v_reg.start);
    প্রতিজ্ঞা(load_paddr);

    /* fill ui_info struct */
    boot_state.ui_info.pv_offset = load_paddr - v_reg.start;
    boot_state.ui_info.p_reg.start = load_paddr;
    load_paddr += v_reg.end - v_reg.start;
    boot_state.ui_info.p_reg.end = load_paddr;
    boot_state.ui_info.v_entry = entry;

    printf("p_start=0x%lx p_end=0x%lx\n",
           boot_state.ui_info.p_reg.start,
           boot_state.ui_info.p_reg.end
          );

    /* initialise all initial userland memory and load potentially sparse ELF image */
    memzero(
        (শূন্য *)boot_state.ui_info.p_reg.start,
        boot_state.ui_info.p_reg.end - boot_state.ui_info.p_reg.start
    );
    elf_load(elf_file, boot_state.ui_info.pv_offset);

    প্রত্যাগম load_paddr;
}

স্থির BOOT_CODE bool_t try_boot_sys_গ্রন্থি(cpu_id_t cpu_id)
{
    যদি (!map_kernel_window(
            boot_state.অঙ্ক_ioapic,
            boot_state.ioapic_paddr,
            boot_state.অঙ্ক_drhu,
            boot_state.drhu_list
        )) {
        প্রত্যাগম false;
    }
    set_Current_VSpace_মূল(kpptr_to_paddr(X86_KERNEL_VSPACE_মূল), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm চঞ্চল("" ::: "memory");

#ifdef CONFIG_KERNEL_SKIM_WINDOW
    যদি (!map_skim_window((vptr_t)ki_skim_start, (vptr_t)ki_skim_end)) {
        প্রত্যাগম false;
    }
#endif

    /* initialise the CPU */
    যদি (!init_cpu(config_set(CONFIG_IRQ_IOAPIC) ? 1 : 0)) {
        প্রত্যাগম false;
    }

    /* initialise NDKS and kernel heap */
    যদি (!init_sys_state(
            cpu_id,
            &boot_state.mem_p_regs,
            boot_state.ui_info,
            /* parameters below not modelled in abstract specification */
            boot_state.অঙ্ক_drhu,
            boot_state.drhu_list,
            &boot_state.rmrr_list,
            &boot_state.acpi_rsdp,
            &boot_state.vbe_info,
            &boot_state.mb_mmap_info,
            &boot_state.fb_info
        )) {
        প্রত্যাগম false;
    }

    প্রত্যাগম true;
}

স্থির BOOT_CODE bool_t add_mem_p_regs(p_region_t reg)
{
    যদি (reg.start == reg.end) {
        // Return true here if asked to add an empty region.
        // Some of the callers round down the end address to
        প্রত্যাগম true;
    }
    যদি (reg.end > PADDR_শীর্ষ && reg.start >= PADDR_শীর্ষ) {
        /* Return true here as it's not an error for there to exist memory outside the kernel window,
         * we're just going to ignore it and leave it to be given out as device memory */
        প্রত্যাগম true;
    }
    যদি (boot_state.mem_p_regs.গুনতি == MAX_অঙ্ক_FREEMEM_REG) {
        printf("Dropping memory region 0x%lx-0x%lx, try increasing MAX_NUM_FREEMEM_REG\n", reg.start, reg.end);
        প্রত্যাগম false;
    }
    যদি (reg.end > PADDR_শীর্ষ) {
        প্রতিজ্ঞা(reg.start < PADDR_শীর্ষ);
        /* Clamp a region to the top of the kernel window if it extends beyond */
        reg.end = PADDR_শীর্ষ;
    }
    printf("Adding physical memory region 0x%lx-0x%lx\n", reg.start, reg.end);
    boot_state.mem_p_regs.list[boot_state.mem_p_regs.গুনতি] = reg;
    boot_state.mem_p_regs.গুনতি++;
    প্রত্যাগম true;
}

/*
 * the code relies that the GRUB provides correct information
 * about the actual physical memory regions.
 */
স্থির BOOT_CODE bool_t parse_mem_map(uint32_t mmap_length, uint32_t mmap_addr)
{
    multiboot_mmap_t *mmap = (multiboot_mmap_t *)((word_t)mmap_addr);
    printf("Parsing GRUB physical memory map\n");

    যাবত্ ((word_t)mmap < (word_t)(mmap_addr + mmap_length)) {
        uint64_t mem_start = mmap->base_addr;
        uint64_t mem_length = mmap->length;
        uint32_t type = mmap->type;
        যদি (mem_start != (uint64_t)(word_t)mem_start) {
            printf("\tPhysical memory region not addressable\n");
        } অন্যথা {
            printf("\tPhysical Memory Region from %lx size %lx type %d\n", (long)mem_start, (long)mem_length, type);
            যদি (type == MULTIBOOT_MMAP_USEABLE_TYPE && mem_start >= HIGHMEM_PADDR && mem_length >= BIT(PAGE_BITS)) {

                যদি (!add_mem_p_regs((p_region_t) {
                ROUND_UP(mem_start, PAGE_BITS), ROUND_DOWN(mem_start + mem_length, PAGE_BITS),
                })) {
                    প্রত্যাগম false;
                }
            }
        }
        mmap++;
    }
    প্রত্যাগম true;
}

স্থির BOOT_CODE bool_t is_compiled_for_microarchitecture(শূন্য)
{
    word_t microarch_generation = 0;
    x86_cpu_identity_t *model_info = x86_cpuid_get_model_info();

    যদি (config_set(CONFIG_ARCH_X86_SKYLAKE)) {
        microarch_generation = 7;
    } অন্যথা যদি (config_set(CONFIG_ARCH_X86_BROADWELL)) {
        microarch_generation = 6;
    } অন্যথা যদি (config_set(CONFIG_ARCH_X86_HASWELL)) {
        microarch_generation = 5;
    } অন্যথা যদি (config_set(CONFIG_ARCH_X86_IVY)) {
        microarch_generation = 4;
    } অন্যথা যদি (config_set(CONFIG_ARCH_X86_SANDY)) {
        microarch_generation = 3;
    } অন্যথা যদি (config_set(CONFIG_ARCH_X86_WESTMERE)) {
        microarch_generation = 2;
    } অন্যথা যদি (config_set(CONFIG_ARCH_X86_NEHALEM)) {
        microarch_generation = 1;
    }

    বিকল্প (model_info->model) {
    পক্ষ SKYLAKE_1_MODEL_ID:
    পক্ষ SKYLAKE_2_MODEL_ID:
    পক্ষ SKYLAKE_X_MODEL_ID:
        যদি (microarch_generation > 7) {
            প্রত্যাগম false;
        }
        ভঙ্গ;

    পক্ষ BROADWELL_1_MODEL_ID:
    পক্ষ BROADWELL_2_MODEL_ID:
    পক্ষ BROADWELL_3_MODEL_ID:
    পক্ষ BROADWELL_4_MODEL_ID:
    পক্ষ BROADWELL_5_MODEL_ID:
        যদি (microarch_generation > 6) {
            প্রত্যাগম false;
        }
        ভঙ্গ;

    পক্ষ HASWELL_1_MODEL_ID:
    পক্ষ HASWELL_2_MODEL_ID:
    পক্ষ HASWELL_3_MODEL_ID:
    পক্ষ HASWELL_4_MODEL_ID:
        যদি (microarch_generation > 5) {
            প্রত্যাগম false;
        }
        ভঙ্গ;

    পক্ষ IVY_BRIDGE_1_MODEL_ID:
    পক্ষ IVY_BRIDGE_2_MODEL_ID:
    পক্ষ IVY_BRIDGE_3_MODEL_ID:
        যদি (microarch_generation > 4) {
            প্রত্যাগম false;
        }
        ভঙ্গ;

    পক্ষ SANDY_BRIDGE_1_MODEL_ID:
    পক্ষ SANDY_BRIDGE_2_MODEL_ID:
        যদি (microarch_generation > 3) {
            প্রত্যাগম false;
        }
        ভঙ্গ;

    পক্ষ WESTMERE_1_MODEL_ID:
    পক্ষ WESTMERE_2_MODEL_ID:
    পক্ষ WESTMERE_3_MODEL_ID:
        যদি (microarch_generation > 2) {
            প্রত্যাগম false;
        }
        ভঙ্গ;

    পক্ষ NEHALEM_1_MODEL_ID:
    পক্ষ NEHALEM_2_MODEL_ID:
    পক্ষ NEHALEM_3_MODEL_ID:
        যদি (microarch_generation > 1) {
            প্রত্যাগম false;
        }
        ভঙ্গ;

    স্বভাব:
        যদি (!config_set(CONFIG_ARCH_X86_GENERIC)) {
            প্রত্যাগম false;
        }
    }

    প্রত্যাগম true;
}

স্থির BOOT_CODE bool_t try_boot_sys(শূন্য)
{
    paddr_t mods_end_paddr = boot_state.mods_end_paddr;
    p_region_t ui_p_regs;
    paddr_t load_paddr;

    boot_state.ki_p_reg = get_p_reg_kernel_img();

    যদি (!x86_cpuid_initialize()) {
        printf("Warning: Your x86 CPU has an unsupported vendor, '%s'.\n"
               "\tYour setup may not be able to competently run seL4 as "
               "intended.\n"
               "\tCurrently supported x86 vendors are AMD and Intel.\n",
               x86_cpuid_get_identity()->vendor_string);
    }

    যদি (!is_compiled_for_microarchitecture()) {
        printf("Warning: Your kernel was not compiled for the current microarchitecture.\n");
    }

    cpuid_007h_edx_t edx;
    edx.শব্দগুলি[0] = x86_cpuid_edx(0x7, 0);
    /* see if we can definitively say whether or not we need the skim window by
     * checking whether the CPU is vulnerable to rogue data cache loads (rdcl) */
    যদি (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.শব্দগুলি[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        যদি (ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && config_set(CONFIG_KERNEL_SKIM_WINDOW)) {
            printf("CPU reports not vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is enabled. Performance is needlessly being impacted, consider disabling.\n");
        } অন্যথা যদি (!ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && !config_set(CONFIG_KERNEL_SKIM_WINDOW)) {
            printf("CPU reports vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is *not* enabled. Please re-build with SKIM window enabled.");
            প্রত্যাগম false;
        }
    } অন্যথা {
        /* hardware doesn't tell us directly so guess based on CPU vendor */
        যদি (config_set(CONFIG_KERNEL_SKIM_WINDOW) && x86_cpuid_get_identity()->vendor == X86_VENDOR_AMD) {
            printf("SKIM window for mitigating Meltdown (https://www.meltdownattack.com) "
                   "not necessary for AMD and performance is being needlessly affected, "
                   "consider disabling\n");
        }
        যদি (!config_set(CONFIG_KERNEL_SKIM_WINDOW) && x86_cpuid_get_identity()->vendor == X86_VENDOR_INTEL) {
            printf("***WARNING*** SKIM window not enabled, this machine is probably vulnerable "
                   "to Meltdown (https://www.meltdownattack.com), consider enabling\n");
        }
    }

#ifdef ENABLE_SMP_SUPPORT
    /* copy boot code for APs to lower memory to run in real mode */
    যদি (!copy_boot_code_aps(boot_state.mem_lower)) {
        প্রত্যাগম false;
    }
    /* Initialize any kernel TLS */
    mode_init_tls(0);
#endif /* ENABLE_SMP_SUPPORT */

    printf("Kernel loaded to: start=0x%lx end=0x%lx size=0x%lx entry=0x%lx\n",
           boot_state.ki_p_reg.start,
           boot_state.ki_p_reg.end,
           boot_state.ki_p_reg.end - boot_state.ki_p_reg.start,
           (paddr_t)_start
          );

    /* remapping legacy IRQs to their correct vectors */
    pic_remap_irqs(IRQ_INT_OFFSET);
    যদি (config_set(CONFIG_IRQ_IOAPIC)) {
        /* Disable the PIC so that it does not generate any interrupts. We need to
         * do this *before* we initialize the apic */
        pic_disable();
    }

    /* validate the ACPI table */
    যদি (!acpi_validate_rsdp(&boot_state.acpi_rsdp)) {
        প্রত্যাগম false;
    }

    /* check if kernel configuration matches platform requirements */
    যদি (!acpi_fadt_scan(&boot_state.acpi_rsdp)) {
        প্রত্যাগম false;
    }

    যদি (!config_set(CONFIG_IOMMU) || cmdline_opt.disable_iommu) {
        boot_state.অঙ্ক_drhu = 0;
    } অন্যথা {
        /* query available IOMMUs from ACPI */
        acpi_dmar_scan(
            &boot_state.acpi_rsdp,
            boot_state.drhu_list,
            &boot_state.অঙ্ক_drhu,
            MAX_অঙ্ক_DRHU,
            &boot_state.rmrr_list
        );
    }

    /* query available CPUs from ACPI */
    boot_state.অঙ্ক_cpus = acpi_madt_scan(&boot_state.acpi_rsdp, boot_state.cpus, &boot_state.অঙ্ক_ioapic,
                                         boot_state.ioapic_paddr);
    যদি (boot_state.অঙ্ক_cpus == 0) {
        printf("No CPUs detected\n");
        প্রত্যাগম false;
    }

    যদি (config_set(CONFIG_IRQ_IOAPIC)) {
        যদি (boot_state.অঙ্ক_ioapic == 0) {
            printf("No IOAPICs detected\n");
            প্রত্যাগম false;
        }
    } অন্যথা {
        যদি (boot_state.অঙ্ক_ioapic > 0) {
            printf("Detected %d IOAPICs, but configured to use PIC instead\n", boot_state.অঙ্ক_ioapic);
        }
    }

    mods_end_paddr = ROUND_UP(mods_end_paddr, PAGE_BITS);
    প্রতিজ্ঞা(mods_end_paddr > boot_state.ki_p_reg.end);

    printf("ELF-loading userland images from boot modules:\n");
    load_paddr = mods_end_paddr;

    load_paddr = load_boot_module(boot_state.boot_module_start, boot_state.boot_module_end, load_paddr);
    যদি (!load_paddr) {
        প্রত্যাগম false;
    }

    /* calculate final location of userland images */
    ui_p_regs.start = boot_state.ki_p_reg.end;
    ui_p_regs.end = ui_p_regs.start + load_paddr - mods_end_paddr;

    printf(
        "Moving loaded userland images to final location: from=0x%lx to=0x%lx size=0x%lx\n",
        mods_end_paddr,
        ui_p_regs.start,
        ui_p_regs.end - ui_p_regs.start
    );
    memcpy((শূন্য *)ui_p_regs.start, (শূন্য *)mods_end_paddr, ui_p_regs.end - ui_p_regs.start);

    /* adjust p_reg and pv_offset to final load address */
    boot_state.ui_info.p_reg.start -= mods_end_paddr - ui_p_regs.start;
    boot_state.ui_info.p_reg.end   -= mods_end_paddr - ui_p_regs.start;
    boot_state.ui_info.pv_offset   -= mods_end_paddr - ui_p_regs.start;

    /* ==== following code corresponds to abstract specification after "select" ==== */

    যদি (!platAddDevices()) {
        প্রত্যাগম false;
    }

    /* Total number of cores we intend to boot */
    ks_অঙ্ক_CPUs = boot_state.অঙ্ক_cpus;

    printf("Starting node #0 with APIC ID %lu\n", boot_state.cpus[0]);
    যদি (!try_boot_sys_গ্রন্থি(boot_state.cpus[0])) {
        প্রত্যাগম false;
    }

    যদি (config_set(CONFIG_IRQ_IOAPIC)) {
        ioapic_init(1, boot_state.cpus, boot_state.অঙ্ক_ioapic);
    }

    /* initialize BKL before booting up APs */
    SMP_COND_STATEMENT(clh_lock_init());
    SMP_COND_STATEMENT(start_boot_aps());

    /* grab BKL before leaving the kernel */
    গ্রন্থি_LOCK_SYS;

    printf("Booting all finished, dropped to user space\n");

    প্রত্যাগম true;
}

স্থির BOOT_CODE bool_t try_boot_sys_mbi1(
    multiboot_info_t *mbi
)
{
    word_t i;
    multiboot_module_t *modules = (multiboot_module_t *)(word_t)mbi->part1.mod_list;

    cmdline_parse((ধ্রুব বর্ণ *)(word_t)mbi->part1.cmdline, &cmdline_opt);

    যদি ((mbi->part1.flags & MULTIBOOT_INFO_MEM_FLAG) == 0) {
        printf("Boot loader did not provide information about physical memory size\n");
        প্রত্যাগম false;
    }

    যদি (!(mbi->part1.flags & MULTIBOOT_INFO_MODS_FLAG)) {
        printf("Boot loader did not provide information about boot modules\n");
        প্রত্যাগম false;
    }

    printf("Detected %d boot module(s):\n", mbi->part1.mod_গুনতি);

    যদি (mbi->part1.mod_গুনতি < 1) {
        printf("Expect at least one boot module (containing a userland image)\n");
        প্রত্যাগম false;
    }

    প্রতি (i = 0; i < mbi->part1.mod_গুনতি; i++) {
        printf(
            "  module #%ld: start=0x%x end=0x%x size=0x%x name='%s'\n",
            i,
            modules[i].start,
            modules[i].end,
            modules[i].end - modules[i].start,
            (বর্ণ *)(long)modules[i].name
        );
        যদি ((sword_t)(modules[i].end - modules[i].start) <= 0) {
            printf("Invalid boot module size! Possible cause: boot module file not found by QEMU\n");
            প্রত্যাগম false;
        }
        যদি (boot_state.mods_end_paddr < modules[i].end) {
            boot_state.mods_end_paddr = modules[i].end;
        }
    }

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.গুনতি = 0;
    যদি (mbi->part1.flags & MULTIBOOT_INFO_MMAP_FLAG) {
        যদি (!parse_mem_map(mbi->part2.mmap_length, mbi->part2.mmap_addr)) {
            প্রত্যাগম false;
        }
        uint32_t multiboot_mmap_length = mbi->part2.mmap_length;
        যদি (multiboot_mmap_length > (SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * পরিমাণ(seL4_X86_mb_mmap_t))) {
            printf("Warning: Multiboot has reported more memory map entries, %zd, "
                   "than the max amount that will be passed in the bootinfo, %d. "
                   "These extra regions will still be turned into untyped caps.",
                   multiboot_mmap_length / পরিমাণ(seL4_X86_mb_mmap_t), SEL4_MULTIBOOT_MAX_MMAP_ENTRIES);
            multiboot_mmap_length = SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * পরিমাণ(seL4_X86_mb_mmap_t);
        }
        memcpy(&boot_state.mb_mmap_info.mmap, (শূন্য *)(word_t)mbi->part2.mmap_addr, multiboot_mmap_length);
        boot_state.mb_mmap_info.mmap_length = multiboot_mmap_length;
    } অন্যথা {
        /* calculate memory the old way */
        p_region_t avail;
        avail.start = HIGHMEM_PADDR;
        avail.end = ROUND_DOWN(avail.start + (mbi->part1.mem_upper << 10), PAGE_BITS);
        যদি (!add_mem_p_regs(avail)) {
            প্রত্যাগম false;
        }
    }

    /* copy VESA information from multiboot header */
    যদি ((mbi->part1.flags & MULTIBOOT_INFO_GRAPHICS_FLAG) == 0) {
        boot_state.vbe_info.vbeMode = -1;
        printf("Multiboot gave us no video information\n");
    } অন্যথা {
        boot_state.vbe_info.vbeInfoBlock = *(seL4_VBEInfoBlock_t *)(seL4_Word)mbi->part2.vbe_control_info;
        boot_state.vbe_info.vbeModeInfoBlock = *(seL4_VBEModeInfoBlock_t *)(seL4_Word)mbi->part2.vbe_mode_info;
        boot_state.vbe_info.vbeMode = mbi->part2.vbe_mode;
        printf("Got VBE info in multiboot. Current video mode is %d\n", mbi->part2.vbe_mode);
        boot_state.vbe_info.vbeInterfaceSeg = mbi->part2.vbe_interface_seg;
        boot_state.vbe_info.vbeInterfaceOff = mbi->part2.vbe_interface_off;
        boot_state.vbe_info.vbeInterfaceLen = mbi->part2.vbe_interface_len;
    }

    boot_state.mem_lower = mbi->part1.mem_lower;
    boot_state.boot_module_start = modules->start;
    boot_state.boot_module_end = modules->end;

    /* Initialize ACPI */
    যদি (!acpi_init(&boot_state.acpi_rsdp)) {
        প্রত্যাগম false;
    }

    প্রত্যাগম true;
}

স্থির BOOT_CODE bool_t try_boot_sys_mbi2(
    multiboot2_header_t *mbi2
)
{
    পূর্ণাঙ্ক mod_গুনতি                  = 0;
    multiboot2_tag_t ধ্রুব *tag   = (multiboot2_tag_t *)(mbi2 + 1);
    multiboot2_tag_t ধ্রুব *tag_e = (multiboot2_tag_t *)((word_t)mbi2 + mbi2->total_size);

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.গুনতি = 0;
    boot_state.mb_mmap_info.mmap_length = 0;
    boot_state.vbe_info.vbeMode = -1;

    যাবত্ (tag < tag_e && tag->type != MULTIBOOT2_TAG_END) {
        word_t ধ্রুব behind_tag = (word_t)tag + পরিমাণ(*tag);

        যদি (tag->type == MULTIBOOT2_TAG_CMDLINE) {
            বর্ণ ধ্রুব *ধ্রুব cmdline = (বর্ণ ধ্রুব * ধ্রুব)(behind_tag);
            cmdline_parse(cmdline, &cmdline_opt);
        } অন্যথা যদি (tag->type == MULTIBOOT2_TAG_ACPI_1) {
            যদি (ACPI_V1_SIZE == tag->size - পরিমাণ(*tag)) {
                memcpy(&boot_state.acpi_rsdp, (শূন্য *)behind_tag, tag->size - পরিমাণ(*tag));
            }
        } অন্যথা যদি (tag->type == MULTIBOOT2_TAG_ACPI_2) {
            যদি (পরিমাণ(boot_state.acpi_rsdp) == tag->size - পরিমাণ(*tag)) {
                memcpy(&boot_state.acpi_rsdp, (শূন্য *)behind_tag, পরিমাণ(boot_state.acpi_rsdp));
            }
        } অন্যথা যদি (tag->type == MULTIBOOT2_TAG_MODULE) {
            multiboot2_module_t ধ্রুব *module = (multiboot2_module_t ধ্রুব *)behind_tag;
            printf(
                "  module #%d: start=0x%x end=0x%x size=0x%x name='%s'\n",
                mod_গুনতি,
                module->start,
                module->end,
                module->end - module->start,
                module->string
            );

            যদি (mod_গুনতি == 0) {
                boot_state.boot_module_start = module->start;
                boot_state.boot_module_end = module->end;
            }

            mod_গুনতি ++;
            যদি ((sword_t)(module->end - module->start) <= 0) {
                printf("Invalid boot module size! Possible cause: boot module file not found\n");
                প্রত্যাগম false;
            }
            যদি (boot_state.mods_end_paddr < module->end) {
                boot_state.mods_end_paddr = module->end;
            }
        } অন্যথা যদি (tag->type == MULTIBOOT2_TAG_MEMORY) {
            multiboot2_memory_t ধ্রুব *s = (multiboot2_memory_t *)(behind_tag + 8);
            multiboot2_memory_t ধ্রুব *e = (multiboot2_memory_t *)((word_t)tag + tag->size);

            প্রতি (multiboot2_memory_t ধ্রুব *m = s; m < e; m++) {
                যদি (!m->addr) {
                    boot_state.mem_lower = m->size;
                }

                printf("\tPhysical Memory Region from %llx size %llx type %u\n", m->addr, m->size, m->type);
                যদি (m->addr != (uint64_t)(word_t)m->addr) {
                    printf("\t\tPhysical memory region not addressable\n");
                    অনুবর্ত;
                }

                যদি (m->type == MULTIBOOT_MMAP_USEABLE_TYPE && m->addr >= HIGHMEM_PADDR) {
                    যদি (!add_mem_p_regs((p_region_t) {
                    m->addr, m->addr + m->size
                }))
                    প্রত্যাগম false;
                }
            }
        } অন্যথা যদি (tag->type == MULTIBOOT2_TAG_FB) {
            multiboot2_fb_t ধ্রুব *fb = (multiboot2_fb_t ধ্রুব *)behind_tag;
            printf("Got framebuffer info in multiboot2. Current video mode is at physical address=%llx pitch=%u resolution=%ux%u@%u type=%u\n",
                   fb->addr, fb->pitch, fb->width, fb->height, fb->bpp, fb->type);
            boot_state.fb_info = *fb;
        }

        tag = (multiboot2_tag_t ধ্রুব *)((word_t)tag + ROUND_UP(tag->size, 3));
    }

    printf("Detected %d boot module(s):\n", mod_গুনতি);

    যদি (mod_গুনতি < 1) {
        printf("Expect at least one boot module (containing a userland image)\n");
        প্রত্যাগম false;
    }

    প্রত্যাগম true;
}

BOOT_CODE VISIBLE শূন্য boot_sys(
    unsigned long multiboot_magic,
    শূন্য *mbi)
{
    bool_t result = false;

    যদি (multiboot_magic == MULTIBOOT_MAGIC) {
        result = try_boot_sys_mbi1(mbi);
    } অন্যথা যদি (multiboot_magic == MULTIBOOT2_MAGIC) {
        result = try_boot_sys_mbi2(mbi);
    } অন্যথা {
        printf("Boot loader is not multiboot 1 or 2 compliant %lx\n", multiboot_magic);
    }

    যদি (result) {
        result = try_boot_sys();
    }

    যদি (!result) {
        fail("boot_sys failed for some reason :(\n");
    }

    ARCH_গ্রন্থি_STATE(x86KScurInterrupt) = int_invalid;
    ARCH_গ্রন্থি_STATE(x86KSPendingInterrupt) = int_invalid;

#ifdef CONFIG_KERNEL_MCS
    গ্রন্থি_STATE(ksCurTime) = getCurrentTime();
    গ্রন্থি_STATE(ksConsumed) = 0;
#endif

    schedule();
    activateThread();
}

