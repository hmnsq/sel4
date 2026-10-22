/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <hardware.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/kernel/apic.h>
#अन्तर्भाव <arch/kernel/cmdline.h>
#अन्तर्भाव <arch/kernel/boot.h>
#अन्तर्भाव <arch/kernel/boot_sys.h>
#अन्तर्भाव <arch/kernel/smp_sys.h>
#अन्तर्भाव <arch/kernel/vspace.h>
#अन्तर्भाव <arch/kernel/elf.h>
#अन्तर्भाव <smp/lock.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/acpi.h>
#अन्तर्भाव <plat/machine/devices.h>
#अन्तर्भाव <plat/machine/pic.h>
#अन्तर्भाव <plat/machine/ioapic.h>
#अन्तर्भाव <sel4/arch/bootinfo_types.h>

/* addresses defined in linker script */
/* need a fake array to get the pointer from the linker script */

/* start/end of CPU boot code */
बाह्य वर्ण boot_cpu_start[1];
बाह्य वर्ण boot_cpu_end[1];

/* start/end of boot stack */
बाह्य वर्ण boot_चळत_bottom[1];
बाह्य वर्ण boot_चळत_शीर्ष[1];

/* locations in kernel image */
बाह्य वर्ण ki_skim_start[1];
बाह्य वर्ण ki_skim_end[1];

#ifdef CONFIG_PRINTING
/* kernel entry point */
बाह्य वर्ण _start[1];
#endif

/* constants */

#define HIGHMEM_PADDR 0x100000

BOOT_BSS
boot_state_t boot_state;

/* global variables (not covered by abstract specification) */

BOOT_BSS
cmdline_opt_t cmdline_opt;

/* functions not modeled in abstract specification */

BOOT_CODE स्थिर paddr_t find_load_paddr(paddr_t min_paddr, word_t image_size)
{
    पूर्णाङ्क i;

    प्रति (i = 0; i < boot_state.mem_p_regs.मोजणी; i++) {
        paddr_t start = MAX(min_paddr, boot_state.mem_p_regs.list[i].start);
        paddr_t end = boot_state.mem_p_regs.list[i].end;
        word_t region_size = end - start;

        यदि (region_size >= image_size) {
            प्रत्यागम start;
        }
    }

    प्रत्यागम 0;
}

BOOT_CODE स्थिर paddr_t load_boot_module(word_t boot_module_start, word_t boot_module_end, paddr_t load_paddr)
{
    v_region_t v_reg;
    word_t entry;
    Elf_Header_t *elf_file = (Elf_Header_t *)boot_module_start;

    यदि (!elf_checkFile(elf_file, boot_module_end - boot_module_start)) {
        printf("Boot module does not contain a valid ELF image\n");
        प्रत्यागम 0;
    }

    v_reg = elf_getMemoryBounds(elf_file);
    entry = elf_file->e_entry;

    यदि (v_reg.end == 0) {
        printf("ELF image in boot module does not contain any segments\n");
        प्रत्यागम 0;
    }
    v_reg.end = ROUND_UP(v_reg.end, PAGE_BITS);

    printf("size=0x%lx v_entry=%p v_start=%p v_end=%p ",
           v_reg.end - v_reg.start,
           (शून्य *)entry,
           (शून्य *)v_reg.start,
           (शून्य *)v_reg.end
          );

    यदि (!IS_ALIGNED(v_reg.start, PAGE_BITS)) {
        printf("Userland image virtual start address must be 4KB-aligned\n");
        प्रत्यागम 0;
    }
    यदि (v_reg.end + 2 * BIT(PAGE_BITS) > USER_शीर्ष) {
        /* for IPC buffer frame and bootinfo frame, need 2*4K of additional userland virtual memory */
        printf("Userland image virtual end address too high\n");
        प्रत्यागम 0;
    }
    यदि ((entry < v_reg.start) || (entry >= v_reg.end)) {
        printf("Userland image entry point does not lie within userland image\n");
        प्रत्यागम 0;
    }

    load_paddr = find_load_paddr(load_paddr, v_reg.end - v_reg.start);
    प्रतिज्ञा(load_paddr);

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
        (शून्य *)boot_state.ui_info.p_reg.start,
        boot_state.ui_info.p_reg.end - boot_state.ui_info.p_reg.start
    );
    elf_load(elf_file, boot_state.ui_info.pv_offset);

    प्रत्यागम load_paddr;
}

स्थिर BOOT_CODE bool_t try_boot_sys_गाठ(cpu_id_t cpu_id)
{
    यदि (!map_kernel_window(
            boot_state.अंक_ioapic,
            boot_state.ioapic_paddr,
            boot_state.अंक_drhu,
            boot_state.drhu_list
        )) {
        प्रत्यागम false;
    }
    set_Current_VSpace_मूळ(kpptr_to_paddr(X86_KERNEL_VSPACE_मूळ), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm चञ्चल("" ::: "memory");

#ifdef CONFIG_KERNEL_SKIM_WINDOW
    यदि (!map_skim_window((vptr_t)ki_skim_start, (vptr_t)ki_skim_end)) {
        प्रत्यागम false;
    }
#endif

    /* initialise the CPU */
    यदि (!init_cpu(config_set(CONFIG_IRQ_IOAPIC) ? 1 : 0)) {
        प्रत्यागम false;
    }

    /* initialise NDKS and kernel heap */
    यदि (!init_sys_state(
            cpu_id,
            &boot_state.mem_p_regs,
            boot_state.ui_info,
            /* parameters below not modelled in abstract specification */
            boot_state.अंक_drhu,
            boot_state.drhu_list,
            &boot_state.rmrr_list,
            &boot_state.acpi_rsdp,
            &boot_state.vbe_info,
            &boot_state.mb_mmap_info,
            &boot_state.fb_info
        )) {
        प्रत्यागम false;
    }

    प्रत्यागम true;
}

स्थिर BOOT_CODE bool_t add_mem_p_regs(p_region_t reg)
{
    यदि (reg.start == reg.end) {
        // Return true here if asked to add an empty region.
        // Some of the callers round down the end address to
        प्रत्यागम true;
    }
    यदि (reg.end > PADDR_शीर्ष && reg.start >= PADDR_शीर्ष) {
        /* Return true here as it's not an error for there to exist memory outside the kernel window,
         * we're just going to ignore it and leave it to be given out as device memory */
        प्रत्यागम true;
    }
    यदि (boot_state.mem_p_regs.मोजणी == MAX_अंक_FREEMEM_REG) {
        printf("Dropping memory region 0x%lx-0x%lx, try increasing MAX_NUM_FREEMEM_REG\n", reg.start, reg.end);
        प्रत्यागम false;
    }
    यदि (reg.end > PADDR_शीर्ष) {
        प्रतिज्ञा(reg.start < PADDR_शीर्ष);
        /* Clamp a region to the top of the kernel window if it extends beyond */
        reg.end = PADDR_शीर्ष;
    }
    printf("Adding physical memory region 0x%lx-0x%lx\n", reg.start, reg.end);
    boot_state.mem_p_regs.list[boot_state.mem_p_regs.मोजणी] = reg;
    boot_state.mem_p_regs.मोजणी++;
    प्रत्यागम true;
}

/*
 * the code relies that the GRUB provides correct information
 * about the actual physical memory regions.
 */
स्थिर BOOT_CODE bool_t parse_mem_map(uint32_t mmap_length, uint32_t mmap_addr)
{
    multiboot_mmap_t *mmap = (multiboot_mmap_t *)((word_t)mmap_addr);
    printf("Parsing GRUB physical memory map\n");

    यावत् ((word_t)mmap < (word_t)(mmap_addr + mmap_length)) {
        uint64_t mem_start = mmap->base_addr;
        uint64_t mem_length = mmap->length;
        uint32_t type = mmap->type;
        यदि (mem_start != (uint64_t)(word_t)mem_start) {
            printf("\tPhysical memory region not addressable\n");
        } अन्यथा {
            printf("\tPhysical Memory Region from %lx size %lx type %d\n", (long)mem_start, (long)mem_length, type);
            यदि (type == MULTIBOOT_MMAP_USEABLE_TYPE && mem_start >= HIGHMEM_PADDR && mem_length >= BIT(PAGE_BITS)) {

                यदि (!add_mem_p_regs((p_region_t) {
                ROUND_UP(mem_start, PAGE_BITS), ROUND_DOWN(mem_start + mem_length, PAGE_BITS),
                })) {
                    प्रत्यागम false;
                }
            }
        }
        mmap++;
    }
    प्रत्यागम true;
}

स्थिर BOOT_CODE bool_t is_compiled_for_microarchitecture(शून्य)
{
    word_t microarch_generation = 0;
    x86_cpu_identity_t *model_info = x86_cpuid_get_model_info();

    यदि (config_set(CONFIG_ARCH_X86_SKYLAKE)) {
        microarch_generation = 7;
    } अन्यथा यदि (config_set(CONFIG_ARCH_X86_BROADWELL)) {
        microarch_generation = 6;
    } अन्यथा यदि (config_set(CONFIG_ARCH_X86_HASWELL)) {
        microarch_generation = 5;
    } अन्यथा यदि (config_set(CONFIG_ARCH_X86_IVY)) {
        microarch_generation = 4;
    } अन्यथा यदि (config_set(CONFIG_ARCH_X86_SANDY)) {
        microarch_generation = 3;
    } अन्यथा यदि (config_set(CONFIG_ARCH_X86_WESTMERE)) {
        microarch_generation = 2;
    } अन्यथा यदि (config_set(CONFIG_ARCH_X86_NEHALEM)) {
        microarch_generation = 1;
    }

    विकल्प (model_info->model) {
    पक्ष SKYLAKE_1_MODEL_ID:
    पक्ष SKYLAKE_2_MODEL_ID:
    पक्ष SKYLAKE_X_MODEL_ID:
        यदि (microarch_generation > 7) {
            प्रत्यागम false;
        }
        भङ्ग;

    पक्ष BROADWELL_1_MODEL_ID:
    पक्ष BROADWELL_2_MODEL_ID:
    पक्ष BROADWELL_3_MODEL_ID:
    पक्ष BROADWELL_4_MODEL_ID:
    पक्ष BROADWELL_5_MODEL_ID:
        यदि (microarch_generation > 6) {
            प्रत्यागम false;
        }
        भङ्ग;

    पक्ष HASWELL_1_MODEL_ID:
    पक्ष HASWELL_2_MODEL_ID:
    पक्ष HASWELL_3_MODEL_ID:
    पक्ष HASWELL_4_MODEL_ID:
        यदि (microarch_generation > 5) {
            प्रत्यागम false;
        }
        भङ्ग;

    पक्ष IVY_BRIDGE_1_MODEL_ID:
    पक्ष IVY_BRIDGE_2_MODEL_ID:
    पक्ष IVY_BRIDGE_3_MODEL_ID:
        यदि (microarch_generation > 4) {
            प्रत्यागम false;
        }
        भङ्ग;

    पक्ष SANDY_BRIDGE_1_MODEL_ID:
    पक्ष SANDY_BRIDGE_2_MODEL_ID:
        यदि (microarch_generation > 3) {
            प्रत्यागम false;
        }
        भङ्ग;

    पक्ष WESTMERE_1_MODEL_ID:
    पक्ष WESTMERE_2_MODEL_ID:
    पक्ष WESTMERE_3_MODEL_ID:
        यदि (microarch_generation > 2) {
            प्रत्यागम false;
        }
        भङ्ग;

    पक्ष NEHALEM_1_MODEL_ID:
    पक्ष NEHALEM_2_MODEL_ID:
    पक्ष NEHALEM_3_MODEL_ID:
        यदि (microarch_generation > 1) {
            प्रत्यागम false;
        }
        भङ्ग;

    स्वभाव:
        यदि (!config_set(CONFIG_ARCH_X86_GENERIC)) {
            प्रत्यागम false;
        }
    }

    प्रत्यागम true;
}

स्थिर BOOT_CODE bool_t try_boot_sys(शून्य)
{
    paddr_t mods_end_paddr = boot_state.mods_end_paddr;
    p_region_t ui_p_regs;
    paddr_t load_paddr;

    boot_state.ki_p_reg = get_p_reg_kernel_img();

    यदि (!x86_cpuid_initialize()) {
        printf("Warning: Your x86 CPU has an unsupported vendor, '%s'.\n"
               "\tYour setup may not be able to competently run seL4 as "
               "intended.\n"
               "\tCurrently supported x86 vendors are AMD and Intel.\n",
               x86_cpuid_get_identity()->vendor_string);
    }

    यदि (!is_compiled_for_microarchitecture()) {
        printf("Warning: Your kernel was not compiled for the current microarchitecture.\n");
    }

    cpuid_007h_edx_t edx;
    edx.शब्द[0] = x86_cpuid_edx(0x7, 0);
    /* see if we can definitively say whether or not we need the skim window by
     * checking whether the CPU is vulnerable to rogue data cache loads (rdcl) */
    यदि (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.शब्द[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        यदि (ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && config_set(CONFIG_KERNEL_SKIM_WINDOW)) {
            printf("CPU reports not vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is enabled. Performance is needlessly being impacted, consider disabling.\n");
        } अन्यथा यदि (!ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && !config_set(CONFIG_KERNEL_SKIM_WINDOW)) {
            printf("CPU reports vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is *not* enabled. Please re-build with SKIM window enabled.");
            प्रत्यागम false;
        }
    } अन्यथा {
        /* hardware doesn't tell us directly so guess based on CPU vendor */
        यदि (config_set(CONFIG_KERNEL_SKIM_WINDOW) && x86_cpuid_get_identity()->vendor == X86_VENDOR_AMD) {
            printf("SKIM window for mitigating Meltdown (https://www.meltdownattack.com) "
                   "not necessary for AMD and performance is being needlessly affected, "
                   "consider disabling\n");
        }
        यदि (!config_set(CONFIG_KERNEL_SKIM_WINDOW) && x86_cpuid_get_identity()->vendor == X86_VENDOR_INTEL) {
            printf("***WARNING*** SKIM window not enabled, this machine is probably vulnerable "
                   "to Meltdown (https://www.meltdownattack.com), consider enabling\n");
        }
    }

#ifdef ENABLE_SMP_SUPPORT
    /* copy boot code for APs to lower memory to run in real mode */
    यदि (!copy_boot_code_aps(boot_state.mem_lower)) {
        प्रत्यागम false;
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
    यदि (config_set(CONFIG_IRQ_IOAPIC)) {
        /* Disable the PIC so that it does not generate any interrupts. We need to
         * do this *before* we initialize the apic */
        pic_disable();
    }

    /* validate the ACPI table */
    यदि (!acpi_validate_rsdp(&boot_state.acpi_rsdp)) {
        प्रत्यागम false;
    }

    /* check if kernel configuration matches platform requirements */
    यदि (!acpi_fadt_scan(&boot_state.acpi_rsdp)) {
        प्रत्यागम false;
    }

    यदि (!config_set(CONFIG_IOMMU) || cmdline_opt.disable_iommu) {
        boot_state.अंक_drhu = 0;
    } अन्यथा {
        /* query available IOMMUs from ACPI */
        acpi_dmar_scan(
            &boot_state.acpi_rsdp,
            boot_state.drhu_list,
            &boot_state.अंक_drhu,
            MAX_अंक_DRHU,
            &boot_state.rmrr_list
        );
    }

    /* query available CPUs from ACPI */
    boot_state.अंक_cpus = acpi_madt_scan(&boot_state.acpi_rsdp, boot_state.cpus, &boot_state.अंक_ioapic,
                                         boot_state.ioapic_paddr);
    यदि (boot_state.अंक_cpus == 0) {
        printf("No CPUs detected\n");
        प्रत्यागम false;
    }

    यदि (config_set(CONFIG_IRQ_IOAPIC)) {
        यदि (boot_state.अंक_ioapic == 0) {
            printf("No IOAPICs detected\n");
            प्रत्यागम false;
        }
    } अन्यथा {
        यदि (boot_state.अंक_ioapic > 0) {
            printf("Detected %d IOAPICs, but configured to use PIC instead\n", boot_state.अंक_ioapic);
        }
    }

    mods_end_paddr = ROUND_UP(mods_end_paddr, PAGE_BITS);
    प्रतिज्ञा(mods_end_paddr > boot_state.ki_p_reg.end);

    printf("ELF-loading userland images from boot modules:\n");
    load_paddr = mods_end_paddr;

    load_paddr = load_boot_module(boot_state.boot_module_start, boot_state.boot_module_end, load_paddr);
    यदि (!load_paddr) {
        प्रत्यागम false;
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
    memcpy((शून्य *)ui_p_regs.start, (शून्य *)mods_end_paddr, ui_p_regs.end - ui_p_regs.start);

    /* adjust p_reg and pv_offset to final load address */
    boot_state.ui_info.p_reg.start -= mods_end_paddr - ui_p_regs.start;
    boot_state.ui_info.p_reg.end   -= mods_end_paddr - ui_p_regs.start;
    boot_state.ui_info.pv_offset   -= mods_end_paddr - ui_p_regs.start;

    /* ==== following code corresponds to abstract specification after "select" ==== */

    यदि (!platAddDevices()) {
        प्रत्यागम false;
    }

    /* Total number of cores we intend to boot */
    ks_अंक_CPUs = boot_state.अंक_cpus;

    printf("Starting node #0 with APIC ID %lu\n", boot_state.cpus[0]);
    यदि (!try_boot_sys_गाठ(boot_state.cpus[0])) {
        प्रत्यागम false;
    }

    यदि (config_set(CONFIG_IRQ_IOAPIC)) {
        ioapic_init(1, boot_state.cpus, boot_state.अंक_ioapic);
    }

    /* initialize BKL before booting up APs */
    SMP_COND_STATEMENT(clh_lock_init());
    SMP_COND_STATEMENT(start_boot_aps());

    /* grab BKL before leaving the kernel */
    गाठ_LOCK_SYS;

    printf("Booting all finished, dropped to user space\n");

    प्रत्यागम true;
}

स्थिर BOOT_CODE bool_t try_boot_sys_mbi1(
    multiboot_info_t *mbi
)
{
    word_t i;
    multiboot_module_t *modules = (multiboot_module_t *)(word_t)mbi->part1.mod_list;

    cmdline_parse((ध्रुव वर्ण *)(word_t)mbi->part1.cmdline, &cmdline_opt);

    यदि ((mbi->part1.flags & MULTIBOOT_INFO_MEM_FLAG) == 0) {
        printf("Boot loader did not provide information about physical memory size\n");
        प्रत्यागम false;
    }

    यदि (!(mbi->part1.flags & MULTIBOOT_INFO_MODS_FLAG)) {
        printf("Boot loader did not provide information about boot modules\n");
        प्रत्यागम false;
    }

    printf("Detected %d boot module(s):\n", mbi->part1.mod_मोजणी);

    यदि (mbi->part1.mod_मोजणी < 1) {
        printf("Expect at least one boot module (containing a userland image)\n");
        प्रत्यागम false;
    }

    प्रति (i = 0; i < mbi->part1.mod_मोजणी; i++) {
        printf(
            "  module #%ld: start=0x%x end=0x%x size=0x%x name='%s'\n",
            i,
            modules[i].start,
            modules[i].end,
            modules[i].end - modules[i].start,
            (वर्ण *)(long)modules[i].name
        );
        यदि ((sword_t)(modules[i].end - modules[i].start) <= 0) {
            printf("Invalid boot module size! Possible cause: boot module file not found by QEMU\n");
            प्रत्यागम false;
        }
        यदि (boot_state.mods_end_paddr < modules[i].end) {
            boot_state.mods_end_paddr = modules[i].end;
        }
    }

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.मोजणी = 0;
    यदि (mbi->part1.flags & MULTIBOOT_INFO_MMAP_FLAG) {
        यदि (!parse_mem_map(mbi->part2.mmap_length, mbi->part2.mmap_addr)) {
            प्रत्यागम false;
        }
        uint32_t multiboot_mmap_length = mbi->part2.mmap_length;
        यदि (multiboot_mmap_length > (SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * परिमाण(seL4_X86_mb_mmap_t))) {
            printf("Warning: Multiboot has reported more memory map entries, %zd, "
                   "than the max amount that will be passed in the bootinfo, %d. "
                   "These extra regions will still be turned into untyped caps.",
                   multiboot_mmap_length / परिमाण(seL4_X86_mb_mmap_t), SEL4_MULTIBOOT_MAX_MMAP_ENTRIES);
            multiboot_mmap_length = SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * परिमाण(seL4_X86_mb_mmap_t);
        }
        memcpy(&boot_state.mb_mmap_info.mmap, (शून्य *)(word_t)mbi->part2.mmap_addr, multiboot_mmap_length);
        boot_state.mb_mmap_info.mmap_length = multiboot_mmap_length;
    } अन्यथा {
        /* calculate memory the old way */
        p_region_t avail;
        avail.start = HIGHMEM_PADDR;
        avail.end = ROUND_DOWN(avail.start + (mbi->part1.mem_upper << 10), PAGE_BITS);
        यदि (!add_mem_p_regs(avail)) {
            प्रत्यागम false;
        }
    }

    /* copy VESA information from multiboot header */
    यदि ((mbi->part1.flags & MULTIBOOT_INFO_GRAPHICS_FLAG) == 0) {
        boot_state.vbe_info.vbeMode = -1;
        printf("Multiboot gave us no video information\n");
    } अन्यथा {
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
    यदि (!acpi_init(&boot_state.acpi_rsdp)) {
        प्रत्यागम false;
    }

    प्रत्यागम true;
}

स्थिर BOOT_CODE bool_t try_boot_sys_mbi2(
    multiboot2_header_t *mbi2
)
{
    पूर्णाङ्क mod_मोजणी                  = 0;
    multiboot2_tag_t ध्रुव *tag   = (multiboot2_tag_t *)(mbi2 + 1);
    multiboot2_tag_t ध्रुव *tag_e = (multiboot2_tag_t *)((word_t)mbi2 + mbi2->total_size);

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.मोजणी = 0;
    boot_state.mb_mmap_info.mmap_length = 0;
    boot_state.vbe_info.vbeMode = -1;

    यावत् (tag < tag_e && tag->type != MULTIBOOT2_TAG_END) {
        word_t ध्रुव behind_tag = (word_t)tag + परिमाण(*tag);

        यदि (tag->type == MULTIBOOT2_TAG_CMDLINE) {
            वर्ण ध्रुव *ध्रुव cmdline = (वर्ण ध्रुव * ध्रुव)(behind_tag);
            cmdline_parse(cmdline, &cmdline_opt);
        } अन्यथा यदि (tag->type == MULTIBOOT2_TAG_ACPI_1) {
            यदि (ACPI_V1_SIZE == tag->size - परिमाण(*tag)) {
                memcpy(&boot_state.acpi_rsdp, (शून्य *)behind_tag, tag->size - परिमाण(*tag));
            }
        } अन्यथा यदि (tag->type == MULTIBOOT2_TAG_ACPI_2) {
            यदि (परिमाण(boot_state.acpi_rsdp) == tag->size - परिमाण(*tag)) {
                memcpy(&boot_state.acpi_rsdp, (शून्य *)behind_tag, परिमाण(boot_state.acpi_rsdp));
            }
        } अन्यथा यदि (tag->type == MULTIBOOT2_TAG_MODULE) {
            multiboot2_module_t ध्रुव *module = (multiboot2_module_t ध्रुव *)behind_tag;
            printf(
                "  module #%d: start=0x%x end=0x%x size=0x%x name='%s'\n",
                mod_मोजणी,
                module->start,
                module->end,
                module->end - module->start,
                module->string
            );

            यदि (mod_मोजणी == 0) {
                boot_state.boot_module_start = module->start;
                boot_state.boot_module_end = module->end;
            }

            mod_मोजणी ++;
            यदि ((sword_t)(module->end - module->start) <= 0) {
                printf("Invalid boot module size! Possible cause: boot module file not found\n");
                प्रत्यागम false;
            }
            यदि (boot_state.mods_end_paddr < module->end) {
                boot_state.mods_end_paddr = module->end;
            }
        } अन्यथा यदि (tag->type == MULTIBOOT2_TAG_MEMORY) {
            multiboot2_memory_t ध्रुव *s = (multiboot2_memory_t *)(behind_tag + 8);
            multiboot2_memory_t ध्रुव *e = (multiboot2_memory_t *)((word_t)tag + tag->size);

            प्रति (multiboot2_memory_t ध्रुव *m = s; m < e; m++) {
                यदि (!m->addr) {
                    boot_state.mem_lower = m->size;
                }

                printf("\tPhysical Memory Region from %llx size %llx type %u\n", m->addr, m->size, m->type);
                यदि (m->addr != (uint64_t)(word_t)m->addr) {
                    printf("\t\tPhysical memory region not addressable\n");
                    अनुवर्त;
                }

                यदि (m->type == MULTIBOOT_MMAP_USEABLE_TYPE && m->addr >= HIGHMEM_PADDR) {
                    यदि (!add_mem_p_regs((p_region_t) {
                    m->addr, m->addr + m->size
                }))
                    प्रत्यागम false;
                }
            }
        } अन्यथा यदि (tag->type == MULTIBOOT2_TAG_FB) {
            multiboot2_fb_t ध्रुव *fb = (multiboot2_fb_t ध्रुव *)behind_tag;
            printf("Got framebuffer info in multiboot2. Current video mode is at physical address=%llx pitch=%u resolution=%ux%u@%u type=%u\n",
                   fb->addr, fb->pitch, fb->width, fb->height, fb->bpp, fb->type);
            boot_state.fb_info = *fb;
        }

        tag = (multiboot2_tag_t ध्रुव *)((word_t)tag + ROUND_UP(tag->size, 3));
    }

    printf("Detected %d boot module(s):\n", mod_मोजणी);

    यदि (mod_मोजणी < 1) {
        printf("Expect at least one boot module (containing a userland image)\n");
        प्रत्यागम false;
    }

    प्रत्यागम true;
}

BOOT_CODE VISIBLE शून्य boot_sys(
    unsigned long multiboot_magic,
    शून्य *mbi)
{
    bool_t result = false;

    यदि (multiboot_magic == MULTIBOOT_MAGIC) {
        result = try_boot_sys_mbi1(mbi);
    } अन्यथा यदि (multiboot_magic == MULTIBOOT2_MAGIC) {
        result = try_boot_sys_mbi2(mbi);
    } अन्यथा {
        printf("Boot loader is not multiboot 1 or 2 compliant %lx\n", multiboot_magic);
    }

    यदि (result) {
        result = try_boot_sys();
    }

    यदि (!result) {
        fail("boot_sys failed for some reason :(\n");
    }

    ARCH_गाठ_STATE(x86KScurInterrupt) = int_invalid;
    ARCH_गाठ_STATE(x86KSPendingInterrupt) = int_invalid;

#ifdef CONFIG_KERNEL_MCS
    गाठ_STATE(ksCurTime) = getCurrentTime();
    गाठ_STATE(ksConsumed) = 0;
#endif

    schedule();
    activateThread();
}

