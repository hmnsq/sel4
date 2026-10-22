/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <kernel/boot.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <object/interrupt.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/kernel/boot.h>
#అన్తర్భావ <arch/kernel/traps.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <arch/benchmark.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/hardware.h>
#అన్తర్భావ <machine.h>

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS స్థిర అస్థిర _Atomic మాట_t కణుపు_boot_తాళం;
#అయితే_ముగింపు

BOOT_BSS స్థిర region_t res_reg[అంకె_RESERVED_REGIONS];

BOOT_సంకేతం cap_t create_mapped_it_చట్రం_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t
                                           use_large, bool_t executable)
{
    cap_t cap;
    vm_పుట_కొలత_t చట్రం_కొలత_2;

    ఒకవేళ (use_large) {
        చట్రం_కొలత_2 = RISCV_Mega_పుట;
    } లేకపోతే {
        చట్రం_కొలత_2 = RISCV_4_K_పుట;
    }

    cap = cap_చట్రం_cap_new(
              asid,                            /* capFMappedASID       */
              pptr,                            /* capFBasePtr          */
              చట్రం_కొలత_2,                      /* capFSize             */
              మాట_From_VMRights(VMReadWrite),   /* capFVMRights         */
              0,                               /* capFIsDevice         */
              vptr                             /* capFMappedAddress    */
          );

    map_it_చట్రం_cap(pd_cap, cap);
    తిరిగి cap;
}

BOOT_సంకేతం స్థిర bool_t arch_init_freemem(region_t ui_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          మాట_t extra_bi_కొలత_బిట్లు)
{
    /* Reserve the kernel image region. This may look a bit awkward, as the
     * symbols are a reference in the kernel image window, but all allocations
     * are done in terms of the main kernel window, so we do some translation.
     */
    res_reg[0] = paddr_గమ్యం_pptr_reg(get_p_reg_kernel_img());
    పూర్ణాంకం సూచీ = 1;

    /* add the dtb region, if it is not empty */
    ఒకవేళ (dtb_p_reg.ప్రారంభం) {
        ఒకవేళ (సూచీ >= ARRAY_కొలత(res_reg)) {
            రాయి("ERROR: no slot to add DTB to reserved regions\n");
            తిరిగి false;
        }
        res_reg[సూచీ] = paddr_గమ్యం_pptr_reg(dtb_p_reg);
        సూచీ += 1;
    }

    /* reserve the user image region */
    ఒకవేళ (సూచీ >= ARRAY_కొలత(res_reg)) {
        రాయి("ERROR: no slot to add user image to reserved regions\n");
        తిరిగి false;
    }
    res_reg[సూచీ] = ui_reg;
    సూచీ += 1;

    /* avail_p_regs comes from the auto-generated code */
    తిరిగి init_freemem(ARRAY_కొలత(avail_p_regs), avail_p_regs,
                        సూచీ, res_reg,
                        it_v_reg, extra_bi_కొలత_బిట్లు);
}

BOOT_సంకేతం స్థిర శూన్య init_irqs(cap_t మూలం_cnode_cap)
{
    irq_t i;

    కొరకు (i = 0; i <= maxIRQ; i++) {
        ఒకవేళ (i != irqInvalid) {
            /* IRQ 0 is irqInvalid */
            సమితి_IRQState(IRQInactive, i);
        }
    }
    సమితి_IRQState(IRQTimer, KERNEL_TIMER_IRQ);
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    సమితి_IRQState(IRQIPI, irq_remote_call_ipi);
    సమితి_IRQState(IRQIPI, irq_reschedule_ipi);
#అయితే_ముగింపు
    /* provide the IRQ control cap */
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

/* This and only this function initialises the CPU. It does NOT initialise any kernel state. */

#నిర్వచితమైతే CONFIG_HAVE_FPU
BOOT_సంకేతం స్థిర శూన్య init_fpu(శూన్య)
{
    సమితి_fs_clean();
    write_fcsr(0);
    disableFpu();
}
#అయితే_ముగింపు

BOOT_సంకేతం స్థిర శూన్య init_cpu(శూన్య)
{

    activate_kernel_vspace();
    /* Write trap entry address to stvec */
    write_stvec((మాట_t)trap_నమోదు);
    initLocalIRQController();
#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
    initTimer();
#అయితే_ముగింపు

    /* disable FPU access */
    సమితి_fs_off();

#నిర్వచితమైతే CONFIG_HAVE_FPU
    init_fpu();
#అయితే_ముగింపు
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_సంకేతం స్థిర శూన్య init_plat(శూన్య)
{
    initIRQController();
}


#నిర్వచితమైతే ENABLE_SMP_SUPPORT
BOOT_సంకేతం స్థిర bool_t try_init_kernel_secondary_core(మాట_t hart_id, మాట_t core_id)
{
    అయితే (!కణుపు_boot_తాళం);

    fence_r_rw();

    init_cpu();
    కణుపు_తాళం_SYS;

    clock_sync_test();
    ks_అంకె_CPUs++;
    init_core_state(SchedulerAction_ResumeCurrentThread);
    ifence_local();
    తిరిగి true;
}

BOOT_సంకేతం స్థిర శూన్య release_secondary_cores(శూన్య)
{
    ప్రతిజ్ఞా(0 == కణుపు_boot_తాళం); /* Sanity check for a proper lock state. */
    కణుపు_boot_తాళం = 1;
    /* At this point in time the primary core (executing this code) already uses
     * the seL4 MMU/cache setup. However, the secondary cores are still using
     * the elfloader's MMU/cache setup, and thus the update of node_boot_lock
     * may not be visible there if the setups differ. Currently, the mappings
     * match, so a barrier is all that is needed.
     */
    fence_rw_rw();

    అయితే (ks_అంకె_CPUs != CONFIG_MAX_అంకె_NODES) {
#నిర్వచితమైతే ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        కణుపు_STATE(ks_Cur_సమయం) = get_Current_సమయం();
#అయితే_ముగింపు
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

స్థిర BOOT_సంకేతం bool_t try_init_kernel(
    paddr_t ui_p_reg_ప్రారంభం,
    paddr_t ui_p_reg_చివర,
    sword_t pv_స్థానభ్రంశం,
    vptr_t  v_నమోదు,
    paddr_t dtb_phys_addr,
    మాట_t  dtb_కొలత
)
{
    cap_t మూలం_cnode_cap;
    cap_t it_pd_cap;
    cap_t it_ap_cap;
    cap_t ipcbuf_cap;
    region_t ui_reg = paddr_గమ్యం_pptr_reg((p_region_t) {
        ui_p_reg_ప్రారంభం, ui_p_reg_చివర
    });
    మాట_t extra_bi_కొలత = 0;
    pptr_t extra_bi_స్థానభ్రంశం = 0;
    vptr_t extra_bi_చట్రం_vptr;
    vptr_t bi_చట్రం_vptr;
    vptr_t ipcbuf_vptr;
    create_చట్రాలు_యొక్క_region_ret_t create_చట్రాలు_ret;
    create_చట్రాలు_యొక్క_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .ప్రారంభం = ui_p_reg_ప్రారంభం - pv_స్థానభ్రంశం,
        .చివర   = ui_p_reg_చివర   - pv_స్థానభ్రంశం
    };

    ipcbuf_vptr = ui_v_reg.చివర;
    bi_చట్రం_vptr = ipcbuf_vptr + బిట్టు(పుట_బిట్లు);
    extra_bi_చట్రం_vptr = bi_చట్రం_vptr + బిట్టు(se_L4_Boot_Info_చట్రం_బిట్లు);

    map_kernel_కిటికీ();

    /* initialise the CPU */
    init_cpu();

    రాయి("Bootstrapping kernel\n");

    /* initialize the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_ఖాళీలు;
    ఒకవేళ (dtb_కొలత > 0) {
#నిర్వచితమైతే CONFIG_PLAT_ROCKETCHIP_ZCU102
        /* The softcore rocketchip instantiation doesn't work well when this
         * page isn't reserved. Round up so the whole page is reserved to
         * avoid the problem
         */
        dtb_కొలత = విడత_పైకి(dtb_కొలత, పుట_బిట్లు);
#అయితే_ముగింపు
        paddr_t dtb_phys_చివర = dtb_phys_addr + dtb_కొలత;
        ఒకవేళ (dtb_phys_చివర < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            రాయి("ERROR: DTB location at %"SEL4_PRIx_మాట
                   " len %"SEL4_PRIu_మాట" invalid\n",
                   dtb_phys_addr, dtb_కొలత);
            తిరిగి false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        ఒకవేళ (dtb_phys_చివర >= PADDR_శిఖరం) {
            రాయి("ERROR: DTB at [%"SEL4_PRIx_మాట"..%"SEL4_PRIx_మాట") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_మాట")\n",
                   dtb_phys_addr, dtb_phys_చివర, PADDR_శిఖరం);
            తిరిగి false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_కొలత += పరిమాణం(se_L4_Boot_Info_శీర్షిక) + dtb_కొలత;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .ప్రారంభం = dtb_phys_addr,
            .చివర   = dtb_phys_చివర
        };
    }

    /* The region of the initial thread is the user image + ipcbuf + boot info + extra */
    మాట_t extra_bi_కొలత_బిట్లు = calculate_extra_bi_కొలత_బిట్లు(extra_bi_కొలత);
    v_region_t it_v_reg = {
        .ప్రారంభం = ui_v_reg.ప్రారంభం,
        .చివర   = extra_bi_చట్రం_vptr + (extra_bi_కొలత_బిట్లు > 0 ? బిట్టు(extra_bi_కొలత_బిట్లు) : 0)
    };
    ఒకవేళ (it_v_reg.చివర >= USER_శిఖరం) {
        /* Variable arguments for printf() require well defined integer types
         * to work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        రాయి("ERROR: userland image virt [%"SEL4_PRIx_మాట"..%"SEL4_PRIx_మాట")"
               "exceeds USER_TOP (%"SEL4_PRIx_మాట")\n",
               it_v_reg.ప్రారంభం, it_v_reg.చివర, (మాట_t)USER_శిఖరం);
        తిరిగి false;
    }

    /* make the free memory available to alloc_region() */
    ఒకవేళ (!arch_init_freemem(ui_reg, dtb_p_reg, it_v_reg, extra_bi_కొలత_బిట్లు)) {
        రాయి("ERROR: free memory management initialization failed\n");
        తిరిగి false;
    }

    /* create the root cnode */
    మూలం_cnode_cap = create_మూలం_cnode();
    ఒకవేళ (cap_get_capType(మూలం_cnode_cap) == cap_null_cap) {
        రాయి("ERROR: root c-node creation failed\n");
        తిరిగి false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(మూలం_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(మూలం_cnode_cap);

    /* create the bootinfo frame */
    populate_bi_చట్రం(0, CONFIG_MAX_అంకె_NODES, ipcbuf_vptr, extra_bi_కొలత);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_శీర్షిక శీర్షిక;
    ఒకవేళ (dtb_కొలత > 0) {
        శీర్షిక.id = SEL4_BOOTINFO_శీర్షిక_FDT;
        శీర్షిక.len = పరిమాణం(శీర్షిక) + dtb_కొలత;
        *(se_L4_Boot_Info_శీర్షిక *)(rootserver.extra_bi + extra_bi_స్థానభ్రంశం) = శీర్షిక;
        extra_bi_స్థానభ్రంశం += పరిమాణం(శీర్షిక);
        జ్ఞాపకం_నకలు((శూన్య *)(rootserver.extra_bi + extra_bi_స్థానభ్రంశం),
               paddr_గమ్యం_pptr(dtb_phys_addr),
               dtb_కొలత);
        extra_bi_స్థానభ్రంశం += dtb_కొలత;
    }

    ప్రతిజ్ఞా(extra_bi_కొలత == extra_bi_స్థానభ్రంశం);

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_చిరునామా_space(మూలం_cnode_cap, it_v_reg);
    ఒకవేళ (cap_get_capType(it_pd_cap) == cap_null_cap) {
        రాయి("ERROR: address space creation for initial thread failed\n");
        తిరిగి false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_చట్రం_cap(
        మూలం_cnode_cap,
        it_pd_cap,
        bi_చట్రం_vptr
    );

    /* create and map extra bootinfo region */
    ఒకవేళ (extra_bi_కొలత > 0) {
        region_t extra_bi_region = {
            .ప్రారంభం = rootserver.extra_bi,
            .చివర = rootserver.extra_bi + extra_bi_కొలత
        };
        extra_bi_ret =
            create_చట్రాలు_యొక్క_region(
                మూలం_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_గమ్యం_paddr((శూన్య *)extra_bi_region.ప్రారంభం) - extra_bi_చట్రం_vptr
            );
        ఒకవేళ (!extra_bi_ret.success) {
            రాయి("ERROR: mapping extra boot info to initial thread failed\n");
            తిరిగి false;
        }
        ndks_boot.bi_చట్రం->extraBIPages = extra_bi_ret.region;
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    init_sched_control(మూలం_cnode_cap, CONFIG_MAX_అంకె_NODES);
#అయితే_ముగింపు

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_చట్రం_cap(మూలం_cnode_cap, it_pd_cap, ipcbuf_vptr);
    ఒకవేళ (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        రాయి("ERROR: could not create IPC buffer for initial thread\n");
        తిరిగి false;
    }

    /* create all userland image frames */
    create_చట్రాలు_ret =
        create_చట్రాలు_యొక్క_region(
            మూలం_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_స్థానభ్రంశం
        );
    ఒకవేళ (!create_చట్రాలు_ret.success) {
        రాయి("ERROR: could not create all userland image frames\n");
        తిరిగి false;
    }
    ndks_boot.bi_చట్రం->user_Image_చట్రాలు = create_చట్రాలు_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(మూలం_cnode_cap);
    ఒకవేళ (cap_get_capType(it_ap_cap) == cap_null_cap) {
        రాయి("ERROR: could not create ASID pool for initial thread\n");
        తిరిగి false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    కణుపు_STATE(ks_Cur_సమయం) = get_Current_సమయం();
#అయితే_ముగింపు

    /* create the idle thread */
    create_idle_thread();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         మూలం_cnode_cap,
                         it_pd_cap,
                         v_నమోదు,
                         bi_చట్రం_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    ఒకవేళ (initial == ఖాళీ) {
        రాయి("ERROR: could not create initial thread\n");
        తిరిగి false;
    }

    init_core_state(initial);

    /* convert the remaining free memory into UT objects and provide the caps */
    ఒకవేళ (!create_untypeds(మూలం_cnode_cap)) {
        రాయి("ERROR: could not create untypteds for kernel image boot memory\n");
        తిరిగి false;
    }

    /* no shared-frame caps (RISC-V has no multikernel support) */
    ndks_boot.bi_చట్రం->shared_చట్రాలు = S_REG_ఖాళీలు;

    /* finalise the bootinfo frame */
    bi_finalise();

    ks_అంకె_CPUs = 1;

    SMP_COND_STATEMENT(clh_తాళం_init());
    SMP_COND_STATEMENT(release_secondary_cores());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    కణుపు_తాళం_SYS;

    రాయి("Booting all finished, dropped to user space\n");
    తిరిగి true;
}

BOOT_సంకేతం VISIBLE శూన్య init_kernel(
    paddr_t ui_p_reg_ప్రారంభం,
    paddr_t ui_p_reg_చివర,
    sword_t pv_స్థానభ్రంశం,
    vptr_t  v_నమోదు,
    paddr_t dtb_addr_p,
    uint32_t dtb_కొలత
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    ,
    మాట_t hart_id,
    మాట_t core_id
#అయితే_ముగింపు
)
{
    bool_t ఫలితం;

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    కూడు_hart_గమ్యం_core_map(hart_id, core_id);
    ఒకవేళ (core_id == 0) {
        ఫలితం = try_init_kernel(ui_p_reg_ప్రారంభం,
                                 ui_p_reg_చివర,
                                 pv_స్థానభ్రంశం,
                                 v_నమోదు,
                                 dtb_addr_p,
                                 dtb_కొలత);
    } లేకపోతే {
        ఫలితం = try_init_kernel_secondary_core(hart_id, core_id);
    }
#లేకపోతే
    ఫలితం = try_init_kernel(ui_p_reg_ప్రారంభం,
                             ui_p_reg_చివర,
                             pv_స్థానభ్రంశం,
                             v_నమోదు,
                             dtb_addr_p,
                             dtb_కొలత);
#అయితే_ముగింపు
    ఒకవేళ (!ఫలితం) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    కణుపు_STATE(ks_Cur_సమయం) = get_Current_సమయం();
    కణుపు_STATE(ksConsumed) = 0;
#అయితే_ముగింపు

    కాలపట్టిక();
    activateThread();
}
