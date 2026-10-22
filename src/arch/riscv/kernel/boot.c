/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <kernel/boot.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <model/statedata.h>
#અન્તર્ભાવ <object/interrupt.h>
#અન્તર્ભાવ <arch/machine.h>
#અન્તર્ભાવ <arch/kernel/boot.h>
#અન્તર્ભાવ <arch/kernel/traps.h>
#અન્તર્ભાવ <arch/kernel/vspace.h>
#અન્તર્ભાવ <arch/benchmark.h>
#અન્તર્ભાવ <linker.h>
#અન્તર્ભાવ <plat/machine/hardware.h>
#અન્તર્ભાવ <machine.h>

#ifdef ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS સ્થિર અસ્થિર _Atomic word_t ગાંઠ_boot_lock;
#endif

BOOT_BSS સ્થિર region_t res_reg[અંક_RESERVED_REGIONS];

BOOT_CODE cap_t create_mapped_it_frame_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t
                                           use_large, bool_t executable)
{
    cap_t cap;
    vm_page_size_t frame_size;

    જો (use_large) {
        frame_size = RISCV_Mega_Page;
    } નહીંતર {
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
    પાછા cap;
}

BOOT_CODE સ્થિર bool_t arch_init_freemem(region_t ui_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          word_t extra_bi_size_bits)
{
    /* Reserve the kernel image region. This may look a bit awkward, as the
     * symbols are a reference in the kernel image window, but all allocations
     * are done in terms of the main kernel window, so we do some translation.
     */
    res_reg[0] = paddr_to_pptr_reg(get_p_reg_kernel_img());
    પૂર્ણાંક index = 1;

    /* add the dtb region, if it is not empty */
    જો (dtb_p_reg.start) {
        જો (index >= ARRAY_SIZE(res_reg)) {
            printf("ERROR: no slot to add DTB to reserved regions\n");
            પાછા false;
        }
        res_reg[index] = paddr_to_pptr_reg(dtb_p_reg);
        index += 1;
    }

    /* reserve the user image region */
    જો (index >= ARRAY_SIZE(res_reg)) {
        printf("ERROR: no slot to add user image to reserved regions\n");
        પાછા false;
    }
    res_reg[index] = ui_reg;
    index += 1;

    /* avail_p_regs comes from the auto-generated code */
    પાછા init_freemem(ARRAY_SIZE(avail_p_regs), avail_p_regs,
                        index, res_reg,
                        it_v_reg, extra_bi_size_bits);
}

BOOT_CODE સ્થિર શૂન્ય init_irqs(cap_t મૂળ_cnode_cap)
{
    irq_t i;

    માટે (i = 0; i <= maxIRQ; i++) {
        જો (i != irqInvalid) {
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
    write_slot(SLOT_PTR(pptr_of_cap(મૂળ_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

/* This and only this function initialises the CPU. It does NOT initialise any kernel state. */

#ifdef CONFIG_HAVE_FPU
BOOT_CODE સ્થિર શૂન્ય init_fpu(શૂન્ય)
{
    set_fs_clean();
    write_fcsr(0);
    disableFpu();
}
#endif

BOOT_CODE સ્થિર શૂન્ય init_cpu(શૂન્ય)
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

BOOT_CODE સ્થિર શૂન્ય init_plat(શૂન્ય)
{
    initIRQController();
}


#ifdef ENABLE_SMP_SUPPORT
BOOT_CODE સ્થિર bool_t try_init_kernel_secondary_core(word_t hart_id, word_t core_id)
{
    જ્યારે (!ગાંઠ_boot_lock);

    fence_r_rw();

    init_cpu();
    ગાંઠ_LOCK_SYS;

    clock_sync_test();
    ks_અંક_CPUs++;
    init_core_state(SchedulerAction_ResumeCurrentThread);
    ifence_local();
    પાછા true;
}

BOOT_CODE સ્થિર શૂન્ય release_secondary_cores(શૂન્ય)
{
    પ્રતિજ્ઞા(0 == ગાંઠ_boot_lock); /* Sanity check for a proper lock state. */
    ગાંઠ_boot_lock = 1;
    /* At this point in time the primary core (executing this code) already uses
     * the seL4 MMU/cache setup. However, the secondary cores are still using
     * the elfloader's MMU/cache setup, and thus the update of node_boot_lock
     * may not be visible there if the setups differ. Currently, the mappings
     * match, so a barrier is all that is needed.
     */
    fence_rw_rw();

    જ્યારે (ks_અંક_CPUs != CONFIG_MAX_અંક_NODES) {
#ifdef ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        ગાંઠ_STATE(ksCurTime) = getCurrentTime();
#endif
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#endif /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

સ્થિર BOOT_CODE bool_t try_init_kernel(
    paddr_t ui_p_reg_start,
    paddr_t ui_p_reg_end,
    sword_t pv_offset,
    vptr_t  v_entry,
    paddr_t dtb_phys_addr,
    word_t  dtb_size
)
{
    cap_t મૂળ_cnode_cap;
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
    જો (dtb_size > 0) {
#ifdef CONFIG_PLAT_ROCKETCHIP_ZCU102
        /* The softcore rocketchip instantiation doesn't work well when this
         * page isn't reserved. Round up so the whole page is reserved to
         * avoid the problem
         */
        dtb_size = ROUND_UP(dtb_size, PAGE_BITS);
#endif
        paddr_t dtb_phys_end = dtb_phys_addr + dtb_size;
        જો (dtb_phys_end < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            printf("ERROR: DTB location at %"SEL4_PRIx_word
                   " len %"SEL4_PRIu_word" invalid\n",
                   dtb_phys_addr, dtb_size);
            પાછા false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        જો (dtb_phys_end >= PADDR_ટોચ) {
            printf("ERROR: DTB at [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_word")\n",
                   dtb_phys_addr, dtb_phys_end, PADDR_ટોચ);
            પાછા false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_size += કદ(seL4_BootInfoHeader) + dtb_size;
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
    જો (it_v_reg.end >= USER_ટોચ) {
        /* Variable arguments for printf() require well defined integer types
         * to work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        printf("ERROR: userland image virt [%"SEL4_PRIx_word"..%"SEL4_PRIx_word")"
               "exceeds USER_TOP (%"SEL4_PRIx_word")\n",
               it_v_reg.start, it_v_reg.end, (word_t)USER_ટોચ);
        પાછા false;
    }

    /* make the free memory available to alloc_region() */
    જો (!arch_init_freemem(ui_reg, dtb_p_reg, it_v_reg, extra_bi_size_bits)) {
        printf("ERROR: free memory management initialization failed\n");
        પાછા false;
    }

    /* create the root cnode */
    મૂળ_cnode_cap = create_મૂળ_cnode();
    જો (cap_get_capType(મૂળ_cnode_cap) == cap_null_cap) {
        printf("ERROR: root c-node creation failed\n");
        પાછા false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(મૂળ_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(મૂળ_cnode_cap);

    /* create the bootinfo frame */
    populate_bi_frame(0, CONFIG_MAX_અંક_NODES, ipcbuf_vptr, extra_bi_size);

    /* put DTB in the bootinfo block, if present. */
    seL4_BootInfoHeader header;
    જો (dtb_size > 0) {
        header.id = SEL4_BOOTINFO_HEADER_FDT;
        header.len = કદ(header) + dtb_size;
        *(seL4_BootInfoHeader *)(rootserver.extra_bi + extra_bi_offset) = header;
        extra_bi_offset += કદ(header);
        memcpy((શૂન્ય *)(rootserver.extra_bi + extra_bi_offset),
               paddr_to_pptr(dtb_phys_addr),
               dtb_size);
        extra_bi_offset += dtb_size;
    }

    પ્રતિજ્ઞા(extra_bi_size == extra_bi_offset);

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_address_space(મૂળ_cnode_cap, it_v_reg);
    જો (cap_get_capType(it_pd_cap) == cap_null_cap) {
        printf("ERROR: address space creation for initial thread failed\n");
        પાછા false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_frame_cap(
        મૂળ_cnode_cap,
        it_pd_cap,
        bi_frame_vptr
    );

    /* create and map extra bootinfo region */
    જો (extra_bi_size > 0) {
        region_t extra_bi_region = {
            .start = rootserver.extra_bi,
            .end = rootserver.extra_bi + extra_bi_size
        };
        extra_bi_ret =
            create_frames_of_region(
                મૂળ_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_to_paddr((શૂન્ય *)extra_bi_region.start) - extra_bi_frame_vptr
            );
        જો (!extra_bi_ret.success) {
            printf("ERROR: mapping extra boot info to initial thread failed\n");
            પાછા false;
        }
        ndks_boot.bi_frame->extraBIPages = extra_bi_ret.region;
    }

#ifdef CONFIG_KERNEL_MCS
    init_sched_control(મૂળ_cnode_cap, CONFIG_MAX_અંક_NODES);
#endif

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_frame_cap(મૂળ_cnode_cap, it_pd_cap, ipcbuf_vptr);
    જો (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        printf("ERROR: could not create IPC buffer for initial thread\n");
        પાછા false;
    }

    /* create all userland image frames */
    create_frames_ret =
        create_frames_of_region(
            મૂળ_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_offset
        );
    જો (!create_frames_ret.success) {
        printf("ERROR: could not create all userland image frames\n");
        પાછા false;
    }
    ndks_boot.bi_frame->userImageFrames = create_frames_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(મૂળ_cnode_cap);
    જો (cap_get_capType(it_ap_cap) == cap_null_cap) {
        printf("ERROR: could not create ASID pool for initial thread\n");
        પાછા false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#ifdef CONFIG_KERNEL_MCS
    ગાંઠ_STATE(ksCurTime) = getCurrentTime();
#endif

    /* create the idle thread */
    create_idle_thread();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         મૂળ_cnode_cap,
                         it_pd_cap,
                         v_entry,
                         bi_frame_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    જો (initial == ખાલી) {
        printf("ERROR: could not create initial thread\n");
        પાછા false;
    }

    init_core_state(initial);

    /* convert the remaining free memory into UT objects and provide the caps */
    જો (!create_untypeds(મૂળ_cnode_cap)) {
        printf("ERROR: could not create untypteds for kernel image boot memory\n");
        પાછા false;
    }

    /* no shared-frame caps (RISC-V has no multikernel support) */
    ndks_boot.bi_frame->sharedFrames = S_REG_EMPTY;

    /* finalise the bootinfo frame */
    bi_finalise();

    ks_અંક_CPUs = 1;

    SMP_COND_STATEMENT(clh_lock_init());
    SMP_COND_STATEMENT(release_secondary_cores());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    ગાંઠ_LOCK_SYS;

    printf("Booting all finished, dropped to user space\n");
    પાછા true;
}

BOOT_CODE VISIBLE શૂન્ય init_kernel(
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
    જો (core_id == 0) {
        result = try_init_kernel(ui_p_reg_start,
                                 ui_p_reg_end,
                                 pv_offset,
                                 v_entry,
                                 dtb_addr_p,
                                 dtb_size);
    } નહીંતર {
        result = try_init_kernel_secondary_core(hart_id, core_id);
    }
#નહીંતર
    result = try_init_kernel(ui_p_reg_start,
                             ui_p_reg_end,
                             pv_offset,
                             v_entry,
                             dtb_addr_p,
                             dtb_size);
#endif
    જો (!result) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#ifdef CONFIG_KERNEL_MCS
    ગાંઠ_STATE(ksCurTime) = getCurrentTime();
    ગાંઠ_STATE(ksConsumed) = 0;
#endif

    schedule();
    activateThread();
}
