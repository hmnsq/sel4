/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <util.h>
#ଅନ୍ତର୍ଭାଵ <hardware.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/apic.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/cmdline.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/boot.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/boot_sys.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/smp_sys.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/elf.h>
#ଅନ୍ତର୍ଭାଵ <smp/lock.h>
#ଅନ୍ତର୍ଭାଵ <linker.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/acpi.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/devices.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/pic.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/ioapic.h>
#ଅନ୍ତର୍ଭାଵ <sel4/arch/bootinfo_types.h>

/* addresses defined in linker script */
/* need a fake array to get the pointer from the linker script */

/* start/end of CPU boot code */
ବାହ୍ୟ ଅକ୍ଷର boot_cpu_start[1];
ବାହ୍ୟ ଅକ୍ଷର boot_cpu_end[1];

/* start/end of boot stack */
ବାହ୍ୟ ଅକ୍ଷର boot_ଗଦା_bottom[1];
ବାହ୍ୟ ଅକ୍ଷର boot_ଗଦା_ଶୀର୍ଷ[1];

/* locations in kernel image */
ବାହ୍ୟ ଅକ୍ଷର ki_skim_start[1];
ବାହ୍ୟ ଅକ୍ଷର ki_skim_end[1];

#ifdef CONFIG_PRINTING
/* kernel entry point */
ବାହ୍ୟ ଅକ୍ଷର _start[1];
#endif

/* constants */

#define HIGHMEM_PADDR 0x100000

BOOT_BSS
boot_state_t boot_state;

/* global variables (not covered by abstract specification) */

BOOT_BSS
cmdline_opt_t cmdline_opt;

/* functions not modeled in abstract specification */

BOOT_CODE ସ୍ଥିର paddr_t find_load_paddr(paddr_t min_paddr, word_t image_size)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; i < boot_state.mem_p_regs.ଗଣନା_2; i++) {
        paddr_t start = MAX(min_paddr, boot_state.mem_p_regs.list[i].start);
        paddr_t end = boot_state.mem_p_regs.list[i].end;
        word_t region_size = end - start;

        ଯଦି (region_size >= image_size) {
            ଫେରନ୍ତୁ start;
        }
    }

    ଫେରନ୍ତୁ 0;
}

BOOT_CODE ସ୍ଥିର paddr_t load_boot_module(word_t boot_module_start, word_t boot_module_end, paddr_t load_paddr)
{
    v_region_t v_reg;
    word_t entry;
    Elf_Header_t *elf_file = (Elf_Header_t *)boot_module_start;

    ଯଦି (!elf_checkFile(elf_file, boot_module_end - boot_module_start)) {
        printf("Boot module does not contain a valid ELF image\n");
        ଫେରନ୍ତୁ 0;
    }

    v_reg = elf_getMemoryBounds(elf_file);
    entry = elf_file->e_entry;

    ଯଦି (v_reg.end == 0) {
        printf("ELF image in boot module does not contain any segments\n");
        ଫେରନ୍ତୁ 0;
    }
    v_reg.end = ROUND_UP(v_reg.end, PAGE_BITS);

    printf("size=0x%lx v_entry=%p v_start=%p v_end=%p ",
           v_reg.end - v_reg.start,
           (ଶୂନ୍ୟ *)entry,
           (ଶୂନ୍ୟ *)v_reg.start,
           (ଶୂନ୍ୟ *)v_reg.end
          );

    ଯଦି (!IS_ALIGNED(v_reg.start, PAGE_BITS)) {
        printf("Userland image virtual start address must be 4KB-aligned\n");
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି (v_reg.end + 2 * BIT(PAGE_BITS) > USER_ଶୀର୍ଷ) {
        /* for IPC buffer frame and bootinfo frame, need 2*4K of additional userland virtual memory */
        printf("Userland image virtual end address too high\n");
        ଫେରନ୍ତୁ 0;
    }
    ଯଦି ((entry < v_reg.start) || (entry >= v_reg.end)) {
        printf("Userland image entry point does not lie within userland image\n");
        ଫେରନ୍ତୁ 0;
    }

    load_paddr = find_load_paddr(load_paddr, v_reg.end - v_reg.start);
    ପ୍ରତିଜ୍ଞା(load_paddr);

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
        (ଶୂନ୍ୟ *)boot_state.ui_info.p_reg.start,
        boot_state.ui_info.p_reg.end - boot_state.ui_info.p_reg.start
    );
    elf_load(elf_file, boot_state.ui_info.pv_offset);

    ଫେରନ୍ତୁ load_paddr;
}

ସ୍ଥିର BOOT_CODE bool_t try_boot_sys_ଗଣ୍ଠି(cpu_id_t cpu_id)
{
    ଯଦି (!map_kernel_window(
            boot_state.ଅଙ୍କ_ioapic,
            boot_state.ioapic_paddr,
            boot_state.ଅଙ୍କ_drhu,
            boot_state.drhu_list
        )) {
        ଫେରନ୍ତୁ false;
    }
    set_Current_VSpace_ମୂଳ(kpptr_to_paddr(X86_KERNEL_VSPACE_ମୂଳ), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm ଅସ୍ଥିର("" ::: "memory");

#ifdef CONFIG_KERNEL_SKIM_WINDOW
    ଯଦି (!map_skim_window((vptr_t)ki_skim_start, (vptr_t)ki_skim_end)) {
        ଫେରନ୍ତୁ false;
    }
#endif

    /* initialise the CPU */
    ଯଦି (!init_cpu(config_set(CONFIG_IRQ_IOAPIC) ? 1 : 0)) {
        ଫେରନ୍ତୁ false;
    }

    /* initialise NDKS and kernel heap */
    ଯଦି (!init_sys_state(
            cpu_id,
            &boot_state.mem_p_regs,
            boot_state.ui_info,
            /* parameters below not modelled in abstract specification */
            boot_state.ଅଙ୍କ_drhu,
            boot_state.drhu_list,
            &boot_state.rmrr_list,
            &boot_state.acpi_rsdp,
            &boot_state.vbe_info,
            &boot_state.mb_mmap_info,
            &boot_state.fb_info
        )) {
        ଫେରନ୍ତୁ false;
    }

    ଫେରନ୍ତୁ true;
}

ସ୍ଥିର BOOT_CODE bool_t add_mem_p_regs(p_region_t reg)
{
    ଯଦି (reg.start == reg.end) {
        // Return true here if asked to add an empty region.
        // Some of the callers round down the end address to
        ଫେରନ୍ତୁ true;
    }
    ଯଦି (reg.end > PADDR_ଶୀର୍ଷ && reg.start >= PADDR_ଶୀର୍ଷ) {
        /* Return true here as it's not an error for there to exist memory outside the kernel window,
         * we're just going to ignore it and leave it to be given out as device memory */
        ଫେରନ୍ତୁ true;
    }
    ଯଦି (boot_state.mem_p_regs.ଗଣନା_2 == MAX_ଅଙ୍କ_FREEMEM_REG) {
        printf("Dropping memory region 0x%lx-0x%lx, try increasing MAX_NUM_FREEMEM_REG\n", reg.start, reg.end);
        ଫେରନ୍ତୁ false;
    }
    ଯଦି (reg.end > PADDR_ଶୀର୍ଷ) {
        ପ୍ରତିଜ୍ଞା(reg.start < PADDR_ଶୀର୍ଷ);
        /* Clamp a region to the top of the kernel window if it extends beyond */
        reg.end = PADDR_ଶୀର୍ଷ;
    }
    printf("Adding physical memory region 0x%lx-0x%lx\n", reg.start, reg.end);
    boot_state.mem_p_regs.list[boot_state.mem_p_regs.ଗଣନା_2] = reg;
    boot_state.mem_p_regs.ଗଣନା_2++;
    ଫେରନ୍ତୁ true;
}

/*
 * the code relies that the GRUB provides correct information
 * about the actual physical memory regions.
 */
ସ୍ଥିର BOOT_CODE bool_t parse_mem_map(uint32_t mmap_length, uint32_t mmap_addr)
{
    multiboot_mmap_t *mmap = (multiboot_mmap_t *)((word_t)mmap_addr);
    printf("Parsing GRUB physical memory map\n");

    ଯେତେବେଳେ ((word_t)mmap < (word_t)(mmap_addr + mmap_length)) {
        uint64_t mem_start = mmap->base_addr;
        uint64_t mem_length = mmap->length;
        uint32_t type = mmap->type;
        ଯଦି (mem_start != (uint64_t)(word_t)mem_start) {
            printf("\tPhysical memory region not addressable\n");
        } ଅନ୍ୟଥା {
            printf("\tPhysical Memory Region from %lx size %lx type %d\n", (long)mem_start, (long)mem_length, type);
            ଯଦି (type == MULTIBOOT_MMAP_USEABLE_TYPE && mem_start >= HIGHMEM_PADDR && mem_length >= BIT(PAGE_BITS)) {

                ଯଦି (!add_mem_p_regs((p_region_t) {
                ROUND_UP(mem_start, PAGE_BITS), ROUND_DOWN(mem_start + mem_length, PAGE_BITS),
                })) {
                    ଫେରନ୍ତୁ false;
                }
            }
        }
        mmap++;
    }
    ଫେରନ୍ତୁ true;
}

ସ୍ଥିର BOOT_CODE bool_t is_compiled_for_microarchitecture(ଶୂନ୍ୟ)
{
    word_t microarch_generation = 0;
    x86_cpu_identity_t *model_info = x86_cpuid_get_model_info();

    ଯଦି (config_set(CONFIG_ARCH_X86_SKYLAKE)) {
        microarch_generation = 7;
    } ଅନ୍ୟଥା ଯଦି (config_set(CONFIG_ARCH_X86_BROADWELL)) {
        microarch_generation = 6;
    } ଅନ୍ୟଥା ଯଦି (config_set(CONFIG_ARCH_X86_HASWELL)) {
        microarch_generation = 5;
    } ଅନ୍ୟଥା ଯଦି (config_set(CONFIG_ARCH_X86_IVY)) {
        microarch_generation = 4;
    } ଅନ୍ୟଥା ଯଦି (config_set(CONFIG_ARCH_X86_SANDY)) {
        microarch_generation = 3;
    } ଅନ୍ୟଥା ଯଦି (config_set(CONFIG_ARCH_X86_WESTMERE)) {
        microarch_generation = 2;
    } ଅନ୍ୟଥା ଯଦି (config_set(CONFIG_ARCH_X86_NEHALEM)) {
        microarch_generation = 1;
    }

    ବାଛ (model_info->model) {
    ସ୍ଥିତି SKYLAKE_1_MODEL_ID:
    ସ୍ଥିତି SKYLAKE_2_MODEL_ID:
    ସ୍ଥିତି SKYLAKE_X_MODEL_ID:
        ଯଦି (microarch_generation > 7) {
            ଫେରନ୍ତୁ false;
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି BROADWELL_1_MODEL_ID:
    ସ୍ଥିତି BROADWELL_2_MODEL_ID:
    ସ୍ଥିତି BROADWELL_3_MODEL_ID:
    ସ୍ଥିତି BROADWELL_4_MODEL_ID:
    ସ୍ଥିତି BROADWELL_5_MODEL_ID:
        ଯଦି (microarch_generation > 6) {
            ଫେରନ୍ତୁ false;
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି HASWELL_1_MODEL_ID:
    ସ୍ଥିତି HASWELL_2_MODEL_ID:
    ସ୍ଥିତି HASWELL_3_MODEL_ID:
    ସ୍ଥିତି HASWELL_4_MODEL_ID:
        ଯଦି (microarch_generation > 5) {
            ଫେରନ୍ତୁ false;
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି IVY_BRIDGE_1_MODEL_ID:
    ସ୍ଥିତି IVY_BRIDGE_2_MODEL_ID:
    ସ୍ଥିତି IVY_BRIDGE_3_MODEL_ID:
        ଯଦି (microarch_generation > 4) {
            ଫେରନ୍ତୁ false;
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି SANDY_BRIDGE_1_MODEL_ID:
    ସ୍ଥିତି SANDY_BRIDGE_2_MODEL_ID:
        ଯଦି (microarch_generation > 3) {
            ଫେରନ୍ତୁ false;
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି WESTMERE_1_MODEL_ID:
    ସ୍ଥିତି WESTMERE_2_MODEL_ID:
    ସ୍ଥିତି WESTMERE_3_MODEL_ID:
        ଯଦି (microarch_generation > 2) {
            ଫେରନ୍ତୁ false;
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି NEHALEM_1_MODEL_ID:
    ସ୍ଥିତି NEHALEM_2_MODEL_ID:
    ସ୍ଥିତି NEHALEM_3_MODEL_ID:
        ଯଦି (microarch_generation > 1) {
            ଫେରନ୍ତୁ false;
        }
        ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଯଦି (!config_set(CONFIG_ARCH_X86_GENERIC)) {
            ଫେରନ୍ତୁ false;
        }
    }

    ଫେରନ୍ତୁ true;
}

ସ୍ଥିର BOOT_CODE bool_t try_boot_sys(ଶୂନ୍ୟ)
{
    paddr_t mods_end_paddr = boot_state.mods_end_paddr;
    p_region_t ui_p_regs;
    paddr_t load_paddr;

    boot_state.ki_p_reg = get_p_reg_kernel_img();

    ଯଦି (!x86_cpuid_initialize()) {
        printf("Warning: Your x86 CPU has an unsupported vendor, '%s'.\n"
               "\tYour setup may not be able to competently run seL4 as "
               "intended.\n"
               "\tCurrently supported x86 vendors are AMD and Intel.\n",
               x86_cpuid_get_identity()->vendor_string);
    }

    ଯଦି (!is_compiled_for_microarchitecture()) {
        printf("Warning: Your kernel was not compiled for the current microarchitecture.\n");
    }

    cpuid_007h_edx_t edx;
    edx.ଶବ୍ଦଗୁଡ଼ିକ[0] = x86_cpuid_edx(0x7, 0);
    /* see if we can definitively say whether or not we need the skim window by
     * checking whether the CPU is vulnerable to rogue data cache loads (rdcl) */
    ଯଦି (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.ଶବ୍ଦଗୁଡ଼ିକ[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        ଯଦି (ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && config_set(CONFIG_KERNEL_SKIM_WINDOW)) {
            printf("CPU reports not vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is enabled. Performance is needlessly being impacted, consider disabling.\n");
        } ଅନ୍ୟଥା ଯଦି (!ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && !config_set(CONFIG_KERNEL_SKIM_WINDOW)) {
            printf("CPU reports vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is *not* enabled. Please re-build with SKIM window enabled.");
            ଫେରନ୍ତୁ false;
        }
    } ଅନ୍ୟଥା {
        /* hardware doesn't tell us directly so guess based on CPU vendor */
        ଯଦି (config_set(CONFIG_KERNEL_SKIM_WINDOW) && x86_cpuid_get_identity()->vendor == X86_VENDOR_AMD) {
            printf("SKIM window for mitigating Meltdown (https://www.meltdownattack.com) "
                   "not necessary for AMD and performance is being needlessly affected, "
                   "consider disabling\n");
        }
        ଯଦି (!config_set(CONFIG_KERNEL_SKIM_WINDOW) && x86_cpuid_get_identity()->vendor == X86_VENDOR_INTEL) {
            printf("***WARNING*** SKIM window not enabled, this machine is probably vulnerable "
                   "to Meltdown (https://www.meltdownattack.com), consider enabling\n");
        }
    }

#ifdef ENABLE_SMP_SUPPORT
    /* copy boot code for APs to lower memory to run in real mode */
    ଯଦି (!copy_boot_code_aps(boot_state.mem_lower)) {
        ଫେରନ୍ତୁ false;
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
    ଯଦି (config_set(CONFIG_IRQ_IOAPIC)) {
        /* Disable the PIC so that it does not generate any interrupts. We need to
         * do this *before* we initialize the apic */
        pic_disable();
    }

    /* validate the ACPI table */
    ଯଦି (!acpi_validate_rsdp(&boot_state.acpi_rsdp)) {
        ଫେରନ୍ତୁ false;
    }

    /* check if kernel configuration matches platform requirements */
    ଯଦି (!acpi_fadt_scan(&boot_state.acpi_rsdp)) {
        ଫେରନ୍ତୁ false;
    }

    ଯଦି (!config_set(CONFIG_IOMMU) || cmdline_opt.disable_iommu) {
        boot_state.ଅଙ୍କ_drhu = 0;
    } ଅନ୍ୟଥା {
        /* query available IOMMUs from ACPI */
        acpi_dmar_scan(
            &boot_state.acpi_rsdp,
            boot_state.drhu_list,
            &boot_state.ଅଙ୍କ_drhu,
            MAX_ଅଙ୍କ_DRHU,
            &boot_state.rmrr_list
        );
    }

    /* query available CPUs from ACPI */
    boot_state.ଅଙ୍କ_cpus = acpi_madt_scan(&boot_state.acpi_rsdp, boot_state.cpus, &boot_state.ଅଙ୍କ_ioapic,
                                         boot_state.ioapic_paddr);
    ଯଦି (boot_state.ଅଙ୍କ_cpus == 0) {
        printf("No CPUs detected\n");
        ଫେରନ୍ତୁ false;
    }

    ଯଦି (config_set(CONFIG_IRQ_IOAPIC)) {
        ଯଦି (boot_state.ଅଙ୍କ_ioapic == 0) {
            printf("No IOAPICs detected\n");
            ଫେରନ୍ତୁ false;
        }
    } ଅନ୍ୟଥା {
        ଯଦି (boot_state.ଅଙ୍କ_ioapic > 0) {
            printf("Detected %d IOAPICs, but configured to use PIC instead\n", boot_state.ଅଙ୍କ_ioapic);
        }
    }

    mods_end_paddr = ROUND_UP(mods_end_paddr, PAGE_BITS);
    ପ୍ରତିଜ୍ଞା(mods_end_paddr > boot_state.ki_p_reg.end);

    printf("ELF-loading userland images from boot modules:\n");
    load_paddr = mods_end_paddr;

    load_paddr = load_boot_module(boot_state.boot_module_start, boot_state.boot_module_end, load_paddr);
    ଯଦି (!load_paddr) {
        ଫେରନ୍ତୁ false;
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
    memcpy((ଶୂନ୍ୟ *)ui_p_regs.start, (ଶୂନ୍ୟ *)mods_end_paddr, ui_p_regs.end - ui_p_regs.start);

    /* adjust p_reg and pv_offset to final load address */
    boot_state.ui_info.p_reg.start -= mods_end_paddr - ui_p_regs.start;
    boot_state.ui_info.p_reg.end   -= mods_end_paddr - ui_p_regs.start;
    boot_state.ui_info.pv_offset   -= mods_end_paddr - ui_p_regs.start;

    /* ==== following code corresponds to abstract specification after "select" ==== */

    ଯଦି (!platAddDevices()) {
        ଫେରନ୍ତୁ false;
    }

    /* Total number of cores we intend to boot */
    ks_ଅଙ୍କ_CPUs = boot_state.ଅଙ୍କ_cpus;

    printf("Starting node #0 with APIC ID %lu\n", boot_state.cpus[0]);
    ଯଦି (!try_boot_sys_ଗଣ୍ଠି(boot_state.cpus[0])) {
        ଫେରନ୍ତୁ false;
    }

    ଯଦି (config_set(CONFIG_IRQ_IOAPIC)) {
        ioapic_init(1, boot_state.cpus, boot_state.ଅଙ୍କ_ioapic);
    }

    /* initialize BKL before booting up APs */
    SMP_COND_STATEMENT(clh_lock_init());
    SMP_COND_STATEMENT(start_boot_aps());

    /* grab BKL before leaving the kernel */
    ଗଣ୍ଠି_LOCK_SYS;

    printf("Booting all finished, dropped to user space\n");

    ଫେରନ୍ତୁ true;
}

ସ୍ଥିର BOOT_CODE bool_t try_boot_sys_mbi1(
    multiboot_info_t *mbi
)
{
    word_t i;
    multiboot_module_t *modules = (multiboot_module_t *)(word_t)mbi->part1.mod_list;

    cmdline_parse((ଅଚଳ ଅକ୍ଷର *)(word_t)mbi->part1.cmdline, &cmdline_opt);

    ଯଦି ((mbi->part1.flags & MULTIBOOT_INFO_MEM_FLAG) == 0) {
        printf("Boot loader did not provide information about physical memory size\n");
        ଫେରନ୍ତୁ false;
    }

    ଯଦି (!(mbi->part1.flags & MULTIBOOT_INFO_MODS_FLAG)) {
        printf("Boot loader did not provide information about boot modules\n");
        ଫେରନ୍ତୁ false;
    }

    printf("Detected %d boot module(s):\n", mbi->part1.mod_ଗଣନା);

    ଯଦି (mbi->part1.mod_ଗଣନା < 1) {
        printf("Expect at least one boot module (containing a userland image)\n");
        ଫେରନ୍ତୁ false;
    }

    ପାଇଁ (i = 0; i < mbi->part1.mod_ଗଣନା; i++) {
        printf(
            "  module #%ld: start=0x%x end=0x%x size=0x%x name='%s'\n",
            i,
            modules[i].start,
            modules[i].end,
            modules[i].end - modules[i].start,
            (ଅକ୍ଷର *)(long)modules[i].name
        );
        ଯଦି ((sword_t)(modules[i].end - modules[i].start) <= 0) {
            printf("Invalid boot module size! Possible cause: boot module file not found by QEMU\n");
            ଫେରନ୍ତୁ false;
        }
        ଯଦି (boot_state.mods_end_paddr < modules[i].end) {
            boot_state.mods_end_paddr = modules[i].end;
        }
    }

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.ଗଣନା_2 = 0;
    ଯଦି (mbi->part1.flags & MULTIBOOT_INFO_MMAP_FLAG) {
        ଯଦି (!parse_mem_map(mbi->part2.mmap_length, mbi->part2.mmap_addr)) {
            ଫେରନ୍ତୁ false;
        }
        uint32_t multiboot_mmap_length = mbi->part2.mmap_length;
        ଯଦି (multiboot_mmap_length > (SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * ଆକାର(seL4_X86_mb_mmap_t))) {
            printf("Warning: Multiboot has reported more memory map entries, %zd, "
                   "than the max amount that will be passed in the bootinfo, %d. "
                   "These extra regions will still be turned into untyped caps.",
                   multiboot_mmap_length / ଆକାର(seL4_X86_mb_mmap_t), SEL4_MULTIBOOT_MAX_MMAP_ENTRIES);
            multiboot_mmap_length = SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * ଆକାର(seL4_X86_mb_mmap_t);
        }
        memcpy(&boot_state.mb_mmap_info.mmap, (ଶୂନ୍ୟ *)(word_t)mbi->part2.mmap_addr, multiboot_mmap_length);
        boot_state.mb_mmap_info.mmap_length = multiboot_mmap_length;
    } ଅନ୍ୟଥା {
        /* calculate memory the old way */
        p_region_t avail;
        avail.start = HIGHMEM_PADDR;
        avail.end = ROUND_DOWN(avail.start + (mbi->part1.mem_upper << 10), PAGE_BITS);
        ଯଦି (!add_mem_p_regs(avail)) {
            ଫେରନ୍ତୁ false;
        }
    }

    /* copy VESA information from multiboot header */
    ଯଦି ((mbi->part1.flags & MULTIBOOT_INFO_GRAPHICS_FLAG) == 0) {
        boot_state.vbe_info.vbeMode = -1;
        printf("Multiboot gave us no video information\n");
    } ଅନ୍ୟଥା {
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
    ଯଦି (!acpi_init(&boot_state.acpi_rsdp)) {
        ଫେରନ୍ତୁ false;
    }

    ଫେରନ୍ତୁ true;
}

ସ୍ଥିର BOOT_CODE bool_t try_boot_sys_mbi2(
    multiboot2_header_t *mbi2
)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mod_ଗଣନା                  = 0;
    multiboot2_tag_t ଅଚଳ *tag   = (multiboot2_tag_t *)(mbi2 + 1);
    multiboot2_tag_t ଅଚଳ *tag_e = (multiboot2_tag_t *)((word_t)mbi2 + mbi2->total_size);

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.ଗଣନା_2 = 0;
    boot_state.mb_mmap_info.mmap_length = 0;
    boot_state.vbe_info.vbeMode = -1;

    ଯେତେବେଳେ (tag < tag_e && tag->type != MULTIBOOT2_TAG_END) {
        word_t ଅଚଳ behind_tag = (word_t)tag + ଆକାର(*tag);

        ଯଦି (tag->type == MULTIBOOT2_TAG_CMDLINE) {
            ଅକ୍ଷର ଅଚଳ *ଅଚଳ cmdline = (ଅକ୍ଷର ଅଚଳ * ଅଚଳ)(behind_tag);
            cmdline_parse(cmdline, &cmdline_opt);
        } ଅନ୍ୟଥା ଯଦି (tag->type == MULTIBOOT2_TAG_ACPI_1) {
            ଯଦି (ACPI_V1_SIZE == tag->size - ଆକାର(*tag)) {
                memcpy(&boot_state.acpi_rsdp, (ଶୂନ୍ୟ *)behind_tag, tag->size - ଆକାର(*tag));
            }
        } ଅନ୍ୟଥା ଯଦି (tag->type == MULTIBOOT2_TAG_ACPI_2) {
            ଯଦି (ଆକାର(boot_state.acpi_rsdp) == tag->size - ଆକାର(*tag)) {
                memcpy(&boot_state.acpi_rsdp, (ଶୂନ୍ୟ *)behind_tag, ଆକାର(boot_state.acpi_rsdp));
            }
        } ଅନ୍ୟଥା ଯଦି (tag->type == MULTIBOOT2_TAG_MODULE) {
            multiboot2_module_t ଅଚଳ *module = (multiboot2_module_t ଅଚଳ *)behind_tag;
            printf(
                "  module #%d: start=0x%x end=0x%x size=0x%x name='%s'\n",
                mod_ଗଣନା,
                module->start,
                module->end,
                module->end - module->start,
                module->string
            );

            ଯଦି (mod_ଗଣନା == 0) {
                boot_state.boot_module_start = module->start;
                boot_state.boot_module_end = module->end;
            }

            mod_ଗଣନା ++;
            ଯଦି ((sword_t)(module->end - module->start) <= 0) {
                printf("Invalid boot module size! Possible cause: boot module file not found\n");
                ଫେରନ୍ତୁ false;
            }
            ଯଦି (boot_state.mods_end_paddr < module->end) {
                boot_state.mods_end_paddr = module->end;
            }
        } ଅନ୍ୟଥା ଯଦି (tag->type == MULTIBOOT2_TAG_MEMORY) {
            multiboot2_memory_t ଅଚଳ *s = (multiboot2_memory_t *)(behind_tag + 8);
            multiboot2_memory_t ଅଚଳ *e = (multiboot2_memory_t *)((word_t)tag + tag->size);

            ପାଇଁ (multiboot2_memory_t ଅଚଳ *m = s; m < e; m++) {
                ଯଦି (!m->addr) {
                    boot_state.mem_lower = m->size;
                }

                printf("\tPhysical Memory Region from %llx size %llx type %u\n", m->addr, m->size, m->type);
                ଯଦି (m->addr != (uint64_t)(word_t)m->addr) {
                    printf("\t\tPhysical memory region not addressable\n");
                    ଜାରି;
                }

                ଯଦି (m->type == MULTIBOOT_MMAP_USEABLE_TYPE && m->addr >= HIGHMEM_PADDR) {
                    ଯଦି (!add_mem_p_regs((p_region_t) {
                    m->addr, m->addr + m->size
                }))
                    ଫେରନ୍ତୁ false;
                }
            }
        } ଅନ୍ୟଥା ଯଦି (tag->type == MULTIBOOT2_TAG_FB) {
            multiboot2_fb_t ଅଚଳ *fb = (multiboot2_fb_t ଅଚଳ *)behind_tag;
            printf("Got framebuffer info in multiboot2. Current video mode is at physical address=%llx pitch=%u resolution=%ux%u@%u type=%u\n",
                   fb->addr, fb->pitch, fb->width, fb->height, fb->bpp, fb->type);
            boot_state.fb_info = *fb;
        }

        tag = (multiboot2_tag_t ଅଚଳ *)((word_t)tag + ROUND_UP(tag->size, 3));
    }

    printf("Detected %d boot module(s):\n", mod_ଗଣନା);

    ଯଦି (mod_ଗଣନା < 1) {
        printf("Expect at least one boot module (containing a userland image)\n");
        ଫେରନ୍ତୁ false;
    }

    ଫେରନ୍ତୁ true;
}

BOOT_CODE VISIBLE ଶୂନ୍ୟ boot_sys(
    unsigned long multiboot_magic,
    ଶୂନ୍ୟ *mbi)
{
    bool_t result = false;

    ଯଦି (multiboot_magic == MULTIBOOT_MAGIC) {
        result = try_boot_sys_mbi1(mbi);
    } ଅନ୍ୟଥା ଯଦି (multiboot_magic == MULTIBOOT2_MAGIC) {
        result = try_boot_sys_mbi2(mbi);
    } ଅନ୍ୟଥା {
        printf("Boot loader is not multiboot 1 or 2 compliant %lx\n", multiboot_magic);
    }

    ଯଦି (result) {
        result = try_boot_sys();
    }

    ଯଦି (!result) {
        fail("boot_sys failed for some reason :(\n");
    }

    ARCH_ଗଣ୍ଠି_STATE(x86KScurInterrupt) = int_invalid;
    ARCH_ଗଣ୍ଠି_STATE(x86KSPendingInterrupt) = int_invalid;

#ifdef CONFIG_KERNEL_MCS
    ଗଣ୍ଠି_STATE(ksCurTime) = getCurrentTime();
    ଗଣ୍ଠି_STATE(ksConsumed) = 0;
#endif

    schedule();
    activateThread();
}

