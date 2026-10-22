/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <kernel/boot.h>
#অন্তর্ভাৱ <machine/io.h>
#অন্তর্ভাৱ <model/statedata.h>
#অন্তর্ভাৱ <object/interrupt.h>
#অন্তর্ভাৱ <arch/machine.h>
#অন্তর্ভাৱ <arch/kernel/boot.h>
#অন্তর্ভাৱ <arch/kernel/vspace.h>
#অন্তর্ভাৱ <arch/benchmark.h>
#অন্তর্ভাৱ <arch/user_access.h>
#অন্তর্ভাৱ <arch/object/iospace.h>
#অন্তর্ভাৱ <linker.h>
#অন্তর্ভাৱ <plat/machine/hardware.h>
#অন্তর্ভাৱ <machine.h>
#অন্তর্ভাৱ <arch/machine/timer.h>
#অন্তর্ভাৱ <arch/machine/fpu.h>
#অন্তর্ভাৱ <arch/machine/tlb.h>

#ifdef CONFIG_ARM_SMMU
#অন্তর্ভাৱ <drivers/smmu/smmuv2.h>
#endif

#ifdef ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS স্থির চঞ্চল _Atomic পূর্ণাঙ্ক গ্ৰন্থি_boot_lock;
#endif /* ENABLE_SMP_SUPPORT */

BOOT_BSS স্থির region_t reserved[অঙ্ক_RESERVED_REGIONS];

BOOT_CODE স্থির bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          word_t extra_bi_size_bits)
{
    /* reserve the kernel image region */
    reserved[0] = paddr_to_pptr_reg(get_p_reg_kernel_img());

    পূর্ণাঙ্ক index = 1;

    /* add the dtb region, if it is not empty */
    যদি (dtb_p_reg.start) {
        যদি (index >= ARRAY_SIZE(reserved)) {
            printf("ERROR: no slot to add DTB to reserved regions\n");
            ঘূৰাই false;
        }
        reserved[index].start = (pptr_t) paddr_to_pptr(dtb_p_reg.start);
        reserved[index].end = (pptr_t) paddr_to_pptr(dtb_p_reg.end);
        index++;
    }

    /* Reserve the user image region and the mode-reserved regions. For now,
     * only one mode-reserved region is supported, because this is all that is
     * needed.
     */
    যদি (MODE_RESERVED > 1) {
        printf("ERROR: MODE_RESERVED > 1 unsupported!\n");
        ঘূৰাই false;
    }
    যদি (ui_p_reg.start < PADDR_শীৰ্ষ) {
        region_t ui_reg = paddr_to_pptr_reg(ui_p_reg);
        যদি (MODE_RESERVED == 1) {
            যদি (index + 1 >= ARRAY_SIZE(reserved)) {
                printf("ERROR: no slot to add the user image and the "
                       "mode-reserved region to the reserved regions\n");
                ঘূৰাই false;
            }
            যদি (ui_reg.end > mode_reserved_region[0].start) {
                reserved[index] = mode_reserved_region[0];
                index++;
                reserved[index] = ui_reg;
            } অন্যথা {
                reserved[index] = ui_reg;
                index++;
                reserved[index] = mode_reserved_region[0];
            }
            index++;
        } অন্যথা {
            যদি (index >= ARRAY_SIZE(reserved)) {
                printf("ERROR: no slot to add the user image to the reserved"
                       "regions\n");
                ঘূৰাই false;
            }
            reserved[index] = ui_reg;
            index++;
        }
    } অন্যথা {
        যদি (MODE_RESERVED == 1) {
            যদি (index >= ARRAY_SIZE(reserved)) {
                printf("ERROR: no slot to add the mode-reserved region\n");
                ঘূৰাই false;
            }
            reserved[index] = mode_reserved_region[0];
            index++;
        }

        /* Reserve the ui_p_reg region still so it doesn't get turned into device UT. */
        reserve_region(ui_p_reg);
    }

    /* avail_p_regs comes from the auto-generated code */
    ঘূৰাই init_freemem(ARRAY_SIZE(avail_p_regs), avail_p_regs,
                        index, reserved,
                        it_v_reg, extra_bi_size_bits);
}


BOOT_CODE স্থির শূন্য init_irqs(cap_t মূল_cnode_cap)
{
    unsigned i;

    বাবে (i = 0; i <= maxIRQ ; i++) {
        setIRQState(IRQInactive, CORE_IRQ_TO_IRQT(0, i));
    }
    setIRQState(IRQTimer, CORE_IRQ_TO_IRQT(0, KERNEL_TIMER_IRQ));
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    setIRQState(IRQReserved, CORE_IRQ_TO_IRQT(0, INTERRUPT_VGIC_MAINTENANCE));
    setIRQState(IRQReserved, CORE_IRQ_TO_IRQT(0, INTERRUPT_VTIMER_EVENT));
#endif
#ifdef CONFIG_TK1_SMMU
    setIRQState(IRQReserved, CORE_IRQ_TO_IRQT(0, INTERRUPT_SMMU));
#endif

#ifdef CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT
#ifdef KERNEL_PMU_IRQ
    setIRQState(IRQReserved, CORE_IRQ_TO_IRQT(0, KERNEL_PMU_IRQ));
#যদি (defined CONFIG_PLAT_TX1 && defined ENABLE_SMP_SUPPORT)
//SELFOUR-1252
#error "This platform doesn't support tracking CPU utilisation on multicore"
#endif /* CONFIG_PLAT_TX1 && ENABLE_SMP_SUPPORT */
#অন্যথা
#error "This platform doesn't support tracking CPU utilisation feature"
#endif /* KERNEL_TIMER_IRQ */
#endif /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#ifdef ENABLE_SMP_SUPPORT
    setIRQState(IRQIPI, CORE_IRQ_TO_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    setIRQState(IRQIPI, CORE_IRQ_TO_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
#endif

    /* provide the IRQ control cap */
    write_slot(SLOT_PTR(pptr_of_cap(মূল_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

#ifdef CONFIG_ARM_SMMU
BOOT_CODE স্থির শূন্য init_smmu(cap_t মূল_cnode_cap)
{
    plat_smmu_init();
    /*provide the SID and CB control cap*/
    write_slot(SLOT_PTR(pptr_of_cap(মূল_cnode_cap), seL4_CapSMMUSIDControl), cap_sid_control_cap_new());
    write_slot(SLOT_PTR(pptr_of_cap(মূল_cnode_cap), seL4_CapSMMUCBControl), cap_cb_control_cap_new());
}

#endif

#ifdef CONFIG_ALLOW_SMC_CALLS
BOOT_CODE স্থির শূন্য init_smc(cap_t মূল_cnode_cap)
{
    /* Provide the SMC cap*/
    write_slot(SLOT_PTR(pptr_of_cap(মূল_cnode_cap), seL4_CapSMC), cap_smc_cap_new(0));
}
#endif

/** This and only this function initialises the CPU.
 *
 * It does NOT initialise any kernel state.
 * @return For the verification build, this currently returns true always.
 */
BOOT_CODE স্থির bool_t init_cpu(শূন্য)
{
    bool_t haveHWFPU;

#ifdef CONFIG_ARCH_AARCH64
    যদি (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        যদি (!checkTCR_EL2()) {
            ঘূৰাই false;
        }
    }
#endif

    activate_kernel_vspace();
    যদি (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_boot_init();
    }

#ifdef CONFIG_ARCH_AARCH64
    /* After activate_kernel_vspace(), so printf() is available. */
    যদি (!config_set(CONFIG_ARM_HYPERVISOR_SUPPORT) && !check16BitASID()) {
        printf("ERROR: kernel requires 16-bit ASIDs\n");
        ঘূৰাই false;
    }
#endif

#ifdef CONFIG_HARDWARE_DEBUG_API
    যদি (!Arch_initHardwareBreakpoints()) {
        printf("Kernel built with CONFIG_HARDWARE_DEBUG_API, but this board doesn't "
               "reliably support it.\n");
        ঘূৰাই false;
    }
#endif

    /* Setup kernel stack pointer.
     * On ARM SMP, the array index here is the CPU ID
     */
    word_t স্তূপ_শীৰ্ষ = ((word_t) kernel_স্তূপ_alloc[CURRENT_CPU_INDEX()]) + BIT(CONFIG_KERNEL_স্তূপ_BITS);
#ifdef ENABLE_SMP_SUPPORT
#ifdef CONFIG_ARCH_AARCH64
    /* the least 12 bits are used to store logical core ID */
    স্তূপ_শীৰ্ষ |= getCurrentCPUIndex();
#নোচেদ্যদি defined(CONFIG_ARCH_AARCH32)
    /* Stack address encodes core ID, ensure it is in the right region */
    স্তূপ_শীৰ্ষ -= 8;
#endif
#endif
    set_Kernel_স্তূপ(স্তূপ_শীৰ্ষ);

#ifdef CONFIG_ARCH_AARCH64
    /* initialise CPU's exception vector table */
    setVtable((pptr_t)arm_vector_table);
#endif /* CONFIG_ARCH_AARCH64 */

    haveHWFPU = fpsimd_HWCapTest();

    /* Disable FPU to avoid channels where a platform has an FPU but doesn't make use of it */
    যদি (haveHWFPU) {
        disableFpu();
    }

#ifdef CONFIG_HAVE_FPU
    যদি (haveHWFPU) {
        যদি (!fpsimd_init()) {
            ঘূৰাই false;
        }
    } অন্যথা {
        printf("Platform claims to have FP hardware, but does not!\n");
        ঘূৰাই false;
    }
#endif /* CONFIG_HAVE_FPU */

    cpu_initLocalIRQController();

#ifdef CONFIG_ENABLE_BENCHMARKS
    arm_init_ccnt();
#endif /* CONFIG_ENABLE_BENCHMARKS */

    /* Export selected CPU features for access by PL0 */
    armv_init_user_access();

    initTimer();

    ঘূৰাই true;
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_CODE স্থির শূন্য init_plat(শূন্য)
{
    initIRQController();
    initL2Cache();
#ifdef CONFIG_ARM_SMMU
    plat_smmu_init();
#endif
}

#ifdef ENABLE_SMP_SUPPORT
BOOT_CODE স্থির bool_t try_init_kernel_secondary_core(শূন্য)
{
    /* need to first wait until some kernel init has been done */
    যেতিয়ালৈ (!গ্ৰন্থি_boot_lock);

    /* Perform cpu init */
    init_cpu();

    বাবে (unsigned পূর্ণাঙ্ক i = 0; i < অঙ্ক_PPI; i++) {
        maskInterrupt(true, CORE_IRQ_TO_IRQT(getCurrentCPUIndex(), i));
    }
    setIRQState(IRQIPI, CORE_IRQ_TO_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    setIRQState(IRQIPI, CORE_IRQ_TO_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
    /* Enable per-CPU timer interrupts */
    setIRQState(IRQTimer, CORE_IRQ_TO_IRQT(getCurrentCPUIndex(), KERNEL_TIMER_IRQ));
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    setIRQState(IRQReserved, CORE_IRQ_TO_IRQT(getCurrentCPUIndex(), INTERRUPT_VGIC_MAINTENANCE));
    setIRQState(IRQReserved, CORE_IRQ_TO_IRQT(getCurrentCPUIndex(), INTERRUPT_VTIMER_EVENT));
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    গ্ৰন্থি_LOCK_SYS;

    clock_sync_test();
    ks_অঙ্ক_CPUs++;

    init_core_state(SchedulerAction_ResumeCurrentThread);

    ঘূৰাই true;
}

BOOT_CODE স্থির শূন্য release_secondary_cpus(শূন্য)
{
    /* release the cpus at the same time */
    প্রতিজ্ঞা(0 == গ্ৰন্থি_boot_lock); /* Sanity check for a proper lock state. */
    গ্ৰন্থি_boot_lock = 1;

    /*
     * At this point in time the primary core (executing this code) already uses
     * the seL4 MMU/cache setup. However, the secondary cores are still using
     * the elfloader's MMU/cache setup, and thus any memory updates may not
     * be visible there.
     *
     * On AARCH64, both elfloader and seL4 map memory inner shareable and have
     * the caches enabled, so no explicit cache maintenance is necessary.
     *
     * On AARCH32 the elfloader uses strongly ordered uncached memory, but seL4
     * has caching enabled, thus explicit cache cleaning is required.
     */
#ifdef CONFIG_ARCH_AARCH32
    cleanInvalidateL1Caches();
    plat_cleanInvalidateL2Cache();
#endif

    /* Wait until all the secondary cores are done initialising */
    যেতিয়ালৈ (ks_অঙ্ক_CPUs != CONFIG_MAX_অঙ্ক_NODES) {
#ifdef ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        গ্ৰন্থি_STATE(ksCurTime) = getCurrentTime();
#endif
        /* perform a memory acquire to get new values of ksNumCPUs, release for ksCurTime */
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
    word_t  dtb_size
)
{
    cap_t মূল_cnode_cap;
    cap_t it_ap_cap;
    cap_t it_pd_cap;
    cap_t ipcbuf_cap;
    p_region_t ui_p_reg = (p_region_t) {
        ui_p_reg_start, ui_p_reg_end
    };
    region_t ui_reg = paddr_to_pptr_reg(ui_p_reg);
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

    /* setup virtual memory for the kernel */
    map_kernel_window();

    /* initialise the CPU */
    যদি (!init_cpu()) {
        printf("ERROR: CPU init failed\n");
        ঘূৰাই false;
    }

    /* debug output via serial port is only available from here */
    printf("Bootstrapping kernel\n");

    /* initialise the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_EMPTY;
    যদি (dtb_size > 0) {
        paddr_t dtb_phys_end = dtb_phys_addr + dtb_size;
        যদি (dtb_phys_end < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            printf("ERROR: DTB location at %"SEL4_PRIx_word
                   " len %"SEL4_PRIu_word" invalid\n",
                   dtb_phys_addr, dtb_size);
            ঘূৰাই false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        যদি (dtb_phys_end >= PADDR_শীৰ্ষ) {
            printf("ERROR: DTB at [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_word")\n",
                   dtb_phys_addr, dtb_phys_end, PADDR_শীৰ্ষ);
            ঘূৰাই false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_size += পরিমাণ(seL4_BootInfoHeader) + dtb_size;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .start = dtb_phys_addr,
            .end   = dtb_phys_end
        };
    }

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    word_t extra_bi_size_bits = calculate_extra_bi_size_bits(extra_bi_size);
    v_region_t it_v_reg = {
        .start = ui_v_reg.start,
        .end   = extra_bi_frame_vptr + (extra_bi_size_bits > 0 ? BIT(extra_bi_size_bits) : 0)
    };
    যদি (it_v_reg.end >= USER_শীৰ্ষ) {
        /* Variable arguments for printf() require well defined integer types to
         * work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        printf("ERROR: userland image virt [%"SEL4_PRIx_word"..%"SEL4_PRIx_word")"
               "exceeds USER_TOP (%"SEL4_PRIx_word")\n",
               it_v_reg.start, it_v_reg.end, (word_t)USER_শীৰ্ষ);
        ঘূৰাই false;
    }

    যদি (!arch_init_freemem(ui_p_reg, dtb_p_reg, it_v_reg, extra_bi_size_bits)) {
        printf("ERROR: free memory management initialization failed\n");
        ঘূৰাই false;
    }

    /* create the root cnode */
    মূল_cnode_cap = create_মূল_cnode();
    যদি (cap_get_capType(মূল_cnode_cap) == cap_null_cap) {
        printf("ERROR: root c-node creation failed\n");
        ঘূৰাই false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(মূল_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(মূল_cnode_cap);

#ifdef CONFIG_ARM_SMMU
    /* initialise the SMMU and provide the SMMU control caps*/
    init_smmu(মূল_cnode_cap);
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    init_smc(মূল_cnode_cap);
#endif

    populate_bi_frame(0, CONFIG_MAX_অঙ্ক_NODES, ipcbuf_vptr, extra_bi_size);

    /* put DTB in the bootinfo block, if present. */
    seL4_BootInfoHeader header;
    যদি (dtb_size > 0) {
        header.id = SEL4_BOOTINFO_HEADER_FDT;
        header.len = পরিমাণ(header) + dtb_size;
        *(seL4_BootInfoHeader *)(rootserver.extra_bi + extra_bi_offset) = header;
        extra_bi_offset += পরিমাণ(header);
        memcpy((শূন্য *)(rootserver.extra_bi + extra_bi_offset),
               paddr_to_pptr(dtb_phys_addr),
               dtb_size);
        extra_bi_offset += dtb_size;
    }

    প্রতিজ্ঞা(extra_bi_size == extra_bi_offset);

    যদি (config_set(CONFIG_TK1_SMMU)) {
        ndks_boot.bi_frame->ioSpaceCaps = create_iospace_caps(মূল_cnode_cap);
        যদি (ndks_boot.bi_frame->ioSpaceCaps.start == 0 &&
            ndks_boot.bi_frame->ioSpaceCaps.end == 0) {
            printf("ERROR: SMMU I/O space creation failed\n");
            ঘূৰাই false;
        }
    } অন্যথা {
        ndks_boot.bi_frame->ioSpaceCaps = S_REG_EMPTY;
    }

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_address_space(মূল_cnode_cap, it_v_reg);
    যদি (cap_get_capType(it_pd_cap) == cap_null_cap) {
        printf("ERROR: address space creation for initial thread failed\n");
        ঘূৰাই false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_frame_cap(
        মূল_cnode_cap,
        it_pd_cap,
        bi_frame_vptr
    );

    /* create and map extra bootinfo region */
    যদি (extra_bi_size > 0) {
        region_t extra_bi_region = {
            .start = rootserver.extra_bi,
            .end = rootserver.extra_bi + extra_bi_size
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
            ঘূৰাই false;
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
        ঘূৰাই false;
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
        ঘূৰাই false;
    }
    ndks_boot.bi_frame->userImageFrames = create_frames_ret.region;

    /* create/initialise the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(মূল_cnode_cap);
    যদি (cap_get_capType(it_ap_cap) == cap_null_cap) {
        printf("ERROR: could not create ASID pool for initial thread\n");
        ঘূৰাই false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#ifdef CONFIG_KERNEL_MCS
    গ্ৰন্থি_STATE(ksCurTime) = getCurrentTime();
#endif

    /* create the idle thread */
    create_idle_thread();

    /* Before creating the initial thread (which also switches to it)
     * we clean the cache so that any page table information written
     * as a result of calling create_frames_of_region will be correctly
     * read by the hardware page table walker */
    cleanInvalidateL1Caches();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         মূল_cnode_cap,
                         it_pd_cap,
                         v_entry,
                         bi_frame_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    যদি (initial == রিক্ত) {
        printf("ERROR: could not create initial thread\n");
        ঘূৰাই false;
    }

    init_core_state(initial);

    /* create all of the untypeds. Both devices and kernel window memory */
    যদি (!create_untypeds(মূল_cnode_cap)) {
        printf("ERROR: could not create untypteds for kernel image boot memory\n");
        ঘূৰাই false;
    }

    /* no shared-frame caps (ARM has no multikernel support) */
    ndks_boot.bi_frame->sharedFrames = S_REG_EMPTY;

    /* finalise the bootinfo frame */
    bi_finalise();

    /* Flushing the L1 cache and invalidating the TLB is good enough here to
     * make sure everything written by the kernel is visible to userland. There
     * are no uncached userland frames at this stage that require enforcing
     * flushing to RAM. Any retyping operation will clean the memory down to RAM
     * anyway.
     */
    cleanInvalidateL1Caches();
    invalidateLocalTLB();
    যদি (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        invalidateHypTLB();
    }

    ks_অঙ্ক_CPUs = 1;

    /* initialize BKL before booting up other cores */
    SMP_COND_STATEMENT(clh_lock_init());
    SMP_COND_STATEMENT(release_secondary_cpus());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    গ্ৰন্থি_LOCK_SYS;

    printf("Booting all finished, dropped to user space\n");

    /* kernel successfully initialized */
    ঘূৰাই true;
}

BOOT_CODE VISIBLE শূন্য init_kernel(
    paddr_t ui_p_reg_start,
    paddr_t ui_p_reg_end,
    sword_t pv_offset,
    vptr_t  v_entry,
    paddr_t dtb_addr_p,
    uint32_t dtb_size
)
{
    bool_t result;

#ifdef ENABLE_SMP_SUPPORT
    /* we assume there exists a cpu with id 0 and will use it for bootstrapping */
    যদি (getCurrentCPUIndex() == 0) {
        result = try_init_kernel(ui_p_reg_start,
                                 ui_p_reg_end,
                                 pv_offset,
                                 v_entry,
                                 dtb_addr_p, dtb_size);
    } অন্যথা {
        result = try_init_kernel_secondary_core();
    }

#অন্যথা
    result = try_init_kernel(ui_p_reg_start,
                             ui_p_reg_end,
                             pv_offset,
                             v_entry,
                             dtb_addr_p, dtb_size);

#endif /* ENABLE_SMP_SUPPORT */

    যদি (!result) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#ifdef CONFIG_KERNEL_MCS
    গ্ৰন্থি_STATE(ksCurTime) = getCurrentTime();
    গ্ৰন্থি_STATE(ksConsumed) = 0;
#endif
    schedule();
    activateThread();
}
