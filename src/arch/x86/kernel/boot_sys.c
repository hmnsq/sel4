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
خارجي حرف boot_cpu_شروع[1];
خارجي حرف boot_cpu_انتها[1];

/* start/end of boot stack */
خارجي حرف boot_پشته_bottom[1];
خارجي حرف boot_پشته_بالاترین[1];

/* locations in kernel image */
خارجي حرف ki_skim_شروع[1];
خارجي حرف ki_skim_انتها[1];

#اگر_تعریف_شده CONFIG_PRINTING
/* kernel entry point */
خارجي حرف شروع_2[1];
#پایان_اگر

/* constants */

#تعریف_کن HIGHMEM_PADDR 0x100000

BOOT_BSS
boot_state_t boot_state;

/* global variables (not covered by abstract specification) */

BOOT_BSS
cmdline_opt_t cmdline_opt;

/* functions not modeled in abstract specification */

BOOT_کد ساكن paddr_t پیدا_کن_بارگیری_paddr(paddr_t min_paddr, کلمه_t image_اندازه)
{
    صحيح i;

    لكل (i = 0; i < boot_state.mem_p_regs.شمار; i++) {
        paddr_t شروع = MAX(min_paddr, boot_state.mem_p_regs.list[i].شروع);
        paddr_t انتها = boot_state.mem_p_regs.list[i].انتها;
        کلمه_t region_اندازه = انتها - شروع;

        إذا (region_اندازه >= image_اندازه) {
            أرجع شروع;
        }
    }

    أرجع 0;
}

BOOT_کد ساكن paddr_t بارگیری_boot_module(کلمه_t boot_module_شروع, کلمه_t boot_module_انتها, paddr_t بارگیری_paddr)
{
    v_region_t v_reg;
    کلمه_t مدخل;
    Elf_سرآیند_t *elf_file = (Elf_سرآیند_t *)boot_module_شروع;

    إذا (!elf_checkFile(elf_file, boot_module_انتها - boot_module_شروع)) {
        چاپ_کن("Boot module does not contain a valid ELF image\n");
        أرجع 0;
    }

    v_reg = elf_get_حافظه_Bounds(elf_file);
    مدخل = elf_file->e_مدخل;

    إذا (v_reg.انتها == 0) {
        چاپ_کن("ELF image in boot module does not contain any segments\n");
        أرجع 0;
    }
    v_reg.انتها = دور_به_بالا(v_reg.انتها, صفحه_بیتها);

    چاپ_کن("size=0x%lx v_entry=%p v_start=%p v_end=%p ",
           v_reg.انتها - v_reg.شروع,
           (فراغ *)مدخل,
           (فراغ *)v_reg.شروع,
           (فراغ *)v_reg.انتها
          );

    إذا (!IS_ALIGNED(v_reg.شروع, صفحه_بیتها)) {
        چاپ_کن("Userland image virtual start address must be 4KB-aligned\n");
        أرجع 0;
    }
    إذا (v_reg.انتها + 2 * بیت(صفحه_بیتها) > USER_بالاترین) {
        /* for IPC buffer frame and bootinfo frame, need 2*4K of additional userland virtual memory */
        چاپ_کن("Userland image virtual end address too high\n");
        أرجع 0;
    }
    إذا ((مدخل < v_reg.شروع) || (مدخل >= v_reg.انتها)) {
        چاپ_کن("Userland image entry point does not lie within userland image\n");
        أرجع 0;
    }

    بارگیری_paddr = پیدا_کن_بارگیری_paddr(بارگیری_paddr, v_reg.انتها - v_reg.شروع);
    أكد(بارگیری_paddr);

    /* fill ui_info struct */
    boot_state.ui_info.pv_جابه_جایی = بارگیری_paddr - v_reg.شروع;
    boot_state.ui_info.p_reg.شروع = بارگیری_paddr;
    بارگیری_paddr += v_reg.انتها - v_reg.شروع;
    boot_state.ui_info.p_reg.انتها = بارگیری_paddr;
    boot_state.ui_info.v_مدخل = مدخل;

    چاپ_کن("p_start=0x%lx p_end=0x%lx\n",
           boot_state.ui_info.p_reg.شروع,
           boot_state.ui_info.p_reg.انتها
          );

    /* initialise all initial userland memory and load potentially sparse ELF image */
    memzero(
        (فراغ *)boot_state.ui_info.p_reg.شروع,
        boot_state.ui_info.p_reg.انتها - boot_state.ui_info.p_reg.شروع
    );
    elf_بارگیری(elf_file, boot_state.ui_info.pv_جابه_جایی);

    أرجع بارگیری_paddr;
}

ساكن BOOT_کد bool_t try_boot_sys_گره(cpu_id_t cpu_id)
{
    إذا (!map_kernel_پنجره(
            boot_state.رقم_ioapic,
            boot_state.ioapic_paddr,
            boot_state.رقم_drhu,
            boot_state.drhu_list
        )) {
        أرجع false;
    }
    مجموعه_Current_VSpace_ریشه(kpptr_مقصد_paddr(X86_KERNEL_VSPACE_ریشه), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm متطاير("" ::: "memory");

#اگر_تعریف_شده CONFIG_KERNEL_SKIM_پنجره
    إذا (!map_skim_پنجره((vptr_t)ki_skim_شروع, (vptr_t)ki_skim_انتها)) {
        أرجع false;
    }
#پایان_اگر

    /* initialise the CPU */
    إذا (!init_cpu(config_مجموعه(CONFIG_IRQ_IOAPIC) ? 1 : 0)) {
        أرجع false;
    }

    /* initialise NDKS and kernel heap */
    إذا (!init_sys_state(
            cpu_id,
            &boot_state.mem_p_regs,
            boot_state.ui_info,
            /* parameters below not modelled in abstract specification */
            boot_state.رقم_drhu,
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

ساكن BOOT_کد bool_t جمع_کن_mem_p_regs(p_region_t reg)
{
    إذا (reg.شروع == reg.انتها) {
        // Return true here if asked to add an empty region.
        // Some of the callers round down the end address to
        أرجع true;
    }
    إذا (reg.انتها > PADDR_بالاترین && reg.شروع >= PADDR_بالاترین) {
        /* Return true here as it's not an error for there to exist memory outside the kernel window,
         * we're just going to ignore it and leave it to be given out as device memory */
        أرجع true;
    }
    إذا (boot_state.mem_p_regs.شمار == MAX_رقم_FREEMEM_REG) {
        چاپ_کن("Dropping memory region 0x%lx-0x%lx, try increasing MAX_NUM_FREEMEM_REG\n", reg.شروع, reg.انتها);
        أرجع false;
    }
    إذا (reg.انتها > PADDR_بالاترین) {
        أكد(reg.شروع < PADDR_بالاترین);
        /* Clamp a region to the top of the kernel window if it extends beyond */
        reg.انتها = PADDR_بالاترین;
    }
    چاپ_کن("Adding physical memory region 0x%lx-0x%lx\n", reg.شروع, reg.انتها);
    boot_state.mem_p_regs.list[boot_state.mem_p_regs.شمار] = reg;
    boot_state.mem_p_regs.شمار++;
    أرجع true;
}

/*
 * the code relies that the GRUB provides correct information
 * about the actual physical memory regions.
 */
ساكن BOOT_کد bool_t parse_mem_map(uint32_t mmap_درازی, uint32_t mmap_addr)
{
    multiboot_mmap_t *mmap = (multiboot_mmap_t *)((کلمه_t)mmap_addr);
    چاپ_کن("Parsing GRUB physical memory map\n");

    طالما ((کلمه_t)mmap < (کلمه_t)(mmap_addr + mmap_درازی)) {
        uint64_t mem_شروع = mmap->پایه_addr;
        uint64_t mem_درازی = mmap->درازی;
        uint32_t type = mmap->type;
        إذا (mem_شروع != (uint64_t)(کلمه_t)mem_شروع) {
            چاپ_کن("\tPhysical memory region not addressable\n");
        } وإلا {
            چاپ_کن("\tPhysical Memory Region from %lx size %lx type %d\n", (دراز)mem_شروع, (دراز)mem_درازی, type);
            إذا (type == MULTIBOOT_MMAP_USEABLE_TYPE && mem_شروع >= HIGHMEM_PADDR && mem_درازی >= بیت(صفحه_بیتها)) {

                إذا (!جمع_کن_mem_p_regs((p_region_t) {
                دور_به_بالا(mem_شروع, صفحه_بیتها), دور_پایین(mem_شروع + mem_درازی, صفحه_بیتها),
                })) {
                    أرجع false;
                }
            }
        }
        mmap++;
    }
    أرجع true;
}

ساكن BOOT_کد bool_t is_compiled_for_microarchitecture(فراغ)
{
    کلمه_t microarch_generation = 0;
    x86_cpu_عنصر_خنثی_t *model_info = x86_cpuid_get_model_info();

    إذا (config_مجموعه(CONFIG_ARCH_X86_SKYLAKE)) {
        microarch_generation = 7;
    } وإلا إذا (config_مجموعه(CONFIG_ARCH_X86_BROADWELL)) {
        microarch_generation = 6;
    } وإلا إذا (config_مجموعه(CONFIG_ARCH_X86_HASWELL)) {
        microarch_generation = 5;
    } وإلا إذا (config_مجموعه(CONFIG_ARCH_X86_IVY)) {
        microarch_generation = 4;
    } وإلا إذا (config_مجموعه(CONFIG_ARCH_X86_SANDY)) {
        microarch_generation = 3;
    } وإلا إذا (config_مجموعه(CONFIG_ARCH_X86_WESTMERE)) {
        microarch_generation = 2;
    } وإلا إذا (config_مجموعه(CONFIG_ARCH_X86_NEHALEM)) {
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
        إذا (!config_مجموعه(CONFIG_ARCH_X86_GENERIC)) {
            أرجع false;
        }
    }

    أرجع true;
}

ساكن BOOT_کد bool_t try_boot_sys(فراغ)
{
    paddr_t mods_انتها_paddr = boot_state.mods_انتها_paddr;
    p_region_t ui_p_regs;
    paddr_t بارگیری_paddr;

    boot_state.ki_p_reg = get_p_reg_kernel_img();

    إذا (!x86_cpuid_initialize()) {
        چاپ_کن("Warning: Your x86 CPU has an unsupported vendor, '%s'.\n"
               "\tYour setup may not be able to competently run seL4 as "
               "intended.\n"
               "\tCurrently supported x86 vendors are AMD and Intel.\n",
               x86_cpuid_get_عنصر_خنثی()->vendor_string);
    }

    إذا (!is_compiled_for_microarchitecture()) {
        چاپ_کن("Warning: Your kernel was not compiled for the current microarchitecture.\n");
    }

    cpuid_007h_edx_t edx;
    edx.واژگان[0] = x86_cpuid_edx(0x7, 0);
    /* see if we can definitively say whether or not we need the skim window by
     * checking whether the CPU is vulnerable to rogue data cache loads (rdcl) */
    إذا (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.واژگان[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        إذا (ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && config_مجموعه(CONFIG_KERNEL_SKIM_پنجره)) {
            چاپ_کن("CPU reports not vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is enabled. Performance is needlessly being impacted, consider disabling.\n");
        } وإلا إذا (!ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && !config_مجموعه(CONFIG_KERNEL_SKIM_پنجره)) {
            چاپ_کن("CPU reports vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is *not* enabled. Please re-build with SKIM window enabled.");
            أرجع false;
        }
    } وإلا {
        /* hardware doesn't tell us directly so guess based on CPU vendor */
        إذا (config_مجموعه(CONFIG_KERNEL_SKIM_پنجره) && x86_cpuid_get_عنصر_خنثی()->vendor == X86_VENDOR_AMD) {
            چاپ_کن("SKIM window for mitigating Meltdown (https://www.meltdownattack.com) "
                   "not necessary for AMD and performance is being needlessly affected, "
                   "consider disabling\n");
        }
        إذا (!config_مجموعه(CONFIG_KERNEL_SKIM_پنجره) && x86_cpuid_get_عنصر_خنثی()->vendor == X86_VENDOR_INTEL) {
            چاپ_کن("***WARNING*** SKIM window not enabled, this machine is probably vulnerable "
                   "to Meltdown (https://www.meltdownattack.com), consider enabling\n");
        }
    }

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    /* copy boot code for APs to lower memory to run in real mode */
    إذا (!copy_boot_کد_aps(boot_state.mem_پایینی)) {
        أرجع false;
    }
    /* Initialize any kernel TLS */
    نما_init_tls(0);
#پایان_اگر /* ENABLE_SMP_SUPPORT */

    چاپ_کن("Kernel loaded to: start=0x%lx end=0x%lx size=0x%lx entry=0x%lx\n",
           boot_state.ki_p_reg.شروع,
           boot_state.ki_p_reg.انتها,
           boot_state.ki_p_reg.انتها - boot_state.ki_p_reg.شروع,
           (paddr_t)شروع_2
          );

    /* remapping legacy IRQs to their correct vectors */
    pic_remap_irqs(IRQ_INT_جابه_جایی);
    إذا (config_مجموعه(CONFIG_IRQ_IOAPIC)) {
        /* Disable the PIC so that it does not generate any interrupts. We need to
         * do this *before* we initialize the apic */
        pic_disable();
    }

    /* validate the ACPI table */
    إذا (!acpi_validate_rsdp(&boot_state.acpi_rsdp)) {
        أرجع false;
    }

    /* check if kernel configuration matches platform requirements */
    إذا (!acpi_fadt_پیمایش(&boot_state.acpi_rsdp)) {
        أرجع false;
    }

    إذا (!config_مجموعه(CONFIG_IOMMU) || cmdline_opt.disable_iommu) {
        boot_state.رقم_drhu = 0;
    } وإلا {
        /* query available IOMMUs from ACPI */
        acpi_dmar_پیمایش(
            &boot_state.acpi_rsdp,
            boot_state.drhu_list,
            &boot_state.رقم_drhu,
            MAX_رقم_DRHU,
            &boot_state.rmrr_list
        );
    }

    /* query available CPUs from ACPI */
    boot_state.رقم_cpus = acpi_madt_پیمایش(&boot_state.acpi_rsdp, boot_state.cpus, &boot_state.رقم_ioapic,
                                         boot_state.ioapic_paddr);
    إذا (boot_state.رقم_cpus == 0) {
        چاپ_کن("No CPUs detected\n");
        أرجع false;
    }

    إذا (config_مجموعه(CONFIG_IRQ_IOAPIC)) {
        إذا (boot_state.رقم_ioapic == 0) {
            چاپ_کن("No IOAPICs detected\n");
            أرجع false;
        }
    } وإلا {
        إذا (boot_state.رقم_ioapic > 0) {
            چاپ_کن("Detected %d IOAPICs, but configured to use PIC instead\n", boot_state.رقم_ioapic);
        }
    }

    mods_انتها_paddr = دور_به_بالا(mods_انتها_paddr, صفحه_بیتها);
    أكد(mods_انتها_paddr > boot_state.ki_p_reg.انتها);

    چاپ_کن("ELF-loading userland images from boot modules:\n");
    بارگیری_paddr = mods_انتها_paddr;

    بارگیری_paddr = بارگیری_boot_module(boot_state.boot_module_شروع, boot_state.boot_module_انتها, بارگیری_paddr);
    إذا (!بارگیری_paddr) {
        أرجع false;
    }

    /* calculate final location of userland images */
    ui_p_regs.شروع = boot_state.ki_p_reg.انتها;
    ui_p_regs.انتها = ui_p_regs.شروع + بارگیری_paddr - mods_انتها_paddr;

    چاپ_کن(
        "Moving loaded userland images to final location: from=0x%lx to=0x%lx size=0x%lx\n",
        mods_انتها_paddr,
        ui_p_regs.شروع,
        ui_p_regs.انتها - ui_p_regs.شروع
    );
    کپی_حافظه((فراغ *)ui_p_regs.شروع, (فراغ *)mods_انتها_paddr, ui_p_regs.انتها - ui_p_regs.شروع);

    /* adjust p_reg and pv_offset to final load address */
    boot_state.ui_info.p_reg.شروع -= mods_انتها_paddr - ui_p_regs.شروع;
    boot_state.ui_info.p_reg.انتها   -= mods_انتها_paddr - ui_p_regs.شروع;
    boot_state.ui_info.pv_جابه_جایی   -= mods_انتها_paddr - ui_p_regs.شروع;

    /* ==== following code corresponds to abstract specification after "select" ==== */

    إذا (!plat_جمع_کن_Devices()) {
        أرجع false;
    }

    /* Total number of cores we intend to boot */
    ks_رقم_CPUs = boot_state.رقم_cpus;

    چاپ_کن("Starting node #0 with APIC ID %lu\n", boot_state.cpus[0]);
    إذا (!try_boot_sys_گره(boot_state.cpus[0])) {
        أرجع false;
    }

    إذا (config_مجموعه(CONFIG_IRQ_IOAPIC)) {
        ioapic_init(1, boot_state.cpus, boot_state.رقم_ioapic);
    }

    /* initialize BKL before booting up APs */
    SMP_COND_STATEMENT(clh_قفل_init());
    SMP_COND_STATEMENT(شروع_boot_aps());

    /* grab BKL before leaving the kernel */
    گره_قفل_SYS;

    چاپ_کن("Booting all finished, dropped to user space\n");

    أرجع true;
}

ساكن BOOT_کد bool_t try_boot_sys_mbi1(
    multiboot_info_t *mbi
)
{
    کلمه_t i;
    multiboot_module_t *modules = (multiboot_module_t *)(کلمه_t)mbi->part1.mod_list;

    cmdline_parse((ثابت حرف *)(کلمه_t)mbi->part1.cmdline, &cmdline_opt);

    إذا ((mbi->part1.flags & MULTIBOOT_INFO_MEM_FLAG) == 0) {
        چاپ_کن("Boot loader did not provide information about physical memory size\n");
        أرجع false;
    }

    إذا (!(mbi->part1.flags & MULTIBOOT_INFO_MODS_FLAG)) {
        چاپ_کن("Boot loader did not provide information about boot modules\n");
        أرجع false;
    }

    چاپ_کن("Detected %d boot module(s):\n", mbi->part1.mod_شمار);

    إذا (mbi->part1.mod_شمار < 1) {
        چاپ_کن("Expect at least one boot module (containing a userland image)\n");
        أرجع false;
    }

    لكل (i = 0; i < mbi->part1.mod_شمار; i++) {
        چاپ_کن(
            "  module #%ld: start=0x%x end=0x%x size=0x%x name='%s'\n",
            i,
            modules[i].شروع,
            modules[i].انتها,
            modules[i].انتها - modules[i].شروع,
            (حرف *)(دراز)modules[i].name
        );
        إذا ((sword_t)(modules[i].انتها - modules[i].شروع) <= 0) {
            چاپ_کن("Invalid boot module size! Possible cause: boot module file not found by QEMU\n");
            أرجع false;
        }
        إذا (boot_state.mods_انتها_paddr < modules[i].انتها) {
            boot_state.mods_انتها_paddr = modules[i].انتها;
        }
    }

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.شمار = 0;
    إذا (mbi->part1.flags & MULTIBOOT_INFO_MMAP_FLAG) {
        إذا (!parse_mem_map(mbi->part2.mmap_درازی, mbi->part2.mmap_addr)) {
            أرجع false;
        }
        uint32_t multiboot_mmap_درازی = mbi->part2.mmap_درازی;
        إذا (multiboot_mmap_درازی > (SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * حجم(seL4_X86_mb_mmap_t))) {
            چاپ_کن("Warning: Multiboot has reported more memory map entries, %zd, "
                   "than the max amount that will be passed in the bootinfo, %d. "
                   "These extra regions will still be turned into untyped caps.",
                   multiboot_mmap_درازی / حجم(seL4_X86_mb_mmap_t), SEL4_MULTIBOOT_MAX_MMAP_ENTRIES);
            multiboot_mmap_درازی = SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * حجم(seL4_X86_mb_mmap_t);
        }
        کپی_حافظه(&boot_state.mb_mmap_info.mmap, (فراغ *)(کلمه_t)mbi->part2.mmap_addr, multiboot_mmap_درازی);
        boot_state.mb_mmap_info.mmap_درازی = multiboot_mmap_درازی;
    } وإلا {
        /* calculate memory the old way */
        p_region_t avail;
        avail.شروع = HIGHMEM_PADDR;
        avail.انتها = دور_پایین(avail.شروع + (mbi->part1.mem_فوقانی << 10), صفحه_بیتها);
        إذا (!جمع_کن_mem_p_regs(avail)) {
            أرجع false;
        }
    }

    /* copy VESA information from multiboot header */
    إذا ((mbi->part1.flags & MULTIBOOT_INFO_GRAPHICS_FLAG) == 0) {
        boot_state.vbe_info.vbe_نما = -1;
        چاپ_کن("Multiboot gave us no video information\n");
    } وإلا {
        boot_state.vbe_info.vbe_Info_بلوک = *(se_L4_VBEInfo_بلوک_t *)(se_L4_کلمه)mbi->part2.vbe_control_info;
        boot_state.vbe_info.vbe_نما_Info_بلوک = *(se_L4_VBEMode_Info_بلوک_t *)(se_L4_کلمه)mbi->part2.vbe_نما_info;
        boot_state.vbe_info.vbe_نما = mbi->part2.vbe_نما_2;
        چاپ_کن("Got VBE info in multiboot. Current video mode is %d\n", mbi->part2.vbe_نما_2);
        boot_state.vbe_info.vbeInterfaceSeg = mbi->part2.vbe_interface_seg;
        boot_state.vbe_info.vbeInterfaceOff = mbi->part2.vbe_interface_off;
        boot_state.vbe_info.vbeInterfaceLen = mbi->part2.vbe_interface_len;
    }

    boot_state.mem_پایینی = mbi->part1.mem_پایینی;
    boot_state.boot_module_شروع = modules->شروع;
    boot_state.boot_module_انتها = modules->انتها;

    /* Initialize ACPI */
    إذا (!acpi_init(&boot_state.acpi_rsdp)) {
        أرجع false;
    }

    أرجع true;
}

ساكن BOOT_کد bool_t try_boot_sys_mbi2(
    multiboot2_سرآیند_t *mbi2
)
{
    صحيح mod_شمار                  = 0;
    multiboot2_برچسب_t ثابت *برچسب   = (multiboot2_برچسب_t *)(mbi2 + 1);
    multiboot2_برچسب_t ثابت *برچسب_e = (multiboot2_برچسب_t *)((کلمه_t)mbi2 + mbi2->مجموع_اندازه);

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.شمار = 0;
    boot_state.mb_mmap_info.mmap_درازی = 0;
    boot_state.vbe_info.vbe_نما = -1;

    طالما (برچسب < برچسب_e && برچسب->type != MULTIBOOT2_برچسب_انتها) {
        کلمه_t ثابت behind_برچسب = (کلمه_t)برچسب + حجم(*برچسب);

        إذا (برچسب->type == MULTIBOOT2_برچسب_CMDLINE) {
            حرف ثابت *ثابت cmdline = (حرف ثابت * ثابت)(behind_برچسب);
            cmdline_parse(cmdline, &cmdline_opt);
        } وإلا إذا (برچسب->type == MULTIBOOT2_برچسب_ACPI_1) {
            إذا (ACPI_V1_اندازه == برچسب->اندازه - حجم(*برچسب)) {
                کپی_حافظه(&boot_state.acpi_rsdp, (فراغ *)behind_برچسب, برچسب->اندازه - حجم(*برچسب));
            }
        } وإلا إذا (برچسب->type == MULTIBOOT2_برچسب_ACPI_2) {
            إذا (حجم(boot_state.acpi_rsdp) == برچسب->اندازه - حجم(*برچسب)) {
                کپی_حافظه(&boot_state.acpi_rsdp, (فراغ *)behind_برچسب, حجم(boot_state.acpi_rsdp));
            }
        } وإلا إذا (برچسب->type == MULTIBOOT2_برچسب_MODULE) {
            multiboot2_module_t ثابت *module = (multiboot2_module_t ثابت *)behind_برچسب;
            چاپ_کن(
                "  module #%d: start=0x%x end=0x%x size=0x%x name='%s'\n",
                mod_شمار,
                module->شروع,
                module->انتها,
                module->انتها - module->شروع,
                module->string
            );

            إذا (mod_شمار == 0) {
                boot_state.boot_module_شروع = module->شروع;
                boot_state.boot_module_انتها = module->انتها;
            }

            mod_شمار ++;
            إذا ((sword_t)(module->انتها - module->شروع) <= 0) {
                چاپ_کن("Invalid boot module size! Possible cause: boot module file not found\n");
                أرجع false;
            }
            إذا (boot_state.mods_انتها_paddr < module->انتها) {
                boot_state.mods_انتها_paddr = module->انتها;
            }
        } وإلا إذا (برچسب->type == MULTIBOOT2_برچسب_حافظه) {
            multiboot2_حافظه_t ثابت *s = (multiboot2_حافظه_t *)(behind_برچسب + 8);
            multiboot2_حافظه_t ثابت *e = (multiboot2_حافظه_t *)((کلمه_t)برچسب + برچسب->اندازه);

            لكل (multiboot2_حافظه_t ثابت *m = s; m < e; m++) {
                إذا (!m->addr) {
                    boot_state.mem_پایینی = m->اندازه;
                }

                چاپ_کن("\tPhysical Memory Region from %llx size %llx type %u\n", m->addr, m->اندازه, m->type);
                إذا (m->addr != (uint64_t)(کلمه_t)m->addr) {
                    چاپ_کن("\t\tPhysical memory region not addressable\n");
                    استمر;
                }

                إذا (m->type == MULTIBOOT_MMAP_USEABLE_TYPE && m->addr >= HIGHMEM_PADDR) {
                    إذا (!جمع_کن_mem_p_regs((p_region_t) {
                    m->addr, m->addr + m->اندازه
                }))
                    أرجع false;
                }
            }
        } وإلا إذا (برچسب->type == MULTIBOOT2_برچسب_FB) {
            multiboot2_fb_t ثابت *fb = (multiboot2_fb_t ثابت *)behind_برچسب;
            چاپ_کن("Got framebuffer info in multiboot2. Current video mode is at physical address=%llx pitch=%u resolution=%ux%u@%u type=%u\n",
                   fb->addr, fb->pitch, fb->عرض, fb->ارتفاع, fb->bpp, fb->type);
            boot_state.fb_info = *fb;
        }

        برچسب = (multiboot2_برچسب_t ثابت *)((کلمه_t)برچسب + دور_به_بالا(برچسب->اندازه, 3));
    }

    چاپ_کن("Detected %d boot module(s):\n", mod_شمار);

    إذا (mod_شمار < 1) {
        چاپ_کن("Expect at least one boot module (containing a userland image)\n");
        أرجع false;
    }

    أرجع true;
}

BOOT_کد VISIBLE فراغ boot_sys(
    بیعلامت دراز multiboot_magic,
    فراغ *mbi)
{
    bool_t نتیجه = false;

    إذا (multiboot_magic == MULTIBOOT_MAGIC) {
        نتیجه = try_boot_sys_mbi1(mbi);
    } وإلا إذا (multiboot_magic == MULTIBOOT2_MAGIC) {
        نتیجه = try_boot_sys_mbi2(mbi);
    } وإلا {
        چاپ_کن("Boot loader is not multiboot 1 or 2 compliant %lx\n", multiboot_magic);
    }

    إذا (نتیجه) {
        نتیجه = try_boot_sys();
    }

    إذا (!نتیجه) {
        fail("boot_sys failed for some reason :(\n");
    }

    ARCH_گره_STATE(x86_KScur_وقفه) = int_invalid;
    ARCH_گره_STATE(x86_KSPending_وقفه) = int_invalid;

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    گره_STATE(ks_Cur_زمان) = get_Current_زمان();
    گره_STATE(ksConsumed) = 0;
#پایان_اگر

    زمانبندی();
    activateThread();
}

