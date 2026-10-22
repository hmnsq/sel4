/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <kernel/boot.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <object/interrupt.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/kernel/boot.h>
#अन्तर्भाव <arch/kernel/traps.h>
#अन्तर्भाव <arch/kernel/vspace.h>
#अन्तर्भाव <arch/benchmark.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/hardware.h>
#अन्तर्भाव <machine.h>

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS स्थिर चञ्चल _Atomic word_t ग्रन्थिः_boot_तालकम्;
#यदि_अन्तः

BOOT_BSS स्थिर region_t res_reg[अङ्कः_RESERVED_REGIONS];

BOOT_संहिता cap_t create_mapped_it_frame_cap(cap_t pd_cap, pptr_t pptr, vptr_t vptr, asid_t asid, bool_t
                                           use_large, bool_t executable)
{
    cap_t cap;
    vm_page_आकार_t frame_आकार_2;

    यदि (use_large) {
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
    प्रत्यागम cap;
}

BOOT_संहिता स्थिर bool_t arch_init_freemem(region_t ui_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          word_t extra_bi_आकार_द्वयङ्काः)
{
    /* Reserve the kernel image region. This may look a bit awkward, as the
     * symbols are a reference in the kernel image window, but all allocations
     * are done in terms of the main kernel window, so we do some translation.
     */
    res_reg[0] = paddr_गन्तव्यम्_pptr_reg(get_p_reg_kernel_img());
    पूर्णाङ्क सूचकांक = 1;

    /* add the dtb region, if it is not empty */
    यदि (dtb_p_reg.start) {
        यदि (सूचकांक >= ARRAY_आकार(res_reg)) {
            प्रारूप_लिख("ERROR: no slot to add DTB to reserved regions\n");
            प्रत्यागम false;
        }
        res_reg[सूचकांक] = paddr_गन्तव्यम्_pptr_reg(dtb_p_reg);
        सूचकांक += 1;
    }

    /* reserve the user image region */
    यदि (सूचकांक >= ARRAY_आकार(res_reg)) {
        प्रारूप_लिख("ERROR: no slot to add user image to reserved regions\n");
        प्रत्यागम false;
    }
    res_reg[सूचकांक] = ui_reg;
    सूचकांक += 1;

    /* avail_p_regs comes from the auto-generated code */
    प्रत्यागम init_freemem(ARRAY_आकार(avail_p_regs), avail_p_regs,
                        सूचकांक, res_reg,
                        it_v_reg, extra_bi_आकार_द्वयङ्काः);
}

BOOT_संहिता स्थिर शून्य init_irqs(cap_t मूलम्_cnode_cap)
{
    irq_t i;

    प्रति (i = 0; i <= maxIRQ; i++) {
        यदि (i != irqInvalid) {
            /* IRQ 0 is irqInvalid */
            समुच्चयः_IRQState(IRQInactive, i);
        }
    }
    समुच्चयः_IRQState(IRQTimer, KERNEL_TIMER_IRQ);
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    समुच्चयः_IRQState(IRQIPI, irq_remote_call_ipi);
    समुच्चयः_IRQState(IRQIPI, irq_reschedule_ipi);
#यदि_अन्तः
    /* provide the IRQ control cap */
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

/* This and only this function initialises the CPU. It does NOT initialise any kernel state. */

#यदि_परिभाषितम् CONFIG_HAVE_FPU
BOOT_संहिता स्थिर शून्य init_fpu(शून्य)
{
    समुच्चयः_fs_clean();
    write_fcsr(0);
    disableFpu();
}
#यदि_अन्तः

BOOT_संहिता स्थिर शून्य init_cpu(शून्य)
{

    activate_kernel_vspace();
    /* Write trap entry address to stvec */
    write_stvec((word_t)trap_प्रविष्टिः);
    initLocalIRQController();
#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
    initTimer();
#यदि_अन्तः

    /* disable FPU access */
    समुच्चयः_fs_off();

#यदि_परिभाषितम् CONFIG_HAVE_FPU
    init_fpu();
#यदि_अन्तः
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_संहिता स्थिर शून्य init_plat(शून्य)
{
    initIRQController();
}


#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
BOOT_संहिता स्थिर bool_t try_init_kernel_secondary_core(word_t hart_id, word_t core_id)
{
    यावत् (!ग्रन्थिः_boot_तालकम्);

    fence_r_rw();

    init_cpu();
    ग्रन्थिः_तालकम्_SYS;

    clock_sync_test();
    ks_अङ्कः_CPUs++;
    init_core_state(SchedulerAction_ResumeCurrentThread);
    ifence_local();
    प्रत्यागम true;
}

BOOT_संहिता स्थिर शून्य release_secondary_cores(शून्य)
{
    प्रतिज्ञा(0 == ग्रन्थिः_boot_तालकम्); /* Sanity check for a proper lock state. */
    ग्रन्थिः_boot_तालकम् = 1;
    /* At this point in time the primary core (executing this code) already uses
     * the seL4 MMU/cache setup. However, the secondary cores are still using
     * the elfloader's MMU/cache setup, and thus the update of node_boot_lock
     * may not be visible there if the setups differ. Currently, the mappings
     * match, so a barrier is all that is needed.
     */
    fence_rw_rw();

    यावत् (ks_अङ्कः_CPUs != CONFIG_MAX_अङ्कः_NODES) {
#यदि_परिभाषितम् ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        ग्रन्थिः_STATE(ksCurTime) = getCurrentTime();
#यदि_अन्तः
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

स्थिर BOOT_संहिता bool_t try_init_kernel(
    paddr_t ui_p_reg_start,
    paddr_t ui_p_reg_अन्तः,
    sword_t pv_विस्थापनम्,
    vptr_t  v_प्रविष्टिः,
    paddr_t dtb_phys_addr,
    word_t  dtb_आकार
)
{
    cap_t मूलम्_cnode_cap;
    cap_t it_pd_cap;
    cap_t it_ap_cap;
    cap_t ipcbuf_cap;
    region_t ui_reg = paddr_गन्तव्यम्_pptr_reg((p_region_t) {
        ui_p_reg_start, ui_p_reg_अन्तः
    });
    word_t extra_bi_आकार = 0;
    pptr_t extra_bi_विस्थापनम् = 0;
    vptr_t extra_bi_frame_vptr;
    vptr_t bi_frame_vptr;
    vptr_t ipcbuf_vptr;
    create_चौकटाः_सम्बन्धि_region_ret_t create_चौकटाः_ret;
    create_चौकटाः_सम्बन्धि_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .start = ui_p_reg_start - pv_विस्थापनम्,
        .अन्तः_2   = ui_p_reg_अन्तः   - pv_विस्थापनम्
    };

    ipcbuf_vptr = ui_v_reg.अन्तः_2;
    bi_frame_vptr = ipcbuf_vptr + BIT(PAGE_द्वयङ्काः);
    extra_bi_frame_vptr = bi_frame_vptr + BIT(se_L4_Boot_Info_Frame_द्वयङ्काः);

    map_kernel_गवाक्षः();

    /* initialise the CPU */
    init_cpu();

    प्रारूप_लिख("Bootstrapping kernel\n");

    /* initialize the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_रिक्तानि;
    यदि (dtb_आकार > 0) {
#यदि_परिभाषितम् CONFIG_PLAT_ROCKETCHIP_ZCU102
        /* The softcore rocketchip instantiation doesn't work well when this
         * page isn't reserved. Round up so the whole page is reserved to
         * avoid the problem
         */
        dtb_आकार = आवर्तनम्_ऊर्ध्वम्(dtb_आकार, PAGE_द्वयङ्काः);
#यदि_अन्तः
        paddr_t dtb_phys_अन्तः = dtb_phys_addr + dtb_आकार;
        यदि (dtb_phys_अन्तः < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            प्रारूप_लिख("ERROR: DTB location at %"SEL4_PRIx_word
                   " len %"SEL4_PRIu_word" invalid\n",
                   dtb_phys_addr, dtb_आकार);
            प्रत्यागम false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        यदि (dtb_phys_अन्तः >= PADDR_शिखरम्) {
            प्रारूप_लिख("ERROR: DTB at [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_word")\n",
                   dtb_phys_addr, dtb_phys_अन्तः, PADDR_शिखरम्);
            प्रत्यागम false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_आकार += परिमाण(se_L4_Boot_Info_शीर्षकम्) + dtb_आकार;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .start = dtb_phys_addr,
            .अन्तः_2   = dtb_phys_अन्तः
        };
    }

    /* The region of the initial thread is the user image + ipcbuf + boot info + extra */
    word_t extra_bi_आकार_द्वयङ्काः = calculate_extra_bi_आकार_द्वयङ्काः(extra_bi_आकार);
    v_region_t it_v_reg = {
        .start = ui_v_reg.start,
        .अन्तः_2   = extra_bi_frame_vptr + (extra_bi_आकार_द्वयङ्काः > 0 ? BIT(extra_bi_आकार_द्वयङ्काः) : 0)
    };
    यदि (it_v_reg.अन्तः_2 >= USER_शिखरम्) {
        /* Variable arguments for printf() require well defined integer types
         * to work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        प्रारूप_लिख("ERROR: userland image virt [%"SEL4_PRIx_word"..%"SEL4_PRIx_word")"
               "exceeds USER_TOP (%"SEL4_PRIx_word")\n",
               it_v_reg.start, it_v_reg.अन्तः_2, (word_t)USER_शिखरम्);
        प्रत्यागम false;
    }

    /* make the free memory available to alloc_region() */
    यदि (!arch_init_freemem(ui_reg, dtb_p_reg, it_v_reg, extra_bi_आकार_द्वयङ्काः)) {
        प्रारूप_लिख("ERROR: free memory management initialization failed\n");
        प्रत्यागम false;
    }

    /* create the root cnode */
    मूलम्_cnode_cap = create_मूलम्_cnode();
    यदि (cap_get_capType(मूलम्_cnode_cap) == cap_null_cap) {
        प्रारूप_लिख("ERROR: root c-node creation failed\n");
        प्रत्यागम false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(मूलम्_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(मूलम्_cnode_cap);

    /* create the bootinfo frame */
    populate_bi_frame(0, CONFIG_MAX_अङ्कः_NODES, ipcbuf_vptr, extra_bi_आकार);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_शीर्षकम् शीर्षकम्;
    यदि (dtb_आकार > 0) {
        शीर्षकम्.id = SEL4_BOOTINFO_शीर्षकम्_FDT;
        शीर्षकम्.len = परिमाण(शीर्षकम्) + dtb_आकार;
        *(se_L4_Boot_Info_शीर्षकम् *)(rootserver.extra_bi + extra_bi_विस्थापनम्) = शीर्षकम्;
        extra_bi_विस्थापनम् += परिमाण(शीर्षकम्);
        स्मृतिं_प्रतिलिख((शून्य *)(rootserver.extra_bi + extra_bi_विस्थापनम्),
               paddr_गन्तव्यम्_pptr(dtb_phys_addr),
               dtb_आकार);
        extra_bi_विस्थापनम् += dtb_आकार;
    }

    प्रतिज्ञा(extra_bi_आकार == extra_bi_विस्थापनम्);

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_स्थानाङ्कः_space(मूलम्_cnode_cap, it_v_reg);
    यदि (cap_get_capType(it_pd_cap) == cap_null_cap) {
        प्रारूप_लिख("ERROR: address space creation for initial thread failed\n");
        प्रत्यागम false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_frame_cap(
        मूलम्_cnode_cap,
        it_pd_cap,
        bi_frame_vptr
    );

    /* create and map extra bootinfo region */
    यदि (extra_bi_आकार > 0) {
        region_t extra_bi_region = {
            .start = rootserver.extra_bi,
            .अन्तः_2 = rootserver.extra_bi + extra_bi_आकार
        };
        extra_bi_ret =
            create_चौकटाः_सम्बन्धि_region(
                मूलम्_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_गन्तव्यम्_paddr((शून्य *)extra_bi_region.start) - extra_bi_frame_vptr
            );
        यदि (!extra_bi_ret.success) {
            प्रारूप_लिख("ERROR: mapping extra boot info to initial thread failed\n");
            प्रत्यागम false;
        }
        ndks_boot.bi_frame->extraBIPages = extra_bi_ret.region;
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    init_sched_control(मूलम्_cnode_cap, CONFIG_MAX_अङ्कः_NODES);
#यदि_अन्तः

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_frame_cap(मूलम्_cnode_cap, it_pd_cap, ipcbuf_vptr);
    यदि (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        प्रारूप_लिख("ERROR: could not create IPC buffer for initial thread\n");
        प्रत्यागम false;
    }

    /* create all userland image frames */
    create_चौकटाः_ret =
        create_चौकटाः_सम्बन्धि_region(
            मूलम्_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_विस्थापनम्
        );
    यदि (!create_चौकटाः_ret.success) {
        प्रारूप_लिख("ERROR: could not create all userland image frames\n");
        प्रत्यागम false;
    }
    ndks_boot.bi_frame->user_Image_चौकटाः = create_चौकटाः_ret.region;

    /* create the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(मूलम्_cnode_cap);
    यदि (cap_get_capType(it_ap_cap) == cap_null_cap) {
        प्रारूप_लिख("ERROR: could not create ASID pool for initial thread\n");
        प्रत्यागम false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    ग्रन्थिः_STATE(ksCurTime) = getCurrentTime();
#यदि_अन्तः

    /* create the idle thread */
    create_idle_thread();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         मूलम्_cnode_cap,
                         it_pd_cap,
                         v_प्रविष्टिः,
                         bi_frame_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    यदि (initial == रिक्त) {
        प्रारूप_लिख("ERROR: could not create initial thread\n");
        प्रत्यागम false;
    }

    init_core_state(initial);

    /* convert the remaining free memory into UT objects and provide the caps */
    यदि (!create_untypeds(मूलम्_cnode_cap)) {
        प्रारूप_लिख("ERROR: could not create untypteds for kernel image boot memory\n");
        प्रत्यागम false;
    }

    /* no shared-frame caps (RISC-V has no multikernel support) */
    ndks_boot.bi_frame->shared_चौकटाः = S_REG_रिक्तानि;

    /* finalise the bootinfo frame */
    bi_finalise();

    ks_अङ्कः_CPUs = 1;

    SMP_COND_STATEMENT(clh_तालकम्_init());
    SMP_COND_STATEMENT(release_secondary_cores());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    ग्रन्थिः_तालकम्_SYS;

    प्रारूप_लिख("Booting all finished, dropped to user space\n");
    प्रत्यागम true;
}

BOOT_संहिता VISIBLE शून्य init_kernel(
    paddr_t ui_p_reg_start,
    paddr_t ui_p_reg_अन्तः,
    sword_t pv_विस्थापनम्,
    vptr_t  v_प्रविष्टिः,
    paddr_t dtb_addr_p,
    uint32_t dtb_आकार
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    ,
    word_t hart_id,
    word_t core_id
#यदि_अन्तः
)
{
    bool_t परिणाम;

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    सङ्कलय_hart_गन्तव्यम्_core_map(hart_id, core_id);
    यदि (core_id == 0) {
        परिणाम = try_init_kernel(ui_p_reg_start,
                                 ui_p_reg_अन्तः,
                                 pv_विस्थापनम्,
                                 v_प्रविष्टिः,
                                 dtb_addr_p,
                                 dtb_आकार);
    } अन्यथा {
        परिणाम = try_init_kernel_secondary_core(hart_id, core_id);
    }
#अन्यथा
    परिणाम = try_init_kernel(ui_p_reg_start,
                             ui_p_reg_अन्तः,
                             pv_विस्थापनम्,
                             v_प्रविष्टिः,
                             dtb_addr_p,
                             dtb_आकार);
#यदि_अन्तः
    यदि (!परिणाम) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    ग्रन्थिः_STATE(ksCurTime) = getCurrentTime();
    ग्रन्थिः_STATE(ksConsumed) = 0;
#यदि_अन्तः

    कालसारणी();
    activateThread();
}
