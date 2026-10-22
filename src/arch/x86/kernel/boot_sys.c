/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/apic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/cmdline.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/boot_sys.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/smp_sys.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/elf.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <smp/lock.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/acpi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/devices.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/pic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/ioapic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sel4/arch/bootinfo_types.h>

/* addresses defined in linker script */
/* need a fake array to get the pointer from the linker script */

/* start/end of CPU boot code */
ਬਾਹਰੀ ਅੱਖਰ boot_cpu_ਸ਼ੁਰੂ[1];
ਬਾਹਰੀ ਅੱਖਰ boot_cpu_ਸਿਰਾ[1];

/* start/end of boot stack */
ਬਾਹਰੀ ਅੱਖਰ boot_ਢੇਰ_bottom[1];
ਬਾਹਰੀ ਅੱਖਰ boot_ਢੇਰ_ਸਿਖਰ[1];

/* locations in kernel image */
ਬਾਹਰੀ ਅੱਖਰ ki_skim_ਸ਼ੁਰੂ[1];
ਬਾਹਰੀ ਅੱਖਰ ki_skim_ਸਿਰਾ[1];

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
/* kernel entry point */
ਬਾਹਰੀ ਅੱਖਰ ਸ਼ੁਰੂ_2[1];
#ਜੇ_ਅੰਤ

/* constants */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HIGHMEM_PADDR 0x100000

BOOT_BSS
boot_state_t boot_state;

/* global variables (not covered by abstract specification) */

BOOT_BSS
cmdline_opt_t cmdline_opt;

/* functions not modeled in abstract specification */

BOOT_ਕੋਡ ਸਥਿਰ paddr_t ਲੱਭੋ_ਲੱਦੋ_paddr(paddr_t min_paddr, ਸ਼ਬਦ_t image_ਮਾਪ)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; i < boot_state.mem_p_regs.ਗਿਣਤੀ_2; i++) {
        paddr_t ਸ਼ੁਰੂ = MAX(min_paddr, boot_state.mem_p_regs.list[i].ਸ਼ੁਰੂ);
        paddr_t ਸਿਰਾ = boot_state.mem_p_regs.list[i].ਸਿਰਾ;
        ਸ਼ਬਦ_t region_ਮਾਪ = ਸਿਰਾ - ਸ਼ੁਰੂ;

        ਜੇ (region_ਮਾਪ >= image_ਮਾਪ) {
            ਵਾਪਸ ਸ਼ੁਰੂ;
        }
    }

    ਵਾਪਸ 0;
}

BOOT_ਕੋਡ ਸਥਿਰ paddr_t ਲੱਦੋ_boot_module(ਸ਼ਬਦ_t boot_module_ਸ਼ੁਰੂ, ਸ਼ਬਦ_t boot_module_ਸਿਰਾ, paddr_t ਲੱਦੋ_paddr)
{
    v_region_t v_reg;
    ਸ਼ਬਦ_t ਇੰਦਰਾਜ;
    Elf_ਸਿਰਲੇਖ_t *elf_file = (Elf_ਸਿਰਲੇਖ_t *)boot_module_ਸ਼ੁਰੂ;

    ਜੇ (!elf_checkFile(elf_file, boot_module_ਸਿਰਾ - boot_module_ਸ਼ੁਰੂ)) {
        ਲਿਖੋ("Boot module does not contain a valid ELF image\n");
        ਵਾਪਸ 0;
    }

    v_reg = elf_get_ਸਮ੍ਰਿਤੀ_Bounds(elf_file);
    ਇੰਦਰਾਜ = elf_file->e_ਇੰਦਰਾਜ;

    ਜੇ (v_reg.ਸਿਰਾ == 0) {
        ਲਿਖੋ("ELF image in boot module does not contain any segments\n");
        ਵਾਪਸ 0;
    }
    v_reg.ਸਿਰਾ = ਗੇੜਾ_ਉੱਪਰ(v_reg.ਸਿਰਾ, ਪੰਨਾ_ਬਿੱਟ);

    ਲਿਖੋ("size=0x%lx v_entry=%p v_start=%p v_end=%p ",
           v_reg.ਸਿਰਾ - v_reg.ਸ਼ੁਰੂ,
           (ਖਾਲੀ *)ਇੰਦਰਾਜ,
           (ਖਾਲੀ *)v_reg.ਸ਼ੁਰੂ,
           (ਖਾਲੀ *)v_reg.ਸਿਰਾ
          );

    ਜੇ (!IS_ALIGNED(v_reg.ਸ਼ੁਰੂ, ਪੰਨਾ_ਬਿੱਟ)) {
        ਲਿਖੋ("Userland image virtual start address must be 4KB-aligned\n");
        ਵਾਪਸ 0;
    }
    ਜੇ (v_reg.ਸਿਰਾ + 2 * ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ) > USER_ਸਿਖਰ) {
        /* for IPC buffer frame and bootinfo frame, need 2*4K of additional userland virtual memory */
        ਲਿਖੋ("Userland image virtual end address too high\n");
        ਵਾਪਸ 0;
    }
    ਜੇ ((ਇੰਦਰਾਜ < v_reg.ਸ਼ੁਰੂ) || (ਇੰਦਰਾਜ >= v_reg.ਸਿਰਾ)) {
        ਲਿਖੋ("Userland image entry point does not lie within userland image\n");
        ਵਾਪਸ 0;
    }

    ਲੱਦੋ_paddr = ਲੱਭੋ_ਲੱਦੋ_paddr(ਲੱਦੋ_paddr, v_reg.ਸਿਰਾ - v_reg.ਸ਼ੁਰੂ);
    ਪ੍ਰਤਿਜ੍ਞਾ(ਲੱਦੋ_paddr);

    /* fill ui_info struct */
    boot_state.ui_info.pv_ਵਿੱਥ = ਲੱਦੋ_paddr - v_reg.ਸ਼ੁਰੂ;
    boot_state.ui_info.p_reg.ਸ਼ੁਰੂ = ਲੱਦੋ_paddr;
    ਲੱਦੋ_paddr += v_reg.ਸਿਰਾ - v_reg.ਸ਼ੁਰੂ;
    boot_state.ui_info.p_reg.ਸਿਰਾ = ਲੱਦੋ_paddr;
    boot_state.ui_info.v_ਇੰਦਰਾਜ = ਇੰਦਰਾਜ;

    ਲਿਖੋ("p_start=0x%lx p_end=0x%lx\n",
           boot_state.ui_info.p_reg.ਸ਼ੁਰੂ,
           boot_state.ui_info.p_reg.ਸਿਰਾ
          );

    /* initialise all initial userland memory and load potentially sparse ELF image */
    memzero(
        (ਖਾਲੀ *)boot_state.ui_info.p_reg.ਸ਼ੁਰੂ,
        boot_state.ui_info.p_reg.ਸਿਰਾ - boot_state.ui_info.p_reg.ਸ਼ੁਰੂ
    );
    elf_ਲੱਦੋ(elf_file, boot_state.ui_info.pv_ਵਿੱਥ);

    ਵਾਪਸ ਲੱਦੋ_paddr;
}

ਸਥਿਰ BOOT_ਕੋਡ bool_t try_boot_sys_ਗੰਢ(cpu_id_t cpu_id)
{
    ਜੇ (!map_kernel_ਖਿੜਕੀ(
            boot_state.ਅੰਕ_ioapic,
            boot_state.ioapic_paddr,
            boot_state.ਅੰਕ_drhu,
            boot_state.drhu_list
        )) {
        ਵਾਪਸ false;
    }
    ਸੈੱਟ_Current_VSpace_ਜੜ੍ਹ(kpptr_ਮੰਜ਼ਿਲ_paddr(X86_KERNEL_VSPACE_ਜੜ੍ਹ), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm ਅਸਥਿਰ("" ::: "memory");

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_SKIM_ਖਿੜਕੀ
    ਜੇ (!map_skim_ਖਿੜਕੀ((vptr_t)ki_skim_ਸ਼ੁਰੂ, (vptr_t)ki_skim_ਸਿਰਾ)) {
        ਵਾਪਸ false;
    }
#ਜੇ_ਅੰਤ

    /* initialise the CPU */
    ਜੇ (!init_cpu(config_ਸੈੱਟ(CONFIG_IRQ_IOAPIC) ? 1 : 0)) {
        ਵਾਪਸ false;
    }

    /* initialise NDKS and kernel heap */
    ਜੇ (!init_sys_state(
            cpu_id,
            &boot_state.mem_p_regs,
            boot_state.ui_info,
            /* parameters below not modelled in abstract specification */
            boot_state.ਅੰਕ_drhu,
            boot_state.drhu_list,
            &boot_state.rmrr_list,
            &boot_state.acpi_rsdp,
            &boot_state.vbe_info,
            &boot_state.mb_mmap_info,
            &boot_state.fb_info
        )) {
        ਵਾਪਸ false;
    }

    ਵਾਪਸ true;
}

ਸਥਿਰ BOOT_ਕੋਡ bool_t ਜੋੜੋ_mem_p_regs(p_region_t reg)
{
    ਜੇ (reg.ਸ਼ੁਰੂ == reg.ਸਿਰਾ) {
        // Return true here if asked to add an empty region.
        // Some of the callers round down the end address to
        ਵਾਪਸ true;
    }
    ਜੇ (reg.ਸਿਰਾ > PADDR_ਸਿਖਰ && reg.ਸ਼ੁਰੂ >= PADDR_ਸਿਖਰ) {
        /* Return true here as it's not an error for there to exist memory outside the kernel window,
         * we're just going to ignore it and leave it to be given out as device memory */
        ਵਾਪਸ true;
    }
    ਜੇ (boot_state.mem_p_regs.ਗਿਣਤੀ_2 == MAX_ਅੰਕ_FREEMEM_REG) {
        ਲਿਖੋ("Dropping memory region 0x%lx-0x%lx, try increasing MAX_NUM_FREEMEM_REG\n", reg.ਸ਼ੁਰੂ, reg.ਸਿਰਾ);
        ਵਾਪਸ false;
    }
    ਜੇ (reg.ਸਿਰਾ > PADDR_ਸਿਖਰ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(reg.ਸ਼ੁਰੂ < PADDR_ਸਿਖਰ);
        /* Clamp a region to the top of the kernel window if it extends beyond */
        reg.ਸਿਰਾ = PADDR_ਸਿਖਰ;
    }
    ਲਿਖੋ("Adding physical memory region 0x%lx-0x%lx\n", reg.ਸ਼ੁਰੂ, reg.ਸਿਰਾ);
    boot_state.mem_p_regs.list[boot_state.mem_p_regs.ਗਿਣਤੀ_2] = reg;
    boot_state.mem_p_regs.ਗਿਣਤੀ_2++;
    ਵਾਪਸ true;
}

/*
 * the code relies that the GRUB provides correct information
 * about the actual physical memory regions.
 */
ਸਥਿਰ BOOT_ਕੋਡ bool_t parse_mem_map(uint32_t mmap_ਲੰਮਾਈ, uint32_t mmap_addr)
{
    multiboot_mmap_t *mmap = (multiboot_mmap_t *)((ਸ਼ਬਦ_t)mmap_addr);
    ਲਿਖੋ("Parsing GRUB physical memory map\n");

    ਜਦੋਂ ((ਸ਼ਬਦ_t)mmap < (ਸ਼ਬਦ_t)(mmap_addr + mmap_ਲੰਮਾਈ)) {
        uint64_t mem_ਸ਼ੁਰੂ = mmap->ਅਧਾਰ_addr;
        uint64_t mem_ਲੰਮਾਈ = mmap->ਲੰਮਾਈ;
        uint32_t type = mmap->type;
        ਜੇ (mem_ਸ਼ੁਰੂ != (uint64_t)(ਸ਼ਬਦ_t)mem_ਸ਼ੁਰੂ) {
            ਲਿਖੋ("\tPhysical memory region not addressable\n");
        } ਨਹੀਂਤਾਂ {
            ਲਿਖੋ("\tPhysical Memory Region from %lx size %lx type %d\n", (ਲੰਮਾ)mem_ਸ਼ੁਰੂ, (ਲੰਮਾ)mem_ਲੰਮਾਈ, type);
            ਜੇ (type == MULTIBOOT_MMAP_USEABLE_TYPE && mem_ਸ਼ੁਰੂ >= HIGHMEM_PADDR && mem_ਲੰਮਾਈ >= ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ)) {

                ਜੇ (!ਜੋੜੋ_mem_p_regs((p_region_t) {
                ਗੇੜਾ_ਉੱਪਰ(mem_ਸ਼ੁਰੂ, ਪੰਨਾ_ਬਿੱਟ), ਗੇੜਾ_ਥੱਲੇ(mem_ਸ਼ੁਰੂ + mem_ਲੰਮਾਈ, ਪੰਨਾ_ਬਿੱਟ),
                })) {
                    ਵਾਪਸ false;
                }
            }
        }
        mmap++;
    }
    ਵਾਪਸ true;
}

ਸਥਿਰ BOOT_ਕੋਡ bool_t is_compiled_for_microarchitecture(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t microarch_generation = 0;
    x86_cpu_ਪਛਾਣ_ਤੱਤ_t *model_info = x86_cpuid_get_model_info();

    ਜੇ (config_ਸੈੱਟ(CONFIG_ARCH_X86_SKYLAKE)) {
        microarch_generation = 7;
    } ਨਹੀਂਤਾਂ ਜੇ (config_ਸੈੱਟ(CONFIG_ARCH_X86_BROADWELL)) {
        microarch_generation = 6;
    } ਨਹੀਂਤਾਂ ਜੇ (config_ਸੈੱਟ(CONFIG_ARCH_X86_HASWELL)) {
        microarch_generation = 5;
    } ਨਹੀਂਤਾਂ ਜੇ (config_ਸੈੱਟ(CONFIG_ARCH_X86_IVY)) {
        microarch_generation = 4;
    } ਨਹੀਂਤਾਂ ਜੇ (config_ਸੈੱਟ(CONFIG_ARCH_X86_SANDY)) {
        microarch_generation = 3;
    } ਨਹੀਂਤਾਂ ਜੇ (config_ਸੈੱਟ(CONFIG_ARCH_X86_WESTMERE)) {
        microarch_generation = 2;
    } ਨਹੀਂਤਾਂ ਜੇ (config_ਸੈੱਟ(CONFIG_ARCH_X86_NEHALEM)) {
        microarch_generation = 1;
    }

    ਚੋਣ (model_info->model) {
    ਹਾਲਤ SKYLAKE_1_MODEL_ID:
    ਹਾਲਤ SKYLAKE_2_MODEL_ID:
    ਹਾਲਤ SKYLAKE_X_MODEL_ID:
        ਜੇ (microarch_generation > 7) {
            ਵਾਪਸ false;
        }
        ਰੋਕੋ;

    ਹਾਲਤ BROADWELL_1_MODEL_ID:
    ਹਾਲਤ BROADWELL_2_MODEL_ID:
    ਹਾਲਤ BROADWELL_3_MODEL_ID:
    ਹਾਲਤ BROADWELL_4_MODEL_ID:
    ਹਾਲਤ BROADWELL_5_MODEL_ID:
        ਜੇ (microarch_generation > 6) {
            ਵਾਪਸ false;
        }
        ਰੋਕੋ;

    ਹਾਲਤ HASWELL_1_MODEL_ID:
    ਹਾਲਤ HASWELL_2_MODEL_ID:
    ਹਾਲਤ HASWELL_3_MODEL_ID:
    ਹਾਲਤ HASWELL_4_MODEL_ID:
        ਜੇ (microarch_generation > 5) {
            ਵਾਪਸ false;
        }
        ਰੋਕੋ;

    ਹਾਲਤ IVY_BRIDGE_1_MODEL_ID:
    ਹਾਲਤ IVY_BRIDGE_2_MODEL_ID:
    ਹਾਲਤ IVY_BRIDGE_3_MODEL_ID:
        ਜੇ (microarch_generation > 4) {
            ਵਾਪਸ false;
        }
        ਰੋਕੋ;

    ਹਾਲਤ SANDY_BRIDGE_1_MODEL_ID:
    ਹਾਲਤ SANDY_BRIDGE_2_MODEL_ID:
        ਜੇ (microarch_generation > 3) {
            ਵਾਪਸ false;
        }
        ਰੋਕੋ;

    ਹਾਲਤ WESTMERE_1_MODEL_ID:
    ਹਾਲਤ WESTMERE_2_MODEL_ID:
    ਹਾਲਤ WESTMERE_3_MODEL_ID:
        ਜੇ (microarch_generation > 2) {
            ਵਾਪਸ false;
        }
        ਰੋਕੋ;

    ਹਾਲਤ NEHALEM_1_MODEL_ID:
    ਹਾਲਤ NEHALEM_2_MODEL_ID:
    ਹਾਲਤ NEHALEM_3_MODEL_ID:
        ਜੇ (microarch_generation > 1) {
            ਵਾਪਸ false;
        }
        ਰੋਕੋ;

    ਮੂਲ:
        ਜੇ (!config_ਸੈੱਟ(CONFIG_ARCH_X86_GENERIC)) {
            ਵਾਪਸ false;
        }
    }

    ਵਾਪਸ true;
}

ਸਥਿਰ BOOT_ਕੋਡ bool_t try_boot_sys(ਖਾਲੀ)
{
    paddr_t mods_ਸਿਰਾ_paddr = boot_state.mods_ਸਿਰਾ_paddr;
    p_region_t ui_p_regs;
    paddr_t ਲੱਦੋ_paddr;

    boot_state.ki_p_reg = get_p_reg_kernel_img();

    ਜੇ (!x86_cpuid_initialize()) {
        ਲਿਖੋ("Warning: Your x86 CPU has an unsupported vendor, '%s'.\n"
               "\tYour setup may not be able to competently run seL4 as "
               "intended.\n"
               "\tCurrently supported x86 vendors are AMD and Intel.\n",
               x86_cpuid_get_ਪਛਾਣ_ਤੱਤ()->vendor_string);
    }

    ਜੇ (!is_compiled_for_microarchitecture()) {
        ਲਿਖੋ("Warning: Your kernel was not compiled for the current microarchitecture.\n");
    }

    cpuid_007h_edx_t edx;
    edx.ਸ਼ਬਦ[0] = x86_cpuid_edx(0x7, 0);
    /* see if we can definitively say whether or not we need the skim window by
     * checking whether the CPU is vulnerable to rogue data cache loads (rdcl) */
    ਜੇ (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.ਸ਼ਬਦ[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        ਜੇ (ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && config_ਸੈੱਟ(CONFIG_KERNEL_SKIM_ਖਿੜਕੀ)) {
            ਲਿਖੋ("CPU reports not vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is enabled. Performance is needlessly being impacted, consider disabling.\n");
        } ਨਹੀਂਤਾਂ ਜੇ (!ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && !config_ਸੈੱਟ(CONFIG_KERNEL_SKIM_ਖਿੜਕੀ)) {
            ਲਿਖੋ("CPU reports vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is *not* enabled. Please re-build with SKIM window enabled.");
            ਵਾਪਸ false;
        }
    } ਨਹੀਂਤਾਂ {
        /* hardware doesn't tell us directly so guess based on CPU vendor */
        ਜੇ (config_ਸੈੱਟ(CONFIG_KERNEL_SKIM_ਖਿੜਕੀ) && x86_cpuid_get_ਪਛਾਣ_ਤੱਤ()->vendor == X86_VENDOR_AMD) {
            ਲਿਖੋ("SKIM window for mitigating Meltdown (https://www.meltdownattack.com) "
                   "not necessary for AMD and performance is being needlessly affected, "
                   "consider disabling\n");
        }
        ਜੇ (!config_ਸੈੱਟ(CONFIG_KERNEL_SKIM_ਖਿੜਕੀ) && x86_cpuid_get_ਪਛਾਣ_ਤੱਤ()->vendor == X86_VENDOR_INTEL) {
            ਲਿਖੋ("***WARNING*** SKIM window not enabled, this machine is probably vulnerable "
                   "to Meltdown (https://www.meltdownattack.com), consider enabling\n");
        }
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    /* copy boot code for APs to lower memory to run in real mode */
    ਜੇ (!copy_boot_ਕੋਡ_aps(boot_state.mem_ਹੇਠਲਾ)) {
        ਵਾਪਸ false;
    }
    /* Initialize any kernel TLS */
    ਬਹੁਲਕ_init_tls(0);
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

    ਲਿਖੋ("Kernel loaded to: start=0x%lx end=0x%lx size=0x%lx entry=0x%lx\n",
           boot_state.ki_p_reg.ਸ਼ੁਰੂ,
           boot_state.ki_p_reg.ਸਿਰਾ,
           boot_state.ki_p_reg.ਸਿਰਾ - boot_state.ki_p_reg.ਸ਼ੁਰੂ,
           (paddr_t)ਸ਼ੁਰੂ_2
          );

    /* remapping legacy IRQs to their correct vectors */
    pic_remap_irqs(IRQ_INT_ਵਿੱਥ);
    ਜੇ (config_ਸੈੱਟ(CONFIG_IRQ_IOAPIC)) {
        /* Disable the PIC so that it does not generate any interrupts. We need to
         * do this *before* we initialize the apic */
        pic_disable();
    }

    /* validate the ACPI table */
    ਜੇ (!acpi_validate_rsdp(&boot_state.acpi_rsdp)) {
        ਵਾਪਸ false;
    }

    /* check if kernel configuration matches platform requirements */
    ਜੇ (!acpi_fadt_ਜਾਂਚ(&boot_state.acpi_rsdp)) {
        ਵਾਪਸ false;
    }

    ਜੇ (!config_ਸੈੱਟ(CONFIG_IOMMU) || cmdline_opt.disable_iommu) {
        boot_state.ਅੰਕ_drhu = 0;
    } ਨਹੀਂਤਾਂ {
        /* query available IOMMUs from ACPI */
        acpi_dmar_ਜਾਂਚ(
            &boot_state.acpi_rsdp,
            boot_state.drhu_list,
            &boot_state.ਅੰਕ_drhu,
            MAX_ਅੰਕ_DRHU,
            &boot_state.rmrr_list
        );
    }

    /* query available CPUs from ACPI */
    boot_state.ਅੰਕ_cpus = acpi_madt_ਜਾਂਚ(&boot_state.acpi_rsdp, boot_state.cpus, &boot_state.ਅੰਕ_ioapic,
                                         boot_state.ioapic_paddr);
    ਜੇ (boot_state.ਅੰਕ_cpus == 0) {
        ਲਿਖੋ("No CPUs detected\n");
        ਵਾਪਸ false;
    }

    ਜੇ (config_ਸੈੱਟ(CONFIG_IRQ_IOAPIC)) {
        ਜੇ (boot_state.ਅੰਕ_ioapic == 0) {
            ਲਿਖੋ("No IOAPICs detected\n");
            ਵਾਪਸ false;
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ (boot_state.ਅੰਕ_ioapic > 0) {
            ਲਿਖੋ("Detected %d IOAPICs, but configured to use PIC instead\n", boot_state.ਅੰਕ_ioapic);
        }
    }

    mods_ਸਿਰਾ_paddr = ਗੇੜਾ_ਉੱਪਰ(mods_ਸਿਰਾ_paddr, ਪੰਨਾ_ਬਿੱਟ);
    ਪ੍ਰਤਿਜ੍ਞਾ(mods_ਸਿਰਾ_paddr > boot_state.ki_p_reg.ਸਿਰਾ);

    ਲਿਖੋ("ELF-loading userland images from boot modules:\n");
    ਲੱਦੋ_paddr = mods_ਸਿਰਾ_paddr;

    ਲੱਦੋ_paddr = ਲੱਦੋ_boot_module(boot_state.boot_module_ਸ਼ੁਰੂ, boot_state.boot_module_ਸਿਰਾ, ਲੱਦੋ_paddr);
    ਜੇ (!ਲੱਦੋ_paddr) {
        ਵਾਪਸ false;
    }

    /* calculate final location of userland images */
    ui_p_regs.ਸ਼ੁਰੂ = boot_state.ki_p_reg.ਸਿਰਾ;
    ui_p_regs.ਸਿਰਾ = ui_p_regs.ਸ਼ੁਰੂ + ਲੱਦੋ_paddr - mods_ਸਿਰਾ_paddr;

    ਲਿਖੋ(
        "Moving loaded userland images to final location: from=0x%lx to=0x%lx size=0x%lx\n",
        mods_ਸਿਰਾ_paddr,
        ui_p_regs.ਸ਼ੁਰੂ,
        ui_p_regs.ਸਿਰਾ - ui_p_regs.ਸ਼ੁਰੂ
    );
    ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਖਾਲੀ *)ui_p_regs.ਸ਼ੁਰੂ, (ਖਾਲੀ *)mods_ਸਿਰਾ_paddr, ui_p_regs.ਸਿਰਾ - ui_p_regs.ਸ਼ੁਰੂ);

    /* adjust p_reg and pv_offset to final load address */
    boot_state.ui_info.p_reg.ਸ਼ੁਰੂ -= mods_ਸਿਰਾ_paddr - ui_p_regs.ਸ਼ੁਰੂ;
    boot_state.ui_info.p_reg.ਸਿਰਾ   -= mods_ਸਿਰਾ_paddr - ui_p_regs.ਸ਼ੁਰੂ;
    boot_state.ui_info.pv_ਵਿੱਥ   -= mods_ਸਿਰਾ_paddr - ui_p_regs.ਸ਼ੁਰੂ;

    /* ==== following code corresponds to abstract specification after "select" ==== */

    ਜੇ (!plat_ਜੋੜੋ_Devices()) {
        ਵਾਪਸ false;
    }

    /* Total number of cores we intend to boot */
    ks_ਅੰਕ_CPUs = boot_state.ਅੰਕ_cpus;

    ਲਿਖੋ("Starting node #0 with APIC ID %lu\n", boot_state.cpus[0]);
    ਜੇ (!try_boot_sys_ਗੰਢ(boot_state.cpus[0])) {
        ਵਾਪਸ false;
    }

    ਜੇ (config_ਸੈੱਟ(CONFIG_IRQ_IOAPIC)) {
        ioapic_init(1, boot_state.cpus, boot_state.ਅੰਕ_ioapic);
    }

    /* initialize BKL before booting up APs */
    SMP_COND_STATEMENT(clh_ਤਾਲਾ_init());
    SMP_COND_STATEMENT(ਸ਼ੁਰੂ_boot_aps());

    /* grab BKL before leaving the kernel */
    ਗੰਢ_ਤਾਲਾ_SYS;

    ਲਿਖੋ("Booting all finished, dropped to user space\n");

    ਵਾਪਸ true;
}

ਸਥਿਰ BOOT_ਕੋਡ bool_t try_boot_sys_mbi1(
    multiboot_info_t *mbi
)
{
    ਸ਼ਬਦ_t i;
    multiboot_module_t *modules = (multiboot_module_t *)(ਸ਼ਬਦ_t)mbi->part1.mod_list;

    cmdline_parse((ਅਚਲ ਅੱਖਰ *)(ਸ਼ਬਦ_t)mbi->part1.cmdline, &cmdline_opt);

    ਜੇ ((mbi->part1.flags & MULTIBOOT_INFO_MEM_FLAG) == 0) {
        ਲਿਖੋ("Boot loader did not provide information about physical memory size\n");
        ਵਾਪਸ false;
    }

    ਜੇ (!(mbi->part1.flags & MULTIBOOT_INFO_MODS_FLAG)) {
        ਲਿਖੋ("Boot loader did not provide information about boot modules\n");
        ਵਾਪਸ false;
    }

    ਲਿਖੋ("Detected %d boot module(s):\n", mbi->part1.mod_ਗਿਣਤੀ);

    ਜੇ (mbi->part1.mod_ਗਿਣਤੀ < 1) {
        ਲਿਖੋ("Expect at least one boot module (containing a userland image)\n");
        ਵਾਪਸ false;
    }

    ਲਈ (i = 0; i < mbi->part1.mod_ਗਿਣਤੀ; i++) {
        ਲਿਖੋ(
            "  module #%ld: start=0x%x end=0x%x size=0x%x name='%s'\n",
            i,
            modules[i].ਸ਼ੁਰੂ,
            modules[i].ਸਿਰਾ,
            modules[i].ਸਿਰਾ - modules[i].ਸ਼ੁਰੂ,
            (ਅੱਖਰ *)(ਲੰਮਾ)modules[i].name
        );
        ਜੇ ((sword_t)(modules[i].ਸਿਰਾ - modules[i].ਸ਼ੁਰੂ) <= 0) {
            ਲਿਖੋ("Invalid boot module size! Possible cause: boot module file not found by QEMU\n");
            ਵਾਪਸ false;
        }
        ਜੇ (boot_state.mods_ਸਿਰਾ_paddr < modules[i].ਸਿਰਾ) {
            boot_state.mods_ਸਿਰਾ_paddr = modules[i].ਸਿਰਾ;
        }
    }

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.ਗਿਣਤੀ_2 = 0;
    ਜੇ (mbi->part1.flags & MULTIBOOT_INFO_MMAP_FLAG) {
        ਜੇ (!parse_mem_map(mbi->part2.mmap_ਲੰਮਾਈ, mbi->part2.mmap_addr)) {
            ਵਾਪਸ false;
        }
        uint32_t multiboot_mmap_ਲੰਮਾਈ = mbi->part2.mmap_ਲੰਮਾਈ;
        ਜੇ (multiboot_mmap_ਲੰਮਾਈ > (SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * ਆਕਾਰ(seL4_X86_mb_mmap_t))) {
            ਲਿਖੋ("Warning: Multiboot has reported more memory map entries, %zd, "
                   "than the max amount that will be passed in the bootinfo, %d. "
                   "These extra regions will still be turned into untyped caps.",
                   multiboot_mmap_ਲੰਮਾਈ / ਆਕਾਰ(seL4_X86_mb_mmap_t), SEL4_MULTIBOOT_MAX_MMAP_ENTRIES);
            multiboot_mmap_ਲੰਮਾਈ = SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * ਆਕਾਰ(seL4_X86_mb_mmap_t);
        }
        ਸਮ੍ਰਿਤੀ_ਨਕਲ(&boot_state.mb_mmap_info.mmap, (ਖਾਲੀ *)(ਸ਼ਬਦ_t)mbi->part2.mmap_addr, multiboot_mmap_ਲੰਮਾਈ);
        boot_state.mb_mmap_info.mmap_ਲੰਮਾਈ = multiboot_mmap_ਲੰਮਾਈ;
    } ਨਹੀਂਤਾਂ {
        /* calculate memory the old way */
        p_region_t avail;
        avail.ਸ਼ੁਰੂ = HIGHMEM_PADDR;
        avail.ਸਿਰਾ = ਗੇੜਾ_ਥੱਲੇ(avail.ਸ਼ੁਰੂ + (mbi->part1.mem_ਉੱਤਲਾ << 10), ਪੰਨਾ_ਬਿੱਟ);
        ਜੇ (!ਜੋੜੋ_mem_p_regs(avail)) {
            ਵਾਪਸ false;
        }
    }

    /* copy VESA information from multiboot header */
    ਜੇ ((mbi->part1.flags & MULTIBOOT_INFO_GRAPHICS_FLAG) == 0) {
        boot_state.vbe_info.vbe_ਬਹੁਲਕ = -1;
        ਲਿਖੋ("Multiboot gave us no video information\n");
    } ਨਹੀਂਤਾਂ {
        boot_state.vbe_info.vbe_Info_ਖੰਡ = *(se_L4_VBEInfo_ਖੰਡ_t *)(se_L4_ਸ਼ਬਦ)mbi->part2.vbe_control_info;
        boot_state.vbe_info.vbe_ਬਹੁਲਕ_Info_ਖੰਡ = *(se_L4_VBEMode_Info_ਖੰਡ_t *)(se_L4_ਸ਼ਬਦ)mbi->part2.vbe_ਬਹੁਲਕ_info;
        boot_state.vbe_info.vbe_ਬਹੁਲਕ = mbi->part2.vbe_ਬਹੁਲਕ_2;
        ਲਿਖੋ("Got VBE info in multiboot. Current video mode is %d\n", mbi->part2.vbe_ਬਹੁਲਕ_2);
        boot_state.vbe_info.vbeInterfaceSeg = mbi->part2.vbe_interface_seg;
        boot_state.vbe_info.vbeInterfaceOff = mbi->part2.vbe_interface_off;
        boot_state.vbe_info.vbeInterfaceLen = mbi->part2.vbe_interface_len;
    }

    boot_state.mem_ਹੇਠਲਾ = mbi->part1.mem_ਹੇਠਲਾ;
    boot_state.boot_module_ਸ਼ੁਰੂ = modules->ਸ਼ੁਰੂ;
    boot_state.boot_module_ਸਿਰਾ = modules->ਸਿਰਾ;

    /* Initialize ACPI */
    ਜੇ (!acpi_init(&boot_state.acpi_rsdp)) {
        ਵਾਪਸ false;
    }

    ਵਾਪਸ true;
}

ਸਥਿਰ BOOT_ਕੋਡ bool_t try_boot_sys_mbi2(
    multiboot2_ਸਿਰਲੇਖ_t *mbi2
)
{
    ਪੂਰਨ mod_ਗਿਣਤੀ                  = 0;
    multiboot2_ਟੈਗ_t ਅਚਲ *ਟੈਗ   = (multiboot2_ਟੈਗ_t *)(mbi2 + 1);
    multiboot2_ਟੈਗ_t ਅਚਲ *ਟੈਗ_e = (multiboot2_ਟੈਗ_t *)((ਸ਼ਬਦ_t)mbi2 + mbi2->ਕੁੱਲ_ਮਾਪ);

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.ਗਿਣਤੀ_2 = 0;
    boot_state.mb_mmap_info.mmap_ਲੰਮਾਈ = 0;
    boot_state.vbe_info.vbe_ਬਹੁਲਕ = -1;

    ਜਦੋਂ (ਟੈਗ < ਟੈਗ_e && ਟੈਗ->type != MULTIBOOT2_ਟੈਗ_ਸਿਰਾ) {
        ਸ਼ਬਦ_t ਅਚਲ behind_ਟੈਗ = (ਸ਼ਬਦ_t)ਟੈਗ + ਆਕਾਰ(*ਟੈਗ);

        ਜੇ (ਟੈਗ->type == MULTIBOOT2_ਟੈਗ_CMDLINE) {
            ਅੱਖਰ ਅਚਲ *ਅਚਲ cmdline = (ਅੱਖਰ ਅਚਲ * ਅਚਲ)(behind_ਟੈਗ);
            cmdline_parse(cmdline, &cmdline_opt);
        } ਨਹੀਂਤਾਂ ਜੇ (ਟੈਗ->type == MULTIBOOT2_ਟੈਗ_ACPI_1) {
            ਜੇ (ACPI_V1_ਮਾਪ == ਟੈਗ->ਮਾਪ - ਆਕਾਰ(*ਟੈਗ)) {
                ਸਮ੍ਰਿਤੀ_ਨਕਲ(&boot_state.acpi_rsdp, (ਖਾਲੀ *)behind_ਟੈਗ, ਟੈਗ->ਮਾਪ - ਆਕਾਰ(*ਟੈਗ));
            }
        } ਨਹੀਂਤਾਂ ਜੇ (ਟੈਗ->type == MULTIBOOT2_ਟੈਗ_ACPI_2) {
            ਜੇ (ਆਕਾਰ(boot_state.acpi_rsdp) == ਟੈਗ->ਮਾਪ - ਆਕਾਰ(*ਟੈਗ)) {
                ਸਮ੍ਰਿਤੀ_ਨਕਲ(&boot_state.acpi_rsdp, (ਖਾਲੀ *)behind_ਟੈਗ, ਆਕਾਰ(boot_state.acpi_rsdp));
            }
        } ਨਹੀਂਤਾਂ ਜੇ (ਟੈਗ->type == MULTIBOOT2_ਟੈਗ_MODULE) {
            multiboot2_module_t ਅਚਲ *module = (multiboot2_module_t ਅਚਲ *)behind_ਟੈਗ;
            ਲਿਖੋ(
                "  module #%d: start=0x%x end=0x%x size=0x%x name='%s'\n",
                mod_ਗਿਣਤੀ,
                module->ਸ਼ੁਰੂ,
                module->ਸਿਰਾ,
                module->ਸਿਰਾ - module->ਸ਼ੁਰੂ,
                module->string
            );

            ਜੇ (mod_ਗਿਣਤੀ == 0) {
                boot_state.boot_module_ਸ਼ੁਰੂ = module->ਸ਼ੁਰੂ;
                boot_state.boot_module_ਸਿਰਾ = module->ਸਿਰਾ;
            }

            mod_ਗਿਣਤੀ ++;
            ਜੇ ((sword_t)(module->ਸਿਰਾ - module->ਸ਼ੁਰੂ) <= 0) {
                ਲਿਖੋ("Invalid boot module size! Possible cause: boot module file not found\n");
                ਵਾਪਸ false;
            }
            ਜੇ (boot_state.mods_ਸਿਰਾ_paddr < module->ਸਿਰਾ) {
                boot_state.mods_ਸਿਰਾ_paddr = module->ਸਿਰਾ;
            }
        } ਨਹੀਂਤਾਂ ਜੇ (ਟੈਗ->type == MULTIBOOT2_ਟੈਗ_ਸਮ੍ਰਿਤੀ) {
            multiboot2_ਸਮ੍ਰਿਤੀ_t ਅਚਲ *s = (multiboot2_ਸਮ੍ਰਿਤੀ_t *)(behind_ਟੈਗ + 8);
            multiboot2_ਸਮ੍ਰਿਤੀ_t ਅਚਲ *e = (multiboot2_ਸਮ੍ਰਿਤੀ_t *)((ਸ਼ਬਦ_t)ਟੈਗ + ਟੈਗ->ਮਾਪ);

            ਲਈ (multiboot2_ਸਮ੍ਰਿਤੀ_t ਅਚਲ *m = s; m < e; m++) {
                ਜੇ (!m->addr) {
                    boot_state.mem_ਹੇਠਲਾ = m->ਮਾਪ;
                }

                ਲਿਖੋ("\tPhysical Memory Region from %llx size %llx type %u\n", m->addr, m->ਮਾਪ, m->type);
                ਜੇ (m->addr != (uint64_t)(ਸ਼ਬਦ_t)m->addr) {
                    ਲਿਖੋ("\t\tPhysical memory region not addressable\n");
                    ਜਾਰੀ;
                }

                ਜੇ (m->type == MULTIBOOT_MMAP_USEABLE_TYPE && m->addr >= HIGHMEM_PADDR) {
                    ਜੇ (!ਜੋੜੋ_mem_p_regs((p_region_t) {
                    m->addr, m->addr + m->ਮਾਪ
                }))
                    ਵਾਪਸ false;
                }
            }
        } ਨਹੀਂਤਾਂ ਜੇ (ਟੈਗ->type == MULTIBOOT2_ਟੈਗ_FB) {
            multiboot2_fb_t ਅਚਲ *fb = (multiboot2_fb_t ਅਚਲ *)behind_ਟੈਗ;
            ਲਿਖੋ("Got framebuffer info in multiboot2. Current video mode is at physical address=%llx pitch=%u resolution=%ux%u@%u type=%u\n",
                   fb->addr, fb->pitch, fb->ਚੌੜਾਈ, fb->ਉਚਾਈ, fb->bpp, fb->type);
            boot_state.fb_info = *fb;
        }

        ਟੈਗ = (multiboot2_ਟੈਗ_t ਅਚਲ *)((ਸ਼ਬਦ_t)ਟੈਗ + ਗੇੜਾ_ਉੱਪਰ(ਟੈਗ->ਮਾਪ, 3));
    }

    ਲਿਖੋ("Detected %d boot module(s):\n", mod_ਗਿਣਤੀ);

    ਜੇ (mod_ਗਿਣਤੀ < 1) {
        ਲਿਖੋ("Expect at least one boot module (containing a userland image)\n");
        ਵਾਪਸ false;
    }

    ਵਾਪਸ true;
}

BOOT_ਕੋਡ VISIBLE ਖਾਲੀ boot_sys(
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ multiboot_magic,
    ਖਾਲੀ *mbi)
{
    bool_t ਨਤੀਜਾ = false;

    ਜੇ (multiboot_magic == MULTIBOOT_MAGIC) {
        ਨਤੀਜਾ = try_boot_sys_mbi1(mbi);
    } ਨਹੀਂਤਾਂ ਜੇ (multiboot_magic == MULTIBOOT2_MAGIC) {
        ਨਤੀਜਾ = try_boot_sys_mbi2(mbi);
    } ਨਹੀਂਤਾਂ {
        ਲਿਖੋ("Boot loader is not multiboot 1 or 2 compliant %lx\n", multiboot_magic);
    }

    ਜੇ (ਨਤੀਜਾ) {
        ਨਤੀਜਾ = try_boot_sys();
    }

    ਜੇ (!ਨਤੀਜਾ) {
        fail("boot_sys failed for some reason :(\n");
    }

    ARCH_ਗੰਢ_STATE(x86_KScur_ਰੁਕਾਵਟ) = int_invalid;
    ARCH_ਗੰਢ_STATE(x86_KSPending_ਰੁਕਾਵਟ) = int_invalid;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਗੰਢ_STATE(ks_Cur_ਸਮਾਂ) = get_Current_ਸਮਾਂ();
    ਗੰਢ_STATE(ksConsumed) = 0;
#ਜੇ_ਅੰਤ

    ਸਮਾਂ_ਸਾਰਣੀ();
    activateThread();
}

