/*
 * Copyright 2014, General Dynamics C4 Systems
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
#अन्तर्भाव <arch/kernel/vspace.h>
#अन्तर्भाव <arch/benchmark.h>
#अन्तर्भाव <arch/user_access.h>
#अन्तर्भाव <arch/object/iospace.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/hardware.h>
#अन्तर्भाव <machine.h>
#अन्तर्भाव <arch/machine/timer.h>
#अन्तर्भाव <arch/machine/fpu.h>
#अन्तर्भाव <arch/machine/tlb.h>

#यदि_परिभाषितम् CONFIG_ARM_SMMU
#अन्तर्भाव <drivers/smmu/smmuv2.h>
#यदि_अन्तः

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS स्थिर चञ्चल _Atomic पूर्णाङ्क ग्रन्थिः_boot_तालकम्;
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

BOOT_BSS स्थिर region_t reserved[अङ्कः_RESERVED_REGIONS];

BOOT_संहिता स्थिर bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          word_t extra_bi_आकार_द्वयङ्काः)
{
    /* reserve the kernel image region */
    reserved[0] = paddr_गन्तव्यम्_pptr_reg(get_p_reg_kernel_img());

    पूर्णाङ्क सूचकांक = 1;

    /* add the dtb region, if it is not empty */
    यदि (dtb_p_reg.start) {
        यदि (सूचकांक >= ARRAY_आकार(reserved)) {
            प्रारूप_लिख("ERROR: no slot to add DTB to reserved regions\n");
            प्रत्यागम false;
        }
        reserved[सूचकांक].start = (pptr_t) paddr_गन्तव्यम्_pptr(dtb_p_reg.start);
        reserved[सूचकांक].अन्तः_2 = (pptr_t) paddr_गन्तव्यम्_pptr(dtb_p_reg.अन्तः_2);
        सूचकांक++;
    }

    /* Reserve the user image region and the mode-reserved regions. For now,
     * only one mode-reserved region is supported, because this is all that is
     * needed.
     */
    यदि (बहुलकम्_RESERVED > 1) {
        प्रारूप_लिख("ERROR: MODE_RESERVED > 1 unsupported!\n");
        प्रत्यागम false;
    }
    यदि (ui_p_reg.start < PADDR_शिखरम्) {
        region_t ui_reg = paddr_गन्तव्यम्_pptr_reg(ui_p_reg);
        यदि (बहुलकम्_RESERVED == 1) {
            यदि (सूचकांक + 1 >= ARRAY_आकार(reserved)) {
                प्रारूप_लिख("ERROR: no slot to add the user image and the "
                       "mode-reserved region to the reserved regions\n");
                प्रत्यागम false;
            }
            यदि (ui_reg.अन्तः_2 > बहुलकम्_reserved_region[0].start) {
                reserved[सूचकांक] = बहुलकम्_reserved_region[0];
                सूचकांक++;
                reserved[सूचकांक] = ui_reg;
            } अन्यथा {
                reserved[सूचकांक] = ui_reg;
                सूचकांक++;
                reserved[सूचकांक] = बहुलकम्_reserved_region[0];
            }
            सूचकांक++;
        } अन्यथा {
            यदि (सूचकांक >= ARRAY_आकार(reserved)) {
                प्रारूप_लिख("ERROR: no slot to add the user image to the reserved"
                       "regions\n");
                प्रत्यागम false;
            }
            reserved[सूचकांक] = ui_reg;
            सूचकांक++;
        }
    } अन्यथा {
        यदि (बहुलकम्_RESERVED == 1) {
            यदि (सूचकांक >= ARRAY_आकार(reserved)) {
                प्रारूप_लिख("ERROR: no slot to add the mode-reserved region\n");
                प्रत्यागम false;
            }
            reserved[सूचकांक] = बहुलकम्_reserved_region[0];
            सूचकांक++;
        }

        /* Reserve the ui_p_reg region still so it doesn't get turned into device UT. */
        reserve_region(ui_p_reg);
    }

    /* avail_p_regs comes from the auto-generated code */
    प्रत्यागम init_freemem(ARRAY_आकार(avail_p_regs), avail_p_regs,
                        सूचकांक, reserved,
                        it_v_reg, extra_bi_आकार_द्वयङ्काः);
}


BOOT_संहिता स्थिर शून्य init_irqs(cap_t मूलम्_cnode_cap)
{
    अचिह्नित i;

    प्रति (i = 0; i <= maxIRQ ; i++) {
        समुच्चयः_IRQState(IRQInactive, CORE_IRQ_गन्तव्यम्_IRQT(0, i));
    }
    समुच्चयः_IRQState(IRQTimer, CORE_IRQ_गन्तव्यम्_IRQT(0, KERNEL_TIMER_IRQ));
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    समुच्चयः_IRQState(IRQReserved, CORE_IRQ_गन्तव्यम्_IRQT(0, व्यवधानम्_VGIC_MAINTENANCE));
    समुच्चयः_IRQState(IRQReserved, CORE_IRQ_गन्तव्यम्_IRQT(0, व्यवधानम्_VTIMER_EVENT));
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_TK1_SMMU
    समुच्चयः_IRQState(IRQReserved, CORE_IRQ_गन्तव्यम्_IRQT(0, व्यवधानम्_SMMU));
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_ARM_ENABLE_PMU_अतिप्रवाहः_व्यवधानम्
#यदि_परिभाषितम् KERNEL_PMU_IRQ
    समुच्चयः_IRQState(IRQReserved, CORE_IRQ_गन्तव्यम्_IRQT(0, KERNEL_PMU_IRQ));
#यदि (defined CONFIG_PLAT_TX1 && defined ENABLE_SMP_SUPPORT)
//SELFOUR-1252
#दोषः "This platform doesn't support tracking CPU utilisation on multicore"
#यदि_अन्तः /* CONFIG_PLAT_TX1 && ENABLE_SMP_SUPPORT */
#अन्यथा
#दोषः "This platform doesn't support tracking CPU utilisation feature"
#यदि_अन्तः /* KERNEL_TIMER_IRQ */
#यदि_अन्तः /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    समुच्चयः_IRQState(IRQIPI, CORE_IRQ_गन्तव्यम्_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    समुच्चयः_IRQState(IRQIPI, CORE_IRQ_गन्तव्यम्_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
#यदि_अन्तः

    /* provide the IRQ control cap */
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

#यदि_परिभाषितम् CONFIG_ARM_SMMU
BOOT_संहिता स्थिर शून्य init_smmu(cap_t मूलम्_cnode_cap)
{
    plat_smmu_init();
    /*provide the SID and CB control cap*/
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapSMMUSIDControl), cap_sid_control_cap_new());
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapSMMUCBControl), cap_cb_control_cap_new());
}

#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_ALLOW_SMC_CALLS
BOOT_संहिता स्थिर शून्य init_smc(cap_t मूलम्_cnode_cap)
{
    /* Provide the SMC cap*/
    write_slot(SLOT_PTR(pptr_सम्बन्धि_cap(मूलम्_cnode_cap), seL4_CapSMC), cap_smc_cap_new(0));
}
#यदि_अन्तः

/** This and only this function initialises the CPU.
 *
 * It does NOT initialise any kernel state.
 * @return For the verification build, this currently returns true always.
 */
BOOT_संहिता स्थिर bool_t init_cpu(शून्य)
{
    bool_t haveHWFPU;

#यदि_परिभाषितम् CONFIG_ARCH_AARCH64
    यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        यदि (!checkTCR_EL2()) {
            प्रत्यागम false;
        }
    }
#यदि_अन्तः

    activate_kernel_vspace();
    यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_boot_init();
    }

#यदि_परिभाषितम् CONFIG_ARCH_AARCH64
    /* After activate_kernel_vspace(), so printf() is available. */
    यदि (!config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT) && !check16BitASID()) {
        प्रारूप_लिख("ERROR: kernel requires 16-bit ASIDs\n");
        प्रत्यागम false;
    }
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_HARDWARE_DEBUG_API
    यदि (!Arch_initHardwareBreakpoints()) {
        प्रारूप_लिख("Kernel built with CONFIG_HARDWARE_DEBUG_API, but this board doesn't "
               "reliably support it.\n");
        प्रत्यागम false;
    }
#यदि_अन्तः

    /* Setup kernel stack pointer.
     * On ARM SMP, the array index here is the CPU ID
     */
    word_t राशिः_शिखरम् = ((word_t) kernel_राशिः_alloc[CURRENT_CPU_सूचकांक()]) + BIT(CONFIG_KERNEL_राशिः_द्वयङ्काः);
#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
#यदि_परिभाषितम् CONFIG_ARCH_AARCH64
    /* the least 12 bits are used to store logical core ID */
    राशिः_शिखरम् |= getCurrentCPUIndex();
#नोचेद्यदि defined(CONFIG_ARCH_AARCH32)
    /* Stack address encodes core ID, ensure it is in the right region */
    राशिः_शिखरम् -= 8;
#यदि_अन्तः
#यदि_अन्तः
    समुच्चयः_Kernel_राशिः(राशिः_शिखरम्);

#यदि_परिभाषितम् CONFIG_ARCH_AARCH64
    /* initialise CPU's exception vector table */
    समुच्चयः_Vtable((pptr_t)arm_सदिशम्_सारणी);
#यदि_अन्तः /* CONFIG_ARCH_AARCH64 */

    haveHWFPU = fpsimd_HWCapTest();

    /* Disable FPU to avoid channels where a platform has an FPU but doesn't make use of it */
    यदि (haveHWFPU) {
        disableFpu();
    }

#यदि_परिभाषितम् CONFIG_HAVE_FPU
    यदि (haveHWFPU) {
        यदि (!fpsimd_init()) {
            प्रत्यागम false;
        }
    } अन्यथा {
        प्रारूप_लिख("Platform claims to have FP hardware, but does not!\n");
        प्रत्यागम false;
    }
#यदि_अन्तः /* CONFIG_HAVE_FPU */

    cpu_initLocalIRQController();

#यदि_परिभाषितम् CONFIG_ENABLE_BENCHMARKS
    arm_init_ccnt();
#यदि_अन्तः /* CONFIG_ENABLE_BENCHMARKS */

    /* Export selected CPU features for access by PL0 */
    armv_init_user_access();

    initTimer();

    प्रत्यागम true;
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_संहिता स्थिर शून्य init_plat(शून्य)
{
    initIRQController();
    initL2Cache();
#यदि_परिभाषितम् CONFIG_ARM_SMMU
    plat_smmu_init();
#यदि_अन्तः
}

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
BOOT_संहिता स्थिर bool_t try_init_kernel_secondary_core(शून्य)
{
    /* need to first wait until some kernel init has been done */
    यावत् (!ग्रन्थिः_boot_तालकम्);

    /* Perform cpu init */
    init_cpu();

    प्रति (अचिह्नित पूर्णाङ्क i = 0; i < अङ्कः_PPI; i++) {
        आवरणम्_व्यवधानम्(true, CORE_IRQ_गन्तव्यम्_IRQT(getCurrentCPUIndex(), i));
    }
    समुच्चयः_IRQState(IRQIPI, CORE_IRQ_गन्तव्यम्_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    समुच्चयः_IRQState(IRQIPI, CORE_IRQ_गन्तव्यम्_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
    /* Enable per-CPU timer interrupts */
    समुच्चयः_IRQState(IRQTimer, CORE_IRQ_गन्तव्यम्_IRQT(getCurrentCPUIndex(), KERNEL_TIMER_IRQ));
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    समुच्चयः_IRQState(IRQReserved, CORE_IRQ_गन्तव्यम्_IRQT(getCurrentCPUIndex(), व्यवधानम्_VGIC_MAINTENANCE));
    समुच्चयः_IRQState(IRQReserved, CORE_IRQ_गन्तव्यम्_IRQT(getCurrentCPUIndex(), व्यवधानम्_VTIMER_EVENT));
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    ग्रन्थिः_तालकम्_SYS;

    clock_sync_test();
    ks_अङ्कः_CPUs++;

    init_core_state(SchedulerAction_ResumeCurrentThread);

    प्रत्यागम true;
}

BOOT_संहिता स्थिर शून्य release_secondary_cpus(शून्य)
{
    /* release the cpus at the same time */
    प्रतिज्ञा(0 == ग्रन्थिः_boot_तालकम्); /* Sanity check for a proper lock state. */
    ग्रन्थिः_boot_तालकम् = 1;

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
#यदि_परिभाषितम् CONFIG_ARCH_AARCH32
    cleanInvalidateL1Caches();
    plat_cleanInvalidateL2Cache();
#यदि_अन्तः

    /* Wait until all the secondary cores are done initialising */
    यावत् (ks_अङ्कः_CPUs != CONFIG_MAX_अङ्कः_NODES) {
#यदि_परिभाषितम् ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        ग्रन्थिः_STATE(ksCurTime) = getCurrentTime();
#यदि_अन्तः
        /* perform a memory acquire to get new values of ksNumCPUs, release for ksCurTime */
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
    cap_t it_ap_cap;
    cap_t it_pd_cap;
    cap_t ipcbuf_cap;
    p_region_t ui_p_reg = (p_region_t) {
        ui_p_reg_start, ui_p_reg_अन्तः
    };
    region_t ui_reg = paddr_गन्तव्यम्_pptr_reg(ui_p_reg);
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

    /* setup virtual memory for the kernel */
    map_kernel_गवाक्षः();

    /* initialise the CPU */
    यदि (!init_cpu()) {
        प्रारूप_लिख("ERROR: CPU init failed\n");
        प्रत्यागम false;
    }

    /* debug output via serial port is only available from here */
    प्रारूप_लिख("Bootstrapping kernel\n");

    /* initialise the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_रिक्तानि;
    यदि (dtb_आकार > 0) {
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

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    word_t extra_bi_आकार_द्वयङ्काः = calculate_extra_bi_आकार_द्वयङ्काः(extra_bi_आकार);
    v_region_t it_v_reg = {
        .start = ui_v_reg.start,
        .अन्तः_2   = extra_bi_frame_vptr + (extra_bi_आकार_द्वयङ्काः > 0 ? BIT(extra_bi_आकार_द्वयङ्काः) : 0)
    };
    यदि (it_v_reg.अन्तः_2 >= USER_शिखरम्) {
        /* Variable arguments for printf() require well defined integer types to
         * work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        प्रारूप_लिख("ERROR: userland image virt [%"SEL4_PRIx_word"..%"SEL4_PRIx_word")"
               "exceeds USER_TOP (%"SEL4_PRIx_word")\n",
               it_v_reg.start, it_v_reg.अन्तः_2, (word_t)USER_शिखरम्);
        प्रत्यागम false;
    }

    यदि (!arch_init_freemem(ui_p_reg, dtb_p_reg, it_v_reg, extra_bi_आकार_द्वयङ्काः)) {
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

#यदि_परिभाषितम् CONFIG_ARM_SMMU
    /* initialise the SMMU and provide the SMMU control caps*/
    init_smmu(मूलम्_cnode_cap);
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_ALLOW_SMC_CALLS
    init_smc(मूलम्_cnode_cap);
#यदि_अन्तः

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

    यदि (config_समुच्चयः(CONFIG_TK1_SMMU)) {
        ndks_boot.bi_frame->ioSpaceCaps = create_iospace_caps(मूलम्_cnode_cap);
        यदि (ndks_boot.bi_frame->ioSpaceCaps.start == 0 &&
            ndks_boot.bi_frame->ioSpaceCaps.अन्तः_2 == 0) {
            प्रारूप_लिख("ERROR: SMMU I/O space creation failed\n");
            प्रत्यागम false;
        }
    } अन्यथा {
        ndks_boot.bi_frame->ioSpaceCaps = S_REG_रिक्तानि;
    }

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

    /* create/initialise the initial thread's ASID pool */
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

    /* Before creating the initial thread (which also switches to it)
     * we clean the cache so that any page table information written
     * as a result of calling create_frames_of_region will be correctly
     * read by the hardware page table walker */
    cleanInvalidateL1Caches();

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

    /* create all of the untypeds. Both devices and kernel window memory */
    यदि (!create_untypeds(मूलम्_cnode_cap)) {
        प्रारूप_लिख("ERROR: could not create untypteds for kernel image boot memory\n");
        प्रत्यागम false;
    }

    /* no shared-frame caps (ARM has no multikernel support) */
    ndks_boot.bi_frame->shared_चौकटाः = S_REG_रिक्तानि;

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
    यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        invalidateHypTLB();
    }

    ks_अङ्कः_CPUs = 1;

    /* initialize BKL before booting up other cores */
    SMP_COND_STATEMENT(clh_तालकम्_init());
    SMP_COND_STATEMENT(release_secondary_cpus());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    ग्रन्थिः_तालकम्_SYS;

    प्रारूप_लिख("Booting all finished, dropped to user space\n");

    /* kernel successfully initialized */
    प्रत्यागम true;
}

BOOT_संहिता VISIBLE शून्य init_kernel(
    paddr_t ui_p_reg_start,
    paddr_t ui_p_reg_अन्तः,
    sword_t pv_विस्थापनम्,
    vptr_t  v_प्रविष्टिः,
    paddr_t dtb_addr_p,
    uint32_t dtb_आकार
)
{
    bool_t परिणाम;

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    /* we assume there exists a cpu with id 0 and will use it for bootstrapping */
    यदि (getCurrentCPUIndex() == 0) {
        परिणाम = try_init_kernel(ui_p_reg_start,
                                 ui_p_reg_अन्तः,
                                 pv_विस्थापनम्,
                                 v_प्रविष्टिः,
                                 dtb_addr_p, dtb_आकार);
    } अन्यथा {
        परिणाम = try_init_kernel_secondary_core();
    }

#अन्यथा
    परिणाम = try_init_kernel(ui_p_reg_start,
                             ui_p_reg_अन्तः,
                             pv_विस्थापनम्,
                             v_प्रविष्टिः,
                             dtb_addr_p, dtb_आकार);

#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

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
