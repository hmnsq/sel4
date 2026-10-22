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
خارجي حرف boot_cpu_سرا[1];

/* start/end of boot stack */
خارجي حرف boot_ڈھیر_bottom[1];
خارجي حرف boot_ڈھیر_چوٹی[1];

/* locations in kernel image */
خارجي حرف ki_skim_شروع[1];
خارجي حرف ki_skim_سرا[1];

#جے_تعریف_شدہ CONFIG_PRINTING
/* kernel entry point */
خارجي حرف شروع_2[1];
#جے_انت

/* constants */

#تعریف_کرو HIGHMEM_PADDR 0x100000

BOOT_BSS
boot_state_t boot_state;

/* global variables (not covered by abstract specification) */

BOOT_BSS
cmdline_opt_t cmdline_opt;

/* functions not modeled in abstract specification */

BOOT_کوڈ ساكن paddr_t لبھو_لدو_paddr(paddr_t min_paddr, لفظ_t image_ناپ)
{
    صحيح i;

    لكل (i = 0; i < boot_state.mem_p_regs.گنتی; i++) {
        paddr_t شروع = MAX(min_paddr, boot_state.mem_p_regs.list[i].شروع);
        paddr_t سرا = boot_state.mem_p_regs.list[i].سرا;
        لفظ_t region_ناپ = سرا - شروع;

        إذا (region_ناپ >= image_ناپ) {
            أرجع شروع;
        }
    }

    أرجع 0;
}

BOOT_کوڈ ساكن paddr_t لدو_boot_module(لفظ_t boot_module_شروع, لفظ_t boot_module_سرا, paddr_t لدو_paddr)
{
    v_region_t v_reg;
    لفظ_t اندراج;
    Elf_سرخی_t *elf_file = (Elf_سرخی_t *)boot_module_شروع;

    إذا (!elf_checkFile(elf_file, boot_module_سرا - boot_module_شروع)) {
        لکھو("Boot module does not contain a valid ELF image\n");
        أرجع 0;
    }

    v_reg = elf_get_حافظہ_Bounds(elf_file);
    اندراج = elf_file->e_اندراج;

    إذا (v_reg.سرا == 0) {
        لکھو("ELF image in boot module does not contain any segments\n");
        أرجع 0;
    }
    v_reg.سرا = گیڑا_اتے(v_reg.سرا, ورقہ_بٹ);

    لکھو("size=0x%lx v_entry=%p v_start=%p v_end=%p ",
           v_reg.سرا - v_reg.شروع,
           (فراغ *)اندراج,
           (فراغ *)v_reg.شروع,
           (فراغ *)v_reg.سرا
          );

    إذا (!IS_ALIGNED(v_reg.شروع, ورقہ_بٹ)) {
        لکھو("Userland image virtual start address must be 4KB-aligned\n");
        أرجع 0;
    }
    إذا (v_reg.سرا + 2 * ثنائی_ہندسہ(ورقہ_بٹ) > USER_چوٹی) {
        /* for IPC buffer frame and bootinfo frame, need 2*4K of additional userland virtual memory */
        لکھو("Userland image virtual end address too high\n");
        أرجع 0;
    }
    إذا ((اندراج < v_reg.شروع) || (اندراج >= v_reg.سرا)) {
        لکھو("Userland image entry point does not lie within userland image\n");
        أرجع 0;
    }

    لدو_paddr = لبھو_لدو_paddr(لدو_paddr, v_reg.سرا - v_reg.شروع);
    أكد(لدو_paddr);

    /* fill ui_info struct */
    boot_state.ui_info.pv_وتھ = لدو_paddr - v_reg.شروع;
    boot_state.ui_info.p_reg.شروع = لدو_paddr;
    لدو_paddr += v_reg.سرا - v_reg.شروع;
    boot_state.ui_info.p_reg.سرا = لدو_paddr;
    boot_state.ui_info.v_اندراج = اندراج;

    لکھو("p_start=0x%lx p_end=0x%lx\n",
           boot_state.ui_info.p_reg.شروع,
           boot_state.ui_info.p_reg.سرا
          );

    /* initialise all initial userland memory and load potentially sparse ELF image */
    memzero(
        (فراغ *)boot_state.ui_info.p_reg.شروع,
        boot_state.ui_info.p_reg.سرا - boot_state.ui_info.p_reg.شروع
    );
    elf_لدو(elf_file, boot_state.ui_info.pv_وتھ);

    أرجع لدو_paddr;
}

ساكن BOOT_کوڈ bool_t try_boot_sys_گرہ(cpu_id_t cpu_id)
{
    إذا (!map_kernel_کھڑکی(
            boot_state.عدد_ioapic,
            boot_state.ioapic_paddr,
            boot_state.عدد_drhu,
            boot_state.drhu_list
        )) {
        أرجع false;
    }
    سیٹ_Current_VSpace_جڑ(kpptr_منزل_paddr(X86_KERNEL_VSPACE_جڑ), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm متطاير("" ::: "memory");

#جے_تعریف_شدہ CONFIG_KERNEL_SKIM_کھڑکی
    إذا (!map_skim_کھڑکی((vptr_t)ki_skim_شروع, (vptr_t)ki_skim_سرا)) {
        أرجع false;
    }
#جے_انت

    /* initialise the CPU */
    إذا (!init_cpu(config_سیٹ(CONFIG_IRQ_IOAPIC) ? 1 : 0)) {
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

ساكن BOOT_کوڈ bool_t جوڑو_mem_p_regs(p_region_t reg)
{
    إذا (reg.شروع == reg.سرا) {
        // Return true here if asked to add an empty region.
        // Some of the callers round down the end address to
        أرجع true;
    }
    إذا (reg.سرا > PADDR_چوٹی && reg.شروع >= PADDR_چوٹی) {
        /* Return true here as it's not an error for there to exist memory outside the kernel window,
         * we're just going to ignore it and leave it to be given out as device memory */
        أرجع true;
    }
    إذا (boot_state.mem_p_regs.گنتی == MAX_عدد_FREEMEM_REG) {
        لکھو("Dropping memory region 0x%lx-0x%lx, try increasing MAX_NUM_FREEMEM_REG\n", reg.شروع, reg.سرا);
        أرجع false;
    }
    إذا (reg.سرا > PADDR_چوٹی) {
        أكد(reg.شروع < PADDR_چوٹی);
        /* Clamp a region to the top of the kernel window if it extends beyond */
        reg.سرا = PADDR_چوٹی;
    }
    لکھو("Adding physical memory region 0x%lx-0x%lx\n", reg.شروع, reg.سرا);
    boot_state.mem_p_regs.list[boot_state.mem_p_regs.گنتی] = reg;
    boot_state.mem_p_regs.گنتی++;
    أرجع true;
}

/*
 * the code relies that the GRUB provides correct information
 * about the actual physical memory regions.
 */
ساكن BOOT_کوڈ bool_t parse_mem_map(uint32_t mmap_لمائی, uint32_t mmap_addr)
{
    multiboot_mmap_t *mmap = (multiboot_mmap_t *)((لفظ_t)mmap_addr);
    لکھو("Parsing GRUB physical memory map\n");

    طالما ((لفظ_t)mmap < (لفظ_t)(mmap_addr + mmap_لمائی)) {
        uint64_t mem_شروع = mmap->ادھار_addr;
        uint64_t mem_لمائی = mmap->لمائی;
        uint32_t type = mmap->type;
        إذا (mem_شروع != (uint64_t)(لفظ_t)mem_شروع) {
            لکھو("\tPhysical memory region not addressable\n");
        } وإلا {
            لکھو("\tPhysical Memory Region from %lx size %lx type %d\n", (لما)mem_شروع, (لما)mem_لمائی, type);
            إذا (type == MULTIBOOT_MMAP_USEABLE_TYPE && mem_شروع >= HIGHMEM_PADDR && mem_لمائی >= ثنائی_ہندسہ(ورقہ_بٹ)) {

                إذا (!جوڑو_mem_p_regs((p_region_t) {
                گیڑا_اتے(mem_شروع, ورقہ_بٹ), گیڑا_تھلے(mem_شروع + mem_لمائی, ورقہ_بٹ),
                })) {
                    أرجع false;
                }
            }
        }
        mmap++;
    }
    أرجع true;
}

ساكن BOOT_کوڈ bool_t is_compiled_for_microarchitecture(فراغ)
{
    لفظ_t microarch_generation = 0;
    x86_cpu_پچھان_تت_t *model_info = x86_cpuid_get_model_info();

    إذا (config_سیٹ(CONFIG_ARCH_X86_SKYLAKE)) {
        microarch_generation = 7;
    } وإلا إذا (config_سیٹ(CONFIG_ARCH_X86_BROADWELL)) {
        microarch_generation = 6;
    } وإلا إذا (config_سیٹ(CONFIG_ARCH_X86_HASWELL)) {
        microarch_generation = 5;
    } وإلا إذا (config_سیٹ(CONFIG_ARCH_X86_IVY)) {
        microarch_generation = 4;
    } وإلا إذا (config_سیٹ(CONFIG_ARCH_X86_SANDY)) {
        microarch_generation = 3;
    } وإلا إذا (config_سیٹ(CONFIG_ARCH_X86_WESTMERE)) {
        microarch_generation = 2;
    } وإلا إذا (config_سیٹ(CONFIG_ARCH_X86_NEHALEM)) {
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
        إذا (!config_سیٹ(CONFIG_ARCH_X86_GENERIC)) {
            أرجع false;
        }
    }

    أرجع true;
}

ساكن BOOT_کوڈ bool_t try_boot_sys(فراغ)
{
    paddr_t mods_سرا_paddr = boot_state.mods_سرا_paddr;
    p_region_t ui_p_regs;
    paddr_t لدو_paddr;

    boot_state.ki_p_reg = get_p_reg_kernel_img();

    إذا (!x86_cpuid_initialize()) {
        لکھو("Warning: Your x86 CPU has an unsupported vendor, '%s'.\n"
               "\tYour setup may not be able to competently run seL4 as "
               "intended.\n"
               "\tCurrently supported x86 vendors are AMD and Intel.\n",
               x86_cpuid_get_پچھان_تت()->vendor_string);
    }

    إذا (!is_compiled_for_microarchitecture()) {
        لکھو("Warning: Your kernel was not compiled for the current microarchitecture.\n");
    }

    cpuid_007h_edx_t edx;
    edx.الفاظ[0] = x86_cpuid_edx(0x7, 0);
    /* see if we can definitively say whether or not we need the skim window by
     * checking whether the CPU is vulnerable to rogue data cache loads (rdcl) */
    إذا (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.الفاظ[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        إذا (ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && config_سیٹ(CONFIG_KERNEL_SKIM_کھڑکی)) {
            لکھو("CPU reports not vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is enabled. Performance is needlessly being impacted, consider disabling.\n");
        } وإلا إذا (!ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && !config_سیٹ(CONFIG_KERNEL_SKIM_کھڑکی)) {
            لکھو("CPU reports vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is *not* enabled. Please re-build with SKIM window enabled.");
            أرجع false;
        }
    } وإلا {
        /* hardware doesn't tell us directly so guess based on CPU vendor */
        إذا (config_سیٹ(CONFIG_KERNEL_SKIM_کھڑکی) && x86_cpuid_get_پچھان_تت()->vendor == X86_VENDOR_AMD) {
            لکھو("SKIM window for mitigating Meltdown (https://www.meltdownattack.com) "
                   "not necessary for AMD and performance is being needlessly affected, "
                   "consider disabling\n");
        }
        إذا (!config_سیٹ(CONFIG_KERNEL_SKIM_کھڑکی) && x86_cpuid_get_پچھان_تت()->vendor == X86_VENDOR_INTEL) {
            لکھو("***WARNING*** SKIM window not enabled, this machine is probably vulnerable "
                   "to Meltdown (https://www.meltdownattack.com), consider enabling\n");
        }
    }

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    /* copy boot code for APs to lower memory to run in real mode */
    إذا (!copy_boot_کوڈ_aps(boot_state.mem_ہیٹھلا)) {
        أرجع false;
    }
    /* Initialize any kernel TLS */
    منوال_init_tls(0);
#جے_انت /* ENABLE_SMP_SUPPORT */

    لکھو("Kernel loaded to: start=0x%lx end=0x%lx size=0x%lx entry=0x%lx\n",
           boot_state.ki_p_reg.شروع,
           boot_state.ki_p_reg.سرا,
           boot_state.ki_p_reg.سرا - boot_state.ki_p_reg.شروع,
           (paddr_t)شروع_2
          );

    /* remapping legacy IRQs to their correct vectors */
    pic_remap_irqs(IRQ_INT_وتھ);
    إذا (config_سیٹ(CONFIG_IRQ_IOAPIC)) {
        /* Disable the PIC so that it does not generate any interrupts. We need to
         * do this *before* we initialize the apic */
        pic_disable();
    }

    /* validate the ACPI table */
    إذا (!acpi_validate_rsdp(&boot_state.acpi_rsdp)) {
        أرجع false;
    }

    /* check if kernel configuration matches platform requirements */
    إذا (!acpi_fadt_جانچ(&boot_state.acpi_rsdp)) {
        أرجع false;
    }

    إذا (!config_سیٹ(CONFIG_IOMMU) || cmdline_opt.disable_iommu) {
        boot_state.عدد_drhu = 0;
    } وإلا {
        /* query available IOMMUs from ACPI */
        acpi_dmar_جانچ(
            &boot_state.acpi_rsdp,
            boot_state.drhu_list,
            &boot_state.عدد_drhu,
            MAX_عدد_DRHU,
            &boot_state.rmrr_list
        );
    }

    /* query available CPUs from ACPI */
    boot_state.عدد_cpus = acpi_madt_جانچ(&boot_state.acpi_rsdp, boot_state.cpus, &boot_state.عدد_ioapic,
                                         boot_state.ioapic_paddr);
    إذا (boot_state.عدد_cpus == 0) {
        لکھو("No CPUs detected\n");
        أرجع false;
    }

    إذا (config_سیٹ(CONFIG_IRQ_IOAPIC)) {
        إذا (boot_state.عدد_ioapic == 0) {
            لکھو("No IOAPICs detected\n");
            أرجع false;
        }
    } وإلا {
        إذا (boot_state.عدد_ioapic > 0) {
            لکھو("Detected %d IOAPICs, but configured to use PIC instead\n", boot_state.عدد_ioapic);
        }
    }

    mods_سرا_paddr = گیڑا_اتے(mods_سرا_paddr, ورقہ_بٹ);
    أكد(mods_سرا_paddr > boot_state.ki_p_reg.سرا);

    لکھو("ELF-loading userland images from boot modules:\n");
    لدو_paddr = mods_سرا_paddr;

    لدو_paddr = لدو_boot_module(boot_state.boot_module_شروع, boot_state.boot_module_سرا, لدو_paddr);
    إذا (!لدو_paddr) {
        أرجع false;
    }

    /* calculate final location of userland images */
    ui_p_regs.شروع = boot_state.ki_p_reg.سرا;
    ui_p_regs.سرا = ui_p_regs.شروع + لدو_paddr - mods_سرا_paddr;

    لکھو(
        "Moving loaded userland images to final location: from=0x%lx to=0x%lx size=0x%lx\n",
        mods_سرا_paddr,
        ui_p_regs.شروع,
        ui_p_regs.سرا - ui_p_regs.شروع
    );
    حافظہ_نقل((فراغ *)ui_p_regs.شروع, (فراغ *)mods_سرا_paddr, ui_p_regs.سرا - ui_p_regs.شروع);

    /* adjust p_reg and pv_offset to final load address */
    boot_state.ui_info.p_reg.شروع -= mods_سرا_paddr - ui_p_regs.شروع;
    boot_state.ui_info.p_reg.سرا   -= mods_سرا_paddr - ui_p_regs.شروع;
    boot_state.ui_info.pv_وتھ   -= mods_سرا_paddr - ui_p_regs.شروع;

    /* ==== following code corresponds to abstract specification after "select" ==== */

    إذا (!plat_جوڑو_Devices()) {
        أرجع false;
    }

    /* Total number of cores we intend to boot */
    ks_عدد_CPUs = boot_state.عدد_cpus;

    لکھو("Starting node #0 with APIC ID %lu\n", boot_state.cpus[0]);
    إذا (!try_boot_sys_گرہ(boot_state.cpus[0])) {
        أرجع false;
    }

    إذا (config_سیٹ(CONFIG_IRQ_IOAPIC)) {
        ioapic_init(1, boot_state.cpus, boot_state.عدد_ioapic);
    }

    /* initialize BKL before booting up APs */
    SMP_COND_STATEMENT(clh_جندرا_init());
    SMP_COND_STATEMENT(شروع_boot_aps());

    /* grab BKL before leaving the kernel */
    گرہ_جندرا_SYS;

    لکھو("Booting all finished, dropped to user space\n");

    أرجع true;
}

ساكن BOOT_کوڈ bool_t try_boot_sys_mbi1(
    multiboot_info_t *mbi
)
{
    لفظ_t i;
    multiboot_module_t *modules = (multiboot_module_t *)(لفظ_t)mbi->part1.mod_list;

    cmdline_parse((ثابت حرف *)(لفظ_t)mbi->part1.cmdline, &cmdline_opt);

    إذا ((mbi->part1.flags & MULTIBOOT_INFO_MEM_FLAG) == 0) {
        لکھو("Boot loader did not provide information about physical memory size\n");
        أرجع false;
    }

    إذا (!(mbi->part1.flags & MULTIBOOT_INFO_MODS_FLAG)) {
        لکھو("Boot loader did not provide information about boot modules\n");
        أرجع false;
    }

    لکھو("Detected %d boot module(s):\n", mbi->part1.mod_گنتی);

    إذا (mbi->part1.mod_گنتی < 1) {
        لکھو("Expect at least one boot module (containing a userland image)\n");
        أرجع false;
    }

    لكل (i = 0; i < mbi->part1.mod_گنتی; i++) {
        لکھو(
            "  module #%ld: start=0x%x end=0x%x size=0x%x name='%s'\n",
            i,
            modules[i].شروع,
            modules[i].سرا,
            modules[i].سرا - modules[i].شروع,
            (حرف *)(لما)modules[i].name
        );
        إذا ((sword_t)(modules[i].سرا - modules[i].شروع) <= 0) {
            لکھو("Invalid boot module size! Possible cause: boot module file not found by QEMU\n");
            أرجع false;
        }
        إذا (boot_state.mods_سرا_paddr < modules[i].سرا) {
            boot_state.mods_سرا_paddr = modules[i].سرا;
        }
    }

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.گنتی = 0;
    إذا (mbi->part1.flags & MULTIBOOT_INFO_MMAP_FLAG) {
        إذا (!parse_mem_map(mbi->part2.mmap_لمائی, mbi->part2.mmap_addr)) {
            أرجع false;
        }
        uint32_t multiboot_mmap_لمائی = mbi->part2.mmap_لمائی;
        إذا (multiboot_mmap_لمائی > (SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * حجم(seL4_X86_mb_mmap_t))) {
            لکھو("Warning: Multiboot has reported more memory map entries, %zd, "
                   "than the max amount that will be passed in the bootinfo, %d. "
                   "These extra regions will still be turned into untyped caps.",
                   multiboot_mmap_لمائی / حجم(seL4_X86_mb_mmap_t), SEL4_MULTIBOOT_MAX_MMAP_ENTRIES);
            multiboot_mmap_لمائی = SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * حجم(seL4_X86_mb_mmap_t);
        }
        حافظہ_نقل(&boot_state.mb_mmap_info.mmap, (فراغ *)(لفظ_t)mbi->part2.mmap_addr, multiboot_mmap_لمائی);
        boot_state.mb_mmap_info.mmap_لمائی = multiboot_mmap_لمائی;
    } وإلا {
        /* calculate memory the old way */
        p_region_t avail;
        avail.شروع = HIGHMEM_PADDR;
        avail.سرا = گیڑا_تھلے(avail.شروع + (mbi->part1.mem_اتلا << 10), ورقہ_بٹ);
        إذا (!جوڑو_mem_p_regs(avail)) {
            أرجع false;
        }
    }

    /* copy VESA information from multiboot header */
    إذا ((mbi->part1.flags & MULTIBOOT_INFO_GRAPHICS_FLAG) == 0) {
        boot_state.vbe_info.vbe_منوال = -1;
        لکھو("Multiboot gave us no video information\n");
    } وإلا {
        boot_state.vbe_info.vbe_Info_بلاک = *(se_L4_VBEInfo_بلاک_t *)(se_L4_لفظ)mbi->part2.vbe_control_info;
        boot_state.vbe_info.vbe_منوال_Info_بلاک = *(se_L4_VBEMode_Info_بلاک_t *)(se_L4_لفظ)mbi->part2.vbe_منوال_info;
        boot_state.vbe_info.vbe_منوال = mbi->part2.vbe_منوال_2;
        لکھو("Got VBE info in multiboot. Current video mode is %d\n", mbi->part2.vbe_منوال_2);
        boot_state.vbe_info.vbeInterfaceSeg = mbi->part2.vbe_interface_seg;
        boot_state.vbe_info.vbeInterfaceOff = mbi->part2.vbe_interface_off;
        boot_state.vbe_info.vbeInterfaceLen = mbi->part2.vbe_interface_len;
    }

    boot_state.mem_ہیٹھلا = mbi->part1.mem_ہیٹھلا;
    boot_state.boot_module_شروع = modules->شروع;
    boot_state.boot_module_سرا = modules->سرا;

    /* Initialize ACPI */
    إذا (!acpi_init(&boot_state.acpi_rsdp)) {
        أرجع false;
    }

    أرجع true;
}

ساكن BOOT_کوڈ bool_t try_boot_sys_mbi2(
    multiboot2_سرخی_t *mbi2
)
{
    صحيح mod_گنتی                  = 0;
    multiboot2_ٹیگ_t ثابت *ٹیگ   = (multiboot2_ٹیگ_t *)(mbi2 + 1);
    multiboot2_ٹیگ_t ثابت *ٹیگ_e = (multiboot2_ٹیگ_t *)((لفظ_t)mbi2 + mbi2->کل_ناپ);

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.گنتی = 0;
    boot_state.mb_mmap_info.mmap_لمائی = 0;
    boot_state.vbe_info.vbe_منوال = -1;

    طالما (ٹیگ < ٹیگ_e && ٹیگ->type != MULTIBOOT2_ٹیگ_سرا) {
        لفظ_t ثابت behind_ٹیگ = (لفظ_t)ٹیگ + حجم(*ٹیگ);

        إذا (ٹیگ->type == MULTIBOOT2_ٹیگ_CMDLINE) {
            حرف ثابت *ثابت cmdline = (حرف ثابت * ثابت)(behind_ٹیگ);
            cmdline_parse(cmdline, &cmdline_opt);
        } وإلا إذا (ٹیگ->type == MULTIBOOT2_ٹیگ_ACPI_1) {
            إذا (ACPI_V1_ناپ == ٹیگ->ناپ - حجم(*ٹیگ)) {
                حافظہ_نقل(&boot_state.acpi_rsdp, (فراغ *)behind_ٹیگ, ٹیگ->ناپ - حجم(*ٹیگ));
            }
        } وإلا إذا (ٹیگ->type == MULTIBOOT2_ٹیگ_ACPI_2) {
            إذا (حجم(boot_state.acpi_rsdp) == ٹیگ->ناپ - حجم(*ٹیگ)) {
                حافظہ_نقل(&boot_state.acpi_rsdp, (فراغ *)behind_ٹیگ, حجم(boot_state.acpi_rsdp));
            }
        } وإلا إذا (ٹیگ->type == MULTIBOOT2_ٹیگ_MODULE) {
            multiboot2_module_t ثابت *module = (multiboot2_module_t ثابت *)behind_ٹیگ;
            لکھو(
                "  module #%d: start=0x%x end=0x%x size=0x%x name='%s'\n",
                mod_گنتی,
                module->شروع,
                module->سرا,
                module->سرا - module->شروع,
                module->string
            );

            إذا (mod_گنتی == 0) {
                boot_state.boot_module_شروع = module->شروع;
                boot_state.boot_module_سرا = module->سرا;
            }

            mod_گنتی ++;
            إذا ((sword_t)(module->سرا - module->شروع) <= 0) {
                لکھو("Invalid boot module size! Possible cause: boot module file not found\n");
                أرجع false;
            }
            إذا (boot_state.mods_سرا_paddr < module->سرا) {
                boot_state.mods_سرا_paddr = module->سرا;
            }
        } وإلا إذا (ٹیگ->type == MULTIBOOT2_ٹیگ_حافظہ) {
            multiboot2_حافظہ_t ثابت *s = (multiboot2_حافظہ_t *)(behind_ٹیگ + 8);
            multiboot2_حافظہ_t ثابت *e = (multiboot2_حافظہ_t *)((لفظ_t)ٹیگ + ٹیگ->ناپ);

            لكل (multiboot2_حافظہ_t ثابت *m = s; m < e; m++) {
                إذا (!m->addr) {
                    boot_state.mem_ہیٹھلا = m->ناپ;
                }

                لکھو("\tPhysical Memory Region from %llx size %llx type %u\n", m->addr, m->ناپ, m->type);
                إذا (m->addr != (uint64_t)(لفظ_t)m->addr) {
                    لکھو("\t\tPhysical memory region not addressable\n");
                    استمر;
                }

                إذا (m->type == MULTIBOOT_MMAP_USEABLE_TYPE && m->addr >= HIGHMEM_PADDR) {
                    إذا (!جوڑو_mem_p_regs((p_region_t) {
                    m->addr, m->addr + m->ناپ
                }))
                    أرجع false;
                }
            }
        } وإلا إذا (ٹیگ->type == MULTIBOOT2_ٹیگ_FB) {
            multiboot2_fb_t ثابت *fb = (multiboot2_fb_t ثابت *)behind_ٹیگ;
            لکھو("Got framebuffer info in multiboot2. Current video mode is at physical address=%llx pitch=%u resolution=%ux%u@%u type=%u\n",
                   fb->addr, fb->pitch, fb->چوڑائی, fb->اچائی, fb->bpp, fb->type);
            boot_state.fb_info = *fb;
        }

        ٹیگ = (multiboot2_ٹیگ_t ثابت *)((لفظ_t)ٹیگ + گیڑا_اتے(ٹیگ->ناپ, 3));
    }

    لکھو("Detected %d boot module(s):\n", mod_گنتی);

    إذا (mod_گنتی < 1) {
        لکھو("Expect at least one boot module (containing a userland image)\n");
        أرجع false;
    }

    أرجع true;
}

BOOT_کوڈ VISIBLE فراغ boot_sys(
    بنانشانی لما multiboot_magic,
    فراغ *mbi)
{
    bool_t نتیجہ = false;

    إذا (multiboot_magic == MULTIBOOT_MAGIC) {
        نتیجہ = try_boot_sys_mbi1(mbi);
    } وإلا إذا (multiboot_magic == MULTIBOOT2_MAGIC) {
        نتیجہ = try_boot_sys_mbi2(mbi);
    } وإلا {
        لکھو("Boot loader is not multiboot 1 or 2 compliant %lx\n", multiboot_magic);
    }

    إذا (نتیجہ) {
        نتیجہ = try_boot_sys();
    }

    إذا (!نتیجہ) {
        fail("boot_sys failed for some reason :(\n");
    }

    ARCH_گرہ_STATE(x86_KScur_رکاوٹ) = int_invalid;
    ARCH_گرہ_STATE(x86_KSPending_رکاوٹ) = int_invalid;

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    گرہ_STATE(ks_Cur_ویلا) = get_Current_ویلا();
    گرہ_STATE(ksConsumed) = 0;
#جے_انت

    ویلا_سارنی();
    activateThread();
}

