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
बाह्य वर्ण boot_cpu_अन्तः[1];

/* start/end of boot stack */
बाह्य वर्ण boot_राशिः_bottom[1];
बाह्य वर्ण boot_राशिः_शिखरम्[1];

/* locations in kernel image */
बाह्य वर्ण ki_skim_start[1];
बाह्य वर्ण ki_skim_अन्तः[1];

#यदि_परिभाषितम् CONFIG_PRINTING
/* kernel entry point */
बाह्य वर्ण _start[1];
#यदि_अन्तः

/* constants */

#परिभाषय HIGHMEM_PADDR 0x100000

BOOT_BSS
boot_state_t boot_state;

/* global variables (not covered by abstract specification) */

BOOT_BSS
cmdline_opt_t cmdline_opt;

/* functions not modeled in abstract specification */

BOOT_संहिता स्थिर paddr_t find_आरोपय_paddr(paddr_t min_paddr, word_t image_आकार)
{
    पूर्णाङ्क i;

    प्रति (i = 0; i < boot_state.mem_p_regs.गणनम्; i++) {
        paddr_t start = MAX(min_paddr, boot_state.mem_p_regs.list[i].start);
        paddr_t अन्तः_2 = boot_state.mem_p_regs.list[i].अन्तः_2;
        word_t region_आकार = अन्तः_2 - start;

        यदि (region_आकार >= image_आकार) {
            प्रत्यागम start;
        }
    }

    प्रत्यागम 0;
}

BOOT_संहिता स्थिर paddr_t आरोपय_boot_module(word_t boot_module_start, word_t boot_module_अन्तः, paddr_t आरोपय_paddr)
{
    v_region_t v_reg;
    word_t प्रविष्टिः;
    Elf_शीर्षकम्_t *elf_file = (Elf_शीर्षकम्_t *)boot_module_start;

    यदि (!elf_checkFile(elf_file, boot_module_अन्तः - boot_module_start)) {
        प्रारूप_लिख("Boot module does not contain a valid ELF image\n");
        प्रत्यागम 0;
    }

    v_reg = elf_get_स्मृतिः_Bounds(elf_file);
    प्रविष्टिः = elf_file->e_प्रविष्टिः;

    यदि (v_reg.अन्तः_2 == 0) {
        प्रारूप_लिख("ELF image in boot module does not contain any segments\n");
        प्रत्यागम 0;
    }
    v_reg.अन्तः_2 = आवर्तनम्_ऊर्ध्वम्(v_reg.अन्तः_2, PAGE_द्वयङ्काः);

    प्रारूप_लिख("size=0x%lx v_entry=%p v_start=%p v_end=%p ",
           v_reg.अन्तः_2 - v_reg.start,
           (शून्य *)प्रविष्टिः,
           (शून्य *)v_reg.start,
           (शून्य *)v_reg.अन्तः_2
          );

    यदि (!IS_ALIGNED(v_reg.start, PAGE_द्वयङ्काः)) {
        प्रारूप_लिख("Userland image virtual start address must be 4KB-aligned\n");
        प्रत्यागम 0;
    }
    यदि (v_reg.अन्तः_2 + 2 * BIT(PAGE_द्वयङ्काः) > USER_शिखरम्) {
        /* for IPC buffer frame and bootinfo frame, need 2*4K of additional userland virtual memory */
        प्रारूप_लिख("Userland image virtual end address too high\n");
        प्रत्यागम 0;
    }
    यदि ((प्रविष्टिः < v_reg.start) || (प्रविष्टिः >= v_reg.अन्तः_2)) {
        प्रारूप_लिख("Userland image entry point does not lie within userland image\n");
        प्रत्यागम 0;
    }

    आरोपय_paddr = find_आरोपय_paddr(आरोपय_paddr, v_reg.अन्तः_2 - v_reg.start);
    प्रतिज्ञा(आरोपय_paddr);

    /* fill ui_info struct */
    boot_state.ui_info.pv_विस्थापनम् = आरोपय_paddr - v_reg.start;
    boot_state.ui_info.p_reg.start = आरोपय_paddr;
    आरोपय_paddr += v_reg.अन्तः_2 - v_reg.start;
    boot_state.ui_info.p_reg.अन्तः_2 = आरोपय_paddr;
    boot_state.ui_info.v_प्रविष्टिः = प्रविष्टिः;

    प्रारूप_लिख("p_start=0x%lx p_end=0x%lx\n",
           boot_state.ui_info.p_reg.start,
           boot_state.ui_info.p_reg.अन्तः_2
          );

    /* initialise all initial userland memory and load potentially sparse ELF image */
    memzero(
        (शून्य *)boot_state.ui_info.p_reg.start,
        boot_state.ui_info.p_reg.अन्तः_2 - boot_state.ui_info.p_reg.start
    );
    elf_आरोपय(elf_file, boot_state.ui_info.pv_विस्थापनम्);

    प्रत्यागम आरोपय_paddr;
}

स्थिर BOOT_संहिता bool_t try_boot_sys_ग्रन्थिः(cpu_id_t cpu_id)
{
    यदि (!map_kernel_गवाक्षः(
            boot_state.अङ्कः_ioapic,
            boot_state.ioapic_paddr,
            boot_state.अङ्कः_drhu,
            boot_state.drhu_list
        )) {
        प्रत्यागम false;
    }
    समुच्चयः_Current_VSpace_मूलम्(kpptr_गन्तव्यम्_paddr(X86_KERNEL_VSPACE_मूलम्), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm चञ्चल("" ::: "memory");

#यदि_परिभाषितम् CONFIG_KERNEL_SKIM_गवाक्षः
    यदि (!map_skim_गवाक्षः((vptr_t)ki_skim_start, (vptr_t)ki_skim_अन्तः)) {
        प्रत्यागम false;
    }
#यदि_अन्तः

    /* initialise the CPU */
    यदि (!init_cpu(config_समुच्चयः(CONFIG_IRQ_IOAPIC) ? 1 : 0)) {
        प्रत्यागम false;
    }

    /* initialise NDKS and kernel heap */
    यदि (!init_sys_state(
            cpu_id,
            &boot_state.mem_p_regs,
            boot_state.ui_info,
            /* parameters below not modelled in abstract specification */
            boot_state.अङ्कः_drhu,
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

स्थिर BOOT_संहिता bool_t सङ्कलय_mem_p_regs(p_region_t reg)
{
    यदि (reg.start == reg.अन्तः_2) {
        // Return true here if asked to add an empty region.
        // Some of the callers round down the end address to
        प्रत्यागम true;
    }
    यदि (reg.अन्तः_2 > PADDR_शिखरम् && reg.start >= PADDR_शिखरम्) {
        /* Return true here as it's not an error for there to exist memory outside the kernel window,
         * we're just going to ignore it and leave it to be given out as device memory */
        प्रत्यागम true;
    }
    यदि (boot_state.mem_p_regs.गणनम् == MAX_अङ्कः_FREEMEM_REG) {
        प्रारूप_लिख("Dropping memory region 0x%lx-0x%lx, try increasing MAX_NUM_FREEMEM_REG\n", reg.start, reg.अन्तः_2);
        प्रत्यागम false;
    }
    यदि (reg.अन्तः_2 > PADDR_शिखरम्) {
        प्रतिज्ञा(reg.start < PADDR_शिखरम्);
        /* Clamp a region to the top of the kernel window if it extends beyond */
        reg.अन्तः_2 = PADDR_शिखरम्;
    }
    प्रारूप_लिख("Adding physical memory region 0x%lx-0x%lx\n", reg.start, reg.अन्तः_2);
    boot_state.mem_p_regs.list[boot_state.mem_p_regs.गणनम्] = reg;
    boot_state.mem_p_regs.गणनम्++;
    प्रत्यागम true;
}

/*
 * the code relies that the GRUB provides correct information
 * about the actual physical memory regions.
 */
स्थिर BOOT_संहिता bool_t parse_mem_map(uint32_t mmap_दीर्घता, uint32_t mmap_addr)
{
    multiboot_mmap_t *mmap = (multiboot_mmap_t *)((word_t)mmap_addr);
    प्रारूप_लिख("Parsing GRUB physical memory map\n");

    यावत् ((word_t)mmap < (word_t)(mmap_addr + mmap_दीर्घता)) {
        uint64_t mem_start = mmap->आधारः_addr;
        uint64_t mem_दीर्घता = mmap->दीर्घता;
        uint32_t type = mmap->type;
        यदि (mem_start != (uint64_t)(word_t)mem_start) {
            प्रारूप_लिख("\tPhysical memory region not addressable\n");
        } अन्यथा {
            प्रारूप_लिख("\tPhysical Memory Region from %lx size %lx type %d\n", (दीर्घ)mem_start, (दीर्घ)mem_दीर्घता, type);
            यदि (type == MULTIBOOT_MMAP_USEABLE_TYPE && mem_start >= HIGHMEM_PADDR && mem_दीर्घता >= BIT(PAGE_द्वयङ्काः)) {

                यदि (!सङ्कलय_mem_p_regs((p_region_t) {
                आवर्तनम्_ऊर्ध्वम्(mem_start, PAGE_द्वयङ्काः), आवर्तनम्_अधस्तात्(mem_start + mem_दीर्घता, PAGE_द्वयङ्काः),
                })) {
                    प्रत्यागम false;
                }
            }
        }
        mmap++;
    }
    प्रत्यागम true;
}

स्थिर BOOT_संहिता bool_t is_compiled_for_microarchitecture(शून्य)
{
    word_t microarch_generation = 0;
    x86_cpu_तटस्थः_t *model_info = x86_cpuid_get_model_info();

    यदि (config_समुच्चयः(CONFIG_ARCH_X86_SKYLAKE)) {
        microarch_generation = 7;
    } अन्यथा यदि (config_समुच्चयः(CONFIG_ARCH_X86_BROADWELL)) {
        microarch_generation = 6;
    } अन्यथा यदि (config_समुच्चयः(CONFIG_ARCH_X86_HASWELL)) {
        microarch_generation = 5;
    } अन्यथा यदि (config_समुच्चयः(CONFIG_ARCH_X86_IVY)) {
        microarch_generation = 4;
    } अन्यथा यदि (config_समुच्चयः(CONFIG_ARCH_X86_SANDY)) {
        microarch_generation = 3;
    } अन्यथा यदि (config_समुच्चयः(CONFIG_ARCH_X86_WESTMERE)) {
        microarch_generation = 2;
    } अन्यथा यदि (config_समुच्चयः(CONFIG_ARCH_X86_NEHALEM)) {
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
        यदि (!config_समुच्चयः(CONFIG_ARCH_X86_GENERIC)) {
            प्रत्यागम false;
        }
    }

    प्रत्यागम true;
}

स्थिर BOOT_संहिता bool_t try_boot_sys(शून्य)
{
    paddr_t mods_अन्तः_paddr = boot_state.mods_अन्तः_paddr;
    p_region_t ui_p_regs;
    paddr_t आरोपय_paddr;

    boot_state.ki_p_reg = get_p_reg_kernel_img();

    यदि (!x86_cpuid_initialize()) {
        प्रारूप_लिख("Warning: Your x86 CPU has an unsupported vendor, '%s'.\n"
               "\tYour setup may not be able to competently run seL4 as "
               "intended.\n"
               "\tCurrently supported x86 vendors are AMD and Intel.\n",
               x86_cpuid_get_तटस्थः()->vendor_string);
    }

    यदि (!is_compiled_for_microarchitecture()) {
        प्रारूप_लिख("Warning: Your kernel was not compiled for the current microarchitecture.\n");
    }

    cpuid_007h_edx_t edx;
    edx.शब्दाः[0] = x86_cpuid_edx(0x7, 0);
    /* see if we can definitively say whether or not we need the skim window by
     * checking whether the CPU is vulnerable to rogue data cache loads (rdcl) */
    यदि (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.शब्दाः[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        यदि (ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && config_समुच्चयः(CONFIG_KERNEL_SKIM_गवाक्षः)) {
            प्रारूप_लिख("CPU reports not vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is enabled. Performance is needlessly being impacted, consider disabling.\n");
        } अन्यथा यदि (!ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && !config_समुच्चयः(CONFIG_KERNEL_SKIM_गवाक्षः)) {
            प्रारूप_लिख("CPU reports vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is *not* enabled. Please re-build with SKIM window enabled.");
            प्रत्यागम false;
        }
    } अन्यथा {
        /* hardware doesn't tell us directly so guess based on CPU vendor */
        यदि (config_समुच्चयः(CONFIG_KERNEL_SKIM_गवाक्षः) && x86_cpuid_get_तटस्थः()->vendor == X86_VENDOR_AMD) {
            प्रारूप_लिख("SKIM window for mitigating Meltdown (https://www.meltdownattack.com) "
                   "not necessary for AMD and performance is being needlessly affected, "
                   "consider disabling\n");
        }
        यदि (!config_समुच्चयः(CONFIG_KERNEL_SKIM_गवाक्षः) && x86_cpuid_get_तटस्थः()->vendor == X86_VENDOR_INTEL) {
            प्रारूप_लिख("***WARNING*** SKIM window not enabled, this machine is probably vulnerable "
                   "to Meltdown (https://www.meltdownattack.com), consider enabling\n");
        }
    }

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    /* copy boot code for APs to lower memory to run in real mode */
    यदि (!copy_boot_संहिता_aps(boot_state.mem_अधस्तनम्)) {
        प्रत्यागम false;
    }
    /* Initialize any kernel TLS */
    बहुलकम्_init_tls(0);
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

    प्रारूप_लिख("Kernel loaded to: start=0x%lx end=0x%lx size=0x%lx entry=0x%lx\n",
           boot_state.ki_p_reg.start,
           boot_state.ki_p_reg.अन्तः_2,
           boot_state.ki_p_reg.अन्तः_2 - boot_state.ki_p_reg.start,
           (paddr_t)_start
          );

    /* remapping legacy IRQs to their correct vectors */
    pic_remap_irqs(IRQ_INT_विस्थापनम्);
    यदि (config_समुच्चयः(CONFIG_IRQ_IOAPIC)) {
        /* Disable the PIC so that it does not generate any interrupts. We need to
         * do this *before* we initialize the apic */
        pic_disable();
    }

    /* validate the ACPI table */
    यदि (!acpi_validate_rsdp(&boot_state.acpi_rsdp)) {
        प्रत्यागम false;
    }

    /* check if kernel configuration matches platform requirements */
    यदि (!acpi_fadt_अवलोकनम्(&boot_state.acpi_rsdp)) {
        प्रत्यागम false;
    }

    यदि (!config_समुच्चयः(CONFIG_IOMMU) || cmdline_opt.disable_iommu) {
        boot_state.अङ्कः_drhu = 0;
    } अन्यथा {
        /* query available IOMMUs from ACPI */
        acpi_dmar_अवलोकनम्(
            &boot_state.acpi_rsdp,
            boot_state.drhu_list,
            &boot_state.अङ्कः_drhu,
            MAX_अङ्कः_DRHU,
            &boot_state.rmrr_list
        );
    }

    /* query available CPUs from ACPI */
    boot_state.अङ्कः_cpus = acpi_madt_अवलोकनम्(&boot_state.acpi_rsdp, boot_state.cpus, &boot_state.अङ्कः_ioapic,
                                         boot_state.ioapic_paddr);
    यदि (boot_state.अङ्कः_cpus == 0) {
        प्रारूप_लिख("No CPUs detected\n");
        प्रत्यागम false;
    }

    यदि (config_समुच्चयः(CONFIG_IRQ_IOAPIC)) {
        यदि (boot_state.अङ्कः_ioapic == 0) {
            प्रारूप_लिख("No IOAPICs detected\n");
            प्रत्यागम false;
        }
    } अन्यथा {
        यदि (boot_state.अङ्कः_ioapic > 0) {
            प्रारूप_लिख("Detected %d IOAPICs, but configured to use PIC instead\n", boot_state.अङ्कः_ioapic);
        }
    }

    mods_अन्तः_paddr = आवर्तनम्_ऊर्ध्वम्(mods_अन्तः_paddr, PAGE_द्वयङ्काः);
    प्रतिज्ञा(mods_अन्तः_paddr > boot_state.ki_p_reg.अन्तः_2);

    प्रारूप_लिख("ELF-loading userland images from boot modules:\n");
    आरोपय_paddr = mods_अन्तः_paddr;

    आरोपय_paddr = आरोपय_boot_module(boot_state.boot_module_start, boot_state.boot_module_अन्तः, आरोपय_paddr);
    यदि (!आरोपय_paddr) {
        प्रत्यागम false;
    }

    /* calculate final location of userland images */
    ui_p_regs.start = boot_state.ki_p_reg.अन्तः_2;
    ui_p_regs.अन्तः_2 = ui_p_regs.start + आरोपय_paddr - mods_अन्तः_paddr;

    प्रारूप_लिख(
        "Moving loaded userland images to final location: from=0x%lx to=0x%lx size=0x%lx\n",
        mods_अन्तः_paddr,
        ui_p_regs.start,
        ui_p_regs.अन्तः_2 - ui_p_regs.start
    );
    स्मृतिं_प्रतिलिख((शून्य *)ui_p_regs.start, (शून्य *)mods_अन्तः_paddr, ui_p_regs.अन्तः_2 - ui_p_regs.start);

    /* adjust p_reg and pv_offset to final load address */
    boot_state.ui_info.p_reg.start -= mods_अन्तः_paddr - ui_p_regs.start;
    boot_state.ui_info.p_reg.अन्तः_2   -= mods_अन्तः_paddr - ui_p_regs.start;
    boot_state.ui_info.pv_विस्थापनम्   -= mods_अन्तः_paddr - ui_p_regs.start;

    /* ==== following code corresponds to abstract specification after "select" ==== */

    यदि (!plat_सङ्कलय_Devices()) {
        प्रत्यागम false;
    }

    /* Total number of cores we intend to boot */
    ks_अङ्कः_CPUs = boot_state.अङ्कः_cpus;

    प्रारूप_लिख("Starting node #0 with APIC ID %lu\n", boot_state.cpus[0]);
    यदि (!try_boot_sys_ग्रन्थिः(boot_state.cpus[0])) {
        प्रत्यागम false;
    }

    यदि (config_समुच्चयः(CONFIG_IRQ_IOAPIC)) {
        ioapic_init(1, boot_state.cpus, boot_state.अङ्कः_ioapic);
    }

    /* initialize BKL before booting up APs */
    SMP_COND_STATEMENT(clh_तालकम्_init());
    SMP_COND_STATEMENT(start_boot_aps());

    /* grab BKL before leaving the kernel */
    ग्रन्थिः_तालकम्_SYS;

    प्रारूप_लिख("Booting all finished, dropped to user space\n");

    प्रत्यागम true;
}

स्थिर BOOT_संहिता bool_t try_boot_sys_mbi1(
    multiboot_info_t *mbi
)
{
    word_t i;
    multiboot_module_t *modules = (multiboot_module_t *)(word_t)mbi->part1.mod_list;

    cmdline_parse((ध्रुव वर्ण *)(word_t)mbi->part1.cmdline, &cmdline_opt);

    यदि ((mbi->part1.flags & MULTIBOOT_INFO_MEM_FLAG) == 0) {
        प्रारूप_लिख("Boot loader did not provide information about physical memory size\n");
        प्रत्यागम false;
    }

    यदि (!(mbi->part1.flags & MULTIBOOT_INFO_MODS_FLAG)) {
        प्रारूप_लिख("Boot loader did not provide information about boot modules\n");
        प्रत्यागम false;
    }

    प्रारूप_लिख("Detected %d boot module(s):\n", mbi->part1.mod_गणनम्);

    यदि (mbi->part1.mod_गणनम् < 1) {
        प्रारूप_लिख("Expect at least one boot module (containing a userland image)\n");
        प्रत्यागम false;
    }

    प्रति (i = 0; i < mbi->part1.mod_गणनम्; i++) {
        प्रारूप_लिख(
            "  module #%ld: start=0x%x end=0x%x size=0x%x name='%s'\n",
            i,
            modules[i].start,
            modules[i].अन्तः_2,
            modules[i].अन्तः_2 - modules[i].start,
            (वर्ण *)(दीर्घ)modules[i].name
        );
        यदि ((sword_t)(modules[i].अन्तः_2 - modules[i].start) <= 0) {
            प्रारूप_लिख("Invalid boot module size! Possible cause: boot module file not found by QEMU\n");
            प्रत्यागम false;
        }
        यदि (boot_state.mods_अन्तः_paddr < modules[i].अन्तः_2) {
            boot_state.mods_अन्तः_paddr = modules[i].अन्तः_2;
        }
    }

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.गणनम् = 0;
    यदि (mbi->part1.flags & MULTIBOOT_INFO_MMAP_FLAG) {
        यदि (!parse_mem_map(mbi->part2.mmap_दीर्घता, mbi->part2.mmap_addr)) {
            प्रत्यागम false;
        }
        uint32_t multiboot_mmap_दीर्घता = mbi->part2.mmap_दीर्घता;
        यदि (multiboot_mmap_दीर्घता > (SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * परिमाण(seL4_X86_mb_mmap_t))) {
            प्रारूप_लिख("Warning: Multiboot has reported more memory map entries, %zd, "
                   "than the max amount that will be passed in the bootinfo, %d. "
                   "These extra regions will still be turned into untyped caps.",
                   multiboot_mmap_दीर्घता / परिमाण(seL4_X86_mb_mmap_t), SEL4_MULTIBOOT_MAX_MMAP_ENTRIES);
            multiboot_mmap_दीर्घता = SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * परिमाण(seL4_X86_mb_mmap_t);
        }
        स्मृतिं_प्रतिलिख(&boot_state.mb_mmap_info.mmap, (शून्य *)(word_t)mbi->part2.mmap_addr, multiboot_mmap_दीर्घता);
        boot_state.mb_mmap_info.mmap_दीर्घता = multiboot_mmap_दीर्घता;
    } अन्यथा {
        /* calculate memory the old way */
        p_region_t avail;
        avail.start = HIGHMEM_PADDR;
        avail.अन्तः_2 = आवर्तनम्_अधस्तात्(avail.start + (mbi->part1.mem_ऊर्ध्वस्थम् << 10), PAGE_द्वयङ्काः);
        यदि (!सङ्कलय_mem_p_regs(avail)) {
            प्रत्यागम false;
        }
    }

    /* copy VESA information from multiboot header */
    यदि ((mbi->part1.flags & MULTIBOOT_INFO_GRAPHICS_FLAG) == 0) {
        boot_state.vbe_info.vbe_बहुलकम् = -1;
        प्रारूप_लिख("Multiboot gave us no video information\n");
    } अन्यथा {
        boot_state.vbe_info.vbe_Info_खण्डम् = *(se_L4_VBEInfo_खण्डम्_t *)(seL4_Word)mbi->part2.vbe_control_info;
        boot_state.vbe_info.vbe_बहुलकम्_Info_खण्डम् = *(se_L4_VBEMode_Info_खण्डम्_t *)(seL4_Word)mbi->part2.vbe_बहुलकम्_info;
        boot_state.vbe_info.vbe_बहुलकम् = mbi->part2.vbe_बहुलकम्_2;
        प्रारूप_लिख("Got VBE info in multiboot. Current video mode is %d\n", mbi->part2.vbe_बहुलकम्_2);
        boot_state.vbe_info.vbeInterfaceSeg = mbi->part2.vbe_interface_seg;
        boot_state.vbe_info.vbeInterfaceOff = mbi->part2.vbe_interface_off;
        boot_state.vbe_info.vbeInterfaceLen = mbi->part2.vbe_interface_len;
    }

    boot_state.mem_अधस्तनम् = mbi->part1.mem_अधस्तनम्;
    boot_state.boot_module_start = modules->start;
    boot_state.boot_module_अन्तः = modules->अन्तः_2;

    /* Initialize ACPI */
    यदि (!acpi_init(&boot_state.acpi_rsdp)) {
        प्रत्यागम false;
    }

    प्रत्यागम true;
}

स्थिर BOOT_संहिता bool_t try_boot_sys_mbi2(
    multiboot2_शीर्षकम्_t *mbi2
)
{
    पूर्णाङ्क mod_गणनम्                  = 0;
    multiboot2_चिह्नकम्_t ध्रुव *चिह्नकम्   = (multiboot2_चिह्नकम्_t *)(mbi2 + 1);
    multiboot2_चिह्नकम्_t ध्रुव *चिह्नकम्_e = (multiboot2_चिह्नकम्_t *)((word_t)mbi2 + mbi2->योग_आकार);

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.गणनम् = 0;
    boot_state.mb_mmap_info.mmap_दीर्घता = 0;
    boot_state.vbe_info.vbe_बहुलकम् = -1;

    यावत् (चिह्नकम् < चिह्नकम्_e && चिह्नकम्->type != MULTIBOOT2_चिह्नकम्_अन्तः) {
        word_t ध्रुव behind_चिह्नकम् = (word_t)चिह्नकम् + परिमाण(*चिह्नकम्);

        यदि (चिह्नकम्->type == MULTIBOOT2_चिह्नकम्_CMDLINE) {
            वर्ण ध्रुव *ध्रुव cmdline = (वर्ण ध्रुव * ध्रुव)(behind_चिह्नकम्);
            cmdline_parse(cmdline, &cmdline_opt);
        } अन्यथा यदि (चिह्नकम्->type == MULTIBOOT2_चिह्नकम्_ACPI_1) {
            यदि (ACPI_V1_आकार == चिह्नकम्->आकार - परिमाण(*चिह्नकम्)) {
                स्मृतिं_प्रतिलिख(&boot_state.acpi_rsdp, (शून्य *)behind_चिह्नकम्, चिह्नकम्->आकार - परिमाण(*चिह्नकम्));
            }
        } अन्यथा यदि (चिह्नकम्->type == MULTIBOOT2_चिह्नकम्_ACPI_2) {
            यदि (परिमाण(boot_state.acpi_rsdp) == चिह्नकम्->आकार - परिमाण(*चिह्नकम्)) {
                स्मृतिं_प्रतिलिख(&boot_state.acpi_rsdp, (शून्य *)behind_चिह्नकम्, परिमाण(boot_state.acpi_rsdp));
            }
        } अन्यथा यदि (चिह्नकम्->type == MULTIBOOT2_चिह्नकम्_MODULE) {
            multiboot2_module_t ध्रुव *module = (multiboot2_module_t ध्रुव *)behind_चिह्नकम्;
            प्रारूप_लिख(
                "  module #%d: start=0x%x end=0x%x size=0x%x name='%s'\n",
                mod_गणनम्,
                module->start,
                module->अन्तः_2,
                module->अन्तः_2 - module->start,
                module->string
            );

            यदि (mod_गणनम् == 0) {
                boot_state.boot_module_start = module->start;
                boot_state.boot_module_अन्तः = module->अन्तः_2;
            }

            mod_गणनम् ++;
            यदि ((sword_t)(module->अन्तः_2 - module->start) <= 0) {
                प्रारूप_लिख("Invalid boot module size! Possible cause: boot module file not found\n");
                प्रत्यागम false;
            }
            यदि (boot_state.mods_अन्तः_paddr < module->अन्तः_2) {
                boot_state.mods_अन्तः_paddr = module->अन्तः_2;
            }
        } अन्यथा यदि (चिह्नकम्->type == MULTIBOOT2_चिह्नकम्_स्मृतिः) {
            multiboot2_स्मृतिः_t ध्रुव *s = (multiboot2_स्मृतिः_t *)(behind_चिह्नकम् + 8);
            multiboot2_स्मृतिः_t ध्रुव *e = (multiboot2_स्मृतिः_t *)((word_t)चिह्नकम् + चिह्नकम्->आकार);

            प्रति (multiboot2_स्मृतिः_t ध्रुव *m = s; m < e; m++) {
                यदि (!m->addr) {
                    boot_state.mem_अधस्तनम् = m->आकार;
                }

                प्रारूप_लिख("\tPhysical Memory Region from %llx size %llx type %u\n", m->addr, m->आकार, m->type);
                यदि (m->addr != (uint64_t)(word_t)m->addr) {
                    प्रारूप_लिख("\t\tPhysical memory region not addressable\n");
                    अनुवर्त;
                }

                यदि (m->type == MULTIBOOT_MMAP_USEABLE_TYPE && m->addr >= HIGHMEM_PADDR) {
                    यदि (!सङ्कलय_mem_p_regs((p_region_t) {
                    m->addr, m->addr + m->आकार
                }))
                    प्रत्यागम false;
                }
            }
        } अन्यथा यदि (चिह्नकम्->type == MULTIBOOT2_चिह्नकम्_FB) {
            multiboot2_fb_t ध्रुव *fb = (multiboot2_fb_t ध्रुव *)behind_चिह्नकम्;
            प्रारूप_लिख("Got framebuffer info in multiboot2. Current video mode is at physical address=%llx pitch=%u resolution=%ux%u@%u type=%u\n",
                   fb->addr, fb->pitch, fb->विस्तारः, fb->height, fb->bpp, fb->type);
            boot_state.fb_info = *fb;
        }

        चिह्नकम् = (multiboot2_चिह्नकम्_t ध्रुव *)((word_t)चिह्नकम् + आवर्तनम्_ऊर्ध्वम्(चिह्नकम्->आकार, 3));
    }

    प्रारूप_लिख("Detected %d boot module(s):\n", mod_गणनम्);

    यदि (mod_गणनम् < 1) {
        प्रारूप_लिख("Expect at least one boot module (containing a userland image)\n");
        प्रत्यागम false;
    }

    प्रत्यागम true;
}

BOOT_संहिता VISIBLE शून्य boot_sys(
    अचिह्नित दीर्घ multiboot_magic,
    शून्य *mbi)
{
    bool_t परिणाम = false;

    यदि (multiboot_magic == MULTIBOOT_MAGIC) {
        परिणाम = try_boot_sys_mbi1(mbi);
    } अन्यथा यदि (multiboot_magic == MULTIBOOT2_MAGIC) {
        परिणाम = try_boot_sys_mbi2(mbi);
    } अन्यथा {
        प्रारूप_लिख("Boot loader is not multiboot 1 or 2 compliant %lx\n", multiboot_magic);
    }

    यदि (परिणाम) {
        परिणाम = try_boot_sys();
    }

    यदि (!परिणाम) {
        fail("boot_sys failed for some reason :(\n");
    }

    ARCH_ग्रन्थिः_STATE(x86_KScur_व्यवधानम्) = int_invalid;
    ARCH_ग्रन्थिः_STATE(x86_KSPending_व्यवधानम्) = int_invalid;

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    ग्रन्थिः_STATE(ksCurTime) = getCurrentTime();
    ग्रन्थिः_STATE(ksConsumed) = 0;
#यदि_अन्तः

    कालसारणी();
    activateThread();
}

