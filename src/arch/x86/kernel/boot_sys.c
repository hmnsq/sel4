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
خارجي حرف boot_cpu_پیل[1];
خارجي حرف boot_cpu_پای_ټکی[1];

/* start/end of boot stack */
خارجي حرف boot_ډېری_bottom[1];
خارجي حرف boot_ډېری_سر[1];

/* locations in kernel image */
خارجي حرف ki_skim_پیل[1];
خارجي حرف ki_skim_پای_ټکی[1];

#که_تعریف_شوی CONFIG_PRINTING
/* kernel entry point */
خارجي حرف پیل_2[1];
#د_که_پای

/* constants */

#تعریف_کړه HIGHMEM_PADDR 0x100000

BOOT_BSS
boot_state_t boot_state;

/* global variables (not covered by abstract specification) */

BOOT_BSS
cmdline_opt_t cmdline_opt;

/* functions not modeled in abstract specification */

BOOT_کوډ ساكن paddr_t پیدا_کړه_بارول_paddr(paddr_t min_paddr, کلمه_t image_اندازه)
{
    صحيح i;

    لكل (i = 0; i < boot_state.mem_p_regs.شمېر; i++) {
        paddr_t پیل = MAX(min_paddr, boot_state.mem_p_regs.list[i].پیل);
        paddr_t پای_ټکی = boot_state.mem_p_regs.list[i].پای_ټکی;
        کلمه_t region_اندازه = پای_ټکی - پیل;

        إذا (region_اندازه >= image_اندازه) {
            أرجع پیل;
        }
    }

    أرجع 0;
}

BOOT_کوډ ساكن paddr_t بارول_boot_module(کلمه_t boot_module_پیل, کلمه_t boot_module_پای_ټکی, paddr_t بارول_paddr)
{
    v_region_t v_reg;
    کلمه_t ثبت;
    Elf_سرلیک_t *elf_file = (Elf_سرلیک_t *)boot_module_پیل;

    إذا (!elf_checkFile(elf_file, boot_module_پای_ټکی - boot_module_پیل)) {
        چاپ_کړه("Boot module does not contain a valid ELF image\n");
        أرجع 0;
    }

    v_reg = elf_get_حافظه_Bounds(elf_file);
    ثبت = elf_file->e_ثبت;

    إذا (v_reg.پای_ټکی == 0) {
        چاپ_کړه("ELF image in boot module does not contain any segments\n");
        أرجع 0;
    }
    v_reg.پای_ټکی = پړاو_پورته(v_reg.پای_ټکی, پاڼه_بټونه);

    چاپ_کړه("size=0x%lx v_entry=%p v_start=%p v_end=%p ",
           v_reg.پای_ټکی - v_reg.پیل,
           (فراغ *)ثبت,
           (فراغ *)v_reg.پیل,
           (فراغ *)v_reg.پای_ټکی
          );

    إذا (!IS_ALIGNED(v_reg.پیل, پاڼه_بټونه)) {
        چاپ_کړه("Userland image virtual start address must be 4KB-aligned\n");
        أرجع 0;
    }
    إذا (v_reg.پای_ټکی + 2 * بټ(پاڼه_بټونه) > USER_سر) {
        /* for IPC buffer frame and bootinfo frame, need 2*4K of additional userland virtual memory */
        چاپ_کړه("Userland image virtual end address too high\n");
        أرجع 0;
    }
    إذا ((ثبت < v_reg.پیل) || (ثبت >= v_reg.پای_ټکی)) {
        چاپ_کړه("Userland image entry point does not lie within userland image\n");
        أرجع 0;
    }

    بارول_paddr = پیدا_کړه_بارول_paddr(بارول_paddr, v_reg.پای_ټکی - v_reg.پیل);
    أكد(بارول_paddr);

    /* fill ui_info struct */
    boot_state.ui_info.pv_دننه_واټن = بارول_paddr - v_reg.پیل;
    boot_state.ui_info.p_reg.پیل = بارول_paddr;
    بارول_paddr += v_reg.پای_ټکی - v_reg.پیل;
    boot_state.ui_info.p_reg.پای_ټکی = بارول_paddr;
    boot_state.ui_info.v_ثبت = ثبت;

    چاپ_کړه("p_start=0x%lx p_end=0x%lx\n",
           boot_state.ui_info.p_reg.پیل,
           boot_state.ui_info.p_reg.پای_ټکی
          );

    /* initialise all initial userland memory and load potentially sparse ELF image */
    memzero(
        (فراغ *)boot_state.ui_info.p_reg.پیل,
        boot_state.ui_info.p_reg.پای_ټکی - boot_state.ui_info.p_reg.پیل
    );
    elf_بارول(elf_file, boot_state.ui_info.pv_دننه_واټن);

    أرجع بارول_paddr;
}

ساكن BOOT_کوډ bool_t try_boot_sys_غوټه(cpu_id_t cpu_id)
{
    إذا (!map_kernel_کړکۍ(
            boot_state.عدد_ioapic,
            boot_state.ioapic_paddr,
            boot_state.عدد_drhu,
            boot_state.drhu_list
        )) {
        أرجع false;
    }
    سېټ_Current_VSpace_ریښه(kpptr_منزل_paddr(X86_KERNEL_VSPACE_ریښه), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm متطاير("" ::: "memory");

#که_تعریف_شوی CONFIG_KERNEL_SKIM_کړکۍ
    إذا (!map_skim_کړکۍ((vptr_t)ki_skim_پیل, (vptr_t)ki_skim_پای_ټکی)) {
        أرجع false;
    }
#د_که_پای

    /* initialise the CPU */
    إذا (!init_cpu(config_سېټ(CONFIG_IRQ_IOAPIC) ? 1 : 0)) {
        أرجع false;
    }

    /* initialise NDKS and kernel heap */
    إذا (!init_sys_state(
            cpu_id,
            &boot_state.mem_p_regs,
            boot_state.ui_info,
            /* parameters below not modelled in abstract specification */
            boot_state.عدد_drhu,
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

ساكن BOOT_کوډ bool_t جمع_کړه_mem_p_regs(p_region_t reg)
{
    إذا (reg.پیل == reg.پای_ټکی) {
        // Return true here if asked to add an empty region.
        // Some of the callers round down the end address to
        أرجع true;
    }
    إذا (reg.پای_ټکی > PADDR_سر && reg.پیل >= PADDR_سر) {
        /* Return true here as it's not an error for there to exist memory outside the kernel window,
         * we're just going to ignore it and leave it to be given out as device memory */
        أرجع true;
    }
    إذا (boot_state.mem_p_regs.شمېر == MAX_عدد_FREEMEM_REG) {
        چاپ_کړه("Dropping memory region 0x%lx-0x%lx, try increasing MAX_NUM_FREEMEM_REG\n", reg.پیل, reg.پای_ټکی);
        أرجع false;
    }
    إذا (reg.پای_ټکی > PADDR_سر) {
        أكد(reg.پیل < PADDR_سر);
        /* Clamp a region to the top of the kernel window if it extends beyond */
        reg.پای_ټکی = PADDR_سر;
    }
    چاپ_کړه("Adding physical memory region 0x%lx-0x%lx\n", reg.پیل, reg.پای_ټکی);
    boot_state.mem_p_regs.list[boot_state.mem_p_regs.شمېر] = reg;
    boot_state.mem_p_regs.شمېر++;
    أرجع true;
}

/*
 * the code relies that the GRUB provides correct information
 * about the actual physical memory regions.
 */
ساكن BOOT_کوډ bool_t parse_mem_map(uint32_t mmap_اوږدتیا, uint32_t mmap_addr)
{
    multiboot_mmap_t *mmap = (multiboot_mmap_t *)((کلمه_t)mmap_addr);
    چاپ_کړه("Parsing GRUB physical memory map\n");

    طالما ((کلمه_t)mmap < (کلمه_t)(mmap_addr + mmap_اوږدتیا)) {
        uint64_t mem_پیل = mmap->بنسټ_addr;
        uint64_t mem_اوږدتیا = mmap->اوږدتیا;
        uint32_t type = mmap->type;
        إذا (mem_پیل != (uint64_t)(کلمه_t)mem_پیل) {
            چاپ_کړه("\tPhysical memory region not addressable\n");
        } وإلا {
            چاپ_کړه("\tPhysical Memory Region from %lx size %lx type %d\n", (اوږد)mem_پیل, (اوږد)mem_اوږدتیا, type);
            إذا (type == MULTIBOOT_MMAP_USEABLE_TYPE && mem_پیل >= HIGHMEM_PADDR && mem_اوږدتیا >= بټ(پاڼه_بټونه)) {

                إذا (!جمع_کړه_mem_p_regs((p_region_t) {
                پړاو_پورته(mem_پیل, پاڼه_بټونه), پړاو_ښکته(mem_پیل + mem_اوږدتیا, پاڼه_بټونه),
                })) {
                    أرجع false;
                }
            }
        }
        mmap++;
    }
    أرجع true;
}

ساكن BOOT_کوډ bool_t is_compiled_for_microarchitecture(فراغ)
{
    کلمه_t microarch_generation = 0;
    x86_cpu_بې_اغېزه_عنصر_t *model_info = x86_cpuid_get_model_info();

    إذا (config_سېټ(CONFIG_ARCH_X86_SKYLAKE)) {
        microarch_generation = 7;
    } وإلا إذا (config_سېټ(CONFIG_ARCH_X86_BROADWELL)) {
        microarch_generation = 6;
    } وإلا إذا (config_سېټ(CONFIG_ARCH_X86_HASWELL)) {
        microarch_generation = 5;
    } وإلا إذا (config_سېټ(CONFIG_ARCH_X86_IVY)) {
        microarch_generation = 4;
    } وإلا إذا (config_سېټ(CONFIG_ARCH_X86_SANDY)) {
        microarch_generation = 3;
    } وإلا إذا (config_سېټ(CONFIG_ARCH_X86_WESTMERE)) {
        microarch_generation = 2;
    } وإلا إذا (config_سېټ(CONFIG_ARCH_X86_NEHALEM)) {
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
        إذا (!config_سېټ(CONFIG_ARCH_X86_GENERIC)) {
            أرجع false;
        }
    }

    أرجع true;
}

ساكن BOOT_کوډ bool_t try_boot_sys(فراغ)
{
    paddr_t mods_پای_ټکی_paddr = boot_state.mods_پای_ټکی_paddr;
    p_region_t ui_p_regs;
    paddr_t بارول_paddr;

    boot_state.ki_p_reg = get_p_reg_kernel_img();

    إذا (!x86_cpuid_initialize()) {
        چاپ_کړه("Warning: Your x86 CPU has an unsupported vendor, '%s'.\n"
               "\tYour setup may not be able to competently run seL4 as "
               "intended.\n"
               "\tCurrently supported x86 vendors are AMD and Intel.\n",
               x86_cpuid_get_بې_اغېزه_عنصر()->vendor_string);
    }

    إذا (!is_compiled_for_microarchitecture()) {
        چاپ_کړه("Warning: Your kernel was not compiled for the current microarchitecture.\n");
    }

    cpuid_007h_edx_t edx;
    edx.ټکي[0] = x86_cpuid_edx(0x7, 0);
    /* see if we can definitively say whether or not we need the skim window by
     * checking whether the CPU is vulnerable to rogue data cache loads (rdcl) */
    إذا (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.ټکي[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        إذا (ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && config_سېټ(CONFIG_KERNEL_SKIM_کړکۍ)) {
            چاپ_کړه("CPU reports not vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is enabled. Performance is needlessly being impacted, consider disabling.\n");
        } وإلا إذا (!ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && !config_سېټ(CONFIG_KERNEL_SKIM_کړکۍ)) {
            چاپ_کړه("CPU reports vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is *not* enabled. Please re-build with SKIM window enabled.");
            أرجع false;
        }
    } وإلا {
        /* hardware doesn't tell us directly so guess based on CPU vendor */
        إذا (config_سېټ(CONFIG_KERNEL_SKIM_کړکۍ) && x86_cpuid_get_بې_اغېزه_عنصر()->vendor == X86_VENDOR_AMD) {
            چاپ_کړه("SKIM window for mitigating Meltdown (https://www.meltdownattack.com) "
                   "not necessary for AMD and performance is being needlessly affected, "
                   "consider disabling\n");
        }
        إذا (!config_سېټ(CONFIG_KERNEL_SKIM_کړکۍ) && x86_cpuid_get_بې_اغېزه_عنصر()->vendor == X86_VENDOR_INTEL) {
            چاپ_کړه("***WARNING*** SKIM window not enabled, this machine is probably vulnerable "
                   "to Meltdown (https://www.meltdownattack.com), consider enabling\n");
        }
    }

#که_تعریف_شوی ENABLE_SMP_SUPPORT
    /* copy boot code for APs to lower memory to run in real mode */
    إذا (!copy_boot_کوډ_aps(boot_state.mem_ښکتنی)) {
        أرجع false;
    }
    /* Initialize any kernel TLS */
    ډېرپېښ_init_tls(0);
#د_که_پای /* ENABLE_SMP_SUPPORT */

    چاپ_کړه("Kernel loaded to: start=0x%lx end=0x%lx size=0x%lx entry=0x%lx\n",
           boot_state.ki_p_reg.پیل,
           boot_state.ki_p_reg.پای_ټکی,
           boot_state.ki_p_reg.پای_ټکی - boot_state.ki_p_reg.پیل,
           (paddr_t)پیل_2
          );

    /* remapping legacy IRQs to their correct vectors */
    pic_remap_irqs(IRQ_INT_دننه_واټن);
    إذا (config_سېټ(CONFIG_IRQ_IOAPIC)) {
        /* Disable the PIC so that it does not generate any interrupts. We need to
         * do this *before* we initialize the apic */
        pic_disable();
    }

    /* validate the ACPI table */
    إذا (!acpi_validate_rsdp(&boot_state.acpi_rsdp)) {
        أرجع false;
    }

    /* check if kernel configuration matches platform requirements */
    إذا (!acpi_fadt_لټون(&boot_state.acpi_rsdp)) {
        أرجع false;
    }

    إذا (!config_سېټ(CONFIG_IOMMU) || cmdline_opt.disable_iommu) {
        boot_state.عدد_drhu = 0;
    } وإلا {
        /* query available IOMMUs from ACPI */
        acpi_dmar_لټون(
            &boot_state.acpi_rsdp,
            boot_state.drhu_list,
            &boot_state.عدد_drhu,
            MAX_عدد_DRHU,
            &boot_state.rmrr_list
        );
    }

    /* query available CPUs from ACPI */
    boot_state.عدد_cpus = acpi_madt_لټون(&boot_state.acpi_rsdp, boot_state.cpus, &boot_state.عدد_ioapic,
                                         boot_state.ioapic_paddr);
    إذا (boot_state.عدد_cpus == 0) {
        چاپ_کړه("No CPUs detected\n");
        أرجع false;
    }

    إذا (config_سېټ(CONFIG_IRQ_IOAPIC)) {
        إذا (boot_state.عدد_ioapic == 0) {
            چاپ_کړه("No IOAPICs detected\n");
            أرجع false;
        }
    } وإلا {
        إذا (boot_state.عدد_ioapic > 0) {
            چاپ_کړه("Detected %d IOAPICs, but configured to use PIC instead\n", boot_state.عدد_ioapic);
        }
    }

    mods_پای_ټکی_paddr = پړاو_پورته(mods_پای_ټکی_paddr, پاڼه_بټونه);
    أكد(mods_پای_ټکی_paddr > boot_state.ki_p_reg.پای_ټکی);

    چاپ_کړه("ELF-loading userland images from boot modules:\n");
    بارول_paddr = mods_پای_ټکی_paddr;

    بارول_paddr = بارول_boot_module(boot_state.boot_module_پیل, boot_state.boot_module_پای_ټکی, بارول_paddr);
    إذا (!بارول_paddr) {
        أرجع false;
    }

    /* calculate final location of userland images */
    ui_p_regs.پیل = boot_state.ki_p_reg.پای_ټکی;
    ui_p_regs.پای_ټکی = ui_p_regs.پیل + بارول_paddr - mods_پای_ټکی_paddr;

    چاپ_کړه(
        "Moving loaded userland images to final location: from=0x%lx to=0x%lx size=0x%lx\n",
        mods_پای_ټکی_paddr,
        ui_p_regs.پیل,
        ui_p_regs.پای_ټکی - ui_p_regs.پیل
    );
    حافظه_نقل((فراغ *)ui_p_regs.پیل, (فراغ *)mods_پای_ټکی_paddr, ui_p_regs.پای_ټکی - ui_p_regs.پیل);

    /* adjust p_reg and pv_offset to final load address */
    boot_state.ui_info.p_reg.پیل -= mods_پای_ټکی_paddr - ui_p_regs.پیل;
    boot_state.ui_info.p_reg.پای_ټکی   -= mods_پای_ټکی_paddr - ui_p_regs.پیل;
    boot_state.ui_info.pv_دننه_واټن   -= mods_پای_ټکی_paddr - ui_p_regs.پیل;

    /* ==== following code corresponds to abstract specification after "select" ==== */

    إذا (!plat_جمع_کړه_Devices()) {
        أرجع false;
    }

    /* Total number of cores we intend to boot */
    ks_عدد_CPUs = boot_state.عدد_cpus;

    چاپ_کړه("Starting node #0 with APIC ID %lu\n", boot_state.cpus[0]);
    إذا (!try_boot_sys_غوټه(boot_state.cpus[0])) {
        أرجع false;
    }

    إذا (config_سېټ(CONFIG_IRQ_IOAPIC)) {
        ioapic_init(1, boot_state.cpus, boot_state.عدد_ioapic);
    }

    /* initialize BKL before booting up APs */
    SMP_COND_STATEMENT(clh_قلف_init());
    SMP_COND_STATEMENT(پیل_boot_aps());

    /* grab BKL before leaving the kernel */
    غوټه_قلف_SYS;

    چاپ_کړه("Booting all finished, dropped to user space\n");

    أرجع true;
}

ساكن BOOT_کوډ bool_t try_boot_sys_mbi1(
    multiboot_info_t *mbi
)
{
    کلمه_t i;
    multiboot_module_t *modules = (multiboot_module_t *)(کلمه_t)mbi->part1.mod_list;

    cmdline_parse((ثابت حرف *)(کلمه_t)mbi->part1.cmdline, &cmdline_opt);

    إذا ((mbi->part1.flags & MULTIBOOT_INFO_MEM_FLAG) == 0) {
        چاپ_کړه("Boot loader did not provide information about physical memory size\n");
        أرجع false;
    }

    إذا (!(mbi->part1.flags & MULTIBOOT_INFO_MODS_FLAG)) {
        چاپ_کړه("Boot loader did not provide information about boot modules\n");
        أرجع false;
    }

    چاپ_کړه("Detected %d boot module(s):\n", mbi->part1.mod_شمېر);

    إذا (mbi->part1.mod_شمېر < 1) {
        چاپ_کړه("Expect at least one boot module (containing a userland image)\n");
        أرجع false;
    }

    لكل (i = 0; i < mbi->part1.mod_شمېر; i++) {
        چاپ_کړه(
            "  module #%ld: start=0x%x end=0x%x size=0x%x name='%s'\n",
            i,
            modules[i].پیل,
            modules[i].پای_ټکی,
            modules[i].پای_ټکی - modules[i].پیل,
            (حرف *)(اوږد)modules[i].name
        );
        إذا ((sword_t)(modules[i].پای_ټکی - modules[i].پیل) <= 0) {
            چاپ_کړه("Invalid boot module size! Possible cause: boot module file not found by QEMU\n");
            أرجع false;
        }
        إذا (boot_state.mods_پای_ټکی_paddr < modules[i].پای_ټکی) {
            boot_state.mods_پای_ټکی_paddr = modules[i].پای_ټکی;
        }
    }

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.شمېر = 0;
    إذا (mbi->part1.flags & MULTIBOOT_INFO_MMAP_FLAG) {
        إذا (!parse_mem_map(mbi->part2.mmap_اوږدتیا, mbi->part2.mmap_addr)) {
            أرجع false;
        }
        uint32_t multiboot_mmap_اوږدتیا = mbi->part2.mmap_اوږدتیا;
        إذا (multiboot_mmap_اوږدتیا > (SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * حجم(seL4_X86_mb_mmap_t))) {
            چاپ_کړه("Warning: Multiboot has reported more memory map entries, %zd, "
                   "than the max amount that will be passed in the bootinfo, %d. "
                   "These extra regions will still be turned into untyped caps.",
                   multiboot_mmap_اوږدتیا / حجم(seL4_X86_mb_mmap_t), SEL4_MULTIBOOT_MAX_MMAP_ENTRIES);
            multiboot_mmap_اوږدتیا = SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * حجم(seL4_X86_mb_mmap_t);
        }
        حافظه_نقل(&boot_state.mb_mmap_info.mmap, (فراغ *)(کلمه_t)mbi->part2.mmap_addr, multiboot_mmap_اوږدتیا);
        boot_state.mb_mmap_info.mmap_اوږدتیا = multiboot_mmap_اوږدتیا;
    } وإلا {
        /* calculate memory the old way */
        p_region_t avail;
        avail.پیل = HIGHMEM_PADDR;
        avail.پای_ټکی = پړاو_ښکته(avail.پیل + (mbi->part1.mem_پاسنی << 10), پاڼه_بټونه);
        إذا (!جمع_کړه_mem_p_regs(avail)) {
            أرجع false;
        }
    }

    /* copy VESA information from multiboot header */
    إذا ((mbi->part1.flags & MULTIBOOT_INFO_GRAPHICS_FLAG) == 0) {
        boot_state.vbe_info.vbe_ډېرپېښ = -1;
        چاپ_کړه("Multiboot gave us no video information\n");
    } وإلا {
        boot_state.vbe_info.vbe_Info_بلاک = *(se_L4_VBEInfo_بلاک_t *)(se_L4_کلمه)mbi->part2.vbe_control_info;
        boot_state.vbe_info.vbe_ډېرپېښ_Info_بلاک = *(se_L4_VBEMode_Info_بلاک_t *)(se_L4_کلمه)mbi->part2.vbe_ډېرپېښ_info;
        boot_state.vbe_info.vbe_ډېرپېښ = mbi->part2.vbe_ډېرپېښ_2;
        چاپ_کړه("Got VBE info in multiboot. Current video mode is %d\n", mbi->part2.vbe_ډېرپېښ_2);
        boot_state.vbe_info.vbeInterfaceSeg = mbi->part2.vbe_interface_seg;
        boot_state.vbe_info.vbeInterfaceOff = mbi->part2.vbe_interface_off;
        boot_state.vbe_info.vbeInterfaceLen = mbi->part2.vbe_interface_len;
    }

    boot_state.mem_ښکتنی = mbi->part1.mem_ښکتنی;
    boot_state.boot_module_پیل = modules->پیل;
    boot_state.boot_module_پای_ټکی = modules->پای_ټکی;

    /* Initialize ACPI */
    إذا (!acpi_init(&boot_state.acpi_rsdp)) {
        أرجع false;
    }

    أرجع true;
}

ساكن BOOT_کوډ bool_t try_boot_sys_mbi2(
    multiboot2_سرلیک_t *mbi2
)
{
    صحيح mod_شمېر                  = 0;
    multiboot2_نښان_t ثابت *نښان   = (multiboot2_نښان_t *)(mbi2 + 1);
    multiboot2_نښان_t ثابت *نښان_e = (multiboot2_نښان_t *)((کلمه_t)mbi2 + mbi2->ټول_اندازه);

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.شمېر = 0;
    boot_state.mb_mmap_info.mmap_اوږدتیا = 0;
    boot_state.vbe_info.vbe_ډېرپېښ = -1;

    طالما (نښان < نښان_e && نښان->type != MULTIBOOT2_نښان_پای_ټکی) {
        کلمه_t ثابت behind_نښان = (کلمه_t)نښان + حجم(*نښان);

        إذا (نښان->type == MULTIBOOT2_نښان_CMDLINE) {
            حرف ثابت *ثابت cmdline = (حرف ثابت * ثابت)(behind_نښان);
            cmdline_parse(cmdline, &cmdline_opt);
        } وإلا إذا (نښان->type == MULTIBOOT2_نښان_ACPI_1) {
            إذا (ACPI_V1_اندازه == نښان->اندازه - حجم(*نښان)) {
                حافظه_نقل(&boot_state.acpi_rsdp, (فراغ *)behind_نښان, نښان->اندازه - حجم(*نښان));
            }
        } وإلا إذا (نښان->type == MULTIBOOT2_نښان_ACPI_2) {
            إذا (حجم(boot_state.acpi_rsdp) == نښان->اندازه - حجم(*نښان)) {
                حافظه_نقل(&boot_state.acpi_rsdp, (فراغ *)behind_نښان, حجم(boot_state.acpi_rsdp));
            }
        } وإلا إذا (نښان->type == MULTIBOOT2_نښان_MODULE) {
            multiboot2_module_t ثابت *module = (multiboot2_module_t ثابت *)behind_نښان;
            چاپ_کړه(
                "  module #%d: start=0x%x end=0x%x size=0x%x name='%s'\n",
                mod_شمېر,
                module->پیل,
                module->پای_ټکی,
                module->پای_ټکی - module->پیل,
                module->string
            );

            إذا (mod_شمېر == 0) {
                boot_state.boot_module_پیل = module->پیل;
                boot_state.boot_module_پای_ټکی = module->پای_ټکی;
            }

            mod_شمېر ++;
            إذا ((sword_t)(module->پای_ټکی - module->پیل) <= 0) {
                چاپ_کړه("Invalid boot module size! Possible cause: boot module file not found\n");
                أرجع false;
            }
            إذا (boot_state.mods_پای_ټکی_paddr < module->پای_ټکی) {
                boot_state.mods_پای_ټکی_paddr = module->پای_ټکی;
            }
        } وإلا إذا (نښان->type == MULTIBOOT2_نښان_حافظه) {
            multiboot2_حافظه_t ثابت *s = (multiboot2_حافظه_t *)(behind_نښان + 8);
            multiboot2_حافظه_t ثابت *e = (multiboot2_حافظه_t *)((کلمه_t)نښان + نښان->اندازه);

            لكل (multiboot2_حافظه_t ثابت *m = s; m < e; m++) {
                إذا (!m->addr) {
                    boot_state.mem_ښکتنی = m->اندازه;
                }

                چاپ_کړه("\tPhysical Memory Region from %llx size %llx type %u\n", m->addr, m->اندازه, m->type);
                إذا (m->addr != (uint64_t)(کلمه_t)m->addr) {
                    چاپ_کړه("\t\tPhysical memory region not addressable\n");
                    استمر;
                }

                إذا (m->type == MULTIBOOT_MMAP_USEABLE_TYPE && m->addr >= HIGHMEM_PADDR) {
                    إذا (!جمع_کړه_mem_p_regs((p_region_t) {
                    m->addr, m->addr + m->اندازه
                }))
                    أرجع false;
                }
            }
        } وإلا إذا (نښان->type == MULTIBOOT2_نښان_FB) {
            multiboot2_fb_t ثابت *fb = (multiboot2_fb_t ثابت *)behind_نښان;
            چاپ_کړه("Got framebuffer info in multiboot2. Current video mode is at physical address=%llx pitch=%u resolution=%ux%u@%u type=%u\n",
                   fb->addr, fb->pitch, fb->پلنوالی, fb->لوړوالی, fb->bpp, fb->type);
            boot_state.fb_info = *fb;
        }

        نښان = (multiboot2_نښان_t ثابت *)((کلمه_t)نښان + پړاو_پورته(نښان->اندازه, 3));
    }

    چاپ_کړه("Detected %d boot module(s):\n", mod_شمېر);

    إذا (mod_شمېر < 1) {
        چاپ_کړه("Expect at least one boot module (containing a userland image)\n");
        أرجع false;
    }

    أرجع true;
}

BOOT_کوډ VISIBLE فراغ boot_sys(
    بینښه اوږد multiboot_magic,
    فراغ *mbi)
{
    bool_t پایله = false;

    إذا (multiboot_magic == MULTIBOOT_MAGIC) {
        پایله = try_boot_sys_mbi1(mbi);
    } وإلا إذا (multiboot_magic == MULTIBOOT2_MAGIC) {
        پایله = try_boot_sys_mbi2(mbi);
    } وإلا {
        چاپ_کړه("Boot loader is not multiboot 1 or 2 compliant %lx\n", multiboot_magic);
    }

    إذا (پایله) {
        پایله = try_boot_sys();
    }

    إذا (!پایله) {
        fail("boot_sys failed for some reason :(\n");
    }

    ARCH_غوټه_STATE(x86_KScur_مداخله) = int_invalid;
    ARCH_غوټه_STATE(x86_KSPending_مداخله) = int_invalid;

#که_تعریف_شوی CONFIG_KERNEL_MCS
    غوټه_STATE(ks_Cur_وخت) = get_Current_وخت();
    غوټه_STATE(ksConsumed) = 0;
#د_که_پای

    مهالوېش();
    activateThread();
}

