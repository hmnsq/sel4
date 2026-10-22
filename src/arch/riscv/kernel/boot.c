/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <assert.h>
#समावेश <kernel/boot.h>
#समावेश <machine/io.h>
#समावेश <model/statedata.h>
#समावेश <object/interrupt.h>
#समावेश <arch/machine.h>
#समावेश <arch/kernel/boot.h>
#समावेश <arch/kernel/traps.h>
#समावेश <arch/kernel/vspace.h>
#समावेश <arch/benchmark.h>
#समावेश <linker.h>
#समावेश <plat/machine/hardware.h>
#समावेश <machine.h>

#अगर_घोषित ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS अटल अस्थिर _Atomic word_t गाँठ_boot_lock;
#पूर्ण_अगर

BOOT_BSS अटल region_t res_reg[अंक_RESERVED_REGIONS];

BOOT_CODE cap_t create_mapped_it_frame_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t
                                           use_large, bool_t executable)
{
    cap_t cap;
    vm_page_आकार_t frame_आकार_2;

    अगर (use_large) {
        frame_आकार_2 = RISCV_Mega_Page;
    } अन्यथा {
        frame_आकार_2 = RISCV_4K_Page;
    }

    cap = cap_frame_cap_new(
              asid,                            /* capFMappedASID       */
              pptr,                            /* capFBasePtr          */
              frame_आकार_2,                      /* capFSize             */
              wordFromVMRights(VMReadWrite),   /* capFVMRights         */
              0,                               /* capFIsDevice         */
              vptr                             /* capFMappedAddress    */
          );

    map_it_frame_cap(pd_cap, cap);
    वापस cap;
}

BOOT_CODE अटल bool_t arch_init_freemem(region_t ui_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          word_t extra_bi_आकार_bits)
{
    /* Reserve the kernel image region. This may look a bit awkward, as the
     * symbols are a reference in the kernel image window, but all allocations
     * are done in terms of the main kernel window, so we do some translation.
     */
    res_reg[0] = paddr_to_pptr_reg(get_p_reg_kernel_img());
    पूर्णांक सूचकांक = 1;

    /* add the dtb region, if it is not empty */
    अगर (dtb_p_reg.start) {
        अगर (सूचकांक >= ARRAY_आकार(res_reg)) {
            म_लिखो("ERROR: no slot to add DTB to reserved regions\n");
            वापस false;
        }
        res_reg[सूचकांक] = paddr_to_pptr_reg(dtb_p_reg);
        सूचकांक += 1;
    }

    /* reserve the user image region */
    अगर (सूचकांक >= ARRAY_आकार(res_reg)) {
        म_लिखो("ERROR: no slot to add user image to reserved regions\n");
        वापस false;
    }
    res_reg[सूचकांक] = ui_reg;
    सूचकांक += 1;

    /* avail_p_regs comes from the auto-generated code */
    वापस init_freemem(ARRAY_आकार(avail_p_regs), avail_p_regs,
                        सूचकांक, res_reg,
                        it_v_reg, extra_bi_आकार_bits);
}

BOOT_CODE अटल व्योम init_irqs(cap_t मूल_cnode_cap)
{
    irq_t i;

    क्रम (i = 0; i <= maxIRQ; i++) {
        अगर (i != irqInvalid) {
            /* IRQ 0 is irqInvalid */
            setIRQState(IRQInactive, i);
        }
    }
    setIRQState(IRQTimer, KERNEL_TIMER_IRQ);
#अगर_घोषित ENABLE_SMP_SUPPORT
    setIRQState(IRQIPI, irq_remote_call_ipi);
    setIRQState(IRQIPI, irq_reschedule_ipi);
#पूर्ण_अगर
    /* provide the IRQ control cap */
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

/* This and only this function initialises the CPU. It does NOT initialise any kernel state. */

#अगर_घोषित CONFIG_HAVE_FPU
BOOT_CODE अटल व्योम init_fpu(व्योम)
{
    set_fs_clean();
    write_fcsr(0);
    disableFpu();
}
#पूर्ण_अगर

BOOT_CODE अटल व्योम init_cpu(व्योम)
{

    activate_kernel_vspace();
    /* Write trap entry address to stvec */
    write_stvec((word_t)trap_entry);
    initLocalIRQController();
#अगर_अघोषित CONFIG_KERNEL_MCS
    initTimer();
#पूर्ण_अगर

    /* disable FPU access */
    set_fs_off();

#अगर_घोषित CONFIG_HAVE_FPU
    init_fpu();
#पूर्ण_अगर
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_CODE अटल व्योम init_plat(व्योम)
{
    initIRQController();
}


#अगर_घोषित ENABLE_SMP_SUPPORT
BOOT_CODE अटल bool_t try_init_kernel_secondary_core(word_t hart_id, word_t core_id)
{
    जबतक (!गाँठ_boot_lock);

    fence_r_rw();

    init_cpu();
    गाँठ_LOCK_SYS;

    clock_sync_test();
    ks_अंक_CPUs++;
    init_core_state(SchedulerAction_ResumeCurrentThread);
    ifence_local();
    वापस true;
}

BOOT_CODE अटल व्योम release_secondary_cores(व्योम)
{
    निश्चित(0 == गाँठ_boot_lock); /* Sanity check for a proper lock state. */
    गाँठ_boot_lock = 1;
    /* At this point in time the primary core (executing this code) already uses
     * the seL4 MMU/cache setup. However, the secondary cores are still using
     * the elfloader's MMU/cache setup, and thus the update of node_boot_lock
     * may not be visible there if the setups differ. Currently, the mappings
     * match, so a barrier is all that is needed.
     */
    fence_rw_rw();

    जबतक (ks_अंक_CPUs != CONFIG_MAX_अंक_NODES) {
#अगर_घोषित ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        गाँठ_STATE(ksCurTime) = getCurrentTime();
#पूर्ण_अगर
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

अटल BOOT_CODE bool_t try_init_kernel(
    paddr_t ui_p_reg_start,
    paddr_t ui_p_reg_end,
    sword_t pv_offset,
    vptr_t  v_entry,
    paddr_t dtb_phys_addr,
    word_t  dtb_आकार
)
{
    cap_t मूल_cnode_cap;
    cap_t it_pd_cap;
    cap_t it_ap_cap;
    cap_t ipcbuf_cap;
    region_t ui_reg = paddr_to_pptr_reg((p_region_t) {
        ui_p_reg_start, ui_p_reg_end
    });
    word_t extra_bi_आकार = 0;
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

    म_लिखो("Bootstrapping kernel\n");

    /* initialize the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_EMPTY;
    अगर (dtb_आकार > 0) {
#अगर_घोषित CONFIG_PLAT_ROCKETCHIP_ZCU102
        /* The softcore rocketchip instantiation doesn't work well when this
         * page isn't reserved. Round up so the whole page is reserved to
         * avoid the problem
         */
        dtb_आकार = ROUND_UP(dtb_आकार, PAGE_BITS);
#पूर्ण_अगर
        paddr_t dtb_phys_end = dtb_phys_addr + dtb_आकार;
        अगर (dtb_phys_end < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            म_लिखो("ERROR: DTB location at %"SEL4_PRIx_word
                   " len %"SEL4_PRIu_word" invalid\n",
                   dtb_phys_addr, dtb_आकार);
            वापस false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        अगर (dtb_phys_end >= PADDR_शीर्ष) {
            म_लिखो("ERROR: DTB at [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_word")\n",
                   dtb_phys_addr, dtb_phys_end, PADDR_शीर्ष);
            वापस false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_आकार += माप(seL4_BootInfoHeader) + dtb_आकार;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .start = dtb_phys_addr,
            .end   = dtb_phys_end
        };
    }

    /* The region of the initial thread is the user image + ipcbuf + boot info + extra */
    word_t extra_bi_आकार_bits = calculate_extra_bi_आकार_bits(extra_bi_आकार);
    v_region_t it_v_reg = {
        .start = ui_v_reg.start,
        .end   = extra_bi_frame_vptr + (extra_bi_आकार_bits > 0 ? BIT(extra_bi_आकार_bits) : 0)
    };
    अगर (it_v_reg.end >= USER_शीर्ष) {
        /* Variable arguments for printf() require well defined integer types
         * to work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        म_लिखो("ERROR: userland image virt [%"SEL4_PRIx_word"..%"SEL4_PRIx_word")"
               "exceeds USER_TOP (%"SEL4_PRIx_word")\n",
               it_v_reg.start, it_v_reg.end, (word_t)USER_शीर्ष);
        वापस false;
    }

    /* make the free memory available to alloc_region() */
    अगर (!arch_init_freemem(ui_reg, dtb_p_reg, it_v_reg, extra_bi_आकार_bits)) {
        म_लिखो("ERROR: free memory management initialization failed\n");
        वापस false;
    }

    /* create the root cnode */
    मूल_cnode_cap = create_मूल_cnode();
    अगर (cap_get_capType(मूल_cnode_cap) == cap_null_cap) {
        म_लिखो("ERROR: root c-node creation failed\n");
        वापस false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(मूल_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(मूल_cnode_cap);

    /* create the bootinfo frame */
    populate_bi_frame(0, CONFIG_MAX_अंक_NODES, ipcbuf_vptr, extra_bi_आकार);

    /* put DTB in the bootinfo block, if present. */
    seL4_BootInfoHeader header;
    अगर (dtb_आकार > 0) {
        header.id = SEL4_BOOTINFO_HEADER_FDT;
        header.len = माप(header) + dtb_आकार;
        *(seL4_BootInfoHeader *)(rootserver.extra_bi + extra_bi_offset) = header;
        extra_bi_offset += माप(header);
        स_नकल((व्योम *)(rootserver.extra_bi + extra_bi_offset),
               paddr_to_pptr(dtb_phys_addr),
               dtb_आकार);
        extra_bi_offset += dtb_आकार;
    }

    निश्चित(extra_bi_आकार == extra_bi_offset);

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_address_space(मूल_cnode_cap, it_v_reg);
    अगर (cap_get_capType(it_pd_cap) == cap_null_cap) {
        म_लिखो("ERROR: address space creation for initial thread failed\n");
        वापस false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_frame_cap(
        मूल_cnode_cap,
        it_pd_cap,
        bi_frame_vptr
    );

    /* create and map extra bootinfo region */
    अगर (extra_bi_आकार > 0) {
        region_t extra_bi_region = {
            .start = rootserver.extra_bi,
            .end = rootserver.extra_bi + extra_bi_आकार
        };
        extra_bi_ret =
            create_frames_of_region(
                मूल_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_to_paddr((व्योम *)extra_bi_region.start) - extra_bi_frame_vptr
            );
        अगर (!extra_bi_ret.success) {
            म_लिखो("ERROR: mapping extra boot info to initial thread failed\n");
            वापस false;
        }
        ndks_boot.bi_frame->extraBIPages = extra_bi_ret.region;
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    init_sched_control(मूल_cnode_cap, CONFIG_MAX_अंक_NODES);
#पूर्ण_अगर

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_frame_cap(मूल_cnode_cap, it_pd_cap, ipcbuf_vptr);
    अगर (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        म_लिखो("ERROR: could not create IPC buffer for initial thread\n");
        वापस false;
    }

    /* create all userland image frames */
    create_frames_ret =
        create_frames_of_region(
            मूल_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_offset
        );
    अगर (!create_frames_ret.success) {
        म_लिखो("ERROR: could not create all userland image frames\n");
        वापस false;
    }
    ndks_boot.bi_frame->userImageFrames = create_frames_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(मूल_cnode_cap);
    अगर (cap_get_capType(it_ap_cap) == cap_null_cap) {
        म_लिखो("ERROR: could not create ASID pool for initial thread\n");
        वापस false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#अगर_घोषित CONFIG_KERNEL_MCS
    गाँठ_STATE(ksCurTime) = getCurrentTime();
#पूर्ण_अगर

    /* create the idle thread */
    create_idle_thread();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         मूल_cnode_cap,
                         it_pd_cap,
                         v_entry,
                         bi_frame_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    अगर (initial == शून्य) {
        म_लिखो("ERROR: could not create initial thread\n");
        वापस false;
    }

    init_core_state(initial);

    /* convert the remaining free memory into UT objects and provide the caps */
    अगर (!create_untypeds(मूल_cnode_cap)) {
        म_लिखो("ERROR: could not create untypteds for kernel image boot memory\n");
        वापस false;
    }

    /* no shared-frame caps (RISC-V has no multikernel support) */
    ndks_boot.bi_frame->sharedFrames = S_REG_EMPTY;

    /* finalise the bootinfo frame */
    bi_finalise();

    ks_अंक_CPUs = 1;

    SMP_COND_STATEMENT(clh_lock_init());
    SMP_COND_STATEMENT(release_secondary_cores());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    गाँठ_LOCK_SYS;

    म_लिखो("Booting all finished, dropped to user space\n");
    वापस true;
}

BOOT_CODE VISIBLE व्योम init_kernel(
    paddr_t ui_p_reg_start,
    paddr_t ui_p_reg_end,
    sword_t pv_offset,
    vptr_t  v_entry,
    paddr_t dtb_addr_p,
    uint32_t dtb_आकार
#अगर_घोषित ENABLE_SMP_SUPPORT
    ,
    word_t hart_id,
    word_t core_id
#पूर्ण_अगर
)
{
    bool_t परिणाम;

#अगर_घोषित ENABLE_SMP_SUPPORT
    add_hart_to_core_map(hart_id, core_id);
    अगर (core_id == 0) {
        परिणाम = try_init_kernel(ui_p_reg_start,
                                 ui_p_reg_end,
                                 pv_offset,
                                 v_entry,
                                 dtb_addr_p,
                                 dtb_आकार);
    } अन्यथा {
        परिणाम = try_init_kernel_secondary_core(hart_id, core_id);
    }
#अन्यथा
    परिणाम = try_init_kernel(ui_p_reg_start,
                             ui_p_reg_end,
                             pv_offset,
                             v_entry,
                             dtb_addr_p,
                             dtb_आकार);
#पूर्ण_अगर
    अगर (!परिणाम) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    गाँठ_STATE(ksCurTime) = getCurrentTime();
    गाँठ_STATE(ksConsumed) = 0;
#पूर्ण_अगर

    schedule();
    activateThread();
}
