/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <object/interrupt.h>
#ಅನ್ತರ್ಭಾವ <arch/object/interrupt.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/apic.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/boot_sys.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/timer.h>
#ಅನ್ತರ್ಭಾವ <arch/object/ioport.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <util.h>

#ಅನ್ತರ್ಭಾವ <plat/machine/intel-vtd.h>

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_RESERVED 1
BOOT_BSS ಸ್ಥಿರ region_t reserved[MAX_RESERVED];

/* functions exactly corresponding to abstract specification */

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ init_irqs(cap_t ಬೇರು_cnode_cap)
{
    irq_t i;

    ಗಾಗಿ (i = 0; i <= maxIRQ; i++) {
        ಆದರೆ (i == irq_timer) {
            ಗಣ_IRQState(IRQTimer, i);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == irq_remote_call_ipi || i == irq_reschedule_ipi) {
            ಗಣ_IRQState(IRQIPI, i);
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == irq_iommu) {
            ಗಣ_IRQState(IRQReserved, i);
#ಆದರೆ_ಅಂತ್ಯ
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i == 2 && config_ಗಣ(CONFIG_IRQ_PIC)) {
            /* cascaded legacy PIC */
            ಗಣ_IRQState(IRQReserved, i);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i >= irq_isa_min && i <= irq_isa_max) {
            ಆದರೆ (config_ಗಣ(CONFIG_IRQ_PIC)) {
                ಗಣ_IRQState(IRQInactive, i);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಗಣ_IRQState(IRQReserved, i);
            }
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i >= irq_user_min && i <= irq_user_max) {
            ಆದರೆ (config_ಗಣ(CONFIG_IRQ_IOAPIC)) {
                ಗಣ_IRQState(IRQInactive, i);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಗಣ_IRQState(IRQReserved, i);
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಣ_IRQState(IRQReserved, i);
        }
    }
    Arch_irqStateInit();
    /* provide the IRQ control cap */
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          v_region_t it_v_reg,
                                          mem_p_regs_t *mem_p_regs,
                                          ಶಬ್ದ_t extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)
{
    // Extend the reserved region down to include the base of the kernel image.
    // KERNEL_ELF_PADDR_BASE is the lowest physical load address used
    // in the x86 linker script.
    ui_p_reg.ಆರಂಭ = KERNEL_ELF_PADDR_ತಳ;
    reserved[0] = paddr_ಗಮ್ಯ_pptr_reg(ui_p_reg);
    ಹಿಂತಿರುಗಿ init_freemem(mem_p_regs->ಎಣಿಕೆ_2, mem_p_regs->list, MAX_RESERVED,
                        reserved, it_v_reg, extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
}

/* This function initialises a node's kernel state. It does NOT initialise the CPU. */

BOOT_ಸಂಕೇತ bool_t init_sys_state(
    cpu_id_t      cpu_id,
    mem_p_regs_t  *mem_p_regs,
    ui_info_t     ui_info,
    /* parameters below not modeled in abstract specification */
    uint32_t      ಅಂಕೆ_drhu,
    paddr_t      *drhu_list,
    acpi_rmrr_list_t *rmrr_list,
    acpi_rsdp_t      *acpi_rsdp,
    seL4_X86_BootInfo_VBE *vbe,
    seL4_X86_BootInfo_mmap_t *mb_mmap,
    seL4_X86_BootInfo_fb_t *fb_info
)
{
    cap_t         ಬೇರು_cnode_cap;
    vptr_t        extra_bi_ಚೌಕಟ್ಟು_vptr;
    vptr_t        bi_ಚೌಕಟ್ಟು_vptr;
    vptr_t        ipcbuf_vptr;
    cap_t         it_vspace_cap;
    cap_t         it_ap_cap;
    cap_t         ipcbuf_cap;
    ಶಬ್ದ_t        extra_bi_ಗಾತ್ರ = 0;
    pptr_t        extra_bi_ಸ್ಥಾನಾಂತರ = 0;
    uint32_t      tsc_freq;
    create_ಚೌಕಟ್ಟುಗಳು_ದ_region_ret_t create_ಚೌಕಟ್ಟುಗಳು_ret;
    create_ಚೌಕಟ್ಟುಗಳು_ದ_region_ret_t extra_bi_ret;

    /* convert from physical addresses to kernel pptrs */
    region_t ui_reg             = paddr_ಗಮ್ಯ_pptr_reg(ui_info.p_reg);

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg;
    v_region_t it_v_reg;
    ui_v_reg.ಆರಂಭ = ui_info.p_reg.ಆರಂಭ - ui_info.pv_ಸ್ಥಾನಾಂತರ;
    ui_v_reg.ಕೊನೆ   = ui_info.p_reg.ಕೊನೆ   - ui_info.pv_ಸ್ಥಾನಾಂತರ;

    ipcbuf_vptr = ui_v_reg.ಕೊನೆ;
    bi_ಚೌಕಟ್ಟು_vptr = ipcbuf_vptr + ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು);
    extra_bi_ಚೌಕಟ್ಟು_vptr = bi_ಚೌಕಟ್ಟು_vptr + ಬಿಟ್(se_L4_Boot_Info_ಚೌಕಟ್ಟು_ಬಿಟ್ಟುಗಳು);

    ಆದರೆ (vbe->vbe_ಬಹುಲಕ != -1) {
        extra_bi_ಗಾತ್ರ += ಗಾತ್ರ(seL4_X86_BootInfo_VBE);
    }
    ಆದರೆ (acpi_rsdp) {
        extra_bi_ಗಾತ್ರ += ಗಾತ್ರ(se_L4_Boot_Info_ಶೀರ್ಷಿಕೆ) + ಗಾತ್ರ(*acpi_rsdp);
    }
    ಆದರೆ (fb_info && fb_info->addr) {
        extra_bi_ಗಾತ್ರ += ಗಾತ್ರ(se_L4_Boot_Info_ಶೀರ್ಷಿಕೆ) + ಗಾತ್ರ(*fb_info);
    }

    ಶಬ್ದ_t mb_mmap_ಗಾತ್ರ = ಗಾತ್ರ(seL4_X86_BootInfo_mmap_t);
    extra_bi_ಗಾತ್ರ += mb_mmap_ಗಾತ್ರ;

    // room for tsc frequency
    extra_bi_ಗಾತ್ರ += ಗಾತ್ರ(se_L4_Boot_Info_ಶೀರ್ಷಿಕೆ) + 4;
    ಶಬ್ದ_t extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು = calculate_extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(extra_bi_ಗಾತ್ರ);

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    it_v_reg.ಆರಂಭ = ui_v_reg.ಆರಂಭ;
    it_v_reg.ಕೊನೆ = ಸುತ್ತು_ಮೇಲೆ(extra_bi_ಚೌಕಟ್ಟು_vptr + (extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು > 0 ? ಬಿಟ್(extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು) : 0), ಪುಟ_ಬಿಟ್ಟುಗಳು);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
    /* calculate the number of io pts before initialising memory */
    ಆದರೆ (!vtd_init_ಅಂಕೆ_iopts(ಅಂಕೆ_drhu)) {
        ಹಿಂತಿರುಗಿ false;
    }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_IOMMU */

    ಆದರೆ (!arch_init_freemem(ui_info.p_reg, it_v_reg, mem_p_regs, extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)) {
        ಬರೆ("ERROR: free memory management initialization failed\n");
        ಹಿಂತಿರುಗಿ false;
    }

    /* create the root cnode */
    ಬೇರು_cnode_cap = create_ಬೇರು_cnode();

    /* create the IO port cap */
    write_ಸ್ಥಾನ(
        ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapIOPortControl),
        cap_io_ಪೋರ್ಟ್_control_cap_new()
    );

    /* create the cap for managing thread domains */
    create_domain_cap(ಬೇರು_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(ಬೇರು_cnode_cap);

    tsc_freq = tsc_init();

    /* populate the bootinfo frame */
    populate_bi_ಚೌಕಟ್ಟು(0, ks_ಅಂಕೆ_CPUs, ipcbuf_vptr, extra_bi_ಗಾತ್ರ);
    region_t extra_bi_region = {
        .ಆರಂಭ = rootserver.extra_bi,
        .ಕೊನೆ = rootserver.extra_bi + (extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು > 0 ? ಬಿಟ್(extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು) : 0)
    };

    /* populate vbe info block */
    ಆದರೆ (vbe->vbe_ಬಹುಲಕ != -1) {
        vbe->ಶೀರ್ಷಿಕೆ.id = SEL4_BOOTINFO_ಶೀರ್ಷಿಕೆ_X86_VBE;
        vbe->ಶೀರ್ಷಿಕೆ.len = ಗಾತ್ರ(seL4_X86_BootInfo_VBE);
        ಸ್ಮರಣೆ_ನಕಲು((ಶೂನ್ಯ *)(rootserver.extra_bi + extra_bi_ಸ್ಥಾನಾಂತರ), vbe, ಗಾತ್ರ(seL4_X86_BootInfo_VBE));
        extra_bi_ಸ್ಥಾನಾಂತರ += ಗಾತ್ರ(seL4_X86_BootInfo_VBE);
    }

    /* populate acpi rsdp block */
    ಆದರೆ (acpi_rsdp) {
        se_L4_Boot_Info_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ;
        ಶೀರ್ಷಿಕೆ.id = SEL4_BOOTINFO_ಶೀರ್ಷಿಕೆ_X86_ACPI_RSDP;
        ಶೀರ್ಷಿಕೆ.len = ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ) + ಗಾತ್ರ(*acpi_rsdp);
        *(se_L4_Boot_Info_ಶೀರ್ಷಿಕೆ *)(rootserver.extra_bi + extra_bi_ಸ್ಥಾನಾಂತರ) = ಶೀರ್ಷಿಕೆ;
        extra_bi_ಸ್ಥಾನಾಂತರ += ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ);
        ಸ್ಮರಣೆ_ನಕಲು((ಶೂನ್ಯ *)(rootserver.extra_bi + extra_bi_ಸ್ಥಾನಾಂತರ), acpi_rsdp, ಗಾತ್ರ(*acpi_rsdp));
        extra_bi_ಸ್ಥಾನಾಂತರ += ಗಾತ್ರ(*acpi_rsdp);
    }

    /* populate framebuffer information block */
    ಆದರೆ (fb_info && fb_info->addr) {
        se_L4_Boot_Info_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ;
        ಶೀರ್ಷಿಕೆ.id = SEL4_BOOTINFO_ಶೀರ್ಷಿಕೆ_X86_FRAMEBUFFER;
        ಶೀರ್ಷಿಕೆ.len = ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ) + ಗಾತ್ರ(*fb_info);
        *(se_L4_Boot_Info_ಶೀರ್ಷಿಕೆ *)(rootserver.extra_bi + extra_bi_ಸ್ಥಾನಾಂತರ) = ಶೀರ್ಷಿಕೆ;
        extra_bi_ಸ್ಥಾನಾಂತರ += ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ);
        ಸ್ಮರಣೆ_ನಕಲು((ಶೂನ್ಯ *)(rootserver.extra_bi + extra_bi_ಸ್ಥಾನಾಂತರ), fb_info, ಗಾತ್ರ(*fb_info));
        extra_bi_ಸ್ಥಾನಾಂತರ += ಗಾತ್ರ(*fb_info);
    }

    /* populate multiboot mmap block */
    mb_mmap->ಶೀರ್ಷಿಕೆ.id = SEL4_BOOTINFO_ಶೀರ್ಷಿಕೆ_X86_MBMMAP;
    mb_mmap->ಶೀರ್ಷಿಕೆ.len = mb_mmap_ಗಾತ್ರ;
    ಸ್ಮರಣೆ_ನಕಲು((ಶೂನ್ಯ *)(rootserver.extra_bi + extra_bi_ಸ್ಥಾನಾಂತರ), mb_mmap, mb_mmap_ಗಾತ್ರ);
    extra_bi_ಸ್ಥಾನಾಂತರ += mb_mmap_ಗಾತ್ರ;

    /* populate tsc frequency block */
    {
        se_L4_Boot_Info_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ;
        ಶೀರ್ಷಿಕೆ.id = SEL4_BOOTINFO_ಶೀರ್ಷಿಕೆ_X86_TSC_FREQ;
        ಶೀರ್ಷಿಕೆ.len = ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ) + 4;
        *(se_L4_Boot_Info_ಶೀರ್ಷಿಕೆ *)(extra_bi_region.ಆರಂಭ + extra_bi_ಸ್ಥಾನಾಂತರ) = ಶೀರ್ಷಿಕೆ;
        extra_bi_ಸ್ಥಾನಾಂತರ += ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ);
        *(uint32_t *)(extra_bi_region.ಆರಂಭ + extra_bi_ಸ್ಥಾನಾಂತರ) = tsc_freq;
        extra_bi_ಸ್ಥಾನಾಂತರ += 4;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    /* set up sched control for each core */
    init_sched_control(ಬೇರು_cnode_cap, CONFIG_MAX_ಅಂಕೆ_NODES);
#ಆದರೆ_ಅಂತ್ಯ

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_vspace_cap = create_it_ವಿಳಾಸ_space(ಬೇರು_cnode_cap, it_v_reg);
    ಆದರೆ (cap_get_capType(it_vspace_cap) == cap_null_cap) {
        ಹಿಂತಿರುಗಿ false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_ಚೌಕಟ್ಟು_cap(
        ಬೇರು_cnode_cap,
        it_vspace_cap,
        bi_ಚೌಕಟ್ಟು_vptr
    );

    /* create and map extra bootinfo region */
    extra_bi_ret =
        create_ಚೌಕಟ್ಟುಗಳು_ದ_region(
            ಬೇರು_cnode_cap,
            it_vspace_cap,
            extra_bi_region,
            true,
            pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)(extra_bi_region.ಆರಂಭ - extra_bi_ಚೌಕಟ್ಟು_vptr))
        );
    ಆದರೆ (!extra_bi_ret.success) {
        ಹಿಂತಿರುಗಿ false;
    }
    ndks_boot.bi_ಚೌಕಟ್ಟು->extraBIPages = extra_bi_ret.region;

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_ಚೌಕಟ್ಟು_cap(ಬೇರು_cnode_cap, it_vspace_cap, ipcbuf_vptr);
    ಆದರೆ (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        ಹಿಂತಿರುಗಿ false;
    }

    /* create all userland image frames */
    create_ಚೌಕಟ್ಟುಗಳು_ret =
        create_ಚೌಕಟ್ಟುಗಳು_ದ_region(
            ಬೇರು_cnode_cap,
            it_vspace_cap,
            ui_reg,
            true,
            ui_info.pv_ಸ್ಥಾನಾಂತರ
        );
    ಆದರೆ (!create_ಚೌಕಟ್ಟುಗಳು_ret.success) {
        ಹಿಂತಿರುಗಿ false;
    }
    ndks_boot.bi_ಚೌಕಟ್ಟು->user_Image_ಚೌಕಟ್ಟುಗಳು = create_ಚೌಕಟ್ಟುಗಳು_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(ಬೇರು_cnode_cap);
    ಆದರೆ (cap_get_capType(it_ap_cap) == cap_null_cap) {
        ಹಿಂತಿರುಗಿ false;
    }
    write_it_asid_pool(it_ap_cap, it_vspace_cap);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಗಂಟು_STATE(ks_Cur_ಸಮಯ) = get_Current_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ

    /* create the idle thread */
    create_idle_thread();

    /* copy i387 FPU initial state from FPU */
    saveFpuState(ಗಂಟು_STATE(ksIdleThread));
    x86KSnullFpuState = ಗಂಟು_STATE(ksIdleThread)->tcbArch.tcbContext.fpuState;
    /* Check that we can load it: */
    ತುಂಬು_Fpu_State(ಗಂಟು_STATE(ksIdleThread));

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(ಬೇರು_cnode_cap,
                                           it_vspace_cap,
                                           ui_info.v_ನಮೂದು,
                                           bi_ಚೌಕಟ್ಟು_vptr,
                                           ipcbuf_vptr,
                                           ipcbuf_cap);
    ಆದರೆ (initial == ಖಾಲಿ) {
        ಹಿಂತಿರುಗಿ false;
    }
    init_core_state(initial);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
    /* initialise VTD-related data structures and the IOMMUs */
    ಆದರೆ (!vtd_init(cpu_id, rmrr_list)) {
        ಹಿಂತಿರುಗಿ false;
    }

    /* write number of IOMMU PT levels into bootinfo */
    ndks_boot.bi_ಚೌಕಟ್ಟು->ಅಂಕೆ_IOPTLevels = x86KSnumIOPTLevels;

    /* write IOSpace master cap */
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapIOSpace), master_iospace_cap());
#ಇಲ್ಲದಿದ್ದರೆ
    ndks_boot.bi_ಚೌಕಟ್ಟು->ಅಂಕೆ_IOPTLevels = -1;
#ಆದರೆ_ಅಂತ್ಯ

    /* create all of the untypeds. Both devices and kernel window memory */
    ಆದರೆ (!create_untypeds(ಬೇರು_cnode_cap)) {
        ಹಿಂತಿರುಗಿ false;
    }

    /* finalise the bootinfo frame */
    bi_finalise();

    ಹಿಂತಿರುಗಿ true;
}

/* This function initialises the CPU. It does NOT initialise any kernel state. */

BOOT_ಸಂಕೇತ bool_t init_cpu(
    bool_t   ಮುಖವಾಡ_legacy_irqs
)
{
    /* initialise virtual-memory-related data structures */
    ಆದರೆ (!init_vm_state()) {
        ಹಿಂತಿರುಗಿ false;
    }

    /* initialise CPU's descriptor table registers (GDTR, IDTR, LDTR, TR) */
    init_dtrs();

    ಆದರೆ (config_ಗಣ(CONFIG_SYSENTER)) {
        /* initialise MSRs (needs an initialised TSS) */
        init_sysenter_msrs();
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (config_ಗಣ(CONFIG_SYSCALL)) {
        init_syscall_msrs();
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ false;
    }

    /* setup additional PAT MSR */
    ಆದರೆ (!init_pat_msr()) {
        ಹಿಂತಿರುಗಿ false;
    }

    /* enable the Write Protect bit in cr0. This prevents the kernel from writing to
     * read only memory, which we shouldn't do under correct execution */
    write_cr0(read_cr0() | CR0_WRITE_PROTECT);

    /* check for SMAP and SMEP and enable */
    cpuid_007h_ebx_t ebx_007;
    ebx_007.ಪದಗಳು[0] = x86_cpuid_ebx(0x7, 0);
    ಆದರೆ (cpuid_007h_ebx_get_smap(ebx_007)) {
        /* if we have user stack trace enabled or dangerous code injection then we cannot
         * enable this as SMAP will make them fault. */
        ಆದರೆ (!config_ಗಣ(CONFIG_PRINTING) && !config_ಗಣ(CONFIG_DANGEROUS_ಸಂಕೇತ_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMAP);
        }
    }
    ಆದರೆ (cpuid_007h_ebx_get_smep(ebx_007)) {
        /* similar to smap we cannot enable smep if using dangerous code injection. it
         * does not affect stack trace printing though */
        ಆದರೆ (!config_ಗಣ(CONFIG_DANGEROUS_ಸಂಕೇತ_INJECTION)) {
            write_cr4(read_cr4() | CR4_SMEP);
        }
    }

    ಆದರೆ (!init_ibrs()) {
        ಹಿಂತಿರುಗಿ false;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API
    /* Initialize hardware breakpoints */
    Arch_initHardwareBreakpoints();
#ಆದರೆ_ಅಂತ್ಯ

    /* initialise floating-point unit */
    ಆದರೆ (!Arch_initFpu()) {
        ಹಿಂತಿರುಗಿ false;
    }

    /* initialise local APIC */
    ಆದರೆ (!apic_init(ಮುಖವಾಡ_legacy_irqs)) {
        ಹಿಂತಿರುಗಿ false;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_DISABLE_PREFETCHERS
    ಆದರೆ (!disablePrefetchers()) {
        ಹಿಂತಿರುಗಿ false;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (config_ಗಣ(CONFIG_EXPORT_PMC_USER)) {
        enablePMCUser();
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    /* initialise Intel VT-x extensions */
    ಆದರೆ (!vtx_init()) {
        ಹಿಂತಿರುಗಿ false;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ true;
}
