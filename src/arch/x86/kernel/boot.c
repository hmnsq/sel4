/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <kernel/boot.h>
#ضمن <machine/io.h>
#ضمن <model/statedata.h>
#ضمن <object/interrupt.h>
#ضمن <arch/object/interrupt.h>
#ضمن <arch/machine.h>
#ضمن <arch/kernel/apic.h>
#ضمن <arch/kernel/boot.h>
#ضمن <arch/kernel/boot_sys.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <machine/fpu.h>
#ضمن <arch/machine/timer.h>
#ضمن <arch/object/ioport.h>
#ضمن <linker.h>
#ضمن <util.h>

#ضمن <plat/machine/intel-vtd.h>

#وصف_ڪريو MAX_RESERVED 1
BOOT_BSS ساكن region_t reserved[MAX_RESERVED];

/* functions exactly corresponding to abstract specification */

BOOT_ڪوڊ ساكن فراغ init_irqs(cap_t پاڙ_cnode_cap)
{
    irq_t i;

    لكل (i = 0; i <= maxIRQ; i++) {
        إذا (i == irq_timer) {
            سيٽ_IRQState(IRQTimer, i);
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
        } وإلا إذا (i == irq_remote_call_ipi || i == irq_reschedule_ipi) {
            سيٽ_IRQState(IRQIPI, i);
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */
#جيڪڏهن_وصف_ٿيل CONFIG_IOMMU
        } وإلا إذا (i == irq_iommu) {
            سيٽ_IRQState(IRQReserved, i);
#جيڪڏهن_پورو
        } وإلا إذا (i == 2 && config_سيٽ(CONFIG_IRQ_PIC)) {
            /* cascaded legacy PIC */
            سيٽ_IRQState(IRQReserved, i);
        } وإلا إذا (i >= irq_isa_min && i <= irq_isa_max) {
            إذا (config_سيٽ(CONFIG_IRQ_PIC)) {
                سيٽ_IRQState(IRQInactive, i);
            } وإلا {
                سيٽ_IRQState(IRQReserved, i);
            }
        } وإلا إذا (i >= irq_user_min && i <= irq_user_max) {
            إذا (config_سيٽ(CONFIG_IRQ_IOAPIC)) {
                سيٽ_IRQState(IRQInactive, i);
            } وإلا {
                سيٽ_IRQState(IRQReserved, i);
            }
        } وإلا {
            سيٽ_IRQState(IRQReserved, i);
        }
    }
    Arch_irqStateInit();
    /* provide the IRQ control cap */
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

BOOT_ڪوڊ ساكن bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          v_region_t it_v_reg,
                                          mem_p_regs_t *mem_p_regs,
                                          لفظ_t extra_bi_سائيز_بٽ)
{
    // Extend the reserved region down to include the base of the kernel image.
    // KERNEL_ELF_PADDR_BASE is the lowest physical load address used
    // in the x86 linker script.
    ui_p_reg.شروعات = KERNEL_ELF_PADDR_بنياد;
    reserved[0] = paddr_منزل_pptr_reg(ui_p_reg);
    أرجع init_freemem(mem_p_regs->ڳڻپ, mem_p_regs->list, MAX_RESERVED,
                        reserved, it_v_reg, extra_bi_سائيز_بٽ);
}

/* This function initialises a node's kernel state. It does NOT initialise the CPU. */

BOOT_ڪوڊ bool_t init_sys_state(
    cpu_id_t      cpu_id,
    mem_p_regs_t  *mem_p_regs,
    ui_info_t     ui_info,
    /* parameters below not modeled in abstract specification */
    uint32_t      انگ_drhu,
    paddr_t      *drhu_list,
    acpi_rmrr_list_t *rmrr_list,
    acpi_rsdp_t      *acpi_rsdp,
    seL4_X86_BootInfo_VBE *vbe,
    seL4_X86_BootInfo_mmap_t *mb_mmap,
    seL4_X86_BootInfo_fb_t *fb_info
)
{
    cap_t         پاڙ_cnode_cap;
    vptr_t        extra_bi_چوکٺو_vptr;
    vptr_t        bi_چوکٺو_vptr;
    vptr_t        ipcbuf_vptr;
    cap_t         it_vspace_cap;
    cap_t         it_ap_cap;
    cap_t         ipcbuf_cap;
    لفظ_t        extra_bi_سائيز = 0;
    pptr_t        extra_bi_وٿي = 0;
    uint32_t      tsc_freq;
    create_چوکٺا_جو_region_ret_t create_چوکٺا_ret;
    create_چوکٺا_جو_region_ret_t extra_bi_ret;

    /* convert from physical addresses to kernel pptrs */
    region_t ui_reg             = paddr_منزل_pptr_reg(ui_info.p_reg);

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg;
    v_region_t it_v_reg;
    ui_v_reg.شروعات = ui_info.p_reg.شروعات - ui_info.pv_وٿي;
    ui_v_reg.پڇاڙي   = ui_info.p_reg.پڇاڙي   - ui_info.pv_وٿي;

    ipcbuf_vptr = ui_v_reg.پڇاڙي;
    bi_چوکٺو_vptr = ipcbuf_vptr + ٻٽو_انگ(صفحو_بٽ);
    extra_bi_چوکٺو_vptr = bi_چوکٺو_vptr + ٻٽو_انگ(se_L4_Boot_Info_چوکٺو_بٽ);

    إذا (vbe->vbe_گهڻائي != -1) {
        extra_bi_سائيز += حجم(seL4_X86_BootInfo_VBE);
    }
    إذا (acpi_rsdp) {
        extra_bi_سائيز += حجم(se_L4_Boot_Info_سرو) + حجم(*acpi_rsdp);
    }
    إذا (fb_info && fb_info->addr) {
        extra_bi_سائيز += حجم(se_L4_Boot_Info_سرو) + حجم(*fb_info);
    }

    لفظ_t mb_mmap_سائيز = حجم(seL4_X86_BootInfo_mmap_t);
    extra_bi_سائيز += mb_mmap_سائيز;

    // room for tsc frequency
    extra_bi_سائيز += حجم(se_L4_Boot_Info_سرو) + 4;
    لفظ_t extra_bi_سائيز_بٽ = calculate_extra_bi_سائيز_بٽ(extra_bi_سائيز);

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    it_v_reg.شروعات = ui_v_reg.شروعات;
    it_v_reg.پڇاڙي = ڦيرو_مٿي(extra_bi_چوکٺو_vptr + (extra_bi_سائيز_بٽ > 0 ? ٻٽو_انگ(extra_bi_سائيز_بٽ) : 0), صفحو_بٽ);
#جيڪڏهن_وصف_ٿيل CONFIG_IOMMU
    /* calculate the number of io pts before initialising memory */
    إذا (!vtd_init_انگ_iopts(انگ_drhu)) {
        أرجع false;
    }
#جيڪڏهن_پورو /* CONFIG_IOMMU */

    إذا (!arch_init_freemem(ui_info.p_reg, it_v_reg, mem_p_regs, extra_bi_سائيز_بٽ)) {
        لکو("ERROR: free memory management initialization failed\n");
        أرجع false;
    }

    /* create the root cnode */
    پاڙ_cnode_cap = create_پاڙ_cnode();

    /* create the IO port cap */
    write_جڳهه(
        جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapIOPortControl),
        cap_io_پورٽ_control_cap_new()
    );

    /* create the cap for managing thread domains */
    create_domain_cap(پاڙ_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(پاڙ_cnode_cap);

    tsc_freq = tsc_init();

    /* populate the bootinfo frame */
    populate_bi_چوکٺو(0, ks_انگ_CPUs, ipcbuf_vptr, extra_bi_سائيز);
    region_t extra_bi_region = {
        .شروعات = rootserver.extra_bi,
        .پڇاڙي = rootserver.extra_bi + (extra_bi_سائيز_بٽ > 0 ? ٻٽو_انگ(extra_bi_سائيز_بٽ) : 0)
    };

    /* populate vbe info block */
    إذا (vbe->vbe_گهڻائي != -1) {
        vbe->سرو.id = SEL4_BOOTINFO_سرو_X86_VBE;
        vbe->سرو.len = حجم(seL4_X86_BootInfo_VBE);
        يادگيري_نقل((فراغ *)(rootserver.extra_bi + extra_bi_وٿي), vbe, حجم(seL4_X86_BootInfo_VBE));
        extra_bi_وٿي += حجم(seL4_X86_BootInfo_VBE);
    }

    /* populate acpi rsdp block */
    إذا (acpi_rsdp) {
        se_L4_Boot_Info_سرو سرو;
        سرو.id = SEL4_BOOTINFO_سرو_X86_ACPI_RSDP;
        سرو.len = حجم(سرو) + حجم(*acpi_rsdp);
        *(se_L4_Boot_Info_سرو *)(rootserver.extra_bi + extra_bi_وٿي) = سرو;
        extra_bi_وٿي += حجم(سرو);
        يادگيري_نقل((فراغ *)(rootserver.extra_bi + extra_bi_وٿي), acpi_rsdp, حجم(*acpi_rsdp));
        extra_bi_وٿي += حجم(*acpi_rsdp);
    }

    /* populate framebuffer information block */
    إذا (fb_info && fb_info->addr) {
        se_L4_Boot_Info_سرو سرو;
        سرو.id = SEL4_BOOTINFO_سرو_X86_FRAMEBUFFER;
        سرو.len = حجم(سرو) + حجم(*fb_info);
        *(se_L4_Boot_Info_سرو *)(rootserver.extra_bi + extra_bi_وٿي) = سرو;
        extra_bi_وٿي += حجم(سرو);
        يادگيري_نقل((فراغ *)(rootserver.extra_bi + extra_bi_وٿي), fb_info, حجم(*fb_info));
        extra_bi_وٿي += حجم(*fb_info);
    }

    /* populate multiboot mmap block */
    mb_mmap->سرو.id = SEL4_BOOTINFO_سرو_X86_MBMMAP;
    mb_mmap->سرو.len = mb_mmap_سائيز;
    يادگيري_نقل((فراغ *)(rootserver.extra_bi + extra_bi_وٿي), mb_mmap, mb_mmap_سائيز);
    extra_bi_وٿي += mb_mmap_سائيز;

    /* populate tsc frequency block */
    {
        se_L4_Boot_Info_سرو سرو;
        سرو.id = SEL4_BOOTINFO_سرو_X86_TSC_FREQ;
        سرو.len = حجم(سرو) + 4;
        *(se_L4_Boot_Info_سرو *)(extra_bi_region.شروعات + extra_bi_وٿي) = سرو;
        extra_bi_وٿي += حجم(سرو);
        *(uint32_t *)(extra_bi_region.شروعات + extra_bi_وٿي) = tsc_freq;
        extra_bi_وٿي += 4;
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    /* set up sched control for each core */
    init_sched_control(پاڙ_cnode_cap, CONFIG_MAX_انگ_NODES);
#جيڪڏهن_پورو

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_vspace_cap = create_it_پتو_space(پاڙ_cnode_cap, it_v_reg);
    إذا (cap_get_capType(it_vspace_cap) == cap_null_cap) {
        أرجع false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_چوکٺو_cap(
        پاڙ_cnode_cap,
        it_vspace_cap,
        bi_چوکٺو_vptr
    );

    /* create and map extra bootinfo region */
    extra_bi_ret =
        create_چوکٺا_جو_region(
            پاڙ_cnode_cap,
            it_vspace_cap,
            extra_bi_region,
            true,
            pptr_منزل_paddr((فراغ *)(extra_bi_region.شروعات - extra_bi_چوکٺو_vptr))
        );
    إذا (!extra_bi_ret.success) {
        أرجع false;
    }
    ndks_boot.bi_چوکٺو->extraBIPages = extra_bi_ret.region;

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_چوکٺو_cap(پاڙ_cnode_cap, it_vspace_cap, ipcbuf_vptr);
    إذا (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        أرجع false;
    }

    /* create all userland image frames */
    create_چوکٺا_ret =
        create_چوکٺا_جو_region(
            پاڙ_cnode_cap,
            it_vspace_cap,
            ui_reg,
            true,
            ui_info.pv_وٿي
        );
    إذا (!create_چوکٺا_ret.success) {
        أرجع false;
    }
    ndks_boot.bi_چوکٺو->user_Image_چوکٺا = create_چوکٺا_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(پاڙ_cnode_cap);
    إذا (cap_get_capType(it_ap_cap) == cap_null_cap) {
        أرجع false;
    }
    write_it_asid_pool(it_ap_cap, it_vspace_cap);

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    ڳنڍ_STATE(ks_Cur_وقت) = get_Current_وقت();
#جيڪڏهن_پورو

    /* create the idle thread */
    create_idle_thread();

    /* copy i387 FPU initial state from FPU */
    saveFpuState(ڳنڍ_STATE(ksIdleThread));
    x86KSnullFpuState = ڳنڍ_STATE(ksIdleThread)->tcbArch.tcbContext.fpuState;
    /* Check that we can load it: */
    ڀريو_Fpu_State(ڳنڍ_STATE(ksIdleThread));

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(پاڙ_cnode_cap,
                                           it_vspace_cap,
                                           ui_info.v_داخلا,
                                           bi_چوکٺو_vptr,
                                           ipcbuf_vptr,
                                           ipcbuf_cap);
    إذا (initial == عدم) {
        أرجع false;
    }
    init_core_state(initial);

#جيڪڏهن_وصف_ٿيل CONFIG_IOMMU
    /* initialise VTD-related data structures and the IOMMUs */
    إذا (!vtd_init(cpu_id, rmrr_list)) {
        أرجع false;
    }

    /* write number of IOMMU PT levels into bootinfo */
    ndks_boot.bi_چوکٺو->انگ_IOPTLevels = x86KSnumIOPTLevels;

    /* write IOSpace master cap */
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapIOSpace), master_iospace_cap());
#وإلا
    ndks_boot.bi_چوکٺو->انگ_IOPTLevels = -1;
#جيڪڏهن_پورو

    /* create all of the untypeds. Both devices and kernel window memory */
    إذا (!create_untypeds(پاڙ_cnode_cap)) {
        أرجع false;
    }

    /* finalise the bootinfo frame */
    bi_finalise();

    أرجع true;
}

/* This function initialises the CPU. It does NOT initialise any kernel state. */

BOOT_ڪوڊ bool_t init_cpu(
    bool_t   نقاب_legacy_irqs
)
{
    /* initialise virtual-memory-related data structures */
    إذا (!init_vm_state()) {
        أرجع false;
    }

    /* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */
    init_dtrs();

    إذا (config_سيٽ(CONFIG_SYSENTER)) {
        /* initialise MSRs (needs an initialised TSS) */
        init_sysenter_msrs();
    } وإلا إذا (config_سيٽ(CONFIG_SYSCALL)) {
        init_syscall_msrs();
    } وإلا {
        أرجع false;
    }

    /* setup additional PAT MSR */
    إذا (!init_pat_msr()) {
        أرجع false;
    }

    /* enable the Write Protect bit in cr0. This prevents the kernel from writing to
     * read only memory, which we shouldn't do under correct execution */
    write_cr0(read_cr0() | CR0_WRITE_PROTECT);

    /* check for SMAP and SMEP and enable */
    cpuid_007h_ebx_t ebx_007;
    ebx_007.لفظ[0] = x86_cpuid_ebx(0x7, 0);
    إذا (cpuid_007h_ebx_get_smap(ebx_007)) {
        /* if we have user stack trace enabled or dangerous code injection then we cannot
         * enable this as SMAP will make them fault. */
        إذا (!config_سيٽ(CONFIG_PRINTING) && !config_سيٽ(CONFIG_DANGEROUS_ڪوڊ_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMAP);
        }
    }
    إذا (cpuid_007h_ebx_get_smep(ebx_007)) {
        /* similar to smap we cannot enable smep if using dangerous code injection. it
         * does not affect stack trace printing though */
        إذا (!config_سيٽ(CONFIG_DANGEROUS_ڪوڊ_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMEP);
        }
    }

    إذا (!init_ibrs()) {
        أرجع false;
    }

#جيڪڏهن_وصف_ٿيل CONFIG_HARDWARE_DEBUG_API
    /* Initialize hardware breakpoints */
    Arch_initHardwareBreakpoints();
#جيڪڏهن_پورو

    /* initialise floating-point unit */
    إذا (!Arch_initFpu()) {
        أرجع false;
    }

    /* initialise local APIC */
    إذا (!apic_init(نقاب_legacy_irqs)) {
        أرجع false;
    }

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_DISABLE_PREFETCHERS
    إذا (!disablePrefetchers()) {
        أرجع false;
    }
#جيڪڏهن_پورو

    إذا (config_سيٽ(CONFIG_EXPORT_PMC_USER)) {
        enablePMCUser();
    }

#جيڪڏهن_وصف_ٿيل CONFIG_VTX
    /* initialise Intel VT-x extensions */
    إذا (!vtx_init()) {
        أرجع false;
    }
#جيڪڏهن_پورو

    أرجع true;
}
