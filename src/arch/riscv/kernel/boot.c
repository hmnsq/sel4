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

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS ساكن متطاير _Atomic لفظ_t گرہ_boot_جندرا;
#جے_انت

BOOT_BSS ساكن region_t res_reg[عدد_RESERVED_REGIONS];

BOOT_کوڈ cap_t create_mapped_it_چوکھٹا_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t
                                           use_large, bool_t executable)
{
    cap_t cap;
    vm_ورقہ_ناپ_t چوکھٹا_ناپ_2;

    إذا (use_large) {
        چوکھٹا_ناپ_2 = RISCV_Mega_ورقہ;
    } وإلا {
        چوکھٹا_ناپ_2 = RISCV_4_K_ورقہ;
    }

    cap = cap_چوکھٹا_cap_new(
              asid,                            /* capFMappedASID       */
              pptr,                            /* capFBasePtr          */
              چوکھٹا_ناپ_2,                      /* capFSize             */
              لفظ_From_VMRights(VMReadWrite),   /* capFVMRights         */
              0,                               /* capFIsDevice         */
              vptr                             /* capFMappedAddress    */
          );

    map_it_چوکھٹا_cap(pd_cap, cap);
    أرجع cap;
}

BOOT_کوڈ ساكن bool_t arch_init_freemem(region_t ui_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          لفظ_t extra_bi_ناپ_بٹ)
{
    /* Reserve the kernel image region. This may look a bit awkward, as the
     * symbols are a reference in the kernel image window, but all allocations
     * are done in terms of the main kernel window, so we do some translation.
     */
    res_reg[0] = paddr_منزل_pptr_reg(get_p_reg_kernel_img());
    صحيح اشاریہ = 1;

    /* add the dtb region, if it is not empty */
    إذا (dtb_p_reg.شروع) {
        إذا (اشاریہ >= ARRAY_ناپ(res_reg)) {
            لکھو("ERROR: no slot to add DTB to reserved regions\n");
            أرجع false;
        }
        res_reg[اشاریہ] = paddr_منزل_pptr_reg(dtb_p_reg);
        اشاریہ += 1;
    }

    /* reserve the user image region */
    إذا (اشاریہ >= ARRAY_ناپ(res_reg)) {
        لکھو("ERROR: no slot to add user image to reserved regions\n");
        أرجع false;
    }
    res_reg[اشاریہ] = ui_reg;
    اشاریہ += 1;

    /* avail_p_regs comes from the auto-generated code */
    أرجع init_freemem(ARRAY_ناپ(avail_p_regs), avail_p_regs,
                        اشاریہ, res_reg,
                        it_v_reg, extra_bi_ناپ_بٹ);
}

BOOT_کوڈ ساكن فراغ init_irqs(cap_t جڑ_cnode_cap)
{
    irq_t i;

    لكل (i = 0; i <= maxIRQ; i++) {
        إذا (i != irqInvalid) {
            /* IRQ 0 is irqInvalid */
            سیٹ_IRQState(IRQInactive, i);
        }
    }
    سیٹ_IRQState(IRQTimer, KERNEL_TIMER_IRQ);
#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    سیٹ_IRQState(IRQIPI, irq_remote_call_ipi);
    سیٹ_IRQState(IRQIPI, irq_reschedule_ipi);
#جے_انت
    /* provide the IRQ control cap */
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

/* This and only this function initialises the CPU. It does NOT initialise any kernel state. */

#جے_تعریف_شدہ CONFIG_HAVE_FPU
BOOT_کوڈ ساكن فراغ init_fpu(فراغ)
{
    سیٹ_fs_clean();
    write_fcsr(0);
    disableFpu();
}
#جے_انت

BOOT_کوڈ ساكن فراغ init_cpu(فراغ)
{

    activate_kernel_vspace();
    /* Write trap entry address to stvec */
    write_stvec((لفظ_t)trap_اندراج);
    initLocalIRQController();
#جے_بنا_تعریف CONFIG_KERNEL_MCS
    initTimer();
#جے_انت

    /* disable FPU access */
    سیٹ_fs_off();

#جے_تعریف_شدہ CONFIG_HAVE_FPU
    init_fpu();
#جے_انت
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_کوڈ ساكن فراغ init_plat(فراغ)
{
    initIRQController();
}


#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
BOOT_کوڈ ساكن bool_t try_init_kernel_secondary_core(لفظ_t hart_id, لفظ_t core_id)
{
    طالما (!گرہ_boot_جندرا);

    fence_r_rw();

    init_cpu();
    گرہ_جندرا_SYS;

    clock_sync_test();
    ks_عدد_CPUs++;
    init_core_state(SchedulerAction_ResumeCurrentThread);
    ifence_local();
    أرجع true;
}

BOOT_کوڈ ساكن فراغ release_secondary_cores(فراغ)
{
    أكد(0 == گرہ_boot_جندرا); /* Sanity check for a proper lock state. */
    گرہ_boot_جندرا = 1;
    /* At this point in time the primary core (executing this code) already uses
     * the seL4 MMU/cache setup. However, the secondary cores are still using
     * the elfloader's MMU/cache setup, and thus the update of node_boot_lock
     * may not be visible there if the setups differ. Currently, the mappings
     * match, so a barrier is all that is needed.
     */
    fence_rw_rw();

    طالما (ks_عدد_CPUs != CONFIG_MAX_عدد_NODES) {
#جے_تعریف_شدہ ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        گرہ_STATE(ks_Cur_ویلا) = get_Current_ویلا();
#جے_انت
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#جے_انت /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

ساكن BOOT_کوڈ bool_t try_init_kernel(
    paddr_t ui_p_reg_شروع,
    paddr_t ui_p_reg_سرا,
    sword_t pv_وتھ,
    vptr_t  v_اندراج,
    paddr_t dtb_phys_addr,
    لفظ_t  dtb_ناپ
)
{
    cap_t جڑ_cnode_cap;
    cap_t it_pd_cap;
    cap_t it_ap_cap;
    cap_t ipcbuf_cap;
    region_t ui_reg = paddr_منزل_pptr_reg((p_region_t) {
        ui_p_reg_شروع, ui_p_reg_سرا
    });
    لفظ_t extra_bi_ناپ = 0;
    pptr_t extra_bi_وتھ = 0;
    vptr_t extra_bi_چوکھٹا_vptr;
    vptr_t bi_چوکھٹا_vptr;
    vptr_t ipcbuf_vptr;
    create_چوکھٹے_دا_region_ret_t create_چوکھٹے_ret;
    create_چوکھٹے_دا_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .شروع = ui_p_reg_شروع - pv_وتھ,
        .سرا   = ui_p_reg_سرا   - pv_وتھ
    };

    ipcbuf_vptr = ui_v_reg.سرا;
    bi_چوکھٹا_vptr = ipcbuf_vptr + ثنائی_ہندسہ(ورقہ_بٹ);
    extra_bi_چوکھٹا_vptr = bi_چوکھٹا_vptr + ثنائی_ہندسہ(se_L4_Boot_Info_چوکھٹا_بٹ);

    map_kernel_کھڑکی();

    /* initialise the CPU */
    init_cpu();

    لکھو("Bootstrapping kernel\n");

    /* initialize the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_خالی_تھاواں;
    إذا (dtb_ناپ > 0) {
#جے_تعریف_شدہ CONFIG_PLAT_ROCKETCHIP_ZCU102
        /* The softcore rocketchip instantiation doesn't work well when this
         * page isn't reserved. Round up so the whole page is reserved to
         * avoid the problem
         */
        dtb_ناپ = گیڑا_اتے(dtb_ناپ, ورقہ_بٹ);
#جے_انت
        paddr_t dtb_phys_سرا = dtb_phys_addr + dtb_ناپ;
        إذا (dtb_phys_سرا < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            لکھو("ERROR: DTB location at %"SEL4_PRIx_لفظ
                   " len %"SEL4_PRIu_لفظ" invalid\n",
                   dtb_phys_addr, dtb_ناپ);
            أرجع false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        إذا (dtb_phys_سرا >= PADDR_چوٹی) {
            لکھو("ERROR: DTB at [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_لفظ")\n",
                   dtb_phys_addr, dtb_phys_سرا, PADDR_چوٹی);
            أرجع false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_ناپ += حجم(se_L4_Boot_Info_سرخی) + dtb_ناپ;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .شروع = dtb_phys_addr,
            .سرا   = dtb_phys_سرا
        };
    }

    /* The region of the initial thread is the user image + ipcbuf + boot info + extra */
    لفظ_t extra_bi_ناپ_بٹ = calculate_extra_bi_ناپ_بٹ(extra_bi_ناپ);
    v_region_t it_v_reg = {
        .شروع = ui_v_reg.شروع,
        .سرا   = extra_bi_چوکھٹا_vptr + (extra_bi_ناپ_بٹ > 0 ? ثنائی_ہندسہ(extra_bi_ناپ_بٹ) : 0)
    };
    إذا (it_v_reg.سرا >= USER_چوٹی) {
        /* Variable arguments for printf() require well defined integer types
         * to work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        لکھو("ERROR: userland image virt [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ")"
               "exceeds USER_TOP (%"SEL4_PRIx_لفظ")\n",
               it_v_reg.شروع, it_v_reg.سرا, (لفظ_t)USER_چوٹی);
        أرجع false;
    }

    /* make the free memory available to alloc_region() */
    إذا (!arch_init_freemem(ui_reg, dtb_p_reg, it_v_reg, extra_bi_ناپ_بٹ)) {
        لکھو("ERROR: free memory management initialization failed\n");
        أرجع false;
    }

    /* create the root cnode */
    جڑ_cnode_cap = create_جڑ_cnode();
    إذا (cap_get_capType(جڑ_cnode_cap) == cap_null_cap) {
        لکھو("ERROR: root c-node creation failed\n");
        أرجع false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(جڑ_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(جڑ_cnode_cap);

    /* create the bootinfo frame */
    populate_bi_چوکھٹا(0, CONFIG_MAX_عدد_NODES, ipcbuf_vptr, extra_bi_ناپ);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_سرخی سرخی;
    إذا (dtb_ناپ > 0) {
        سرخی.id = SEL4_BOOTINFO_سرخی_FDT;
        سرخی.len = حجم(سرخی) + dtb_ناپ;
        *(se_L4_Boot_Info_سرخی *)(rootserver.extra_bi + extra_bi_وتھ) = سرخی;
        extra_bi_وتھ += حجم(سرخی);
        حافظہ_نقل((فراغ *)(rootserver.extra_bi + extra_bi_وتھ),
               paddr_منزل_pptr(dtb_phys_addr),
               dtb_ناپ);
        extra_bi_وتھ += dtb_ناپ;
    }

    أكد(extra_bi_ناپ == extra_bi_وتھ);

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_پتہ_space(جڑ_cnode_cap, it_v_reg);
    إذا (cap_get_capType(it_pd_cap) == cap_null_cap) {
        لکھو("ERROR: address space creation for initial thread failed\n");
        أرجع false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_چوکھٹا_cap(
        جڑ_cnode_cap,
        it_pd_cap,
        bi_چوکھٹا_vptr
    );

    /* create and map extra bootinfo region */
    إذا (extra_bi_ناپ > 0) {
        region_t extra_bi_region = {
            .شروع = rootserver.extra_bi,
            .سرا = rootserver.extra_bi + extra_bi_ناپ
        };
        extra_bi_ret =
            create_چوکھٹے_دا_region(
                جڑ_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_منزل_paddr((فراغ *)extra_bi_region.شروع) - extra_bi_چوکھٹا_vptr
            );
        إذا (!extra_bi_ret.success) {
            لکھو("ERROR: mapping extra boot info to initial thread failed\n");
            أرجع false;
        }
        ndks_boot.bi_چوکھٹا->extraBIPages = extra_bi_ret.region;
    }

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    init_sched_control(جڑ_cnode_cap, CONFIG_MAX_عدد_NODES);
#جے_انت

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_چوکھٹا_cap(جڑ_cnode_cap, it_pd_cap, ipcbuf_vptr);
    إذا (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        لکھو("ERROR: could not create IPC buffer for initial thread\n");
        أرجع false;
    }

    /* create all userland image frames */
    create_چوکھٹے_ret =
        create_چوکھٹے_دا_region(
            جڑ_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_وتھ
        );
    إذا (!create_چوکھٹے_ret.success) {
        لکھو("ERROR: could not create all userland image frames\n");
        أرجع false;
    }
    ndks_boot.bi_چوکھٹا->user_Image_چوکھٹے = create_چوکھٹے_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(جڑ_cnode_cap);
    إذا (cap_get_capType(it_ap_cap) == cap_null_cap) {
        لکھو("ERROR: could not create ASID pool for initial thread\n");
        أرجع false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    گرہ_STATE(ks_Cur_ویلا) = get_Current_ویلا();
#جے_انت

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

    إذا (initial == عدم) {
        لکھو("ERROR: could not create initial thread\n");
        أرجع false;
    }

    init_core_state(initial);

    /* convert the remaining free memory into UT objects and provide the caps */
    إذا (!create_untypeds(جڑ_cnode_cap)) {
        لکھو("ERROR: could not create untypteds for kernel image boot memory\n");
        أرجع false;
    }

    /* no shared-frame caps (RISC-V has no multikernel support) */
    ndks_boot.bi_چوکھٹا->shared_چوکھٹے = S_REG_خالی_تھاواں;

    /* finalise the bootinfo frame */
    bi_finalise();

    ks_عدد_CPUs = 1;

    SMP_COND_STATEMENT(clh_جندرا_init());
    SMP_COND_STATEMENT(release_secondary_cores());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    گرہ_جندرا_SYS;

    لکھو("Booting all finished, dropped to user space\n");
    أرجع true;
}

BOOT_کوڈ VISIBLE فراغ init_kernel(
    paddr_t ui_p_reg_شروع,
    paddr_t ui_p_reg_سرا,
    sword_t pv_وتھ,
    vptr_t  v_اندراج,
    paddr_t dtb_addr_p,
    uint32_t dtb_ناپ
#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    ,
    لفظ_t hart_id,
    لفظ_t core_id
#جے_انت
)
{
    bool_t نتیجہ;

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    جوڑو_hart_منزل_core_map(hart_id, core_id);
    إذا (core_id == 0) {
        نتیجہ = try_init_kernel(ui_p_reg_شروع,
                                 ui_p_reg_سرا,
                                 pv_وتھ,
                                 v_اندراج,
                                 dtb_addr_p,
                                 dtb_ناپ);
    } وإلا {
        نتیجہ = try_init_kernel_secondary_core(hart_id, core_id);
    }
#وإلا
    نتیجہ = try_init_kernel(ui_p_reg_شروع,
                             ui_p_reg_سرا,
                             pv_وتھ,
                             v_اندراج,
                             dtb_addr_p,
                             dtb_ناپ);
#جے_انت
    إذا (!نتیجہ) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    گرہ_STATE(ks_Cur_ویلا) = get_Current_ویلا();
    گرہ_STATE(ksConsumed) = 0;
#جے_انت

    ویلا_سارنی();
    activateThread();
}
