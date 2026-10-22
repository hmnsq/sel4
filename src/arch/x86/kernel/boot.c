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

#परिभाषय MAX_RESERVED 1
BOOT_BSS स्थिर region_t reserved[MAX_RESERVED];

/* functions exactly corresponding to abstract specification */

BOOT_संहिता स्थिर शून्य init_irqs(cap_t मूलम्_cnode_cap)
{
    irq_t i;

    प्रति (i = 0; i <= maxIRQ; i++) {
        यदि (i == irq_timer) {
            समुच्चयः_IRQState(IRQTimer, i);
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
        } अन्यथा यदि (i == irq_remote_call_ipi || i == irq_reschedule_ipi) {
            समुच्चयः_IRQState(IRQIPI, i);
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */
#यदि_परिभाषितम् CONFIG_IOMMU
        } अन्यथा यदि (i == irq_iommu) {
            समुच्चयः_IRQState(IRQReserved, i);
#यदि_अन्तः
        } अन्यथा यदि (i == 2 && config_समुच्चयः(CONFIG_IRQ_PIC)) {
            /* cascaded legacy PIC */
            समुच्चयः_IRQState(IRQReserved, i);
        } अन्यथा यदि (i >= irq_isa_min && i <= irq_isa_max) {
            यदि (config_समुच्चयः(CONFIG_IRQ_PIC)) {
                समुच्चयः_IRQState(IRQInactive, i);
            } अन्यथा {
                समुच्चयः_IRQState(IRQReserved, i);
            }
        } अन्यथा यदि (i >= irq_user_min && i <= irq_user_max) {
            यदि (config_समुच्चयः(CONFIG_IRQ_IOAPIC)) {
                समुच्चयः_IRQState(IRQInactive, i);
            } अन्यथा {
                समुच्चयः_IRQState(IRQReserved, i);
            }
        } अन्यथा {
            समुच्चयः_IRQState(IRQReserved, i);
        }
    }
    Arch_irqStateInit();
    /* provide the IRQ control cap */
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

BOOT_संहिता स्थिर bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          v_region_t it_v_reg,
                                          mem_p_regs_t *mem_p_regs,
                                          word_t extra_bi_आकार_द्वयङ्काः)
{
    // Extend the reserved region down to include the base of the kernel image.
    // KERNEL_ELF_PADDR_BASE is the lowest physical load address used
    // in the x86 linker script.
    ui_p_reg.start = KERNEL_ELF_PADDR_आधारः;
    reserved[0] = paddr_गन्तव्यम्_pptr_reg(ui_p_reg);
    प्रत्यागम init_freemem(mem_p_regs->गणनम्, mem_p_regs->list, MAX_RESERVED,
                        reserved, it_v_reg, extra_bi_आकार_द्वयङ्काः);
}

/* This function initialises a node's kernel state. It does NOT initialise the CPU. */

BOOT_संहिता bool_t init_sys_state(
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
    word_t        extra_bi_आकार = 0;
    pptr_t        extra_bi_विस्थापनम् = 0;
    uint32_t      tsc_freq;
    create_चौकटाः_सम्बन्धि_region_ret_t create_चौकटाः_ret;
    create_चौकटाः_सम्बन्धि_region_ret_t extra_bi_ret;

    /* convert from physical addresses to kernel pptrs */
    region_t ui_reg             = paddr_गन्तव्यम्_pptr_reg(ui_info.p_reg);

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg;
    v_region_t it_v_reg;
    ui_v_reg.start = ui_info.p_reg.start - ui_info.pv_विस्थापनम्;
    ui_v_reg.अन्तः_2   = ui_info.p_reg.अन्तः_2   - ui_info.pv_विस्थापनम्;

    ipcbuf_vptr = ui_v_reg.अन्तः_2;
    bi_frame_vptr = ipcbuf_vptr + BIT(PAGE_द्वयङ्काः);
    extra_bi_frame_vptr = bi_frame_vptr + BIT(se_L4_Boot_Info_Frame_द्वयङ्काः);

    यदि (vbe->vbe_बहुलकम् != -1) {
        extra_bi_आकार += परिमाण(seL4_X86_BootInfo_VBE);
    }
    यदि (acpi_rsdp) {
        extra_bi_आकार += परिमाण(se_L4_Boot_Info_शीर्षकम्) + परिमाण(*acpi_rsdp);
    }
    यदि (fb_info && fb_info->addr) {
        extra_bi_आकार += परिमाण(se_L4_Boot_Info_शीर्षकम्) + परिमाण(*fb_info);
    }

    word_t mb_mmap_आकार = परिमाण(seL4_X86_BootInfo_mmap_t);
    extra_bi_आकार += mb_mmap_आकार;

    // room for tsc frequency
    extra_bi_आकार += परिमाण(se_L4_Boot_Info_शीर्षकम्) + 4;
    word_t extra_bi_आकार_द्वयङ्काः = calculate_extra_bi_आकार_द्वयङ्काः(extra_bi_आकार);

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    it_v_reg.start = ui_v_reg.start;
    it_v_reg.अन्तः_2 = आवर्तनम्_ऊर्ध्वम्(extra_bi_frame_vptr + (extra_bi_आकार_द्वयङ्काः > 0 ? BIT(extra_bi_आकार_द्वयङ्काः) : 0), PAGE_द्वयङ्काः);
#यदि_परिभाषितम् CONFIG_IOMMU
    /* calculate the number of io pts before initialising memory */
    यदि (!vtd_init_अङ्कः_iopts(अङ्कः_drhu)) {
        प्रत्यागम false;
    }
#यदि_अन्तः /* CONFIG_IOMMU */

    यदि (!arch_init_freemem(ui_info.p_reg, it_v_reg, mem_p_regs, extra_bi_आकार_द्वयङ्काः)) {
        प्रारूप_लिख("ERROR: free memory management initialization failed\n");
        प्रत्यागम false;
    }

    /* create the root cnode */
    मूलम्_cnode_cap = create_मूलम्_cnode();

    /* create the IO port cap */
    write_slot(
        SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapIOPortControl),
        cap_io_पत्तनम्_control_cap_new()
    );

    /* create the cap for managing thread domains */
    create_domain_cap(मूलम्_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(मूलम्_cnode_cap);

    tsc_freq = tsc_init();

    /* populate the bootinfo frame */
    populate_bi_frame(0, ks_अङ्कः_CPUs, ipcbuf_vptr, extra_bi_आकार);
    region_t extra_bi_region = {
        .start = rootserver.extra_bi,
        .अन्तः_2 = rootserver.extra_bi + (extra_bi_आकार_द्वयङ्काः > 0 ? BIT(extra_bi_आकार_द्वयङ्काः) : 0)
    };

    /* populate vbe info block */
    यदि (vbe->vbe_बहुलकम् != -1) {
        vbe->शीर्षकम्.id = SEL4_BOOTINFO_शीर्षकम्_X86_VBE;
        vbe->शीर्षकम्.len = परिमाण(seL4_X86_BootInfo_VBE);
        स्मृतिं_प्रतिलिख((शून्य *)(rootserver.extra_bi + extra_bi_विस्थापनम्), vbe, परिमाण(seL4_X86_BootInfo_VBE));
        extra_bi_विस्थापनम् += परिमाण(seL4_X86_BootInfo_VBE);
    }

    /* populate acpi rsdp block */
    यदि (acpi_rsdp) {
        se_L4_Boot_Info_शीर्षकम् शीर्षकम्;
        शीर्षकम्.id = SEL4_BOOTINFO_शीर्षकम्_X86_ACPI_RSDP;
        शीर्षकम्.len = परिमाण(शीर्षकम्) + परिमाण(*acpi_rsdp);
        *(se_L4_Boot_Info_शीर्षकम् *)(rootserver.extra_bi + extra_bi_विस्थापनम्) = शीर्षकम्;
        extra_bi_विस्थापनम् += परिमाण(शीर्षकम्);
        स्मृतिं_प्रतिलिख((शून्य *)(rootserver.extra_bi + extra_bi_विस्थापनम्), acpi_rsdp, परिमाण(*acpi_rsdp));
        extra_bi_विस्थापनम् += परिमाण(*acpi_rsdp);
    }

    /* populate framebuffer information block */
    यदि (fb_info && fb_info->addr) {
        se_L4_Boot_Info_शीर्षकम् शीर्षकम्;
        शीर्षकम्.id = SEL4_BOOTINFO_शीर्षकम्_X86_FRAMEBUFFER;
        शीर्षकम्.len = परिमाण(शीर्षकम्) + परिमाण(*fb_info);
        *(se_L4_Boot_Info_शीर्षकम् *)(rootserver.extra_bi + extra_bi_विस्थापनम्) = शीर्षकम्;
        extra_bi_विस्थापनम् += परिमाण(शीर्षकम्);
        स्मृतिं_प्रतिलिख((शून्य *)(rootserver.extra_bi + extra_bi_विस्थापनम्), fb_info, परिमाण(*fb_info));
        extra_bi_विस्थापनम् += परिमाण(*fb_info);
    }

    /* populate multiboot mmap block */
    mb_mmap->शीर्षकम्.id = SEL4_BOOTINFO_शीर्षकम्_X86_MBMMAP;
    mb_mmap->शीर्षकम्.len = mb_mmap_आकार;
    स्मृतिं_प्रतिलिख((शून्य *)(rootserver.extra_bi + extra_bi_विस्थापनम्), mb_mmap, mb_mmap_आकार);
    extra_bi_विस्थापनम् += mb_mmap_आकार;

    /* populate tsc frequency block */
    {
        se_L4_Boot_Info_शीर्षकम् शीर्षकम्;
        शीर्षकम्.id = SEL4_BOOTINFO_शीर्षकम्_X86_TSC_FREQ;
        शीर्षकम्.len = परिमाण(शीर्षकम्) + 4;
        *(se_L4_Boot_Info_शीर्षकम् *)(extra_bi_region.start + extra_bi_विस्थापनम्) = शीर्षकम्;
        extra_bi_विस्थापनम् += परिमाण(शीर्षकम्);
        *(uint32_t *)(extra_bi_region.start + extra_bi_विस्थापनम्) = tsc_freq;
        extra_bi_विस्थापनम् += 4;
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    /* set up sched control for each core */
    init_sched_control(मूलम्_cnode_cap, CONFIG_MAX_अङ्कः_NODES);
#यदि_अन्तः

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_vspace_cap = create_it_स्थानाङ्कः_space(मूलम्_cnode_cap, it_v_reg);
    यदि (cap_get_capType(it_vspace_cap) == cap_null_cap) {
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
        create_चौकटाः_सम्बन्धि_region(
            मूलम्_cnode_cap,
            it_vspace_cap,
            extra_bi_region,
            true,
            pptr_गन्तव्यम्_paddr((शून्य *)(extra_bi_region.start - extra_bi_frame_vptr))
        );
    यदि (!extra_bi_ret.success) {
        प्रत्यागम false;
    }
    ndks_boot.bi_frame->extraBIPages = extra_bi_ret.region;

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_frame_cap(मूलम्_cnode_cap, it_vspace_cap, ipcbuf_vptr);
    यदि (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        प्रत्यागम false;
    }

    /* create all userland image frames */
    create_चौकटाः_ret =
        create_चौकटाः_सम्बन्धि_region(
            मूलम्_cnode_cap,
            it_vspace_cap,
            ui_reg,
            true,
            ui_info.pv_विस्थापनम्
        );
    यदि (!create_चौकटाः_ret.success) {
        प्रत्यागम false;
    }
    ndks_boot.bi_frame->user_Image_चौकटाः = create_चौकटाः_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(मूलम्_cnode_cap);
    यदि (cap_get_capType(it_ap_cap) == cap_null_cap) {
        प्रत्यागम false;
    }
    write_it_asid_pool(it_ap_cap, it_vspace_cap);

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    ग्रन्थिः_STATE(ksCurTime) = getCurrentTime();
#यदि_अन्तः

    /* create the idle thread */
    create_idle_thread();

    /* copy i387 FPU initial state from FPU */
    saveFpuState(ग्रन्थिः_STATE(ksIdleThread));
    x86KSnullFpuState = ग्रन्थिः_STATE(ksIdleThread)->tcbArch.tcbContext.fpuState;
    /* Check that we can load it: */
    आरोपय_Fpu_State(ग्रन्थिः_STATE(ksIdleThread));

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(मूलम्_cnode_cap,
                                           it_vspace_cap,
                                           ui_info.v_प्रविष्टिः,
                                           bi_frame_vptr,
                                           ipcbuf_vptr,
                                           ipcbuf_cap);
    यदि (initial == रिक्त) {
        प्रत्यागम false;
    }
    init_core_state(initial);

#यदि_परिभाषितम् CONFIG_IOMMU
    /* initialise VTD-related data structures and the IOMMUs */
    यदि (!vtd_init(cpu_id, rmrr_list)) {
        प्रत्यागम false;
    }

    /* write number of IOMMU PT levels into bootinfo */
    ndks_boot.bi_frame->अङ्कः_IOPTLevels = x86KSnumIOPTLevels;

    /* write IOSpace master cap */
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapIOSpace), master_iospace_cap());
#अन्यथा
    ndks_boot.bi_frame->अङ्कः_IOPTLevels = -1;
#यदि_अन्तः

    /* create all of the untypeds. Both devices and kernel window memory */
    यदि (!create_untypeds(मूलम्_cnode_cap)) {
        प्रत्यागम false;
    }

    /* finalise the bootinfo frame */
    bi_finalise();

    प्रत्यागम true;
}

/* This function initialises the CPU. It does NOT initialise any kernel state. */

BOOT_संहिता bool_t init_cpu(
    bool_t   आवरणम्_legacy_irqs
)
{
    /* initialise virtual-memory-related data structures */
    यदि (!init_vm_state()) {
        प्रत्यागम false;
    }

    /* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */
    init_dtrs();

    यदि (config_समुच्चयः(CONFIG_SYSENTER)) {
        /* initialise MSRs (needs an initialised TSS) */
        init_sysenter_msrs();
    } अन्यथा यदि (config_समुच्चयः(CONFIG_SYSCALL)) {
        init_syscall_msrs();
    } अन्यथा {
        प्रत्यागम false;
    }

    /* setup additional PAT MSR */
    यदि (!init_pat_msr()) {
        प्रत्यागम false;
    }

    /* enable the Write Protect bit in cr0. This prevents the kernel from writing to
     * read only memory, which we shouldn't do under correct execution */
    write_cr0(read_cr0() | CR0_WRITE_PROTECT);

    /* check for SMAP and SMEP and enable */
    cpuid_007h_ebx_t ebx_007;
    ebx_007.शब्दाः[0] = x86_cpuid_ebx(0x7, 0);
    यदि (cpuid_007h_ebx_get_smap(ebx_007)) {
        /* if we have user stack trace enabled or dangerous code injection then we cannot
         * enable this as SMAP will make them fault. */
        यदि (!config_समुच्चयः(CONFIG_PRINTING) && !config_समुच्चयः(CONFIG_DANGEROUS_संहिता_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMAP);
        }
    }
    यदि (cpuid_007h_ebx_get_smep(ebx_007)) {
        /* similar to smap we cannot enable smep if using dangerous code injection. it
         * does not affect stack trace printing though */
        यदि (!config_समुच्चयः(CONFIG_DANGEROUS_संहिता_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMEP);
        }
    }

    यदि (!init_ibrs()) {
        प्रत्यागम false;
    }

#यदि_परिभाषितम् CONFIG_HARDWARE_DEBUG_API
    /* Initialize hardware breakpoints */
    Arch_initHardwareBreakpoints();
#यदि_अन्तः

    /* initialise floating-point unit */
    यदि (!Arch_initFpu()) {
        प्रत्यागम false;
    }

    /* initialise local APIC */
    यदि (!apic_init(आवरणम्_legacy_irqs)) {
        प्रत्यागम false;
    }

#यदि_परिभाषितम् CONFIG_DEBUG_DISABLE_PREFETCHERS
    यदि (!disablePrefetchers()) {
        प्रत्यागम false;
    }
#यदि_अन्तः

    यदि (config_समुच्चयः(CONFIG_EXPORT_PMC_USER)) {
        enablePMCUser();
    }

#यदि_परिभाषितम् CONFIG_VTX
    /* initialise Intel VT-x extensions */
    यदि (!vtx_init()) {
        प्रत्यागम false;
    }
#यदि_अन्तः

    प्रत्यागम true;
}
