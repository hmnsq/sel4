/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <kernel/boot.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <model/statedata.h>
#অন্তর্ভাব <object/interrupt.h>
#অন্তর্ভাব <arch/machine.h>
#অন্তর্ভাব <arch/kernel/boot.h>
#অন্তর্ভাব <arch/kernel/traps.h>
#অন্তর্ভাব <arch/kernel/vspace.h>
#অন্তর্ভাব <arch/benchmark.h>
#অন্তর্ভাব <linker.h>
#অন্তর্ভাব <plat/machine/hardware.h>
#অন্তর্ভাব <machine.h>

#ifdef ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS স্থির অস্থির _Atomic word_t গ্রন্থি_boot_lock;
#endif

BOOT_BSS স্থির region_t res_reg[অঙ্ক_RESERVED_REGIONS];

BOOT_CODE cap_t create_mapped_it_frame_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t
                                           use_large, bool_t executable)
{
    cap_t cap;
    vm_page_আকার_t frame_আকার_2;

    যদি (use_large) {
        frame_আকার_2 = RISCV_Mega_Page;
    } অন্যথা {
        frame_আকার_2 = RISCV_4K_Page;
    }

    cap = cap_frame_cap_new(
              asid,                            /* capFMappedASID       */
              pptr,                            /* capFBasePtr          */
              frame_আকার_2,                      /* capFSize             */
              wordFromVMRights(VMReadWrite),   /* capFVMRights         */
              0,                               /* capFIsDevice         */
              vptr                             /* capFMappedAddress    */
          );

    map_it_frame_cap(pd_cap, cap);
    ফেরত cap;
}

BOOT_CODE স্থির bool_t arch_init_freemem(region_t ui_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          word_t extra_bi_আকার_bits)
{
    /* Reserve the kernel image region. This may look a bit awkward, as the
     * symbols are a reference in the kernel image window, but all allocations
     * are done in terms of the main kernel window, so we do some translation.
     */
    res_reg[0] = paddr_to_pptr_reg(get_p_reg_kernel_img());
    পূর্ণসংখ্যা সূচক = 1;

    /* add the dtb region, if it is not empty */
    যদি (dtb_p_reg.start) {
        যদি (সূচক >= ARRAY_আকার(res_reg)) {
            printf("ERROR: no slot to add DTB to reserved regions\n");
            ফেরত false;
        }
        res_reg[সূচক] = paddr_to_pptr_reg(dtb_p_reg);
        সূচক += 1;
    }

    /* reserve the user image region */
    যদি (সূচক >= ARRAY_আকার(res_reg)) {
        printf("ERROR: no slot to add user image to reserved regions\n");
        ফেরত false;
    }
    res_reg[সূচক] = ui_reg;
    সূচক += 1;

    /* avail_p_regs comes from the auto-generated code */
    ফেরত init_freemem(ARRAY_আকার(avail_p_regs), avail_p_regs,
                        সূচক, res_reg,
                        it_v_reg, extra_bi_আকার_bits);
}

BOOT_CODE স্থির শূন্য init_irqs(cap_t মূল_cnode_cap)
{
    irq_t i;

    জন্য (i = 0; i <= maxIRQ; i++) {
        যদি (i != irqInvalid) {
            /* IRQ 0 is irqInvalid */
            setIRQState(IRQInactive, i);
        }
    }
    setIRQState(IRQTimer, KERNEL_TIMER_IRQ);
#ifdef ENABLE_SMP_SUPPORT
    setIRQState(IRQIPI, irq_remote_call_ipi);
    setIRQState(IRQIPI, irq_reschedule_ipi);
#endif
    /* provide the IRQ control cap */
    write_slot(SLOT_PTR(pptr_of_cap(মূল_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

/* This and only this function initialises the CPU. It does NOT initialise any kernel state. */

#ifdef CONFIG_HAVE_FPU
BOOT_CODE স্থির শূন্য init_fpu(শূন্য)
{
    set_fs_clean();
    write_fcsr(0);
    disableFpu();
}
#endif

BOOT_CODE স্থির শূন্য init_cpu(শূন্য)
{

    activate_kernel_vspace();
    /* Write trap entry address to stvec */
    write_stvec((word_t)trap_entry);
    initLocalIRQController();
#ifndef CONFIG_KERNEL_MCS
    initTimer();
#endif

    /* disable FPU access */
    set_fs_off();

#ifdef CONFIG_HAVE_FPU
    init_fpu();
#endif
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_CODE স্থির শূন্য init_plat(শূন্য)
{
    initIRQController();
}


#ifdef ENABLE_SMP_SUPPORT
BOOT_CODE স্থির bool_t try_init_kernel_secondary_core(word_t hart_id, word_t core_id)
{
    যখন (!গ্রন্থি_boot_lock);

    fence_r_rw();

    init_cpu();
    গ্রন্থি_LOCK_SYS;

    clock_sync_test();
    ks_অঙ্ক_CPUs++;
    init_core_state(SchedulerAction_ResumeCurrentThread);
    ifence_local();
    ফেরত true;
}

BOOT_CODE স্থির শূন্য release_secondary_cores(শূন্য)
{
    প্রতিজ্ঞা(0 == গ্রন্থি_boot_lock); /* Sanity check for a proper lock state. */
    গ্রন্থি_boot_lock = 1;
    /* At this point in time the primary core (executing this code) already uses
     * the seL4 MMU/cache setup. However, the secondary cores are still using
     * the elfloader's MMU/cache setup, and thus the update of node_boot_lock
     * may not be visible there if the setups differ. Currently, the mappings
     * match, so a barrier is all that is needed.
     */
    fence_rw_rw();

    যখন (ks_অঙ্ক_CPUs != CONFIG_MAX_অঙ্ক_NODES) {
#ifdef ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        গ্রন্থি_STATE(ksCurTime) = getCurrentTime();
#endif
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#endif /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

স্থির BOOT_CODE bool_t try_init_kernel(
    paddr_t ui_p_reg_start,
    paddr_t ui_p_reg_end,
    sword_t pv_offset,
    vptr_t  v_entry,
    paddr_t dtb_phys_addr,
    word_t  dtb_আকার
)
{
    cap_t মূল_cnode_cap;
    cap_t it_pd_cap;
    cap_t it_ap_cap;
    cap_t ipcbuf_cap;
    region_t ui_reg = paddr_to_pptr_reg((p_region_t) {
        ui_p_reg_start, ui_p_reg_end
    });
    word_t extra_bi_আকার = 0;
    pptr_t extra_bi_offset = 0;
    vptr_t extra_bi_frame_vptr;
    vptr_t bi_frame_vptr;
    vptr_t ipcbuf_vptr;
    create_frames_of_region_ret_t create_frames_ret;
    create_frames_of_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .start = ui_p_reg_start - pv_offset,
        .end   = ui_p_reg_end   - pv_offset
    };

    ipcbuf_vptr = ui_v_reg.end;
    bi_frame_vptr = ipcbuf_vptr + BIT(PAGE_BITS);
    extra_bi_frame_vptr = bi_frame_vptr + BIT(seL4_BootInfoFrameBits);

    map_kernel_window();

    /* initialise the CPU */
    init_cpu();

    printf("Bootstrapping kernel\n");

    /* initialize the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_EMPTY;
    যদি (dtb_আকার > 0) {
#ifdef CONFIG_PLAT_ROCKETCHIP_ZCU102
        /* The softcore rocketchip instantiation doesn't work well when this
         * page isn't reserved. Round up so the whole page is reserved to
         * avoid the problem
         */
        dtb_আকার = ROUND_UP(dtb_আকার, PAGE_BITS);
#endif
        paddr_t dtb_phys_end = dtb_phys_addr + dtb_আকার;
        যদি (dtb_phys_end < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            printf("ERROR: DTB location at %"SEL4_PRIx_word
                   " len %"SEL4_PRIu_word" invalid\n",
                   dtb_phys_addr, dtb_আকার);
            ফেরত false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        যদি (dtb_phys_end >= PADDR_শীর্ষ) {
            printf("ERROR: DTB at [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_word")\n",
                   dtb_phys_addr, dtb_phys_end, PADDR_শীর্ষ);
            ফেরত false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_আকার += আকার(seL4_BootInfoHeader) + dtb_আকার;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .start = dtb_phys_addr,
            .end   = dtb_phys_end
        };
    }

    /* The region of the initial thread is the user image + ipcbuf + boot info + extra */
    word_t extra_bi_আকার_bits = calculate_extra_bi_আকার_bits(extra_bi_আকার);
    v_region_t it_v_reg = {
        .start = ui_v_reg.start,
        .end   = extra_bi_frame_vptr + (extra_bi_আকার_bits > 0 ? BIT(extra_bi_আকার_bits) : 0)
    };
    যদি (it_v_reg.end >= USER_শীর্ষ) {
        /* Variable arguments for printf() require well defined integer types
         * to work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        printf("ERROR: userland image virt [%"SEL4_PRIx_word"..%"SEL4_PRIx_word")"
               "exceeds USER_TOP (%"SEL4_PRIx_word")\n",
               it_v_reg.start, it_v_reg.end, (word_t)USER_শীর্ষ);
        ফেরত false;
    }

    /* make the free memory available to alloc_region() */
    যদি (!arch_init_freemem(ui_reg, dtb_p_reg, it_v_reg, extra_bi_আকার_bits)) {
        printf("ERROR: free memory management initialization failed\n");
        ফেরত false;
    }

    /* create the root cnode */
    মূল_cnode_cap = create_মূল_cnode();
    যদি (cap_get_capType(মূল_cnode_cap) == cap_null_cap) {
        printf("ERROR: root c-node creation failed\n");
        ফেরত false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(মূল_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(মূল_cnode_cap);

    /* create the bootinfo frame */
    populate_bi_frame(0, CONFIG_MAX_অঙ্ক_NODES, ipcbuf_vptr, extra_bi_আকার);

    /* put DTB in the bootinfo block, if present. */
    seL4_BootInfoHeader header;
    যদি (dtb_আকার > 0) {
        header.id = SEL4_BOOTINFO_HEADER_FDT;
        header.len = আকার(header) + dtb_আকার;
        *(seL4_BootInfoHeader *)(rootserver.extra_bi + extra_bi_offset) = header;
        extra_bi_offset += আকার(header);
        memcpy((শূন্য *)(rootserver.extra_bi + extra_bi_offset),
               paddr_to_pptr(dtb_phys_addr),
               dtb_আকার);
        extra_bi_offset += dtb_আকার;
    }

    প্রতিজ্ঞা(extra_bi_আকার == extra_bi_offset);

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_address_space(মূল_cnode_cap, it_v_reg);
    যদি (cap_get_capType(it_pd_cap) == cap_null_cap) {
        printf("ERROR: address space creation for initial thread failed\n");
        ফেরত false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_frame_cap(
        মূল_cnode_cap,
        it_pd_cap,
        bi_frame_vptr
    );

    /* create and map extra bootinfo region */
    যদি (extra_bi_আকার > 0) {
        region_t extra_bi_region = {
            .start = rootserver.extra_bi,
            .end = rootserver.extra_bi + extra_bi_আকার
        };
        extra_bi_ret =
            create_frames_of_region(
                মূল_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_to_paddr((শূন্য *)extra_bi_region.start) - extra_bi_frame_vptr
            );
        যদি (!extra_bi_ret.success) {
            printf("ERROR: mapping extra boot info to initial thread failed\n");
            ফেরত false;
        }
        ndks_boot.bi_frame->extraBIPages = extra_bi_ret.region;
    }

#ifdef CONFIG_KERNEL_MCS
    init_sched_control(মূল_cnode_cap, CONFIG_MAX_অঙ্ক_NODES);
#endif

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_frame_cap(মূল_cnode_cap, it_pd_cap, ipcbuf_vptr);
    যদি (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        printf("ERROR: could not create IPC buffer for initial thread\n");
        ফেরত false;
    }

    /* create all userland image frames */
    create_frames_ret =
        create_frames_of_region(
            মূল_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_offset
        );
    যদি (!create_frames_ret.success) {
        printf("ERROR: could not create all userland image frames\n");
        ফেরত false;
    }
    ndks_boot.bi_frame->userImageFrames = create_frames_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(মূল_cnode_cap);
    যদি (cap_get_capType(it_ap_cap) == cap_null_cap) {
        printf("ERROR: could not create ASID pool for initial thread\n");
        ফেরত false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#ifdef CONFIG_KERNEL_MCS
    গ্রন্থি_STATE(ksCurTime) = getCurrentTime();
#endif

    /* create the idle thread */
    create_idle_thread();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         মূল_cnode_cap,
                         it_pd_cap,
                         v_entry,
                         bi_frame_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    যদি (initial == ফাঁকা) {
        printf("ERROR: could not create initial thread\n");
        ফেরত false;
    }

    init_core_state(initial);

    /* convert the remaining free memory into UT objects and provide the caps */
    যদি (!create_untypeds(মূল_cnode_cap)) {
        printf("ERROR: could not create untypteds for kernel image boot memory\n");
        ফেরত false;
    }

    /* no shared-frame caps (RISC-V has no multikernel support) */
    ndks_boot.bi_frame->sharedFrames = S_REG_EMPTY;

    /* finalise the bootinfo frame */
    bi_finalise();

    ks_অঙ্ক_CPUs = 1;

    SMP_COND_STATEMENT(clh_lock_init());
    SMP_COND_STATEMENT(release_secondary_cores());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    গ্রন্থি_LOCK_SYS;

    printf("Booting all finished, dropped to user space\n");
    ফেরত true;
}

BOOT_CODE VISIBLE শূন্য init_kernel(
    paddr_t ui_p_reg_start,
    paddr_t ui_p_reg_end,
    sword_t pv_offset,
    vptr_t  v_entry,
    paddr_t dtb_addr_p,
    uint32_t dtb_আকার
#ifdef ENABLE_SMP_SUPPORT
    ,
    word_t hart_id,
    word_t core_id
#endif
)
{
    bool_t ফলাফল;

#ifdef ENABLE_SMP_SUPPORT
    add_hart_to_core_map(hart_id, core_id);
    যদি (core_id == 0) {
        ফলাফল = try_init_kernel(ui_p_reg_start,
                                 ui_p_reg_end,
                                 pv_offset,
                                 v_entry,
                                 dtb_addr_p,
                                 dtb_আকার);
    } অন্যথা {
        ফলাফল = try_init_kernel_secondary_core(hart_id, core_id);
    }
#অন্যথা
    ফলাফল = try_init_kernel(ui_p_reg_start,
                             ui_p_reg_end,
                             pv_offset,
                             v_entry,
                             dtb_addr_p,
                             dtb_আকার);
#endif
    যদি (!ফলাফল) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#ifdef CONFIG_KERNEL_MCS
    গ্রন্থি_STATE(ksCurTime) = getCurrentTime();
    গ্রন্থি_STATE(ksConsumed) = 0;
#endif

    schedule();
    activateThread();
}
