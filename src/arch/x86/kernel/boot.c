/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <kernel/boot.h>
#شامل <machine/io.h>
#شامل <model/statedata.h>
#شامل <object/interrupt.h>
#شامل <arch/object/interrupt.h>
#شامل <arch/machine.h>
#شامل <arch/kernel/apic.h>
#شامل <arch/kernel/boot.h>
#شامل <arch/kernel/boot_sys.h>
#شامل <arch/kernel/vspace.h>
#شامل <machine/fpu.h>
#شامل <arch/machine/timer.h>
#شامل <arch/object/ioport.h>
#شامل <linker.h>
#شامل <util.h>

#شامل <plat/machine/intel-vtd.h>

#تعریف_کرو MAX_RESERVED 1
BOOT_BSS ساکن region_t reserved[MAX_RESERVED];

/* functions exactly corresponding to abstract specification */

BOOT_رمز ساکن خالی init_irqs(cap_t جڑ_cnode_cap)
{
    irq_t i;

    برائے (i = 0; i <= maxIRQ; i++) {
        اگر (i == irq_timer) {
            سیٹ_IRQState(IRQTimer, i);
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
        } ورنہ اگر (i == irq_remote_call_ipi || i == irq_reschedule_ipi) {
            سیٹ_IRQState(IRQIPI, i);
#اگر_ختم /* ENABLE_SMP_SUPPORT */
#اگر_تعریف_شدہ CONFIG_IOMMU
        } ورنہ اگر (i == irq_iommu) {
            سیٹ_IRQState(IRQReserved, i);
#اگر_ختم
        } ورنہ اگر (i == 2 && config_سیٹ(CONFIG_IRQ_PIC)) {
            /* cascaded legacy PIC */
            سیٹ_IRQState(IRQReserved, i);
        } ورنہ اگر (i >= irq_isa_min && i <= irq_isa_max) {
            اگر (config_سیٹ(CONFIG_IRQ_PIC)) {
                سیٹ_IRQState(IRQInactive, i);
            } ورنہ {
                سیٹ_IRQState(IRQReserved, i);
            }
        } ورنہ اگر (i >= irq_user_min && i <= irq_user_max) {
            اگر (config_سیٹ(CONFIG_IRQ_IOAPIC)) {
                سیٹ_IRQState(IRQInactive, i);
            } ورنہ {
                سیٹ_IRQState(IRQReserved, i);
            }
        } ورنہ {
            سیٹ_IRQState(IRQReserved, i);
        }
    }
    Arch_irqStateInit();
    /* provide the IRQ control cap */
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

BOOT_رمز ساکن bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          v_region_t it_v_reg,
                                          mem_p_regs_t *mem_p_regs,
                                          لفظ_t extra_bi_سائز_بٹ)
{
    // Extend the reserved region down to include the base of the kernel image.
    // KERNEL_ELF_PADDR_BASE is the lowest physical load address used
    // in the x86 linker script.
    ui_p_reg.آغاز = KERNEL_ELF_PADDR_بنیاد;
    reserved[0] = paddr_مقصد_pptr_reg(ui_p_reg);
    واپس init_freemem(mem_p_regs->گنتی, mem_p_regs->list, MAX_RESERVED,
                        reserved, it_v_reg, extra_bi_سائز_بٹ);
}

/* This function initialises a node's kernel state. It does NOT initialise the CPU. */

BOOT_رمز bool_t init_sys_state(
    cpu_id_t      cpu_id,
    mem_p_regs_t  *mem_p_regs,
    ui_info_t     ui_info,
    /* parameters below not modeled in abstract specification */
    uint32_t      عدد_drhu,
    paddr_t      *drhu_list,
    acpi_rmrr_list_t *rmrr_list,
    acpi_rsdp_t      *acpi_rsdp,
    seL4_X86_BootInfo_VBE *vbe,
    seL4_X86_BootInfo_mmap_t *mb_mmap,
    seL4_X86_BootInfo_fb_t *fb_info
)
{
    cap_t         جڑ_cnode_cap;
    vptr_t        extra_bi_چوکھٹا_vptr;
    vptr_t        bi_چوکھٹا_vptr;
    vptr_t        ipcbuf_vptr;
    cap_t         it_vspace_cap;
    cap_t         it_ap_cap;
    cap_t         ipcbuf_cap;
    لفظ_t        extra_bi_سائز = 0;
    pptr_t        extra_bi_انحراف = 0;
    uint32_t      tsc_freq;
    create_چوکھٹے_کا_region_ret_t create_چوکھٹے_ret;
    create_چوکھٹے_کا_region_ret_t extra_bi_ret;

    /* convert from physical addresses to kernel pptrs */
    region_t ui_reg             = paddr_مقصد_pptr_reg(ui_info.p_reg);

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg;
    v_region_t it_v_reg;
    ui_v_reg.آغاز = ui_info.p_reg.آغاز - ui_info.pv_انحراف;
    ui_v_reg.انتہا   = ui_info.p_reg.انتہا   - ui_info.pv_انحراف;

    ipcbuf_vptr = ui_v_reg.انتہا;
    bi_چوکھٹا_vptr = ipcbuf_vptr + ثنائی_ہندسہ(صفحہ_بٹ);
    extra_bi_چوکھٹا_vptr = bi_چوکھٹا_vptr + ثنائی_ہندسہ(se_L4_Boot_Info_چوکھٹا_بٹ);

    اگر (vbe->vbe_منوال != -1) {
        extra_bi_سائز += حجم(seL4_X86_BootInfo_VBE);
    }
    اگر (acpi_rsdp) {
        extra_bi_سائز += حجم(se_L4_Boot_Info_سرخی) + حجم(*acpi_rsdp);
    }
    اگر (fb_info && fb_info->addr) {
        extra_bi_سائز += حجم(se_L4_Boot_Info_سرخی) + حجم(*fb_info);
    }

    لفظ_t mb_mmap_سائز = حجم(seL4_X86_BootInfo_mmap_t);
    extra_bi_سائز += mb_mmap_سائز;

    // room for tsc frequency
    extra_bi_سائز += حجم(se_L4_Boot_Info_سرخی) + 4;
    لفظ_t extra_bi_سائز_بٹ = calculate_extra_bi_سائز_بٹ(extra_bi_سائز);

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    it_v_reg.آغاز = ui_v_reg.آغاز;
    it_v_reg.انتہا = باری_اوپر(extra_bi_چوکھٹا_vptr + (extra_bi_سائز_بٹ > 0 ? ثنائی_ہندسہ(extra_bi_سائز_بٹ) : 0), صفحہ_بٹ);
#اگر_تعریف_شدہ CONFIG_IOMMU
    /* calculate the number of io pts before initialising memory */
    اگر (!vtd_init_عدد_iopts(عدد_drhu)) {
        واپس false;
    }
#اگر_ختم /* CONFIG_IOMMU */

    اگر (!arch_init_freemem(ui_info.p_reg, it_v_reg, mem_p_regs, extra_bi_سائز_بٹ)) {
        لکھو("ERROR: free memory management initialization failed\n");
        واپس false;
    }

    /* create the root cnode */
    جڑ_cnode_cap = create_جڑ_cnode();

    /* create the IO port cap */
    write_خانہ(
        خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapIOPortControl),
        cap_io_پورٹ_control_cap_new()
    );

    /* create the cap for managing thread domains */
    create_domain_cap(جڑ_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(جڑ_cnode_cap);

    tsc_freq = tsc_init();

    /* populate the bootinfo frame */
    populate_bi_چوکھٹا(0, ks_عدد_CPUs, ipcbuf_vptr, extra_bi_سائز);
    region_t extra_bi_region = {
        .آغاز = rootserver.extra_bi,
        .انتہا = rootserver.extra_bi + (extra_bi_سائز_بٹ > 0 ? ثنائی_ہندسہ(extra_bi_سائز_بٹ) : 0)
    };

    /* populate vbe info block */
    اگر (vbe->vbe_منوال != -1) {
        vbe->سرخی.id = SEL4_BOOTINFO_سرخی_X86_VBE;
        vbe->سرخی.len = حجم(seL4_X86_BootInfo_VBE);
        حافظہ_نقل((خالی *)(rootserver.extra_bi + extra_bi_انحراف), vbe, حجم(seL4_X86_BootInfo_VBE));
        extra_bi_انحراف += حجم(seL4_X86_BootInfo_VBE);
    }

    /* populate acpi rsdp block */
    اگر (acpi_rsdp) {
        se_L4_Boot_Info_سرخی سرخی;
        سرخی.id = SEL4_BOOTINFO_سرخی_X86_ACPI_RSDP;
        سرخی.len = حجم(سرخی) + حجم(*acpi_rsdp);
        *(se_L4_Boot_Info_سرخی *)(rootserver.extra_bi + extra_bi_انحراف) = سرخی;
        extra_bi_انحراف += حجم(سرخی);
        حافظہ_نقل((خالی *)(rootserver.extra_bi + extra_bi_انحراف), acpi_rsdp, حجم(*acpi_rsdp));
        extra_bi_انحراف += حجم(*acpi_rsdp);
    }

    /* populate framebuffer information block */
    اگر (fb_info && fb_info->addr) {
        se_L4_Boot_Info_سرخی سرخی;
        سرخی.id = SEL4_BOOTINFO_سرخی_X86_FRAMEBUFFER;
        سرخی.len = حجم(سرخی) + حجم(*fb_info);
        *(se_L4_Boot_Info_سرخی *)(rootserver.extra_bi + extra_bi_انحراف) = سرخی;
        extra_bi_انحراف += حجم(سرخی);
        حافظہ_نقل((خالی *)(rootserver.extra_bi + extra_bi_انحراف), fb_info, حجم(*fb_info));
        extra_bi_انحراف += حجم(*fb_info);
    }

    /* populate multiboot mmap block */
    mb_mmap->سرخی.id = SEL4_BOOTINFO_سرخی_X86_MBMMAP;
    mb_mmap->سرخی.len = mb_mmap_سائز;
    حافظہ_نقل((خالی *)(rootserver.extra_bi + extra_bi_انحراف), mb_mmap, mb_mmap_سائز);
    extra_bi_انحراف += mb_mmap_سائز;

    /* populate tsc frequency block */
    {
        se_L4_Boot_Info_سرخی سرخی;
        سرخی.id = SEL4_BOOTINFO_سرخی_X86_TSC_FREQ;
        سرخی.len = حجم(سرخی) + 4;
        *(se_L4_Boot_Info_سرخی *)(extra_bi_region.آغاز + extra_bi_انحراف) = سرخی;
        extra_bi_انحراف += حجم(سرخی);
        *(uint32_t *)(extra_bi_region.آغاز + extra_bi_انحراف) = tsc_freq;
        extra_bi_انحراف += 4;
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    /* set up sched control for each core */
    init_sched_control(جڑ_cnode_cap, CONFIG_MAX_عدد_NODES);
#اگر_ختم

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_vspace_cap = create_it_پتہ_space(جڑ_cnode_cap, it_v_reg);
    اگر (cap_get_capType(it_vspace_cap) == cap_null_cap) {
        واپس false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_چوکھٹا_cap(
        جڑ_cnode_cap,
        it_vspace_cap,
        bi_چوکھٹا_vptr
    );

    /* create and map extra bootinfo region */
    extra_bi_ret =
        create_چوکھٹے_کا_region(
            جڑ_cnode_cap,
            it_vspace_cap,
            extra_bi_region,
            true,
            pptr_مقصد_paddr((خالی *)(extra_bi_region.آغاز - extra_bi_چوکھٹا_vptr))
        );
    اگر (!extra_bi_ret.success) {
        واپس false;
    }
    ndks_boot.bi_چوکھٹا->extraBIPages = extra_bi_ret.region;

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_چوکھٹا_cap(جڑ_cnode_cap, it_vspace_cap, ipcbuf_vptr);
    اگر (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        واپس false;
    }

    /* create all userland image frames */
    create_چوکھٹے_ret =
        create_چوکھٹے_کا_region(
            جڑ_cnode_cap,
            it_vspace_cap,
            ui_reg,
            true,
            ui_info.pv_انحراف
        );
    اگر (!create_چوکھٹے_ret.success) {
        واپس false;
    }
    ndks_boot.bi_چوکھٹا->user_Image_چوکھٹے = create_چوکھٹے_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(جڑ_cnode_cap);
    اگر (cap_get_capType(it_ap_cap) == cap_null_cap) {
        واپس false;
    }
    write_it_asid_pool(it_ap_cap, it_vspace_cap);

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    گرہ_STATE(ks_Cur_وقت) = get_Current_وقت();
#اگر_ختم

    /* create the idle thread */
    create_idle_thread();

    /* copy i387 FPU initial state from FPU */
    saveFpuState(گرہ_STATE(ksIdleThread));
    x86KSnullFpuState = گرہ_STATE(ksIdleThread)->tcbArch.tcbContext.fpuState;
    /* Check that we can load it: */
    بھرو_Fpu_State(گرہ_STATE(ksIdleThread));

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(جڑ_cnode_cap,
                                           it_vspace_cap,
                                           ui_info.v_اندراج,
                                           bi_چوکھٹا_vptr,
                                           ipcbuf_vptr,
                                           ipcbuf_cap);
    اگر (initial == تہی) {
        واپس false;
    }
    init_core_state(initial);

#اگر_تعریف_شدہ CONFIG_IOMMU
    /* initialise VTD-related data structures and the IOMMUs */
    اگر (!vtd_init(cpu_id, rmrr_list)) {
        واپس false;
    }

    /* write number of IOMMU PT levels into bootinfo */
    ndks_boot.bi_چوکھٹا->عدد_IOPTLevels = x86KSnumIOPTLevels;

    /* write IOSpace master cap */
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapIOSpace), master_iospace_cap());
#ورنہ
    ndks_boot.bi_چوکھٹا->عدد_IOPTLevels = -1;
#اگر_ختم

    /* create all of the untypeds. Both devices and kernel window memory */
    اگر (!create_untypeds(جڑ_cnode_cap)) {
        واپس false;
    }

    /* finalise the bootinfo frame */
    bi_finalise();

    واپس true;
}

/* This function initialises the CPU. It does NOT initialise any kernel state. */

BOOT_رمز bool_t init_cpu(
    bool_t   نقاب_legacy_irqs
)
{
    /* initialise virtual-memory-related data structures */
    اگر (!init_vm_state()) {
        واپس false;
    }

    /* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */
    init_dtrs();

    اگر (config_سیٹ(CONFIG_SYSENTER)) {
        /* initialise MSRs (needs an initialised TSS) */
        init_sysenter_msrs();
    } ورنہ اگر (config_سیٹ(CONFIG_SYSCALL)) {
        init_syscall_msrs();
    } ورنہ {
        واپس false;
    }

    /* setup additional PAT MSR */
    اگر (!init_pat_msr()) {
        واپس false;
    }

    /* enable the Write Protect bit in cr0. This prevents the kernel from writing to
     * read only memory, which we shouldn't do under correct execution */
    write_cr0(read_cr0() | CR0_WRITE_PROTECT);

    /* check for SMAP and SMEP and enable */
    cpuid_007h_ebx_t ebx_007;
    ebx_007.الفاظ[0] = x86_cpuid_ebx(0x7, 0);
    اگر (cpuid_007h_ebx_get_smap(ebx_007)) {
        /* if we have user stack trace enabled or dangerous code injection then we cannot
         * enable this as SMAP will make them fault. */
        اگر (!config_سیٹ(CONFIG_PRINTING) && !config_سیٹ(CONFIG_DANGEROUS_رمز_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMAP);
        }
    }
    اگر (cpuid_007h_ebx_get_smep(ebx_007)) {
        /* similar to smap we cannot enable smep if using dangerous code injection. it
         * does not affect stack trace printing though */
        اگر (!config_سیٹ(CONFIG_DANGEROUS_رمز_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMEP);
        }
    }

    اگر (!init_ibrs()) {
        واپس false;
    }

#اگر_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
    /* Initialize hardware breakpoints */
    Arch_initHardwareBreakpoints();
#اگر_ختم

    /* initialise floating-point unit */
    اگر (!Arch_initFpu()) {
        واپس false;
    }

    /* initialise local APIC */
    اگر (!apic_init(نقاب_legacy_irqs)) {
        واپس false;
    }

#اگر_تعریف_شدہ CONFIG_DEBUG_DISABLE_PREFETCHERS
    اگر (!disablePrefetchers()) {
        واپس false;
    }
#اگر_ختم

    اگر (config_سیٹ(CONFIG_EXPORT_PMC_USER)) {
        enablePMCUser();
    }

#اگر_تعریف_شدہ CONFIG_VTX
    /* initialise Intel VT-x extensions */
    اگر (!vtx_init()) {
        واپس false;
    }
#اگر_ختم

    واپس true;
}
