/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <util.h>
#அந்தர்பாவ <hardware.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <arch/machine.h>
#அந்தர்பாவ <arch/kernel/apic.h>
#அந்தர்பாவ <arch/kernel/cmdline.h>
#அந்தர்பாவ <arch/kernel/boot.h>
#அந்தர்பாவ <arch/kernel/boot_sys.h>
#அந்தர்பாவ <arch/kernel/smp_sys.h>
#அந்தர்பாவ <arch/kernel/vspace.h>
#அந்தர்பாவ <arch/kernel/elf.h>
#அந்தர்பாவ <smp/lock.h>
#அந்தர்பாவ <linker.h>
#அந்தர்பாவ <plat/machine/acpi.h>
#அந்தர்பாவ <plat/machine/devices.h>
#அந்தர்பாவ <plat/machine/pic.h>
#அந்தர்பாவ <plat/machine/ioapic.h>
#அந்தர்பாவ <sel4/arch/bootinfo_types.h>

/* addresses defined in linker script */
/* need a fake array to get the pointer from the linker script */

/* start/end of CPU boot code */
வெளி எழுத்து boot_cpu_start[1];
வெளி எழுத்து boot_cpu_end[1];

/* start/end of boot stack */
வெளி எழுத்து boot_அடுக்கு_bottom[1];
வெளி எழுத்து boot_அடுக்கு_உச்சி[1];

/* locations in kernel image */
வெளி எழுத்து ki_skim_start[1];
வெளி எழுத்து ki_skim_end[1];

#ifdef CONFIG_PRINTING
/* kernel entry point */
வெளி எழுத்து _start[1];
#endif

/* constants */

#define HIGHMEM_PADDR 0x100000

BOOT_BSS
boot_state_t boot_state;

/* global variables (not covered by abstract specification) */

BOOT_BSS
cmdline_opt_t cmdline_opt;

/* functions not modeled in abstract specification */

BOOT_CODE நிலையான paddr_t find_load_paddr(paddr_t min_paddr, word_t image_size)
{
    முழு i;

    க்கு (i = 0; i < boot_state.mem_p_regs.மொத்தம்; i++) {
        paddr_t start = MAX(min_paddr, boot_state.mem_p_regs.list[i].start);
        paddr_t end = boot_state.mem_p_regs.list[i].end;
        word_t region_size = end - start;

        எனில் (region_size >= image_size) {
            திரும்பு start;
        }
    }

    திரும்பு 0;
}

BOOT_CODE நிலையான paddr_t load_boot_module(word_t boot_module_start, word_t boot_module_end, paddr_t load_paddr)
{
    v_region_t v_reg;
    word_t entry;
    Elf_Header_t *elf_file = (Elf_Header_t *)boot_module_start;

    எனில் (!elf_checkFile(elf_file, boot_module_end - boot_module_start)) {
        printf("Boot module does not contain a valid ELF image\n");
        திரும்பு 0;
    }

    v_reg = elf_getMemoryBounds(elf_file);
    entry = elf_file->e_entry;

    எனில் (v_reg.end == 0) {
        printf("ELF image in boot module does not contain any segments\n");
        திரும்பு 0;
    }
    v_reg.end = ROUND_UP(v_reg.end, PAGE_BITS);

    printf("size=0x%lx v_entry=%p v_start=%p v_end=%p ",
           v_reg.end - v_reg.start,
           (வெறுமை *)entry,
           (வெறுமை *)v_reg.start,
           (வெறுமை *)v_reg.end
          );

    எனில் (!IS_ALIGNED(v_reg.start, PAGE_BITS)) {
        printf("Userland image virtual start address must be 4KB-aligned\n");
        திரும்பு 0;
    }
    எனில் (v_reg.end + 2 * BIT(PAGE_BITS) > USER_உச்சி) {
        /* for IPC buffer frame and bootinfo frame, need 2*4K of additional userland virtual memory */
        printf("Userland image virtual end address too high\n");
        திரும்பு 0;
    }
    எனில் ((entry < v_reg.start) || (entry >= v_reg.end)) {
        printf("Userland image entry point does not lie within userland image\n");
        திரும்பு 0;
    }

    load_paddr = find_load_paddr(load_paddr, v_reg.end - v_reg.start);
    ப்ரதிஜ்ஞா(load_paddr);

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
        (வெறுமை *)boot_state.ui_info.p_reg.start,
        boot_state.ui_info.p_reg.end - boot_state.ui_info.p_reg.start
    );
    elf_load(elf_file, boot_state.ui_info.pv_offset);

    திரும்பு load_paddr;
}

நிலையான BOOT_CODE bool_t try_boot_sys_முனை(cpu_id_t cpu_id)
{
    எனில் (!map_kernel_window(
            boot_state.இலக்கம்_ioapic,
            boot_state.ioapic_paddr,
            boot_state.இலக்கம்_drhu,
            boot_state.drhu_list
        )) {
        திரும்பு false;
    }
    set_Current_VSpace_வேர்(kpptr_to_paddr(X86_KERNEL_VSPACE_வேர்), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm நிலையற்ற("" ::: "memory");

#ifdef CONFIG_KERNEL_SKIM_WINDOW
    எனில் (!map_skim_window((vptr_t)ki_skim_start, (vptr_t)ki_skim_end)) {
        திரும்பு false;
    }
#endif

    /* initialise the CPU */
    எனில் (!init_cpu(config_set(CONFIG_IRQ_IOAPIC) ? 1 : 0)) {
        திரும்பு false;
    }

    /* initialise NDKS and kernel heap */
    எனில் (!init_sys_state(
            cpu_id,
            &boot_state.mem_p_regs,
            boot_state.ui_info,
            /* parameters below not modelled in abstract specification */
            boot_state.இலக்கம்_drhu,
            boot_state.drhu_list,
            &boot_state.rmrr_list,
            &boot_state.acpi_rsdp,
            &boot_state.vbe_info,
            &boot_state.mb_mmap_info,
            &boot_state.fb_info
        )) {
        திரும்பு false;
    }

    திரும்பு true;
}

நிலையான BOOT_CODE bool_t add_mem_p_regs(p_region_t reg)
{
    எனில் (reg.start == reg.end) {
        // Return true here if asked to add an empty region.
        // Some of the callers round down the end address to
        திரும்பு true;
    }
    எனில் (reg.end > PADDR_உச்சி && reg.start >= PADDR_உச்சி) {
        /* Return true here as it's not an error for there to exist memory outside the kernel window,
         * we're just going to ignore it and leave it to be given out as device memory */
        திரும்பு true;
    }
    எனில் (boot_state.mem_p_regs.மொத்தம் == MAX_இலக்கம்_FREEMEM_REG) {
        printf("Dropping memory region 0x%lx-0x%lx, try increasing MAX_NUM_FREEMEM_REG\n", reg.start, reg.end);
        திரும்பு false;
    }
    எனில் (reg.end > PADDR_உச்சி) {
        ப்ரதிஜ்ஞா(reg.start < PADDR_உச்சி);
        /* Clamp a region to the top of the kernel window if it extends beyond */
        reg.end = PADDR_உச்சி;
    }
    printf("Adding physical memory region 0x%lx-0x%lx\n", reg.start, reg.end);
    boot_state.mem_p_regs.list[boot_state.mem_p_regs.மொத்தம்] = reg;
    boot_state.mem_p_regs.மொத்தம்++;
    திரும்பு true;
}

/*
 * the code relies that the GRUB provides correct information
 * about the actual physical memory regions.
 */
நிலையான BOOT_CODE bool_t parse_mem_map(uint32_t mmap_length, uint32_t mmap_addr)
{
    multiboot_mmap_t *mmap = (multiboot_mmap_t *)((word_t)mmap_addr);
    printf("Parsing GRUB physical memory map\n");

    போது ((word_t)mmap < (word_t)(mmap_addr + mmap_length)) {
        uint64_t mem_start = mmap->base_addr;
        uint64_t mem_length = mmap->length;
        uint32_t type = mmap->type;
        எனில் (mem_start != (uint64_t)(word_t)mem_start) {
            printf("\tPhysical memory region not addressable\n");
        } இல்லையெனில் {
            printf("\tPhysical Memory Region from %lx size %lx type %d\n", (long)mem_start, (long)mem_length, type);
            எனில் (type == MULTIBOOT_MMAP_USEABLE_TYPE && mem_start >= HIGHMEM_PADDR && mem_length >= BIT(PAGE_BITS)) {

                எனில் (!add_mem_p_regs((p_region_t) {
                ROUND_UP(mem_start, PAGE_BITS), ROUND_DOWN(mem_start + mem_length, PAGE_BITS),
                })) {
                    திரும்பு false;
                }
            }
        }
        mmap++;
    }
    திரும்பு true;
}

நிலையான BOOT_CODE bool_t is_compiled_for_microarchitecture(வெறுமை)
{
    word_t microarch_generation = 0;
    x86_cpu_identity_t *model_info = x86_cpuid_get_model_info();

    எனில் (config_set(CONFIG_ARCH_X86_SKYLAKE)) {
        microarch_generation = 7;
    } இல்லையெனில் எனில் (config_set(CONFIG_ARCH_X86_BROADWELL)) {
        microarch_generation = 6;
    } இல்லையெனில் எனில் (config_set(CONFIG_ARCH_X86_HASWELL)) {
        microarch_generation = 5;
    } இல்லையெனில் எனில் (config_set(CONFIG_ARCH_X86_IVY)) {
        microarch_generation = 4;
    } இல்லையெனில் எனில் (config_set(CONFIG_ARCH_X86_SANDY)) {
        microarch_generation = 3;
    } இல்லையெனில் எனில் (config_set(CONFIG_ARCH_X86_WESTMERE)) {
        microarch_generation = 2;
    } இல்லையெனில் எனில் (config_set(CONFIG_ARCH_X86_NEHALEM)) {
        microarch_generation = 1;
    }

    தேர்வு (model_info->model) {
    நிலை SKYLAKE_1_MODEL_ID:
    நிலை SKYLAKE_2_MODEL_ID:
    நிலை SKYLAKE_X_MODEL_ID:
        எனில் (microarch_generation > 7) {
            திரும்பு false;
        }
        நிறுத்து;

    நிலை BROADWELL_1_MODEL_ID:
    நிலை BROADWELL_2_MODEL_ID:
    நிலை BROADWELL_3_MODEL_ID:
    நிலை BROADWELL_4_MODEL_ID:
    நிலை BROADWELL_5_MODEL_ID:
        எனில் (microarch_generation > 6) {
            திரும்பு false;
        }
        நிறுத்து;

    நிலை HASWELL_1_MODEL_ID:
    நிலை HASWELL_2_MODEL_ID:
    நிலை HASWELL_3_MODEL_ID:
    நிலை HASWELL_4_MODEL_ID:
        எனில் (microarch_generation > 5) {
            திரும்பு false;
        }
        நிறுத்து;

    நிலை IVY_BRIDGE_1_MODEL_ID:
    நிலை IVY_BRIDGE_2_MODEL_ID:
    நிலை IVY_BRIDGE_3_MODEL_ID:
        எனில் (microarch_generation > 4) {
            திரும்பு false;
        }
        நிறுத்து;

    நிலை SANDY_BRIDGE_1_MODEL_ID:
    நிலை SANDY_BRIDGE_2_MODEL_ID:
        எனில் (microarch_generation > 3) {
            திரும்பு false;
        }
        நிறுத்து;

    நிலை WESTMERE_1_MODEL_ID:
    நிலை WESTMERE_2_MODEL_ID:
    நிலை WESTMERE_3_MODEL_ID:
        எனில் (microarch_generation > 2) {
            திரும்பு false;
        }
        நிறுத்து;

    நிலை NEHALEM_1_MODEL_ID:
    நிலை NEHALEM_2_MODEL_ID:
    நிலை NEHALEM_3_MODEL_ID:
        எனில் (microarch_generation > 1) {
            திரும்பு false;
        }
        நிறுத்து;

    இயல்பு:
        எனில் (!config_set(CONFIG_ARCH_X86_GENERIC)) {
            திரும்பு false;
        }
    }

    திரும்பு true;
}

நிலையான BOOT_CODE bool_t try_boot_sys(வெறுமை)
{
    paddr_t mods_end_paddr = boot_state.mods_end_paddr;
    p_region_t ui_p_regs;
    paddr_t load_paddr;

    boot_state.ki_p_reg = get_p_reg_kernel_img();

    எனில் (!x86_cpuid_initialize()) {
        printf("Warning: Your x86 CPU has an unsupported vendor, '%s'.\n"
               "\tYour setup may not be able to competently run seL4 as "
               "intended.\n"
               "\tCurrently supported x86 vendors are AMD and Intel.\n",
               x86_cpuid_get_identity()->vendor_string);
    }

    எனில் (!is_compiled_for_microarchitecture()) {
        printf("Warning: Your kernel was not compiled for the current microarchitecture.\n");
    }

    cpuid_007h_edx_t edx;
    edx.சொற்கள்[0] = x86_cpuid_edx(0x7, 0);
    /* see if we can definitively say whether or not we need the skim window by
     * checking whether the CPU is vulnerable to rogue data cache loads (rdcl) */
    எனில் (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.சொற்கள்[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        எனில் (ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && config_set(CONFIG_KERNEL_SKIM_WINDOW)) {
            printf("CPU reports not vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is enabled. Performance is needlessly being impacted, consider disabling.\n");
        } இல்லையெனில் எனில் (!ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && !config_set(CONFIG_KERNEL_SKIM_WINDOW)) {
            printf("CPU reports vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is *not* enabled. Please re-build with SKIM window enabled.");
            திரும்பு false;
        }
    } இல்லையெனில் {
        /* hardware doesn't tell us directly so guess based on CPU vendor */
        எனில் (config_set(CONFIG_KERNEL_SKIM_WINDOW) && x86_cpuid_get_identity()->vendor == X86_VENDOR_AMD) {
            printf("SKIM window for mitigating Meltdown (https://www.meltdownattack.com) "
                   "not necessary for AMD and performance is being needlessly affected, "
                   "consider disabling\n");
        }
        எனில் (!config_set(CONFIG_KERNEL_SKIM_WINDOW) && x86_cpuid_get_identity()->vendor == X86_VENDOR_INTEL) {
            printf("***WARNING*** SKIM window not enabled, this machine is probably vulnerable "
                   "to Meltdown (https://www.meltdownattack.com), consider enabling\n");
        }
    }

#ifdef ENABLE_SMP_SUPPORT
    /* copy boot code for APs to lower memory to run in real mode */
    எனில் (!copy_boot_code_aps(boot_state.mem_lower)) {
        திரும்பு false;
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
    எனில் (config_set(CONFIG_IRQ_IOAPIC)) {
        /* Disable the PIC so that it does not generate any interrupts. We need to
         * do this *before* we initialize the apic */
        pic_disable();
    }

    /* validate the ACPI table */
    எனில் (!acpi_validate_rsdp(&boot_state.acpi_rsdp)) {
        திரும்பு false;
    }

    /* check if kernel configuration matches platform requirements */
    எனில் (!acpi_fadt_scan(&boot_state.acpi_rsdp)) {
        திரும்பு false;
    }

    எனில் (!config_set(CONFIG_IOMMU) || cmdline_opt.disable_iommu) {
        boot_state.இலக்கம்_drhu = 0;
    } இல்லையெனில் {
        /* query available IOMMUs from ACPI */
        acpi_dmar_scan(
            &boot_state.acpi_rsdp,
            boot_state.drhu_list,
            &boot_state.இலக்கம்_drhu,
            MAX_இலக்கம்_DRHU,
            &boot_state.rmrr_list
        );
    }

    /* query available CPUs from ACPI */
    boot_state.இலக்கம்_cpus = acpi_madt_scan(&boot_state.acpi_rsdp, boot_state.cpus, &boot_state.இலக்கம்_ioapic,
                                         boot_state.ioapic_paddr);
    எனில் (boot_state.இலக்கம்_cpus == 0) {
        printf("No CPUs detected\n");
        திரும்பு false;
    }

    எனில் (config_set(CONFIG_IRQ_IOAPIC)) {
        எனில் (boot_state.இலக்கம்_ioapic == 0) {
            printf("No IOAPICs detected\n");
            திரும்பு false;
        }
    } இல்லையெனில் {
        எனில் (boot_state.இலக்கம்_ioapic > 0) {
            printf("Detected %d IOAPICs, but configured to use PIC instead\n", boot_state.இலக்கம்_ioapic);
        }
    }

    mods_end_paddr = ROUND_UP(mods_end_paddr, PAGE_BITS);
    ப்ரதிஜ்ஞா(mods_end_paddr > boot_state.ki_p_reg.end);

    printf("ELF-loading userland images from boot modules:\n");
    load_paddr = mods_end_paddr;

    load_paddr = load_boot_module(boot_state.boot_module_start, boot_state.boot_module_end, load_paddr);
    எனில் (!load_paddr) {
        திரும்பு false;
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
    memcpy((வெறுமை *)ui_p_regs.start, (வெறுமை *)mods_end_paddr, ui_p_regs.end - ui_p_regs.start);

    /* adjust p_reg and pv_offset to final load address */
    boot_state.ui_info.p_reg.start -= mods_end_paddr - ui_p_regs.start;
    boot_state.ui_info.p_reg.end   -= mods_end_paddr - ui_p_regs.start;
    boot_state.ui_info.pv_offset   -= mods_end_paddr - ui_p_regs.start;

    /* ==== following code corresponds to abstract specification after "select" ==== */

    எனில் (!platAddDevices()) {
        திரும்பு false;
    }

    /* Total number of cores we intend to boot */
    ks_இலக்கம்_CPUs = boot_state.இலக்கம்_cpus;

    printf("Starting node #0 with APIC ID %lu\n", boot_state.cpus[0]);
    எனில் (!try_boot_sys_முனை(boot_state.cpus[0])) {
        திரும்பு false;
    }

    எனில் (config_set(CONFIG_IRQ_IOAPIC)) {
        ioapic_init(1, boot_state.cpus, boot_state.இலக்கம்_ioapic);
    }

    /* initialize BKL before booting up APs */
    SMP_COND_STATEMENT(clh_lock_init());
    SMP_COND_STATEMENT(start_boot_aps());

    /* grab BKL before leaving the kernel */
    முனை_LOCK_SYS;

    printf("Booting all finished, dropped to user space\n");

    திரும்பு true;
}

நிலையான BOOT_CODE bool_t try_boot_sys_mbi1(
    multiboot_info_t *mbi
)
{
    word_t i;
    multiboot_module_t *modules = (multiboot_module_t *)(word_t)mbi->part1.mod_list;

    cmdline_parse((மாறிலி எழுத்து *)(word_t)mbi->part1.cmdline, &cmdline_opt);

    எனில் ((mbi->part1.flags & MULTIBOOT_INFO_MEM_FLAG) == 0) {
        printf("Boot loader did not provide information about physical memory size\n");
        திரும்பு false;
    }

    எனில் (!(mbi->part1.flags & MULTIBOOT_INFO_MODS_FLAG)) {
        printf("Boot loader did not provide information about boot modules\n");
        திரும்பு false;
    }

    printf("Detected %d boot module(s):\n", mbi->part1.mod_மொத்தம்);

    எனில் (mbi->part1.mod_மொத்தம் < 1) {
        printf("Expect at least one boot module (containing a userland image)\n");
        திரும்பு false;
    }

    க்கு (i = 0; i < mbi->part1.mod_மொத்தம்; i++) {
        printf(
            "  module #%ld: start=0x%x end=0x%x size=0x%x name='%s'\n",
            i,
            modules[i].start,
            modules[i].end,
            modules[i].end - modules[i].start,
            (எழுத்து *)(long)modules[i].name
        );
        எனில் ((sword_t)(modules[i].end - modules[i].start) <= 0) {
            printf("Invalid boot module size! Possible cause: boot module file not found by QEMU\n");
            திரும்பு false;
        }
        எனில் (boot_state.mods_end_paddr < modules[i].end) {
            boot_state.mods_end_paddr = modules[i].end;
        }
    }

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.மொத்தம் = 0;
    எனில் (mbi->part1.flags & MULTIBOOT_INFO_MMAP_FLAG) {
        எனில் (!parse_mem_map(mbi->part2.mmap_length, mbi->part2.mmap_addr)) {
            திரும்பு false;
        }
        uint32_t multiboot_mmap_length = mbi->part2.mmap_length;
        எனில் (multiboot_mmap_length > (SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * அளவு(seL4_X86_mb_mmap_t))) {
            printf("Warning: Multiboot has reported more memory map entries, %zd, "
                   "than the max amount that will be passed in the bootinfo, %d. "
                   "These extra regions will still be turned into untyped caps.",
                   multiboot_mmap_length / அளவு(seL4_X86_mb_mmap_t), SEL4_MULTIBOOT_MAX_MMAP_ENTRIES);
            multiboot_mmap_length = SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * அளவு(seL4_X86_mb_mmap_t);
        }
        memcpy(&boot_state.mb_mmap_info.mmap, (வெறுமை *)(word_t)mbi->part2.mmap_addr, multiboot_mmap_length);
        boot_state.mb_mmap_info.mmap_length = multiboot_mmap_length;
    } இல்லையெனில் {
        /* calculate memory the old way */
        p_region_t avail;
        avail.start = HIGHMEM_PADDR;
        avail.end = ROUND_DOWN(avail.start + (mbi->part1.mem_upper << 10), PAGE_BITS);
        எனில் (!add_mem_p_regs(avail)) {
            திரும்பு false;
        }
    }

    /* copy VESA information from multiboot header */
    எனில் ((mbi->part1.flags & MULTIBOOT_INFO_GRAPHICS_FLAG) == 0) {
        boot_state.vbe_info.vbeMode = -1;
        printf("Multiboot gave us no video information\n");
    } இல்லையெனில் {
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
    எனில் (!acpi_init(&boot_state.acpi_rsdp)) {
        திரும்பு false;
    }

    திரும்பு true;
}

நிலையான BOOT_CODE bool_t try_boot_sys_mbi2(
    multiboot2_header_t *mbi2
)
{
    முழு mod_மொத்தம்                  = 0;
    multiboot2_tag_t மாறிலி *tag   = (multiboot2_tag_t *)(mbi2 + 1);
    multiboot2_tag_t மாறிலி *tag_e = (multiboot2_tag_t *)((word_t)mbi2 + mbi2->total_size);

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.மொத்தம் = 0;
    boot_state.mb_mmap_info.mmap_length = 0;
    boot_state.vbe_info.vbeMode = -1;

    போது (tag < tag_e && tag->type != MULTIBOOT2_TAG_END) {
        word_t மாறிலி behind_tag = (word_t)tag + அளவு(*tag);

        எனில் (tag->type == MULTIBOOT2_TAG_CMDLINE) {
            எழுத்து மாறிலி *மாறிலி cmdline = (எழுத்து மாறிலி * மாறிலி)(behind_tag);
            cmdline_parse(cmdline, &cmdline_opt);
        } இல்லையெனில் எனில் (tag->type == MULTIBOOT2_TAG_ACPI_1) {
            எனில் (ACPI_V1_SIZE == tag->size - அளவு(*tag)) {
                memcpy(&boot_state.acpi_rsdp, (வெறுமை *)behind_tag, tag->size - அளவு(*tag));
            }
        } இல்லையெனில் எனில் (tag->type == MULTIBOOT2_TAG_ACPI_2) {
            எனில் (அளவு(boot_state.acpi_rsdp) == tag->size - அளவு(*tag)) {
                memcpy(&boot_state.acpi_rsdp, (வெறுமை *)behind_tag, அளவு(boot_state.acpi_rsdp));
            }
        } இல்லையெனில் எனில் (tag->type == MULTIBOOT2_TAG_MODULE) {
            multiboot2_module_t மாறிலி *module = (multiboot2_module_t மாறிலி *)behind_tag;
            printf(
                "  module #%d: start=0x%x end=0x%x size=0x%x name='%s'\n",
                mod_மொத்தம்,
                module->start,
                module->end,
                module->end - module->start,
                module->string
            );

            எனில் (mod_மொத்தம் == 0) {
                boot_state.boot_module_start = module->start;
                boot_state.boot_module_end = module->end;
            }

            mod_மொத்தம் ++;
            எனில் ((sword_t)(module->end - module->start) <= 0) {
                printf("Invalid boot module size! Possible cause: boot module file not found\n");
                திரும்பு false;
            }
            எனில் (boot_state.mods_end_paddr < module->end) {
                boot_state.mods_end_paddr = module->end;
            }
        } இல்லையெனில் எனில் (tag->type == MULTIBOOT2_TAG_MEMORY) {
            multiboot2_memory_t மாறிலி *s = (multiboot2_memory_t *)(behind_tag + 8);
            multiboot2_memory_t மாறிலி *e = (multiboot2_memory_t *)((word_t)tag + tag->size);

            க்கு (multiboot2_memory_t மாறிலி *m = s; m < e; m++) {
                எனில் (!m->addr) {
                    boot_state.mem_lower = m->size;
                }

                printf("\tPhysical Memory Region from %llx size %llx type %u\n", m->addr, m->size, m->type);
                எனில் (m->addr != (uint64_t)(word_t)m->addr) {
                    printf("\t\tPhysical memory region not addressable\n");
                    தொடர்;
                }

                எனில் (m->type == MULTIBOOT_MMAP_USEABLE_TYPE && m->addr >= HIGHMEM_PADDR) {
                    எனில் (!add_mem_p_regs((p_region_t) {
                    m->addr, m->addr + m->size
                }))
                    திரும்பு false;
                }
            }
        } இல்லையெனில் எனில் (tag->type == MULTIBOOT2_TAG_FB) {
            multiboot2_fb_t மாறிலி *fb = (multiboot2_fb_t மாறிலி *)behind_tag;
            printf("Got framebuffer info in multiboot2. Current video mode is at physical address=%llx pitch=%u resolution=%ux%u@%u type=%u\n",
                   fb->addr, fb->pitch, fb->width, fb->height, fb->bpp, fb->type);
            boot_state.fb_info = *fb;
        }

        tag = (multiboot2_tag_t மாறிலி *)((word_t)tag + ROUND_UP(tag->size, 3));
    }

    printf("Detected %d boot module(s):\n", mod_மொத்தம்);

    எனில் (mod_மொத்தம் < 1) {
        printf("Expect at least one boot module (containing a userland image)\n");
        திரும்பு false;
    }

    திரும்பு true;
}

BOOT_CODE VISIBLE வெறுமை boot_sys(
    unsigned long multiboot_magic,
    வெறுமை *mbi)
{
    bool_t result = false;

    எனில் (multiboot_magic == MULTIBOOT_MAGIC) {
        result = try_boot_sys_mbi1(mbi);
    } இல்லையெனில் எனில் (multiboot_magic == MULTIBOOT2_MAGIC) {
        result = try_boot_sys_mbi2(mbi);
    } இல்லையெனில் {
        printf("Boot loader is not multiboot 1 or 2 compliant %lx\n", multiboot_magic);
    }

    எனில் (result) {
        result = try_boot_sys();
    }

    எனில் (!result) {
        fail("boot_sys failed for some reason :(\n");
    }

    ARCH_முனை_STATE(x86KScurInterrupt) = int_invalid;
    ARCH_முனை_STATE(x86KSPendingInterrupt) = int_invalid;

#ifdef CONFIG_KERNEL_MCS
    முனை_STATE(ksCurTime) = getCurrentTime();
    முனை_STATE(ksConsumed) = 0;
#endif

    schedule();
    activateThread();
}

