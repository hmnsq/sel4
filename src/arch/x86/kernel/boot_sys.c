/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <util.h>
#include <hardware.h>
#include <machine/io.h>
#include <arch/machine.h>
#include <arch/kernel/apic.h>
#include <arch/kernel/cmdline.h>
#include <arch/kernel/boot.h>
#include <arch/kernel/boot_sys.h>
#include <arch/kernel/smp_sys.h>
#include <arch/kernel/vspace.h>
#include <arch/kernel/elf.h>
#include <smp/lock.h>
#include <linker.h>
#include <plat/machine/acpi.h>
#include <plat/machine/devices.h>
#include <plat/machine/pic.h>
#include <plat/machine/ioapic.h>
#include <sel4/arch/bootinfo_types.h>

/* addresses defined in linker script */
/* need a fake array to get the pointer from the linker script */

/* start/end of CPU boot code */
extern var_na boot_cpu_start[1];
extern var_na boot_cpu_end[1];

/* start/end of boot stack */
extern var_na boot_stack_bottom[1];
extern var_na boot_stack_top[1];

/* locations in kernel image */
extern var_na ki_skim_start[1];
extern var_na ki_skim_end[1];

#ifdef CONFIG_PRINTING
/* kernel entry point */
extern var_na _start[1];
#endif

/* constants */

#define HIGHMEM_PADDR 0x100000

BOOT_BSS
boot_state_t boot_state;

/* global variables (not covered by abstract specification) */

BOOT_BSS
cmdline_opt_t cmdline_opt;

/* functions not modeled in abstract specification */

BOOT_CODE static paddr_t find_load_paddr(paddr_t min_paddr, word_t image_size)
{
    ཧྲིལ i;

    དོན་ལུ (i = 0; i < boot_state.mem_p_regs.count; i++) {
        paddr_t start = MAX(min_paddr, boot_state.mem_p_regs.list[i].start);
        paddr_t end = boot_state.mem_p_regs.list[i].end;
        word_t region_size = end - start;

        གལ་སྲིད (region_size >= image_size) {
            ལོག start;
        }
    }

    ལོག 0;
}

BOOT_CODE static paddr_t load_boot_module(word_t boot_module_start, word_t boot_module_end, paddr_t load_paddr)
{
    v_region_t v_reg;
    word_t entry;
    Elf_Header_t *elf_file = (Elf_Header_t *)boot_module_start;

    གལ་སྲིད (!elf_checkFile(elf_file, boot_module_end - boot_module_start)) {
        printf("Boot module does not contain a valid ELF image\n");
        ལོག 0;
    }

    v_reg = elf_getMemoryBounds(elf_file);
    entry = elf_file->e_entry;

    གལ་སྲིད (v_reg.end == 0) {
        printf("ELF image in boot module does not contain any segments\n");
        ལོག 0;
    }
    v_reg.end = ROUND_UP(v_reg.end, PAGE_BITS);

    printf("size=0x%lx v_entry=%p v_start=%p v_end=%p ",
           v_reg.end - v_reg.start,
           (void *)entry,
           (void *)v_reg.start,
           (void *)v_reg.end
          );

    གལ་སྲིད (!IS_ALIGNED(v_reg.start, PAGE_BITS)) {
        printf("Userland image virtual start address must be 4KB-aligned\n");
        ལོག 0;
    }
    གལ་སྲིད (v_reg.end + 2 * BIT(PAGE_BITS) > USER_TOP) {
        /* for IPC buffer frame and bootinfo frame, need 2*4K of additional userland virtual memory */
        printf("Userland image virtual end address too high\n");
        ལོག 0;
    }
    གལ་སྲིད ((entry < v_reg.start) || (entry >= v_reg.end)) {
        printf("Userland image entry point does not lie within userland image\n");
        ལོག 0;
    }

    load_paddr = find_load_paddr(load_paddr, v_reg.end - v_reg.start);
    assert(load_paddr);

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
        (void *)boot_state.ui_info.p_reg.start,
        boot_state.ui_info.p_reg.end - boot_state.ui_info.p_reg.start
    );
    elf_load(elf_file, boot_state.ui_info.pv_offset);

    ལོག load_paddr;
}

static BOOT_CODE bool_t try_boot_sys_node(cpu_id_t cpu_id)
{
    གལ་སྲིད (!map_kernel_window(
            boot_state.num_ioapic,
            boot_state.ioapic_paddr,
            boot_state.num_drhu,
            boot_state.drhu_list
        )) {
        ལོག false;
    }
    setCurrentVSpaceRoot(kpptr_to_paddr(X86_KERNEL_VSPACE_ROOT), 0);
    /* Sync up the compilers view of the world here to force the PD to actually
     * be set *right now* instead of delayed */
    asm ca_ncala("" ::: "memory");

#ifdef CONFIG_KERNEL_SKIM_WINDOW
    གལ་སྲིད (!map_skim_window((vptr_t)ki_skim_start, (vptr_t)ki_skim_end)) {
        ལོག false;
    }
#endif

    /* initialise the CPU */
    གལ་སྲིད (!init_cpu(config_set(CONFIG_IRQ_IOAPIC) ? 1 : 0)) {
        ལོག false;
    }

    /* initialise NDKS and kernel heap */
    གལ་སྲིད (!init_sys_state(
            cpu_id,
            &boot_state.mem_p_regs,
            boot_state.ui_info,
            /* parameters below not modelled in abstract specification */
            boot_state.num_drhu,
            boot_state.drhu_list,
            &boot_state.rmrr_list,
            &boot_state.acpi_rsdp,
            &boot_state.vbe_info,
            &boot_state.mb_mmap_info,
            &boot_state.fb_info
        )) {
        ལོག false;
    }

    ལོག true;
}

static BOOT_CODE bool_t add_mem_p_regs(p_region_t reg)
{
    གལ་སྲིད (reg.start == reg.end) {
        // Return true here if asked to add an empty region.
        // Some of the callers round down the end address to
        ལོག true;
    }
    གལ་སྲིད (reg.end > PADDR_TOP && reg.start >= PADDR_TOP) {
        /* Return true here as it's not an error for there to exist memory outside the kernel window,
         * we're just going to ignore it and leave it to be given out as device memory */
        ལོག true;
    }
    གལ་སྲིད (boot_state.mem_p_regs.count == MAX_NUM_FREEMEM_REG) {
        printf("Dropping memory region 0x%lx-0x%lx, try increasing MAX_NUM_FREEMEM_REG\n", reg.start, reg.end);
        ལོག false;
    }
    གལ་སྲིད (reg.end > PADDR_TOP) {
        assert(reg.start < PADDR_TOP);
        /* Clamp a region to the top of the kernel window if it extends beyond */
        reg.end = PADDR_TOP;
    }
    printf("Adding physical memory region 0x%lx-0x%lx\n", reg.start, reg.end);
    boot_state.mem_p_regs.list[boot_state.mem_p_regs.count] = reg;
    boot_state.mem_p_regs.count++;
    ལོག true;
}

/*
 * the code relies that the GRUB provides correct information
 * about the actual physical memory regions.
 */
static BOOT_CODE bool_t parse_mem_map(uint32_t mmap_length, uint32_t mmap_addr)
{
    multiboot_mmap_t *mmap = (multiboot_mmap_t *)((word_t)mmap_addr);
    printf("Parsing GRUB physical memory map\n");

    སྐབས ((word_t)mmap < (word_t)(mmap_addr + mmap_length)) {
        uint64_t mem_start = mmap->base_addr;
        uint64_t mem_length = mmap->length;
        uint32_t type = mmap->type;
        གལ་སྲིད (mem_start != (uint64_t)(word_t)mem_start) {
            printf("\tPhysical memory region not addressable\n");
        } དེ་མིན {
            printf("\tPhysical Memory Region from %lx size %lx type %d\n", (long)mem_start, (long)mem_length, type);
            གལ་སྲིད (type == MULTIBOOT_MMAP_USEABLE_TYPE && mem_start >= HIGHMEM_PADDR && mem_length >= BIT(PAGE_BITS)) {

                གལ་སྲིད (!add_mem_p_regs((p_region_t) {
                ROUND_UP(mem_start, PAGE_BITS), ROUND_DOWN(mem_start + mem_length, PAGE_BITS),
                })) {
                    ལོག false;
                }
            }
        }
        mmap++;
    }
    ལོག true;
}

static BOOT_CODE bool_t is_compiled_for_microarchitecture(void)
{
    word_t microarch_generation = 0;
    x86_cpu_identity_t *model_info = x86_cpuid_get_model_info();

    གལ་སྲིད (config_set(CONFIG_ARCH_X86_SKYLAKE)) {
        microarch_generation = 7;
    } དེ་མིན གལ་སྲིད (config_set(CONFIG_ARCH_X86_BROADWELL)) {
        microarch_generation = 6;
    } དེ་མིན གལ་སྲིད (config_set(CONFIG_ARCH_X86_HASWELL)) {
        microarch_generation = 5;
    } དེ་མིན གལ་སྲིད (config_set(CONFIG_ARCH_X86_IVY)) {
        microarch_generation = 4;
    } དེ་མིན གལ་སྲིད (config_set(CONFIG_ARCH_X86_SANDY)) {
        microarch_generation = 3;
    } དེ་མིན གལ་སྲིད (config_set(CONFIG_ARCH_X86_WESTMERE)) {
        microarch_generation = 2;
    } དེ་མིན གལ་སྲིད (config_set(CONFIG_ARCH_X86_NEHALEM)) {
        microarch_generation = 1;
    }

    switch (model_info->model) {
    pak_sa SKYLAKE_1_MODEL_ID:
    pak_sa SKYLAKE_2_MODEL_ID:
    pak_sa SKYLAKE_X_MODEL_ID:
        གལ་སྲིད (microarch_generation > 7) {
            ལོག false;
        }
        bha_nga;

    pak_sa BROADWELL_1_MODEL_ID:
    pak_sa BROADWELL_2_MODEL_ID:
    pak_sa BROADWELL_3_MODEL_ID:
    pak_sa BROADWELL_4_MODEL_ID:
    pak_sa BROADWELL_5_MODEL_ID:
        གལ་སྲིད (microarch_generation > 6) {
            ལོག false;
        }
        bha_nga;

    pak_sa HASWELL_1_MODEL_ID:
    pak_sa HASWELL_2_MODEL_ID:
    pak_sa HASWELL_3_MODEL_ID:
    pak_sa HASWELL_4_MODEL_ID:
        གལ་སྲིད (microarch_generation > 5) {
            ལོག false;
        }
        bha_nga;

    pak_sa IVY_BRIDGE_1_MODEL_ID:
    pak_sa IVY_BRIDGE_2_MODEL_ID:
    pak_sa IVY_BRIDGE_3_MODEL_ID:
        གལ་སྲིད (microarch_generation > 4) {
            ལོག false;
        }
        bha_nga;

    pak_sa SANDY_BRIDGE_1_MODEL_ID:
    pak_sa SANDY_BRIDGE_2_MODEL_ID:
        གལ་སྲིད (microarch_generation > 3) {
            ལོག false;
        }
        bha_nga;

    pak_sa WESTMERE_1_MODEL_ID:
    pak_sa WESTMERE_2_MODEL_ID:
    pak_sa WESTMERE_3_MODEL_ID:
        གལ་སྲིད (microarch_generation > 2) {
            ལོག false;
        }
        bha_nga;

    pak_sa NEHALEM_1_MODEL_ID:
    pak_sa NEHALEM_2_MODEL_ID:
    pak_sa NEHALEM_3_MODEL_ID:
        གལ་སྲིད (microarch_generation > 1) {
            ལོག false;
        }
        bha_nga;

    default:
        གལ་སྲིད (!config_set(CONFIG_ARCH_X86_GENERIC)) {
            ལོག false;
        }
    }

    ལོག true;
}

static BOOT_CODE bool_t try_boot_sys(void)
{
    paddr_t mods_end_paddr = boot_state.mods_end_paddr;
    p_region_t ui_p_regs;
    paddr_t load_paddr;

    boot_state.ki_p_reg = get_p_reg_kernel_img();

    གལ་སྲིད (!x86_cpuid_initialize()) {
        printf("Warning: Your x86 CPU has an unsupported vendor, '%s'.\n"
               "\tYour setup may not be able to competently run seL4 as "
               "intended.\n"
               "\tCurrently supported x86 vendors are AMD and Intel.\n",
               x86_cpuid_get_identity()->vendor_string);
    }

    གལ་སྲིད (!is_compiled_for_microarchitecture()) {
        printf("Warning: Your kernel was not compiled for the current microarchitecture.\n");
    }

    cpuid_007h_edx_t edx;
    edx.words[0] = x86_cpuid_edx(0x7, 0);
    /* see if we can definitively say whether or not we need the skim window by
     * checking whether the CPU is vulnerable to rogue data cache loads (rdcl) */
    གལ་སྲིད (cpuid_007h_edx_get_ia32_arch_cap_msr(edx)) {
        ia32_arch_capabilities_msr_t cap_msr;
        cap_msr.words[0] = x86_rdmsr(IA32_ARCH_CAPABILITIES_MSR);
        གལ་སྲིད (ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && config_set(CONFIG_KERNEL_SKIM_WINDOW)) {
            printf("CPU reports not vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is enabled. Performance is needlessly being impacted, consider disabling.\n");
        } དེ་མིན གལ་སྲིད (!ia32_arch_capabilities_msr_get_rdcl_no(cap_msr) && !config_set(CONFIG_KERNEL_SKIM_WINDOW)) {
            printf("CPU reports vulnerable to Rogue Data Cache Load (aka Meltdown https://meltdownattack.com) "
                   "yet SKIM window is *not* enabled. Please re-build with SKIM window enabled.");
            ལོག false;
        }
    } དེ་མིན {
        /* hardware doesn't tell us directly so guess based on CPU vendor */
        གལ་སྲིད (config_set(CONFIG_KERNEL_SKIM_WINDOW) && x86_cpuid_get_identity()->vendor == X86_VENDOR_AMD) {
            printf("SKIM window for mitigating Meltdown (https://www.meltdownattack.com) "
                   "not necessary for AMD and performance is being needlessly affected, "
                   "consider disabling\n");
        }
        གལ་སྲིད (!config_set(CONFIG_KERNEL_SKIM_WINDOW) && x86_cpuid_get_identity()->vendor == X86_VENDOR_INTEL) {
            printf("***WARNING*** SKIM window not enabled, this machine is probably vulnerable "
                   "to Meltdown (https://www.meltdownattack.com), consider enabling\n");
        }
    }

#ifdef ENABLE_SMP_SUPPORT
    /* copy boot code for APs to lower memory to run in real mode */
    གལ་སྲིད (!copy_boot_code_aps(boot_state.mem_lower)) {
        ལོག false;
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
    གལ་སྲིད (config_set(CONFIG_IRQ_IOAPIC)) {
        /* Disable the PIC so that it does not generate any interrupts. We need to
         * do this *before* we initialize the apic */
        pic_disable();
    }

    /* validate the ACPI table */
    གལ་སྲིད (!acpi_validate_rsdp(&boot_state.acpi_rsdp)) {
        ལོག false;
    }

    /* check if kernel configuration matches platform requirements */
    གལ་སྲིད (!acpi_fadt_scan(&boot_state.acpi_rsdp)) {
        ལོག false;
    }

    གལ་སྲིད (!config_set(CONFIG_IOMMU) || cmdline_opt.disable_iommu) {
        boot_state.num_drhu = 0;
    } དེ་མིན {
        /* query available IOMMUs from ACPI */
        acpi_dmar_scan(
            &boot_state.acpi_rsdp,
            boot_state.drhu_list,
            &boot_state.num_drhu,
            MAX_NUM_DRHU,
            &boot_state.rmrr_list
        );
    }

    /* query available CPUs from ACPI */
    boot_state.num_cpus = acpi_madt_scan(&boot_state.acpi_rsdp, boot_state.cpus, &boot_state.num_ioapic,
                                         boot_state.ioapic_paddr);
    གལ་སྲིད (boot_state.num_cpus == 0) {
        printf("No CPUs detected\n");
        ལོག false;
    }

    གལ་སྲིད (config_set(CONFIG_IRQ_IOAPIC)) {
        གལ་སྲིད (boot_state.num_ioapic == 0) {
            printf("No IOAPICs detected\n");
            ལོག false;
        }
    } དེ་མིན {
        གལ་སྲིད (boot_state.num_ioapic > 0) {
            printf("Detected %d IOAPICs, but configured to use PIC instead\n", boot_state.num_ioapic);
        }
    }

    mods_end_paddr = ROUND_UP(mods_end_paddr, PAGE_BITS);
    assert(mods_end_paddr > boot_state.ki_p_reg.end);

    printf("ELF-loading userland images from boot modules:\n");
    load_paddr = mods_end_paddr;

    load_paddr = load_boot_module(boot_state.boot_module_start, boot_state.boot_module_end, load_paddr);
    གལ་སྲིད (!load_paddr) {
        ལོག false;
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
    memcpy((void *)ui_p_regs.start, (void *)mods_end_paddr, ui_p_regs.end - ui_p_regs.start);

    /* adjust p_reg and pv_offset to final load address */
    boot_state.ui_info.p_reg.start -= mods_end_paddr - ui_p_regs.start;
    boot_state.ui_info.p_reg.end   -= mods_end_paddr - ui_p_regs.start;
    boot_state.ui_info.pv_offset   -= mods_end_paddr - ui_p_regs.start;

    /* ==== following code corresponds to abstract specification after "select" ==== */

    གལ་སྲིད (!platAddDevices()) {
        ལོག false;
    }

    /* Total number of cores we intend to boot */
    ksNumCPUs = boot_state.num_cpus;

    printf("Starting node #0 with APIC ID %lu\n", boot_state.cpus[0]);
    གལ་སྲིད (!try_boot_sys_node(boot_state.cpus[0])) {
        ལོག false;
    }

    གལ་སྲིད (config_set(CONFIG_IRQ_IOAPIC)) {
        ioapic_init(1, boot_state.cpus, boot_state.num_ioapic);
    }

    /* initialize BKL before booting up APs */
    SMP_COND_STATEMENT(clh_lock_init());
    SMP_COND_STATEMENT(start_boot_aps());

    /* grab BKL before leaving the kernel */
    NODE_LOCK_SYS;

    printf("Booting all finished, dropped to user space\n");

    ལོག true;
}

static BOOT_CODE bool_t try_boot_sys_mbi1(
    multiboot_info_t *mbi
)
{
    word_t i;
    multiboot_module_t *modules = (multiboot_module_t *)(word_t)mbi->part1.mod_list;

    cmdline_parse((const var_na *)(word_t)mbi->part1.cmdline, &cmdline_opt);

    གལ་སྲིད ((mbi->part1.flags & MULTIBOOT_INFO_MEM_FLAG) == 0) {
        printf("Boot loader did not provide information about physical memory size\n");
        ལོག false;
    }

    གལ་སྲིད (!(mbi->part1.flags & MULTIBOOT_INFO_MODS_FLAG)) {
        printf("Boot loader did not provide information about boot modules\n");
        ལོག false;
    }

    printf("Detected %d boot module(s):\n", mbi->part1.mod_count);

    གལ་སྲིད (mbi->part1.mod_count < 1) {
        printf("Expect at least one boot module (containing a userland image)\n");
        ལོག false;
    }

    དོན་ལུ (i = 0; i < mbi->part1.mod_count; i++) {
        printf(
            "  module #%ld: start=0x%x end=0x%x size=0x%x name='%s'\n",
            i,
            modules[i].start,
            modules[i].end,
            modules[i].end - modules[i].start,
            (var_na *)(long)modules[i].name
        );
        གལ་སྲིད ((sword_t)(modules[i].end - modules[i].start) <= 0) {
            printf("Invalid boot module size! Possible cause: boot module file not found by QEMU\n");
            ལོག false;
        }
        གལ་སྲིད (boot_state.mods_end_paddr < modules[i].end) {
            boot_state.mods_end_paddr = modules[i].end;
        }
    }

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.count = 0;
    གལ་སྲིད (mbi->part1.flags & MULTIBOOT_INFO_MMAP_FLAG) {
        གལ་སྲིད (!parse_mem_map(mbi->part2.mmap_length, mbi->part2.mmap_addr)) {
            ལོག false;
        }
        uint32_t multiboot_mmap_length = mbi->part2.mmap_length;
        གལ་སྲིད (multiboot_mmap_length > (SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * parimaa_na(seL4_X86_mb_mmap_t))) {
            printf("Warning: Multiboot has reported more memory map entries, %zd, "
                   "than the max amount that will be passed in the bootinfo, %d. "
                   "These extra regions will still be turned into untyped caps.",
                   multiboot_mmap_length / parimaa_na(seL4_X86_mb_mmap_t), SEL4_MULTIBOOT_MAX_MMAP_ENTRIES);
            multiboot_mmap_length = SEL4_MULTIBOOT_MAX_MMAP_ENTRIES * parimaa_na(seL4_X86_mb_mmap_t);
        }
        memcpy(&boot_state.mb_mmap_info.mmap, (void *)(word_t)mbi->part2.mmap_addr, multiboot_mmap_length);
        boot_state.mb_mmap_info.mmap_length = multiboot_mmap_length;
    } དེ་མིན {
        /* calculate memory the old way */
        p_region_t avail;
        avail.start = HIGHMEM_PADDR;
        avail.end = ROUND_DOWN(avail.start + (mbi->part1.mem_upper << 10), PAGE_BITS);
        གལ་སྲིད (!add_mem_p_regs(avail)) {
            ལོག false;
        }
    }

    /* copy VESA information from multiboot header */
    གལ་སྲིད ((mbi->part1.flags & MULTIBOOT_INFO_GRAPHICS_FLAG) == 0) {
        boot_state.vbe_info.vbeMode = -1;
        printf("Multiboot gave us no video information\n");
    } དེ་མིན {
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
    གལ་སྲིད (!acpi_init(&boot_state.acpi_rsdp)) {
        ལོག false;
    }

    ལོག true;
}

static BOOT_CODE bool_t try_boot_sys_mbi2(
    multiboot2_header_t *mbi2
)
{
    ཧྲིལ mod_count                  = 0;
    multiboot2_tag_t const *tag   = (multiboot2_tag_t *)(mbi2 + 1);
    multiboot2_tag_t const *tag_e = (multiboot2_tag_t *)((word_t)mbi2 + mbi2->total_size);

    /* initialize the memory. We track two kinds of memory regions. Physical memory
     * that we will use for the kernel, and physical memory regions that we must
     * not give to the user. Memory regions that must not be given to the user
     * include all the physical memory in the kernel window, but also includes any
     * important or kernel devices. */
    boot_state.mem_p_regs.count = 0;
    boot_state.mb_mmap_info.mmap_length = 0;
    boot_state.vbe_info.vbeMode = -1;

    སྐབས (tag < tag_e && tag->type != MULTIBOOT2_TAG_END) {
        word_t const behind_tag = (word_t)tag + parimaa_na(*tag);

        གལ་སྲིད (tag->type == MULTIBOOT2_TAG_CMDLINE) {
            var_na const *const cmdline = (var_na const * const)(behind_tag);
            cmdline_parse(cmdline, &cmdline_opt);
        } དེ་མིན གལ་སྲིད (tag->type == MULTIBOOT2_TAG_ACPI_1) {
            གལ་སྲིད (ACPI_V1_SIZE == tag->size - parimaa_na(*tag)) {
                memcpy(&boot_state.acpi_rsdp, (void *)behind_tag, tag->size - parimaa_na(*tag));
            }
        } དེ་མིན གལ་སྲིད (tag->type == MULTIBOOT2_TAG_ACPI_2) {
            གལ་སྲིད (parimaa_na(boot_state.acpi_rsdp) == tag->size - parimaa_na(*tag)) {
                memcpy(&boot_state.acpi_rsdp, (void *)behind_tag, parimaa_na(boot_state.acpi_rsdp));
            }
        } དེ་མིན གལ་སྲིད (tag->type == MULTIBOOT2_TAG_MODULE) {
            multiboot2_module_t const *module = (multiboot2_module_t const *)behind_tag;
            printf(
                "  module #%d: start=0x%x end=0x%x size=0x%x name='%s'\n",
                mod_count,
                module->start,
                module->end,
                module->end - module->start,
                module->string
            );

            གལ་སྲིད (mod_count == 0) {
                boot_state.boot_module_start = module->start;
                boot_state.boot_module_end = module->end;
            }

            mod_count ++;
            གལ་སྲིད ((sword_t)(module->end - module->start) <= 0) {
                printf("Invalid boot module size! Possible cause: boot module file not found\n");
                ལོག false;
            }
            གལ་སྲིད (boot_state.mods_end_paddr < module->end) {
                boot_state.mods_end_paddr = module->end;
            }
        } དེ་མིན གལ་སྲིད (tag->type == MULTIBOOT2_TAG_MEMORY) {
            multiboot2_memory_t const *s = (multiboot2_memory_t *)(behind_tag + 8);
            multiboot2_memory_t const *e = (multiboot2_memory_t *)((word_t)tag + tag->size);

            དོན་ལུ (multiboot2_memory_t const *m = s; m < e; m++) {
                གལ་སྲིད (!m->addr) {
                    boot_state.mem_lower = m->size;
                }

                printf("\tPhysical Memory Region from %llx size %llx type %u\n", m->addr, m->size, m->type);
                གལ་སྲིད (m->addr != (uint64_t)(word_t)m->addr) {
                    printf("\t\tPhysical memory region not addressable\n");
                    continue;
                }

                གལ་སྲིད (m->type == MULTIBOOT_MMAP_USEABLE_TYPE && m->addr >= HIGHMEM_PADDR) {
                    གལ་སྲིད (!add_mem_p_regs((p_region_t) {
                    m->addr, m->addr + m->size
                }))
                    ལོག false;
                }
            }
        } དེ་མིན གལ་སྲིད (tag->type == MULTIBOOT2_TAG_FB) {
            multiboot2_fb_t const *fb = (multiboot2_fb_t const *)behind_tag;
            printf("Got framebuffer info in multiboot2. Current video mode is at physical address=%llx pitch=%u resolution=%ux%u@%u type=%u\n",
                   fb->addr, fb->pitch, fb->width, fb->height, fb->bpp, fb->type);
            boot_state.fb_info = *fb;
        }

        tag = (multiboot2_tag_t const *)((word_t)tag + ROUND_UP(tag->size, 3));
    }

    printf("Detected %d boot module(s):\n", mod_count);

    གལ་སྲིད (mod_count < 1) {
        printf("Expect at least one boot module (containing a userland image)\n");
        ལོག false;
    }

    ལོག true;
}

BOOT_CODE VISIBLE void boot_sys(
    unsigned long multiboot_magic,
    void *mbi)
{
    bool_t result = false;

    གལ་སྲིད (multiboot_magic == MULTIBOOT_MAGIC) {
        result = try_boot_sys_mbi1(mbi);
    } དེ་མིན གལ་སྲིད (multiboot_magic == MULTIBOOT2_MAGIC) {
        result = try_boot_sys_mbi2(mbi);
    } དེ་མིན {
        printf("Boot loader is not multiboot 1 or 2 compliant %lx\n", multiboot_magic);
    }

    གལ་སྲིད (result) {
        result = try_boot_sys();
    }

    གལ་སྲིད (!result) {
        fail("boot_sys failed for some reason :(\n");
    }

    ARCH_NODE_STATE(x86KScurInterrupt) = int_invalid;
    ARCH_NODE_STATE(x86KSPendingInterrupt) = int_invalid;

#ifdef CONFIG_KERNEL_MCS
    NODE_STATE(ksCurTime) = getCurrentTime();
    NODE_STATE(ksConsumed) = 0;
#endif

    schedule();
    activateThread();
}

