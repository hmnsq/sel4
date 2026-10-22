/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <kernel/boot.h>
#समावेश <machine/io.h>
#समावेश <model/statedata.h>
#समावेश <object/interrupt.h>
#समावेश <arch/object/interrupt.h>
#समावेश <arch/machine.h>
#समावेश <arch/kernel/apic.h>
#समावेश <arch/kernel/boot.h>
#समावेश <arch/kernel/boot_sys.h>
#समावेश <arch/kernel/vspace.h>
#समावेश <machine/fpu.h>
#समावेश <arch/machine/timer.h>
#समावेश <arch/object/ioport.h>
#समावेश <linker.h>
#समावेश <util.h>

#समावेश <plat/machine/intel-vtd.h>

#घोषणा MAX_RESERVED 1
BOOT_BSS अटल region_t reserved[MAX_RESERVED];

/* functions exactly corresponding to abstract specification */

BOOT_CODE अटल व्योम init_irqs(cap_t मूल_cnode_cap)
{
    irq_t i;

    क्रम (i = 0; i <= maxIRQ; i++) {
        अगर (i == irq_timer) {
            setIRQState(IRQTimer, i);
#अगर_घोषित ENABLE_SMP_SUPPORT
        } अन्यथा अगर (i == irq_remote_call_ipi || i == irq_reschedule_ipi) {
            setIRQState(IRQIPI, i);
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */
#अगर_घोषित CONFIG_IOMMU
        } अन्यथा अगर (i == irq_iommu) {
            setIRQState(IRQReserved, i);
#पूर्ण_अगर
        } अन्यथा अगर (i == 2 && config_set(CONFIG_IRQ_PIC)) {
            /* cascaded legacy PIC */
            setIRQState(IRQReserved, i);
        } अन्यथा अगर (i >= irq_isa_min && i <= irq_isa_max) {
            अगर (config_set(CONFIG_IRQ_PIC)) {
                setIRQState(IRQInactive, i);
            } अन्यथा {
                setIRQState(IRQReserved, i);
            }
        } अन्यथा अगर (i >= irq_user_min && i <= irq_user_max) {
            अगर (config_set(CONFIG_IRQ_IOAPIC)) {
                setIRQState(IRQInactive, i);
            } अन्यथा {
                setIRQState(IRQReserved, i);
            }
        } अन्यथा {
            setIRQState(IRQReserved, i);
        }
    }
    Arch_irqStateInit();
    /* provide the IRQ control cap */
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

BOOT_CODE अटल bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          v_region_t it_v_reg,
                                          mem_p_regs_t *mem_p_regs,
                                          word_t extra_bi_आकार_bits)
{
    // Extend the reserved region down to include the base of the kernel image.
    // KERNEL_ELF_PADDR_BASE is the lowest physical load address used
    // in the x86 linker script.
    ui_p_reg.start = KERNEL_ELF_PADDR_BASE;
    reserved[0] = paddr_to_pptr_reg(ui_p_reg);
    वापस init_freemem(mem_p_regs->गिनती, mem_p_regs->list, MAX_RESERVED,
                        reserved, it_v_reg, extra_bi_आकार_bits);
}

/* This function initialises a node's kernel state. It does NOT initialise the CPU. */

BOOT_CODE bool_t init_sys_state(
    cpu_id_t      cpu_id,
    mem_p_regs_t  *mem_p_regs,
    ui_info_t     ui_info,
    /* parameters below not modeled in abstract specification */
    uint32_t      अंक_drhu,
    paddr_t      *drhu_list,
    acpi_rmrr_list_t *rmrr_list,
    acpi_rsdp_t      *acpi_rsdp,
    seL4_X86_BootInfo_VBE *vbe,
    seL4_X86_BootInfo_mmap_t *mb_mmap,
    seL4_X86_BootInfo_fb_t *fb_info
)
{
    cap_t         मूल_cnode_cap;
    vptr_t        extra_bi_frame_vptr;
    vptr_t        bi_frame_vptr;
    vptr_t        ipcbuf_vptr;
    cap_t         it_vspace_cap;
    cap_t         it_ap_cap;
    cap_t         ipcbuf_cap;
    word_t        extra_bi_आकार = 0;
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

    अगर (vbe->vbeMode != -1) {
        extra_bi_आकार += माप(seL4_X86_BootInfo_VBE);
    }
    अगर (acpi_rsdp) {
        extra_bi_आकार += माप(seL4_BootInfoHeader) + माप(*acpi_rsdp);
    }
    अगर (fb_info && fb_info->addr) {
        extra_bi_आकार += माप(seL4_BootInfoHeader) + माप(*fb_info);
    }

    word_t mb_mmap_आकार = माप(seL4_X86_BootInfo_mmap_t);
    extra_bi_आकार += mb_mmap_आकार;

    // room for tsc frequency
    extra_bi_आकार += माप(seL4_BootInfoHeader) + 4;
    word_t extra_bi_आकार_bits = calculate_extra_bi_आकार_bits(extra_bi_आकार);

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    it_v_reg.start = ui_v_reg.start;
    it_v_reg.end = ROUND_UP(extra_bi_frame_vptr + (extra_bi_आकार_bits > 0 ? BIT(extra_bi_आकार_bits) : 0), PAGE_BITS);
#अगर_घोषित CONFIG_IOMMU
    /* calculate the number of io pts before initialising memory */
    अगर (!vtd_init_अंक_iopts(अंक_drhu)) {
        वापस false;
    }
#पूर्ण_अगर /* CONFIG_IOMMU */

    अगर (!arch_init_freemem(ui_info.p_reg, it_v_reg, mem_p_regs, extra_bi_आकार_bits)) {
        म_लिखो("ERROR: free memory management initialization failed\n");
        वापस false;
    }

    /* create the root cnode */
    मूल_cnode_cap = create_मूल_cnode();

    /* create the IO port cap */
    write_slot(
        SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapIOPortControl),
        cap_io_port_control_cap_new()
    );

    /* create the cap for managing thread domains */
    create_domain_cap(मूल_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(मूल_cnode_cap);

    tsc_freq = tsc_init();

    /* populate the bootinfo frame */
    populate_bi_frame(0, ks_अंक_CPUs, ipcbuf_vptr, extra_bi_आकार);
    region_t extra_bi_region = {
        .start = rootserver.extra_bi,
        .end = rootserver.extra_bi + (extra_bi_आकार_bits > 0 ? BIT(extra_bi_आकार_bits) : 0)
    };

    /* populate vbe info block */
    अगर (vbe->vbeMode != -1) {
        vbe->header.id = SEL4_BOOTINFO_HEADER_X86_VBE;
        vbe->header.len = माप(seL4_X86_BootInfo_VBE);
        स_नकल((व्योम *)(rootserver.extra_bi + extra_bi_offset), vbe, माप(seL4_X86_BootInfo_VBE));
        extra_bi_offset += माप(seL4_X86_BootInfo_VBE);
    }

    /* populate acpi rsdp block */
    अगर (acpi_rsdp) {
        seL4_BootInfoHeader header;
        header.id = SEL4_BOOTINFO_HEADER_X86_ACPI_RSDP;
        header.len = माप(header) + माप(*acpi_rsdp);
        *(seL4_BootInfoHeader *)(rootserver.extra_bi + extra_bi_offset) = header;
        extra_bi_offset += माप(header);
        स_नकल((व्योम *)(rootserver.extra_bi + extra_bi_offset), acpi_rsdp, माप(*acpi_rsdp));
        extra_bi_offset += माप(*acpi_rsdp);
    }

    /* populate framebuffer information block */
    अगर (fb_info && fb_info->addr) {
        seL4_BootInfoHeader header;
        header.id = SEL4_BOOTINFO_HEADER_X86_FRAMEBUFFER;
        header.len = माप(header) + माप(*fb_info);
        *(seL4_BootInfoHeader *)(rootserver.extra_bi + extra_bi_offset) = header;
        extra_bi_offset += माप(header);
        स_नकल((व्योम *)(rootserver.extra_bi + extra_bi_offset), fb_info, माप(*fb_info));
        extra_bi_offset += माप(*fb_info);
    }

    /* populate multiboot mmap block */
    mb_mmap->header.id = SEL4_BOOTINFO_HEADER_X86_MBMMAP;
    mb_mmap->header.len = mb_mmap_आकार;
    स_नकल((व्योम *)(rootserver.extra_bi + extra_bi_offset), mb_mmap, mb_mmap_आकार);
    extra_bi_offset += mb_mmap_आकार;

    /* populate tsc frequency block */
    {
        seL4_BootInfoHeader header;
        header.id = SEL4_BOOTINFO_HEADER_X86_TSC_FREQ;
        header.len = माप(header) + 4;
        *(seL4_BootInfoHeader *)(extra_bi_region.start + extra_bi_offset) = header;
        extra_bi_offset += माप(header);
        *(uint32_t *)(extra_bi_region.start + extra_bi_offset) = tsc_freq;
        extra_bi_offset += 4;
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    /* set up sched control for each core */
    init_sched_control(मूल_cnode_cap, CONFIG_MAX_अंक_NODES);
#पूर्ण_अगर

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_vspace_cap = create_it_address_space(मूल_cnode_cap, it_v_reg);
    अगर (cap_get_capType(it_vspace_cap) == cap_null_cap) {
        वापस false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_frame_cap(
        मूल_cnode_cap,
        it_vspace_cap,
        bi_frame_vptr
    );

    /* create and map extra bootinfo region */
    extra_bi_ret =
        create_frames_of_region(
            मूल_cnode_cap,
            it_vspace_cap,
            extra_bi_region,
            true,
            pptr_to_paddr((व्योम *)(extra_bi_region.start - extra_bi_frame_vptr))
        );
    अगर (!extra_bi_ret.success) {
        वापस false;
    }
    ndks_boot.bi_frame->extraBIPages = extra_bi_ret.region;

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_frame_cap(मूल_cnode_cap, it_vspace_cap, ipcbuf_vptr);
    अगर (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        वापस false;
    }

    /* create all userland image frames */
    create_frames_ret =
        create_frames_of_region(
            मूल_cnode_cap,
            it_vspace_cap,
            ui_reg,
            true,
            ui_info.pv_offset
        );
    अगर (!create_frames_ret.success) {
        वापस false;
    }
    ndks_boot.bi_frame->userImageFrames = create_frames_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(मूल_cnode_cap);
    अगर (cap_get_capType(it_ap_cap) == cap_null_cap) {
        वापस false;
    }
    write_it_asid_pool(it_ap_cap, it_vspace_cap);

#अगर_घोषित CONFIG_KERNEL_MCS
    गाँठ_STATE(ksCurTime) = getCurrentTime();
#पूर्ण_अगर

    /* create the idle thread */
    create_idle_thread();

    /* copy i387 FPU initial state from FPU */
    saveFpuState(गाँठ_STATE(ksIdleThread));
    x86KSnullFpuState = गाँठ_STATE(ksIdleThread)->tcbArch.tcbContext.fpuState;
    /* Check that we can load it: */
    loadFpuState(गाँठ_STATE(ksIdleThread));

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(मूल_cnode_cap,
                                           it_vspace_cap,
                                           ui_info.v_entry,
                                           bi_frame_vptr,
                                           ipcbuf_vptr,
                                           ipcbuf_cap);
    अगर (initial == शून्य) {
        वापस false;
    }
    init_core_state(initial);

#अगर_घोषित CONFIG_IOMMU
    /* initialise VTD-related data structures and the IOMMUs */
    अगर (!vtd_init(cpu_id, rmrr_list)) {
        वापस false;
    }

    /* write number of IOMMU PT levels into bootinfo */
    ndks_boot.bi_frame->अंक_IOPTLevels = x86KSnumIOPTLevels;

    /* write IOSpace master cap */
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapIOSpace), master_iospace_cap());
#अन्यथा
    ndks_boot.bi_frame->अंक_IOPTLevels = -1;
#पूर्ण_अगर

    /* create all of the untypeds. Both devices and kernel window memory */
    अगर (!create_untypeds(मूल_cnode_cap)) {
        वापस false;
    }

    /* finalise the bootinfo frame */
    bi_finalise();

    वापस true;
}

/* This function initialises the CPU. It does NOT initialise any kernel state. */

BOOT_CODE bool_t init_cpu(
    bool_t   mask_legacy_irqs
)
{
    /* initialise virtual-memory-related data structures */
    अगर (!init_vm_state()) {
        वापस false;
    }

    /* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */
    init_dtrs();

    अगर (config_set(CONFIG_SYSENTER)) {
        /* initialise MSRs (needs an initialised TSS) */
        init_sysenter_msrs();
    } अन्यथा अगर (config_set(CONFIG_SYSCALL)) {
        init_syscall_msrs();
    } अन्यथा {
        वापस false;
    }

    /* setup additional PAT MSR */
    अगर (!init_pat_msr()) {
        वापस false;
    }

    /* enable the Write Protect bit in cr0. This prevents the kernel from writing to
     * read only memory, which we shouldn't do under correct execution */
    write_cr0(read_cr0() | CR0_WRITE_PROTECT);

    /* check for SMAP and SMEP and enable */
    cpuid_007h_ebx_t ebx_007;
    ebx_007.शब्दगण[0] = x86_cpuid_ebx(0x7, 0);
    अगर (cpuid_007h_ebx_get_smap(ebx_007)) {
        /* if we have user stack trace enabled or dangerous code injection then we cannot
         * enable this as SMAP will make them fault. */
        अगर (!config_set(CONFIG_PRINTING) && !config_set(CONFIG_DANGEROUS_CODE_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMAP);
        }
    }
    अगर (cpuid_007h_ebx_get_smep(ebx_007)) {
        /* similar to smap we cannot enable smep if using dangerous code injection. it
         * does not affect stack trace printing though */
        अगर (!config_set(CONFIG_DANGEROUS_CODE_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMEP);
        }
    }

    अगर (!init_ibrs()) {
        वापस false;
    }

#अगर_घोषित CONFIG_HARDWARE_DEBUG_API
    /* Initialize hardware breakpoints */
    Arch_initHardwareBreakpoints();
#पूर्ण_अगर

    /* initialise floating-point unit */
    अगर (!Arch_initFpu()) {
        वापस false;
    }

    /* initialise local APIC */
    अगर (!apic_init(mask_legacy_irqs)) {
        वापस false;
    }

#अगर_घोषित CONFIG_DEBUG_DISABLE_PREFETCHERS
    अगर (!disablePrefetchers()) {
        वापस false;
    }
#पूर्ण_अगर

    अगर (config_set(CONFIG_EXPORT_PMC_USER)) {
        enablePMCUser();
    }

#अगर_घोषित CONFIG_VTX
    /* initialise Intel VT-x extensions */
    अगर (!vtx_init()) {
        वापस false;
    }
#पूर्ण_अगर

    वापस true;
}
