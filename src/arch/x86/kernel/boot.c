/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <kernel/boot.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <model/statedata.h>
#അന്തര്ഭാവ <object/interrupt.h>
#അന്തര്ഭാവ <arch/object/interrupt.h>
#അന്തര്ഭാവ <arch/machine.h>
#അന്തര്ഭാവ <arch/kernel/apic.h>
#അന്തര്ഭാവ <arch/kernel/boot.h>
#അന്തര്ഭാവ <arch/kernel/boot_sys.h>
#അന്തര്ഭാവ <arch/kernel/vspace.h>
#അന്തര്ഭാവ <machine/fpu.h>
#അന്തര്ഭാവ <arch/machine/timer.h>
#അന്തര്ഭാവ <arch/object/ioport.h>
#അന്തര്ഭാവ <linker.h>
#അന്തര്ഭാവ <util.h>

#അന്തര്ഭാവ <plat/machine/intel-vtd.h>

#define MAX_RESERVED 1
BOOT_BSS സ്ഥിതം region_t reserved[MAX_RESERVED];

/* functions exactly corresponding to abstract specification */

BOOT_CODE സ്ഥിതം ശൂന്യം init_irqs(cap_t വേര്_cnode_cap)
{
    irq_t i;

    വേണ്ടി (i = 0; i <= maxIRQ; i++) {
        എങ്കിൽ (i == irq_timer) {
            setIRQState(IRQTimer, i);
#ifdef ENABLE_SMP_SUPPORT
        } അല്ലെങ്കിൽ എങ്കിൽ (i == irq_remote_call_ipi || i == irq_reschedule_ipi) {
            setIRQState(IRQIPI, i);
#endif /* ENABLE_SMP_SUPPORT */
#ifdef CONFIG_IOMMU
        } അല്ലെങ്കിൽ എങ്കിൽ (i == irq_iommu) {
            setIRQState(IRQReserved, i);
#endif
        } അല്ലെങ്കിൽ എങ്കിൽ (i == 2 && config_set(CONFIG_IRQ_PIC)) {
            /* cascaded legacy PIC */
            setIRQState(IRQReserved, i);
        } അല്ലെങ്കിൽ എങ്കിൽ (i >= irq_isa_min && i <= irq_isa_max) {
            എങ്കിൽ (config_set(CONFIG_IRQ_PIC)) {
                setIRQState(IRQInactive, i);
            } അല്ലെങ്കിൽ {
                setIRQState(IRQReserved, i);
            }
        } അല്ലെങ്കിൽ എങ്കിൽ (i >= irq_user_min && i <= irq_user_max) {
            എങ്കിൽ (config_set(CONFIG_IRQ_IOAPIC)) {
                setIRQState(IRQInactive, i);
            } അല്ലെങ്കിൽ {
                setIRQState(IRQReserved, i);
            }
        } അല്ലെങ്കിൽ {
            setIRQState(IRQReserved, i);
        }
    }
    Arch_irqStateInit();
    /* provide the IRQ control cap */
    write_slot(SLOT_PTR(pptr_of_cap(വേര്_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

BOOT_CODE സ്ഥിതം bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          v_region_t it_v_reg,
                                          mem_p_regs_t *mem_p_regs,
                                          word_t extra_bi_size_bits)
{
    // Extend the reserved region down to include the base of the kernel image.
    // KERNEL_ELF_PADDR_BASE is the lowest physical load address used
    // in the x86 linker script.
    ui_p_reg.start = KERNEL_ELF_PADDR_BASE;
    reserved[0] = paddr_to_pptr_reg(ui_p_reg);
    തിരികെ init_freemem(mem_p_regs->എണ്ണം, mem_p_regs->list, MAX_RESERVED,
                        reserved, it_v_reg, extra_bi_size_bits);
}

/* This function initialises a node's kernel state. It does NOT initialise the CPU. */

BOOT_CODE bool_t init_sys_state(
    cpu_id_t      cpu_id,
    mem_p_regs_t  *mem_p_regs,
    ui_info_t     ui_info,
    /* parameters below not modeled in abstract specification */
    uint32_t      അക്കം_drhu,
    paddr_t      *drhu_list,
    acpi_rmrr_list_t *rmrr_list,
    acpi_rsdp_t      *acpi_rsdp,
    seL4_X86_BootInfo_VBE *vbe,
    seL4_X86_BootInfo_mmap_t *mb_mmap,
    seL4_X86_BootInfo_fb_t *fb_info
)
{
    cap_t         വേര്_cnode_cap;
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

    എങ്കിൽ (vbe->vbeMode != -1) {
        extra_bi_size += വലുപ്പം(seL4_X86_BootInfo_VBE);
    }
    എങ്കിൽ (acpi_rsdp) {
        extra_bi_size += വലുപ്പം(seL4_BootInfoHeader) + വലുപ്പം(*acpi_rsdp);
    }
    എങ്കിൽ (fb_info && fb_info->addr) {
        extra_bi_size += വലുപ്പം(seL4_BootInfoHeader) + വലുപ്പം(*fb_info);
    }

    word_t mb_mmap_size = വലുപ്പം(seL4_X86_BootInfo_mmap_t);
    extra_bi_size += mb_mmap_size;

    // room for tsc frequency
    extra_bi_size += വലുപ്പം(seL4_BootInfoHeader) + 4;
    word_t extra_bi_size_bits = calculate_extra_bi_size_bits(extra_bi_size);

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    it_v_reg.start = ui_v_reg.start;
    it_v_reg.end = ROUND_UP(extra_bi_frame_vptr + (extra_bi_size_bits > 0 ? BIT(extra_bi_size_bits) : 0), PAGE_BITS);
#ifdef CONFIG_IOMMU
    /* calculate the number of io pts before initialising memory */
    എങ്കിൽ (!vtd_init_അക്കം_iopts(അക്കം_drhu)) {
        തിരികെ false;
    }
#endif /* CONFIG_IOMMU */

    എങ്കിൽ (!arch_init_freemem(ui_info.p_reg, it_v_reg, mem_p_regs, extra_bi_size_bits)) {
        printf("ERROR: free memory management initialization failed\n");
        തിരികെ false;
    }

    /* create the root cnode */
    വേര്_cnode_cap = create_വേര്_cnode();

    /* create the IO port cap */
    write_slot(
        SLOT_PTR(pptr_of_cap(വേര്_cnode_cap), seL4_CapIOPortControl),
        cap_io_port_control_cap_new()
    );

    /* create the cap for managing thread domains */
    create_domain_cap(വേര്_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(വേര്_cnode_cap);

    tsc_freq = tsc_init();

    /* populate the bootinfo frame */
    populate_bi_frame(0, ks_അക്കം_CPUs, ipcbuf_vptr, extra_bi_size);
    region_t extra_bi_region = {
        .start = rootserver.extra_bi,
        .end = rootserver.extra_bi + (extra_bi_size_bits > 0 ? BIT(extra_bi_size_bits) : 0)
    };

    /* populate vbe info block */
    എങ്കിൽ (vbe->vbeMode != -1) {
        vbe->header.id = SEL4_BOOTINFO_HEADER_X86_VBE;
        vbe->header.len = വലുപ്പം(seL4_X86_BootInfo_VBE);
        memcpy((ശൂന്യം *)(rootserver.extra_bi + extra_bi_offset), vbe, വലുപ്പം(seL4_X86_BootInfo_VBE));
        extra_bi_offset += വലുപ്പം(seL4_X86_BootInfo_VBE);
    }

    /* populate acpi rsdp block */
    എങ്കിൽ (acpi_rsdp) {
        seL4_BootInfoHeader header;
        header.id = SEL4_BOOTINFO_HEADER_X86_ACPI_RSDP;
        header.len = വലുപ്പം(header) + വലുപ്പം(*acpi_rsdp);
        *(seL4_BootInfoHeader *)(rootserver.extra_bi + extra_bi_offset) = header;
        extra_bi_offset += വലുപ്പം(header);
        memcpy((ശൂന്യം *)(rootserver.extra_bi + extra_bi_offset), acpi_rsdp, വലുപ്പം(*acpi_rsdp));
        extra_bi_offset += വലുപ്പം(*acpi_rsdp);
    }

    /* populate framebuffer information block */
    എങ്കിൽ (fb_info && fb_info->addr) {
        seL4_BootInfoHeader header;
        header.id = SEL4_BOOTINFO_HEADER_X86_FRAMEBUFFER;
        header.len = വലുപ്പം(header) + വലുപ്പം(*fb_info);
        *(seL4_BootInfoHeader *)(rootserver.extra_bi + extra_bi_offset) = header;
        extra_bi_offset += വലുപ്പം(header);
        memcpy((ശൂന്യം *)(rootserver.extra_bi + extra_bi_offset), fb_info, വലുപ്പം(*fb_info));
        extra_bi_offset += വലുപ്പം(*fb_info);
    }

    /* populate multiboot mmap block */
    mb_mmap->header.id = SEL4_BOOTINFO_HEADER_X86_MBMMAP;
    mb_mmap->header.len = mb_mmap_size;
    memcpy((ശൂന്യം *)(rootserver.extra_bi + extra_bi_offset), mb_mmap, mb_mmap_size);
    extra_bi_offset += mb_mmap_size;

    /* populate tsc frequency block */
    {
        seL4_BootInfoHeader header;
        header.id = SEL4_BOOTINFO_HEADER_X86_TSC_FREQ;
        header.len = വലുപ്പം(header) + 4;
        *(seL4_BootInfoHeader *)(extra_bi_region.start + extra_bi_offset) = header;
        extra_bi_offset += വലുപ്പം(header);
        *(uint32_t *)(extra_bi_region.start + extra_bi_offset) = tsc_freq;
        extra_bi_offset += 4;
    }

#ifdef CONFIG_KERNEL_MCS
    /* set up sched control for each core */
    init_sched_control(വേര്_cnode_cap, CONFIG_MAX_അക്കം_NODES);
#endif

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_vspace_cap = create_it_address_space(വേര്_cnode_cap, it_v_reg);
    എങ്കിൽ (cap_get_capType(it_vspace_cap) == cap_null_cap) {
        തിരികെ false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_frame_cap(
        വേര്_cnode_cap,
        it_vspace_cap,
        bi_frame_vptr
    );

    /* create and map extra bootinfo region */
    extra_bi_ret =
        create_frames_of_region(
            വേര്_cnode_cap,
            it_vspace_cap,
            extra_bi_region,
            true,
            pptr_to_paddr((ശൂന്യം *)(extra_bi_region.start - extra_bi_frame_vptr))
        );
    എങ്കിൽ (!extra_bi_ret.success) {
        തിരികെ false;
    }
    ndks_boot.bi_frame->extraBIPages = extra_bi_ret.region;

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_frame_cap(വേര്_cnode_cap, it_vspace_cap, ipcbuf_vptr);
    എങ്കിൽ (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        തിരികെ false;
    }

    /* create all userland image frames */
    create_frames_ret =
        create_frames_of_region(
            വേര്_cnode_cap,
            it_vspace_cap,
            ui_reg,
            true,
            ui_info.pv_offset
        );
    എങ്കിൽ (!create_frames_ret.success) {
        തിരികെ false;
    }
    ndks_boot.bi_frame->userImageFrames = create_frames_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(വേര്_cnode_cap);
    എങ്കിൽ (cap_get_capType(it_ap_cap) == cap_null_cap) {
        തിരികെ false;
    }
    write_it_asid_pool(it_ap_cap, it_vspace_cap);

#ifdef CONFIG_KERNEL_MCS
    കണ്ണി_STATE(ksCurTime) = getCurrentTime();
#endif

    /* create the idle thread */
    create_idle_thread();

    /* copy i387 FPU initial state from FPU */
    saveFpuState(കണ്ണി_STATE(ksIdleThread));
    x86KSnullFpuState = കണ്ണി_STATE(ksIdleThread)->tcbArch.tcbContext.fpuState;
    /* Check that we can load it: */
    loadFpuState(കണ്ണി_STATE(ksIdleThread));

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(വേര്_cnode_cap,
                                           it_vspace_cap,
                                           ui_info.v_entry,
                                           bi_frame_vptr,
                                           ipcbuf_vptr,
                                           ipcbuf_cap);
    എങ്കിൽ (initial == ശൂന്യ) {
        തിരികെ false;
    }
    init_core_state(initial);

#ifdef CONFIG_IOMMU
    /* initialise VTD-related data structures and the IOMMUs */
    എങ്കിൽ (!vtd_init(cpu_id, rmrr_list)) {
        തിരികെ false;
    }

    /* write number of IOMMU PT levels into bootinfo */
    ndks_boot.bi_frame->അക്കം_IOPTLevels = x86KSnumIOPTLevels;

    /* write IOSpace master cap */
    write_slot(SLOT_PTR(pptr_of_cap(വേര്_cnode_cap), seL4_CapIOSpace), master_iospace_cap());
#അല്ലെങ്കിൽ
    ndks_boot.bi_frame->അക്കം_IOPTLevels = -1;
#endif

    /* create all of the untypeds. Both devices and kernel window memory */
    എങ്കിൽ (!create_untypeds(വേര്_cnode_cap)) {
        തിരികെ false;
    }

    /* finalise the bootinfo frame */
    bi_finalise();

    തിരികെ true;
}

/* This function initialises the CPU. It does NOT initialise any kernel state. */

BOOT_CODE bool_t init_cpu(
    bool_t   mask_legacy_irqs
)
{
    /* initialise virtual-memory-related data structures */
    എങ്കിൽ (!init_vm_state()) {
        തിരികെ false;
    }

    /* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */
    init_dtrs();

    എങ്കിൽ (config_set(CONFIG_SYSENTER)) {
        /* initialise MSRs (needs an initialised TSS) */
        init_sysenter_msrs();
    } അല്ലെങ്കിൽ എങ്കിൽ (config_set(CONFIG_SYSCALL)) {
        init_syscall_msrs();
    } അല്ലെങ്കിൽ {
        തിരികെ false;
    }

    /* setup additional PAT MSR */
    എങ്കിൽ (!init_pat_msr()) {
        തിരികെ false;
    }

    /* enable the Write Protect bit in cr0. This prevents the kernel from writing to
     * read only memory, which we shouldn't do under correct execution */
    write_cr0(read_cr0() | CR0_WRITE_PROTECT);

    /* check for SMAP and SMEP and enable */
    cpuid_007h_ebx_t ebx_007;
    ebx_007.വാക്കുകൾ[0] = x86_cpuid_ebx(0x7, 0);
    എങ്കിൽ (cpuid_007h_ebx_get_smap(ebx_007)) {
        /* if we have user stack trace enabled or dangerous code injection then we cannot
         * enable this as SMAP will make them fault. */
        എങ്കിൽ (!config_set(CONFIG_PRINTING) && !config_set(CONFIG_DANGEROUS_CODE_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMAP);
        }
    }
    എങ്കിൽ (cpuid_007h_ebx_get_smep(ebx_007)) {
        /* similar to smap we cannot enable smep if using dangerous code injection. it
         * does not affect stack trace printing though */
        എങ്കിൽ (!config_set(CONFIG_DANGEROUS_CODE_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMEP);
        }
    }

    എങ്കിൽ (!init_ibrs()) {
        തിരികെ false;
    }

#ifdef CONFIG_HARDWARE_DEBUG_API
    /* Initialize hardware breakpoints */
    Arch_initHardwareBreakpoints();
#endif

    /* initialise floating-point unit */
    എങ്കിൽ (!Arch_initFpu()) {
        തിരികെ false;
    }

    /* initialise local APIC */
    എങ്കിൽ (!apic_init(mask_legacy_irqs)) {
        തിരികെ false;
    }

#ifdef CONFIG_DEBUG_DISABLE_PREFETCHERS
    എങ്കിൽ (!disablePrefetchers()) {
        തിരികെ false;
    }
#endif

    എങ്കിൽ (config_set(CONFIG_EXPORT_PMC_USER)) {
        enablePMCUser();
    }

#ifdef CONFIG_VTX
    /* initialise Intel VT-x extensions */
    എങ്കിൽ (!vtx_init()) {
        തിരികെ false;
    }
#endif

    തിരികെ true;
}
