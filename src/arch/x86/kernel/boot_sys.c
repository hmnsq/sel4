/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <util.h>
#شامل <hardware.h>
#شامل <machine/io.h>
#شامل <arch/machine.h>
#شامل <arch/kernel/apic.h>
#شامل <arch/kernel/cmdline.h>
#شامل <arch/kernel/boot.h>
#شامل <arch/kernel/boot_sys.h>
#شامل <arch/kernel/smp_sys.h>
#شامل <arch/kernel/vspace.h>
#شامل <arch/kernel/elf.h>
#شامل <smp/lock.h>
#شامل <linker.h>
#شامل <plat/machine/acpi.h>
#شامل <plat/machine/devices.h>
#شامل <plat/machine/pic.h>
#شامل <plat/machine/ioapic.h>
#شامل <sel4/arch/bootinfo_types.h>

/* addresses defined in linker script */
/* need a fake array to get the pointer from the linker script */

/* start/end of CPU boot code */
خارجی حرف boot_cpu_آغاز[1];
خارجی حرف boot_cpu_انتہا[1];

/* start/end of boot stack */
خارجی حرف boot_ڈھیر_bottom[1];
خارجی حرف boot_ڈھیر_چوٹی[1];

/* locations in kernel image */
خارجی حرف ki_skim_آغاز[1];
خارجی حرف ki_skim_انتہا[1];

#اگر_تعریف_شدہ CONFIG_PRINTING
/* kernel entry point */
خارجی حرف آغاز_2[1];
#اگر_ختم

/* constants */

#تعریف_کرو HIGHMEM_PADDR 0x100000

BOOT_BSS
boot_state_t boot_state;

/* global variables (not covered by abstract specification) */

BOOT_BSS
cmdline_opt_t cmdline_opt;

/* functions not modeled in abstract specification */

BOOT_رمز ساکن paddr_t ڈھونڈو_بھرو_paddr(paddr_t min_paddr, لفظ_t image_سائز)
{
    صحیح i;

    برائے (i = 0; i < boot_state.mem_p_regs.گنتی; i++) {
        paddr_t آغاز = MAX(min_paddr, boot_state.mem_p_regs.list[i].آغاز);
        paddr_t انتہا = boot_state.mem_p_regs.list[i].انتہا;
        لفظ_t region_سائز = انتہا - آغاز;

        اگر (region_سائز >= image_سائز) {
            واپس آغاز;
        }
    }

    واپس 0;
}

BOOT_رمز ساکن paddr_t بھرو_boot_module(لفظ_t boot_module_آغاز, لفظ_t boot_module_انتہا, paddr_t بھرو_paddr)
{
    v_region_t v_reg;
    لفظ_t اندراج_2;
    Elf_سرخی_t *elf_file = (Elf_سرخی_t *)boot_module_آغاز;

    اگر (!elf_checkFile(elf_file, boot_module_انتہا - boot_module_آغاز)) {
        لکھو("Boot module does not contain a valid ELF image\n");
        واپس 0;
    }

    v_reg = elf_get_حافظہ_Bounds(elf_file);
    اندراج_2 = elf_file->e_اندراج;

    اگر (v_reg.انتہا == 0) {
        لکھو("ELF image in boot module does not contain any segments\n");
        واپس 0;
    }
    v_reg.انتہا = باری_اوپر(v_reg.انتہا, صفحہ_بٹ);

    لکھو("size=0x%lx v_entry=%p v_start=%p v_end=%p ",
           v_reg.انتہا - v_reg.آغاز,
           (خالی *)اندراج_2,
           (خالی *)v_reg.آغاز,
           (خالی *)v_reg.انتہا
          );

    اگر (!IS_ALIGNED(v_reg.آغاز, صفحہ_بٹ)) {
        لکھو("Userland image virtual start address must be 4KB-aligned\n");
        واپس 0;
    }
    اگر (v_reg.انتہا + 2 * ثنائی_ہندسہ(صفحہ_بٹ) > USER_چوٹی) {
        /* for IPC buffer frame and bootinfo frame, need 2*4K of additional userland virtual memory */
        لکھو("Userland image virtual end address too high\n");
        واپس 0;
    }
    اگر ((اندراج_2 < v_reg.آغاز) || (اندراج_2 >= v_reg.انتہا)) {
        لکھو("Userland image entry point does not lie within userland image\n");
        واپس 0;
    }

    بھرو_paddr = ڈھونڈو_بھرو_paddr(بھرو_paddr, v_reg.انتہا - v_reg.آغاز);
    أكد(بھرو_paddr);

    /* fill ui_info struct */
    boot_state.ui_info.pv_انحراف = بھرو_paddr - v_reg.آغاز;
    boot_state.ui_info.p_reg.آغاز = بھرو_paddr;
    بھرو_paddr += v_reg.انتہا - v_reg.آغاز;
    boot_state.ui_info.p_reg.انتہا = بھرو_paddr;
    boot_state.ui_info.v_اندراج = اندراج_2;

    لکھو("p_start=0x%lx p_end=0x%lx\n",
           boot_state.ui_info.p_reg.آغاز,
           boot_state.ui_info.p_reg.انتہا
          );

    /* initialise all initial userland memory and load potentially sparse ELF image */
    memzero(
        (خالی *)boot_state.ui_info.p_reg.آغاز,
        boot_state.ui_info.p_reg.انتہا - boot_state.ui_info.p_reg.آغاز
    );
    elf_بھرو(elf_file, boot_state.ui_info.pv_انحراف);

    واپس بھرو_paddr;
}

ساکن BOOT_رمز bool_t try_boot_sys_گرہ(cpu_id_t cpu_id)
{
    اگر (!map_kernel_کھڑکی(
            boot_state.عدد_ioapic,
            boot_state.ioapic_paddr,
            boot_state.عدد_drhu,
            boot_state.drhu_list
        )) {
        واپس false;
    }
    سیٹ_Current_VSpace_جڑ(kpptr_مقصد_paddr(X86_KERNEL_VSPACE_جڑ), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm متلون("" ::: "memory");

#اگر_تعریف_شدہ CONFIG_KERNEL_SKIM_کھڑکی
    اگر (!map_skim_کھڑکی((vptr_t)ki_skim_آغاز, (vptr_t)ki_skim_انتہا)) {
        واپس false;
    }
#اگر_ختم

    /* initialise the CPU */
    اگر (!init_cpu(config_سیٹ(CONFIG_IRQ_IOAPIC) ? 1 : 0)) {
        واپس false;
    }

    /* initialise NDKS and kernel heap */
    اگر (!init_sys_state(
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
        واپس false;
    }

    واپس true;
}

ساکن BOOT_رمز bool_t جوڑو_mem_p_regs(p_region_t reg)
{
    اگر (reg.آغاز == reg.انتہا) {
        // Return true here if asked to add an empty region.
        // Some of the callers round down the end address to
        واپس true;
    }
    اگر (reg.انتہا > PADDR_چوٹی && reg.آغاز >= PADDR_چوٹی) {
        /* Return true here as it's not an error for there to exist memory outside the kernel window,
         * we're just going to ignore it and leave it to be given out as device memory */
        واپس true;
    }
    اگر (boot_state.mem_p_regs.گنتی == MAX_عدد_FREEMEM_REG) {
        لکھو("Dropping memory region 0x%lx-0x%lx, try increasing MAX_NUM_FREEMEM_REG\n", reg.آغاز, reg.انتہا);
        واپس false;
    }
    اگر (reg.انتہا > PADDR_چوٹی) {
        أكد(reg.آغاز < PADDR_چوٹی);
        /* Clamp a region to the top of the kernel window if it extends beyond */
        reg.انتہا = PADDR_چوٹی;
    }
    لکھو("Adding physical memory region 0x%lx-0x%lx\n", reg.آغاز, reg.انتہا);
    boot_state.mem_p_regs.list[boot_state.mem_p_regs.گنتی] = reg;
    boot_state.mem_p_regs.گنتی++;
    واپس true;
}

/*
 * the code relies that the GRUB provides correct information
 * about the actual physical memory regions.
 */
ساکن BOOT_رمز bool_t parse_mem_map(uint32_t mmap_لمبائی, uint32_t mmap_addr)
{
    multiboot_mmap_t *mmap = (multiboot_mmap_t *)((لفظ_t)mmap_addr);
    لکھو("Parsing GRUB physical memory map\n");

    جب ((لفظ_t)mmap < (لفظ_t)(mmap_addr + mmap_لمبائی)) {
        uint64_t mem_آغاز = mmap->بنیاد_addr;
        uint64_t mem_لمبائی = mmap->لمبائی;
        uint32_t type = mmap->type;
        اگر (mem_آغاز != (uint64_t)(لفظ_t)mem_آغاز) {
            لکھو("\tPhysical memory region not addressable\n");
        } ورنہ {
            لکھو("\tPhysical Memory Region from %lx size %lx type %d\n", (طویل)mem_آغاز, (طویل)mem_لمبائی, type);
            اگر (type == MULTIBOOT_MMAP_USEABLE_TYPE && mem_آغاز >= HIGHMEM_PADDR && mem_لمبائی >= ثنائی_ہندسہ(صفحہ_بٹ)) {

                اگر (!جوڑو_mem_p_regs((p_region_t) {
                باری_اوپر(mem_آغاز, صفحہ_بٹ), باری_نیچے_کو(mem_آغاز + mem_لمبائی, صفحہ_بٹ),
                })) {
                    واپس false;
                }
            }
        }
        mmap++;
    }
    واپس true;
}

ساکن BOOT_رمز bool_t is_compiled_for_microarchitecture(خالی)
{
    لفظ_t microarch_generation = 0;
    x86_cpu_شناختی_عنصر_t *model_info = x86_cpuid_get_model_info();

    اگر (config_سیٹ(CONFIG_ARCH_X86_SKYLAKE)) {
        microarch_generation = 7;
    } ورنہ اگر (config_سیٹ(CONFIG_ARCH_X86_BROADWELL)) {
        microarch_generation = 6;
    } ورنہ اگر (config_سیٹ(CONFIG_ARCH_X86_HASWELL)) {
        microarch_generation = 5;
    } ورنہ اگر (config_سیٹ(CONFIG_ARCH_X86_IVY)) {
        microarch_generation = 4;
    } ورنہ اگر (config_سیٹ(CONFIG_ARCH_X86_SANDY)) {
        microarch_generation = 3;
    } ورنہ اگر (config_سیٹ(CONFIG_ARCH_X86_WESTMERE)) {
        microarch_generation = 2;
    } ورنہ اگر (config_سیٹ(CONFIG_ARCH_X86_NEHALEM)) {
        microarch_generation = 1;
    }

    انتخاب (model_info->model) {
    حالت SKYLAKE_1_MODEL_ID:
    حالت SKYLAKE_2_MODEL_ID:
    حالت SKYLAKE_X_MODEL_ID:
        اگر (microarch_generation > 7) {
            واپس false;
        }
        توڑو;

    حالت BROADWELL_1_MODEL_ID:
    حالت BROADWELL_2_MODEL_ID:
    حالت BROADWELL_3_MODEL_ID:
    حالت BROADWELL_4_MODEL_ID:
    حالت BROADWELL_5_MODEL_ID:
        اگر (microarch_generation > 6) {
            واپس false;
        }
        توڑو;

    حالت HASWELL_1_MODEL_ID:
    حالت HASWELL_2_MODEL_ID:
    حالت HASWELL_3_MODEL_ID:
    حالت HASWELL_4_MODEL_ID:
        اگر (microarch_generation > 5) {
            واپس false;
        }
        توڑو;

    حالت IVY_BRIDGE_1_MODEL_ID:
    حالت IVY_BRIDGE_2_MODEL_ID:
    حالت IVY_BRIDGE_3_MODEL_ID:
        اگر (microarch_generation > 4) {
            واپس false;
        }
        توڑو;

    حالت SANDY_BRIDGE_1_MODEL_ID:
    حالت SANDY_BRIDGE_2_MODEL_ID:
        اگر (microarch_generation > 3) {
            واپس false;
        }
        توڑو;

    حالت WESTMERE_1_MODEL_ID:
    حالت WESTMERE_2_MODEL_ID:
    حالت WESTMERE_3_MODEL_ID:
        اگر (microarch_generation > 2) {
            واپس false;
        }
        توڑو;

    حالت NEHALEM_1_MODEL_ID:
    حالت NEHALEM_2_MODEL_ID:
    حالت NEHALEM_3_MODEL_ID:
        اگر (microarch_generation > 1) {
            واپس false;
        }
        توڑو;

    طےشدہ:
        اگر (!config_سیٹ(CONFIG_ARCH_X86_GENERIC)) {
            واپس false;
        }
    }

    واپس true;
}

ساکن BOOT_رمز bool_t try_boot_sys(خالی)
{
    paddr_t mods_انتہا_paddr = boot_state.mods_انتہا_paddr;
    p_region_t ui_p_regs;
    paddr_t بھرو_paddr;

    boot_state.ki_p_reg = get_p_reg_kernel_img();

    اگر (!x86_cpuid_initialize()) {
        لکھو("Warning: Your x86 CPU has an unsupported vendor, '%s'.\n"
               "\tYour setup may not be able to competently run seL4 as "
               "intended.\n"
               "\tCurrently supported x86 vendors are AMD and Intel.\n",
               x86_cpuid_get_شناختی_عنصر()->vendor_string);
    }

    اگر (!is_compiled_for_microarchitecture()) {
        لکھو("Warning: Your kernel was not compiled for the current microarchitecture.\n");
    }

    cpuid_007h_edx_t edx;
    edx.الفاظ[0] = x86_cpuid_edx(0x7, 0);
    /* see if we can definitively say whether or not we need the skim window by
     * checking whether the CPU is vulnerable to rogue data cache loads (rdcl) */
    اگر (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.الفاظ[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        اگر (ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && config_سیٹ(CONFIG_KERNEL_SKIM_کھڑکی)) {
            لکھو("CPU reports not vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is enabled. Performance is needlessly being impacted, consider disabling.\n");
        } ورنہ اگر (!ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && !config_سیٹ(CONFIG_KERNEL_SKIM_کھڑکی)) {
            لکھو("CPU reports vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is *not* enabled. Please re-build with SKIM window enabled.");
            واپس false;
        }
    } ورنہ {
        /* hardware doesn't tell us directly so guess based on CPU vendor */
        اگر (config_سیٹ(CONFIG_KERNEL_SKIM_کھڑکی) && x86_cpuid_get_شناختی_عنصر()->vendor == X86_VENDOR_AMD) {
            لکھو("SKIM window for mitigating Meltdown (https://www.meltdownattack.com) "
                   "not necessary for AMD and performance is being needlessly affected, "
                   "consider disabling\n");
        }
        اگر (!config_سیٹ(CONFIG_KERNEL_SKIM_کھڑکی) && x86_cpuid_get_شناختی_عنصر()->vendor == X86_VENDOR_INTEL) {
            لکھو("***WARNING*** SKIM window not enabled, this machine is probably vulnerable "
                   "to Meltdown (https://www.meltdownattack.com), consider enabling\n");
        }
    }

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    /* copy boot code for APs to lower memory to run in real mode */
    اگر (!copy_boot_رمز_aps(boot_state.mem_زیریں)) {
        واپس false;
    }
    /* Initialize any kernel TLS */
    منوال_init_tls(0);
#اگر_ختم /* ENABLE_SMP_SUPPORT */

    لکھو("Kernel loaded to: start=0x%lx end=0x%lx size=0x%lx entry=0x%lx\n",
           boot_state.ki_p_reg.آغاز,
           boot_state.ki_p_reg.انتہا,
           boot_state.ki_p_reg.انتہا - boot_state.ki_p_reg.آغاز,
           (paddr_t)آغاز_2
          );

    /* remapping legacy IRQs to their correct vectors */
    pic_remap_irqs(IRQ_INT_انحراف);
    اگر (config_سیٹ(CONFIG_IRQ_IOAPIC)) {
        /* Disable the PIC so that it does not generate any interrupts. We need to
         * do this *before* we initialize the apic */
        pic_disable();
    }

    /* validate the ACPI table */
    اگر (!acpi_validate_rsdp(&boot_state.acpi_rsdp)) {
        واپس false;
    }

    /* check if kernel configuration matches platform requirements */
    اگر (!acpi_fadt_جائزہ(&boot_state.acpi_rsdp)) {
        واپس false;
    }

    اگر (!config_سیٹ(CONFIG_IOMMU) || cmdline_opt.disable_iommu) {
        boot_state.عدد_drhu = 0;
    } ورنہ {
        /* query available IOMMUs from ACPI */
        acpi_dmar_جائزہ(
            &boot_state.acpi_rsdp,
            boot_state.drhu_list,
            &boot_state.عدد_drhu,
            MAX_عدد_DRHU,
            &boot_state.rmrr_list
        );
    }

    /* query available CPUs from ACPI */
    boot_state.عدد_cpus = acpi_madt_جائزہ(&boot_state.acpi_rsdp, boot_state.cpus, &boot_state.عدد_ioapic,
                                         boot_state.ioapic_paddr);
    اگر (boot_state.عدد_cpus == 0) {
        لکھو("No CPUs detected\n");
        واپس false;
    }

    اگر (config_سیٹ(CONFIG_IRQ_IOAPIC)) {
        اگر (boot_state.عدد_ioapic == 0) {
            لکھو("No IOAPICs detected\n");
            واپس false;
        }
    } ورنہ {
        اگر (boot_state.عدد_ioapic > 0) {
            لکھو("Detected %d IOAPICs, but configured to use PIC instead\n", boot_state.عدد_ioapic);
        }
    }

    mods_انتہا_paddr = باری_اوپر(mods_انتہا_paddr, صفحہ_بٹ);
    أكد(mods_انتہا_paddr > boot_state.ki_p_reg.انتہا);

    لکھو("ELF-loading userland images from boot modules:\n");
    بھرو_paddr = mods_انتہا_paddr;

    بھرو_paddr = بھرو_boot_module(boot_state.boot_module_آغاز, boot_state.boot_module_انتہا, بھرو_paddr);
    اگر (!بھرو_paddr) {
        واپس false;
    }

    /* calculate final location of userland images */
    ui_p_regs.آغاز = boot_state.ki_p_reg.انتہا;
    ui_p_regs.انتہا = ui_p_regs.آغاز + بھرو_paddr - mods_انتہا_paddr;

    لکھو(
        "Moving loaded userland images to final location: from=0x%lx to=0x%lx size=0x%lx\n",
        mods_انتہا_paddr,
        ui_p_regs.آغاز,
        ui_p_regs.انتہا - ui_p_regs.آغاز
    );
    حافظہ_نقل((خالی *)ui_p_regs.آغاز, (خالی *)mods_انتہا_paddr, ui_p_regs.انتہا - ui_p_regs.آغاز);

    /* adjust p_reg and pv_offset to final load address */
    boot_state.ui_info.p_reg.آغاز -= mods_انتہا_paddr - ui_p_regs.آغاز;
    boot_state.ui_info.p_reg.انتہا   -= mods_انتہا_paddr - ui_p_regs.آغاز;
    boot_state.ui_info.pv_انحراف   -= mods_انتہا_paddr - ui_p_regs.آغاز;

    /* ==== following code corresponds to abstract specification after "select" ==== */

    اگر (!plat_جوڑو_Devices()) {
        واپس false;
    }

    /* Total number of cores we intend to boot */
    ks_عدد_CPUs = boot_state.عدد_cpus;

    لکھو("Starting node #0 with APIC ID %lu\n", boot_state.cpus[0]);
    اگر (!try_boot_sys_گرہ(boot_state.cpus[0])) {
        واپس false;
    }

    اگر (config_سیٹ(CONFIG_IRQ_IOAPIC)) {
        ioapic_init(1, boot_state.cpus, boot_state.عدد_ioapic);
    }

    /* initialize BKL before booting up APs */
    SMP_COND_STATEMENT(clh_تالا_init());
    SMP_COND_STATEMENT(آغاز_boot_aps());

    /* grab BKL before leaving the kernel */
    گرہ_تالا_SYS;

    لکھو("Booting all finished, dropped to user space\n");

    واپس true;
}

ساکن BOOT_رمز bool_t try_boot_sys_mbi1(
    multiboot_info_t *mbi
)
{
    لفظ_t i;
    multiboot_module_t *modules = (multiboot_module_t *)(لفظ_t)mbi->part1.mod_list;

    cmdline_parse((ثابت حرف *)(لفظ_t)mbi->part1.cmdline, &cmdline_opt);

    اگر ((mbi->part1.flags & MULTIBOOT_INFO_MEM_FLAG) == 0) {
        لکھو("Boot loader did not provide information about physical memory size\n");
        واپس false;
    }

    اگر (!(mbi->part1.flags & MULTIBOOT_INFO_MODS_FLAG)) {
        لکھو("Boot loader did not provide information about boot modules\n");
        واپس false;
    }

    لکھو("Detected %d boot module(s):\n", mbi->part1.mod_گنتی);

    اگر (mbi->part1.mod_گنتی < 1) {
        لکھو("Expect at least one boot module (containing a userland image)\n");
        واپس false;
    }

    برائے (i = 0; i < mbi->part1.mod_گنتی; i++) {
        لکھو(
            "  module #%ld: start=0x%x end=0x%x size=0x%x name='%s'\n",
            i,
            modules[i].آغاز,
            modules[i].انتہا,
            modules[i].انتہا - modules[i].آغاز,
            (حرف *)(طویل)modules[i].name
        );
        اگر ((sword_t)(modules[i].انتہا - modules[i].آغاز) <= 0) {
            لکھو("Invalid boot module size! Possible cause: boot module file not found by QEMU\n");
            واپس false;
        }
        اگر (boot_state.mods_انتہا_paddr < modules[i].انتہا) {
            boot_state.mods_انتہا_paddr = modules[i].انتہا;
        }
    }

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.گنتی = 0;
    اگر (mbi->part1.flags & MULTIBOOT_INFO_MMAP_FLAG) {
        اگر (!parse_mem_map(mbi->part2.mmap_لمبائی, mbi->part2.mmap_addr)) {
            واپس false;
        }
        uint32_t multiboot_mmap_لمبائی = mbi->part2.mmap_لمبائی;
        اگر (multiboot_mmap_لمبائی > (SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * حجم(seL4_X86_mb_mmap_t))) {
            لکھو("Warning: Multiboot has reported more memory map entries, %zd, "
                   "than the max amount that will be passed in the bootinfo, %d. "
                   "These extra regions will still be turned into untyped caps.",
                   multiboot_mmap_لمبائی / حجم(seL4_X86_mb_mmap_t), SEL4_MULTIBOOT_MAX_MMAP_ENTRIES);
            multiboot_mmap_لمبائی = SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * حجم(seL4_X86_mb_mmap_t);
        }
        حافظہ_نقل(&boot_state.mb_mmap_info.mmap, (خالی *)(لفظ_t)mbi->part2.mmap_addr, multiboot_mmap_لمبائی);
        boot_state.mb_mmap_info.mmap_لمبائی = multiboot_mmap_لمبائی;
    } ورنہ {
        /* calculate memory the old way */
        p_region_t avail;
        avail.آغاز = HIGHMEM_PADDR;
        avail.انتہا = باری_نیچے_کو(avail.آغاز + (mbi->part1.mem_بالائی << 10), صفحہ_بٹ);
        اگر (!جوڑو_mem_p_regs(avail)) {
            واپس false;
        }
    }

    /* copy VESA information from multiboot header */
    اگر ((mbi->part1.flags & MULTIBOOT_INFO_GRAPHICS_FLAG) == 0) {
        boot_state.vbe_info.vbe_منوال = -1;
        لکھو("Multiboot gave us no video information\n");
    } ورنہ {
        boot_state.vbe_info.vbe_Info_بلاک = *(se_L4_VBEInfo_بلاک_t *)(se_L4_لفظ)mbi->part2.vbe_control_info;
        boot_state.vbe_info.vbe_منوال_Info_بلاک = *(se_L4_VBEMode_Info_بلاک_t *)(se_L4_لفظ)mbi->part2.vbe_منوال_info;
        boot_state.vbe_info.vbe_منوال = mbi->part2.vbe_منوال_2;
        لکھو("Got VBE info in multiboot. Current video mode is %d\n", mbi->part2.vbe_منوال_2);
        boot_state.vbe_info.vbeInterfaceSeg = mbi->part2.vbe_interface_seg;
        boot_state.vbe_info.vbeInterfaceOff = mbi->part2.vbe_interface_off;
        boot_state.vbe_info.vbeInterfaceLen = mbi->part2.vbe_interface_len;
    }

    boot_state.mem_زیریں = mbi->part1.mem_زیریں;
    boot_state.boot_module_آغاز = modules->آغاز;
    boot_state.boot_module_انتہا = modules->انتہا;

    /* Initialize ACPI */
    اگر (!acpi_init(&boot_state.acpi_rsdp)) {
        واپس false;
    }

    واپس true;
}

ساکن BOOT_رمز bool_t try_boot_sys_mbi2(
    multiboot2_سرخی_t *mbi2
)
{
    صحیح mod_گنتی                  = 0;
    multiboot2_لیبل_t ثابت *لیبل   = (multiboot2_لیبل_t *)(mbi2 + 1);
    multiboot2_لیبل_t ثابت *لیبل_e = (multiboot2_لیبل_t *)((لفظ_t)mbi2 + mbi2->کل_سائز);

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.گنتی = 0;
    boot_state.mb_mmap_info.mmap_لمبائی = 0;
    boot_state.vbe_info.vbe_منوال = -1;

    جب (لیبل < لیبل_e && لیبل->type != MULTIBOOT2_لیبل_انتہا) {
        لفظ_t ثابت behind_لیبل = (لفظ_t)لیبل + حجم(*لیبل);

        اگر (لیبل->type == MULTIBOOT2_لیبل_CMDLINE) {
            حرف ثابت *ثابت cmdline = (حرف ثابت * ثابت)(behind_لیبل);
            cmdline_parse(cmdline, &cmdline_opt);
        } ورنہ اگر (لیبل->type == MULTIBOOT2_لیبل_ACPI_1) {
            اگر (ACPI_V1_سائز == لیبل->سائز - حجم(*لیبل)) {
                حافظہ_نقل(&boot_state.acpi_rsdp, (خالی *)behind_لیبل, لیبل->سائز - حجم(*لیبل));
            }
        } ورنہ اگر (لیبل->type == MULTIBOOT2_لیبل_ACPI_2) {
            اگر (حجم(boot_state.acpi_rsdp) == لیبل->سائز - حجم(*لیبل)) {
                حافظہ_نقل(&boot_state.acpi_rsdp, (خالی *)behind_لیبل, حجم(boot_state.acpi_rsdp));
            }
        } ورنہ اگر (لیبل->type == MULTIBOOT2_لیبل_MODULE) {
            multiboot2_module_t ثابت *module = (multiboot2_module_t ثابت *)behind_لیبل;
            لکھو(
                "  module #%d: start=0x%x end=0x%x size=0x%x name='%s'\n",
                mod_گنتی,
                module->آغاز,
                module->انتہا,
                module->انتہا - module->آغاز,
                module->string
            );

            اگر (mod_گنتی == 0) {
                boot_state.boot_module_آغاز = module->آغاز;
                boot_state.boot_module_انتہا = module->انتہا;
            }

            mod_گنتی ++;
            اگر ((sword_t)(module->انتہا - module->آغاز) <= 0) {
                لکھو("Invalid boot module size! Possible cause: boot module file not found\n");
                واپس false;
            }
            اگر (boot_state.mods_انتہا_paddr < module->انتہا) {
                boot_state.mods_انتہا_paddr = module->انتہا;
            }
        } ورنہ اگر (لیبل->type == MULTIBOOT2_لیبل_حافظہ) {
            multiboot2_حافظہ_t ثابت *s = (multiboot2_حافظہ_t *)(behind_لیبل + 8);
            multiboot2_حافظہ_t ثابت *e = (multiboot2_حافظہ_t *)((لفظ_t)لیبل + لیبل->سائز);

            برائے (multiboot2_حافظہ_t ثابت *m = s; m < e; m++) {
                اگر (!m->addr) {
                    boot_state.mem_زیریں = m->سائز;
                }

                لکھو("\tPhysical Memory Region from %llx size %llx type %u\n", m->addr, m->سائز, m->type);
                اگر (m->addr != (uint64_t)(لفظ_t)m->addr) {
                    لکھو("\t\tPhysical memory region not addressable\n");
                    جاری;
                }

                اگر (m->type == MULTIBOOT_MMAP_USEABLE_TYPE && m->addr >= HIGHMEM_PADDR) {
                    اگر (!جوڑو_mem_p_regs((p_region_t) {
                    m->addr, m->addr + m->سائز
                }))
                    واپس false;
                }
            }
        } ورنہ اگر (لیبل->type == MULTIBOOT2_لیبل_FB) {
            multiboot2_fb_t ثابت *fb = (multiboot2_fb_t ثابت *)behind_لیبل;
            لکھو("Got framebuffer info in multiboot2. Current video mode is at physical address=%llx pitch=%u resolution=%ux%u@%u type=%u\n",
                   fb->addr, fb->pitch, fb->چوڑائی, fb->اونچائی, fb->bpp, fb->type);
            boot_state.fb_info = *fb;
        }

        لیبل = (multiboot2_لیبل_t ثابت *)((لفظ_t)لیبل + باری_اوپر(لیبل->سائز, 3));
    }

    لکھو("Detected %d boot module(s):\n", mod_گنتی);

    اگر (mod_گنتی < 1) {
        لکھو("Expect at least one boot module (containing a userland image)\n");
        واپس false;
    }

    واپس true;
}

BOOT_رمز VISIBLE خالی boot_sys(
    غیرعلامتی طویل multiboot_magic,
    خالی *mbi)
{
    bool_t نتیجہ = false;

    اگر (multiboot_magic == MULTIBOOT_MAGIC) {
        نتیجہ = try_boot_sys_mbi1(mbi);
    } ورنہ اگر (multiboot_magic == MULTIBOOT2_MAGIC) {
        نتیجہ = try_boot_sys_mbi2(mbi);
    } ورنہ {
        لکھو("Boot loader is not multiboot 1 or 2 compliant %lx\n", multiboot_magic);
    }

    اگر (نتیجہ) {
        نتیجہ = try_boot_sys();
    }

    اگر (!نتیجہ) {
        fail("boot_sys failed for some reason :(\n");
    }

    ARCH_گرہ_STATE(x86_KScur_مداخلت) = int_invalid;
    ARCH_گرہ_STATE(x86_KSPending_مداخلت) = int_invalid;

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    گرہ_STATE(ks_Cur_وقت) = get_Current_وقت();
    گرہ_STATE(ksConsumed) = 0;
#اگر_ختم

    جدول_وقت();
    activateThread();
}

