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

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS ساكن متطاير _Atomic لفظ_t ڳنڍ_boot_تالو;
#جيڪڏهن_پورو

BOOT_BSS ساكن region_t res_reg[انگ_RESERVED_REGIONS];

BOOT_ڪوڊ cap_t create_mapped_it_چوکٺو_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t
                                           use_large, bool_t executable)
{
    cap_t cap;
    vm_صفحو_سائيز_t چوکٺو_سائيز_2;

    إذا (use_large) {
        چوکٺو_سائيز_2 = RISCV_Mega_صفحو;
    } وإلا {
        چوکٺو_سائيز_2 = RISCV_4_K_صفحو;
    }

    cap = cap_چوکٺو_cap_new(
              asid,                            /* capFMappedASID       */
              pptr,                            /* capFBasePtr          */
              چوکٺو_سائيز_2,                      /* capFSize             */
              لفظ_From_VMRights(VMReadWrite),   /* capFVMRights         */
              0,                               /* capFIsDevice         */
              vptr                             /* capFMappedAddress    */
          );

    map_it_چوکٺو_cap(pd_cap, cap);
    أرجع cap;
}

BOOT_ڪوڊ ساكن bool_t arch_init_freemem(region_t ui_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          لفظ_t extra_bi_سائيز_بٽ)
{
    /* Reserve the kernel image region. This may look a bit awkward, as the
     * symbols are a reference in the kernel image window, but all allocations
     * are done in terms of the main kernel window, so we do some translation.
     */
    res_reg[0] = paddr_منزل_pptr_reg(get_p_reg_kernel_img());
    صحيح اشاريو = 1;

    /* add the dtb region, if it is not empty */
    إذا (dtb_p_reg.شروعات) {
        إذا (اشاريو >= ARRAY_سائيز(res_reg)) {
            لکو("ERROR: no slot to add DTB to reserved regions\n");
            أرجع false;
        }
        res_reg[اشاريو] = paddr_منزل_pptr_reg(dtb_p_reg);
        اشاريو += 1;
    }

    /* reserve the user image region */
    إذا (اشاريو >= ARRAY_سائيز(res_reg)) {
        لکو("ERROR: no slot to add user image to reserved regions\n");
        أرجع false;
    }
    res_reg[اشاريو] = ui_reg;
    اشاريو += 1;

    /* avail_p_regs comes from the auto-generated code */
    أرجع init_freemem(ARRAY_سائيز(avail_p_regs), avail_p_regs,
                        اشاريو, res_reg,
                        it_v_reg, extra_bi_سائيز_بٽ);
}

BOOT_ڪوڊ ساكن فراغ init_irqs(cap_t پاڙ_cnode_cap)
{
    irq_t i;

    لكل (i = 0; i <= maxIRQ; i++) {
        إذا (i != irqInvalid) {
            /* IRQ 0 is irqInvalid */
            سيٽ_IRQState(IRQInactive, i);
        }
    }
    سيٽ_IRQState(IRQTimer, KERNEL_TIMER_IRQ);
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    سيٽ_IRQState(IRQIPI, irq_remote_call_ipi);
    سيٽ_IRQState(IRQIPI, irq_reschedule_ipi);
#جيڪڏهن_پورو
    /* provide the IRQ control cap */
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

/* This and only this function initialises the CPU. It does NOT initialise any kernel state. */

#جيڪڏهن_وصف_ٿيل CONFIG_HAVE_FPU
BOOT_ڪوڊ ساكن فراغ init_fpu(فراغ)
{
    سيٽ_fs_clean();
    write_fcsr(0);
    disableFpu();
}
#جيڪڏهن_پورو

BOOT_ڪوڊ ساكن فراغ init_cpu(فراغ)
{

    activate_kernel_vspace();
    /* Write trap entry address to stvec */
    write_stvec((لفظ_t)trap_داخلا);
    initLocalIRQController();
#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
    initTimer();
#جيڪڏهن_پورو

    /* disable FPU access */
    سيٽ_fs_off();

#جيڪڏهن_وصف_ٿيل CONFIG_HAVE_FPU
    init_fpu();
#جيڪڏهن_پورو
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_ڪوڊ ساكن فراغ init_plat(فراغ)
{
    initIRQController();
}


#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
BOOT_ڪوڊ ساكن bool_t try_init_kernel_secondary_core(لفظ_t hart_id, لفظ_t core_id)
{
    طالما (!ڳنڍ_boot_تالو);

    fence_r_rw();

    init_cpu();
    ڳنڍ_تالو_SYS;

    clock_sync_test();
    ks_انگ_CPUs++;
    init_core_state(SchedulerAction_ResumeCurrentThread);
    ifence_local();
    أرجع true;
}

BOOT_ڪوڊ ساكن فراغ release_secondary_cores(فراغ)
{
    أكد(0 == ڳنڍ_boot_تالو); /* Sanity check for a proper lock state. */
    ڳنڍ_boot_تالو = 1;
    /* At this point in time the primary core (executing this code) already uses
     * the seL4 MMU/cache setup. However, the secondary cores are still using
     * the elfloader's MMU/cache setup, and thus the update of node_boot_lock
     * may not be visible there if the setups differ. Currently, the mappings
     * match, so a barrier is all that is needed.
     */
    fence_rw_rw();

    طالما (ks_انگ_CPUs != CONFIG_MAX_انگ_NODES) {
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        ڳنڍ_STATE(ks_Cur_وقت) = get_Current_وقت();
#جيڪڏهن_پورو
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

ساكن BOOT_ڪوڊ bool_t try_init_kernel(
    paddr_t ui_p_reg_شروعات,
    paddr_t ui_p_reg_پڇاڙي,
    sword_t pv_وٿي,
    vptr_t  v_داخلا,
    paddr_t dtb_phys_addr,
    لفظ_t  dtb_سائيز
)
{
    cap_t پاڙ_cnode_cap;
    cap_t it_pd_cap;
    cap_t it_ap_cap;
    cap_t ipcbuf_cap;
    region_t ui_reg = paddr_منزل_pptr_reg((p_region_t) {
        ui_p_reg_شروعات, ui_p_reg_پڇاڙي
    });
    لفظ_t extra_bi_سائيز = 0;
    pptr_t extra_bi_وٿي = 0;
    vptr_t extra_bi_چوکٺو_vptr;
    vptr_t bi_چوکٺو_vptr;
    vptr_t ipcbuf_vptr;
    create_چوکٺا_جو_region_ret_t create_چوکٺا_ret;
    create_چوکٺا_جو_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .شروعات = ui_p_reg_شروعات - pv_وٿي,
        .پڇاڙي   = ui_p_reg_پڇاڙي   - pv_وٿي
    };

    ipcbuf_vptr = ui_v_reg.پڇاڙي;
    bi_چوکٺو_vptr = ipcbuf_vptr + ٻٽو_انگ(صفحو_بٽ);
    extra_bi_چوکٺو_vptr = bi_چوکٺو_vptr + ٻٽو_انگ(se_L4_Boot_Info_چوکٺو_بٽ);

    map_kernel_دري();

    /* initialise the CPU */
    init_cpu();

    لکو("Bootstrapping kernel\n");

    /* initialize the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_خالي_جايون;
    إذا (dtb_سائيز > 0) {
#جيڪڏهن_وصف_ٿيل CONFIG_PLAT_ROCKETCHIP_ZCU102
        /* The softcore rocketchip instantiation doesn't work well when this
         * page isn't reserved. Round up so the whole page is reserved to
         * avoid the problem
         */
        dtb_سائيز = ڦيرو_مٿي(dtb_سائيز, صفحو_بٽ);
#جيڪڏهن_پورو
        paddr_t dtb_phys_پڇاڙي = dtb_phys_addr + dtb_سائيز;
        إذا (dtb_phys_پڇاڙي < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            لکو("ERROR: DTB location at %"SEL4_PRIx_لفظ
                   " len %"SEL4_PRIu_لفظ" invalid\n",
                   dtb_phys_addr, dtb_سائيز);
            أرجع false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        إذا (dtb_phys_پڇاڙي >= PADDR_چوٽي) {
            لکو("ERROR: DTB at [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_لفظ")\n",
                   dtb_phys_addr, dtb_phys_پڇاڙي, PADDR_چوٽي);
            أرجع false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_سائيز += حجم(se_L4_Boot_Info_سرو) + dtb_سائيز;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .شروعات = dtb_phys_addr,
            .پڇاڙي   = dtb_phys_پڇاڙي
        };
    }

    /* The region of the initial thread is the user image + ipcbuf + boot info + extra */
    لفظ_t extra_bi_سائيز_بٽ = calculate_extra_bi_سائيز_بٽ(extra_bi_سائيز);
    v_region_t it_v_reg = {
        .شروعات = ui_v_reg.شروعات,
        .پڇاڙي   = extra_bi_چوکٺو_vptr + (extra_bi_سائيز_بٽ > 0 ? ٻٽو_انگ(extra_bi_سائيز_بٽ) : 0)
    };
    إذا (it_v_reg.پڇاڙي >= USER_چوٽي) {
        /* Variable arguments for printf() require well defined integer types
         * to work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        لکو("ERROR: userland image virt [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ")"
               "exceeds USER_TOP (%"SEL4_PRIx_لفظ")\n",
               it_v_reg.شروعات, it_v_reg.پڇاڙي, (لفظ_t)USER_چوٽي);
        أرجع false;
    }

    /* make the free memory available to alloc_region() */
    إذا (!arch_init_freemem(ui_reg, dtb_p_reg, it_v_reg, extra_bi_سائيز_بٽ)) {
        لکو("ERROR: free memory management initialization failed\n");
        أرجع false;
    }

    /* create the root cnode */
    پاڙ_cnode_cap = create_پاڙ_cnode();
    إذا (cap_get_capType(پاڙ_cnode_cap) == cap_null_cap) {
        لکو("ERROR: root c-node creation failed\n");
        أرجع false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(پاڙ_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(پاڙ_cnode_cap);

    /* create the bootinfo frame */
    populate_bi_چوکٺو(0, CONFIG_MAX_انگ_NODES, ipcbuf_vptr, extra_bi_سائيز);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_سرو سرو;
    إذا (dtb_سائيز > 0) {
        سرو.id = SEL4_BOOTINFO_سرو_FDT;
        سرو.len = حجم(سرو) + dtb_سائيز;
        *(se_L4_Boot_Info_سرو *)(rootserver.extra_bi + extra_bi_وٿي) = سرو;
        extra_bi_وٿي += حجم(سرو);
        يادگيري_نقل((فراغ *)(rootserver.extra_bi + extra_bi_وٿي),
               paddr_منزل_pptr(dtb_phys_addr),
               dtb_سائيز);
        extra_bi_وٿي += dtb_سائيز;
    }

    أكد(extra_bi_سائيز == extra_bi_وٿي);

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_پتو_space(پاڙ_cnode_cap, it_v_reg);
    إذا (cap_get_capType(it_pd_cap) == cap_null_cap) {
        لکو("ERROR: address space creation for initial thread failed\n");
        أرجع false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_چوکٺو_cap(
        پاڙ_cnode_cap,
        it_pd_cap,
        bi_چوکٺو_vptr
    );

    /* create and map extra bootinfo region */
    إذا (extra_bi_سائيز > 0) {
        region_t extra_bi_region = {
            .شروعات = rootserver.extra_bi,
            .پڇاڙي = rootserver.extra_bi + extra_bi_سائيز
        };
        extra_bi_ret =
            create_چوکٺا_جو_region(
                پاڙ_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_منزل_paddr((فراغ *)extra_bi_region.شروعات) - extra_bi_چوکٺو_vptr
            );
        إذا (!extra_bi_ret.success) {
            لکو("ERROR: mapping extra boot info to initial thread failed\n");
            أرجع false;
        }
        ndks_boot.bi_چوکٺو->extraBIPages = extra_bi_ret.region;
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    init_sched_control(پاڙ_cnode_cap, CONFIG_MAX_انگ_NODES);
#جيڪڏهن_پورو

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_چوکٺو_cap(پاڙ_cnode_cap, it_pd_cap, ipcbuf_vptr);
    إذا (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        لکو("ERROR: could not create IPC buffer for initial thread\n");
        أرجع false;
    }

    /* create all userland image frames */
    create_چوکٺا_ret =
        create_چوکٺا_جو_region(
            پاڙ_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_وٿي
        );
    إذا (!create_چوکٺا_ret.success) {
        لکو("ERROR: could not create all userland image frames\n");
        أرجع false;
    }
    ndks_boot.bi_چوکٺو->user_Image_چوکٺا = create_چوکٺا_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(پاڙ_cnode_cap);
    إذا (cap_get_capType(it_ap_cap) == cap_null_cap) {
        لکو("ERROR: could not create ASID pool for initial thread\n");
        أرجع false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    ڳنڍ_STATE(ks_Cur_وقت) = get_Current_وقت();
#جيڪڏهن_پورو

    /* create the idle thread */
    create_idle_thread();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         پاڙ_cnode_cap,
                         it_pd_cap,
                         v_داخلا,
                         bi_چوکٺو_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    إذا (initial == عدم) {
        لکو("ERROR: could not create initial thread\n");
        أرجع false;
    }

    init_core_state(initial);

    /* convert the remaining free memory into UT objects and provide the caps */
    إذا (!create_untypeds(پاڙ_cnode_cap)) {
        لکو("ERROR: could not create untypteds for kernel image boot memory\n");
        أرجع false;
    }

    /* no shared-frame caps (RISC-V has no multikernel support) */
    ndks_boot.bi_چوکٺو->shared_چوکٺا = S_REG_خالي_جايون;

    /* finalise the bootinfo frame */
    bi_finalise();

    ks_انگ_CPUs = 1;

    SMP_COND_STATEMENT(clh_تالو_init());
    SMP_COND_STATEMENT(release_secondary_cores());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    ڳنڍ_تالو_SYS;

    لکو("Booting all finished, dropped to user space\n");
    أرجع true;
}

BOOT_ڪوڊ VISIBLE فراغ init_kernel(
    paddr_t ui_p_reg_شروعات,
    paddr_t ui_p_reg_پڇاڙي,
    sword_t pv_وٿي,
    vptr_t  v_داخلا,
    paddr_t dtb_addr_p,
    uint32_t dtb_سائيز
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    ,
    لفظ_t hart_id,
    لفظ_t core_id
#جيڪڏهن_پورو
)
{
    bool_t نتيجو;

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    جوڙيو_hart_منزل_core_map(hart_id, core_id);
    إذا (core_id == 0) {
        نتيجو = try_init_kernel(ui_p_reg_شروعات,
                                 ui_p_reg_پڇاڙي,
                                 pv_وٿي,
                                 v_داخلا,
                                 dtb_addr_p,
                                 dtb_سائيز);
    } وإلا {
        نتيجو = try_init_kernel_secondary_core(hart_id, core_id);
    }
#وإلا
    نتيجو = try_init_kernel(ui_p_reg_شروعات,
                             ui_p_reg_پڇاڙي,
                             pv_وٿي,
                             v_داخلا,
                             dtb_addr_p,
                             dtb_سائيز);
#جيڪڏهن_پورو
    إذا (!نتيجو) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    ڳنڍ_STATE(ks_Cur_وقت) = get_Current_وقت();
    ڳنڍ_STATE(ksConsumed) = 0;
#جيڪڏهن_پورو

    وقت_جدول();
    activateThread();
}
