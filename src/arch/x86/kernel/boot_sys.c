/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <hardware.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/apic.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/cmdline.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/boot_sys.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/smp_sys.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/elf.h>
#ಅನ್ತರ್ಭಾವ <smp/lock.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/acpi.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/devices.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/pic.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/ioapic.h>
#ಅನ್ತರ್ಭಾವ <sel4/arch/bootinfo_types.h>

/* addresses defined in linker script */
/* need a fake array to get the pointer from the linker script */

/* start/end of CPU boot code */
ಬಾಹ್ಯ ಅಕ್ಷರ boot_cpu_ಆರಂಭ[1];
ಬಾಹ್ಯ ಅಕ್ಷರ boot_cpu_ಕೊನೆ[1];

/* start/end of boot stack */
ಬಾಹ್ಯ ಅಕ್ಷರ boot_ರಾಶಿ_bottom[1];
ಬಾಹ್ಯ ಅಕ್ಷರ boot_ರಾಶಿ_ತುದಿ[1];

/* locations in kernel image */
ಬಾಹ್ಯ ಅಕ್ಷರ ki_skim_ಆರಂಭ[1];
ಬಾಹ್ಯ ಅಕ್ಷರ ki_skim_ಕೊನೆ[1];

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
/* kernel entry point */
ಬಾಹ್ಯ ಅಕ್ಷರ ಆರಂಭ_2[1];
#ಆದರೆ_ಅಂತ್ಯ

/* constants */

#ವ್ಯಾಖ್ಯಾನಿಸು HIGHMEM_PADDR 0x100000

BOOT_BSS
boot_state_t boot_state;

/* global variables (not covered by abstract specification) */

BOOT_BSS
cmdline_opt_t cmdline_opt;

/* functions not modeled in abstract specification */

BOOT_ಸಂಕೇತ ಸ್ಥಿರ paddr_t ಕಂಡುಹಿಡಿ_ತುಂಬು_paddr(paddr_t min_paddr, ಶಬ್ದ_t image_ಗಾತ್ರ)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < boot_state.mem_p_regs.ಎಣಿಕೆ_2; i++) {
        paddr_t ಆರಂಭ = MAX(min_paddr, boot_state.mem_p_regs.list[i].ಆರಂಭ);
        paddr_t ಕೊನೆ = boot_state.mem_p_regs.list[i].ಕೊನೆ;
        ಶಬ್ದ_t region_ಗಾತ್ರ = ಕೊನೆ - ಆರಂಭ;

        ಆದರೆ (region_ಗಾತ್ರ >= image_ಗಾತ್ರ) {
            ಹಿಂತಿರುಗಿ ಆರಂಭ;
        }
    }

    ಹಿಂತಿರುಗಿ 0;
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ paddr_t ತುಂಬು_boot_module(ಶಬ್ದ_t boot_module_ಆರಂಭ, ಶಬ್ದ_t boot_module_ಕೊನೆ, paddr_t ತುಂಬು_paddr)
{
    v_region_t v_reg;
    ಶಬ್ದ_t ನಮೂದು;
    Elf_ಶೀರ್ಷಿಕೆ_t *elf_file = (Elf_ಶೀರ್ಷಿಕೆ_t *)boot_module_ಆರಂಭ;

    ಆದರೆ (!elf_checkFile(elf_file, boot_module_ಕೊನೆ - boot_module_ಆರಂಭ)) {
        ಬರೆ("Boot module does not contain a valid ELF image\n");
        ಹಿಂತಿರುಗಿ 0;
    }

    v_reg = elf_get_ಸ್ಮರಣೆ_Bounds(elf_file);
    ನಮೂದು = elf_file->e_ನಮೂದು;

    ಆದರೆ (v_reg.ಕೊನೆ == 0) {
        ಬರೆ("ELF image in boot module does not contain any segments\n");
        ಹಿಂತಿರುಗಿ 0;
    }
    v_reg.ಕೊನೆ = ಸುತ್ತು_ಮೇಲೆ(v_reg.ಕೊನೆ, ಪುಟ_ಬಿಟ್ಟುಗಳು);

    ಬರೆ("size=0x%lx v_entry=%p v_start=%p v_end=%p ",
           v_reg.ಕೊನೆ - v_reg.ಆರಂಭ,
           (ಶೂನ್ಯ *)ನಮೂದು,
           (ಶೂನ್ಯ *)v_reg.ಆರಂಭ,
           (ಶೂನ್ಯ *)v_reg.ಕೊನೆ
          );

    ಆದರೆ (!IS_ALIGNED(v_reg.ಆರಂಭ, ಪುಟ_ಬಿಟ್ಟುಗಳು)) {
        ಬರೆ("Userland image virtual start address must be 4KB-aligned\n");
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (v_reg.ಕೊನೆ + 2 * ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು) > USER_ತುದಿ) {
        /* for IPC buffer frame and bootinfo frame, need 2*4K of additional userland virtual memory */
        ಬರೆ("Userland image virtual end address too high\n");
        ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ ((ನಮೂದು < v_reg.ಆರಂಭ) || (ನಮೂದು >= v_reg.ಕೊನೆ)) {
        ಬರೆ("Userland image entry point does not lie within userland image\n");
        ಹಿಂತಿರುಗಿ 0;
    }

    ತುಂಬು_paddr = ಕಂಡುಹಿಡಿ_ತುಂಬು_paddr(ತುಂಬು_paddr, v_reg.ಕೊನೆ - v_reg.ಆರಂಭ);
    ಪ್ರತಿಜ್ಞಾ(ತುಂಬು_paddr);

    /* fill ui_info struct */
    boot_state.ui_info.pv_ಸ್ಥಾನಾಂತರ = ತುಂಬು_paddr - v_reg.ಆರಂಭ;
    boot_state.ui_info.p_reg.ಆರಂಭ = ತುಂಬು_paddr;
    ತುಂಬು_paddr += v_reg.ಕೊನೆ - v_reg.ಆರಂಭ;
    boot_state.ui_info.p_reg.ಕೊನೆ = ತುಂಬು_paddr;
    boot_state.ui_info.v_ನಮೂದು = ನಮೂದು;

    ಬರೆ("p_start=0x%lx p_end=0x%lx\n",
           boot_state.ui_info.p_reg.ಆರಂಭ,
           boot_state.ui_info.p_reg.ಕೊನೆ
          );

    /* initialise all initial userland memory and load potentially sparse ELF image */
    memzero(
        (ಶೂನ್ಯ *)boot_state.ui_info.p_reg.ಆರಂಭ,
        boot_state.ui_info.p_reg.ಕೊನೆ - boot_state.ui_info.p_reg.ಆರಂಭ
    );
    elf_ತುಂಬು(elf_file, boot_state.ui_info.pv_ಸ್ಥಾನಾಂತರ);

    ಹಿಂತಿರುಗಿ ತುಂಬು_paddr;
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ bool_t try_boot_sys_ಗಂಟು(cpu_id_t cpu_id)
{
    ಆದರೆ (!map_kernel_ಕಿಟಕಿ(
            boot_state.ಅಂಕೆ_ioapic,
            boot_state.ioapic_paddr,
            boot_state.ಅಂಕೆ_drhu,
            boot_state.drhu_list
        )) {
        ಹಿಂತಿರುಗಿ false;
    }
    ಗಣ_Current_VSpace_ಬೇರು(kpptr_ಗಮ್ಯ_paddr(X86_KERNEL_VSPACE_ಬೇರು), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm ಅಸ್ಥಿರ("" ::: "memory");

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_SKIM_ಕಿಟಕಿ
    ಆದರೆ (!map_skim_ಕಿಟಕಿ((vptr_t)ki_skim_ಆರಂಭ, (vptr_t)ki_skim_ಕೊನೆ)) {
        ಹಿಂತಿರುಗಿ false;
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* initialise the CPU */
    ಆದರೆ (!init_cpu(config_ಗಣ(CONFIG_IRQ_IOAPIC) ? 1 : 0)) {
        ಹಿಂತಿರುಗಿ false;
    }

    /* initialise NDKS and kernel heap */
    ಆದರೆ (!init_sys_state(
            cpu_id,
            &boot_state.mem_p_regs,
            boot_state.ui_info,
            /* parameters below not modelled in abstract specification */
            boot_state.ಅಂಕೆ_drhu,
            boot_state.drhu_list,
            &boot_state.rmrr_list,
            &boot_state.acpi_rsdp,
            &boot_state.vbe_info,
            &boot_state.mb_mmap_info,
            &boot_state.fb_info
        )) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ bool_t ಕೂಡಿಸು_mem_p_regs(p_region_t reg)
{
    ಆದರೆ (reg.ಆರಂಭ == reg.ಕೊನೆ) {
        // Return true here if asked to add an empty region.
        // Some of the callers round down the end address to
        ಹಿಂತಿರುಗಿ true;
    }
    ಆದರೆ (reg.ಕೊನೆ > PADDR_ತುದಿ && reg.ಆರಂಭ >= PADDR_ತುದಿ) {
        /* Return true here as it's not an error for there to exist memory outside the kernel window,
         * we're just going to ignore it and leave it to be given out as device memory */
        ಹಿಂತಿರುಗಿ true;
    }
    ಆದರೆ (boot_state.mem_p_regs.ಎಣಿಕೆ_2 == MAX_ಅಂಕೆ_FREEMEM_REG) {
        ಬರೆ("Dropping memory region 0x%lx-0x%lx, try increasing MAX_NUM_FREEMEM_REG\n", reg.ಆರಂಭ, reg.ಕೊನೆ);
        ಹಿಂತಿರುಗಿ false;
    }
    ಆದರೆ (reg.ಕೊನೆ > PADDR_ತುದಿ) {
        ಪ್ರತಿಜ್ಞಾ(reg.ಆರಂಭ < PADDR_ತುದಿ);
        /* Clamp a region to the top of the kernel window if it extends beyond */
        reg.ಕೊನೆ = PADDR_ತುದಿ;
    }
    ಬರೆ("Adding physical memory region 0x%lx-0x%lx\n", reg.ಆರಂಭ, reg.ಕೊನೆ);
    boot_state.mem_p_regs.list[boot_state.mem_p_regs.ಎಣಿಕೆ_2] = reg;
    boot_state.mem_p_regs.ಎಣಿಕೆ_2++;
    ಹಿಂತಿರುಗಿ true;
}

/*
 * the code relies that the GRUB provides correct information
 * about the actual physical memory regions.
 */
ಸ್ಥಿರ BOOT_ಸಂಕೇತ bool_t parse_mem_map(uint32_t mmap_ಉದ್ದಳತೆ, uint32_t mmap_addr)
{
    multiboot_mmap_t *mmap = (multiboot_mmap_t *)((ಶಬ್ದ_t)mmap_addr);
    ಬರೆ("Parsing GRUB physical memory map\n");

    ಆಗ ((ಶಬ್ದ_t)mmap < (ಶಬ್ದ_t)(mmap_addr + mmap_ಉದ್ದಳತೆ)) {
        uint64_t mem_ಆರಂಭ = mmap->ತಳ_addr;
        uint64_t mem_ಉದ್ದಳತೆ = mmap->ಉದ್ದಳತೆ;
        uint32_t type = mmap->type;
        ಆದರೆ (mem_ಆರಂಭ != (uint64_t)(ಶಬ್ದ_t)mem_ಆರಂಭ) {
            ಬರೆ("\tPhysical memory region not addressable\n");
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಬರೆ("\tPhysical Memory Region from %lx size %lx type %d\n", (ದೀರ್ಘ)mem_ಆರಂಭ, (ದೀರ್ಘ)mem_ಉದ್ದಳತೆ, type);
            ಆದರೆ (type == MULTIBOOT_MMAP_USEABLE_TYPE && mem_ಆರಂಭ >= HIGHMEM_PADDR && mem_ಉದ್ದಳತೆ >= ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು)) {

                ಆದರೆ (!ಕೂಡಿಸು_mem_p_regs((p_region_t) {
                ಸುತ್ತು_ಮೇಲೆ(mem_ಆರಂಭ, ಪುಟ_ಬಿಟ್ಟುಗಳು), ಸುತ್ತು_ಕೆಳಕ್ಕೆ(mem_ಆರಂಭ + mem_ಉದ್ದಳತೆ, ಪುಟ_ಬಿಟ್ಟುಗಳು),
                })) {
                    ಹಿಂತಿರುಗಿ false;
                }
            }
        }
        mmap++;
    }
    ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ bool_t is_compiled_for_microarchitecture(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t microarch_generation = 0;
    x86_cpu_ತಟಸ್ಥ_t *model_info = x86_cpuid_get_model_info();

    ಆದರೆ (config_ಗಣ(CONFIG_ARCH_X86_SKYLAKE)) {
        microarch_generation = 7;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (config_ಗಣ(CONFIG_ARCH_X86_BROADWELL)) {
        microarch_generation = 6;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (config_ಗಣ(CONFIG_ARCH_X86_HASWELL)) {
        microarch_generation = 5;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (config_ಗಣ(CONFIG_ARCH_X86_IVY)) {
        microarch_generation = 4;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (config_ಗಣ(CONFIG_ARCH_X86_SANDY)) {
        microarch_generation = 3;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (config_ಗಣ(CONFIG_ARCH_X86_WESTMERE)) {
        microarch_generation = 2;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (config_ಗಣ(CONFIG_ARCH_X86_NEHALEM)) {
        microarch_generation = 1;
    }

    ಆಯ್ಕೆ (model_info->model) {
    ಸ್ಥಿತಿ SKYLAKE_1_MODEL_ID:
    ಸ್ಥಿತಿ SKYLAKE_2_MODEL_ID:
    ಸ್ಥಿತಿ SKYLAKE_X_MODEL_ID:
        ಆದರೆ (microarch_generation > 7) {
            ಹಿಂತಿರುಗಿ false;
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ BROADWELL_1_MODEL_ID:
    ಸ್ಥಿತಿ BROADWELL_2_MODEL_ID:
    ಸ್ಥಿತಿ BROADWELL_3_MODEL_ID:
    ಸ್ಥಿತಿ BROADWELL_4_MODEL_ID:
    ಸ್ಥಿತಿ BROADWELL_5_MODEL_ID:
        ಆದರೆ (microarch_generation > 6) {
            ಹಿಂತಿರುಗಿ false;
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ HASWELL_1_MODEL_ID:
    ಸ್ಥಿತಿ HASWELL_2_MODEL_ID:
    ಸ್ಥಿತಿ HASWELL_3_MODEL_ID:
    ಸ್ಥಿತಿ HASWELL_4_MODEL_ID:
        ಆದರೆ (microarch_generation > 5) {
            ಹಿಂತಿರುಗಿ false;
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ IVY_BRIDGE_1_MODEL_ID:
    ಸ್ಥಿತಿ IVY_BRIDGE_2_MODEL_ID:
    ಸ್ಥಿತಿ IVY_BRIDGE_3_MODEL_ID:
        ಆದರೆ (microarch_generation > 4) {
            ಹಿಂತಿರುಗಿ false;
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ SANDY_BRIDGE_1_MODEL_ID:
    ಸ್ಥಿತಿ SANDY_BRIDGE_2_MODEL_ID:
        ಆದರೆ (microarch_generation > 3) {
            ಹಿಂತಿರುಗಿ false;
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ WESTMERE_1_MODEL_ID:
    ಸ್ಥಿತಿ WESTMERE_2_MODEL_ID:
    ಸ್ಥಿತಿ WESTMERE_3_MODEL_ID:
        ಆದರೆ (microarch_generation > 2) {
            ಹಿಂತಿರುಗಿ false;
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ NEHALEM_1_MODEL_ID:
    ಸ್ಥಿತಿ NEHALEM_2_MODEL_ID:
    ಸ್ಥಿತಿ NEHALEM_3_MODEL_ID:
        ಆದರೆ (microarch_generation > 1) {
            ಹಿಂತಿರುಗಿ false;
        }
        ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಆದರೆ (!config_ಗಣ(CONFIG_ARCH_X86_GENERIC)) {
            ಹಿಂತಿರುಗಿ false;
        }
    }

    ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ bool_t try_boot_sys(ಶೂನ್ಯ)
{
    paddr_t mods_ಕೊನೆ_paddr = boot_state.mods_ಕೊನೆ_paddr;
    p_region_t ui_p_regs;
    paddr_t ತುಂಬು_paddr;

    boot_state.ki_p_reg = get_p_reg_kernel_img();

    ಆದರೆ (!x86_cpuid_initialize()) {
        ಬರೆ("Warning: Your x86 CPU has an unsupported vendor, '%s'.\n"
               "\tYour setup may not be able to competently run seL4 as "
               "intended.\n"
               "\tCurrently supported x86 vendors are AMD and Intel.\n",
               x86_cpuid_get_ತಟಸ್ಥ()->vendor_string);
    }

    ಆದರೆ (!is_compiled_for_microarchitecture()) {
        ಬರೆ("Warning: Your kernel was not compiled for the current microarchitecture.\n");
    }

    cpuid_007h_edx_t edx;
    edx.ಪದಗಳು[0] = x86_cpuid_edx(0x7, 0);
    /* see if we can definitively say whether or not we need the skim window by
     * checking whether the CPU is vulnerable to rogue data cache loads (rdcl) */
    ಆದರೆ (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.ಪದಗಳು[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        ಆದರೆ (ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && config_ಗಣ(CONFIG_KERNEL_SKIM_ಕಿಟಕಿ)) {
            ಬರೆ("CPU reports not vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is enabled. Performance is needlessly being impacted, consider disabling.\n");
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && !config_ಗಣ(CONFIG_KERNEL_SKIM_ಕಿಟಕಿ)) {
            ಬರೆ("CPU reports vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is *not* enabled. Please re-build with SKIM window enabled.");
            ಹಿಂತಿರುಗಿ false;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* hardware doesn't tell us directly so guess based on CPU vendor */
        ಆದರೆ (config_ಗಣ(CONFIG_KERNEL_SKIM_ಕಿಟಕಿ) && x86_cpuid_get_ತಟಸ್ಥ()->vendor == X86_VENDOR_AMD) {
            ಬರೆ("SKIM window for mitigating Meltdown (https://www.meltdownattack.com) "
                   "not necessary for AMD and performance is being needlessly affected, "
                   "consider disabling\n");
        }
        ಆದರೆ (!config_ಗಣ(CONFIG_KERNEL_SKIM_ಕಿಟಕಿ) && x86_cpuid_get_ತಟಸ್ಥ()->vendor == X86_VENDOR_INTEL) {
            ಬರೆ("***WARNING*** SKIM window not enabled, this machine is probably vulnerable "
                   "to Meltdown (https://www.meltdownattack.com), consider enabling\n");
        }
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    /* copy boot code for APs to lower memory to run in real mode */
    ಆದರೆ (!copy_boot_ಸಂಕೇತ_aps(boot_state.mem_ಅಧೋ)) {
        ಹಿಂತಿರುಗಿ false;
    }
    /* Initialize any kernel TLS */
    ಬಹುಲಕ_init_tls(0);
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

    ಬರೆ("Kernel loaded to: start=0x%lx end=0x%lx size=0x%lx entry=0x%lx\n",
           boot_state.ki_p_reg.ಆರಂಭ,
           boot_state.ki_p_reg.ಕೊನೆ,
           boot_state.ki_p_reg.ಕೊನೆ - boot_state.ki_p_reg.ಆರಂಭ,
           (paddr_t)ಆರಂಭ_2
          );

    /* remapping legacy IRQs to their correct vectors */
    pic_remap_irqs(IRQ_INT_ಸ್ಥಾನಾಂತರ);
    ಆದರೆ (config_ಗಣ(CONFIG_IRQ_IOAPIC)) {
        /* Disable the PIC so that it does not generate any interrupts. We need to
         * do this *before* we initialize the apic */
        pic_disable();
    }

    /* validate the ACPI table */
    ಆದರೆ (!acpi_validate_rsdp(&boot_state.acpi_rsdp)) {
        ಹಿಂತಿರುಗಿ false;
    }

    /* check if kernel configuration matches platform requirements */
    ಆದರೆ (!acpi_fadt_ಪರಿಶೀಲನೆ(&boot_state.acpi_rsdp)) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (!config_ಗಣ(CONFIG_IOMMU) || cmdline_opt.disable_iommu) {
        boot_state.ಅಂಕೆ_drhu = 0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* query available IOMMUs from ACPI */
        acpi_dmar_ಪರಿಶೀಲನೆ(
            &boot_state.acpi_rsdp,
            boot_state.drhu_list,
            &boot_state.ಅಂಕೆ_drhu,
            MAX_ಅಂಕೆ_DRHU,
            &boot_state.rmrr_list
        );
    }

    /* query available CPUs from ACPI */
    boot_state.ಅಂಕೆ_cpus = acpi_madt_ಪರಿಶೀಲನೆ(&boot_state.acpi_rsdp, boot_state.cpus, &boot_state.ಅಂಕೆ_ioapic,
                                         boot_state.ioapic_paddr);
    ಆದರೆ (boot_state.ಅಂಕೆ_cpus == 0) {
        ಬರೆ("No CPUs detected\n");
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (config_ಗಣ(CONFIG_IRQ_IOAPIC)) {
        ಆದರೆ (boot_state.ಅಂಕೆ_ioapic == 0) {
            ಬರೆ("No IOAPICs detected\n");
            ಹಿಂತಿರುಗಿ false;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (boot_state.ಅಂಕೆ_ioapic > 0) {
            ಬರೆ("Detected %d IOAPICs, but configured to use PIC instead\n", boot_state.ಅಂಕೆ_ioapic);
        }
    }

    mods_ಕೊನೆ_paddr = ಸುತ್ತು_ಮೇಲೆ(mods_ಕೊನೆ_paddr, ಪುಟ_ಬಿಟ್ಟುಗಳು);
    ಪ್ರತಿಜ್ಞಾ(mods_ಕೊನೆ_paddr > boot_state.ki_p_reg.ಕೊನೆ);

    ಬರೆ("ELF-loading userland images from boot modules:\n");
    ತುಂಬು_paddr = mods_ಕೊನೆ_paddr;

    ತುಂಬು_paddr = ತುಂಬು_boot_module(boot_state.boot_module_ಆರಂಭ, boot_state.boot_module_ಕೊನೆ, ತುಂಬು_paddr);
    ಆದರೆ (!ತುಂಬು_paddr) {
        ಹಿಂತಿರುಗಿ false;
    }

    /* calculate final location of userland images */
    ui_p_regs.ಆರಂಭ = boot_state.ki_p_reg.ಕೊನೆ;
    ui_p_regs.ಕೊನೆ = ui_p_regs.ಆರಂಭ + ತುಂಬು_paddr - mods_ಕೊನೆ_paddr;

    ಬರೆ(
        "Moving loaded userland images to final location: from=0x%lx to=0x%lx size=0x%lx\n",
        mods_ಕೊನೆ_paddr,
        ui_p_regs.ಆರಂಭ,
        ui_p_regs.ಕೊನೆ - ui_p_regs.ಆರಂಭ
    );
    ಸ್ಮರಣೆ_ನಕಲು((ಶೂನ್ಯ *)ui_p_regs.ಆರಂಭ, (ಶೂನ್ಯ *)mods_ಕೊನೆ_paddr, ui_p_regs.ಕೊನೆ - ui_p_regs.ಆರಂಭ);

    /* adjust p_reg and pv_offset to final load address */
    boot_state.ui_info.p_reg.ಆರಂಭ -= mods_ಕೊನೆ_paddr - ui_p_regs.ಆರಂಭ;
    boot_state.ui_info.p_reg.ಕೊನೆ   -= mods_ಕೊನೆ_paddr - ui_p_regs.ಆರಂಭ;
    boot_state.ui_info.pv_ಸ್ಥಾನಾಂತರ   -= mods_ಕೊನೆ_paddr - ui_p_regs.ಆರಂಭ;

    /* ==== following code corresponds to abstract specification after "select" ==== */

    ಆದರೆ (!plat_ಕೂಡಿಸು_Devices()) {
        ಹಿಂತಿರುಗಿ false;
    }

    /* Total number of cores we intend to boot */
    ks_ಅಂಕೆ_CPUs = boot_state.ಅಂಕೆ_cpus;

    ಬರೆ("Starting node #0 with APIC ID %lu\n", boot_state.cpus[0]);
    ಆದರೆ (!try_boot_sys_ಗಂಟು(boot_state.cpus[0])) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (config_ಗಣ(CONFIG_IRQ_IOAPIC)) {
        ioapic_init(1, boot_state.cpus, boot_state.ಅಂಕೆ_ioapic);
    }

    /* initialize BKL before booting up APs */
    SMP_COND_STATEMENT(clh_ಬೀಗ_init());
    SMP_COND_STATEMENT(ಆರಂಭ_boot_aps());

    /* grab BKL before leaving the kernel */
    ಗಂಟು_ಬೀಗ_SYS;

    ಬರೆ("Booting all finished, dropped to user space\n");

    ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ bool_t try_boot_sys_mbi1(
    multiboot_info_t *mbi
)
{
    ಶಬ್ದ_t i;
    multiboot_module_t *modules = (multiboot_module_t *)(ಶಬ್ದ_t)mbi->part1.mod_list;

    cmdline_parse((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)(ಶಬ್ದ_t)mbi->part1.cmdline, &cmdline_opt);

    ಆದರೆ ((mbi->part1.flags & MULTIBOOT_INFO_MEM_FLAG) == 0) {
        ಬರೆ("Boot loader did not provide information about physical memory size\n");
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (!(mbi->part1.flags & MULTIBOOT_INFO_MODS_FLAG)) {
        ಬರೆ("Boot loader did not provide information about boot modules\n");
        ಹಿಂತಿರುಗಿ false;
    }

    ಬರೆ("Detected %d boot module(s):\n", mbi->part1.mod_ಎಣಿಕೆ);

    ಆದರೆ (mbi->part1.mod_ಎಣಿಕೆ < 1) {
        ಬರೆ("Expect at least one boot module (containing a userland image)\n");
        ಹಿಂತಿರುಗಿ false;
    }

    ಗಾಗಿ (i = 0; i < mbi->part1.mod_ಎಣಿಕೆ; i++) {
        ಬರೆ(
            "  module #%ld: start=0x%x end=0x%x size=0x%x name='%s'\n",
            i,
            modules[i].ಆರಂಭ,
            modules[i].ಕೊನೆ,
            modules[i].ಕೊನೆ - modules[i].ಆರಂಭ,
            (ಅಕ್ಷರ *)(ದೀರ್ಘ)modules[i].name
        );
        ಆದರೆ ((sword_t)(modules[i].ಕೊನೆ - modules[i].ಆರಂಭ) <= 0) {
            ಬರೆ("Invalid boot module size! Possible cause: boot module file not found by QEMU\n");
            ಹಿಂತಿರುಗಿ false;
        }
        ಆದರೆ (boot_state.mods_ಕೊನೆ_paddr < modules[i].ಕೊನೆ) {
            boot_state.mods_ಕೊನೆ_paddr = modules[i].ಕೊನೆ;
        }
    }

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.ಎಣಿಕೆ_2 = 0;
    ಆದರೆ (mbi->part1.flags & MULTIBOOT_INFO_MMAP_FLAG) {
        ಆದರೆ (!parse_mem_map(mbi->part2.mmap_ಉದ್ದಳತೆ, mbi->part2.mmap_addr)) {
            ಹಿಂತಿರುಗಿ false;
        }
        uint32_t multiboot_mmap_ಉದ್ದಳತೆ = mbi->part2.mmap_ಉದ್ದಳತೆ;
        ಆದರೆ (multiboot_mmap_ಉದ್ದಳತೆ > (SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * ಗಾತ್ರ(seL4_X86_mb_mmap_t))) {
            ಬರೆ("Warning: Multiboot has reported more memory map entries, %zd, "
                   "than the max amount that will be passed in the bootinfo, %d. "
                   "These extra regions will still be turned into untyped caps.",
                   multiboot_mmap_ಉದ್ದಳತೆ / ಗಾತ್ರ(seL4_X86_mb_mmap_t), SEL4_MULTIBOOT_MAX_MMAP_ENTRIES);
            multiboot_mmap_ಉದ್ದಳತೆ = SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * ಗಾತ್ರ(seL4_X86_mb_mmap_t);
        }
        ಸ್ಮರಣೆ_ನಕಲು(&boot_state.mb_mmap_info.mmap, (ಶೂನ್ಯ *)(ಶಬ್ದ_t)mbi->part2.mmap_addr, multiboot_mmap_ಉದ್ದಳತೆ);
        boot_state.mb_mmap_info.mmap_ಉದ್ದಳತೆ = multiboot_mmap_ಉದ್ದಳತೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* calculate memory the old way */
        p_region_t avail;
        avail.ಆರಂಭ = HIGHMEM_PADDR;
        avail.ಕೊನೆ = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(avail.ಆರಂಭ + (mbi->part1.mem_ಊರ್ಧ್ವ << 10), ಪುಟ_ಬಿಟ್ಟುಗಳು);
        ಆದರೆ (!ಕೂಡಿಸು_mem_p_regs(avail)) {
            ಹಿಂತಿರುಗಿ false;
        }
    }

    /* copy VESA information from multiboot header */
    ಆದರೆ ((mbi->part1.flags & MULTIBOOT_INFO_GRAPHICS_FLAG) == 0) {
        boot_state.vbe_info.vbe_ಬಹುಲಕ = -1;
        ಬರೆ("Multiboot gave us no video information\n");
    } ಇಲ್ಲದಿದ್ದರೆ {
        boot_state.vbe_info.vbe_Info_ಖಂಡ = *(se_L4_VBEInfo_ಖಂಡ_t *)(se_L4_ಶಬ್ದ)mbi->part2.vbe_control_info;
        boot_state.vbe_info.vbe_ಬಹುಲಕ_Info_ಖಂಡ = *(se_L4_VBEMode_Info_ಖಂಡ_t *)(se_L4_ಶಬ್ದ)mbi->part2.vbe_ಬಹುಲಕ_info;
        boot_state.vbe_info.vbe_ಬಹುಲಕ = mbi->part2.vbe_ಬಹುಲಕ_2;
        ಬರೆ("Got VBE info in multiboot. Current video mode is %d\n", mbi->part2.vbe_ಬಹುಲಕ_2);
        boot_state.vbe_info.vbeInterfaceSeg = mbi->part2.vbe_interface_seg;
        boot_state.vbe_info.vbeInterfaceOff = mbi->part2.vbe_interface_off;
        boot_state.vbe_info.vbeInterfaceLen = mbi->part2.vbe_interface_len;
    }

    boot_state.mem_ಅಧೋ = mbi->part1.mem_ಅಧೋ;
    boot_state.boot_module_ಆರಂಭ = modules->ಆರಂಭ;
    boot_state.boot_module_ಕೊನೆ = modules->ಕೊನೆ;

    /* Initialize ACPI */
    ಆದರೆ (!acpi_init(&boot_state.acpi_rsdp)) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ BOOT_ಸಂಕೇತ bool_t try_boot_sys_mbi2(
    multiboot2_ಶೀರ್ಷಿಕೆ_t *mbi2
)
{
    ಪೂರ್ಣಾಂಕ mod_ಎಣಿಕೆ                  = 0;
    multiboot2_ಟ್ಯಾಗ್_t ಸ್ಥಿರಾಂಕ *ಟ್ಯಾಗ್   = (multiboot2_ಟ್ಯಾಗ್_t *)(mbi2 + 1);
    multiboot2_ಟ್ಯಾಗ್_t ಸ್ಥಿರಾಂಕ *ಟ್ಯಾಗ್_e = (multiboot2_ಟ್ಯಾಗ್_t *)((ಶಬ್ದ_t)mbi2 + mbi2->ಒಟ್ಟು_ಗಾತ್ರ);

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.ಎಣಿಕೆ_2 = 0;
    boot_state.mb_mmap_info.mmap_ಉದ್ದಳತೆ = 0;
    boot_state.vbe_info.vbe_ಬಹುಲಕ = -1;

    ಆಗ (ಟ್ಯಾಗ್ < ಟ್ಯಾಗ್_e && ಟ್ಯಾಗ್->type != MULTIBOOT2_ಟ್ಯಾಗ್_ಕೊನೆ) {
        ಶಬ್ದ_t ಸ್ಥಿರಾಂಕ behind_ಟ್ಯಾಗ್ = (ಶಬ್ದ_t)ಟ್ಯಾಗ್ + ಗಾತ್ರ(*ಟ್ಯಾಗ್);

        ಆದರೆ (ಟ್ಯಾಗ್->type == MULTIBOOT2_ಟ್ಯಾಗ್_CMDLINE) {
            ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ *ಸ್ಥಿರಾಂಕ cmdline = (ಅಕ್ಷರ ಸ್ಥಿರಾಂಕ * ಸ್ಥಿರಾಂಕ)(behind_ಟ್ಯಾಗ್);
            cmdline_parse(cmdline, &cmdline_opt);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಟ್ಯಾಗ್->type == MULTIBOOT2_ಟ್ಯಾಗ್_ACPI_1) {
            ಆದರೆ (ACPI_V1_ಗಾತ್ರ == ಟ್ಯಾಗ್->ಗಾತ್ರ_2 - ಗಾತ್ರ(*ಟ್ಯಾಗ್)) {
                ಸ್ಮರಣೆ_ನಕಲು(&boot_state.acpi_rsdp, (ಶೂನ್ಯ *)behind_ಟ್ಯಾಗ್, ಟ್ಯಾಗ್->ಗಾತ್ರ_2 - ಗಾತ್ರ(*ಟ್ಯಾಗ್));
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಟ್ಯಾಗ್->type == MULTIBOOT2_ಟ್ಯಾಗ್_ACPI_2) {
            ಆದರೆ (ಗಾತ್ರ(boot_state.acpi_rsdp) == ಟ್ಯಾಗ್->ಗಾತ್ರ_2 - ಗಾತ್ರ(*ಟ್ಯಾಗ್)) {
                ಸ್ಮರಣೆ_ನಕಲು(&boot_state.acpi_rsdp, (ಶೂನ್ಯ *)behind_ಟ್ಯಾಗ್, ಗಾತ್ರ(boot_state.acpi_rsdp));
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಟ್ಯಾಗ್->type == MULTIBOOT2_ಟ್ಯಾಗ್_MODULE) {
            multiboot2_module_t ಸ್ಥಿರಾಂಕ *module = (multiboot2_module_t ಸ್ಥಿರಾಂಕ *)behind_ಟ್ಯಾಗ್;
            ಬರೆ(
                "  module #%d: start=0x%x end=0x%x size=0x%x name='%s'\n",
                mod_ಎಣಿಕೆ,
                module->ಆರಂಭ,
                module->ಕೊನೆ,
                module->ಕೊನೆ - module->ಆರಂಭ,
                module->string
            );

            ಆದರೆ (mod_ಎಣಿಕೆ == 0) {
                boot_state.boot_module_ಆರಂಭ = module->ಆರಂಭ;
                boot_state.boot_module_ಕೊನೆ = module->ಕೊನೆ;
            }

            mod_ಎಣಿಕೆ ++;
            ಆದರೆ ((sword_t)(module->ಕೊನೆ - module->ಆರಂಭ) <= 0) {
                ಬರೆ("Invalid boot module size! Possible cause: boot module file not found\n");
                ಹಿಂತಿರುಗಿ false;
            }
            ಆದರೆ (boot_state.mods_ಕೊನೆ_paddr < module->ಕೊನೆ) {
                boot_state.mods_ಕೊನೆ_paddr = module->ಕೊನೆ;
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಟ್ಯಾಗ್->type == MULTIBOOT2_ಟ್ಯಾಗ್_ಸ್ಮರಣೆ) {
            multiboot2_ಸ್ಮರಣೆ_t ಸ್ಥಿರಾಂಕ *s = (multiboot2_ಸ್ಮರಣೆ_t *)(behind_ಟ್ಯಾಗ್ + 8);
            multiboot2_ಸ್ಮರಣೆ_t ಸ್ಥಿರಾಂಕ *e = (multiboot2_ಸ್ಮರಣೆ_t *)((ಶಬ್ದ_t)ಟ್ಯಾಗ್ + ಟ್ಯಾಗ್->ಗಾತ್ರ_2);

            ಗಾಗಿ (multiboot2_ಸ್ಮರಣೆ_t ಸ್ಥಿರಾಂಕ *m = s; m < e; m++) {
                ಆದರೆ (!m->addr) {
                    boot_state.mem_ಅಧೋ = m->ಗಾತ್ರ_2;
                }

                ಬರೆ("\tPhysical Memory Region from %llx size %llx type %u\n", m->addr, m->ಗಾತ್ರ_2, m->type);
                ಆದರೆ (m->addr != (uint64_t)(ಶಬ್ದ_t)m->addr) {
                    ಬರೆ("\t\tPhysical memory region not addressable\n");
                    ಮುಂದುವರಿ;
                }

                ಆದರೆ (m->type == MULTIBOOT_MMAP_USEABLE_TYPE && m->addr >= HIGHMEM_PADDR) {
                    ಆದರೆ (!ಕೂಡಿಸು_mem_p_regs((p_region_t) {
                    m->addr, m->addr + m->ಗಾತ್ರ_2
                }))
                    ಹಿಂತಿರುಗಿ false;
                }
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಟ್ಯಾಗ್->type == MULTIBOOT2_ಟ್ಯಾಗ್_FB) {
            multiboot2_fb_t ಸ್ಥಿರಾಂಕ *fb = (multiboot2_fb_t ಸ್ಥಿರಾಂಕ *)behind_ಟ್ಯಾಗ್;
            ಬರೆ("Got framebuffer info in multiboot2. Current video mode is at physical address=%llx pitch=%u resolution=%ux%u@%u type=%u\n",
                   fb->addr, fb->pitch, fb->ಅಗಲ, fb->ಎತ್ತರ, fb->bpp, fb->type);
            boot_state.fb_info = *fb;
        }

        ಟ್ಯಾಗ್ = (multiboot2_ಟ್ಯಾಗ್_t ಸ್ಥಿರಾಂಕ *)((ಶಬ್ದ_t)ಟ್ಯಾಗ್ + ಸುತ್ತು_ಮೇಲೆ(ಟ್ಯಾಗ್->ಗಾತ್ರ_2, 3));
    }

    ಬರೆ("Detected %d boot module(s):\n", mod_ಎಣಿಕೆ);

    ಆದರೆ (mod_ಎಣಿಕೆ < 1) {
        ಬರೆ("Expect at least one boot module (containing a userland image)\n");
        ಹಿಂತಿರುಗಿ false;
    }

    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ VISIBLE ಶೂನ್ಯ boot_sys(
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ multiboot_magic,
    ಶೂನ್ಯ *mbi)
{
    bool_t ಫಲಿತಾಂಶ = false;

    ಆದರೆ (multiboot_magic == MULTIBOOT_MAGIC) {
        ಫಲಿತಾಂಶ = try_boot_sys_mbi1(mbi);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (multiboot_magic == MULTIBOOT2_MAGIC) {
        ಫಲಿತಾಂಶ = try_boot_sys_mbi2(mbi);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಬರೆ("Boot loader is not multiboot 1 or 2 compliant %lx\n", multiboot_magic);
    }

    ಆದರೆ (ಫಲಿತಾಂಶ) {
        ಫಲಿತಾಂಶ = try_boot_sys();
    }

    ಆದರೆ (!ಫಲಿತಾಂಶ) {
        fail("boot_sys failed for some reason :(\n");
    }

    ARCH_ಗಂಟು_STATE(x86_KScur_ಅಡಚಣೆ) = int_invalid;
    ARCH_ಗಂಟು_STATE(x86_KSPending_ಅಡಚಣೆ) = int_invalid;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಗಂಟು_STATE(ks_Cur_ಸಮಯ) = get_Current_ಸಮಯ();
    ಗಂಟು_STATE(ksConsumed) = 0;
#ಆದರೆ_ಅಂತ್ಯ

    ವೇಳಾಪಟ್ಟಿ();
    activateThread();
}

