/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <kernel/boot.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <object/interrupt.h>
#అన్తర్భావ <arch/object/interrupt.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/kernel/apic.h>
#అన్తర్భావ <arch/kernel/boot.h>
#అన్తర్భావ <arch/kernel/boot_sys.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <machine/fpu.h>
#అన్తర్భావ <arch/machine/timer.h>
#అన్తర్భావ <arch/object/ioport.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <util.h>

#అన్తర్భావ <plat/machine/intel-vtd.h>

#నిర్వచించు MAX_RESERVED 1
BOOT_BSS స్థిర region_t reserved[MAX_RESERVED];

/* functions exactly corresponding to abstract specification */

BOOT_సంకేతం స్థిర శూన్య init_irqs(cap_t మూలం_cnode_cap)
{
    irq_t i;

    కొరకు (i = 0; i <= maxIRQ; i++) {
        ఒకవేళ (i == irq_timer) {
            సమితి_IRQState(IRQTimer, i);
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
        } లేకపోతే ఒకవేళ (i == irq_remote_call_ipi || i == irq_reschedule_ipi) {
            సమితి_IRQState(IRQIPI, i);
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
#నిర్వచితమైతే CONFIG_IOMMU
        } లేకపోతే ఒకవేళ (i == irq_iommu) {
            సమితి_IRQState(IRQReserved, i);
#అయితే_ముగింపు
        } లేకపోతే ఒకవేళ (i == 2 && config_సమితి(CONFIG_IRQ_PIC)) {
            /* cascaded legacy PIC */
            సమితి_IRQState(IRQReserved, i);
        } లేకపోతే ఒకవేళ (i >= irq_isa_min && i <= irq_isa_max) {
            ఒకవేళ (config_సమితి(CONFIG_IRQ_PIC)) {
                సమితి_IRQState(IRQInactive, i);
            } లేకపోతే {
                సమితి_IRQState(IRQReserved, i);
            }
        } లేకపోతే ఒకవేళ (i >= irq_user_min && i <= irq_user_max) {
            ఒకవేళ (config_సమితి(CONFIG_IRQ_IOAPIC)) {
                సమితి_IRQState(IRQInactive, i);
            } లేకపోతే {
                సమితి_IRQState(IRQReserved, i);
            }
        } లేకపోతే {
            సమితి_IRQState(IRQReserved, i);
        }
    }
    Arch_irqStateInit();
    /* provide the IRQ control cap */
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

BOOT_సంకేతం స్థిర bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          v_region_t it_v_reg,
                                          mem_p_regs_t *mem_p_regs,
                                          మాట_t extra_bi_కొలత_బిట్లు)
{
    // Extend the reserved region down to include the base of the kernel image.
    // KERNEL_ELF_PADDR_BASE is the lowest physical load address used
    // in the x86 linker script.
    ui_p_reg.ప్రారంభం = KERNEL_ELF_PADDR_పునాది;
    reserved[0] = paddr_గమ్యం_pptr_reg(ui_p_reg);
    తిరిగి init_freemem(mem_p_regs->లెక్క, mem_p_regs->list, MAX_RESERVED,
                        reserved, it_v_reg, extra_bi_కొలత_బిట్లు);
}

/* This function initialises a node's kernel state. It does NOT initialise the CPU. */

BOOT_సంకేతం bool_t init_sys_state(
    cpu_id_t      cpu_id,
    mem_p_regs_t  *mem_p_regs,
    ui_info_t     ui_info,
    /* parameters below not modeled in abstract specification */
    uint32_t      అంకె_drhu,
    paddr_t      *drhu_list,
    acpi_rmrr_list_t *rmrr_list,
    acpi_rsdp_t      *acpi_rsdp,
    seL4_X86_BootInfo_VBE *vbe,
    seL4_X86_BootInfo_mmap_t *mb_mmap,
    seL4_X86_BootInfo_fb_t *fb_info
)
{
    cap_t         మూలం_cnode_cap;
    vptr_t        extra_bi_చట్రం_vptr;
    vptr_t        bi_చట్రం_vptr;
    vptr_t        ipcbuf_vptr;
    cap_t         it_vspace_cap;
    cap_t         it_ap_cap;
    cap_t         ipcbuf_cap;
    మాట_t        extra_bi_కొలత = 0;
    pptr_t        extra_bi_స్థానభ్రంశం = 0;
    uint32_t      tsc_freq;
    create_చట్రాలు_యొక్క_region_ret_t create_చట్రాలు_ret;
    create_చట్రాలు_యొక్క_region_ret_t extra_bi_ret;

    /* convert from physical addresses to kernel pptrs */
    region_t ui_reg             = paddr_గమ్యం_pptr_reg(ui_info.p_reg);

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg;
    v_region_t it_v_reg;
    ui_v_reg.ప్రారంభం = ui_info.p_reg.ప్రారంభం - ui_info.pv_స్థానభ్రంశం;
    ui_v_reg.చివర   = ui_info.p_reg.చివర   - ui_info.pv_స్థానభ్రంశం;

    ipcbuf_vptr = ui_v_reg.చివర;
    bi_చట్రం_vptr = ipcbuf_vptr + బిట్టు(పుట_బిట్లు);
    extra_bi_చట్రం_vptr = bi_చట్రం_vptr + బిట్టు(se_L4_Boot_Info_చట్రం_బిట్లు);

    ఒకవేళ (vbe->vbe_బహుళకం != -1) {
        extra_bi_కొలత += పరిమాణం(seL4_X86_BootInfo_VBE);
    }
    ఒకవేళ (acpi_rsdp) {
        extra_bi_కొలత += పరిమాణం(se_L4_Boot_Info_శీర్షిక) + పరిమాణం(*acpi_rsdp);
    }
    ఒకవేళ (fb_info && fb_info->addr) {
        extra_bi_కొలత += పరిమాణం(se_L4_Boot_Info_శీర్షిక) + పరిమాణం(*fb_info);
    }

    మాట_t mb_mmap_కొలత = పరిమాణం(seL4_X86_BootInfo_mmap_t);
    extra_bi_కొలత += mb_mmap_కొలత;

    // room for tsc frequency
    extra_bi_కొలత += పరిమాణం(se_L4_Boot_Info_శీర్షిక) + 4;
    మాట_t extra_bi_కొలత_బిట్లు = calculate_extra_bi_కొలత_బిట్లు(extra_bi_కొలత);

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    it_v_reg.ప్రారంభం = ui_v_reg.ప్రారంభం;
    it_v_reg.చివర = విడత_పైకి(extra_bi_చట్రం_vptr + (extra_bi_కొలత_బిట్లు > 0 ? బిట్టు(extra_bi_కొలత_బిట్లు) : 0), పుట_బిట్లు);
#నిర్వచితమైతే CONFIG_IOMMU
    /* calculate the number of io pts before initialising memory */
    ఒకవేళ (!vtd_init_అంకె_iopts(అంకె_drhu)) {
        తిరిగి false;
    }
#అయితే_ముగింపు /* CONFIG_IOMMU */

    ఒకవేళ (!arch_init_freemem(ui_info.p_reg, it_v_reg, mem_p_regs, extra_bi_కొలత_బిట్లు)) {
        రాయి("ERROR: free memory management initialization failed\n");
        తిరిగి false;
    }

    /* create the root cnode */
    మూలం_cnode_cap = create_మూలం_cnode();

    /* create the IO port cap */
    write_స్థానం(
        స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapIOPortControl),
        cap_io_పోర్టు_control_cap_new()
    );

    /* create the cap for managing thread domains */
    create_domain_cap(మూలం_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(మూలం_cnode_cap);

    tsc_freq = tsc_init();

    /* populate the bootinfo frame */
    populate_bi_చట్రం(0, ks_అంకె_CPUs, ipcbuf_vptr, extra_bi_కొలత);
    region_t extra_bi_region = {
        .ప్రారంభం = rootserver.extra_bi,
        .చివర = rootserver.extra_bi + (extra_bi_కొలత_బిట్లు > 0 ? బిట్టు(extra_bi_కొలత_బిట్లు) : 0)
    };

    /* populate vbe info block */
    ఒకవేళ (vbe->vbe_బహుళకం != -1) {
        vbe->శీర్షిక.id = SEL4_BOOTINFO_శీర్షిక_X86_VBE;
        vbe->శీర్షిక.len = పరిమాణం(seL4_X86_BootInfo_VBE);
        జ్ఞాపకం_నకలు((శూన్య *)(rootserver.extra_bi + extra_bi_స్థానభ్రంశం), vbe, పరిమాణం(seL4_X86_BootInfo_VBE));
        extra_bi_స్థానభ్రంశం += పరిమాణం(seL4_X86_BootInfo_VBE);
    }

    /* populate acpi rsdp block */
    ఒకవేళ (acpi_rsdp) {
        se_L4_Boot_Info_శీర్షిక శీర్షిక;
        శీర్షిక.id = SEL4_BOOTINFO_శీర్షిక_X86_ACPI_RSDP;
        శీర్షిక.len = పరిమాణం(శీర్షిక) + పరిమాణం(*acpi_rsdp);
        *(se_L4_Boot_Info_శీర్షిక *)(rootserver.extra_bi + extra_bi_స్థానభ్రంశం) = శీర్షిక;
        extra_bi_స్థానభ్రంశం += పరిమాణం(శీర్షిక);
        జ్ఞాపకం_నకలు((శూన్య *)(rootserver.extra_bi + extra_bi_స్థానభ్రంశం), acpi_rsdp, పరిమాణం(*acpi_rsdp));
        extra_bi_స్థానభ్రంశం += పరిమాణం(*acpi_rsdp);
    }

    /* populate framebuffer information block */
    ఒకవేళ (fb_info && fb_info->addr) {
        se_L4_Boot_Info_శీర్షిక శీర్షిక;
        శీర్షిక.id = SEL4_BOOTINFO_శీర్షిక_X86_FRAMEBUFFER;
        శీర్షిక.len = పరిమాణం(శీర్షిక) + పరిమాణం(*fb_info);
        *(se_L4_Boot_Info_శీర్షిక *)(rootserver.extra_bi + extra_bi_స్థానభ్రంశం) = శీర్షిక;
        extra_bi_స్థానభ్రంశం += పరిమాణం(శీర్షిక);
        జ్ఞాపకం_నకలు((శూన్య *)(rootserver.extra_bi + extra_bi_స్థానభ్రంశం), fb_info, పరిమాణం(*fb_info));
        extra_bi_స్థానభ్రంశం += పరిమాణం(*fb_info);
    }

    /* populate multiboot mmap block */
    mb_mmap->శీర్షిక.id = SEL4_BOOTINFO_శీర్షిక_X86_MBMMAP;
    mb_mmap->శీర్షిక.len = mb_mmap_కొలత;
    జ్ఞాపకం_నకలు((శూన్య *)(rootserver.extra_bi + extra_bi_స్థానభ్రంశం), mb_mmap, mb_mmap_కొలత);
    extra_bi_స్థానభ్రంశం += mb_mmap_కొలత;

    /* populate tsc frequency block */
    {
        se_L4_Boot_Info_శీర్షిక శీర్షిక;
        శీర్షిక.id = SEL4_BOOTINFO_శీర్షిక_X86_TSC_FREQ;
        శీర్షిక.len = పరిమాణం(శీర్షిక) + 4;
        *(se_L4_Boot_Info_శీర్షిక *)(extra_bi_region.ప్రారంభం + extra_bi_స్థానభ్రంశం) = శీర్షిక;
        extra_bi_స్థానభ్రంశం += పరిమాణం(శీర్షిక);
        *(uint32_t *)(extra_bi_region.ప్రారంభం + extra_bi_స్థానభ్రంశం) = tsc_freq;
        extra_bi_స్థానభ్రంశం += 4;
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    /* set up sched control for each core */
    init_sched_control(మూలం_cnode_cap, CONFIG_MAX_అంకె_NODES);
#అయితే_ముగింపు

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_vspace_cap = create_it_చిరునామా_space(మూలం_cnode_cap, it_v_reg);
    ఒకవేళ (cap_get_capType(it_vspace_cap) == cap_null_cap) {
        తిరిగి false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_చట్రం_cap(
        మూలం_cnode_cap,
        it_vspace_cap,
        bi_చట్రం_vptr
    );

    /* create and map extra bootinfo region */
    extra_bi_ret =
        create_చట్రాలు_యొక్క_region(
            మూలం_cnode_cap,
            it_vspace_cap,
            extra_bi_region,
            true,
            pptr_గమ్యం_paddr((శూన్య *)(extra_bi_region.ప్రారంభం - extra_bi_చట్రం_vptr))
        );
    ఒకవేళ (!extra_bi_ret.success) {
        తిరిగి false;
    }
    ndks_boot.bi_చట్రం->extraBIPages = extra_bi_ret.region;

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_చట్రం_cap(మూలం_cnode_cap, it_vspace_cap, ipcbuf_vptr);
    ఒకవేళ (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        తిరిగి false;
    }

    /* create all userland image frames */
    create_చట్రాలు_ret =
        create_చట్రాలు_యొక్క_region(
            మూలం_cnode_cap,
            it_vspace_cap,
            ui_reg,
            true,
            ui_info.pv_స్థానభ్రంశం
        );
    ఒకవేళ (!create_చట్రాలు_ret.success) {
        తిరిగి false;
    }
    ndks_boot.bi_చట్రం->user_Image_చట్రాలు = create_చట్రాలు_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(మూలం_cnode_cap);
    ఒకవేళ (cap_get_capType(it_ap_cap) == cap_null_cap) {
        తిరిగి false;
    }
    write_it_asid_pool(it_ap_cap, it_vspace_cap);

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    కణుపు_STATE(ks_Cur_సమయం) = get_Current_సమయం();
#అయితే_ముగింపు

    /* create the idle thread */
    create_idle_thread();

    /* copy i387 FPU initial state from FPU */
    saveFpuState(కణుపు_STATE(ksIdleThread));
    x86KSnullFpuState = కణుపు_STATE(ksIdleThread)->tcbArch.tcbContext.fpuState;
    /* Check that we can load it: */
    ఎక్కించు_Fpu_State(కణుపు_STATE(ksIdleThread));

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(మూలం_cnode_cap,
                                           it_vspace_cap,
                                           ui_info.v_నమోదు,
                                           bi_చట్రం_vptr,
                                           ipcbuf_vptr,
                                           ipcbuf_cap);
    ఒకవేళ (initial == ఖాళీ) {
        తిరిగి false;
    }
    init_core_state(initial);

#నిర్వచితమైతే CONFIG_IOMMU
    /* initialise VTD-related data structures and the IOMMUs */
    ఒకవేళ (!vtd_init(cpu_id, rmrr_list)) {
        తిరిగి false;
    }

    /* write number of IOMMU PT levels into bootinfo */
    ndks_boot.bi_చట్రం->అంకె_IOPTLevels = x86KSnumIOPTLevels;

    /* write IOSpace master cap */
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapIOSpace), master_iospace_cap());
#లేకపోతే
    ndks_boot.bi_చట్రం->అంకె_IOPTLevels = -1;
#అయితే_ముగింపు

    /* create all of the untypeds. Both devices and kernel window memory */
    ఒకవేళ (!create_untypeds(మూలం_cnode_cap)) {
        తిరిగి false;
    }

    /* finalise the bootinfo frame */
    bi_finalise();

    తిరిగి true;
}

/* This function initialises the CPU. It does NOT initialise any kernel state. */

BOOT_సంకేతం bool_t init_cpu(
    bool_t   ముసుగు_legacy_irqs
)
{
    /* initialise virtual-memory-related data structures */
    ఒకవేళ (!init_vm_state()) {
        తిరిగి false;
    }

    /* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */
    init_dtrs();

    ఒకవేళ (config_సమితి(CONFIG_SYSENTER)) {
        /* initialise MSRs (needs an initialised TSS) */
        init_sysenter_msrs();
    } లేకపోతే ఒకవేళ (config_సమితి(CONFIG_SYSCALL)) {
        init_syscall_msrs();
    } లేకపోతే {
        తిరిగి false;
    }

    /* setup additional PAT MSR */
    ఒకవేళ (!init_pat_msr()) {
        తిరిగి false;
    }

    /* enable the Write Protect bit in cr0. This prevents the kernel from writing to
     * read only memory, which we shouldn't do under correct execution */
    write_cr0(read_cr0() | CR0_WRITE_PROTECT);

    /* check for SMAP and SMEP and enable */
    cpuid_007h_ebx_t ebx_007;
    ebx_007.పదాలు[0] = x86_cpuid_ebx(0x7, 0);
    ఒకవేళ (cpuid_007h_ebx_get_smap(ebx_007)) {
        /* if we have user stack trace enabled or dangerous code injection then we cannot
         * enable this as SMAP will make them fault. */
        ఒకవేళ (!config_సమితి(CONFIG_PRINTING) && !config_సమితి(CONFIG_DANGEROUS_సంకేతం_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMAP);
        }
    }
    ఒకవేళ (cpuid_007h_ebx_get_smep(ebx_007)) {
        /* similar to smap we cannot enable smep if using dangerous code injection. it
         * does not affect stack trace printing though */
        ఒకవేళ (!config_సమితి(CONFIG_DANGEROUS_సంకేతం_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMEP);
        }
    }

    ఒకవేళ (!init_ibrs()) {
        తిరిగి false;
    }

#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API
    /* Initialize hardware breakpoints */
    Arch_initHardwareBreakpoints();
#అయితే_ముగింపు

    /* initialise floating-point unit */
    ఒకవేళ (!Arch_initFpu()) {
        తిరిగి false;
    }

    /* initialise local APIC */
    ఒకవేళ (!apic_init(ముసుగు_legacy_irqs)) {
        తిరిగి false;
    }

#నిర్వచితమైతే CONFIG_DEBUG_DISABLE_PREFETCHERS
    ఒకవేళ (!disablePrefetchers()) {
        తిరిగి false;
    }
#అయితే_ముగింపు

    ఒకవేళ (config_సమితి(CONFIG_EXPORT_PMC_USER)) {
        enablePMCUser();
    }

#నిర్వచితమైతే CONFIG_VTX
    /* initialise Intel VT-x extensions */
    ఒకవేళ (!vtx_init()) {
        తిరిగి false;
    }
#అయితే_ముగింపు

    తిరిగి true;
}
