/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <hardware.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/kernel/apic.h>
#అన్తర్భావ <arch/kernel/cmdline.h>
#అన్తర్భావ <arch/kernel/boot.h>
#అన్తర్భావ <arch/kernel/boot_sys.h>
#అన్తర్భావ <arch/kernel/smp_sys.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <arch/kernel/elf.h>
#అన్తర్భావ <smp/lock.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/acpi.h>
#అన్తర్భావ <plat/machine/devices.h>
#అన్తర్భావ <plat/machine/pic.h>
#అన్తర్భావ <plat/machine/ioapic.h>
#అన్తర్భావ <sel4/arch/bootinfo_types.h>

/* addresses defined in linker script */
/* need a fake array to get the pointer from the linker script */

/* start/end of CPU boot code */
బాహ్య అక్షరం boot_cpu_ప్రారంభం[1];
బాహ్య అక్షరం boot_cpu_చివర[1];

/* start/end of boot stack */
బాహ్య అక్షరం boot_దొంతర_bottom[1];
బాహ్య అక్షరం boot_దొంతర_శిఖరం[1];

/* locations in kernel image */
బాహ్య అక్షరం ki_skim_ప్రారంభం[1];
బాహ్య అక్షరం ki_skim_చివర[1];

#నిర్వచితమైతే CONFIG_PRINTING
/* kernel entry point */
బాహ్య అక్షరం ప్రారంభం_2[1];
#అయితే_ముగింపు

/* constants */

#నిర్వచించు HIGHMEM_PADDR 0x100000

BOOT_BSS
boot_state_t boot_state;

/* global variables (not covered by abstract specification) */

BOOT_BSS
cmdline_opt_t cmdline_opt;

/* functions not modeled in abstract specification */

BOOT_సంకేతం స్థిర paddr_t కనుగొను_ఎక్కించు_paddr(paddr_t min_paddr, మాట_t image_కొలత)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < boot_state.mem_p_regs.లెక్క; i++) {
        paddr_t ప్రారంభం = MAX(min_paddr, boot_state.mem_p_regs.list[i].ప్రారంభం);
        paddr_t చివర = boot_state.mem_p_regs.list[i].చివర;
        మాట_t region_కొలత = చివర - ప్రారంభం;

        ఒకవేళ (region_కొలత >= image_కొలత) {
            తిరిగి ప్రారంభం;
        }
    }

    తిరిగి 0;
}

BOOT_సంకేతం స్థిర paddr_t ఎక్కించు_boot_module(మాట_t boot_module_ప్రారంభం, మాట_t boot_module_చివర, paddr_t ఎక్కించు_paddr)
{
    v_region_t v_reg;
    మాట_t నమోదు_2;
    Elf_శీర్షిక_t *elf_file = (Elf_శీర్షిక_t *)boot_module_ప్రారంభం;

    ఒకవేళ (!elf_checkFile(elf_file, boot_module_చివర - boot_module_ప్రారంభం)) {
        రాయి("Boot module does not contain a valid ELF image\n");
        తిరిగి 0;
    }

    v_reg = elf_get_జ్ఞాపకం_Bounds(elf_file);
    నమోదు_2 = elf_file->e_నమోదు;

    ఒకవేళ (v_reg.చివర == 0) {
        రాయి("ELF image in boot module does not contain any segments\n");
        తిరిగి 0;
    }
    v_reg.చివర = విడత_పైకి(v_reg.చివర, పుట_బిట్లు);

    రాయి("size=0x%lx v_entry=%p v_start=%p v_end=%p ",
           v_reg.చివర - v_reg.ప్రారంభం,
           (శూన్య *)నమోదు_2,
           (శూన్య *)v_reg.ప్రారంభం,
           (శూన్య *)v_reg.చివర
          );

    ఒకవేళ (!IS_ALIGNED(v_reg.ప్రారంభం, పుట_బిట్లు)) {
        రాయి("Userland image virtual start address must be 4KB-aligned\n");
        తిరిగి 0;
    }
    ఒకవేళ (v_reg.చివర + 2 * బిట్టు(పుట_బిట్లు) > USER_శిఖరం) {
        /* for IPC buffer frame and bootinfo frame, need 2*4K of additional userland virtual memory */
        రాయి("Userland image virtual end address too high\n");
        తిరిగి 0;
    }
    ఒకవేళ ((నమోదు_2 < v_reg.ప్రారంభం) || (నమోదు_2 >= v_reg.చివర)) {
        రాయి("Userland image entry point does not lie within userland image\n");
        తిరిగి 0;
    }

    ఎక్కించు_paddr = కనుగొను_ఎక్కించు_paddr(ఎక్కించు_paddr, v_reg.చివర - v_reg.ప్రారంభం);
    ప్రతిజ్ఞా(ఎక్కించు_paddr);

    /* fill ui_info struct */
    boot_state.ui_info.pv_స్థానభ్రంశం = ఎక్కించు_paddr - v_reg.ప్రారంభం;
    boot_state.ui_info.p_reg.ప్రారంభం = ఎక్కించు_paddr;
    ఎక్కించు_paddr += v_reg.చివర - v_reg.ప్రారంభం;
    boot_state.ui_info.p_reg.చివర = ఎక్కించు_paddr;
    boot_state.ui_info.v_నమోదు = నమోదు_2;

    రాయి("p_start=0x%lx p_end=0x%lx\n",
           boot_state.ui_info.p_reg.ప్రారంభం,
           boot_state.ui_info.p_reg.చివర
          );

    /* initialise all initial userland memory and load potentially sparse ELF image */
    memzero(
        (శూన్య *)boot_state.ui_info.p_reg.ప్రారంభం,
        boot_state.ui_info.p_reg.చివర - boot_state.ui_info.p_reg.ప్రారంభం
    );
    elf_ఎక్కించు(elf_file, boot_state.ui_info.pv_స్థానభ్రంశం);

    తిరిగి ఎక్కించు_paddr;
}

స్థిర BOOT_సంకేతం bool_t try_boot_sys_కణుపు(cpu_id_t cpu_id)
{
    ఒకవేళ (!map_kernel_కిటికీ(
            boot_state.అంకె_ioapic,
            boot_state.ioapic_paddr,
            boot_state.అంకె_drhu,
            boot_state.drhu_list
        )) {
        తిరిగి false;
    }
    సమితి_Current_VSpace_మూలం(kpptr_గమ్యం_paddr(X86_KERNEL_VSPACE_మూలం), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm అస్థిర("" ::: "memory");

#నిర్వచితమైతే CONFIG_KERNEL_SKIM_కిటికీ
    ఒకవేళ (!map_skim_కిటికీ((vptr_t)ki_skim_ప్రారంభం, (vptr_t)ki_skim_చివర)) {
        తిరిగి false;
    }
#అయితే_ముగింపు

    /* initialise the CPU */
    ఒకవేళ (!init_cpu(config_సమితి(CONFIG_IRQ_IOAPIC) ? 1 : 0)) {
        తిరిగి false;
    }

    /* initialise NDKS and kernel heap */
    ఒకవేళ (!init_sys_state(
            cpu_id,
            &boot_state.mem_p_regs,
            boot_state.ui_info,
            /* parameters below not modelled in abstract specification */
            boot_state.అంకె_drhu,
            boot_state.drhu_list,
            &boot_state.rmrr_list,
            &boot_state.acpi_rsdp,
            &boot_state.vbe_info,
            &boot_state.mb_mmap_info,
            &boot_state.fb_info
        )) {
        తిరిగి false;
    }

    తిరిగి true;
}

స్థిర BOOT_సంకేతం bool_t కూడు_mem_p_regs(p_region_t reg)
{
    ఒకవేళ (reg.ప్రారంభం == reg.చివర) {
        // Return true here if asked to add an empty region.
        // Some of the callers round down the end address to
        తిరిగి true;
    }
    ఒకవేళ (reg.చివర > PADDR_శిఖరం && reg.ప్రారంభం >= PADDR_శిఖరం) {
        /* Return true here as it's not an error for there to exist memory outside the kernel window,
         * we're just going to ignore it and leave it to be given out as device memory */
        తిరిగి true;
    }
    ఒకవేళ (boot_state.mem_p_regs.లెక్క == MAX_అంకె_FREEMEM_REG) {
        రాయి("Dropping memory region 0x%lx-0x%lx, try increasing MAX_NUM_FREEMEM_REG\n", reg.ప్రారంభం, reg.చివర);
        తిరిగి false;
    }
    ఒకవేళ (reg.చివర > PADDR_శిఖరం) {
        ప్రతిజ్ఞా(reg.ప్రారంభం < PADDR_శిఖరం);
        /* Clamp a region to the top of the kernel window if it extends beyond */
        reg.చివర = PADDR_శిఖరం;
    }
    రాయి("Adding physical memory region 0x%lx-0x%lx\n", reg.ప్రారంభం, reg.చివర);
    boot_state.mem_p_regs.list[boot_state.mem_p_regs.లెక్క] = reg;
    boot_state.mem_p_regs.లెక్క++;
    తిరిగి true;
}

/*
 * the code relies that the GRUB provides correct information
 * about the actual physical memory regions.
 */
స్థిర BOOT_సంకేతం bool_t parse_mem_map(uint32_t mmap_నిడివి, uint32_t mmap_addr)
{
    multiboot_mmap_t *mmap = (multiboot_mmap_t *)((మాట_t)mmap_addr);
    రాయి("Parsing GRUB physical memory map\n");

    అయితే ((మాట_t)mmap < (మాట_t)(mmap_addr + mmap_నిడివి)) {
        uint64_t mem_ప్రారంభం = mmap->పునాది_addr;
        uint64_t mem_నిడివి = mmap->నిడివి;
        uint32_t type = mmap->type;
        ఒకవేళ (mem_ప్రారంభం != (uint64_t)(మాట_t)mem_ప్రారంభం) {
            రాయి("\tPhysical memory region not addressable\n");
        } లేకపోతే {
            రాయి("\tPhysical Memory Region from %lx size %lx type %d\n", (దీర్ఘ)mem_ప్రారంభం, (దీర్ఘ)mem_నిడివి, type);
            ఒకవేళ (type == MULTIBOOT_MMAP_USEABLE_TYPE && mem_ప్రారంభం >= HIGHMEM_PADDR && mem_నిడివి >= బిట్టు(పుట_బిట్లు)) {

                ఒకవేళ (!కూడు_mem_p_regs((p_region_t) {
                విడత_పైకి(mem_ప్రారంభం, పుట_బిట్లు), విడత_కిందికి(mem_ప్రారంభం + mem_నిడివి, పుట_బిట్లు),
                })) {
                    తిరిగి false;
                }
            }
        }
        mmap++;
    }
    తిరిగి true;
}

స్థిర BOOT_సంకేతం bool_t is_compiled_for_microarchitecture(శూన్య)
{
    మాట_t microarch_generation = 0;
    x86_cpu_తటస్థం_t *model_info = x86_cpuid_get_model_info();

    ఒకవేళ (config_సమితి(CONFIG_ARCH_X86_SKYLAKE)) {
        microarch_generation = 7;
    } లేకపోతే ఒకవేళ (config_సమితి(CONFIG_ARCH_X86_BROADWELL)) {
        microarch_generation = 6;
    } లేకపోతే ఒకవేళ (config_సమితి(CONFIG_ARCH_X86_HASWELL)) {
        microarch_generation = 5;
    } లేకపోతే ఒకవేళ (config_సమితి(CONFIG_ARCH_X86_IVY)) {
        microarch_generation = 4;
    } లేకపోతే ఒకవేళ (config_సమితి(CONFIG_ARCH_X86_SANDY)) {
        microarch_generation = 3;
    } లేకపోతే ఒకవేళ (config_సమితి(CONFIG_ARCH_X86_WESTMERE)) {
        microarch_generation = 2;
    } లేకపోతే ఒకవేళ (config_సమితి(CONFIG_ARCH_X86_NEHALEM)) {
        microarch_generation = 1;
    }

    ఎంపిక (model_info->model) {
    స్థితి SKYLAKE_1_MODEL_ID:
    స్థితి SKYLAKE_2_MODEL_ID:
    స్థితి SKYLAKE_X_MODEL_ID:
        ఒకవేళ (microarch_generation > 7) {
            తిరిగి false;
        }
        ఆపు;

    స్థితి BROADWELL_1_MODEL_ID:
    స్థితి BROADWELL_2_MODEL_ID:
    స్థితి BROADWELL_3_MODEL_ID:
    స్థితి BROADWELL_4_MODEL_ID:
    స్థితి BROADWELL_5_MODEL_ID:
        ఒకవేళ (microarch_generation > 6) {
            తిరిగి false;
        }
        ఆపు;

    స్థితి HASWELL_1_MODEL_ID:
    స్థితి HASWELL_2_MODEL_ID:
    స్థితి HASWELL_3_MODEL_ID:
    స్థితి HASWELL_4_MODEL_ID:
        ఒకవేళ (microarch_generation > 5) {
            తిరిగి false;
        }
        ఆపు;

    స్థితి IVY_BRIDGE_1_MODEL_ID:
    స్థితి IVY_BRIDGE_2_MODEL_ID:
    స్థితి IVY_BRIDGE_3_MODEL_ID:
        ఒకవేళ (microarch_generation > 4) {
            తిరిగి false;
        }
        ఆపు;

    స్థితి SANDY_BRIDGE_1_MODEL_ID:
    స్థితి SANDY_BRIDGE_2_MODEL_ID:
        ఒకవేళ (microarch_generation > 3) {
            తిరిగి false;
        }
        ఆపు;

    స్థితి WESTMERE_1_MODEL_ID:
    స్థితి WESTMERE_2_MODEL_ID:
    స్థితి WESTMERE_3_MODEL_ID:
        ఒకవేళ (microarch_generation > 2) {
            తిరిగి false;
        }
        ఆపు;

    స్థితి NEHALEM_1_MODEL_ID:
    స్థితి NEHALEM_2_MODEL_ID:
    స్థితి NEHALEM_3_MODEL_ID:
        ఒకవేళ (microarch_generation > 1) {
            తిరిగి false;
        }
        ఆపు;

    అప్రమేయ:
        ఒకవేళ (!config_సమితి(CONFIG_ARCH_X86_GENERIC)) {
            తిరిగి false;
        }
    }

    తిరిగి true;
}

స్థిర BOOT_సంకేతం bool_t try_boot_sys(శూన్య)
{
    paddr_t mods_చివర_paddr = boot_state.mods_చివర_paddr;
    p_region_t ui_p_regs;
    paddr_t ఎక్కించు_paddr;

    boot_state.ki_p_reg = get_p_reg_kernel_img();

    ఒకవేళ (!x86_cpuid_initialize()) {
        రాయి("Warning: Your x86 CPU has an unsupported vendor, '%s'.\n"
               "\tYour setup may not be able to competently run seL4 as "
               "intended.\n"
               "\tCurrently supported x86 vendors are AMD and Intel.\n",
               x86_cpuid_get_తటస్థం()->vendor_string);
    }

    ఒకవేళ (!is_compiled_for_microarchitecture()) {
        రాయి("Warning: Your kernel was not compiled for the current microarchitecture.\n");
    }

    cpuid_007h_edx_t edx;
    edx.పదాలు[0] = x86_cpuid_edx(0x7, 0);
    /* see if we can definitively say whether or not we need the skim window by
     * checking whether the CPU is vulnerable to rogue data cache loads (rdcl) */
    ఒకవేళ (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.పదాలు[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        ఒకవేళ (ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && config_సమితి(CONFIG_KERNEL_SKIM_కిటికీ)) {
            రాయి("CPU reports not vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is enabled. Performance is needlessly being impacted, consider disabling.\n");
        } లేకపోతే ఒకవేళ (!ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && !config_సమితి(CONFIG_KERNEL_SKIM_కిటికీ)) {
            రాయి("CPU reports vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is *not* enabled. Please re-build with SKIM window enabled.");
            తిరిగి false;
        }
    } లేకపోతే {
        /* hardware doesn't tell us directly so guess based on CPU vendor */
        ఒకవేళ (config_సమితి(CONFIG_KERNEL_SKIM_కిటికీ) && x86_cpuid_get_తటస్థం()->vendor == X86_VENDOR_AMD) {
            రాయి("SKIM window for mitigating Meltdown (https://www.meltdownattack.com) "
                   "not necessary for AMD and performance is being needlessly affected, "
                   "consider disabling\n");
        }
        ఒకవేళ (!config_సమితి(CONFIG_KERNEL_SKIM_కిటికీ) && x86_cpuid_get_తటస్థం()->vendor == X86_VENDOR_INTEL) {
            రాయి("***WARNING*** SKIM window not enabled, this machine is probably vulnerable "
                   "to Meltdown (https://www.meltdownattack.com), consider enabling\n");
        }
    }

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    /* copy boot code for APs to lower memory to run in real mode */
    ఒకవేళ (!copy_boot_సంకేతం_aps(boot_state.mem_క్రింది)) {
        తిరిగి false;
    }
    /* Initialize any kernel TLS */
    బహుళకం_init_tls(0);
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

    రాయి("Kernel loaded to: start=0x%lx end=0x%lx size=0x%lx entry=0x%lx\n",
           boot_state.ki_p_reg.ప్రారంభం,
           boot_state.ki_p_reg.చివర,
           boot_state.ki_p_reg.చివర - boot_state.ki_p_reg.ప్రారంభం,
           (paddr_t)ప్రారంభం_2
          );

    /* remapping legacy IRQs to their correct vectors */
    pic_remap_irqs(IRQ_INT_స్థానభ్రంశం);
    ఒకవేళ (config_సమితి(CONFIG_IRQ_IOAPIC)) {
        /* Disable the PIC so that it does not generate any interrupts. We need to
         * do this *before* we initialize the apic */
        pic_disable();
    }

    /* validate the ACPI table */
    ఒకవేళ (!acpi_validate_rsdp(&boot_state.acpi_rsdp)) {
        తిరిగి false;
    }

    /* check if kernel configuration matches platform requirements */
    ఒకవేళ (!acpi_fadt_పరిశీలన(&boot_state.acpi_rsdp)) {
        తిరిగి false;
    }

    ఒకవేళ (!config_సమితి(CONFIG_IOMMU) || cmdline_opt.disable_iommu) {
        boot_state.అంకె_drhu = 0;
    } లేకపోతే {
        /* query available IOMMUs from ACPI */
        acpi_dmar_పరిశీలన(
            &boot_state.acpi_rsdp,
            boot_state.drhu_list,
            &boot_state.అంకె_drhu,
            MAX_అంకె_DRHU,
            &boot_state.rmrr_list
        );
    }

    /* query available CPUs from ACPI */
    boot_state.అంకె_cpus = acpi_madt_పరిశీలన(&boot_state.acpi_rsdp, boot_state.cpus, &boot_state.అంకె_ioapic,
                                         boot_state.ioapic_paddr);
    ఒకవేళ (boot_state.అంకె_cpus == 0) {
        రాయి("No CPUs detected\n");
        తిరిగి false;
    }

    ఒకవేళ (config_సమితి(CONFIG_IRQ_IOAPIC)) {
        ఒకవేళ (boot_state.అంకె_ioapic == 0) {
            రాయి("No IOAPICs detected\n");
            తిరిగి false;
        }
    } లేకపోతే {
        ఒకవేళ (boot_state.అంకె_ioapic > 0) {
            రాయి("Detected %d IOAPICs, but configured to use PIC instead\n", boot_state.అంకె_ioapic);
        }
    }

    mods_చివర_paddr = విడత_పైకి(mods_చివర_paddr, పుట_బిట్లు);
    ప్రతిజ్ఞా(mods_చివర_paddr > boot_state.ki_p_reg.చివర);

    రాయి("ELF-loading userland images from boot modules:\n");
    ఎక్కించు_paddr = mods_చివర_paddr;

    ఎక్కించు_paddr = ఎక్కించు_boot_module(boot_state.boot_module_ప్రారంభం, boot_state.boot_module_చివర, ఎక్కించు_paddr);
    ఒకవేళ (!ఎక్కించు_paddr) {
        తిరిగి false;
    }

    /* calculate final location of userland images */
    ui_p_regs.ప్రారంభం = boot_state.ki_p_reg.చివర;
    ui_p_regs.చివర = ui_p_regs.ప్రారంభం + ఎక్కించు_paddr - mods_చివర_paddr;

    రాయి(
        "Moving loaded userland images to final location: from=0x%lx to=0x%lx size=0x%lx\n",
        mods_చివర_paddr,
        ui_p_regs.ప్రారంభం,
        ui_p_regs.చివర - ui_p_regs.ప్రారంభం
    );
    జ్ఞాపకం_నకలు((శూన్య *)ui_p_regs.ప్రారంభం, (శూన్య *)mods_చివర_paddr, ui_p_regs.చివర - ui_p_regs.ప్రారంభం);

    /* adjust p_reg and pv_offset to final load address */
    boot_state.ui_info.p_reg.ప్రారంభం -= mods_చివర_paddr - ui_p_regs.ప్రారంభం;
    boot_state.ui_info.p_reg.చివర   -= mods_చివర_paddr - ui_p_regs.ప్రారంభం;
    boot_state.ui_info.pv_స్థానభ్రంశం   -= mods_చివర_paddr - ui_p_regs.ప్రారంభం;

    /* ==== following code corresponds to abstract specification after "select" ==== */

    ఒకవేళ (!plat_కూడు_Devices()) {
        తిరిగి false;
    }

    /* Total number of cores we intend to boot */
    ks_అంకె_CPUs = boot_state.అంకె_cpus;

    రాయి("Starting node #0 with APIC ID %lu\n", boot_state.cpus[0]);
    ఒకవేళ (!try_boot_sys_కణుపు(boot_state.cpus[0])) {
        తిరిగి false;
    }

    ఒకవేళ (config_సమితి(CONFIG_IRQ_IOAPIC)) {
        ioapic_init(1, boot_state.cpus, boot_state.అంకె_ioapic);
    }

    /* initialize BKL before booting up APs */
    SMP_COND_STATEMENT(clh_తాళం_init());
    SMP_COND_STATEMENT(ప్రారంభం_boot_aps());

    /* grab BKL before leaving the kernel */
    కణుపు_తాళం_SYS;

    రాయి("Booting all finished, dropped to user space\n");

    తిరిగి true;
}

స్థిర BOOT_సంకేతం bool_t try_boot_sys_mbi1(
    multiboot_info_t *mbi
)
{
    మాట_t i;
    multiboot_module_t *modules = (multiboot_module_t *)(మాట_t)mbi->part1.mod_list;

    cmdline_parse((స్థిరరాశి అక్షరం *)(మాట_t)mbi->part1.cmdline, &cmdline_opt);

    ఒకవేళ ((mbi->part1.flags & MULTIBOOT_INFO_MEM_FLAG) == 0) {
        రాయి("Boot loader did not provide information about physical memory size\n");
        తిరిగి false;
    }

    ఒకవేళ (!(mbi->part1.flags & MULTIBOOT_INFO_MODS_FLAG)) {
        రాయి("Boot loader did not provide information about boot modules\n");
        తిరిగి false;
    }

    రాయి("Detected %d boot module(s):\n", mbi->part1.mod_లెక్క);

    ఒకవేళ (mbi->part1.mod_లెక్క < 1) {
        రాయి("Expect at least one boot module (containing a userland image)\n");
        తిరిగి false;
    }

    కొరకు (i = 0; i < mbi->part1.mod_లెక్క; i++) {
        రాయి(
            "  module #%ld: start=0x%x end=0x%x size=0x%x name='%s'\n",
            i,
            modules[i].ప్రారంభం,
            modules[i].చివర,
            modules[i].చివర - modules[i].ప్రారంభం,
            (అక్షరం *)(దీర్ఘ)modules[i].name
        );
        ఒకవేళ ((sword_t)(modules[i].చివర - modules[i].ప్రారంభం) <= 0) {
            రాయి("Invalid boot module size! Possible cause: boot module file not found by QEMU\n");
            తిరిగి false;
        }
        ఒకవేళ (boot_state.mods_చివర_paddr < modules[i].చివర) {
            boot_state.mods_చివర_paddr = modules[i].చివర;
        }
    }

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.లెక్క = 0;
    ఒకవేళ (mbi->part1.flags & MULTIBOOT_INFO_MMAP_FLAG) {
        ఒకవేళ (!parse_mem_map(mbi->part2.mmap_నిడివి, mbi->part2.mmap_addr)) {
            తిరిగి false;
        }
        uint32_t multiboot_mmap_నిడివి = mbi->part2.mmap_నిడివి;
        ఒకవేళ (multiboot_mmap_నిడివి > (SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * పరిమాణం(seL4_X86_mb_mmap_t))) {
            రాయి("Warning: Multiboot has reported more memory map entries, %zd, "
                   "than the max amount that will be passed in the bootinfo, %d. "
                   "These extra regions will still be turned into untyped caps.",
                   multiboot_mmap_నిడివి / పరిమాణం(seL4_X86_mb_mmap_t), SEL4_MULTIBOOT_MAX_MMAP_ENTRIES);
            multiboot_mmap_నిడివి = SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * పరిమాణం(seL4_X86_mb_mmap_t);
        }
        జ్ఞాపకం_నకలు(&boot_state.mb_mmap_info.mmap, (శూన్య *)(మాట_t)mbi->part2.mmap_addr, multiboot_mmap_నిడివి);
        boot_state.mb_mmap_info.mmap_నిడివి = multiboot_mmap_నిడివి;
    } లేకపోతే {
        /* calculate memory the old way */
        p_region_t avail;
        avail.ప్రారంభం = HIGHMEM_PADDR;
        avail.చివర = విడత_కిందికి(avail.ప్రారంభం + (mbi->part1.mem_పై << 10), పుట_బిట్లు);
        ఒకవేళ (!కూడు_mem_p_regs(avail)) {
            తిరిగి false;
        }
    }

    /* copy VESA information from multiboot header */
    ఒకవేళ ((mbi->part1.flags & MULTIBOOT_INFO_GRAPHICS_FLAG) == 0) {
        boot_state.vbe_info.vbe_బహుళకం = -1;
        రాయి("Multiboot gave us no video information\n");
    } లేకపోతే {
        boot_state.vbe_info.vbe_Info_ఖండం = *(se_L4_VBEInfo_ఖండం_t *)(se_L4_మాట)mbi->part2.vbe_control_info;
        boot_state.vbe_info.vbe_బహుళకం_Info_ఖండం = *(se_L4_VBEMode_Info_ఖండం_t *)(se_L4_మాట)mbi->part2.vbe_బహుళకం_info;
        boot_state.vbe_info.vbe_బహుళకం = mbi->part2.vbe_బహుళకం_2;
        రాయి("Got VBE info in multiboot. Current video mode is %d\n", mbi->part2.vbe_బహుళకం_2);
        boot_state.vbe_info.vbeInterfaceSeg = mbi->part2.vbe_interface_seg;
        boot_state.vbe_info.vbeInterfaceOff = mbi->part2.vbe_interface_off;
        boot_state.vbe_info.vbeInterfaceLen = mbi->part2.vbe_interface_len;
    }

    boot_state.mem_క్రింది = mbi->part1.mem_క్రింది;
    boot_state.boot_module_ప్రారంభం = modules->ప్రారంభం;
    boot_state.boot_module_చివర = modules->చివర;

    /* Initialize ACPI */
    ఒకవేళ (!acpi_init(&boot_state.acpi_rsdp)) {
        తిరిగి false;
    }

    తిరిగి true;
}

స్థిర BOOT_సంకేతం bool_t try_boot_sys_mbi2(
    multiboot2_శీర్షిక_t *mbi2
)
{
    పూర్ణాంకం mod_లెక్క                  = 0;
    multiboot2_ట్యాగ్_t స్థిరరాశి *ట్యాగ్   = (multiboot2_ట్యాగ్_t *)(mbi2 + 1);
    multiboot2_ట్యాగ్_t స్థిరరాశి *ట్యాగ్_e = (multiboot2_ట్యాగ్_t *)((మాట_t)mbi2 + mbi2->మొత్తం_కొలత);

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.లెక్క = 0;
    boot_state.mb_mmap_info.mmap_నిడివి = 0;
    boot_state.vbe_info.vbe_బహుళకం = -1;

    అయితే (ట్యాగ్ < ట్యాగ్_e && ట్యాగ్->type != MULTIBOOT2_ట్యాగ్_చివర) {
        మాట_t స్థిరరాశి behind_ట్యాగ్ = (మాట_t)ట్యాగ్ + పరిమాణం(*ట్యాగ్);

        ఒకవేళ (ట్యాగ్->type == MULTIBOOT2_ట్యాగ్_CMDLINE) {
            అక్షరం స్థిరరాశి *స్థిరరాశి cmdline = (అక్షరం స్థిరరాశి * స్థిరరాశి)(behind_ట్యాగ్);
            cmdline_parse(cmdline, &cmdline_opt);
        } లేకపోతే ఒకవేళ (ట్యాగ్->type == MULTIBOOT2_ట్యాగ్_ACPI_1) {
            ఒకవేళ (ACPI_V1_కొలత == ట్యాగ్->కొలత - పరిమాణం(*ట్యాగ్)) {
                జ్ఞాపకం_నకలు(&boot_state.acpi_rsdp, (శూన్య *)behind_ట్యాగ్, ట్యాగ్->కొలత - పరిమాణం(*ట్యాగ్));
            }
        } లేకపోతే ఒకవేళ (ట్యాగ్->type == MULTIBOOT2_ట్యాగ్_ACPI_2) {
            ఒకవేళ (పరిమాణం(boot_state.acpi_rsdp) == ట్యాగ్->కొలత - పరిమాణం(*ట్యాగ్)) {
                జ్ఞాపకం_నకలు(&boot_state.acpi_rsdp, (శూన్య *)behind_ట్యాగ్, పరిమాణం(boot_state.acpi_rsdp));
            }
        } లేకపోతే ఒకవేళ (ట్యాగ్->type == MULTIBOOT2_ట్యాగ్_MODULE) {
            multiboot2_module_t స్థిరరాశి *module = (multiboot2_module_t స్థిరరాశి *)behind_ట్యాగ్;
            రాయి(
                "  module #%d: start=0x%x end=0x%x size=0x%x name='%s'\n",
                mod_లెక్క,
                module->ప్రారంభం,
                module->చివర,
                module->చివర - module->ప్రారంభం,
                module->string
            );

            ఒకవేళ (mod_లెక్క == 0) {
                boot_state.boot_module_ప్రారంభం = module->ప్రారంభం;
                boot_state.boot_module_చివర = module->చివర;
            }

            mod_లెక్క ++;
            ఒకవేళ ((sword_t)(module->చివర - module->ప్రారంభం) <= 0) {
                రాయి("Invalid boot module size! Possible cause: boot module file not found\n");
                తిరిగి false;
            }
            ఒకవేళ (boot_state.mods_చివర_paddr < module->చివర) {
                boot_state.mods_చివర_paddr = module->చివర;
            }
        } లేకపోతే ఒకవేళ (ట్యాగ్->type == MULTIBOOT2_ట్యాగ్_జ్ఞాపకం) {
            multiboot2_జ్ఞాపకం_t స్థిరరాశి *s = (multiboot2_జ్ఞాపకం_t *)(behind_ట్యాగ్ + 8);
            multiboot2_జ్ఞాపకం_t స్థిరరాశి *e = (multiboot2_జ్ఞాపకం_t *)((మాట_t)ట్యాగ్ + ట్యాగ్->కొలత);

            కొరకు (multiboot2_జ్ఞాపకం_t స్థిరరాశి *m = s; m < e; m++) {
                ఒకవేళ (!m->addr) {
                    boot_state.mem_క్రింది = m->కొలత;
                }

                రాయి("\tPhysical Memory Region from %llx size %llx type %u\n", m->addr, m->కొలత, m->type);
                ఒకవేళ (m->addr != (uint64_t)(మాట_t)m->addr) {
                    రాయి("\t\tPhysical memory region not addressable\n");
                    కొనసాగు;
                }

                ఒకవేళ (m->type == MULTIBOOT_MMAP_USEABLE_TYPE && m->addr >= HIGHMEM_PADDR) {
                    ఒకవేళ (!కూడు_mem_p_regs((p_region_t) {
                    m->addr, m->addr + m->కొలత
                }))
                    తిరిగి false;
                }
            }
        } లేకపోతే ఒకవేళ (ట్యాగ్->type == MULTIBOOT2_ట్యాగ్_FB) {
            multiboot2_fb_t స్థిరరాశి *fb = (multiboot2_fb_t స్థిరరాశి *)behind_ట్యాగ్;
            రాయి("Got framebuffer info in multiboot2. Current video mode is at physical address=%llx pitch=%u resolution=%ux%u@%u type=%u\n",
                   fb->addr, fb->pitch, fb->వెడల్పు, fb->ఎత్తు, fb->bpp, fb->type);
            boot_state.fb_info = *fb;
        }

        ట్యాగ్ = (multiboot2_ట్యాగ్_t స్థిరరాశి *)((మాట_t)ట్యాగ్ + విడత_పైకి(ట్యాగ్->కొలత, 3));
    }

    రాయి("Detected %d boot module(s):\n", mod_లెక్క);

    ఒకవేళ (mod_లెక్క < 1) {
        రాయి("Expect at least one boot module (containing a userland image)\n");
        తిరిగి false;
    }

    తిరిగి true;
}

BOOT_సంకేతం VISIBLE శూన్య boot_sys(
    గుర్తులేని దీర్ఘ multiboot_magic,
    శూన్య *mbi)
{
    bool_t ఫలితం = false;

    ఒకవేళ (multiboot_magic == MULTIBOOT_MAGIC) {
        ఫలితం = try_boot_sys_mbi1(mbi);
    } లేకపోతే ఒకవేళ (multiboot_magic == MULTIBOOT2_MAGIC) {
        ఫలితం = try_boot_sys_mbi2(mbi);
    } లేకపోతే {
        రాయి("Boot loader is not multiboot 1 or 2 compliant %lx\n", multiboot_magic);
    }

    ఒకవేళ (ఫలితం) {
        ఫలితం = try_boot_sys();
    }

    ఒకవేళ (!ఫలితం) {
        fail("boot_sys failed for some reason :(\n");
    }

    ARCH_కణుపు_STATE(x86_KScur_అంతరాయం) = int_invalid;
    ARCH_కణుపు_STATE(x86_KSPending_అంతరాయం) = int_invalid;

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    కణుపు_STATE(ks_Cur_సమయం) = get_Current_సమయం();
    కణుపు_STATE(ksConsumed) = 0;
#అయితే_ముగింపు

    కాలపట్టిక();
    activateThread();
}

