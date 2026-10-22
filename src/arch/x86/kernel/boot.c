/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <kernel/boot.h>
#include <machine/io.h>
#include <model/statedata.h>
#include <object/interrupt.h>
#include <arch/object/interrupt.h>
#include <arch/machine.h>
#include <arch/kernel/apic.h>
#include <arch/kernel/boot.h>
#include <arch/kernel/boot_sys.h>
#include <arch/kernel/vspace.h>
#include <machine/fpu.h>
#include <arch/machine/timer.h>
#include <arch/object/ioport.h>
#include <linker.h>
#include <util.h>

#include <plat/machine/intel-vtd.h>

#define MAX_RESERVED 1
BOOT_BSS static region_t reserved[MAX_RESERVED];

/* functions exactly corresponding to abstract specification */

BOOT_CODE static void init_irqs(cap_t root_cnode_cap)
{
    irq_t i;

    за (i = 0; i <= maxIRQ; i++) {
        ако (i == irq_timer) {
            setIRQState(IRQTimer, i);
#ifdef ENABLE_SMP_SUPPORT
        } инако ако (i == irq_remote_call_ipi || i == irq_reschedule_ipi) {
            setIRQState(IRQIPI, i);
#endif /* ENABLE_SMP_SUPPORT */
#ifdef CONFIG_IOMMU
        } инако ако (i == irq_iommu) {
            setIRQState(IRQReserved, i);
#endif
        } инако ако (i == 2 && config_set(CONFIG_IRQ_PIC)) {
            /* cascaded legacy PIC */
            setIRQState(IRQReserved, i);
        } инако ако (i >= irq_isa_min && i <= irq_isa_max) {
            ако (config_set(CONFIG_IRQ_PIC)) {
                setIRQState(IRQInactive, i);
            } инако {
                setIRQState(IRQReserved, i);
            }
        } инако ако (i >= irq_user_min && i <= irq_user_max) {
            ако (config_set(CONFIG_IRQ_IOAPIC)) {
                setIRQState(IRQInactive, i);
            } инако {
                setIRQState(IRQReserved, i);
            }
        } инако {
            setIRQState(IRQReserved, i);
        }
    }
    Arch_irqStateInit();
    /* provide the IRQ control cap */
    write_slot(SLOT_PTR(pptr_of_cap(root_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

BOOT_CODE static bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          v_region_t it_v_reg,
                                          mem_p_regs_t *mem_p_regs,
                                          word_t extra_bi_size_bits)
{
    // Extend the reserved region down to include the base of the kernel image.
    // KERNEL_ELF_PADDR_BASE is the lowest physical load address used
    // in the x86 linker script.
    ui_p_reg.start = KERNEL_ELF_PADDR_BASE;
    reserved[0] = paddr_to_pptr_reg(ui_p_reg);
    врати init_freemem(mem_p_regs->count, mem_p_regs->list, MAX_RESERVED,
                        reserved, it_v_reg, extra_bi_size_bits);
}

/* This function initialises a node's kernel state. It does NOT initialise the CPU. */

BOOT_CODE bool_t init_sys_state(
    cpu_id_t      cpu_id,
    mem_p_regs_t  *mem_p_regs,
    ui_info_t     ui_info,
    /* parameters below not modeled in abstract specification */
    uint32_t      num_drhu,
    paddr_t      *drhu_list,
    acpi_rmrr_list_t *rmrr_list,
    acpi_rsdp_t      *acpi_rsdp,
    seL4_X86_BootInfo_VBE *vbe,
    seL4_X86_BootInfo_mmap_t *mb_mmap,
    seL4_X86_BootInfo_fb_t *fb_info
)
{
    cap_t         root_cnode_cap;
    vptr_t        extra_bi_frame_vptr;
    vptr_t        bi_frame_vptr;
    vptr_t        ipcbuf_vptr;
    cap_t         it_vspace_cap;
    cap_t         it_ap_cap;
    cap_t         ipcbuf_cap;
    word_t        extra_bi_size = 0;
    pptr_t        extra_bi_offset = 0;
    uint32_t      tsc_freq;
    create_frames_of_region_ret_t create_frames_ret;
    create_frames_of_region_ret_t extra_bi_ret;

    /* convert from physical addresses to kernel pptrs */
    region_t ui_reg             = paddr_to_pptr_reg(ui_info.p_reg);

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg;
    v_region_t it_v_reg;
    ui_v_reg.start = ui_info.p_reg.start - ui_info.pv_offset;
    ui_v_reg.end   = ui_info.p_reg.end   - ui_info.pv_offset;

    ipcbuf_vptr = ui_v_reg.end;
    bi_frame_vptr = ipcbuf_vptr + BIT(PAGE_BITS);
    extra_bi_frame_vptr = bi_frame_vptr + BIT(seL4_BootInfoFrameBits);

    ако (vbe->vbeMode != -1) {
        extra_bi_size += sizeof(seL4_X86_BootInfo_VBE);
    }
    ако (acpi_rsdp) {
        extra_bi_size += sizeof(seL4_BootInfoHeader) + sizeof(*acpi_rsdp);
    }
    ако (fb_info && fb_info->addr) {
        extra_bi_size += sizeof(seL4_BootInfoHeader) + sizeof(*fb_info);
    }

    word_t mb_mmap_size = sizeof(seL4_X86_BootInfo_mmap_t);
    extra_bi_size += mb_mmap_size;

    // room for tsc frequency
    extra_bi_size += sizeof(seL4_BootInfoHeader) + 4;
    word_t extra_bi_size_bits = calculate_extra_bi_size_bits(extra_bi_size);

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    it_v_reg.start = ui_v_reg.start;
    it_v_reg.end = ROUND_UP(extra_bi_frame_vptr + (extra_bi_size_bits > 0 ? BIT(extra_bi_size_bits) : 0), PAGE_BITS);
#ifdef CONFIG_IOMMU
    /* calculate the number of io pts before initialising memory */
    ако (!vtd_init_num_iopts(num_drhu)) {
        врати false;
    }
#endif /* CONFIG_IOMMU */

    ако (!arch_init_freemem(ui_info.p_reg, it_v_reg, mem_p_regs, extra_bi_size_bits)) {
        printf("ERROR: free memory management initialization failed\n");
        врати false;
    }

    /* create the root cnode */
    root_cnode_cap = create_root_cnode();

    /* create the IO port cap */
    write_slot(
        SLOT_PTR(pptr_of_cap(root_cnode_cap), seL4_CapIOPortControl),
        cap_io_port_control_cap_new()
    );

    /* create the cap for managing thread domains */
    create_domain_cap(root_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(root_cnode_cap);

    tsc_freq = tsc_init();

    /* populate the bootinfo frame */
    populate_bi_frame(0, ksNumCPUs, ipcbuf_vptr, extra_bi_size);
    region_t extra_bi_region = {
        .start = rootserver.extra_bi,
        .end = rootserver.extra_bi + (extra_bi_size_bits > 0 ? BIT(extra_bi_size_bits) : 0)
    };

    /* populate vbe info block */
    ако (vbe->vbeMode != -1) {
        vbe->header.id = SEL4_BOOTINFO_HEADER_X86_VBE;
        vbe->header.len = sizeof(seL4_X86_BootInfo_VBE);
        memcpy((void *)(rootserver.extra_bi + extra_bi_offset), vbe, sizeof(seL4_X86_BootInfo_VBE));
        extra_bi_offset += sizeof(seL4_X86_BootInfo_VBE);
    }

    /* populate acpi rsdp block */
    ако (acpi_rsdp) {
        seL4_BootInfoHeader header;
        header.id = SEL4_BOOTINFO_HEADER_X86_ACPI_RSDP;
        header.len = sizeof(header) + sizeof(*acpi_rsdp);
        *(seL4_BootInfoHeader *)(rootserver.extra_bi + extra_bi_offset) = header;
        extra_bi_offset += sizeof(header);
        memcpy((void *)(rootserver.extra_bi + extra_bi_offset), acpi_rsdp, sizeof(*acpi_rsdp));
        extra_bi_offset += sizeof(*acpi_rsdp);
    }

    /* populate framebuffer information block */
    ако (fb_info && fb_info->addr) {
        seL4_BootInfoHeader header;
        header.id = SEL4_BOOTINFO_HEADER_X86_FRAMEBUFFER;
        header.len = sizeof(header) + sizeof(*fb_info);
        *(seL4_BootInfoHeader *)(rootserver.extra_bi + extra_bi_offset) = header;
        extra_bi_offset += sizeof(header);
        memcpy((void *)(rootserver.extra_bi + extra_bi_offset), fb_info, sizeof(*fb_info));
        extra_bi_offset += sizeof(*fb_info);
    }

    /* populate multiboot mmap block */
    mb_mmap->header.id = SEL4_BOOTINFO_HEADER_X86_MBMMAP;
    mb_mmap->header.len = mb_mmap_size;
    memcpy((void *)(rootserver.extra_bi + extra_bi_offset), mb_mmap, mb_mmap_size);
    extra_bi_offset += mb_mmap_size;

    /* populate tsc frequency block */
    {
        seL4_BootInfoHeader header;
        header.id = SEL4_BOOTINFO_HEADER_X86_TSC_FREQ;
        header.len = sizeof(header) + 4;
        *(seL4_BootInfoHeader *)(extra_bi_region.start + extra_bi_offset) = header;
        extra_bi_offset += sizeof(header);
        *(uint32_t *)(extra_bi_region.start + extra_bi_offset) = tsc_freq;
        extra_bi_offset += 4;
    }

#ifdef CONFIG_KERNEL_MCS
    /* set up sched control for each core */
    init_sched_control(root_cnode_cap, CONFIG_MAX_NUM_NODES);
#endif

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_vspace_cap = create_it_address_space(root_cnode_cap, it_v_reg);
    ако (cap_get_capType(it_vspace_cap) == cap_null_cap) {
        врати false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_frame_cap(
        root_cnode_cap,
        it_vspace_cap,
        bi_frame_vptr
    );

    /* create and map extra bootinfo region */
    extra_bi_ret =
        create_frames_of_region(
            root_cnode_cap,
            it_vspace_cap,
            extra_bi_region,
            true,
            pptr_to_paddr((void *)(extra_bi_region.start - extra_bi_frame_vptr))
        );
    ако (!extra_bi_ret.success) {
        врати false;
    }
    ndks_boot.bi_frame->extraBIPages = extra_bi_ret.region;

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_frame_cap(root_cnode_cap, it_vspace_cap, ipcbuf_vptr);
    ако (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        врати false;
    }

    /* create all userland image frames */
    create_frames_ret =
        create_frames_of_region(
            root_cnode_cap,
            it_vspace_cap,
            ui_reg,
            true,
            ui_info.pv_offset
        );
    ако (!create_frames_ret.success) {
        врати false;
    }
    ndks_boot.bi_frame->userImageFrames = create_frames_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(root_cnode_cap);
    ако (cap_get_capType(it_ap_cap) == cap_null_cap) {
        врати false;
    }
    write_it_asid_pool(it_ap_cap, it_vspace_cap);

#ifdef CONFIG_KERNEL_MCS
    NODE_STATE(ksCurTime) = getCurrentTime();
#endif

    /* create the idle thread */
    create_idle_thread();

    /* copy i387 FPU initial state from FPU */
    saveFpuState(NODE_STATE(ksIdleThread));
    x86KSnullFpuState = NODE_STATE(ksIdleThread)->tcbArch.tcbContext.fpuState;
    /* Check that we can load it: */
    loadFpuState(NODE_STATE(ksIdleThread));

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(root_cnode_cap,
                                           it_vspace_cap,
                                           ui_info.v_entry,
                                           bi_frame_vptr,
                                           ipcbuf_vptr,
                                           ipcbuf_cap);
    ако (initial == NULL) {
        врати false;
    }
    init_core_state(initial);

#ifdef CONFIG_IOMMU
    /* initialise VTD-related data structures and the IOMMUs */
    ако (!vtd_init(cpu_id, rmrr_list)) {
        врати false;
    }

    /* write number of IOMMU PT levels into bootinfo */
    ndks_boot.bi_frame->numIOPTLevels = x86KSnumIOPTLevels;

    /* write IOSpace master cap */
    write_slot(SLOT_PTR(pptr_of_cap(root_cnode_cap), seL4_CapIOSpace), master_iospace_cap());
#инако
    ndks_boot.bi_frame->numIOPTLevels = -1;
#endif

    /* create all of the untypeds. Both devices and kernel window memory */
    ако (!create_untypeds(root_cnode_cap)) {
        врати false;
    }

    /* finalise the bootinfo frame */
    bi_finalise();

    врати true;
}

/* This function initialises the CPU. It does NOT initialise any kernel state. */

BOOT_CODE bool_t init_cpu(
    bool_t   mask_legacy_irqs
)
{
    /* initialise virtual-memory-related data structures */
    ако (!init_vm_state()) {
        врати false;
    }

    /* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */
    init_dtrs();

    ако (config_set(CONFIG_SYSENTER)) {
        /* initialise MSRs (needs an initialised TSS) */
        init_sysenter_msrs();
    } инако ако (config_set(CONFIG_SYSCALL)) {
        init_syscall_msrs();
    } инако {
        врати false;
    }

    /* setup additional PAT MSR */
    ако (!init_pat_msr()) {
        врати false;
    }

    /* enable the Write Protect bit in cr0. This prevents the kernel from writing to
     * read only memory, which we shouldn't do under correct execution */
    write_cr0(read_cr0() | CR0_WRITE_PROTECT);

    /* check for SMAP and SMEP and enable */
    cpuid_007h_ebx_t ebx_007;
    ebx_007.words[0] = x86_cpuid_ebx(0x7, 0);
    ако (cpuid_007h_ebx_get_smap(ebx_007)) {
        /* if we have user stack trace enabled or dangerous code injection then we cannot
         * enable this as SMAP will make them fault. */
        ако (!config_set(CONFIG_PRINTING) && !config_set(CONFIG_DANGEROUS_CODE_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMAP);
        }
    }
    ако (cpuid_007h_ebx_get_smep(ebx_007)) {
        /* similar to smap we cannot enable smep if using dangerous code injection. it
         * does not affect stack trace printing though */
        ако (!config_set(CONFIG_DANGEROUS_CODE_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMEP);
        }
    }

    ако (!init_ibrs()) {
        врати false;
    }

#ifdef CONFIG_HARDWARE_DEBUG_API
    /* Initialize hardware breakpoints */
    Arch_initHardwareBreakpoints();
#endif

    /* initialise floating-point unit */
    ако (!Arch_initFpu()) {
        врати false;
    }

    /* initialise local APIC */
    ако (!apic_init(mask_legacy_irqs)) {
        врати false;
    }

#ifdef CONFIG_DEBUG_DISABLE_PREFETCHERS
    ако (!disablePrefetchers()) {
        врати false;
    }
#endif

    ако (config_set(CONFIG_EXPORT_PMC_USER)) {
        enablePMCUser();
    }

#ifdef CONFIG_VTX
    /* initialise Intel VT-x extensions */
    ако (!vtx_init()) {
        врати false;
    }
#endif

    врати true;
}
