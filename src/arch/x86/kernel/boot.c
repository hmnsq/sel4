/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/interrupt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/interrupt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/apic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/boot_sys.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/fpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/ioport.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/intel-vtd.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_RESERVED 1
BOOT_BSS ਸਥਿਰ region_t reserved[MAX_RESERVED];

/* functions exactly corresponding to abstract specification */

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ init_irqs(cap_t ਜੜ੍ਹ_cnode_cap)
{
    irq_t i;

    ਲਈ (i = 0; i <= maxIRQ; i++) {
        ਜੇ (i == irq_timer) {
            ਸੈੱਟ_IRQState(IRQTimer, i);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
        } ਨਹੀਂਤਾਂ ਜੇ (i == irq_remote_call_ipi || i == irq_reschedule_ipi) {
            ਸੈੱਟ_IRQState(IRQIPI, i);
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
        } ਨਹੀਂਤਾਂ ਜੇ (i == irq_iommu) {
            ਸੈੱਟ_IRQState(IRQReserved, i);
#ਜੇ_ਅੰਤ
        } ਨਹੀਂਤਾਂ ਜੇ (i == 2 && config_ਸੈੱਟ(CONFIG_IRQ_PIC)) {
            /* cascaded legacy PIC */
            ਸੈੱਟ_IRQState(IRQReserved, i);
        } ਨਹੀਂਤਾਂ ਜੇ (i >= irq_isa_min && i <= irq_isa_max) {
            ਜੇ (config_ਸੈੱਟ(CONFIG_IRQ_PIC)) {
                ਸੈੱਟ_IRQState(IRQInactive, i);
            } ਨਹੀਂਤਾਂ {
                ਸੈੱਟ_IRQState(IRQReserved, i);
            }
        } ਨਹੀਂਤਾਂ ਜੇ (i >= irq_user_min && i <= irq_user_max) {
            ਜੇ (config_ਸੈੱਟ(CONFIG_IRQ_IOAPIC)) {
                ਸੈੱਟ_IRQState(IRQInactive, i);
            } ਨਹੀਂਤਾਂ {
                ਸੈੱਟ_IRQState(IRQReserved, i);
            }
        } ਨਹੀਂਤਾਂ {
            ਸੈੱਟ_IRQState(IRQReserved, i);
        }
    }
    Arch_irqStateInit();
    /* provide the IRQ control cap */
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

BOOT_ਕੋਡ ਸਥਿਰ bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          v_region_t it_v_reg,
                                          mem_p_regs_t *mem_p_regs,
                                          ਸ਼ਬਦ_t extra_bi_ਮਾਪ_ਬਿੱਟ)
{
    // Extend the reserved region down to include the base of the kernel image.
    // KERNEL_ELF_PADDR_BASE is the lowest physical load address used
    // in the x86 linker script.
    ui_p_reg.ਸ਼ੁਰੂ = KERNEL_ELF_PADDR_ਅਧਾਰ;
    reserved[0] = paddr_ਮੰਜ਼ਿਲ_pptr_reg(ui_p_reg);
    ਵਾਪਸ init_freemem(mem_p_regs->ਗਿਣਤੀ_2, mem_p_regs->list, MAX_RESERVED,
                        reserved, it_v_reg, extra_bi_ਮਾਪ_ਬਿੱਟ);
}

/* This function initialises a node's kernel state. It does NOT initialise the CPU. */

BOOT_ਕੋਡ bool_t init_sys_state(
    cpu_id_t      cpu_id,
    mem_p_regs_t  *mem_p_regs,
    ui_info_t     ui_info,
    /* parameters below not modeled in abstract specification */
    uint32_t      ਅੰਕ_drhu,
    paddr_t      *drhu_list,
    acpi_rmrr_list_t *rmrr_list,
    acpi_rsdp_t      *acpi_rsdp,
    seL4_X86_BootInfo_VBE *vbe,
    seL4_X86_BootInfo_mmap_t *mb_mmap,
    seL4_X86_BootInfo_fb_t *fb_info
)
{
    cap_t         ਜੜ੍ਹ_cnode_cap;
    vptr_t        extra_bi_ਚੌਖਟਾ_vptr;
    vptr_t        bi_ਚੌਖਟਾ_vptr;
    vptr_t        ipcbuf_vptr;
    cap_t         it_vspace_cap;
    cap_t         it_ap_cap;
    cap_t         ipcbuf_cap;
    ਸ਼ਬਦ_t        extra_bi_ਮਾਪ = 0;
    pptr_t        extra_bi_ਵਿੱਥ = 0;
    uint32_t      tsc_freq;
    create_ਚੌਖਟੇ_ਦਾ_region_ret_t create_ਚੌਖਟੇ_ret;
    create_ਚੌਖਟੇ_ਦਾ_region_ret_t extra_bi_ret;

    /* convert from physical addresses to kernel pptrs */
    region_t ui_reg             = paddr_ਮੰਜ਼ਿਲ_pptr_reg(ui_info.p_reg);

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg;
    v_region_t it_v_reg;
    ui_v_reg.ਸ਼ੁਰੂ = ui_info.p_reg.ਸ਼ੁਰੂ - ui_info.pv_ਵਿੱਥ;
    ui_v_reg.ਸਿਰਾ   = ui_info.p_reg.ਸਿਰਾ   - ui_info.pv_ਵਿੱਥ;

    ipcbuf_vptr = ui_v_reg.ਸਿਰਾ;
    bi_ਚੌਖਟਾ_vptr = ipcbuf_vptr + ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ);
    extra_bi_ਚੌਖਟਾ_vptr = bi_ਚੌਖਟਾ_vptr + ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Boot_Info_ਚੌਖਟਾ_ਬਿੱਟ);

    ਜੇ (vbe->vbe_ਬਹੁਲਕ != -1) {
        extra_bi_ਮਾਪ += ਆਕਾਰ(seL4_X86_BootInfo_VBE);
    }
    ਜੇ (acpi_rsdp) {
        extra_bi_ਮਾਪ += ਆਕਾਰ(se_L4_Boot_Info_ਸਿਰਲੇਖ) + ਆਕਾਰ(*acpi_rsdp);
    }
    ਜੇ (fb_info && fb_info->addr) {
        extra_bi_ਮਾਪ += ਆਕਾਰ(se_L4_Boot_Info_ਸਿਰਲੇਖ) + ਆਕਾਰ(*fb_info);
    }

    ਸ਼ਬਦ_t mb_mmap_ਮਾਪ = ਆਕਾਰ(seL4_X86_BootInfo_mmap_t);
    extra_bi_ਮਾਪ += mb_mmap_ਮਾਪ;

    // room for tsc frequency
    extra_bi_ਮਾਪ += ਆਕਾਰ(se_L4_Boot_Info_ਸਿਰਲੇਖ) + 4;
    ਸ਼ਬਦ_t extra_bi_ਮਾਪ_ਬਿੱਟ = calculate_extra_bi_ਮਾਪ_ਬਿੱਟ(extra_bi_ਮਾਪ);

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    it_v_reg.ਸ਼ੁਰੂ = ui_v_reg.ਸ਼ੁਰੂ;
    it_v_reg.ਸਿਰਾ = ਗੇੜਾ_ਉੱਪਰ(extra_bi_ਚੌਖਟਾ_vptr + (extra_bi_ਮਾਪ_ਬਿੱਟ > 0 ? ਦੋਆਧਾਰੀ_ਅੰਕ(extra_bi_ਮਾਪ_ਬਿੱਟ) : 0), ਪੰਨਾ_ਬਿੱਟ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
    /* calculate the number of io pts before initialising memory */
    ਜੇ (!vtd_init_ਅੰਕ_iopts(ਅੰਕ_drhu)) {
        ਵਾਪਸ false;
    }
#ਜੇ_ਅੰਤ /* CONFIG_IOMMU */

    ਜੇ (!arch_init_freemem(ui_info.p_reg, it_v_reg, mem_p_regs, extra_bi_ਮਾਪ_ਬਿੱਟ)) {
        ਲਿਖੋ("ERROR: free memory management initialization failed\n");
        ਵਾਪਸ false;
    }

    /* create the root cnode */
    ਜੜ੍ਹ_cnode_cap = create_ਜੜ੍ਹ_cnode();

    /* create the IO port cap */
    write_ਥਾਂ(
        ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapIOPortControl),
        cap_io_ਪੋਰਟ_control_cap_new()
    );

    /* create the cap for managing thread domains */
    create_domain_cap(ਜੜ੍ਹ_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(ਜੜ੍ਹ_cnode_cap);

    tsc_freq = tsc_init();

    /* populate the bootinfo frame */
    populate_bi_ਚੌਖਟਾ(0, ks_ਅੰਕ_CPUs, ipcbuf_vptr, extra_bi_ਮਾਪ);
    region_t extra_bi_region = {
        .ਸ਼ੁਰੂ = rootserver.extra_bi,
        .ਸਿਰਾ = rootserver.extra_bi + (extra_bi_ਮਾਪ_ਬਿੱਟ > 0 ? ਦੋਆਧਾਰੀ_ਅੰਕ(extra_bi_ਮਾਪ_ਬਿੱਟ) : 0)
    };

    /* populate vbe info block */
    ਜੇ (vbe->vbe_ਬਹੁਲਕ != -1) {
        vbe->ਸਿਰਲੇਖ.id = SEL4_BOOTINFO_ਸਿਰਲੇਖ_X86_VBE;
        vbe->ਸਿਰਲੇਖ.len = ਆਕਾਰ(seL4_X86_BootInfo_VBE);
        ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਖਾਲੀ *)(rootserver.extra_bi + extra_bi_ਵਿੱਥ), vbe, ਆਕਾਰ(seL4_X86_BootInfo_VBE));
        extra_bi_ਵਿੱਥ += ਆਕਾਰ(seL4_X86_BootInfo_VBE);
    }

    /* populate acpi rsdp block */
    ਜੇ (acpi_rsdp) {
        se_L4_Boot_Info_ਸਿਰਲੇਖ ਸਿਰਲੇਖ;
        ਸਿਰਲੇਖ.id = SEL4_BOOTINFO_ਸਿਰਲੇਖ_X86_ACPI_RSDP;
        ਸਿਰਲੇਖ.len = ਆਕਾਰ(ਸਿਰਲੇਖ) + ਆਕਾਰ(*acpi_rsdp);
        *(se_L4_Boot_Info_ਸਿਰਲੇਖ *)(rootserver.extra_bi + extra_bi_ਵਿੱਥ) = ਸਿਰਲੇਖ;
        extra_bi_ਵਿੱਥ += ਆਕਾਰ(ਸਿਰਲੇਖ);
        ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਖਾਲੀ *)(rootserver.extra_bi + extra_bi_ਵਿੱਥ), acpi_rsdp, ਆਕਾਰ(*acpi_rsdp));
        extra_bi_ਵਿੱਥ += ਆਕਾਰ(*acpi_rsdp);
    }

    /* populate framebuffer information block */
    ਜੇ (fb_info && fb_info->addr) {
        se_L4_Boot_Info_ਸਿਰਲੇਖ ਸਿਰਲੇਖ;
        ਸਿਰਲੇਖ.id = SEL4_BOOTINFO_ਸਿਰਲੇਖ_X86_FRAMEBUFFER;
        ਸਿਰਲੇਖ.len = ਆਕਾਰ(ਸਿਰਲੇਖ) + ਆਕਾਰ(*fb_info);
        *(se_L4_Boot_Info_ਸਿਰਲੇਖ *)(rootserver.extra_bi + extra_bi_ਵਿੱਥ) = ਸਿਰਲੇਖ;
        extra_bi_ਵਿੱਥ += ਆਕਾਰ(ਸਿਰਲੇਖ);
        ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਖਾਲੀ *)(rootserver.extra_bi + extra_bi_ਵਿੱਥ), fb_info, ਆਕਾਰ(*fb_info));
        extra_bi_ਵਿੱਥ += ਆਕਾਰ(*fb_info);
    }

    /* populate multiboot mmap block */
    mb_mmap->ਸਿਰਲੇਖ.id = SEL4_BOOTINFO_ਸਿਰਲੇਖ_X86_MBMMAP;
    mb_mmap->ਸਿਰਲੇਖ.len = mb_mmap_ਮਾਪ;
    ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਖਾਲੀ *)(rootserver.extra_bi + extra_bi_ਵਿੱਥ), mb_mmap, mb_mmap_ਮਾਪ);
    extra_bi_ਵਿੱਥ += mb_mmap_ਮਾਪ;

    /* populate tsc frequency block */
    {
        se_L4_Boot_Info_ਸਿਰਲੇਖ ਸਿਰਲੇਖ;
        ਸਿਰਲੇਖ.id = SEL4_BOOTINFO_ਸਿਰਲੇਖ_X86_TSC_FREQ;
        ਸਿਰਲੇਖ.len = ਆਕਾਰ(ਸਿਰਲੇਖ) + 4;
        *(se_L4_Boot_Info_ਸਿਰਲੇਖ *)(extra_bi_region.ਸ਼ੁਰੂ + extra_bi_ਵਿੱਥ) = ਸਿਰਲੇਖ;
        extra_bi_ਵਿੱਥ += ਆਕਾਰ(ਸਿਰਲੇਖ);
        *(uint32_t *)(extra_bi_region.ਸ਼ੁਰੂ + extra_bi_ਵਿੱਥ) = tsc_freq;
        extra_bi_ਵਿੱਥ += 4;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    /* set up sched control for each core */
    init_sched_control(ਜੜ੍ਹ_cnode_cap, CONFIG_MAX_ਅੰਕ_NODES);
#ਜੇ_ਅੰਤ

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_vspace_cap = create_it_ਪਤਾ_space(ਜੜ੍ਹ_cnode_cap, it_v_reg);
    ਜੇ (cap_get_capType(it_vspace_cap) == cap_null_cap) {
        ਵਾਪਸ false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_ਚੌਖਟਾ_cap(
        ਜੜ੍ਹ_cnode_cap,
        it_vspace_cap,
        bi_ਚੌਖਟਾ_vptr
    );

    /* create and map extra bootinfo region */
    extra_bi_ret =
        create_ਚੌਖਟੇ_ਦਾ_region(
            ਜੜ੍ਹ_cnode_cap,
            it_vspace_cap,
            extra_bi_region,
            true,
            pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)(extra_bi_region.ਸ਼ੁਰੂ - extra_bi_ਚੌਖਟਾ_vptr))
        );
    ਜੇ (!extra_bi_ret.success) {
        ਵਾਪਸ false;
    }
    ndks_boot.bi_ਚੌਖਟਾ->extraBIPages = extra_bi_ret.region;

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_ਚੌਖਟਾ_cap(ਜੜ੍ਹ_cnode_cap, it_vspace_cap, ipcbuf_vptr);
    ਜੇ (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        ਵਾਪਸ false;
    }

    /* create all userland image frames */
    create_ਚੌਖਟੇ_ret =
        create_ਚੌਖਟੇ_ਦਾ_region(
            ਜੜ੍ਹ_cnode_cap,
            it_vspace_cap,
            ui_reg,
            true,
            ui_info.pv_ਵਿੱਥ
        );
    ਜੇ (!create_ਚੌਖਟੇ_ret.success) {
        ਵਾਪਸ false;
    }
    ndks_boot.bi_ਚੌਖਟਾ->user_Image_ਚੌਖਟੇ = create_ਚੌਖਟੇ_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(ਜੜ੍ਹ_cnode_cap);
    ਜੇ (cap_get_capType(it_ap_cap) == cap_null_cap) {
        ਵਾਪਸ false;
    }
    write_it_asid_pool(it_ap_cap, it_vspace_cap);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਗੰਢ_STATE(ks_Cur_ਸਮਾਂ) = get_Current_ਸਮਾਂ();
#ਜੇ_ਅੰਤ

    /* create the idle thread */
    create_idle_thread();

    /* copy i387 FPU initial state from FPU */
    saveFpuState(ਗੰਢ_STATE(ksIdleThread));
    x86KSnullFpuState = ਗੰਢ_STATE(ksIdleThread)->tcbArch.tcbContext.fpuState;
    /* Check that we can load it: */
    ਲੱਦੋ_Fpu_State(ਗੰਢ_STATE(ksIdleThread));

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(ਜੜ੍ਹ_cnode_cap,
                                           it_vspace_cap,
                                           ui_info.v_ਇੰਦਰਾਜ,
                                           bi_ਚੌਖਟਾ_vptr,
                                           ipcbuf_vptr,
                                           ipcbuf_cap);
    ਜੇ (initial == ਨਿਰਾ) {
        ਵਾਪਸ false;
    }
    init_core_state(initial);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
    /* initialise VTD-related data structures and the IOMMUs */
    ਜੇ (!vtd_init(cpu_id, rmrr_list)) {
        ਵਾਪਸ false;
    }

    /* write number of IOMMU PT levels into bootinfo */
    ndks_boot.bi_ਚੌਖਟਾ->ਅੰਕ_IOPTLevels = x86KSnumIOPTLevels;

    /* write IOSpace master cap */
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapIOSpace), master_iospace_cap());
#ਨਹੀਂਤਾਂ
    ndks_boot.bi_ਚੌਖਟਾ->ਅੰਕ_IOPTLevels = -1;
#ਜੇ_ਅੰਤ

    /* create all of the untypeds. Both devices and kernel window memory */
    ਜੇ (!create_untypeds(ਜੜ੍ਹ_cnode_cap)) {
        ਵਾਪਸ false;
    }

    /* finalise the bootinfo frame */
    bi_finalise();

    ਵਾਪਸ true;
}

/* This function initialises the CPU. It does NOT initialise any kernel state. */

BOOT_ਕੋਡ bool_t init_cpu(
    bool_t   ਨਕਾਬ_legacy_irqs
)
{
    /* initialise virtual-memory-related data structures */
    ਜੇ (!init_vm_state()) {
        ਵਾਪਸ false;
    }

    /* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */
    init_dtrs();

    ਜੇ (config_ਸੈੱਟ(CONFIG_SYSENTER)) {
        /* initialise MSRs (needs an initialised TSS) */
        init_sysenter_msrs();
    } ਨਹੀਂਤਾਂ ਜੇ (config_ਸੈੱਟ(CONFIG_SYSCALL)) {
        init_syscall_msrs();
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ false;
    }

    /* setup additional PAT MSR */
    ਜੇ (!init_pat_msr()) {
        ਵਾਪਸ false;
    }

    /* enable the Write Protect bit in cr0. This prevents the kernel from writing to
     * read only memory, which we shouldn't do under correct execution */
    write_cr0(read_cr0() | CR0_WRITE_PROTECT);

    /* check for SMAP and SMEP and enable */
    cpuid_007h_ebx_t ebx_007;
    ebx_007.ਸ਼ਬਦ[0] = x86_cpuid_ebx(0x7, 0);
    ਜੇ (cpuid_007h_ebx_get_smap(ebx_007)) {
        /* if we have user stack trace enabled or dangerous code injection then we cannot
         * enable this as SMAP will make them fault. */
        ਜੇ (!config_ਸੈੱਟ(CONFIG_PRINTING) && !config_ਸੈੱਟ(CONFIG_DANGEROUS_ਕੋਡ_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMAP);
        }
    }
    ਜੇ (cpuid_007h_ebx_get_smep(ebx_007)) {
        /* similar to smap we cannot enable smep if using dangerous code injection. it
         * does not affect stack trace printing though */
        ਜੇ (!config_ਸੈੱਟ(CONFIG_DANGEROUS_ਕੋਡ_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMEP);
        }
    }

    ਜੇ (!init_ibrs()) {
        ਵਾਪਸ false;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API
    /* Initialize hardware breakpoints */
    Arch_initHardwareBreakpoints();
#ਜੇ_ਅੰਤ

    /* initialise floating-point unit */
    ਜੇ (!Arch_initFpu()) {
        ਵਾਪਸ false;
    }

    /* initialise local APIC */
    ਜੇ (!apic_init(ਨਕਾਬ_legacy_irqs)) {
        ਵਾਪਸ false;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_DISABLE_PREFETCHERS
    ਜੇ (!disablePrefetchers()) {
        ਵਾਪਸ false;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (config_ਸੈੱਟ(CONFIG_EXPORT_PMC_USER)) {
        enablePMCUser();
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    /* initialise Intel VT-x extensions */
    ਜੇ (!vtx_init()) {
        ਵਾਪਸ false;
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ true;
}
