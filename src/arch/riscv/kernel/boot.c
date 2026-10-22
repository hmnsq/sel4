/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <object/interrupt.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/traps.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <arch/benchmark.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <machine.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS ಸ್ಥಿರ ಅಸ್ಥಿರ _Atomic ಶಬ್ದ_t ಗಂಟು_boot_ಬೀಗ;
#ಆದರೆ_ಅಂತ್ಯ

BOOT_BSS ಸ್ಥಿರ region_t res_reg[ಅಂಕೆ_RESERVED_REGIONS];

BOOT_ಸಂಕೇತ cap_t create_mapped_it_ಚೌಕಟ್ಟು_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t
                                           use_large, bool_t executable)
{
    cap_t cap;
    vm_ಪುಟ_ಗಾತ್ರ_t ಚೌಕಟ್ಟು_ಗಾತ್ರ_2;

    ಆದರೆ (use_large) {
        ಚೌಕಟ್ಟು_ಗಾತ್ರ_2 = RISCV_Mega_ಪುಟ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಚೌಕಟ್ಟು_ಗಾತ್ರ_2 = RISCV_4_K_ಪುಟ;
    }

    cap = cap_ಚೌಕಟ್ಟು_cap_new(
              asid,                            /* capFMappedASID       */
              pptr,                            /* capFBasePtr          */
              ಚೌಕಟ್ಟು_ಗಾತ್ರ_2,                      /* capFSize             */
              ಶಬ್ದ_From_VMRights(VMReadWrite),   /* capFVMRights         */
              0,                               /* capFIsDevice         */
              vptr                             /* capFMappedAddress    */
          );

    map_it_ಚೌಕಟ್ಟು_cap(pd_cap, cap);
    ಹಿಂತಿರುಗಿ cap;
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ bool_t arch_init_freemem(region_t ui_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          ಶಬ್ದ_t extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)
{
    /* Reserve the kernel image region. This may look a bit awkward, as the
     * symbols are a reference in the kernel image window, but all allocations
     * are done in terms of the main kernel window, so we do some translation.
     */
    res_reg[0] = paddr_ಗಮ್ಯ_pptr_reg(get_p_reg_kernel_img());
    ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = 1;

    /* add the dtb region, if it is not empty */
    ಆದರೆ (dtb_p_reg.ಆರಂಭ) {
        ಆದರೆ (ಸೂಚ್ಯಂಕ >= ARRAY_ಗಾತ್ರ(res_reg)) {
            ಬರೆ("ERROR: no slot to add DTB to reserved regions\n");
            ಹಿಂತಿರುಗಿ false;
        }
        res_reg[ಸೂಚ್ಯಂಕ] = paddr_ಗಮ್ಯ_pptr_reg(dtb_p_reg);
        ಸೂಚ್ಯಂಕ += 1;
    }

    /* reserve the user image region */
    ಆದರೆ (ಸೂಚ್ಯಂಕ >= ARRAY_ಗಾತ್ರ(res_reg)) {
        ಬರೆ("ERROR: no slot to add user image to reserved regions\n");
        ಹಿಂತಿರುಗಿ false;
    }
    res_reg[ಸೂಚ್ಯಂಕ] = ui_reg;
    ಸೂಚ್ಯಂಕ += 1;

    /* avail_p_regs comes from the auto-generated code */
    ಹಿಂತಿರುಗಿ init_freemem(ARRAY_ಗಾತ್ರ(avail_p_regs), avail_p_regs,
                        ಸೂಚ್ಯಂಕ, res_reg,
                        it_v_reg, extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ init_irqs(cap_t ಬೇರು_cnode_cap)
{
    irq_t i;

    ಗಾಗಿ (i = 0; i <= maxIRQ; i++) {
        ಆದರೆ (i != irqInvalid) {
            /* IRQ 0 is irqInvalid */
            ಗಣ_IRQState(IRQInactive, i);
        }
    }
    ಗಣ_IRQState(IRQTimer, KERNEL_TIMER_IRQ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ಗಣ_IRQState(IRQIPI, irq_remote_call_ipi);
    ಗಣ_IRQState(IRQIPI, irq_reschedule_ipi);
#ಆದರೆ_ಅಂತ್ಯ
    /* provide the IRQ control cap */
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

/* This and only this function initialises the CPU. It does NOT initialise any kernel state. */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ init_fpu(ಶೂನ್ಯ)
{
    ಗಣ_fs_clean();
    write_fcsr(0);
    disableFpu();
}
#ಆದರೆ_ಅಂತ್ಯ

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ init_cpu(ಶೂನ್ಯ)
{

    activate_kernel_vspace();
    /* Write trap entry address to stvec */
    write_stvec((ಶಬ್ದ_t)trap_ನಮೂದು);
    initLocalIRQController();
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
    initTimer();
#ಆದರೆ_ಅಂತ್ಯ

    /* disable FPU access */
    ಗಣ_fs_off();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
    init_fpu();
#ಆದರೆ_ಅಂತ್ಯ
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ init_plat(ಶೂನ್ಯ)
{
    initIRQController();
}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
BOOT_ಸಂಕೇತ ಸ್ಥಿರ bool_t try_init_kernel_secondary_core(ಶಬ್ದ_t hart_id, ಶಬ್ದ_t core_id)
{
    ಆಗ (!ಗಂಟು_boot_ಬೀಗ);

    fence_r_rw();

    init_cpu();
    ಗಂಟು_ಬೀಗ_SYS;

    clock_sync_test();
    ks_ಅಂಕೆ_CPUs++;
    init_core_state(SchedulerAction_ResumeCurrentThread);
    ifence_local();
    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ release_secondary_cores(ಶೂನ್ಯ)
{
    ಪ್ರತಿಜ್ಞಾ(0 == ಗಂಟು_boot_ಬೀಗ); /* Sanity check for a proper lock state. */
    ಗಂಟು_boot_ಬೀಗ = 1;
    /* At this point in time the primary core (executing this code) already uses
     * the seL4 MMU/cache setup. However, the secondary cores are still using
     * the elfloader's MMU/cache setup, and thus the update of node_boot_lock
     * may not be visible there if the setups differ. Currently, the mappings
     * match, so a barrier is all that is needed.
     */
    fence_rw_rw();

    ಆಗ (ks_ಅಂಕೆ_CPUs != CONFIG_MAX_ಅಂಕೆ_NODES) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        ಗಂಟು_STATE(ks_Cur_ಸಮಯ) = get_Current_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

ಸ್ಥಿರ BOOT_ಸಂಕೇತ bool_t try_init_kernel(
    paddr_t ui_p_reg_ಆರಂಭ,
    paddr_t ui_p_reg_ಕೊನೆ,
    sword_t pv_ಸ್ಥಾನಾಂತರ,
    vptr_t  v_ನಮೂದು,
    paddr_t dtb_phys_addr,
    ಶಬ್ದ_t  dtb_ಗಾತ್ರ
)
{
    cap_t ಬೇರು_cnode_cap;
    cap_t it_pd_cap;
    cap_t it_ap_cap;
    cap_t ipcbuf_cap;
    region_t ui_reg = paddr_ಗಮ್ಯ_pptr_reg((p_region_t) {
        ui_p_reg_ಆರಂಭ, ui_p_reg_ಕೊನೆ
    });
    ಶಬ್ದ_t extra_bi_ಗಾತ್ರ = 0;
    pptr_t extra_bi_ಸ್ಥಾನಾಂತರ = 0;
    vptr_t extra_bi_ಚೌಕಟ್ಟು_vptr;
    vptr_t bi_ಚೌಕಟ್ಟು_vptr;
    vptr_t ipcbuf_vptr;
    create_ಚೌಕಟ್ಟುಗಳು_ದ_region_ret_t create_ಚೌಕಟ್ಟುಗಳು_ret;
    create_ಚೌಕಟ್ಟುಗಳು_ದ_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .ಆರಂಭ = ui_p_reg_ಆರಂಭ - pv_ಸ್ಥಾನಾಂತರ,
        .ಕೊನೆ   = ui_p_reg_ಕೊನೆ   - pv_ಸ್ಥಾನಾಂತರ
    };

    ipcbuf_vptr = ui_v_reg.ಕೊನೆ;
    bi_ಚೌಕಟ್ಟು_vptr = ipcbuf_vptr + ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು);
    extra_bi_ಚೌಕಟ್ಟು_vptr = bi_ಚೌಕಟ್ಟು_vptr + ಬಿಟ್(se_L4_Boot_Info_ಚೌಕಟ್ಟು_ಬಿಟ್ಟುಗಳು);

    map_kernel_ಕಿಟಕಿ();

    /* initialise the CPU */
    init_cpu();

    ಬರೆ("Bootstrapping kernel\n");

    /* initialize the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_ಖಾಲಿಗಳು;
    ಆದರೆ (dtb_ಗಾತ್ರ > 0) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PLAT_ROCKETCHIP_ZCU102
        /* The softcore rocketchip instantiation doesn't work well when this
         * page isn't reserved. Round up so the whole page is reserved to
         * avoid the problem
         */
        dtb_ಗಾತ್ರ = ಸುತ್ತು_ಮೇಲೆ(dtb_ಗಾತ್ರ, ಪುಟ_ಬಿಟ್ಟುಗಳು);
#ಆದರೆ_ಅಂತ್ಯ
        paddr_t dtb_phys_ಕೊನೆ = dtb_phys_addr + dtb_ಗಾತ್ರ;
        ಆದರೆ (dtb_phys_ಕೊನೆ < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            ಬರೆ("ERROR: DTB location at %"SEL4_PRIx_ಶಬ್ದ
                   " len %"SEL4_PRIu_ಶಬ್ದ" invalid\n",
                   dtb_phys_addr, dtb_ಗಾತ್ರ);
            ಹಿಂತಿರುಗಿ false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        ಆದರೆ (dtb_phys_ಕೊನೆ >= PADDR_ತುದಿ) {
            ಬರೆ("ERROR: DTB at [%"SEL4_PRIx_ಶಬ್ದ"..%"SEL4_PRIx_ಶಬ್ದ") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_ಶಬ್ದ")\n",
                   dtb_phys_addr, dtb_phys_ಕೊನೆ, PADDR_ತುದಿ);
            ಹಿಂತಿರುಗಿ false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_ಗಾತ್ರ += ಗಾತ್ರ(se_L4_Boot_Info_ಶೀರ್ಷಿಕೆ) + dtb_ಗಾತ್ರ;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .ಆರಂಭ = dtb_phys_addr,
            .ಕೊನೆ   = dtb_phys_ಕೊನೆ
        };
    }

    /* The region of the initial thread is the user image + ipcbuf + boot info + extra */
    ಶಬ್ದ_t extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು = calculate_extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(extra_bi_ಗಾತ್ರ);
    v_region_t it_v_reg = {
        .ಆರಂಭ = ui_v_reg.ಆರಂಭ,
        .ಕೊನೆ   = extra_bi_ಚೌಕಟ್ಟು_vptr + (extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು > 0 ? ಬಿಟ್(extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು) : 0)
    };
    ಆದರೆ (it_v_reg.ಕೊನೆ >= USER_ತುದಿ) {
        /* Variable arguments for printf() require well defined integer types
         * to work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        ಬರೆ("ERROR: userland image virt [%"SEL4_PRIx_ಶಬ್ದ"..%"SEL4_PRIx_ಶಬ್ದ")"
               "exceeds USER_TOP (%"SEL4_PRIx_ಶಬ್ದ")\n",
               it_v_reg.ಆರಂಭ, it_v_reg.ಕೊನೆ, (ಶಬ್ದ_t)USER_ತುದಿ);
        ಹಿಂತಿರುಗಿ false;
    }

    /* make the free memory available to alloc_region() */
    ಆದರೆ (!arch_init_freemem(ui_reg, dtb_p_reg, it_v_reg, extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)) {
        ಬರೆ("ERROR: free memory management initialization failed\n");
        ಹಿಂತಿರುಗಿ false;
    }

    /* create the root cnode */
    ಬೇರು_cnode_cap = create_ಬೇರು_cnode();
    ಆದರೆ (cap_get_capType(ಬೇರು_cnode_cap) == cap_null_cap) {
        ಬರೆ("ERROR: root c-node creation failed\n");
        ಹಿಂತಿರುಗಿ false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(ಬೇರು_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(ಬೇರು_cnode_cap);

    /* create the bootinfo frame */
    populate_bi_ಚೌಕಟ್ಟು(0, CONFIG_MAX_ಅಂಕೆ_NODES, ipcbuf_vptr, extra_bi_ಗಾತ್ರ);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ;
    ಆದರೆ (dtb_ಗಾತ್ರ > 0) {
        ಶೀರ್ಷಿಕೆ.id = SEL4_BOOTINFO_ಶೀರ್ಷಿಕೆ_FDT;
        ಶೀರ್ಷಿಕೆ.len = ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ) + dtb_ಗಾತ್ರ;
        *(se_L4_Boot_Info_ಶೀರ್ಷಿಕೆ *)(rootserver.extra_bi + extra_bi_ಸ್ಥಾನಾಂತರ) = ಶೀರ್ಷಿಕೆ;
        extra_bi_ಸ್ಥಾನಾಂತರ += ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ);
        ಸ್ಮರಣೆ_ನಕಲು((ಶೂನ್ಯ *)(rootserver.extra_bi + extra_bi_ಸ್ಥಾನಾಂತರ),
               paddr_ಗಮ್ಯ_pptr(dtb_phys_addr),
               dtb_ಗಾತ್ರ);
        extra_bi_ಸ್ಥಾನಾಂತರ += dtb_ಗಾತ್ರ;
    }

    ಪ್ರತಿಜ್ಞಾ(extra_bi_ಗಾತ್ರ == extra_bi_ಸ್ಥಾನಾಂತರ);

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_ವಿಳಾಸ_space(ಬೇರು_cnode_cap, it_v_reg);
    ಆದರೆ (cap_get_capType(it_pd_cap) == cap_null_cap) {
        ಬರೆ("ERROR: address space creation for initial thread failed\n");
        ಹಿಂತಿರುಗಿ false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_ಚೌಕಟ್ಟು_cap(
        ಬೇರು_cnode_cap,
        it_pd_cap,
        bi_ಚೌಕಟ್ಟು_vptr
    );

    /* create and map extra bootinfo region */
    ಆದರೆ (extra_bi_ಗಾತ್ರ > 0) {
        region_t extra_bi_region = {
            .ಆರಂಭ = rootserver.extra_bi,
            .ಕೊನೆ = rootserver.extra_bi + extra_bi_ಗಾತ್ರ
        };
        extra_bi_ret =
            create_ಚೌಕಟ್ಟುಗಳು_ದ_region(
                ಬೇರು_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)extra_bi_region.ಆರಂಭ) - extra_bi_ಚೌಕಟ್ಟು_vptr
            );
        ಆದರೆ (!extra_bi_ret.success) {
            ಬರೆ("ERROR: mapping extra boot info to initial thread failed\n");
            ಹಿಂತಿರುಗಿ false;
        }
        ndks_boot.bi_ಚೌಕಟ್ಟು->extraBIPages = extra_bi_ret.region;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    init_sched_control(ಬೇರು_cnode_cap, CONFIG_MAX_ಅಂಕೆ_NODES);
#ಆದರೆ_ಅಂತ್ಯ

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_ಚೌಕಟ್ಟು_cap(ಬೇರು_cnode_cap, it_pd_cap, ipcbuf_vptr);
    ಆದರೆ (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        ಬರೆ("ERROR: could not create IPC buffer for initial thread\n");
        ಹಿಂತಿರುಗಿ false;
    }

    /* create all userland image frames */
    create_ಚೌಕಟ್ಟುಗಳು_ret =
        create_ಚೌಕಟ್ಟುಗಳು_ದ_region(
            ಬೇರು_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_ಸ್ಥಾನಾಂತರ
        );
    ಆದರೆ (!create_ಚೌಕಟ್ಟುಗಳು_ret.success) {
        ಬರೆ("ERROR: could not create all userland image frames\n");
        ಹಿಂತಿರುಗಿ false;
    }
    ndks_boot.bi_ಚೌಕಟ್ಟು->user_Image_ಚೌಕಟ್ಟುಗಳು = create_ಚೌಕಟ್ಟುಗಳು_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(ಬೇರು_cnode_cap);
    ಆದರೆ (cap_get_capType(it_ap_cap) == cap_null_cap) {
        ಬರೆ("ERROR: could not create ASID pool for initial thread\n");
        ಹಿಂತಿರುಗಿ false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಗಂಟು_STATE(ks_Cur_ಸಮಯ) = get_Current_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ

    /* create the idle thread */
    create_idle_thread();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         ಬೇರು_cnode_cap,
                         it_pd_cap,
                         v_ನಮೂದು,
                         bi_ಚೌಕಟ್ಟು_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    ಆದರೆ (initial == ಖಾಲಿ) {
        ಬರೆ("ERROR: could not create initial thread\n");
        ಹಿಂತಿರುಗಿ false;
    }

    init_core_state(initial);

    /* convert the remaining free memory into UT objects and provide the caps */
    ಆದರೆ (!create_untypeds(ಬೇರು_cnode_cap)) {
        ಬರೆ("ERROR: could not create untypteds for kernel image boot memory\n");
        ಹಿಂತಿರುಗಿ false;
    }

    /* no shared-frame caps (RISC-V has no multikernel support) */
    ndks_boot.bi_ಚೌಕಟ್ಟು->shared_ಚೌಕಟ್ಟುಗಳು = S_REG_ಖಾಲಿಗಳು;

    /* finalise the bootinfo frame */
    bi_finalise();

    ks_ಅಂಕೆ_CPUs = 1;

    SMP_COND_STATEMENT(clh_ಬೀಗ_init());
    SMP_COND_STATEMENT(release_secondary_cores());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    ಗಂಟು_ಬೀಗ_SYS;

    ಬರೆ("Booting all finished, dropped to user space\n");
    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ VISIBLE ಶೂನ್ಯ init_kernel(
    paddr_t ui_p_reg_ಆರಂಭ,
    paddr_t ui_p_reg_ಕೊನೆ,
    sword_t pv_ಸ್ಥಾನಾಂತರ,
    vptr_t  v_ನಮೂದು,
    paddr_t dtb_addr_p,
    uint32_t dtb_ಗಾತ್ರ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ,
    ಶಬ್ದ_t hart_id,
    ಶಬ್ದ_t core_id
#ಆದರೆ_ಅಂತ್ಯ
)
{
    bool_t ಫಲಿತಾಂಶ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ಕೂಡಿಸು_hart_ಗಮ್ಯ_core_map(hart_id, core_id);
    ಆದರೆ (core_id == 0) {
        ಫಲಿತಾಂಶ = try_init_kernel(ui_p_reg_ಆರಂಭ,
                                 ui_p_reg_ಕೊನೆ,
                                 pv_ಸ್ಥಾನಾಂತರ,
                                 v_ನಮೂದು,
                                 dtb_addr_p,
                                 dtb_ಗಾತ್ರ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ = try_init_kernel_secondary_core(hart_id, core_id);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = try_init_kernel(ui_p_reg_ಆರಂಭ,
                             ui_p_reg_ಕೊನೆ,
                             pv_ಸ್ಥಾನಾಂತರ,
                             v_ನಮೂದು,
                             dtb_addr_p,
                             dtb_ಗಾತ್ರ);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (!ಫಲಿತಾಂಶ) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಗಂಟು_STATE(ks_Cur_ಸಮಯ) = get_Current_ಸಮಯ();
    ಗಂಟು_STATE(ksConsumed) = 0;
#ಆದರೆ_ಅಂತ್ಯ

    ವೇಳಾಪಟ್ಟಿ();
    activateThread();
}
