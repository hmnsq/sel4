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

#ifdef ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS ایستا فرار _Atomic word_t گره_boot_lock;
#endif

BOOT_BSS ایستا region_t res_reg[رقم_RESERVED_REGIONS];

BOOT_CODE cap_t create_mapped_it_frame_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t
                                           use_large, bool_t executable)
{
    cap_t cap;
    vm_page_size_t frame_size;

    اگر (use_large) {
        frame_size = RISCV_Mega_Page;
    } وگرنه {
        frame_size = RISCV_4K_Page;
    }

    cap = cap_frame_cap_new(
              asid,                            /* capFMappedASID       */
              pptr,                            /* capFBasePtr          */
              frame_size,                      /* capFSize             */
              wordFromVMRights(VMReadWrite),   /* capFVMRights         */
              0,                               /* capFIsDevice         */
              vptr                             /* capFMappedAddress    */
          );

    map_it_frame_cap(pd_cap, cap);
    بازگشت cap;
}

BOOT_CODE ایستا bool_t arch_init_freemem(region_t ui_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          word_t extra_bi_size_bits)
{
    /* Reserve the kernel image region. This may look a bit awkward, as the
     * symbols are a reference in the kernel image window, but all allocations
     * are done in terms of the main kernel window, so we do some translation.
     */
    res_reg[0] = paddr_to_pptr_reg(get_p_reg_kernel_img());
    صحیح index = 1;

    /* add the dtb region, if it is not empty */
    اگر (dtb_p_reg.start) {
        اگر (index >= ARRAY_SIZE(res_reg)) {
            printf("ERROR: no slot to add DTB to reserved regions\n");
            بازگشت false;
        }
        res_reg[index] = paddr_to_pptr_reg(dtb_p_reg);
        index += 1;
    }

    /* reserve the user image region */
    اگر (index >= ARRAY_SIZE(res_reg)) {
        printf("ERROR: no slot to add user image to reserved regions\n");
        بازگشت false;
    }
    res_reg[index] = ui_reg;
    index += 1;

    /* avail_p_regs comes from the auto-generated code */
    بازگشت init_freemem(ARRAY_SIZE(avail_p_regs), avail_p_regs,
                        index, res_reg,
                        it_v_reg, extra_bi_size_bits);
}

BOOT_CODE ایستا تهی init_irqs(cap_t ریشه_cnode_cap)
{
    irq_t i;

    برای (i = 0; i <= maxIRQ; i++) {
        اگر (i != irqInvalid) {
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
    write_slot(SLOT_PTR(pptr_of_cap(ریشه_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

/* This and only this function initialises the CPU. It does NOT initialise any kernel state. */

#ifdef CONFIG_HAVE_FPU
BOOT_CODE ایستا تهی init_fpu(تهی)
{
    set_fs_clean();
    write_fcsr(0);
    disableFpu();
}
#endif

BOOT_CODE ایستا تهی init_cpu(تهی)
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

BOOT_CODE ایستا تهی init_plat(تهی)
{
    initIRQController();
}


#ifdef ENABLE_SMP_SUPPORT
BOOT_CODE ایستا bool_t try_init_kernel_secondary_core(word_t hart_id, word_t core_id)
{
    وقتی (!گره_boot_lock);

    fence_r_rw();

    init_cpu();
    گره_LOCK_SYS;

    clock_sync_test();
    ks_رقم_CPUs++;
    init_core_state(SchedulerAction_ResumeCurrentThread);
    ifence_local();
    بازگشت true;
}

BOOT_CODE ایستا تهی release_secondary_cores(تهی)
{
    أكد(0 == گره_boot_lock); /* Sanity check for a proper lock state. */
    گره_boot_lock = 1;
    /* At this point in time the primary core (executing this code) already uses
     * the seL4 MMU/cache setup. However, the secondary cores are still using
     * the elfloader's MMU/cache setup, and thus the update of node_boot_lock
     * may not be visible there if the setups differ. Currently, the mappings
     * match, so a barrier is all that is needed.
     */
    fence_rw_rw();

    وقتی (ks_رقم_CPUs != CONFIG_MAX_رقم_NODES) {
#ifdef ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        گره_STATE(ksCurTime) = getCurrentTime();
#endif
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#endif /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

ایستا BOOT_CODE bool_t try_init_kernel(
    paddr_t ui_p_reg_start,
    paddr_t ui_p_reg_end,
    sword_t pv_offset,
    vptr_t  v_entry,
    paddr_t dtb_phys_addr,
    word_t  dtb_size
)
{
    cap_t ریشه_cnode_cap;
    cap_t it_pd_cap;
    cap_t it_ap_cap;
    cap_t ipcbuf_cap;
    region_t ui_reg = paddr_to_pptr_reg((p_region_t) {
        ui_p_reg_start, ui_p_reg_end
    });
    word_t extra_bi_size = 0;
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
    اگر (dtb_size > 0) {
#ifdef CONFIG_PLAT_ROCKETCHIP_ZCU102
        /* The softcore rocketchip instantiation doesn't work well when this
         * page isn't reserved. Round up so the whole page is reserved to
         * avoid the problem
         */
        dtb_size = ROUND_UP(dtb_size, PAGE_BITS);
#endif
        paddr_t dtb_phys_end = dtb_phys_addr + dtb_size;
        اگر (dtb_phys_end < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            printf("ERROR: DTB location at %"SEL4_PRIx_word
                   " len %"SEL4_PRIu_word" invalid\n",
                   dtb_phys_addr, dtb_size);
            بازگشت false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        اگر (dtb_phys_end >= PADDR_بالاترین) {
            printf("ERROR: DTB at [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_word")\n",
                   dtb_phys_addr, dtb_phys_end, PADDR_بالاترین);
            بازگشت false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_size += اندازه(seL4_BootInfoHeader) + dtb_size;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .start = dtb_phys_addr,
            .end   = dtb_phys_end
        };
    }

    /* The region of the initial thread is the user image + ipcbuf + boot info + extra */
    word_t extra_bi_size_bits = calculate_extra_bi_size_bits(extra_bi_size);
    v_region_t it_v_reg = {
        .start = ui_v_reg.start,
        .end   = extra_bi_frame_vptr + (extra_bi_size_bits > 0 ? BIT(extra_bi_size_bits) : 0)
    };
    اگر (it_v_reg.end >= USER_بالاترین) {
        /* Variable arguments for printf() require well defined integer types
         * to work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        printf("ERROR: userland image virt [%"SEL4_PRIx_word"..%"SEL4_PRIx_word")"
               "exceeds USER_TOP (%"SEL4_PRIx_word")\n",
               it_v_reg.start, it_v_reg.end, (word_t)USER_بالاترین);
        بازگشت false;
    }

    /* make the free memory available to alloc_region() */
    اگر (!arch_init_freemem(ui_reg, dtb_p_reg, it_v_reg, extra_bi_size_bits)) {
        printf("ERROR: free memory management initialization failed\n");
        بازگشت false;
    }

    /* create the root cnode */
    ریشه_cnode_cap = create_ریشه_cnode();
    اگر (cap_get_capType(ریشه_cnode_cap) == cap_null_cap) {
        printf("ERROR: root c-node creation failed\n");
        بازگشت false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(ریشه_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(ریشه_cnode_cap);

    /* create the bootinfo frame */
    populate_bi_frame(0, CONFIG_MAX_رقم_NODES, ipcbuf_vptr, extra_bi_size);

    /* put DTB in the bootinfo block, if present. */
    seL4_BootInfoHeader header;
    اگر (dtb_size > 0) {
        header.id = SEL4_BOOTINFO_HEADER_FDT;
        header.len = اندازه(header) + dtb_size;
        *(seL4_BootInfoHeader *)(rootserver.extra_bi + extra_bi_offset) = header;
        extra_bi_offset += اندازه(header);
        memcpy((تهی *)(rootserver.extra_bi + extra_bi_offset),
               paddr_to_pptr(dtb_phys_addr),
               dtb_size);
        extra_bi_offset += dtb_size;
    }

    أكد(extra_bi_size == extra_bi_offset);

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_address_space(ریشه_cnode_cap, it_v_reg);
    اگر (cap_get_capType(it_pd_cap) == cap_null_cap) {
        printf("ERROR: address space creation for initial thread failed\n");
        بازگشت false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_frame_cap(
        ریشه_cnode_cap,
        it_pd_cap,
        bi_frame_vptr
    );

    /* create and map extra bootinfo region */
    اگر (extra_bi_size > 0) {
        region_t extra_bi_region = {
            .start = rootserver.extra_bi,
            .end = rootserver.extra_bi + extra_bi_size
        };
        extra_bi_ret =
            create_frames_of_region(
                ریشه_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_to_paddr((تهی *)extra_bi_region.start) - extra_bi_frame_vptr
            );
        اگر (!extra_bi_ret.success) {
            printf("ERROR: mapping extra boot info to initial thread failed\n");
            بازگشت false;
        }
        ndks_boot.bi_frame->extraBIPages = extra_bi_ret.region;
    }

#ifdef CONFIG_KERNEL_MCS
    init_sched_control(ریشه_cnode_cap, CONFIG_MAX_رقم_NODES);
#endif

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_frame_cap(ریشه_cnode_cap, it_pd_cap, ipcbuf_vptr);
    اگر (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        printf("ERROR: could not create IPC buffer for initial thread\n");
        بازگشت false;
    }

    /* create all userland image frames */
    create_frames_ret =
        create_frames_of_region(
            ریشه_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_offset
        );
    اگر (!create_frames_ret.success) {
        printf("ERROR: could not create all userland image frames\n");
        بازگشت false;
    }
    ndks_boot.bi_frame->userImageFrames = create_frames_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(ریشه_cnode_cap);
    اگر (cap_get_capType(it_ap_cap) == cap_null_cap) {
        printf("ERROR: could not create ASID pool for initial thread\n");
        بازگشت false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#ifdef CONFIG_KERNEL_MCS
    گره_STATE(ksCurTime) = getCurrentTime();
#endif

    /* create the idle thread */
    create_idle_thread();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         ریشه_cnode_cap,
                         it_pd_cap,
                         v_entry,
                         bi_frame_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    اگر (initial == پوچ) {
        printf("ERROR: could not create initial thread\n");
        بازگشت false;
    }

    init_core_state(initial);

    /* convert the remaining free memory into UT objects and provide the caps */
    اگر (!create_untypeds(ریشه_cnode_cap)) {
        printf("ERROR: could not create untypteds for kernel image boot memory\n");
        بازگشت false;
    }

    /* no shared-frame caps (RISC-V has no multikernel support) */
    ndks_boot.bi_frame->sharedFrames = S_REG_EMPTY;

    /* finalise the bootinfo frame */
    bi_finalise();

    ks_رقم_CPUs = 1;

    SMP_COND_STATEMENT(clh_lock_init());
    SMP_COND_STATEMENT(release_secondary_cores());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    گره_LOCK_SYS;

    printf("Booting all finished, dropped to user space\n");
    بازگشت true;
}

BOOT_CODE VISIBLE تهی init_kernel(
    paddr_t ui_p_reg_start,
    paddr_t ui_p_reg_end,
    sword_t pv_offset,
    vptr_t  v_entry,
    paddr_t dtb_addr_p,
    uint32_t dtb_size
#ifdef ENABLE_SMP_SUPPORT
    ,
    word_t hart_id,
    word_t core_id
#endif
)
{
    bool_t result;

#ifdef ENABLE_SMP_SUPPORT
    add_hart_to_core_map(hart_id, core_id);
    اگر (core_id == 0) {
        result = try_init_kernel(ui_p_reg_start,
                                 ui_p_reg_end,
                                 pv_offset,
                                 v_entry,
                                 dtb_addr_p,
                                 dtb_size);
    } وگرنه {
        result = try_init_kernel_secondary_core(hart_id, core_id);
    }
#وگرنه
    result = try_init_kernel(ui_p_reg_start,
                             ui_p_reg_end,
                             pv_offset,
                             v_entry,
                             dtb_addr_p,
                             dtb_size);
#endif
    اگر (!result) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#ifdef CONFIG_KERNEL_MCS
    گره_STATE(ksCurTime) = getCurrentTime();
    گره_STATE(ksConsumed) = 0;
#endif

    schedule();
    activateThread();
}
