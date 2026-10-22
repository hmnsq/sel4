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

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS ساكن متطاير _Atomic کلمه_t گره_boot_قفل;
#پایان_اگر

BOOT_BSS ساكن region_t res_reg[رقم_RESERVED_REGIONS];

BOOT_کد cap_t create_mapped_it_قاب_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t
                                           use_large, bool_t executable)
{
    cap_t cap;
    vm_صفحه_اندازه_t قاب_اندازه_2;

    إذا (use_large) {
        قاب_اندازه_2 = RISCV_Mega_صفحه;
    } وإلا {
        قاب_اندازه_2 = RISCV_4_K_صفحه;
    }

    cap = cap_قاب_cap_new(
              asid,                            /* capFMappedASID       */
              pptr,                            /* capFBasePtr          */
              قاب_اندازه_2,                      /* capFSize             */
              کلمه_From_VMRights(VMReadWrite),   /* capFVMRights         */
              0,                               /* capFIsDevice         */
              vptr                             /* capFMappedAddress    */
          );

    map_it_قاب_cap(pd_cap, cap);
    أرجع cap;
}

BOOT_کد ساكن bool_t arch_init_freemem(region_t ui_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          کلمه_t extra_bi_اندازه_بیتها)
{
    /* Reserve the kernel image region. This may look a bit awkward, as the
     * symbols are a reference in the kernel image window, but all allocations
     * are done in terms of the main kernel window, so we do some translation.
     */
    res_reg[0] = paddr_مقصد_pptr_reg(get_p_reg_kernel_img());
    صحيح شاخص = 1;

    /* add the dtb region, if it is not empty */
    إذا (dtb_p_reg.شروع) {
        إذا (شاخص >= ARRAY_اندازه(res_reg)) {
            چاپ_کن("ERROR: no slot to add DTB to reserved regions\n");
            أرجع false;
        }
        res_reg[شاخص] = paddr_مقصد_pptr_reg(dtb_p_reg);
        شاخص += 1;
    }

    /* reserve the user image region */
    إذا (شاخص >= ARRAY_اندازه(res_reg)) {
        چاپ_کن("ERROR: no slot to add user image to reserved regions\n");
        أرجع false;
    }
    res_reg[شاخص] = ui_reg;
    شاخص += 1;

    /* avail_p_regs comes from the auto-generated code */
    أرجع init_freemem(ARRAY_اندازه(avail_p_regs), avail_p_regs,
                        شاخص, res_reg,
                        it_v_reg, extra_bi_اندازه_بیتها);
}

BOOT_کد ساكن فراغ init_irqs(cap_t ریشه_cnode_cap)
{
    irq_t i;

    لكل (i = 0; i <= maxIRQ; i++) {
        إذا (i != irqInvalid) {
            /* IRQ 0 is irqInvalid */
            مجموعه_IRQState(IRQInactive, i);
        }
    }
    مجموعه_IRQState(IRQTimer, KERNEL_TIMER_IRQ);
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    مجموعه_IRQState(IRQIPI, irq_remote_call_ipi);
    مجموعه_IRQState(IRQIPI, irq_reschedule_ipi);
#پایان_اگر
    /* provide the IRQ control cap */
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

/* This and only this function initialises the CPU. It does NOT initialise any kernel state. */

#اگر_تعریف_شده CONFIG_HAVE_FPU
BOOT_کد ساكن فراغ init_fpu(فراغ)
{
    مجموعه_fs_clean();
    write_fcsr(0);
    disableFpu();
}
#پایان_اگر

BOOT_کد ساكن فراغ init_cpu(فراغ)
{

    activate_kernel_vspace();
    /* Write trap entry address to stvec */
    write_stvec((کلمه_t)trap_مدخل);
    initLocalIRQController();
#اگر_تعریف_نشده CONFIG_KERNEL_MCS
    initTimer();
#پایان_اگر

    /* disable FPU access */
    مجموعه_fs_off();

#اگر_تعریف_شده CONFIG_HAVE_FPU
    init_fpu();
#پایان_اگر
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_کد ساكن فراغ init_plat(فراغ)
{
    initIRQController();
}


#اگر_تعریف_شده ENABLE_SMP_SUPPORT
BOOT_کد ساكن bool_t try_init_kernel_secondary_core(کلمه_t hart_id, کلمه_t core_id)
{
    طالما (!گره_boot_قفل);

    fence_r_rw();

    init_cpu();
    گره_قفل_SYS;

    clock_sync_test();
    ks_رقم_CPUs++;
    init_core_state(SchedulerAction_ResumeCurrentThread);
    ifence_local();
    أرجع true;
}

BOOT_کد ساكن فراغ release_secondary_cores(فراغ)
{
    أكد(0 == گره_boot_قفل); /* Sanity check for a proper lock state. */
    گره_boot_قفل = 1;
    /* At this point in time the primary core (executing this code) already uses
     * the seL4 MMU/cache setup. However, the secondary cores are still using
     * the elfloader's MMU/cache setup, and thus the update of node_boot_lock
     * may not be visible there if the setups differ. Currently, the mappings
     * match, so a barrier is all that is needed.
     */
    fence_rw_rw();

    طالما (ks_رقم_CPUs != CONFIG_MAX_رقم_NODES) {
#اگر_تعریف_شده ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        گره_STATE(ks_Cur_زمان) = get_Current_زمان();
#پایان_اگر
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#پایان_اگر /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

ساكن BOOT_کد bool_t try_init_kernel(
    paddr_t ui_p_reg_شروع,
    paddr_t ui_p_reg_انتها,
    sword_t pv_جابه_جایی,
    vptr_t  v_مدخل,
    paddr_t dtb_phys_addr,
    کلمه_t  dtb_اندازه
)
{
    cap_t ریشه_cnode_cap;
    cap_t it_pd_cap;
    cap_t it_ap_cap;
    cap_t ipcbuf_cap;
    region_t ui_reg = paddr_مقصد_pptr_reg((p_region_t) {
        ui_p_reg_شروع, ui_p_reg_انتها
    });
    کلمه_t extra_bi_اندازه = 0;
    pptr_t extra_bi_جابه_جایی = 0;
    vptr_t extra_bi_قاب_vptr;
    vptr_t bi_قاب_vptr;
    vptr_t ipcbuf_vptr;
    create_قابها_از_region_ret_t create_قابها_ret;
    create_قابها_از_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .شروع = ui_p_reg_شروع - pv_جابه_جایی,
        .انتها   = ui_p_reg_انتها   - pv_جابه_جایی
    };

    ipcbuf_vptr = ui_v_reg.انتها;
    bi_قاب_vptr = ipcbuf_vptr + بیت(صفحه_بیتها);
    extra_bi_قاب_vptr = bi_قاب_vptr + بیت(se_L4_Boot_Info_قاب_بیتها);

    map_kernel_پنجره();

    /* initialise the CPU */
    init_cpu();

    چاپ_کن("Bootstrapping kernel\n");

    /* initialize the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_خالی;
    إذا (dtb_اندازه > 0) {
#اگر_تعریف_شده CONFIG_PLAT_ROCKETCHIP_ZCU102
        /* The softcore rocketchip instantiation doesn't work well when this
         * page isn't reserved. Round up so the whole page is reserved to
         * avoid the problem
         */
        dtb_اندازه = دور_به_بالا(dtb_اندازه, صفحه_بیتها);
#پایان_اگر
        paddr_t dtb_phys_انتها = dtb_phys_addr + dtb_اندازه;
        إذا (dtb_phys_انتها < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            چاپ_کن("ERROR: DTB location at %"SEL4_PRIx_کلمه
                   " len %"SEL4_PRIu_کلمه" invalid\n",
                   dtb_phys_addr, dtb_اندازه);
            أرجع false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        إذا (dtb_phys_انتها >= PADDR_بالاترین) {
            چاپ_کن("ERROR: DTB at [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_کلمه")\n",
                   dtb_phys_addr, dtb_phys_انتها, PADDR_بالاترین);
            أرجع false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_اندازه += حجم(se_L4_Boot_Info_سرآیند) + dtb_اندازه;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .شروع = dtb_phys_addr,
            .انتها   = dtb_phys_انتها
        };
    }

    /* The region of the initial thread is the user image + ipcbuf + boot info + extra */
    کلمه_t extra_bi_اندازه_بیتها = calculate_extra_bi_اندازه_بیتها(extra_bi_اندازه);
    v_region_t it_v_reg = {
        .شروع = ui_v_reg.شروع,
        .انتها   = extra_bi_قاب_vptr + (extra_bi_اندازه_بیتها > 0 ? بیت(extra_bi_اندازه_بیتها) : 0)
    };
    إذا (it_v_reg.انتها >= USER_بالاترین) {
        /* Variable arguments for printf() require well defined integer types
         * to work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        چاپ_کن("ERROR: userland image virt [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه")"
               "exceeds USER_TOP (%"SEL4_PRIx_کلمه")\n",
               it_v_reg.شروع, it_v_reg.انتها, (کلمه_t)USER_بالاترین);
        أرجع false;
    }

    /* make the free memory available to alloc_region() */
    إذا (!arch_init_freemem(ui_reg, dtb_p_reg, it_v_reg, extra_bi_اندازه_بیتها)) {
        چاپ_کن("ERROR: free memory management initialization failed\n");
        أرجع false;
    }

    /* create the root cnode */
    ریشه_cnode_cap = create_ریشه_cnode();
    إذا (cap_get_capType(ریشه_cnode_cap) == cap_null_cap) {
        چاپ_کن("ERROR: root c-node creation failed\n");
        أرجع false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(ریشه_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(ریشه_cnode_cap);

    /* create the bootinfo frame */
    populate_bi_قاب(0, CONFIG_MAX_رقم_NODES, ipcbuf_vptr, extra_bi_اندازه);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_سرآیند سرآیند;
    إذا (dtb_اندازه > 0) {
        سرآیند.id = SEL4_BOOTINFO_سرآیند_FDT;
        سرآیند.len = حجم(سرآیند) + dtb_اندازه;
        *(se_L4_Boot_Info_سرآیند *)(rootserver.extra_bi + extra_bi_جابه_جایی) = سرآیند;
        extra_bi_جابه_جایی += حجم(سرآیند);
        کپی_حافظه((فراغ *)(rootserver.extra_bi + extra_bi_جابه_جایی),
               paddr_مقصد_pptr(dtb_phys_addr),
               dtb_اندازه);
        extra_bi_جابه_جایی += dtb_اندازه;
    }

    أكد(extra_bi_اندازه == extra_bi_جابه_جایی);

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_آدرس_space(ریشه_cnode_cap, it_v_reg);
    إذا (cap_get_capType(it_pd_cap) == cap_null_cap) {
        چاپ_کن("ERROR: address space creation for initial thread failed\n");
        أرجع false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_قاب_cap(
        ریشه_cnode_cap,
        it_pd_cap,
        bi_قاب_vptr
    );

    /* create and map extra bootinfo region */
    إذا (extra_bi_اندازه > 0) {
        region_t extra_bi_region = {
            .شروع = rootserver.extra_bi,
            .انتها = rootserver.extra_bi + extra_bi_اندازه
        };
        extra_bi_ret =
            create_قابها_از_region(
                ریشه_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_مقصد_paddr((فراغ *)extra_bi_region.شروع) - extra_bi_قاب_vptr
            );
        إذا (!extra_bi_ret.success) {
            چاپ_کن("ERROR: mapping extra boot info to initial thread failed\n");
            أرجع false;
        }
        ndks_boot.bi_قاب->extraBIPages = extra_bi_ret.region;
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    init_sched_control(ریشه_cnode_cap, CONFIG_MAX_رقم_NODES);
#پایان_اگر

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_قاب_cap(ریشه_cnode_cap, it_pd_cap, ipcbuf_vptr);
    إذا (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        چاپ_کن("ERROR: could not create IPC buffer for initial thread\n");
        أرجع false;
    }

    /* create all userland image frames */
    create_قابها_ret =
        create_قابها_از_region(
            ریشه_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_جابه_جایی
        );
    إذا (!create_قابها_ret.success) {
        چاپ_کن("ERROR: could not create all userland image frames\n");
        أرجع false;
    }
    ndks_boot.bi_قاب->user_Image_قابها = create_قابها_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(ریشه_cnode_cap);
    إذا (cap_get_capType(it_ap_cap) == cap_null_cap) {
        چاپ_کن("ERROR: could not create ASID pool for initial thread\n");
        أرجع false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    گره_STATE(ks_Cur_زمان) = get_Current_زمان();
#پایان_اگر

    /* create the idle thread */
    create_idle_thread();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         ریشه_cnode_cap,
                         it_pd_cap,
                         v_مدخل,
                         bi_قاب_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    إذا (initial == عدم) {
        چاپ_کن("ERROR: could not create initial thread\n");
        أرجع false;
    }

    init_core_state(initial);

    /* convert the remaining free memory into UT objects and provide the caps */
    إذا (!create_untypeds(ریشه_cnode_cap)) {
        چاپ_کن("ERROR: could not create untypteds for kernel image boot memory\n");
        أرجع false;
    }

    /* no shared-frame caps (RISC-V has no multikernel support) */
    ndks_boot.bi_قاب->shared_قابها = S_REG_خالی;

    /* finalise the bootinfo frame */
    bi_finalise();

    ks_رقم_CPUs = 1;

    SMP_COND_STATEMENT(clh_قفل_init());
    SMP_COND_STATEMENT(release_secondary_cores());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    گره_قفل_SYS;

    چاپ_کن("Booting all finished, dropped to user space\n");
    أرجع true;
}

BOOT_کد VISIBLE فراغ init_kernel(
    paddr_t ui_p_reg_شروع,
    paddr_t ui_p_reg_انتها,
    sword_t pv_جابه_جایی,
    vptr_t  v_مدخل,
    paddr_t dtb_addr_p,
    uint32_t dtb_اندازه
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    ,
    کلمه_t hart_id,
    کلمه_t core_id
#پایان_اگر
)
{
    bool_t نتیجه;

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    جمع_کن_hart_مقصد_core_map(hart_id, core_id);
    إذا (core_id == 0) {
        نتیجه = try_init_kernel(ui_p_reg_شروع,
                                 ui_p_reg_انتها,
                                 pv_جابه_جایی,
                                 v_مدخل,
                                 dtb_addr_p,
                                 dtb_اندازه);
    } وإلا {
        نتیجه = try_init_kernel_secondary_core(hart_id, core_id);
    }
#وإلا
    نتیجه = try_init_kernel(ui_p_reg_شروع,
                             ui_p_reg_انتها,
                             pv_جابه_جایی,
                             v_مدخل,
                             dtb_addr_p,
                             dtb_اندازه);
#پایان_اگر
    إذا (!نتیجه) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    گره_STATE(ks_Cur_زمان) = get_Current_زمان();
    گره_STATE(ksConsumed) = 0;
#پایان_اگر

    زمانبندی();
    activateThread();
}
