/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/interrupt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/traps.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/benchmark.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS ਸਥਿਰ ਅਸਥਿਰ _Atomic ਸ਼ਬਦ_t ਗੰਢ_boot_ਤਾਲਾ;
#ਜੇ_ਅੰਤ

BOOT_BSS ਸਥਿਰ region_t res_reg[ਅੰਕ_RESERVED_REGIONS];

BOOT_ਕੋਡ cap_t create_mapped_it_ਚੌਖਟਾ_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t
                                           use_large, bool_t executable)
{
    cap_t cap;
    vm_ਪੰਨਾ_ਮਾਪ_t ਚੌਖਟਾ_ਮਾਪ_2;

    ਜੇ (use_large) {
        ਚੌਖਟਾ_ਮਾਪ_2 = RISCV_Mega_ਪੰਨਾ;
    } ਨਹੀਂਤਾਂ {
        ਚੌਖਟਾ_ਮਾਪ_2 = RISCV_4_K_ਪੰਨਾ;
    }

    cap = cap_ਚੌਖਟਾ_cap_new(
              asid,                            /* capFMappedASID       */
              pptr,                            /* capFBasePtr          */
              ਚੌਖਟਾ_ਮਾਪ_2,                      /* capFSize             */
              ਸ਼ਬਦ_From_VMRights(VMReadWrite),   /* capFVMRights         */
              0,                               /* capFIsDevice         */
              vptr                             /* capFMappedAddress    */
          );

    map_it_ਚੌਖਟਾ_cap(pd_cap, cap);
    ਵਾਪਸ cap;
}

BOOT_ਕੋਡ ਸਥਿਰ bool_t arch_init_freemem(region_t ui_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          ਸ਼ਬਦ_t extra_bi_ਮਾਪ_ਬਿੱਟ)
{
    /* Reserve the kernel image region. This may look a bit awkward, as the
     * symbols are a reference in the kernel image window, but all allocations
     * are done in terms of the main kernel window, so we do some translation.
     */
    res_reg[0] = paddr_ਮੰਜ਼ਿਲ_pptr_reg(get_p_reg_kernel_img());
    ਪੂਰਨ ਸੂਚਕਾਂਕ = 1;

    /* add the dtb region, if it is not empty */
    ਜੇ (dtb_p_reg.ਸ਼ੁਰੂ) {
        ਜੇ (ਸੂਚਕਾਂਕ >= ARRAY_ਮਾਪ(res_reg)) {
            ਲਿਖੋ("ERROR: no slot to add DTB to reserved regions\n");
            ਵਾਪਸ false;
        }
        res_reg[ਸੂਚਕਾਂਕ] = paddr_ਮੰਜ਼ਿਲ_pptr_reg(dtb_p_reg);
        ਸੂਚਕਾਂਕ += 1;
    }

    /* reserve the user image region */
    ਜੇ (ਸੂਚਕਾਂਕ >= ARRAY_ਮਾਪ(res_reg)) {
        ਲਿਖੋ("ERROR: no slot to add user image to reserved regions\n");
        ਵਾਪਸ false;
    }
    res_reg[ਸੂਚਕਾਂਕ] = ui_reg;
    ਸੂਚਕਾਂਕ += 1;

    /* avail_p_regs comes from the auto-generated code */
    ਵਾਪਸ init_freemem(ARRAY_ਮਾਪ(avail_p_regs), avail_p_regs,
                        ਸੂਚਕਾਂਕ, res_reg,
                        it_v_reg, extra_bi_ਮਾਪ_ਬਿੱਟ);
}

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ init_irqs(cap_t ਜੜ੍ਹ_cnode_cap)
{
    irq_t i;

    ਲਈ (i = 0; i <= maxIRQ; i++) {
        ਜੇ (i != irqInvalid) {
            /* IRQ 0 is irqInvalid */
            ਸੈੱਟ_IRQState(IRQInactive, i);
        }
    }
    ਸੈੱਟ_IRQState(IRQTimer, KERNEL_TIMER_IRQ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ਸੈੱਟ_IRQState(IRQIPI, irq_remote_call_ipi);
    ਸੈੱਟ_IRQState(IRQIPI, irq_reschedule_ipi);
#ਜੇ_ਅੰਤ
    /* provide the IRQ control cap */
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

/* This and only this function initialises the CPU. It does NOT initialise any kernel state. */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ init_fpu(ਖਾਲੀ)
{
    ਸੈੱਟ_fs_clean();
    write_fcsr(0);
    disableFpu();
}
#ਜੇ_ਅੰਤ

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ init_cpu(ਖਾਲੀ)
{

    activate_kernel_vspace();
    /* Write trap entry address to stvec */
    write_stvec((ਸ਼ਬਦ_t)trap_ਇੰਦਰਾਜ);
    initLocalIRQController();
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    initTimer();
#ਜੇ_ਅੰਤ

    /* disable FPU access */
    ਸੈੱਟ_fs_off();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
    init_fpu();
#ਜੇ_ਅੰਤ
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ init_plat(ਖਾਲੀ)
{
    initIRQController();
}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
BOOT_ਕੋਡ ਸਥਿਰ bool_t try_init_kernel_secondary_core(ਸ਼ਬਦ_t hart_id, ਸ਼ਬਦ_t core_id)
{
    ਜਦੋਂ (!ਗੰਢ_boot_ਤਾਲਾ);

    fence_r_rw();

    init_cpu();
    ਗੰਢ_ਤਾਲਾ_SYS;

    clock_sync_test();
    ks_ਅੰਕ_CPUs++;
    init_core_state(SchedulerAction_ResumeCurrentThread);
    ifence_local();
    ਵਾਪਸ true;
}

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ release_secondary_cores(ਖਾਲੀ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(0 == ਗੰਢ_boot_ਤਾਲਾ); /* Sanity check for a proper lock state. */
    ਗੰਢ_boot_ਤਾਲਾ = 1;
    /* At this point in time the primary core (executing this code) already uses
     * the seL4 MMU/cache setup. However, the secondary cores are still using
     * the elfloader's MMU/cache setup, and thus the update of node_boot_lock
     * may not be visible there if the setups differ. Currently, the mappings
     * match, so a barrier is all that is needed.
     */
    fence_rw_rw();

    ਜਦੋਂ (ks_ਅੰਕ_CPUs != CONFIG_MAX_ਅੰਕ_NODES) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        ਗੰਢ_STATE(ks_Cur_ਸਮਾਂ) = get_Current_ਸਮਾਂ();
#ਜੇ_ਅੰਤ
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

ਸਥਿਰ BOOT_ਕੋਡ bool_t try_init_kernel(
    paddr_t ui_p_reg_ਸ਼ੁਰੂ,
    paddr_t ui_p_reg_ਸਿਰਾ,
    sword_t pv_ਵਿੱਥ,
    vptr_t  v_ਇੰਦਰਾਜ,
    paddr_t dtb_phys_addr,
    ਸ਼ਬਦ_t  dtb_ਮਾਪ
)
{
    cap_t ਜੜ੍ਹ_cnode_cap;
    cap_t it_pd_cap;
    cap_t it_ap_cap;
    cap_t ipcbuf_cap;
    region_t ui_reg = paddr_ਮੰਜ਼ਿਲ_pptr_reg((p_region_t) {
        ui_p_reg_ਸ਼ੁਰੂ, ui_p_reg_ਸਿਰਾ
    });
    ਸ਼ਬਦ_t extra_bi_ਮਾਪ = 0;
    pptr_t extra_bi_ਵਿੱਥ = 0;
    vptr_t extra_bi_ਚੌਖਟਾ_vptr;
    vptr_t bi_ਚੌਖਟਾ_vptr;
    vptr_t ipcbuf_vptr;
    create_ਚੌਖਟੇ_ਦਾ_region_ret_t create_ਚੌਖਟੇ_ret;
    create_ਚੌਖਟੇ_ਦਾ_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .ਸ਼ੁਰੂ = ui_p_reg_ਸ਼ੁਰੂ - pv_ਵਿੱਥ,
        .ਸਿਰਾ   = ui_p_reg_ਸਿਰਾ   - pv_ਵਿੱਥ
    };

    ipcbuf_vptr = ui_v_reg.ਸਿਰਾ;
    bi_ਚੌਖਟਾ_vptr = ipcbuf_vptr + ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ);
    extra_bi_ਚੌਖਟਾ_vptr = bi_ਚੌਖਟਾ_vptr + ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Boot_Info_ਚੌਖਟਾ_ਬਿੱਟ);

    map_kernel_ਖਿੜਕੀ();

    /* initialise the CPU */
    init_cpu();

    ਲਿਖੋ("Bootstrapping kernel\n");

    /* initialize the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_ਖਾਲੀ_ਥਾਵਾਂ;
    ਜੇ (dtb_ਮਾਪ > 0) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PLAT_ROCKETCHIP_ZCU102
        /* The softcore rocketchip instantiation doesn't work well when this
         * page isn't reserved. Round up so the whole page is reserved to
         * avoid the problem
         */
        dtb_ਮਾਪ = ਗੇੜਾ_ਉੱਪਰ(dtb_ਮਾਪ, ਪੰਨਾ_ਬਿੱਟ);
#ਜੇ_ਅੰਤ
        paddr_t dtb_phys_ਸਿਰਾ = dtb_phys_addr + dtb_ਮਾਪ;
        ਜੇ (dtb_phys_ਸਿਰਾ < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            ਲਿਖੋ("ERROR: DTB location at %"SEL4_PRIx_ਸ਼ਬਦ
                   " len %"SEL4_PRIu_ਸ਼ਬਦ" invalid\n",
                   dtb_phys_addr, dtb_ਮਾਪ);
            ਵਾਪਸ false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        ਜੇ (dtb_phys_ਸਿਰਾ >= PADDR_ਸਿਖਰ) {
            ਲਿਖੋ("ERROR: DTB at [%"SEL4_PRIx_ਸ਼ਬਦ"..%"SEL4_PRIx_ਸ਼ਬਦ") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_ਸ਼ਬਦ")\n",
                   dtb_phys_addr, dtb_phys_ਸਿਰਾ, PADDR_ਸਿਖਰ);
            ਵਾਪਸ false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_ਮਾਪ += ਆਕਾਰ(se_L4_Boot_Info_ਸਿਰਲੇਖ) + dtb_ਮਾਪ;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .ਸ਼ੁਰੂ = dtb_phys_addr,
            .ਸਿਰਾ   = dtb_phys_ਸਿਰਾ
        };
    }

    /* The region of the initial thread is the user image + ipcbuf + boot info + extra */
    ਸ਼ਬਦ_t extra_bi_ਮਾਪ_ਬਿੱਟ = calculate_extra_bi_ਮਾਪ_ਬਿੱਟ(extra_bi_ਮਾਪ);
    v_region_t it_v_reg = {
        .ਸ਼ੁਰੂ = ui_v_reg.ਸ਼ੁਰੂ,
        .ਸਿਰਾ   = extra_bi_ਚੌਖਟਾ_vptr + (extra_bi_ਮਾਪ_ਬਿੱਟ > 0 ? ਦੋਆਧਾਰੀ_ਅੰਕ(extra_bi_ਮਾਪ_ਬਿੱਟ) : 0)
    };
    ਜੇ (it_v_reg.ਸਿਰਾ >= USER_ਸਿਖਰ) {
        /* Variable arguments for printf() require well defined integer types
         * to work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        ਲਿਖੋ("ERROR: userland image virt [%"SEL4_PRIx_ਸ਼ਬਦ"..%"SEL4_PRIx_ਸ਼ਬਦ")"
               "exceeds USER_TOP (%"SEL4_PRIx_ਸ਼ਬਦ")\n",
               it_v_reg.ਸ਼ੁਰੂ, it_v_reg.ਸਿਰਾ, (ਸ਼ਬਦ_t)USER_ਸਿਖਰ);
        ਵਾਪਸ false;
    }

    /* make the free memory available to alloc_region() */
    ਜੇ (!arch_init_freemem(ui_reg, dtb_p_reg, it_v_reg, extra_bi_ਮਾਪ_ਬਿੱਟ)) {
        ਲਿਖੋ("ERROR: free memory management initialization failed\n");
        ਵਾਪਸ false;
    }

    /* create the root cnode */
    ਜੜ੍ਹ_cnode_cap = create_ਜੜ੍ਹ_cnode();
    ਜੇ (cap_get_capType(ਜੜ੍ਹ_cnode_cap) == cap_null_cap) {
        ਲਿਖੋ("ERROR: root c-node creation failed\n");
        ਵਾਪਸ false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(ਜੜ੍ਹ_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(ਜੜ੍ਹ_cnode_cap);

    /* create the bootinfo frame */
    populate_bi_ਚੌਖਟਾ(0, CONFIG_MAX_ਅੰਕ_NODES, ipcbuf_vptr, extra_bi_ਮਾਪ);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_ਸਿਰਲੇਖ ਸਿਰਲੇਖ;
    ਜੇ (dtb_ਮਾਪ > 0) {
        ਸਿਰਲੇਖ.id = SEL4_BOOTINFO_ਸਿਰਲੇਖ_FDT;
        ਸਿਰਲੇਖ.len = ਆਕਾਰ(ਸਿਰਲੇਖ) + dtb_ਮਾਪ;
        *(se_L4_Boot_Info_ਸਿਰਲੇਖ *)(rootserver.extra_bi + extra_bi_ਵਿੱਥ) = ਸਿਰਲੇਖ;
        extra_bi_ਵਿੱਥ += ਆਕਾਰ(ਸਿਰਲੇਖ);
        ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਖਾਲੀ *)(rootserver.extra_bi + extra_bi_ਵਿੱਥ),
               paddr_ਮੰਜ਼ਿਲ_pptr(dtb_phys_addr),
               dtb_ਮਾਪ);
        extra_bi_ਵਿੱਥ += dtb_ਮਾਪ;
    }

    ਪ੍ਰਤਿਜ੍ਞਾ(extra_bi_ਮਾਪ == extra_bi_ਵਿੱਥ);

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_ਪਤਾ_space(ਜੜ੍ਹ_cnode_cap, it_v_reg);
    ਜੇ (cap_get_capType(it_pd_cap) == cap_null_cap) {
        ਲਿਖੋ("ERROR: address space creation for initial thread failed\n");
        ਵਾਪਸ false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_ਚੌਖਟਾ_cap(
        ਜੜ੍ਹ_cnode_cap,
        it_pd_cap,
        bi_ਚੌਖਟਾ_vptr
    );

    /* create and map extra bootinfo region */
    ਜੇ (extra_bi_ਮਾਪ > 0) {
        region_t extra_bi_region = {
            .ਸ਼ੁਰੂ = rootserver.extra_bi,
            .ਸਿਰਾ = rootserver.extra_bi + extra_bi_ਮਾਪ
        };
        extra_bi_ret =
            create_ਚੌਖਟੇ_ਦਾ_region(
                ਜੜ੍ਹ_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)extra_bi_region.ਸ਼ੁਰੂ) - extra_bi_ਚੌਖਟਾ_vptr
            );
        ਜੇ (!extra_bi_ret.success) {
            ਲਿਖੋ("ERROR: mapping extra boot info to initial thread failed\n");
            ਵਾਪਸ false;
        }
        ndks_boot.bi_ਚੌਖਟਾ->extraBIPages = extra_bi_ret.region;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    init_sched_control(ਜੜ੍ਹ_cnode_cap, CONFIG_MAX_ਅੰਕ_NODES);
#ਜੇ_ਅੰਤ

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_ਚੌਖਟਾ_cap(ਜੜ੍ਹ_cnode_cap, it_pd_cap, ipcbuf_vptr);
    ਜੇ (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        ਲਿਖੋ("ERROR: could not create IPC buffer for initial thread\n");
        ਵਾਪਸ false;
    }

    /* create all userland image frames */
    create_ਚੌਖਟੇ_ret =
        create_ਚੌਖਟੇ_ਦਾ_region(
            ਜੜ੍ਹ_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_ਵਿੱਥ
        );
    ਜੇ (!create_ਚੌਖਟੇ_ret.success) {
        ਲਿਖੋ("ERROR: could not create all userland image frames\n");
        ਵਾਪਸ false;
    }
    ndks_boot.bi_ਚੌਖਟਾ->user_Image_ਚੌਖਟੇ = create_ਚੌਖਟੇ_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(ਜੜ੍ਹ_cnode_cap);
    ਜੇ (cap_get_capType(it_ap_cap) == cap_null_cap) {
        ਲਿਖੋ("ERROR: could not create ASID pool for initial thread\n");
        ਵਾਪਸ false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਗੰਢ_STATE(ks_Cur_ਸਮਾਂ) = get_Current_ਸਮਾਂ();
#ਜੇ_ਅੰਤ

    /* create the idle thread */
    create_idle_thread();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         ਜੜ੍ਹ_cnode_cap,
                         it_pd_cap,
                         v_ਇੰਦਰਾਜ,
                         bi_ਚੌਖਟਾ_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    ਜੇ (initial == ਨਿਰਾ) {
        ਲਿਖੋ("ERROR: could not create initial thread\n");
        ਵਾਪਸ false;
    }

    init_core_state(initial);

    /* convert the remaining free memory into UT objects and provide the caps */
    ਜੇ (!create_untypeds(ਜੜ੍ਹ_cnode_cap)) {
        ਲਿਖੋ("ERROR: could not create untypteds for kernel image boot memory\n");
        ਵਾਪਸ false;
    }

    /* no shared-frame caps (RISC-V has no multikernel support) */
    ndks_boot.bi_ਚੌਖਟਾ->shared_ਚੌਖਟੇ = S_REG_ਖਾਲੀ_ਥਾਵਾਂ;

    /* finalise the bootinfo frame */
    bi_finalise();

    ks_ਅੰਕ_CPUs = 1;

    SMP_COND_STATEMENT(clh_ਤਾਲਾ_init());
    SMP_COND_STATEMENT(release_secondary_cores());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    ਗੰਢ_ਤਾਲਾ_SYS;

    ਲਿਖੋ("Booting all finished, dropped to user space\n");
    ਵਾਪਸ true;
}

BOOT_ਕੋਡ VISIBLE ਖਾਲੀ init_kernel(
    paddr_t ui_p_reg_ਸ਼ੁਰੂ,
    paddr_t ui_p_reg_ਸਿਰਾ,
    sword_t pv_ਵਿੱਥ,
    vptr_t  v_ਇੰਦਰਾਜ,
    paddr_t dtb_addr_p,
    uint32_t dtb_ਮਾਪ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ,
    ਸ਼ਬਦ_t hart_id,
    ਸ਼ਬਦ_t core_id
#ਜੇ_ਅੰਤ
)
{
    bool_t ਨਤੀਜਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ਜੋੜੋ_hart_ਮੰਜ਼ਿਲ_core_map(hart_id, core_id);
    ਜੇ (core_id == 0) {
        ਨਤੀਜਾ = try_init_kernel(ui_p_reg_ਸ਼ੁਰੂ,
                                 ui_p_reg_ਸਿਰਾ,
                                 pv_ਵਿੱਥ,
                                 v_ਇੰਦਰਾਜ,
                                 dtb_addr_p,
                                 dtb_ਮਾਪ);
    } ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ = try_init_kernel_secondary_core(hart_id, core_id);
    }
#ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = try_init_kernel(ui_p_reg_ਸ਼ੁਰੂ,
                             ui_p_reg_ਸਿਰਾ,
                             pv_ਵਿੱਥ,
                             v_ਇੰਦਰਾਜ,
                             dtb_addr_p,
                             dtb_ਮਾਪ);
#ਜੇ_ਅੰਤ
    ਜੇ (!ਨਤੀਜਾ) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਗੰਢ_STATE(ks_Cur_ਸਮਾਂ) = get_Current_ਸਮਾਂ();
    ਗੰਢ_STATE(ksConsumed) = 0;
#ਜੇ_ਅੰਤ

    ਸਮਾਂ_ਸਾਰਣੀ();
    activateThread();
}
