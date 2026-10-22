/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <assert.h>
#شامل <kernel/boot.h>
#شامل <machine/io.h>
#شامل <model/statedata.h>
#شامل <object/interrupt.h>
#شامل <arch/machine.h>
#شامل <arch/kernel/boot.h>
#شامل <arch/kernel/traps.h>
#شامل <arch/kernel/vspace.h>
#شامل <arch/benchmark.h>
#شامل <linker.h>
#شامل <plat/machine/hardware.h>
#شامل <machine.h>

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS ساکن متلون _Atomic لفظ_t گرہ_boot_تالا;
#اگر_ختم

BOOT_BSS ساکن region_t res_reg[عدد_RESERVED_REGIONS];

BOOT_رمز cap_t create_mapped_it_چوکھٹا_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t
                                           use_large, bool_t executable)
{
    cap_t cap;
    vm_صفحہ_سائز_t چوکھٹا_سائز_2;

    اگر (use_large) {
        چوکھٹا_سائز_2 = RISCV_Mega_صفحہ;
    } ورنہ {
        چوکھٹا_سائز_2 = RISCV_4_K_صفحہ;
    }

    cap = cap_چوکھٹا_cap_new(
              asid,                            /* capFMappedASID       */
              pptr,                            /* capFBasePtr          */
              چوکھٹا_سائز_2,                      /* capFSize             */
              لفظ_From_VMRights(VMReadWrite),   /* capFVMRights         */
              0,                               /* capFIsDevice         */
              vptr                             /* capFMappedAddress    */
          );

    map_it_چوکھٹا_cap(pd_cap, cap);
    واپس cap;
}

BOOT_رمز ساکن bool_t arch_init_freemem(region_t ui_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          لفظ_t extra_bi_سائز_بٹ)
{
    /* Reserve the kernel image region. This may look a bit awkward, as the
     * symbols are a reference in the kernel image window, but all allocations
     * are done in terms of the main kernel window, so we do some translation.
     */
    res_reg[0] = paddr_مقصد_pptr_reg(get_p_reg_kernel_img());
    صحیح اشاریہ = 1;

    /* add the dtb region, if it is not empty */
    اگر (dtb_p_reg.آغاز) {
        اگر (اشاریہ >= ARRAY_سائز(res_reg)) {
            لکھو("ERROR: no slot to add DTB to reserved regions\n");
            واپس false;
        }
        res_reg[اشاریہ] = paddr_مقصد_pptr_reg(dtb_p_reg);
        اشاریہ += 1;
    }

    /* reserve the user image region */
    اگر (اشاریہ >= ARRAY_سائز(res_reg)) {
        لکھو("ERROR: no slot to add user image to reserved regions\n");
        واپس false;
    }
    res_reg[اشاریہ] = ui_reg;
    اشاریہ += 1;

    /* avail_p_regs comes from the auto-generated code */
    واپس init_freemem(ARRAY_سائز(avail_p_regs), avail_p_regs,
                        اشاریہ, res_reg,
                        it_v_reg, extra_bi_سائز_بٹ);
}

BOOT_رمز ساکن خالی init_irqs(cap_t جڑ_cnode_cap)
{
    irq_t i;

    برائے (i = 0; i <= maxIRQ; i++) {
        اگر (i != irqInvalid) {
            /* IRQ 0 is irqInvalid */
            سیٹ_IRQState(IRQInactive, i);
        }
    }
    سیٹ_IRQState(IRQTimer, KERNEL_TIMER_IRQ);
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    سیٹ_IRQState(IRQIPI, irq_remote_call_ipi);
    سیٹ_IRQState(IRQIPI, irq_reschedule_ipi);
#اگر_ختم
    /* provide the IRQ control cap */
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

/* This and only this function initialises the CPU. It does NOT initialise any kernel state. */

#اگر_تعریف_شدہ CONFIG_HAVE_FPU
BOOT_رمز ساکن خالی init_fpu(خالی)
{
    سیٹ_fs_clean();
    write_fcsr(0);
    disableFpu();
}
#اگر_ختم

BOOT_رمز ساکن خالی init_cpu(خالی)
{

    activate_kernel_vspace();
    /* Write trap entry address to stvec */
    write_stvec((لفظ_t)trap_اندراج);
    initLocalIRQController();
#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
    initTimer();
#اگر_ختم

    /* disable FPU access */
    سیٹ_fs_off();

#اگر_تعریف_شدہ CONFIG_HAVE_FPU
    init_fpu();
#اگر_ختم
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_رمز ساکن خالی init_plat(خالی)
{
    initIRQController();
}


#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
BOOT_رمز ساکن bool_t try_init_kernel_secondary_core(لفظ_t hart_id, لفظ_t core_id)
{
    جب (!گرہ_boot_تالا);

    fence_r_rw();

    init_cpu();
    گرہ_تالا_SYS;

    clock_sync_test();
    ks_عدد_CPUs++;
    init_core_state(SchedulerAction_ResumeCurrentThread);
    ifence_local();
    واپس true;
}

BOOT_رمز ساکن خالی release_secondary_cores(خالی)
{
    أكد(0 == گرہ_boot_تالا); /* Sanity check for a proper lock state. */
    گرہ_boot_تالا = 1;
    /* At this point in time the primary core (executing this code) already uses
     * the seL4 MMU/cache setup. However, the secondary cores are still using
     * the elfloader's MMU/cache setup, and thus the update of node_boot_lock
     * may not be visible there if the setups differ. Currently, the mappings
     * match, so a barrier is all that is needed.
     */
    fence_rw_rw();

    جب (ks_عدد_CPUs != CONFIG_MAX_عدد_NODES) {
#اگر_تعریف_شدہ ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        گرہ_STATE(ks_Cur_وقت) = get_Current_وقت();
#اگر_ختم
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#اگر_ختم /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

ساکن BOOT_رمز bool_t try_init_kernel(
    paddr_t ui_p_reg_آغاز,
    paddr_t ui_p_reg_انتہا,
    sword_t pv_انحراف,
    vptr_t  v_اندراج,
    paddr_t dtb_phys_addr,
    لفظ_t  dtb_سائز
)
{
    cap_t جڑ_cnode_cap;
    cap_t it_pd_cap;
    cap_t it_ap_cap;
    cap_t ipcbuf_cap;
    region_t ui_reg = paddr_مقصد_pptr_reg((p_region_t) {
        ui_p_reg_آغاز, ui_p_reg_انتہا
    });
    لفظ_t extra_bi_سائز = 0;
    pptr_t extra_bi_انحراف = 0;
    vptr_t extra_bi_چوکھٹا_vptr;
    vptr_t bi_چوکھٹا_vptr;
    vptr_t ipcbuf_vptr;
    create_چوکھٹے_کا_region_ret_t create_چوکھٹے_ret;
    create_چوکھٹے_کا_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .آغاز = ui_p_reg_آغاز - pv_انحراف,
        .انتہا   = ui_p_reg_انتہا   - pv_انحراف
    };

    ipcbuf_vptr = ui_v_reg.انتہا;
    bi_چوکھٹا_vptr = ipcbuf_vptr + ثنائی_ہندسہ(صفحہ_بٹ);
    extra_bi_چوکھٹا_vptr = bi_چوکھٹا_vptr + ثنائی_ہندسہ(se_L4_Boot_Info_چوکھٹا_بٹ);

    map_kernel_کھڑکی();

    /* initialise the CPU */
    init_cpu();

    لکھو("Bootstrapping kernel\n");

    /* initialize the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_خالی_خانے;
    اگر (dtb_سائز > 0) {
#اگر_تعریف_شدہ CONFIG_PLAT_ROCKETCHIP_ZCU102
        /* The softcore rocketchip instantiation doesn't work well when this
         * page isn't reserved. Round up so the whole page is reserved to
         * avoid the problem
         */
        dtb_سائز = باری_اوپر(dtb_سائز, صفحہ_بٹ);
#اگر_ختم
        paddr_t dtb_phys_انتہا = dtb_phys_addr + dtb_سائز;
        اگر (dtb_phys_انتہا < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            لکھو("ERROR: DTB location at %"SEL4_PRIx_لفظ
                   " len %"SEL4_PRIu_لفظ" invalid\n",
                   dtb_phys_addr, dtb_سائز);
            واپس false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        اگر (dtb_phys_انتہا >= PADDR_چوٹی) {
            لکھو("ERROR: DTB at [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_لفظ")\n",
                   dtb_phys_addr, dtb_phys_انتہا, PADDR_چوٹی);
            واپس false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_سائز += حجم(se_L4_Boot_Info_سرخی) + dtb_سائز;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .آغاز = dtb_phys_addr,
            .انتہا   = dtb_phys_انتہا
        };
    }

    /* The region of the initial thread is the user image + ipcbuf + boot info + extra */
    لفظ_t extra_bi_سائز_بٹ = calculate_extra_bi_سائز_بٹ(extra_bi_سائز);
    v_region_t it_v_reg = {
        .آغاز = ui_v_reg.آغاز,
        .انتہا   = extra_bi_چوکھٹا_vptr + (extra_bi_سائز_بٹ > 0 ? ثنائی_ہندسہ(extra_bi_سائز_بٹ) : 0)
    };
    اگر (it_v_reg.انتہا >= USER_چوٹی) {
        /* Variable arguments for printf() require well defined integer types
         * to work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        لکھو("ERROR: userland image virt [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ")"
               "exceeds USER_TOP (%"SEL4_PRIx_لفظ")\n",
               it_v_reg.آغاز, it_v_reg.انتہا, (لفظ_t)USER_چوٹی);
        واپس false;
    }

    /* make the free memory available to alloc_region() */
    اگر (!arch_init_freemem(ui_reg, dtb_p_reg, it_v_reg, extra_bi_سائز_بٹ)) {
        لکھو("ERROR: free memory management initialization failed\n");
        واپس false;
    }

    /* create the root cnode */
    جڑ_cnode_cap = create_جڑ_cnode();
    اگر (cap_get_capType(جڑ_cnode_cap) == cap_null_cap) {
        لکھو("ERROR: root c-node creation failed\n");
        واپس false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(جڑ_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(جڑ_cnode_cap);

    /* create the bootinfo frame */
    populate_bi_چوکھٹا(0, CONFIG_MAX_عدد_NODES, ipcbuf_vptr, extra_bi_سائز);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_سرخی سرخی;
    اگر (dtb_سائز > 0) {
        سرخی.id = SEL4_BOOTINFO_سرخی_FDT;
        سرخی.len = حجم(سرخی) + dtb_سائز;
        *(se_L4_Boot_Info_سرخی *)(rootserver.extra_bi + extra_bi_انحراف) = سرخی;
        extra_bi_انحراف += حجم(سرخی);
        حافظہ_نقل((خالی *)(rootserver.extra_bi + extra_bi_انحراف),
               paddr_مقصد_pptr(dtb_phys_addr),
               dtb_سائز);
        extra_bi_انحراف += dtb_سائز;
    }

    أكد(extra_bi_سائز == extra_bi_انحراف);

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_پتہ_space(جڑ_cnode_cap, it_v_reg);
    اگر (cap_get_capType(it_pd_cap) == cap_null_cap) {
        لکھو("ERROR: address space creation for initial thread failed\n");
        واپس false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_چوکھٹا_cap(
        جڑ_cnode_cap,
        it_pd_cap,
        bi_چوکھٹا_vptr
    );

    /* create and map extra bootinfo region */
    اگر (extra_bi_سائز > 0) {
        region_t extra_bi_region = {
            .آغاز = rootserver.extra_bi,
            .انتہا = rootserver.extra_bi + extra_bi_سائز
        };
        extra_bi_ret =
            create_چوکھٹے_کا_region(
                جڑ_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_مقصد_paddr((خالی *)extra_bi_region.آغاز) - extra_bi_چوکھٹا_vptr
            );
        اگر (!extra_bi_ret.success) {
            لکھو("ERROR: mapping extra boot info to initial thread failed\n");
            واپس false;
        }
        ndks_boot.bi_چوکھٹا->extraBIPages = extra_bi_ret.region;
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    init_sched_control(جڑ_cnode_cap, CONFIG_MAX_عدد_NODES);
#اگر_ختم

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_چوکھٹا_cap(جڑ_cnode_cap, it_pd_cap, ipcbuf_vptr);
    اگر (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        لکھو("ERROR: could not create IPC buffer for initial thread\n");
        واپس false;
    }

    /* create all userland image frames */
    create_چوکھٹے_ret =
        create_چوکھٹے_کا_region(
            جڑ_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_انحراف
        );
    اگر (!create_چوکھٹے_ret.success) {
        لکھو("ERROR: could not create all userland image frames\n");
        واپس false;
    }
    ndks_boot.bi_چوکھٹا->user_Image_چوکھٹے = create_چوکھٹے_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(جڑ_cnode_cap);
    اگر (cap_get_capType(it_ap_cap) == cap_null_cap) {
        لکھو("ERROR: could not create ASID pool for initial thread\n");
        واپس false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    گرہ_STATE(ks_Cur_وقت) = get_Current_وقت();
#اگر_ختم

    /* create the idle thread */
    create_idle_thread();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         جڑ_cnode_cap,
                         it_pd_cap,
                         v_اندراج,
                         bi_چوکھٹا_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    اگر (initial == تہی) {
        لکھو("ERROR: could not create initial thread\n");
        واپس false;
    }

    init_core_state(initial);

    /* convert the remaining free memory into UT objects and provide the caps */
    اگر (!create_untypeds(جڑ_cnode_cap)) {
        لکھو("ERROR: could not create untypteds for kernel image boot memory\n");
        واپس false;
    }

    /* no shared-frame caps (RISC-V has no multikernel support) */
    ndks_boot.bi_چوکھٹا->shared_چوکھٹے = S_REG_خالی_خانے;

    /* finalise the bootinfo frame */
    bi_finalise();

    ks_عدد_CPUs = 1;

    SMP_COND_STATEMENT(clh_تالا_init());
    SMP_COND_STATEMENT(release_secondary_cores());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    گرہ_تالا_SYS;

    لکھو("Booting all finished, dropped to user space\n");
    واپس true;
}

BOOT_رمز VISIBLE خالی init_kernel(
    paddr_t ui_p_reg_آغاز,
    paddr_t ui_p_reg_انتہا,
    sword_t pv_انحراف,
    vptr_t  v_اندراج,
    paddr_t dtb_addr_p,
    uint32_t dtb_سائز
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    ,
    لفظ_t hart_id,
    لفظ_t core_id
#اگر_ختم
)
{
    bool_t نتیجہ;

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    جوڑو_hart_مقصد_core_map(hart_id, core_id);
    اگر (core_id == 0) {
        نتیجہ = try_init_kernel(ui_p_reg_آغاز,
                                 ui_p_reg_انتہا,
                                 pv_انحراف,
                                 v_اندراج,
                                 dtb_addr_p,
                                 dtb_سائز);
    } ورنہ {
        نتیجہ = try_init_kernel_secondary_core(hart_id, core_id);
    }
#ورنہ
    نتیجہ = try_init_kernel(ui_p_reg_آغاز,
                             ui_p_reg_انتہا,
                             pv_انحراف,
                             v_اندراج,
                             dtb_addr_p,
                             dtb_سائز);
#اگر_ختم
    اگر (!نتیجہ) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    گرہ_STATE(ks_Cur_وقت) = get_Current_وقت();
    گرہ_STATE(ksConsumed) = 0;
#اگر_ختم

    جدول_وقت();
    activateThread();
}
