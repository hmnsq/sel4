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

#تعریف_کرو MAX_RESERVED 1
BOOT_BSS ساكن region_t reserved[MAX_RESERVED];

/* functions exactly corresponding to abstract specification */

BOOT_کوڈ ساكن فراغ init_irqs(cap_t جڑ_cnode_cap)
{
    irq_t i;

    لكل (i = 0; i <= maxIRQ; i++) {
        إذا (i == irq_timer) {
            سیٹ_IRQState(IRQTimer, i);
#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
        } وإلا إذا (i == irq_remote_call_ipi || i == irq_reschedule_ipi) {
            سیٹ_IRQState(IRQIPI, i);
#جے_انت /* ENABLE_SMP_SUPPORT */
#جے_تعریف_شدہ CONFIG_IOMMU
        } وإلا إذا (i == irq_iommu) {
            سیٹ_IRQState(IRQReserved, i);
#جے_انت
        } وإلا إذا (i == 2 && config_سیٹ(CONFIG_IRQ_PIC)) {
            /* cascaded legacy PIC */
            سیٹ_IRQState(IRQReserved, i);
        } وإلا إذا (i >= irq_isa_min && i <= irq_isa_max) {
            إذا (config_سیٹ(CONFIG_IRQ_PIC)) {
                سیٹ_IRQState(IRQInactive, i);
            } وإلا {
                سیٹ_IRQState(IRQReserved, i);
            }
        } وإلا إذا (i >= irq_user_min && i <= irq_user_max) {
            إذا (config_سیٹ(CONFIG_IRQ_IOAPIC)) {
                سیٹ_IRQState(IRQInactive, i);
            } وإلا {
                سیٹ_IRQState(IRQReserved, i);
            }
        } وإلا {
            سیٹ_IRQState(IRQReserved, i);
        }
    }
    Arch_irqStateInit();
    /* provide the IRQ control cap */
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

BOOT_کوڈ ساكن bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          v_region_t it_v_reg,
                                          mem_p_regs_t *mem_p_regs,
                                          لفظ_t extra_bi_ناپ_بٹ)
{
    // Extend the reserved region down to include the base of the kernel image.
    // KERNEL_ELF_PADDR_BASE is the lowest physical load address used
    // in the x86 linker script.
    ui_p_reg.شروع = KERNEL_ELF_PADDR_ادھار;
    reserved[0] = paddr_منزل_pptr_reg(ui_p_reg);
    أرجع init_freemem(mem_p_regs->گنتی, mem_p_regs->list, MAX_RESERVED,
                        reserved, it_v_reg, extra_bi_ناپ_بٹ);
}

/* This function initialises a node's kernel state. It does NOT initialise the CPU. */

BOOT_کوڈ bool_t init_sys_state(
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
    لفظ_t        extra_bi_ناپ = 0;
    pptr_t        extra_bi_وتھ = 0;
    uint32_t      tsc_freq;
    create_چوکھٹے_دا_region_ret_t create_چوکھٹے_ret;
    create_چوکھٹے_دا_region_ret_t extra_bi_ret;

    /* convert from physical addresses to kernel pptrs */
    region_t ui_reg             = paddr_منزل_pptr_reg(ui_info.p_reg);

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg;
    v_region_t it_v_reg;
    ui_v_reg.شروع = ui_info.p_reg.شروع - ui_info.pv_وتھ;
    ui_v_reg.سرا   = ui_info.p_reg.سرا   - ui_info.pv_وتھ;

    ipcbuf_vptr = ui_v_reg.سرا;
    bi_چوکھٹا_vptr = ipcbuf_vptr + ثنائی_ہندسہ(ورقہ_بٹ);
    extra_bi_چوکھٹا_vptr = bi_چوکھٹا_vptr + ثنائی_ہندسہ(se_L4_Boot_Info_چوکھٹا_بٹ);

    إذا (vbe->vbe_منوال != -1) {
        extra_bi_ناپ += حجم(seL4_X86_BootInfo_VBE);
    }
    إذا (acpi_rsdp) {
        extra_bi_ناپ += حجم(se_L4_Boot_Info_سرخی) + حجم(*acpi_rsdp);
    }
    إذا (fb_info && fb_info->addr) {
        extra_bi_ناپ += حجم(se_L4_Boot_Info_سرخی) + حجم(*fb_info);
    }

    لفظ_t mb_mmap_ناپ = حجم(seL4_X86_BootInfo_mmap_t);
    extra_bi_ناپ += mb_mmap_ناپ;

    // room for tsc frequency
    extra_bi_ناپ += حجم(se_L4_Boot_Info_سرخی) + 4;
    لفظ_t extra_bi_ناپ_بٹ = calculate_extra_bi_ناپ_بٹ(extra_bi_ناپ);

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    it_v_reg.شروع = ui_v_reg.شروع;
    it_v_reg.سرا = گیڑا_اتے(extra_bi_چوکھٹا_vptr + (extra_bi_ناپ_بٹ > 0 ? ثنائی_ہندسہ(extra_bi_ناپ_بٹ) : 0), ورقہ_بٹ);
#جے_تعریف_شدہ CONFIG_IOMMU
    /* calculate the number of io pts before initialising memory */
    إذا (!vtd_init_عدد_iopts(عدد_drhu)) {
        أرجع false;
    }
#جے_انت /* CONFIG_IOMMU */

    إذا (!arch_init_freemem(ui_info.p_reg, it_v_reg, mem_p_regs, extra_bi_ناپ_بٹ)) {
        لکھو("ERROR: free memory management initialization failed\n");
        أرجع false;
    }

    /* create the root cnode */
    جڑ_cnode_cap = create_جڑ_cnode();

    /* create the IO port cap */
    write_تھاں(
        تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapIOPortControl),
        cap_io_پورٹ_control_cap_new()
    );

    /* create the cap for managing thread domains */
    create_domain_cap(جڑ_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(جڑ_cnode_cap);

    tsc_freq = tsc_init();

    /* populate the bootinfo frame */
    populate_bi_چوکھٹا(0, ks_عدد_CPUs, ipcbuf_vptr, extra_bi_ناپ);
    region_t extra_bi_region = {
        .شروع = rootserver.extra_bi,
        .سرا = rootserver.extra_bi + (extra_bi_ناپ_بٹ > 0 ? ثنائی_ہندسہ(extra_bi_ناپ_بٹ) : 0)
    };

    /* populate vbe info block */
    إذا (vbe->vbe_منوال != -1) {
        vbe->سرخی.id = SEL4_BOOTINFO_سرخی_X86_VBE;
        vbe->سرخی.len = حجم(seL4_X86_BootInfo_VBE);
        حافظہ_نقل((فراغ *)(rootserver.extra_bi + extra_bi_وتھ), vbe, حجم(seL4_X86_BootInfo_VBE));
        extra_bi_وتھ += حجم(seL4_X86_BootInfo_VBE);
    }

    /* populate acpi rsdp block */
    إذا (acpi_rsdp) {
        se_L4_Boot_Info_سرخی سرخی;
        سرخی.id = SEL4_BOOTINFO_سرخی_X86_ACPI_RSDP;
        سرخی.len = حجم(سرخی) + حجم(*acpi_rsdp);
        *(se_L4_Boot_Info_سرخی *)(rootserver.extra_bi + extra_bi_وتھ) = سرخی;
        extra_bi_وتھ += حجم(سرخی);
        حافظہ_نقل((فراغ *)(rootserver.extra_bi + extra_bi_وتھ), acpi_rsdp, حجم(*acpi_rsdp));
        extra_bi_وتھ += حجم(*acpi_rsdp);
    }

    /* populate framebuffer information block */
    إذا (fb_info && fb_info->addr) {
        se_L4_Boot_Info_سرخی سرخی;
        سرخی.id = SEL4_BOOTINFO_سرخی_X86_FRAMEBUFFER;
        سرخی.len = حجم(سرخی) + حجم(*fb_info);
        *(se_L4_Boot_Info_سرخی *)(rootserver.extra_bi + extra_bi_وتھ) = سرخی;
        extra_bi_وتھ += حجم(سرخی);
        حافظہ_نقل((فراغ *)(rootserver.extra_bi + extra_bi_وتھ), fb_info, حجم(*fb_info));
        extra_bi_وتھ += حجم(*fb_info);
    }

    /* populate multiboot mmap block */
    mb_mmap->سرخی.id = SEL4_BOOTINFO_سرخی_X86_MBMMAP;
    mb_mmap->سرخی.len = mb_mmap_ناپ;
    حافظہ_نقل((فراغ *)(rootserver.extra_bi + extra_bi_وتھ), mb_mmap, mb_mmap_ناپ);
    extra_bi_وتھ += mb_mmap_ناپ;

    /* populate tsc frequency block */
    {
        se_L4_Boot_Info_سرخی سرخی;
        سرخی.id = SEL4_BOOTINFO_سرخی_X86_TSC_FREQ;
        سرخی.len = حجم(سرخی) + 4;
        *(se_L4_Boot_Info_سرخی *)(extra_bi_region.شروع + extra_bi_وتھ) = سرخی;
        extra_bi_وتھ += حجم(سرخی);
        *(uint32_t *)(extra_bi_region.شروع + extra_bi_وتھ) = tsc_freq;
        extra_bi_وتھ += 4;
    }

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    /* set up sched control for each core */
    init_sched_control(جڑ_cnode_cap, CONFIG_MAX_عدد_NODES);
#جے_انت

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_vspace_cap = create_it_پتہ_space(جڑ_cnode_cap, it_v_reg);
    إذا (cap_get_capType(it_vspace_cap) == cap_null_cap) {
        أرجع false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_چوکھٹا_cap(
        جڑ_cnode_cap,
        it_vspace_cap,
        bi_چوکھٹا_vptr
    );

    /* create and map extra bootinfo region */
    extra_bi_ret =
        create_چوکھٹے_دا_region(
            جڑ_cnode_cap,
            it_vspace_cap,
            extra_bi_region,
            true,
            pptr_منزل_paddr((فراغ *)(extra_bi_region.شروع - extra_bi_چوکھٹا_vptr))
        );
    إذا (!extra_bi_ret.success) {
        أرجع false;
    }
    ndks_boot.bi_چوکھٹا->extraBIPages = extra_bi_ret.region;

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_چوکھٹا_cap(جڑ_cnode_cap, it_vspace_cap, ipcbuf_vptr);
    إذا (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        أرجع false;
    }

    /* create all userland image frames */
    create_چوکھٹے_ret =
        create_چوکھٹے_دا_region(
            جڑ_cnode_cap,
            it_vspace_cap,
            ui_reg,
            true,
            ui_info.pv_وتھ
        );
    إذا (!create_چوکھٹے_ret.success) {
        أرجع false;
    }
    ndks_boot.bi_چوکھٹا->user_Image_چوکھٹے = create_چوکھٹے_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(جڑ_cnode_cap);
    إذا (cap_get_capType(it_ap_cap) == cap_null_cap) {
        أرجع false;
    }
    write_it_asid_pool(it_ap_cap, it_vspace_cap);

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    گرہ_STATE(ks_Cur_ویلا) = get_Current_ویلا();
#جے_انت

    /* create the idle thread */
    create_idle_thread();

    /* copy i387 FPU initial state from FPU */
    saveFpuState(گرہ_STATE(ksIdleThread));
    x86KSnullFpuState = گرہ_STATE(ksIdleThread)->tcbArch.tcbContext.fpuState;
    /* Check that we can load it: */
    لدو_Fpu_State(گرہ_STATE(ksIdleThread));

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(جڑ_cnode_cap,
                                           it_vspace_cap,
                                           ui_info.v_اندراج,
                                           bi_چوکھٹا_vptr,
                                           ipcbuf_vptr,
                                           ipcbuf_cap);
    إذا (initial == عدم) {
        أرجع false;
    }
    init_core_state(initial);

#جے_تعریف_شدہ CONFIG_IOMMU
    /* initialise VTD-related data structures and the IOMMUs */
    إذا (!vtd_init(cpu_id, rmrr_list)) {
        أرجع false;
    }

    /* write number of IOMMU PT levels into bootinfo */
    ndks_boot.bi_چوکھٹا->عدد_IOPTLevels = x86KSnumIOPTLevels;

    /* write IOSpace master cap */
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapIOSpace), master_iospace_cap());
#وإلا
    ndks_boot.bi_چوکھٹا->عدد_IOPTLevels = -1;
#جے_انت

    /* create all of the untypeds. Both devices and kernel window memory */
    إذا (!create_untypeds(جڑ_cnode_cap)) {
        أرجع false;
    }

    /* finalise the bootinfo frame */
    bi_finalise();

    أرجع true;
}

/* This function initialises the CPU. It does NOT initialise any kernel state. */

BOOT_کوڈ bool_t init_cpu(
    bool_t   نقاب_legacy_irqs
)
{
    /* initialise virtual-memory-related data structures */
    إذا (!init_vm_state()) {
        أرجع false;
    }

    /* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */
    init_dtrs();

    إذا (config_سیٹ(CONFIG_SYSENTER)) {
        /* initialise MSRs (needs an initialised TSS) */
        init_sysenter_msrs();
    } وإلا إذا (config_سیٹ(CONFIG_SYSCALL)) {
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
    ebx_007.الفاظ[0] = x86_cpuid_ebx(0x7, 0);
    إذا (cpuid_007h_ebx_get_smap(ebx_007)) {
        /* if we have user stack trace enabled or dangerous code injection then we cannot
         * enable this as SMAP will make them fault. */
        إذا (!config_سیٹ(CONFIG_PRINTING) && !config_سیٹ(CONFIG_DANGEROUS_کوڈ_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMAP);
        }
    }
    إذا (cpuid_007h_ebx_get_smep(ebx_007)) {
        /* similar to smap we cannot enable smep if using dangerous code injection. it
         * does not affect stack trace printing though */
        إذا (!config_سیٹ(CONFIG_DANGEROUS_کوڈ_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMEP);
        }
    }

    إذا (!init_ibrs()) {
        أرجع false;
    }

#جے_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
    /* Initialize hardware breakpoints */
    Arch_initHardwareBreakpoints();
#جے_انت

    /* initialise floating-point unit */
    إذا (!Arch_initFpu()) {
        أرجع false;
    }

    /* initialise local APIC */
    إذا (!apic_init(نقاب_legacy_irqs)) {
        أرجع false;
    }

#جے_تعریف_شدہ CONFIG_DEBUG_DISABLE_PREFETCHERS
    إذا (!disablePrefetchers()) {
        أرجع false;
    }
#جے_انت

    إذا (config_سیٹ(CONFIG_EXPORT_PMC_USER)) {
        enablePMCUser();
    }

#جے_تعریف_شدہ CONFIG_VTX
    /* initialise Intel VT-x extensions */
    إذا (!vtx_init()) {
        أرجع false;
    }
#جے_انت

    أرجع true;
}
