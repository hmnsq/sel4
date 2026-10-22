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

#تعریف_کن MAX_RESERVED 1
BOOT_BSS ساكن region_t reserved[MAX_RESERVED];

/* functions exactly corresponding to abstract specification */

BOOT_کد ساكن فراغ init_irqs(cap_t ریشه_cnode_cap)
{
    irq_t i;

    لكل (i = 0; i <= maxIRQ; i++) {
        إذا (i == irq_timer) {
            مجموعه_IRQState(IRQTimer, i);
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
        } وإلا إذا (i == irq_remote_call_ipi || i == irq_reschedule_ipi) {
            مجموعه_IRQState(IRQIPI, i);
#پایان_اگر /* ENABLE_SMP_SUPPORT */
#اگر_تعریف_شده CONFIG_IOMMU
        } وإلا إذا (i == irq_iommu) {
            مجموعه_IRQState(IRQReserved, i);
#پایان_اگر
        } وإلا إذا (i == 2 && config_مجموعه(CONFIG_IRQ_PIC)) {
            /* cascaded legacy PIC */
            مجموعه_IRQState(IRQReserved, i);
        } وإلا إذا (i >= irq_isa_min && i <= irq_isa_max) {
            إذا (config_مجموعه(CONFIG_IRQ_PIC)) {
                مجموعه_IRQState(IRQInactive, i);
            } وإلا {
                مجموعه_IRQState(IRQReserved, i);
            }
        } وإلا إذا (i >= irq_user_min && i <= irq_user_max) {
            إذا (config_مجموعه(CONFIG_IRQ_IOAPIC)) {
                مجموعه_IRQState(IRQInactive, i);
            } وإلا {
                مجموعه_IRQState(IRQReserved, i);
            }
        } وإلا {
            مجموعه_IRQState(IRQReserved, i);
        }
    }
    Arch_irqStateInit();
    /* provide the IRQ control cap */
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

BOOT_کد ساكن bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          v_region_t it_v_reg,
                                          mem_p_regs_t *mem_p_regs,
                                          کلمه_t extra_bi_اندازه_بیتها)
{
    // Extend the reserved region down to include the base of the kernel image.
    // KERNEL_ELF_PADDR_BASE is the lowest physical load address used
    // in the x86 linker script.
    ui_p_reg.شروع = KERNEL_ELF_PADDR_پایه;
    reserved[0] = paddr_مقصد_pptr_reg(ui_p_reg);
    أرجع init_freemem(mem_p_regs->شمار, mem_p_regs->list, MAX_RESERVED,
                        reserved, it_v_reg, extra_bi_اندازه_بیتها);
}

/* This function initialises a node's kernel state. It does NOT initialise the CPU. */

BOOT_کد bool_t init_sys_state(
    cpu_id_t      cpu_id,
    mem_p_regs_t  *mem_p_regs,
    ui_info_t     ui_info,
    /* parameters below not modeled in abstract specification */
    uint32_t      رقم_drhu,
    paddr_t      *drhu_list,
    acpi_rmrr_list_t *rmrr_list,
    acpi_rsdp_t      *acpi_rsdp,
    seL4_X86_BootInfo_VBE *vbe,
    seL4_X86_BootInfo_mmap_t *mb_mmap,
    seL4_X86_BootInfo_fb_t *fb_info
)
{
    cap_t         ریشه_cnode_cap;
    vptr_t        extra_bi_قاب_vptr;
    vptr_t        bi_قاب_vptr;
    vptr_t        ipcbuf_vptr;
    cap_t         it_vspace_cap;
    cap_t         it_ap_cap;
    cap_t         ipcbuf_cap;
    کلمه_t        extra_bi_اندازه = 0;
    pptr_t        extra_bi_جابه_جایی = 0;
    uint32_t      tsc_freq;
    create_قابها_از_region_ret_t create_قابها_ret;
    create_قابها_از_region_ret_t extra_bi_ret;

    /* convert from physical addresses to kernel pptrs */
    region_t ui_reg             = paddr_مقصد_pptr_reg(ui_info.p_reg);

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg;
    v_region_t it_v_reg;
    ui_v_reg.شروع = ui_info.p_reg.شروع - ui_info.pv_جابه_جایی;
    ui_v_reg.انتها   = ui_info.p_reg.انتها   - ui_info.pv_جابه_جایی;

    ipcbuf_vptr = ui_v_reg.انتها;
    bi_قاب_vptr = ipcbuf_vptr + بیت(صفحه_بیتها);
    extra_bi_قاب_vptr = bi_قاب_vptr + بیت(se_L4_Boot_Info_قاب_بیتها);

    إذا (vbe->vbe_نما != -1) {
        extra_bi_اندازه += حجم(seL4_X86_BootInfo_VBE);
    }
    إذا (acpi_rsdp) {
        extra_bi_اندازه += حجم(se_L4_Boot_Info_سرآیند) + حجم(*acpi_rsdp);
    }
    إذا (fb_info && fb_info->addr) {
        extra_bi_اندازه += حجم(se_L4_Boot_Info_سرآیند) + حجم(*fb_info);
    }

    کلمه_t mb_mmap_اندازه = حجم(seL4_X86_BootInfo_mmap_t);
    extra_bi_اندازه += mb_mmap_اندازه;

    // room for tsc frequency
    extra_bi_اندازه += حجم(se_L4_Boot_Info_سرآیند) + 4;
    کلمه_t extra_bi_اندازه_بیتها = calculate_extra_bi_اندازه_بیتها(extra_bi_اندازه);

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    it_v_reg.شروع = ui_v_reg.شروع;
    it_v_reg.انتها = دور_به_بالا(extra_bi_قاب_vptr + (extra_bi_اندازه_بیتها > 0 ? بیت(extra_bi_اندازه_بیتها) : 0), صفحه_بیتها);
#اگر_تعریف_شده CONFIG_IOMMU
    /* calculate the number of io pts before initialising memory */
    إذا (!vtd_init_رقم_iopts(رقم_drhu)) {
        أرجع false;
    }
#پایان_اگر /* CONFIG_IOMMU */

    إذا (!arch_init_freemem(ui_info.p_reg, it_v_reg, mem_p_regs, extra_bi_اندازه_بیتها)) {
        چاپ_کن("ERROR: free memory management initialization failed\n");
        أرجع false;
    }

    /* create the root cnode */
    ریشه_cnode_cap = create_ریشه_cnode();

    /* create the IO port cap */
    write_جایگاه(
        جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapIOPortControl),
        cap_io_پورت_control_cap_new()
    );

    /* create the cap for managing thread domains */
    create_domain_cap(ریشه_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(ریشه_cnode_cap);

    tsc_freq = tsc_init();

    /* populate the bootinfo frame */
    populate_bi_قاب(0, ks_رقم_CPUs, ipcbuf_vptr, extra_bi_اندازه);
    region_t extra_bi_region = {
        .شروع = rootserver.extra_bi,
        .انتها = rootserver.extra_bi + (extra_bi_اندازه_بیتها > 0 ? بیت(extra_bi_اندازه_بیتها) : 0)
    };

    /* populate vbe info block */
    إذا (vbe->vbe_نما != -1) {
        vbe->سرآیند.id = SEL4_BOOTINFO_سرآیند_X86_VBE;
        vbe->سرآیند.len = حجم(seL4_X86_BootInfo_VBE);
        کپی_حافظه((فراغ *)(rootserver.extra_bi + extra_bi_جابه_جایی), vbe, حجم(seL4_X86_BootInfo_VBE));
        extra_bi_جابه_جایی += حجم(seL4_X86_BootInfo_VBE);
    }

    /* populate acpi rsdp block */
    إذا (acpi_rsdp) {
        se_L4_Boot_Info_سرآیند سرآیند;
        سرآیند.id = SEL4_BOOTINFO_سرآیند_X86_ACPI_RSDP;
        سرآیند.len = حجم(سرآیند) + حجم(*acpi_rsdp);
        *(se_L4_Boot_Info_سرآیند *)(rootserver.extra_bi + extra_bi_جابه_جایی) = سرآیند;
        extra_bi_جابه_جایی += حجم(سرآیند);
        کپی_حافظه((فراغ *)(rootserver.extra_bi + extra_bi_جابه_جایی), acpi_rsdp, حجم(*acpi_rsdp));
        extra_bi_جابه_جایی += حجم(*acpi_rsdp);
    }

    /* populate framebuffer information block */
    إذا (fb_info && fb_info->addr) {
        se_L4_Boot_Info_سرآیند سرآیند;
        سرآیند.id = SEL4_BOOTINFO_سرآیند_X86_FRAMEBUFFER;
        سرآیند.len = حجم(سرآیند) + حجم(*fb_info);
        *(se_L4_Boot_Info_سرآیند *)(rootserver.extra_bi + extra_bi_جابه_جایی) = سرآیند;
        extra_bi_جابه_جایی += حجم(سرآیند);
        کپی_حافظه((فراغ *)(rootserver.extra_bi + extra_bi_جابه_جایی), fb_info, حجم(*fb_info));
        extra_bi_جابه_جایی += حجم(*fb_info);
    }

    /* populate multiboot mmap block */
    mb_mmap->سرآیند.id = SEL4_BOOTINFO_سرآیند_X86_MBMMAP;
    mb_mmap->سرآیند.len = mb_mmap_اندازه;
    کپی_حافظه((فراغ *)(rootserver.extra_bi + extra_bi_جابه_جایی), mb_mmap, mb_mmap_اندازه);
    extra_bi_جابه_جایی += mb_mmap_اندازه;

    /* populate tsc frequency block */
    {
        se_L4_Boot_Info_سرآیند سرآیند;
        سرآیند.id = SEL4_BOOTINFO_سرآیند_X86_TSC_FREQ;
        سرآیند.len = حجم(سرآیند) + 4;
        *(se_L4_Boot_Info_سرآیند *)(extra_bi_region.شروع + extra_bi_جابه_جایی) = سرآیند;
        extra_bi_جابه_جایی += حجم(سرآیند);
        *(uint32_t *)(extra_bi_region.شروع + extra_bi_جابه_جایی) = tsc_freq;
        extra_bi_جابه_جایی += 4;
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    /* set up sched control for each core */
    init_sched_control(ریشه_cnode_cap, CONFIG_MAX_رقم_NODES);
#پایان_اگر

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_vspace_cap = create_it_آدرس_space(ریشه_cnode_cap, it_v_reg);
    إذا (cap_get_capType(it_vspace_cap) == cap_null_cap) {
        أرجع false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_قاب_cap(
        ریشه_cnode_cap,
        it_vspace_cap,
        bi_قاب_vptr
    );

    /* create and map extra bootinfo region */
    extra_bi_ret =
        create_قابها_از_region(
            ریشه_cnode_cap,
            it_vspace_cap,
            extra_bi_region,
            true,
            pptr_مقصد_paddr((فراغ *)(extra_bi_region.شروع - extra_bi_قاب_vptr))
        );
    إذا (!extra_bi_ret.success) {
        أرجع false;
    }
    ndks_boot.bi_قاب->extraBIPages = extra_bi_ret.region;

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_قاب_cap(ریشه_cnode_cap, it_vspace_cap, ipcbuf_vptr);
    إذا (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        أرجع false;
    }

    /* create all userland image frames */
    create_قابها_ret =
        create_قابها_از_region(
            ریشه_cnode_cap,
            it_vspace_cap,
            ui_reg,
            true,
            ui_info.pv_جابه_جایی
        );
    إذا (!create_قابها_ret.success) {
        أرجع false;
    }
    ndks_boot.bi_قاب->user_Image_قابها = create_قابها_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(ریشه_cnode_cap);
    إذا (cap_get_capType(it_ap_cap) == cap_null_cap) {
        أرجع false;
    }
    write_it_asid_pool(it_ap_cap, it_vspace_cap);

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    گره_STATE(ks_Cur_زمان) = get_Current_زمان();
#پایان_اگر

    /* create the idle thread */
    create_idle_thread();

    /* copy i387 FPU initial state from FPU */
    saveFpuState(گره_STATE(ksIdleThread));
    x86KSnullFpuState = گره_STATE(ksIdleThread)->tcbArch.tcbContext.fpuState;
    /* Check that we can load it: */
    بارگیری_Fpu_State(گره_STATE(ksIdleThread));

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(ریشه_cnode_cap,
                                           it_vspace_cap,
                                           ui_info.v_مدخل,
                                           bi_قاب_vptr,
                                           ipcbuf_vptr,
                                           ipcbuf_cap);
    إذا (initial == عدم) {
        أرجع false;
    }
    init_core_state(initial);

#اگر_تعریف_شده CONFIG_IOMMU
    /* initialise VTD-related data structures and the IOMMUs */
    إذا (!vtd_init(cpu_id, rmrr_list)) {
        أرجع false;
    }

    /* write number of IOMMU PT levels into bootinfo */
    ndks_boot.bi_قاب->رقم_IOPTLevels = x86KSnumIOPTLevels;

    /* write IOSpace master cap */
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapIOSpace), master_iospace_cap());
#وإلا
    ndks_boot.bi_قاب->رقم_IOPTLevels = -1;
#پایان_اگر

    /* create all of the untypeds. Both devices and kernel window memory */
    إذا (!create_untypeds(ریشه_cnode_cap)) {
        أرجع false;
    }

    /* finalise the bootinfo frame */
    bi_finalise();

    أرجع true;
}

/* This function initialises the CPU. It does NOT initialise any kernel state. */

BOOT_کد bool_t init_cpu(
    bool_t   ماسک_legacy_irqs
)
{
    /* initialise virtual-memory-related data structures */
    إذا (!init_vm_state()) {
        أرجع false;
    }

    /* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */
    init_dtrs();

    إذا (config_مجموعه(CONFIG_SYSENTER)) {
        /* initialise MSRs (needs an initialised TSS) */
        init_sysenter_msrs();
    } وإلا إذا (config_مجموعه(CONFIG_SYSCALL)) {
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
    ebx_007.واژگان[0] = x86_cpuid_ebx(0x7, 0);
    إذا (cpuid_007h_ebx_get_smap(ebx_007)) {
        /* if we have user stack trace enabled or dangerous code injection then we cannot
         * enable this as SMAP will make them fault. */
        إذا (!config_مجموعه(CONFIG_PRINTING) && !config_مجموعه(CONFIG_DANGEROUS_کد_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMAP);
        }
    }
    إذا (cpuid_007h_ebx_get_smep(ebx_007)) {
        /* similar to smap we cannot enable smep if using dangerous code injection. it
         * does not affect stack trace printing though */
        إذا (!config_مجموعه(CONFIG_DANGEROUS_کد_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMEP);
        }
    }

    إذا (!init_ibrs()) {
        أرجع false;
    }

#اگر_تعریف_شده CONFIG_HARDWARE_DEBUG_API
    /* Initialize hardware breakpoints */
    Arch_initHardwareBreakpoints();
#پایان_اگر

    /* initialise floating-point unit */
    إذا (!Arch_initFpu()) {
        أرجع false;
    }

    /* initialise local APIC */
    إذا (!apic_init(ماسک_legacy_irqs)) {
        أرجع false;
    }

#اگر_تعریف_شده CONFIG_DEBUG_DISABLE_PREFETCHERS
    إذا (!disablePrefetchers()) {
        أرجع false;
    }
#پایان_اگر

    إذا (config_مجموعه(CONFIG_EXPORT_PMC_USER)) {
        enablePMCUser();
    }

#اگر_تعریف_شده CONFIG_VTX
    /* initialise Intel VT-x extensions */
    إذا (!vtx_init()) {
        أرجع false;
    }
#پایان_اگر

    أرجع true;
}
