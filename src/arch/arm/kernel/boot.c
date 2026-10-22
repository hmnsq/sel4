/*
 * Copyright 2014, General Dynamics C4 Systems
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
#समावेश <arch/kernel/vspace.h>
#समावेश <arch/benchmark.h>
#समावेश <arch/user_access.h>
#समावेश <arch/object/iospace.h>
#समावेश <linker.h>
#समावेश <plat/machine/hardware.h>
#समावेश <machine.h>
#समावेश <arch/machine/timer.h>
#समावेश <arch/machine/fpu.h>
#समावेश <arch/machine/tlb.h>

#अगर_घोषित CONFIG_ARM_SMMU
#समावेश <drivers/smmu/smmuv2.h>
#पूर्ण_अगर

#अगर_घोषित ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS अटल अस्थिर _Atomic पूर्णांक गाँठ_boot_lock;
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */

BOOT_BSS अटल region_t reserved[अंक_RESERVED_REGIONS];

BOOT_CODE अटल bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          word_t extra_bi_आकार_bits)
{
    /* reserve the kernel image region */
    reserved[0] = paddr_to_pptr_reg(get_p_reg_kernel_img());

    पूर्णांक सूचकांक = 1;

    /* add the dtb region, if it is not empty */
    अगर (dtb_p_reg.start) {
        अगर (सूचकांक >= ARRAY_आकार(reserved)) {
            म_लिखो("ERROR: no slot to add DTB to reserved regions\n");
            वापस false;
        }
        reserved[सूचकांक].start = (pptr_t) paddr_to_pptr(dtb_p_reg.start);
        reserved[सूचकांक].end = (pptr_t) paddr_to_pptr(dtb_p_reg.end);
        सूचकांक++;
    }

    /* Reserve the user image region and the mode-reserved regions. For now,
     * only one mode-reserved region is supported, because this is all that is
     * needed.
     */
    अगर (MODE_RESERVED > 1) {
        म_लिखो("ERROR: MODE_RESERVED > 1 unsupported!\n");
        वापस false;
    }
    अगर (ui_p_reg.start < PADDR_शीर्ष) {
        region_t ui_reg = paddr_to_pptr_reg(ui_p_reg);
        अगर (MODE_RESERVED == 1) {
            अगर (सूचकांक + 1 >= ARRAY_आकार(reserved)) {
                म_लिखो("ERROR: no slot to add the user image and the "
                       "mode-reserved region to the reserved regions\n");
                वापस false;
            }
            अगर (ui_reg.end > mode_reserved_region[0].start) {
                reserved[सूचकांक] = mode_reserved_region[0];
                सूचकांक++;
                reserved[सूचकांक] = ui_reg;
            } अन्यथा {
                reserved[सूचकांक] = ui_reg;
                सूचकांक++;
                reserved[सूचकांक] = mode_reserved_region[0];
            }
            सूचकांक++;
        } अन्यथा {
            अगर (सूचकांक >= ARRAY_आकार(reserved)) {
                म_लिखो("ERROR: no slot to add the user image to the reserved"
                       "regions\n");
                वापस false;
            }
            reserved[सूचकांक] = ui_reg;
            सूचकांक++;
        }
    } अन्यथा {
        अगर (MODE_RESERVED == 1) {
            अगर (सूचकांक >= ARRAY_आकार(reserved)) {
                म_लिखो("ERROR: no slot to add the mode-reserved region\n");
                वापस false;
            }
            reserved[सूचकांक] = mode_reserved_region[0];
            सूचकांक++;
        }

        /* Reserve the ui_p_reg region still so it doesn't get turned into device UT. */
        reserve_region(ui_p_reg);
    }

    /* avail_p_regs comes from the auto-generated code */
    वापस init_freemem(ARRAY_आकार(avail_p_regs), avail_p_regs,
                        सूचकांक, reserved,
                        it_v_reg, extra_bi_आकार_bits);
}


BOOT_CODE अटल व्योम init_irqs(cap_t मूल_cnode_cap)
{
    अचिन्हित i;

    क्रम (i = 0; i <= maxIRQ ; i++) {
        setIRQState(IRQInactive, CORE_IRQ_TO_IRQT(0, i));
    }
    setIRQState(IRQTimer, CORE_IRQ_TO_IRQT(0, KERNEL_TIMER_IRQ));
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    setIRQState(IRQReserved, CORE_IRQ_TO_IRQT(0, INTERRUPT_VGIC_MAINTENANCE));
    setIRQState(IRQReserved, CORE_IRQ_TO_IRQT(0, INTERRUPT_VTIMER_EVENT));
#पूर्ण_अगर
#अगर_घोषित CONFIG_TK1_SMMU
    setIRQState(IRQReserved, CORE_IRQ_TO_IRQT(0, INTERRUPT_SMMU));
#पूर्ण_अगर

#अगर_घोषित CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT
#अगर_घोषित KERNEL_PMU_IRQ
    setIRQState(IRQReserved, CORE_IRQ_TO_IRQT(0, KERNEL_PMU_IRQ));
#अगर (defined CONFIG_PLAT_TX1 && defined ENABLE_SMP_SUPPORT)
//SELFOUR-1252
#त्रुटि "This platform doesn't support tracking CPU utilisation on multicore"
#पूर्ण_अगर /* CONFIG_PLAT_TX1 && ENABLE_SMP_SUPPORT */
#अन्यथा
#त्रुटि "This platform doesn't support tracking CPU utilisation feature"
#पूर्ण_अगर /* KERNEL_TIMER_IRQ */
#पूर्ण_अगर /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#अगर_घोषित ENABLE_SMP_SUPPORT
    setIRQState(IRQIPI, CORE_IRQ_TO_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    setIRQState(IRQIPI, CORE_IRQ_TO_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
#पूर्ण_अगर

    /* provide the IRQ control cap */
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

#अगर_घोषित CONFIG_ARM_SMMU
BOOT_CODE अटल व्योम init_smmu(cap_t मूल_cnode_cap)
{
    plat_smmu_init();
    /*provide the SID and CB control cap*/
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapSMMUSIDControl), cap_sid_control_cap_new());
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapSMMUCBControl), cap_cb_control_cap_new());
}

#पूर्ण_अगर

#अगर_घोषित CONFIG_ALLOW_SMC_CALLS
BOOT_CODE अटल व्योम init_smc(cap_t मूल_cnode_cap)
{
    /* Provide the SMC cap*/
    write_slot(SLOT_PTR(pptr_of_cap(मूल_cnode_cap), seL4_CapSMC), cap_smc_cap_new(0));
}
#पूर्ण_अगर

/** This and only this function initialises the CPU.
 *
 * It does NOT initialise any kernel state.
 * @return For the verification build, this currently returns true always.
 */
BOOT_CODE अटल bool_t init_cpu(व्योम)
{
    bool_t haveHWFPU;

#अगर_घोषित CONFIG_ARCH_AARCH64
    अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        अगर (!checkTCR_EL2()) {
            वापस false;
        }
    }
#पूर्ण_अगर

    activate_kernel_vspace();
    अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_boot_init();
    }

#अगर_घोषित CONFIG_ARCH_AARCH64
    /* After activate_kernel_vspace(), so printf() is available. */
    अगर (!config_set(CONFIG_ARM_HYPERVISOR_SUPPORT) && !check16BitASID()) {
        म_लिखो("ERROR: kernel requires 16-bit ASIDs\n");
        वापस false;
    }
#पूर्ण_अगर

#अगर_घोषित CONFIG_HARDWARE_DEBUG_API
    अगर (!Arch_initHardwareBreakpoints()) {
        म_लिखो("Kernel built with CONFIG_HARDWARE_DEBUG_API, but this board doesn't "
               "reliably support it.\n");
        वापस false;
    }
#पूर्ण_अगर

    /* Setup kernel stack pointer.
     * On ARM SMP, the array index here is the CPU ID
     */
    word_t ढेर_शीर्ष = ((word_t) kernel_ढेर_alloc[CURRENT_CPU_सूचकांक()]) + BIT(CONFIG_KERNEL_ढेर_BITS);
#अगर_घोषित ENABLE_SMP_SUPPORT
#अगर_घोषित CONFIG_ARCH_AARCH64
    /* the least 12 bits are used to store logical core ID */
    ढेर_शीर्ष |= getCurrentCPUIndex();
#या_अगर defined(CONFIG_ARCH_AARCH32)
    /* Stack address encodes core ID, ensure it is in the right region */
    ढेर_शीर्ष -= 8;
#पूर्ण_अगर
#पूर्ण_अगर
    set_Kernel_ढेर(ढेर_शीर्ष);

#अगर_घोषित CONFIG_ARCH_AARCH64
    /* initialise CPU's exception vector table */
    setVtable((pptr_t)arm_vector_table);
#पूर्ण_अगर /* CONFIG_ARCH_AARCH64 */

    haveHWFPU = fpsimd_HWCapTest();

    /* Disable FPU to avoid channels where a platform has an FPU but doesn't make use of it */
    अगर (haveHWFPU) {
        disableFpu();
    }

#अगर_घोषित CONFIG_HAVE_FPU
    अगर (haveHWFPU) {
        अगर (!fpsimd_init()) {
            वापस false;
        }
    } अन्यथा {
        म_लिखो("Platform claims to have FP hardware, but does not!\n");
        वापस false;
    }
#पूर्ण_अगर /* CONFIG_HAVE_FPU */

    cpu_initLocalIRQController();

#अगर_घोषित CONFIG_ENABLE_BENCHMARKS
    arm_init_ccnt();
#पूर्ण_अगर /* CONFIG_ENABLE_BENCHMARKS */

    /* Export selected CPU features for access by PL0 */
    armv_init_user_access();

    initTimer();

    वापस true;
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_CODE अटल व्योम init_plat(व्योम)
{
    initIRQController();
    initL2Cache();
#अगर_घोषित CONFIG_ARM_SMMU
    plat_smmu_init();
#पूर्ण_अगर
}

#अगर_घोषित ENABLE_SMP_SUPPORT
BOOT_CODE अटल bool_t try_init_kernel_secondary_core(व्योम)
{
    /* need to first wait until some kernel init has been done */
    जबतक (!गाँठ_boot_lock);

    /* Perform cpu init */
    init_cpu();

    क्रम (अचिन्हित पूर्णांक i = 0; i < अंक_PPI; i++) {
        maskInterrupt(true, CORE_IRQ_TO_IRQT(getCurrentCPUIndex(), i));
    }
    setIRQState(IRQIPI, CORE_IRQ_TO_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    setIRQState(IRQIPI, CORE_IRQ_TO_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
    /* Enable per-CPU timer interrupts */
    setIRQState(IRQTimer, CORE_IRQ_TO_IRQT(getCurrentCPUIndex(), KERNEL_TIMER_IRQ));
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    setIRQState(IRQReserved, CORE_IRQ_TO_IRQT(getCurrentCPUIndex(), INTERRUPT_VGIC_MAINTENANCE));
    setIRQState(IRQReserved, CORE_IRQ_TO_IRQT(getCurrentCPUIndex(), INTERRUPT_VTIMER_EVENT));
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    गाँठ_LOCK_SYS;

    clock_sync_test();
    ks_अंक_CPUs++;

    init_core_state(SchedulerAction_ResumeCurrentThread);

    वापस true;
}

BOOT_CODE अटल व्योम release_secondary_cpus(व्योम)
{
    /* release the cpus at the same time */
    निश्चित(0 == गाँठ_boot_lock); /* Sanity check for a proper lock state. */
    गाँठ_boot_lock = 1;

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
#अगर_घोषित CONFIG_ARCH_AARCH32
    cleanInvalidateL1Caches();
    plat_cleanInvalidateL2Cache();
#पूर्ण_अगर

    /* Wait until all the secondary cores are done initialising */
    जबतक (ks_अंक_CPUs != CONFIG_MAX_अंक_NODES) {
#अगर_घोषित ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        गाँठ_STATE(ksCurTime) = getCurrentTime();
#पूर्ण_अगर
        /* perform a memory acquire to get new values of ksNumCPUs, release for ksCurTime */
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
    cap_t it_ap_cap;
    cap_t it_pd_cap;
    cap_t ipcbuf_cap;
    p_region_t ui_p_reg = (p_region_t) {
        ui_p_reg_start, ui_p_reg_end
    };
    region_t ui_reg = paddr_to_pptr_reg(ui_p_reg);
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

    /* setup virtual memory for the kernel */
    map_kernel_window();

    /* initialise the CPU */
    अगर (!init_cpu()) {
        म_लिखो("ERROR: CPU init failed\n");
        वापस false;
    }

    /* debug output via serial port is only available from here */
    म_लिखो("Bootstrapping kernel\n");

    /* initialise the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_EMPTY;
    अगर (dtb_आकार > 0) {
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

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    word_t extra_bi_आकार_bits = calculate_extra_bi_आकार_bits(extra_bi_आकार);
    v_region_t it_v_reg = {
        .start = ui_v_reg.start,
        .end   = extra_bi_frame_vptr + (extra_bi_आकार_bits > 0 ? BIT(extra_bi_आकार_bits) : 0)
    };
    अगर (it_v_reg.end >= USER_शीर्ष) {
        /* Variable arguments for printf() require well defined integer types to
         * work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        म_लिखो("ERROR: userland image virt [%"SEL4_PRIx_word"..%"SEL4_PRIx_word")"
               "exceeds USER_TOP (%"SEL4_PRIx_word")\n",
               it_v_reg.start, it_v_reg.end, (word_t)USER_शीर्ष);
        वापस false;
    }

    अगर (!arch_init_freemem(ui_p_reg, dtb_p_reg, it_v_reg, extra_bi_आकार_bits)) {
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

#अगर_घोषित CONFIG_ARM_SMMU
    /* initialise the SMMU and provide the SMMU control caps*/
    init_smmu(मूल_cnode_cap);
#पूर्ण_अगर
#अगर_घोषित CONFIG_ALLOW_SMC_CALLS
    init_smc(मूल_cnode_cap);
#पूर्ण_अगर

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

    अगर (config_set(CONFIG_TK1_SMMU)) {
        ndks_boot.bi_frame->ioSpaceCaps = create_iospace_caps(मूल_cnode_cap);
        अगर (ndks_boot.bi_frame->ioSpaceCaps.start == 0 &&
            ndks_boot.bi_frame->ioSpaceCaps.end == 0) {
            म_लिखो("ERROR: SMMU I/O space creation failed\n");
            वापस false;
        }
    } अन्यथा {
        ndks_boot.bi_frame->ioSpaceCaps = S_REG_EMPTY;
    }

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

    /* create/initialise the initial thread's ASID pool */
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

    /* Before creating the initial thread (which also switches to it)
     * we clean the cache so that any page table information written
     * as a result of calling create_frames_of_region will be correctly
     * read by the hardware page table walker */
    cleanInvalidateL1Caches();

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

    /* create all of the untypeds. Both devices and kernel window memory */
    अगर (!create_untypeds(मूल_cnode_cap)) {
        म_लिखो("ERROR: could not create untypteds for kernel image boot memory\n");
        वापस false;
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
    अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        invalidateHypTLB();
    }

    ks_अंक_CPUs = 1;

    /* initialize BKL before booting up other cores */
    SMP_COND_STATEMENT(clh_lock_init());
    SMP_COND_STATEMENT(release_secondary_cpus());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    गाँठ_LOCK_SYS;

    म_लिखो("Booting all finished, dropped to user space\n");

    /* kernel successfully initialized */
    वापस true;
}

BOOT_CODE VISIBLE व्योम init_kernel(
    paddr_t ui_p_reg_start,
    paddr_t ui_p_reg_end,
    sword_t pv_offset,
    vptr_t  v_entry,
    paddr_t dtb_addr_p,
    uint32_t dtb_आकार
)
{
    bool_t परिणाम;

#अगर_घोषित ENABLE_SMP_SUPPORT
    /* we assume there exists a cpu with id 0 and will use it for bootstrapping */
    अगर (getCurrentCPUIndex() == 0) {
        परिणाम = try_init_kernel(ui_p_reg_start,
                                 ui_p_reg_end,
                                 pv_offset,
                                 v_entry,
                                 dtb_addr_p, dtb_आकार);
    } अन्यथा {
        परिणाम = try_init_kernel_secondary_core();
    }

#अन्यथा
    परिणाम = try_init_kernel(ui_p_reg_start,
                             ui_p_reg_end,
                             pv_offset,
                             v_entry,
                             dtb_addr_p, dtb_आकार);

#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */

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
