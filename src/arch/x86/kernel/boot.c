/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <kernel/boot.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <object/interrupt.h>
#अन्तर्भाव <arch/object/interrupt.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/kernel/apic.h>
#अन्तर्भाव <arch/kernel/boot.h>
#अन्तर्भाव <arch/kernel/boot_sys.h>
#अन्तर्भाव <arch/kernel/vspace.h>
#अन्तर्भाव <machine/fpu.h>
#अन्तर्भाव <arch/machine/timer.h>
#अन्तर्भाव <arch/object/ioport.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <util.h>

#अन्तर्भाव <plat/machine/intel-vtd.h>

#define MAX_RESERVED 1
BOOT_BSS स्थिर region_t reserved[MAX_RESERVED];

/* functions exactly corresponding to abstract specification */

BOOT_CODE स्थिर शून्य init_irqs(cap_t मूलम्_cnode_cap)
{
    irq_t i;

    प्रति (i = 0; i <= maxIRQ; i++) {
        जइ (i == irq_timer) {
            setIRQState(IRQTimer, i);
#ifdef ENABLE_SMP_SUPPORT
        } अण्णहा जइ (i == irq_remote_call_ipi || i == irq_reschedule_ipi) {
            setIRQState(IRQIPI, i);
#endif /* ENABLE_SMP_SUPPORT */
#ifdef CONFIG_IOMMU
        } अण्णहा जइ (i == irq_iommu) {
            setIRQState(IRQReserved, i);
#endif
        } अण्णहा जइ (i == 2 && config_set(CONFIG_IRQ_PIC)) {
            /* cascaded legacy PIC */
            setIRQState(IRQReserved, i);
        } अण्णहा जइ (i >= irq_isa_min && i <= irq_isa_max) {
            जइ (config_set(CONFIG_IRQ_PIC)) {
                setIRQState(IRQInactive, i);
            } अण्णहा {
                setIRQState(IRQReserved, i);
            }
        } अण्णहा जइ (i >= irq_user_min && i <= irq_user_max) {
            जइ (config_set(CONFIG_IRQ_IOAPIC)) {
                setIRQState(IRQInactive, i);
            } अण्णहा {
                setIRQState(IRQReserved, i);
            }
        } अण्णहा {
            setIRQState(IRQReserved, i);
        }
    }
    Arch_irqStateInit();
    /* provide the IRQ control cap */
    write_slot(SLOT_PTR(pptr_of_cap(मूलम्_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

BOOT_CODE स्थिर bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          v_region_t it_v_reg,
                                          mem_p_regs_t *mem_p_regs,
                                          word_t extra_bi_size_bits)
{
    // Extend the reserved region down to include the base of the kernel image.
    // KERNEL_ELF_PADDR_BASE is the lowest physical load address used
    // in the x86 linker script.
    ui_p_reg.start = KERNEL_ELF_PADDR_BASE;
    reserved[0] = paddr_to_pptr_reg(ui_p_reg);
    प्रत्यागम init_freemem(mem_p_regs->गणनम्, mem_p_regs->list, MAX_RESERVED,
                        reserved, it_v_reg, extra_bi_size_bits);
}

/* This function initialises a node's kernel state. It does NOT initialise the CPU. */

BOOT_CODE bool_t init_sys_state(
    cpu_id_t      cpu_id,
    mem_p_regs_t  *mem_p_regs,
    ui_info_t     ui_info,
    /* parameters below not modeled in abstract specification */
    uint32_t      अङ्कः_drhu,
    paddr_t      *drhu_list,
    acpi_rmrr_list_t *rmrr_list,
    acpi_rsdp_t      *acpi_rsdp,
    seL4_X86_BootInfo_VBE *vbe,
    seL4_X86_BootInfo_mmap_t *mb_mmap,
    seL4_X86_BootInfo_fb_t *fb_info
)
{
    cap_t         मूलम्_cnode_cap;
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

    जइ (vbe->vbeMode != -1) {
        extra_bi_size += परिमाण(seL4_X86_BootInfo_VBE);
    }
    जइ (acpi_rsdp) {
        extra_bi_size += परिमाण(seL4_BootInfoHeader) + परिमाण(*acpi_rsdp);
    }
    जइ (fb_info && fb_info->addr) {
        extra_bi_size += परिमाण(seL4_BootInfoHeader) + परिमाण(*fb_info);
    }

    word_t mb_mmap_size = परिमाण(seL4_X86_BootInfo_mmap_t);
    extra_bi_size += mb_mmap_size;

    // room for tsc frequency
    extra_bi_size += परिमाण(seL4_BootInfoHeader) + 4;
    word_t extra_bi_size_bits = calculate_extra_bi_size_bits(extra_bi_size);

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    it_v_reg.start = ui_v_reg.start;
    it_v_reg.end = ROUND_UP(extra_bi_frame_vptr + (extra_bi_size_bits > 0 ? BIT(extra_bi_size_bits) : 0), PAGE_BITS);
#ifdef CONFIG_IOMMU
    /* calculate the number of io pts before initialising memory */
    जइ (!vtd_init_अङ्कः_iopts(अङ्कः_drhu)) {
        प्रत्यागम false;
    }
#endif /* CONFIG_IOMMU */

    जइ (!arch_init_freemem(ui_info.p_reg, it_v_reg, mem_p_regs, extra_bi_size_bits)) {
        printf("ERROR: free memory management initialization failed\n");
        प्रत्यागम false;
    }

    /* create the root cnode */
    मूलम्_cnode_cap = create_मूलम्_cnode();

    /* create the IO port cap */
    write_slot(
        SLOT_PTR(pptr_of_cap(मूलम्_cnode_cap), seL4_CapIOPortControl),
        cap_io_port_control_cap_new()
    );

    /* create the cap for managing thread domains */
    create_domain_cap(मूलम्_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(मूलम्_cnode_cap);

    tsc_freq = tsc_init();

    /* populate the bootinfo frame */
    populate_bi_frame(0, ks_अङ्कः_CPUs, ipcbuf_vptr, extra_bi_size);
    region_t extra_bi_region = {
        .start = rootserver.extra_bi,
        .end = rootserver.extra_bi + (extra_bi_size_bits > 0 ? BIT(extra_bi_size_bits) : 0)
    };

    /* populate vbe info block */
    जइ (vbe->vbeMode != -1) {
        vbe->header.id = SEL4_BOOTINFO_HEADER_X86_VBE;
        vbe->header.len = परिमाण(seL4_X86_BootInfo_VBE);
        memcpy((शून्य *)(rootserver.extra_bi + extra_bi_offset), vbe, परिमाण(seL4_X86_BootInfo_VBE));
        extra_bi_offset += परिमाण(seL4_X86_BootInfo_VBE);
    }

    /* populate acpi rsdp block */
    जइ (acpi_rsdp) {
        seL4_BootInfoHeader header;
        header.id = SEL4_BOOTINFO_HEADER_X86_ACPI_RSDP;
        header.len = परिमाण(header) + परिमाण(*acpi_rsdp);
        *(seL4_BootInfoHeader *)(rootserver.extra_bi + extra_bi_offset) = header;
        extra_bi_offset += परिमाण(header);
        memcpy((शून्य *)(rootserver.extra_bi + extra_bi_offset), acpi_rsdp, परिमाण(*acpi_rsdp));
        extra_bi_offset += परिमाण(*acpi_rsdp);
    }

    /* populate framebuffer information block */
    जइ (fb_info && fb_info->addr) {
        seL4_BootInfoHeader header;
        header.id = SEL4_BOOTINFO_HEADER_X86_FRAMEBUFFER;
        header.len = परिमाण(header) + परिमाण(*fb_info);
        *(seL4_BootInfoHeader *)(rootserver.extra_bi + extra_bi_offset) = header;
        extra_bi_offset += परिमाण(header);
        memcpy((शून्य *)(rootserver.extra_bi + extra_bi_offset), fb_info, परिमाण(*fb_info));
        extra_bi_offset += परिमाण(*fb_info);
    }

    /* populate multiboot mmap block */
    mb_mmap->header.id = SEL4_BOOTINFO_HEADER_X86_MBMMAP;
    mb_mmap->header.len = mb_mmap_size;
    memcpy((शून्य *)(rootserver.extra_bi + extra_bi_offset), mb_mmap, mb_mmap_size);
    extra_bi_offset += mb_mmap_size;

    /* populate tsc frequency block */
    {
        seL4_BootInfoHeader header;
        header.id = SEL4_BOOTINFO_HEADER_X86_TSC_FREQ;
        header.len = परिमाण(header) + 4;
        *(seL4_BootInfoHeader *)(extra_bi_region.start + extra_bi_offset) = header;
        extra_bi_offset += परिमाण(header);
        *(uint32_t *)(extra_bi_region.start + extra_bi_offset) = tsc_freq;
        extra_bi_offset += 4;
    }

#ifdef CONFIG_KERNEL_MCS
    /* set up sched control for each core */
    init_sched_control(मूलम्_cnode_cap, CONFIG_MAX_अङ्कः_NODES);
#endif

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_vspace_cap = create_it_address_space(मूलम्_cnode_cap, it_v_reg);
    जइ (cap_get_capType(it_vspace_cap) == cap_null_cap) {
        प्रत्यागम false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_frame_cap(
        मूलम्_cnode_cap,
        it_vspace_cap,
        bi_frame_vptr
    );

    /* create and map extra bootinfo region */
    extra_bi_ret =
        create_frames_of_region(
            मूलम्_cnode_cap,
            it_vspace_cap,
            extra_bi_region,
            true,
            pptr_to_paddr((शून्य *)(extra_bi_region.start - extra_bi_frame_vptr))
        );
    जइ (!extra_bi_ret.success) {
        प्रत्यागम false;
    }
    ndks_boot.bi_frame->extraBIPages = extra_bi_ret.region;

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_frame_cap(मूलम्_cnode_cap, it_vspace_cap, ipcbuf_vptr);
    जइ (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        प्रत्यागम false;
    }

    /* create all userland image frames */
    create_frames_ret =
        create_frames_of_region(
            मूलम्_cnode_cap,
            it_vspace_cap,
            ui_reg,
            true,
            ui_info.pv_offset
        );
    जइ (!create_frames_ret.success) {
        प्रत्यागम false;
    }
    ndks_boot.bi_frame->userImageFrames = create_frames_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(मूलम्_cnode_cap);
    जइ (cap_get_capType(it_ap_cap) == cap_null_cap) {
        प्रत्यागम false;
    }
    write_it_asid_pool(it_ap_cap, it_vspace_cap);

#ifdef CONFIG_KERNEL_MCS
    ग्रन्थिः_STATE(ksCurTime) = getCurrentTime();
#endif

    /* create the idle thread */
    create_idle_thread();

    /* copy i387 FPU initial state from FPU */
    saveFpuState(ग्रन्थिः_STATE(ksIdleThread));
    x86KSnullFpuState = ग्रन्थिः_STATE(ksIdleThread)->tcbArch.tcbContext.fpuState;
    /* Check that we can load it: */
    loadFpuState(ग्रन्थिः_STATE(ksIdleThread));

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(मूलम्_cnode_cap,
                                           it_vspace_cap,
                                           ui_info.v_entry,
                                           bi_frame_vptr,
                                           ipcbuf_vptr,
                                           ipcbuf_cap);
    जइ (initial == रिक्त) {
        प्रत्यागम false;
    }
    init_core_state(initial);

#ifdef CONFIG_IOMMU
    /* initialise VTD-related data structures and the IOMMUs */
    जइ (!vtd_init(cpu_id, rmrr_list)) {
        प्रत्यागम false;
    }

    /* write number of IOMMU PT levels into bootinfo */
    ndks_boot.bi_frame->अङ्कः_IOPTLevels = x86KSnumIOPTLevels;

    /* write IOSpace master cap */
    write_slot(SLOT_PTR(pptr_of_cap(मूलम्_cnode_cap), seL4_CapIOSpace), master_iospace_cap());
#अण्णहा
    ndks_boot.bi_frame->अङ्कः_IOPTLevels = -1;
#endif

    /* create all of the untypeds. Both devices and kernel window memory */
    जइ (!create_untypeds(मूलम्_cnode_cap)) {
        प्रत्यागम false;
    }

    /* finalise the bootinfo frame */
    bi_finalise();

    प्रत्यागम true;
}

/* This function initialises the CPU. It does NOT initialise any kernel state. */

BOOT_CODE bool_t init_cpu(
    bool_t   mask_legacy_irqs
)
{
    /* initialise virtual-memory-related data structures */
    जइ (!init_vm_state()) {
        प्रत्यागम false;
    }

    /* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */
    init_dtrs();

    जइ (config_set(CONFIG_SYSENTER)) {
        /* initialise MSRs (needs an initialised TSS) */
        init_sysenter_msrs();
    } अण्णहा जइ (config_set(CONFIG_SYSCALL)) {
        init_syscall_msrs();
    } अण्णहा {
        प्रत्यागम false;
    }

    /* setup additional PAT MSR */
    जइ (!init_pat_msr()) {
        प्रत्यागम false;
    }

    /* enable the Write Protect bit in cr0. This prevents the kernel from writing to
     * read only memory, which we shouldn't do under correct execution */
    write_cr0(read_cr0() | CR0_WRITE_PROTECT);

    /* check for SMAP and SMEP and enable */
    cpuid_007h_ebx_t ebx_007;
    ebx_007.शब्दाः[0] = x86_cpuid_ebx(0x7, 0);
    जइ (cpuid_007h_ebx_get_smap(ebx_007)) {
        /* if we have user stack trace enabled or dangerous code injection then we cannot
         * enable this as SMAP will make them fault. */
        जइ (!config_set(CONFIG_PRINTING) && !config_set(CONFIG_DANGEROUS_CODE_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMAP);
        }
    }
    जइ (cpuid_007h_ebx_get_smep(ebx_007)) {
        /* similar to smap we cannot enable smep if using dangerous code injection. it
         * does not affect stack trace printing though */
        जइ (!config_set(CONFIG_DANGEROUS_CODE_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMEP);
        }
    }

    जइ (!init_ibrs()) {
        प्रत्यागम false;
    }

#ifdef CONFIG_HARDWARE_DEBUG_API
    /* Initialize hardware breakpoints */
    Arch_initHardwareBreakpoints();
#endif

    /* initialise floating-point unit */
    जइ (!Arch_initFpu()) {
        प्रत्यागम false;
    }

    /* initialise local APIC */
    जइ (!apic_init(mask_legacy_irqs)) {
        प्रत्यागम false;
    }

#ifdef CONFIG_DEBUG_DISABLE_PREFETCHERS
    जइ (!disablePrefetchers()) {
        प्रत्यागम false;
    }
#endif

    जइ (config_set(CONFIG_EXPORT_PMC_USER)) {
        enablePMCUser();
    }

#ifdef CONFIG_VTX
    /* initialise Intel VT-x extensions */
    जइ (!vtx_init()) {
        प्रत्यागम false;
    }
#endif

    प्रत्यागम true;
}
