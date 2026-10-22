/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <assert.h>
#ضمن <kernel/boot.h>
#ضمن <machine/io.h>
#ضمن <model/statedata.h>
#ضمن <object/interrupt.h>
#ضمن <arch/machine.h>
#ضمن <arch/kernel/boot.h>
#ضمن <arch/kernel/traps.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <arch/benchmark.h>
#ضمن <linker.h>
#ضمن <plat/machine/hardware.h>
#ضمن <machine.h>

#که_تعریف_شوی ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS ساكن متطاير _Atomic کلمه_t غوټه_boot_قلف;
#د_که_پای

BOOT_BSS ساكن region_t res_reg[عدد_RESERVED_REGIONS];

BOOT_کوډ cap_t create_mapped_it_چوکاټ_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t
                                           use_large, bool_t executable)
{
    cap_t cap;
    vm_پاڼه_اندازه_t چوکاټ_اندازه_2;

    إذا (use_large) {
        چوکاټ_اندازه_2 = RISCV_Mega_پاڼه;
    } وإلا {
        چوکاټ_اندازه_2 = RISCV_4_K_پاڼه;
    }

    cap = cap_چوکاټ_cap_new(
              asid,                            /* capFMappedASID       */
              pptr,                            /* capFBasePtr          */
              چوکاټ_اندازه_2,                      /* capFSize             */
              کلمه_From_VMRights(VMReadWrite),   /* capFVMRights         */
              0,                               /* capFIsDevice         */
              vptr                             /* capFMappedAddress    */
          );

    map_it_چوکاټ_cap(pd_cap, cap);
    أرجع cap;
}

BOOT_کوډ ساكن bool_t arch_init_freemem(region_t ui_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          کلمه_t extra_bi_اندازه_بټونه)
{
    /* Reserve the kernel image region. This may look a bit awkward, as the
     * symbols are a reference in the kernel image window, but all allocations
     * are done in terms of the main kernel window, so we do some translation.
     */
    res_reg[0] = paddr_منزل_pptr_reg(get_p_reg_kernel_img());
    صحيح شاخص = 1;

    /* add the dtb region, if it is not empty */
    إذا (dtb_p_reg.پیل) {
        إذا (شاخص >= ARRAY_اندازه(res_reg)) {
            چاپ_کړه("ERROR: no slot to add DTB to reserved regions\n");
            أرجع false;
        }
        res_reg[شاخص] = paddr_منزل_pptr_reg(dtb_p_reg);
        شاخص += 1;
    }

    /* reserve the user image region */
    إذا (شاخص >= ARRAY_اندازه(res_reg)) {
        چاپ_کړه("ERROR: no slot to add user image to reserved regions\n");
        أرجع false;
    }
    res_reg[شاخص] = ui_reg;
    شاخص += 1;

    /* avail_p_regs comes from the auto-generated code */
    أرجع init_freemem(ARRAY_اندازه(avail_p_regs), avail_p_regs,
                        شاخص, res_reg,
                        it_v_reg, extra_bi_اندازه_بټونه);
}

BOOT_کوډ ساكن فراغ init_irqs(cap_t ریښه_cnode_cap)
{
    irq_t i;

    لكل (i = 0; i <= maxIRQ; i++) {
        إذا (i != irqInvalid) {
            /* IRQ 0 is irqInvalid */
            سېټ_IRQState(IRQInactive, i);
        }
    }
    سېټ_IRQState(IRQTimer, KERNEL_TIMER_IRQ);
#که_تعریف_شوی ENABLE_SMP_SUPPORT
    سېټ_IRQState(IRQIPI, irq_remote_call_ipi);
    سېټ_IRQState(IRQIPI, irq_reschedule_ipi);
#د_که_پای
    /* provide the IRQ control cap */
    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

/* This and only this function initialises the CPU. It does NOT initialise any kernel state. */

#که_تعریف_شوی CONFIG_HAVE_FPU
BOOT_کوډ ساكن فراغ init_fpu(فراغ)
{
    سېټ_fs_clean();
    write_fcsr(0);
    disableFpu();
}
#د_که_پای

BOOT_کوډ ساكن فراغ init_cpu(فراغ)
{

    activate_kernel_vspace();
    /* Write trap entry address to stvec */
    write_stvec((کلمه_t)trap_ثبت);
    initLocalIRQController();
#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
    initTimer();
#د_که_پای

    /* disable FPU access */
    سېټ_fs_off();

#که_تعریف_شوی CONFIG_HAVE_FPU
    init_fpu();
#د_که_پای
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_کوډ ساكن فراغ init_plat(فراغ)
{
    initIRQController();
}


#که_تعریف_شوی ENABLE_SMP_SUPPORT
BOOT_کوډ ساكن bool_t try_init_kernel_secondary_core(کلمه_t hart_id, کلمه_t core_id)
{
    طالما (!غوټه_boot_قلف);

    fence_r_rw();

    init_cpu();
    غوټه_قلف_SYS;

    clock_sync_test();
    ks_عدد_CPUs++;
    init_core_state(SchedulerAction_ResumeCurrentThread);
    ifence_local();
    أرجع true;
}

BOOT_کوډ ساكن فراغ release_secondary_cores(فراغ)
{
    أكد(0 == غوټه_boot_قلف); /* Sanity check for a proper lock state. */
    غوټه_boot_قلف = 1;
    /* At this point in time the primary core (executing this code) already uses
     * the seL4 MMU/cache setup. However, the secondary cores are still using
     * the elfloader's MMU/cache setup, and thus the update of node_boot_lock
     * may not be visible there if the setups differ. Currently, the mappings
     * match, so a barrier is all that is needed.
     */
    fence_rw_rw();

    طالما (ks_عدد_CPUs != CONFIG_MAX_عدد_NODES) {
#که_تعریف_شوی ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        غوټه_STATE(ks_Cur_وخت) = get_Current_وخت();
#د_که_پای
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#د_که_پای /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

ساكن BOOT_کوډ bool_t try_init_kernel(
    paddr_t ui_p_reg_پیل,
    paddr_t ui_p_reg_پای_ټکی,
    sword_t pv_دننه_واټن,
    vptr_t  v_ثبت,
    paddr_t dtb_phys_addr,
    کلمه_t  dtb_اندازه
)
{
    cap_t ریښه_cnode_cap;
    cap_t it_pd_cap;
    cap_t it_ap_cap;
    cap_t ipcbuf_cap;
    region_t ui_reg = paddr_منزل_pptr_reg((p_region_t) {
        ui_p_reg_پیل, ui_p_reg_پای_ټکی
    });
    کلمه_t extra_bi_اندازه = 0;
    pptr_t extra_bi_دننه_واټن = 0;
    vptr_t extra_bi_چوکاټ_vptr;
    vptr_t bi_چوکاټ_vptr;
    vptr_t ipcbuf_vptr;
    create_چوکاټونه_د_region_ret_t create_چوکاټونه_ret;
    create_چوکاټونه_د_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .پیل = ui_p_reg_پیل - pv_دننه_واټن,
        .پای_ټکی   = ui_p_reg_پای_ټکی   - pv_دننه_واټن
    };

    ipcbuf_vptr = ui_v_reg.پای_ټکی;
    bi_چوکاټ_vptr = ipcbuf_vptr + بټ(پاڼه_بټونه);
    extra_bi_چوکاټ_vptr = bi_چوکاټ_vptr + بټ(se_L4_Boot_Info_چوکاټ_بټونه);

    map_kernel_کړکۍ();

    /* initialise the CPU */
    init_cpu();

    چاپ_کړه("Bootstrapping kernel\n");

    /* initialize the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_تشه;
    إذا (dtb_اندازه > 0) {
#که_تعریف_شوی CONFIG_PLAT_ROCKETCHIP_ZCU102
        /* The softcore rocketchip instantiation doesn't work well when this
         * page isn't reserved. Round up so the whole page is reserved to
         * avoid the problem
         */
        dtb_اندازه = پړاو_پورته(dtb_اندازه, پاڼه_بټونه);
#د_که_پای
        paddr_t dtb_phys_پای_ټکی = dtb_phys_addr + dtb_اندازه;
        إذا (dtb_phys_پای_ټکی < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            چاپ_کړه("ERROR: DTB location at %"SEL4_PRIx_کلمه
                   " len %"SEL4_PRIu_کلمه" invalid\n",
                   dtb_phys_addr, dtb_اندازه);
            أرجع false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        إذا (dtb_phys_پای_ټکی >= PADDR_سر) {
            چاپ_کړه("ERROR: DTB at [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_کلمه")\n",
                   dtb_phys_addr, dtb_phys_پای_ټکی, PADDR_سر);
            أرجع false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_اندازه += حجم(se_L4_Boot_Info_سرلیک) + dtb_اندازه;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .پیل = dtb_phys_addr,
            .پای_ټکی   = dtb_phys_پای_ټکی
        };
    }

    /* The region of the initial thread is the user image + ipcbuf + boot info + extra */
    کلمه_t extra_bi_اندازه_بټونه = calculate_extra_bi_اندازه_بټونه(extra_bi_اندازه);
    v_region_t it_v_reg = {
        .پیل = ui_v_reg.پیل,
        .پای_ټکی   = extra_bi_چوکاټ_vptr + (extra_bi_اندازه_بټونه > 0 ? بټ(extra_bi_اندازه_بټونه) : 0)
    };
    إذا (it_v_reg.پای_ټکی >= USER_سر) {
        /* Variable arguments for printf() require well defined integer types
         * to work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        چاپ_کړه("ERROR: userland image virt [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه")"
               "exceeds USER_TOP (%"SEL4_PRIx_کلمه")\n",
               it_v_reg.پیل, it_v_reg.پای_ټکی, (کلمه_t)USER_سر);
        أرجع false;
    }

    /* make the free memory available to alloc_region() */
    إذا (!arch_init_freemem(ui_reg, dtb_p_reg, it_v_reg, extra_bi_اندازه_بټونه)) {
        چاپ_کړه("ERROR: free memory management initialization failed\n");
        أرجع false;
    }

    /* create the root cnode */
    ریښه_cnode_cap = create_ریښه_cnode();
    إذا (cap_get_capType(ریښه_cnode_cap) == cap_null_cap) {
        چاپ_کړه("ERROR: root c-node creation failed\n");
        أرجع false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(ریښه_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(ریښه_cnode_cap);

    /* create the bootinfo frame */
    populate_bi_چوکاټ(0, CONFIG_MAX_عدد_NODES, ipcbuf_vptr, extra_bi_اندازه);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_سرلیک سرلیک;
    إذا (dtb_اندازه > 0) {
        سرلیک.id = SEL4_BOOTINFO_سرلیک_FDT;
        سرلیک.len = حجم(سرلیک) + dtb_اندازه;
        *(se_L4_Boot_Info_سرلیک *)(rootserver.extra_bi + extra_bi_دننه_واټن) = سرلیک;
        extra_bi_دننه_واټن += حجم(سرلیک);
        حافظه_نقل((فراغ *)(rootserver.extra_bi + extra_bi_دننه_واټن),
               paddr_منزل_pptr(dtb_phys_addr),
               dtb_اندازه);
        extra_bi_دننه_واټن += dtb_اندازه;
    }

    أكد(extra_bi_اندازه == extra_bi_دننه_واټن);

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_پته_space(ریښه_cnode_cap, it_v_reg);
    إذا (cap_get_capType(it_pd_cap) == cap_null_cap) {
        چاپ_کړه("ERROR: address space creation for initial thread failed\n");
        أرجع false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_چوکاټ_cap(
        ریښه_cnode_cap,
        it_pd_cap,
        bi_چوکاټ_vptr
    );

    /* create and map extra bootinfo region */
    إذا (extra_bi_اندازه > 0) {
        region_t extra_bi_region = {
            .پیل = rootserver.extra_bi,
            .پای_ټکی = rootserver.extra_bi + extra_bi_اندازه
        };
        extra_bi_ret =
            create_چوکاټونه_د_region(
                ریښه_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_منزل_paddr((فراغ *)extra_bi_region.پیل) - extra_bi_چوکاټ_vptr
            );
        إذا (!extra_bi_ret.success) {
            چاپ_کړه("ERROR: mapping extra boot info to initial thread failed\n");
            أرجع false;
        }
        ndks_boot.bi_چوکاټ->extraBIPages = extra_bi_ret.region;
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    init_sched_control(ریښه_cnode_cap, CONFIG_MAX_عدد_NODES);
#د_که_پای

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_چوکاټ_cap(ریښه_cnode_cap, it_pd_cap, ipcbuf_vptr);
    إذا (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        چاپ_کړه("ERROR: could not create IPC buffer for initial thread\n");
        أرجع false;
    }

    /* create all userland image frames */
    create_چوکاټونه_ret =
        create_چوکاټونه_د_region(
            ریښه_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_دننه_واټن
        );
    إذا (!create_چوکاټونه_ret.success) {
        چاپ_کړه("ERROR: could not create all userland image frames\n");
        أرجع false;
    }
    ndks_boot.bi_چوکاټ->user_Image_چوکاټونه = create_چوکاټونه_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(ریښه_cnode_cap);
    إذا (cap_get_capType(it_ap_cap) == cap_null_cap) {
        چاپ_کړه("ERROR: could not create ASID pool for initial thread\n");
        أرجع false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#که_تعریف_شوی CONFIG_KERNEL_MCS
    غوټه_STATE(ks_Cur_وخت) = get_Current_وخت();
#د_که_پای

    /* create the idle thread */
    create_idle_thread();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         ریښه_cnode_cap,
                         it_pd_cap,
                         v_ثبت,
                         bi_چوکاټ_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    إذا (initial == عدم) {
        چاپ_کړه("ERROR: could not create initial thread\n");
        أرجع false;
    }

    init_core_state(initial);

    /* convert the remaining free memory into UT objects and provide the caps */
    إذا (!create_untypeds(ریښه_cnode_cap)) {
        چاپ_کړه("ERROR: could not create untypteds for kernel image boot memory\n");
        أرجع false;
    }

    /* no shared-frame caps (RISC-V has no multikernel support) */
    ndks_boot.bi_چوکاټ->shared_چوکاټونه = S_REG_تشه;

    /* finalise the bootinfo frame */
    bi_finalise();

    ks_عدد_CPUs = 1;

    SMP_COND_STATEMENT(clh_قلف_init());
    SMP_COND_STATEMENT(release_secondary_cores());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    غوټه_قلف_SYS;

    چاپ_کړه("Booting all finished, dropped to user space\n");
    أرجع true;
}

BOOT_کوډ VISIBLE فراغ init_kernel(
    paddr_t ui_p_reg_پیل,
    paddr_t ui_p_reg_پای_ټکی,
    sword_t pv_دننه_واټن,
    vptr_t  v_ثبت,
    paddr_t dtb_addr_p,
    uint32_t dtb_اندازه
#که_تعریف_شوی ENABLE_SMP_SUPPORT
    ,
    کلمه_t hart_id,
    کلمه_t core_id
#د_که_پای
)
{
    bool_t پایله;

#که_تعریف_شوی ENABLE_SMP_SUPPORT
    جمع_کړه_hart_منزل_core_map(hart_id, core_id);
    إذا (core_id == 0) {
        پایله = try_init_kernel(ui_p_reg_پیل,
                                 ui_p_reg_پای_ټکی,
                                 pv_دننه_واټن,
                                 v_ثبت,
                                 dtb_addr_p,
                                 dtb_اندازه);
    } وإلا {
        پایله = try_init_kernel_secondary_core(hart_id, core_id);
    }
#وإلا
    پایله = try_init_kernel(ui_p_reg_پیل,
                             ui_p_reg_پای_ټکی,
                             pv_دننه_واټن,
                             v_ثبت,
                             dtb_addr_p,
                             dtb_اندازه);
#د_که_پای
    إذا (!پایله) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    غوټه_STATE(ks_Cur_وخت) = get_Current_وخت();
    غوټه_STATE(ksConsumed) = 0;
#د_که_پای

    مهالوېش();
    activateThread();
}
