/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <util.h>
#ضمن <hardware.h>
#ضمن <machine/io.h>
#ضمن <arch/machine.h>
#ضمن <arch/kernel/apic.h>
#ضمن <arch/kernel/cmdline.h>
#ضمن <arch/kernel/boot.h>
#ضمن <arch/kernel/boot_sys.h>
#ضمن <arch/kernel/smp_sys.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <arch/kernel/elf.h>
#ضمن <smp/lock.h>
#ضمن <linker.h>
#ضمن <plat/machine/acpi.h>
#ضمن <plat/machine/devices.h>
#ضمن <plat/machine/pic.h>
#ضمن <plat/machine/ioapic.h>
#ضمن <sel4/arch/bootinfo_types.h>

/* addresses defined in linker script */
/* need a fake array to get the pointer from the linker script */

/* start/end of CPU boot code */
خارجي حرف boot_cpu_شروعات[1];
خارجي حرف boot_cpu_پڇاڙي[1];

/* start/end of boot stack */
خارجي حرف boot_ڍير_bottom[1];
خارجي حرف boot_ڍير_چوٽي[1];

/* locations in kernel image */
خارجي حرف ki_skim_شروعات[1];
خارجي حرف ki_skim_پڇاڙي[1];

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
/* kernel entry point */
خارجي حرف شروعات_2[1];
#جيڪڏهن_پورو

/* constants */

#وصف_ڪريو HIGHMEM_PADDR 0x100000

BOOT_BSS
boot_state_t boot_state;

/* global variables (not covered by abstract specification) */

BOOT_BSS
cmdline_opt_t cmdline_opt;

/* functions not modeled in abstract specification */

BOOT_ڪوڊ ساكن paddr_t ڳوليو_ڀريو_paddr(paddr_t min_paddr, لفظ_t image_سائيز)
{
    صحيح i;

    لكل (i = 0; i < boot_state.mem_p_regs.ڳڻپ; i++) {
        paddr_t شروعات = MAX(min_paddr, boot_state.mem_p_regs.list[i].شروعات);
        paddr_t پڇاڙي = boot_state.mem_p_regs.list[i].پڇاڙي;
        لفظ_t region_سائيز = پڇاڙي - شروعات;

        إذا (region_سائيز >= image_سائيز) {
            أرجع شروعات;
        }
    }

    أرجع 0;
}

BOOT_ڪوڊ ساكن paddr_t ڀريو_boot_module(لفظ_t boot_module_شروعات, لفظ_t boot_module_پڇاڙي, paddr_t ڀريو_paddr)
{
    v_region_t v_reg;
    لفظ_t داخلا;
    Elf_سرو_t *elf_file = (Elf_سرو_t *)boot_module_شروعات;

    إذا (!elf_checkFile(elf_file, boot_module_پڇاڙي - boot_module_شروعات)) {
        لکو("Boot module does not contain a valid ELF image\n");
        أرجع 0;
    }

    v_reg = elf_get_يادگيري_Bounds(elf_file);
    داخلا = elf_file->e_داخلا;

    إذا (v_reg.پڇاڙي == 0) {
        لکو("ELF image in boot module does not contain any segments\n");
        أرجع 0;
    }
    v_reg.پڇاڙي = ڦيرو_مٿي(v_reg.پڇاڙي, صفحو_بٽ);

    لکو("size=0x%lx v_entry=%p v_start=%p v_end=%p ",
           v_reg.پڇاڙي - v_reg.شروعات,
           (فراغ *)داخلا,
           (فراغ *)v_reg.شروعات,
           (فراغ *)v_reg.پڇاڙي
          );

    إذا (!IS_ALIGNED(v_reg.شروعات, صفحو_بٽ)) {
        لکو("Userland image virtual start address must be 4KB-aligned\n");
        أرجع 0;
    }
    إذا (v_reg.پڇاڙي + 2 * ٻٽو_انگ(صفحو_بٽ) > USER_چوٽي) {
        /* for IPC buffer frame and bootinfo frame, need 2*4K of additional userland virtual memory */
        لکو("Userland image virtual end address too high\n");
        أرجع 0;
    }
    إذا ((داخلا < v_reg.شروعات) || (داخلا >= v_reg.پڇاڙي)) {
        لکو("Userland image entry point does not lie within userland image\n");
        أرجع 0;
    }

    ڀريو_paddr = ڳوليو_ڀريو_paddr(ڀريو_paddr, v_reg.پڇاڙي - v_reg.شروعات);
    أكد(ڀريو_paddr);

    /* fill ui_info struct */
    boot_state.ui_info.pv_وٿي = ڀريو_paddr - v_reg.شروعات;
    boot_state.ui_info.p_reg.شروعات = ڀريو_paddr;
    ڀريو_paddr += v_reg.پڇاڙي - v_reg.شروعات;
    boot_state.ui_info.p_reg.پڇاڙي = ڀريو_paddr;
    boot_state.ui_info.v_داخلا = داخلا;

    لکو("p_start=0x%lx p_end=0x%lx\n",
           boot_state.ui_info.p_reg.شروعات,
           boot_state.ui_info.p_reg.پڇاڙي
          );

    /* initialise all initial userland memory and load potentially sparse ELF image */
    memzero(
        (فراغ *)boot_state.ui_info.p_reg.شروعات,
        boot_state.ui_info.p_reg.پڇاڙي - boot_state.ui_info.p_reg.شروعات
    );
    elf_ڀريو(elf_file, boot_state.ui_info.pv_وٿي);

    أرجع ڀريو_paddr;
}

ساكن BOOT_ڪوڊ bool_t try_boot_sys_ڳنڍ(cpu_id_t cpu_id)
{
    إذا (!map_kernel_دري(
            boot_state.انگ_ioapic,
            boot_state.ioapic_paddr,
            boot_state.انگ_drhu,
            boot_state.drhu_list
        )) {
        أرجع false;
    }
    سيٽ_Current_VSpace_پاڙ(kpptr_منزل_paddr(X86_KERNEL_VSPACE_پاڙ), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm متطاير("" ::: "memory");

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_SKIM_دري
    إذا (!map_skim_دري((vptr_t)ki_skim_شروعات, (vptr_t)ki_skim_پڇاڙي)) {
        أرجع false;
    }
#جيڪڏهن_پورو

    /* initialise the CPU */
    إذا (!init_cpu(config_سيٽ(CONFIG_IRQ_IOAPIC) ? 1 : 0)) {
        أرجع false;
    }

    /* initialise NDKS and kernel heap */
    إذا (!init_sys_state(
            cpu_id,
            &boot_state.mem_p_regs,
            boot_state.ui_info,
            /* parameters below not modelled in abstract specification */
            boot_state.انگ_drhu,
            boot_state.drhu_list,
            &boot_state.rmrr_list,
            &boot_state.acpi_rsdp,
            &boot_state.vbe_info,
            &boot_state.mb_mmap_info,
            &boot_state.fb_info
        )) {
        أرجع false;
    }

    أرجع true;
}

ساكن BOOT_ڪوڊ bool_t جوڙيو_mem_p_regs(p_region_t reg)
{
    إذا (reg.شروعات == reg.پڇاڙي) {
        // Return true here if asked to add an empty region.
        // Some of the callers round down the end address to
        أرجع true;
    }
    إذا (reg.پڇاڙي > PADDR_چوٽي && reg.شروعات >= PADDR_چوٽي) {
        /* Return true here as it's not an error for there to exist memory outside the kernel window,
         * we're just going to ignore it and leave it to be given out as device memory */
        أرجع true;
    }
    إذا (boot_state.mem_p_regs.ڳڻپ == MAX_انگ_FREEMEM_REG) {
        لکو("Dropping memory region 0x%lx-0x%lx, try increasing MAX_NUM_FREEMEM_REG\n", reg.شروعات, reg.پڇاڙي);
        أرجع false;
    }
    إذا (reg.پڇاڙي > PADDR_چوٽي) {
        أكد(reg.شروعات < PADDR_چوٽي);
        /* Clamp a region to the top of the kernel window if it extends beyond */
        reg.پڇاڙي = PADDR_چوٽي;
    }
    لکو("Adding physical memory region 0x%lx-0x%lx\n", reg.شروعات, reg.پڇاڙي);
    boot_state.mem_p_regs.list[boot_state.mem_p_regs.ڳڻپ] = reg;
    boot_state.mem_p_regs.ڳڻپ++;
    أرجع true;
}

/*
 * the code relies that the GRUB provides correct information
 * about the actual physical memory regions.
 */
ساكن BOOT_ڪوڊ bool_t parse_mem_map(uint32_t mmap_ڊيگهائي, uint32_t mmap_addr)
{
    multiboot_mmap_t *mmap = (multiboot_mmap_t *)((لفظ_t)mmap_addr);
    لکو("Parsing GRUB physical memory map\n");

    طالما ((لفظ_t)mmap < (لفظ_t)(mmap_addr + mmap_ڊيگهائي)) {
        uint64_t mem_شروعات = mmap->بنياد_addr;
        uint64_t mem_ڊيگهائي = mmap->ڊيگهائي;
        uint32_t type = mmap->type;
        إذا (mem_شروعات != (uint64_t)(لفظ_t)mem_شروعات) {
            لکو("\tPhysical memory region not addressable\n");
        } وإلا {
            لکو("\tPhysical Memory Region from %lx size %lx type %d\n", (ڊگهو)mem_شروعات, (ڊگهو)mem_ڊيگهائي, type);
            إذا (type == MULTIBOOT_MMAP_USEABLE_TYPE && mem_شروعات >= HIGHMEM_PADDR && mem_ڊيگهائي >= ٻٽو_انگ(صفحو_بٽ)) {

                إذا (!جوڙيو_mem_p_regs((p_region_t) {
                ڦيرو_مٿي(mem_شروعات, صفحو_بٽ), ڦيرو_هيٺ_ڏانهن(mem_شروعات + mem_ڊيگهائي, صفحو_بٽ),
                })) {
                    أرجع false;
                }
            }
        }
        mmap++;
    }
    أرجع true;
}

ساكن BOOT_ڪوڊ bool_t is_compiled_for_microarchitecture(فراغ)
{
    لفظ_t microarch_generation = 0;
    x86_cpu_سڃاڻپ_عنصر_t *model_info = x86_cpuid_get_model_info();

    إذا (config_سيٽ(CONFIG_ARCH_X86_SKYLAKE)) {
        microarch_generation = 7;
    } وإلا إذا (config_سيٽ(CONFIG_ARCH_X86_BROADWELL)) {
        microarch_generation = 6;
    } وإلا إذا (config_سيٽ(CONFIG_ARCH_X86_HASWELL)) {
        microarch_generation = 5;
    } وإلا إذا (config_سيٽ(CONFIG_ARCH_X86_IVY)) {
        microarch_generation = 4;
    } وإلا إذا (config_سيٽ(CONFIG_ARCH_X86_SANDY)) {
        microarch_generation = 3;
    } وإلا إذا (config_سيٽ(CONFIG_ARCH_X86_WESTMERE)) {
        microarch_generation = 2;
    } وإلا إذا (config_سيٽ(CONFIG_ARCH_X86_NEHALEM)) {
        microarch_generation = 1;
    }

    بدل (model_info->model) {
    حالة SKYLAKE_1_MODEL_ID:
    حالة SKYLAKE_2_MODEL_ID:
    حالة SKYLAKE_X_MODEL_ID:
        إذا (microarch_generation > 7) {
            أرجع false;
        }
        اكسر;

    حالة BROADWELL_1_MODEL_ID:
    حالة BROADWELL_2_MODEL_ID:
    حالة BROADWELL_3_MODEL_ID:
    حالة BROADWELL_4_MODEL_ID:
    حالة BROADWELL_5_MODEL_ID:
        إذا (microarch_generation > 6) {
            أرجع false;
        }
        اكسر;

    حالة HASWELL_1_MODEL_ID:
    حالة HASWELL_2_MODEL_ID:
    حالة HASWELL_3_MODEL_ID:
    حالة HASWELL_4_MODEL_ID:
        إذا (microarch_generation > 5) {
            أرجع false;
        }
        اكسر;

    حالة IVY_BRIDGE_1_MODEL_ID:
    حالة IVY_BRIDGE_2_MODEL_ID:
    حالة IVY_BRIDGE_3_MODEL_ID:
        إذا (microarch_generation > 4) {
            أرجع false;
        }
        اكسر;

    حالة SANDY_BRIDGE_1_MODEL_ID:
    حالة SANDY_BRIDGE_2_MODEL_ID:
        إذا (microarch_generation > 3) {
            أرجع false;
        }
        اكسر;

    حالة WESTMERE_1_MODEL_ID:
    حالة WESTMERE_2_MODEL_ID:
    حالة WESTMERE_3_MODEL_ID:
        إذا (microarch_generation > 2) {
            أرجع false;
        }
        اكسر;

    حالة NEHALEM_1_MODEL_ID:
    حالة NEHALEM_2_MODEL_ID:
    حالة NEHALEM_3_MODEL_ID:
        إذا (microarch_generation > 1) {
            أرجع false;
        }
        اكسر;

    افتراضي:
        إذا (!config_سيٽ(CONFIG_ARCH_X86_GENERIC)) {
            أرجع false;
        }
    }

    أرجع true;
}

ساكن BOOT_ڪوڊ bool_t try_boot_sys(فراغ)
{
    paddr_t mods_پڇاڙي_paddr = boot_state.mods_پڇاڙي_paddr;
    p_region_t ui_p_regs;
    paddr_t ڀريو_paddr;

    boot_state.ki_p_reg = get_p_reg_kernel_img();

    إذا (!x86_cpuid_initialize()) {
        لکو("Warning: Your x86 CPU has an unsupported vendor, '%s'.\n"
               "\tYour setup may not be able to competently run seL4 as "
               "intended.\n"
               "\tCurrently supported x86 vendors are AMD and Intel.\n",
               x86_cpuid_get_سڃاڻپ_عنصر()->vendor_string);
    }

    إذا (!is_compiled_for_microarchitecture()) {
        لکو("Warning: Your kernel was not compiled for the current microarchitecture.\n");
    }

    cpuid_007h_edx_t edx;
    edx.لفظ[0] = x86_cpuid_edx(0x7, 0);
    /* see if we can definitively say whether or not we need the skim window by
     * checking whether the CPU is vulnerable to rogue data cache loads (rdcl) */
    إذا (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.لفظ[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        إذا (ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && config_سيٽ(CONFIG_KERNEL_SKIM_دري)) {
            لکو("CPU reports not vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is enabled. Performance is needlessly being impacted, consider disabling.\n");
        } وإلا إذا (!ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && !config_سيٽ(CONFIG_KERNEL_SKIM_دري)) {
            لکو("CPU reports vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is *not* enabled. Please re-build with SKIM window enabled.");
            أرجع false;
        }
    } وإلا {
        /* hardware doesn't tell us directly so guess based on CPU vendor */
        إذا (config_سيٽ(CONFIG_KERNEL_SKIM_دري) && x86_cpuid_get_سڃاڻپ_عنصر()->vendor == X86_VENDOR_AMD) {
            لکو("SKIM window for mitigating Meltdown (https://www.meltdownattack.com) "
                   "not necessary for AMD and performance is being needlessly affected, "
                   "consider disabling\n");
        }
        إذا (!config_سيٽ(CONFIG_KERNEL_SKIM_دري) && x86_cpuid_get_سڃاڻپ_عنصر()->vendor == X86_VENDOR_INTEL) {
            لکو("***WARNING*** SKIM window not enabled, this machine is probably vulnerable "
                   "to Meltdown (https://www.meltdownattack.com), consider enabling\n");
        }
    }

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    /* copy boot code for APs to lower memory to run in real mode */
    إذا (!copy_boot_ڪوڊ_aps(boot_state.mem_هيٺاهون)) {
        أرجع false;
    }
    /* Initialize any kernel TLS */
    گهڻائي_init_tls(0);
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

    لکو("Kernel loaded to: start=0x%lx end=0x%lx size=0x%lx entry=0x%lx\n",
           boot_state.ki_p_reg.شروعات,
           boot_state.ki_p_reg.پڇاڙي,
           boot_state.ki_p_reg.پڇاڙي - boot_state.ki_p_reg.شروعات,
           (paddr_t)شروعات_2
          );

    /* remapping legacy IRQs to their correct vectors */
    pic_remap_irqs(IRQ_INT_وٿي);
    إذا (config_سيٽ(CONFIG_IRQ_IOAPIC)) {
        /* Disable the PIC so that it does not generate any interrupts. We need to
         * do this *before* we initialize the apic */
        pic_disable();
    }

    /* validate the ACPI table */
    إذا (!acpi_validate_rsdp(&boot_state.acpi_rsdp)) {
        أرجع false;
    }

    /* check if kernel configuration matches platform requirements */
    إذا (!acpi_fadt_جاچ(&boot_state.acpi_rsdp)) {
        أرجع false;
    }

    إذا (!config_سيٽ(CONFIG_IOMMU) || cmdline_opt.disable_iommu) {
        boot_state.انگ_drhu = 0;
    } وإلا {
        /* query available IOMMUs from ACPI */
        acpi_dmar_جاچ(
            &boot_state.acpi_rsdp,
            boot_state.drhu_list,
            &boot_state.انگ_drhu,
            MAX_انگ_DRHU,
            &boot_state.rmrr_list
        );
    }

    /* query available CPUs from ACPI */
    boot_state.انگ_cpus = acpi_madt_جاچ(&boot_state.acpi_rsdp, boot_state.cpus, &boot_state.انگ_ioapic,
                                         boot_state.ioapic_paddr);
    إذا (boot_state.انگ_cpus == 0) {
        لکو("No CPUs detected\n");
        أرجع false;
    }

    إذا (config_سيٽ(CONFIG_IRQ_IOAPIC)) {
        إذا (boot_state.انگ_ioapic == 0) {
            لکو("No IOAPICs detected\n");
            أرجع false;
        }
    } وإلا {
        إذا (boot_state.انگ_ioapic > 0) {
            لکو("Detected %d IOAPICs, but configured to use PIC instead\n", boot_state.انگ_ioapic);
        }
    }

    mods_پڇاڙي_paddr = ڦيرو_مٿي(mods_پڇاڙي_paddr, صفحو_بٽ);
    أكد(mods_پڇاڙي_paddr > boot_state.ki_p_reg.پڇاڙي);

    لکو("ELF-loading userland images from boot modules:\n");
    ڀريو_paddr = mods_پڇاڙي_paddr;

    ڀريو_paddr = ڀريو_boot_module(boot_state.boot_module_شروعات, boot_state.boot_module_پڇاڙي, ڀريو_paddr);
    إذا (!ڀريو_paddr) {
        أرجع false;
    }

    /* calculate final location of userland images */
    ui_p_regs.شروعات = boot_state.ki_p_reg.پڇاڙي;
    ui_p_regs.پڇاڙي = ui_p_regs.شروعات + ڀريو_paddr - mods_پڇاڙي_paddr;

    لکو(
        "Moving loaded userland images to final location: from=0x%lx to=0x%lx size=0x%lx\n",
        mods_پڇاڙي_paddr,
        ui_p_regs.شروعات,
        ui_p_regs.پڇاڙي - ui_p_regs.شروعات
    );
    يادگيري_نقل((فراغ *)ui_p_regs.شروعات, (فراغ *)mods_پڇاڙي_paddr, ui_p_regs.پڇاڙي - ui_p_regs.شروعات);

    /* adjust p_reg and pv_offset to final load address */
    boot_state.ui_info.p_reg.شروعات -= mods_پڇاڙي_paddr - ui_p_regs.شروعات;
    boot_state.ui_info.p_reg.پڇاڙي   -= mods_پڇاڙي_paddr - ui_p_regs.شروعات;
    boot_state.ui_info.pv_وٿي   -= mods_پڇاڙي_paddr - ui_p_regs.شروعات;

    /* ==== following code corresponds to abstract specification after "select" ==== */

    إذا (!plat_جوڙيو_Devices()) {
        أرجع false;
    }

    /* Total number of cores we intend to boot */
    ks_انگ_CPUs = boot_state.انگ_cpus;

    لکو("Starting node #0 with APIC ID %lu\n", boot_state.cpus[0]);
    إذا (!try_boot_sys_ڳنڍ(boot_state.cpus[0])) {
        أرجع false;
    }

    إذا (config_سيٽ(CONFIG_IRQ_IOAPIC)) {
        ioapic_init(1, boot_state.cpus, boot_state.انگ_ioapic);
    }

    /* initialize BKL before booting up APs */
    SMP_COND_STATEMENT(clh_تالو_init());
    SMP_COND_STATEMENT(شروعات_boot_aps());

    /* grab BKL before leaving the kernel */
    ڳنڍ_تالو_SYS;

    لکو("Booting all finished, dropped to user space\n");

    أرجع true;
}

ساكن BOOT_ڪوڊ bool_t try_boot_sys_mbi1(
    multiboot_info_t *mbi
)
{
    لفظ_t i;
    multiboot_module_t *modules = (multiboot_module_t *)(لفظ_t)mbi->part1.mod_list;

    cmdline_parse((ثابت حرف *)(لفظ_t)mbi->part1.cmdline, &cmdline_opt);

    إذا ((mbi->part1.flags & MULTIBOOT_INFO_MEM_FLAG) == 0) {
        لکو("Boot loader did not provide information about physical memory size\n");
        أرجع false;
    }

    إذا (!(mbi->part1.flags & MULTIBOOT_INFO_MODS_FLAG)) {
        لکو("Boot loader did not provide information about boot modules\n");
        أرجع false;
    }

    لکو("Detected %d boot module(s):\n", mbi->part1.mod_ڳڻپ);

    إذا (mbi->part1.mod_ڳڻپ < 1) {
        لکو("Expect at least one boot module (containing a userland image)\n");
        أرجع false;
    }

    لكل (i = 0; i < mbi->part1.mod_ڳڻپ; i++) {
        لکو(
            "  module #%ld: start=0x%x end=0x%x size=0x%x name='%s'\n",
            i,
            modules[i].شروعات,
            modules[i].پڇاڙي,
            modules[i].پڇاڙي - modules[i].شروعات,
            (حرف *)(ڊگهو)modules[i].name
        );
        إذا ((sword_t)(modules[i].پڇاڙي - modules[i].شروعات) <= 0) {
            لکو("Invalid boot module size! Possible cause: boot module file not found by QEMU\n");
            أرجع false;
        }
        إذا (boot_state.mods_پڇاڙي_paddr < modules[i].پڇاڙي) {
            boot_state.mods_پڇاڙي_paddr = modules[i].پڇاڙي;
        }
    }

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.ڳڻپ = 0;
    إذا (mbi->part1.flags & MULTIBOOT_INFO_MMAP_FLAG) {
        إذا (!parse_mem_map(mbi->part2.mmap_ڊيگهائي, mbi->part2.mmap_addr)) {
            أرجع false;
        }
        uint32_t multiboot_mmap_ڊيگهائي = mbi->part2.mmap_ڊيگهائي;
        إذا (multiboot_mmap_ڊيگهائي > (SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * حجم(seL4_X86_mb_mmap_t))) {
            لکو("Warning: Multiboot has reported more memory map entries, %zd, "
                   "than the max amount that will be passed in the bootinfo, %d. "
                   "These extra regions will still be turned into untyped caps.",
                   multiboot_mmap_ڊيگهائي / حجم(seL4_X86_mb_mmap_t), SEL4_MULTIBOOT_MAX_MMAP_ENTRIES);
            multiboot_mmap_ڊيگهائي = SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * حجم(seL4_X86_mb_mmap_t);
        }
        يادگيري_نقل(&boot_state.mb_mmap_info.mmap, (فراغ *)(لفظ_t)mbi->part2.mmap_addr, multiboot_mmap_ڊيگهائي);
        boot_state.mb_mmap_info.mmap_ڊيگهائي = multiboot_mmap_ڊيگهائي;
    } وإلا {
        /* calculate memory the old way */
        p_region_t avail;
        avail.شروعات = HIGHMEM_PADDR;
        avail.پڇاڙي = ڦيرو_هيٺ_ڏانهن(avail.شروعات + (mbi->part1.mem_مٿانهون << 10), صفحو_بٽ);
        إذا (!جوڙيو_mem_p_regs(avail)) {
            أرجع false;
        }
    }

    /* copy VESA information from multiboot header */
    إذا ((mbi->part1.flags & MULTIBOOT_INFO_GRAPHICS_FLAG) == 0) {
        boot_state.vbe_info.vbe_گهڻائي = -1;
        لکو("Multiboot gave us no video information\n");
    } وإلا {
        boot_state.vbe_info.vbe_Info_بلاڪ = *(se_L4_VBEInfo_بلاڪ_t *)(se_L4_لفظ)mbi->part2.vbe_control_info;
        boot_state.vbe_info.vbe_گهڻائي_Info_بلاڪ = *(se_L4_VBEMode_Info_بلاڪ_t *)(se_L4_لفظ)mbi->part2.vbe_گهڻائي_info;
        boot_state.vbe_info.vbe_گهڻائي = mbi->part2.vbe_گهڻائي_2;
        لکو("Got VBE info in multiboot. Current video mode is %d\n", mbi->part2.vbe_گهڻائي_2);
        boot_state.vbe_info.vbeInterfaceSeg = mbi->part2.vbe_interface_seg;
        boot_state.vbe_info.vbeInterfaceOff = mbi->part2.vbe_interface_off;
        boot_state.vbe_info.vbeInterfaceLen = mbi->part2.vbe_interface_len;
    }

    boot_state.mem_هيٺاهون = mbi->part1.mem_هيٺاهون;
    boot_state.boot_module_شروعات = modules->شروعات;
    boot_state.boot_module_پڇاڙي = modules->پڇاڙي;

    /* Initialize ACPI */
    إذا (!acpi_init(&boot_state.acpi_rsdp)) {
        أرجع false;
    }

    أرجع true;
}

ساكن BOOT_ڪوڊ bool_t try_boot_sys_mbi2(
    multiboot2_سرو_t *mbi2
)
{
    صحيح mod_ڳڻپ                  = 0;
    multiboot2_ٽيگ_t ثابت *ٽيگ   = (multiboot2_ٽيگ_t *)(mbi2 + 1);
    multiboot2_ٽيگ_t ثابت *ٽيگ_e = (multiboot2_ٽيگ_t *)((لفظ_t)mbi2 + mbi2->ڪل_سائيز);

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.ڳڻپ = 0;
    boot_state.mb_mmap_info.mmap_ڊيگهائي = 0;
    boot_state.vbe_info.vbe_گهڻائي = -1;

    طالما (ٽيگ < ٽيگ_e && ٽيگ->type != MULTIBOOT2_ٽيگ_پڇاڙي) {
        لفظ_t ثابت behind_ٽيگ = (لفظ_t)ٽيگ + حجم(*ٽيگ);

        إذا (ٽيگ->type == MULTIBOOT2_ٽيگ_CMDLINE) {
            حرف ثابت *ثابت cmdline = (حرف ثابت * ثابت)(behind_ٽيگ);
            cmdline_parse(cmdline, &cmdline_opt);
        } وإلا إذا (ٽيگ->type == MULTIBOOT2_ٽيگ_ACPI_1) {
            إذا (ACPI_V1_سائيز == ٽيگ->سائيز - حجم(*ٽيگ)) {
                يادگيري_نقل(&boot_state.acpi_rsdp, (فراغ *)behind_ٽيگ, ٽيگ->سائيز - حجم(*ٽيگ));
            }
        } وإلا إذا (ٽيگ->type == MULTIBOOT2_ٽيگ_ACPI_2) {
            إذا (حجم(boot_state.acpi_rsdp) == ٽيگ->سائيز - حجم(*ٽيگ)) {
                يادگيري_نقل(&boot_state.acpi_rsdp, (فراغ *)behind_ٽيگ, حجم(boot_state.acpi_rsdp));
            }
        } وإلا إذا (ٽيگ->type == MULTIBOOT2_ٽيگ_MODULE) {
            multiboot2_module_t ثابت *module = (multiboot2_module_t ثابت *)behind_ٽيگ;
            لکو(
                "  module #%d: start=0x%x end=0x%x size=0x%x name='%s'\n",
                mod_ڳڻپ,
                module->شروعات,
                module->پڇاڙي,
                module->پڇاڙي - module->شروعات,
                module->string
            );

            إذا (mod_ڳڻپ == 0) {
                boot_state.boot_module_شروعات = module->شروعات;
                boot_state.boot_module_پڇاڙي = module->پڇاڙي;
            }

            mod_ڳڻپ ++;
            إذا ((sword_t)(module->پڇاڙي - module->شروعات) <= 0) {
                لکو("Invalid boot module size! Possible cause: boot module file not found\n");
                أرجع false;
            }
            إذا (boot_state.mods_پڇاڙي_paddr < module->پڇاڙي) {
                boot_state.mods_پڇاڙي_paddr = module->پڇاڙي;
            }
        } وإلا إذا (ٽيگ->type == MULTIBOOT2_ٽيگ_يادگيري) {
            multiboot2_يادگيري_t ثابت *s = (multiboot2_يادگيري_t *)(behind_ٽيگ + 8);
            multiboot2_يادگيري_t ثابت *e = (multiboot2_يادگيري_t *)((لفظ_t)ٽيگ + ٽيگ->سائيز);

            لكل (multiboot2_يادگيري_t ثابت *m = s; m < e; m++) {
                إذا (!m->addr) {
                    boot_state.mem_هيٺاهون = m->سائيز;
                }

                لکو("\tPhysical Memory Region from %llx size %llx type %u\n", m->addr, m->سائيز, m->type);
                إذا (m->addr != (uint64_t)(لفظ_t)m->addr) {
                    لکو("\t\tPhysical memory region not addressable\n");
                    استمر;
                }

                إذا (m->type == MULTIBOOT_MMAP_USEABLE_TYPE && m->addr >= HIGHMEM_PADDR) {
                    إذا (!جوڙيو_mem_p_regs((p_region_t) {
                    m->addr, m->addr + m->سائيز
                }))
                    أرجع false;
                }
            }
        } وإلا إذا (ٽيگ->type == MULTIBOOT2_ٽيگ_FB) {
            multiboot2_fb_t ثابت *fb = (multiboot2_fb_t ثابت *)behind_ٽيگ;
            لکو("Got framebuffer info in multiboot2. Current video mode is at physical address=%llx pitch=%u resolution=%ux%u@%u type=%u\n",
                   fb->addr, fb->pitch, fb->ويڪر, fb->اوچائي, fb->bpp, fb->type);
            boot_state.fb_info = *fb;
        }

        ٽيگ = (multiboot2_ٽيگ_t ثابت *)((لفظ_t)ٽيگ + ڦيرو_مٿي(ٽيگ->سائيز, 3));
    }

    لکو("Detected %d boot module(s):\n", mod_ڳڻپ);

    إذا (mod_ڳڻپ < 1) {
        لکو("Expect at least one boot module (containing a userland image)\n");
        أرجع false;
    }

    أرجع true;
}

BOOT_ڪوڊ VISIBLE فراغ boot_sys(
    بنانشان ڊگهو multiboot_magic,
    فراغ *mbi)
{
    bool_t نتيجو = false;

    إذا (multiboot_magic == MULTIBOOT_MAGIC) {
        نتيجو = try_boot_sys_mbi1(mbi);
    } وإلا إذا (multiboot_magic == MULTIBOOT2_MAGIC) {
        نتيجو = try_boot_sys_mbi2(mbi);
    } وإلا {
        لکو("Boot loader is not multiboot 1 or 2 compliant %lx\n", multiboot_magic);
    }

    إذا (نتيجو) {
        نتيجو = try_boot_sys();
    }

    إذا (!نتيجو) {
        fail("boot_sys failed for some reason :(\n");
    }

    ARCH_ڳنڍ_STATE(x86_KScur_مداخلت) = int_invalid;
    ARCH_ڳنڍ_STATE(x86_KSPending_مداخلت) = int_invalid;

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    ڳنڍ_STATE(ks_Cur_وقت) = get_Current_وقت();
    ڳنڍ_STATE(ksConsumed) = 0;
#جيڪڏهن_پورو

    وقت_جدول();
    activateThread();
}

