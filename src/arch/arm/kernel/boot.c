/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <kernel/boot.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <object/interrupt.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/kernel/boot.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <arch/benchmark.h>
#అన్తర్భావ <arch/user_access.h>
#అన్తర్భావ <arch/object/iospace.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/hardware.h>
#అన్తర్భావ <machine.h>
#అన్తర్భావ <arch/machine/timer.h>
#అన్తర్భావ <arch/machine/fpu.h>
#అన్తర్భావ <arch/machine/tlb.h>

#నిర్వచితమైతే CONFIG_ARM_SMMU
#అన్తర్భావ <drivers/smmu/smmuv2.h>
#అయితే_ముగింపు

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS స్థిర అస్థిర _Atomic పూర్ణాంకం కణుపు_boot_తాళం;
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

BOOT_BSS స్థిర region_t reserved[అంకె_RESERVED_REGIONS];

BOOT_సంకేతం స్థిర bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          మాట_t extra_bi_కొలత_బిట్లు)
{
    /* reserve the kernel image region */
    reserved[0] = paddr_గమ్యం_pptr_reg(get_p_reg_kernel_img());

    పూర్ణాంకం సూచీ = 1;

    /* add the dtb region, if it is not empty */
    ఒకవేళ (dtb_p_reg.ప్రారంభం) {
        ఒకవేళ (సూచీ >= ARRAY_కొలత(reserved)) {
            రాయి("ERROR: no slot to add DTB to reserved regions\n");
            తిరిగి false;
        }
        reserved[సూచీ].ప్రారంభం = (pptr_t) paddr_గమ్యం_pptr(dtb_p_reg.ప్రారంభం);
        reserved[సూచీ].చివర = (pptr_t) paddr_గమ్యం_pptr(dtb_p_reg.చివర);
        సూచీ++;
    }

    /* Reserve the user image region and the mode-reserved regions. For now,
     * only one mode-reserved region is supported, because this is all that is
     * needed.
     */
    ఒకవేళ (బహుళకం_RESERVED > 1) {
        రాయి("ERROR: MODE_RESERVED > 1 unsupported!\n");
        తిరిగి false;
    }
    ఒకవేళ (ui_p_reg.ప్రారంభం < PADDR_శిఖరం) {
        region_t ui_reg = paddr_గమ్యం_pptr_reg(ui_p_reg);
        ఒకవేళ (బహుళకం_RESERVED == 1) {
            ఒకవేళ (సూచీ + 1 >= ARRAY_కొలత(reserved)) {
                రాయి("ERROR: no slot to add the user image and the "
                       "mode-reserved region to the reserved regions\n");
                తిరిగి false;
            }
            ఒకవేళ (ui_reg.చివర > బహుళకం_reserved_region[0].ప్రారంభం) {
                reserved[సూచీ] = బహుళకం_reserved_region[0];
                సూచీ++;
                reserved[సూచీ] = ui_reg;
            } లేకపోతే {
                reserved[సూచీ] = ui_reg;
                సూచీ++;
                reserved[సూచీ] = బహుళకం_reserved_region[0];
            }
            సూచీ++;
        } లేకపోతే {
            ఒకవేళ (సూచీ >= ARRAY_కొలత(reserved)) {
                రాయి("ERROR: no slot to add the user image to the reserved"
                       "regions\n");
                తిరిగి false;
            }
            reserved[సూచీ] = ui_reg;
            సూచీ++;
        }
    } లేకపోతే {
        ఒకవేళ (బహుళకం_RESERVED == 1) {
            ఒకవేళ (సూచీ >= ARRAY_కొలత(reserved)) {
                రాయి("ERROR: no slot to add the mode-reserved region\n");
                తిరిగి false;
            }
            reserved[సూచీ] = బహుళకం_reserved_region[0];
            సూచీ++;
        }

        /* Reserve the ui_p_reg region still so it doesn't get turned into device UT. */
        reserve_region(ui_p_reg);
    }

    /* avail_p_regs comes from the auto-generated code */
    తిరిగి init_freemem(ARRAY_కొలత(avail_p_regs), avail_p_regs,
                        సూచీ, reserved,
                        it_v_reg, extra_bi_కొలత_బిట్లు);
}


BOOT_సంకేతం స్థిర శూన్య init_irqs(cap_t మూలం_cnode_cap)
{
    గుర్తులేని i;

    కొరకు (i = 0; i <= maxIRQ ; i++) {
        సమితి_IRQState(IRQInactive, CORE_IRQ_గమ్యం_IRQT(0, i));
    }
    సమితి_IRQState(IRQTimer, CORE_IRQ_గమ్యం_IRQT(0, KERNEL_TIMER_IRQ));
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    సమితి_IRQState(IRQReserved, CORE_IRQ_గమ్యం_IRQT(0, అంతరాయం_VGIC_MAINTENANCE));
    సమితి_IRQState(IRQReserved, CORE_IRQ_గమ్యం_IRQT(0, అంతరాయం_VTIMER_EVENT));
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_TK1_SMMU
    సమితి_IRQState(IRQReserved, CORE_IRQ_గమ్యం_IRQT(0, అంతరాయం_SMMU));
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_ARM_ENABLE_PMU_అధికప్రవాహం_అంతరాయం
#నిర్వచితమైతే KERNEL_PMU_IRQ
    సమితి_IRQState(IRQReserved, CORE_IRQ_గమ్యం_IRQT(0, KERNEL_PMU_IRQ));
#ఒకవేళ (defined CONFIG_PLAT_TX1 && defined ENABLE_SMP_SUPPORT)
//SELFOUR-1252
#దోషం "This platform doesn't support tracking CPU utilisation on multicore"
#అయితే_ముగింపు /* CONFIG_PLAT_TX1 && ENABLE_SMP_SUPPORT */
#లేకపోతే
#దోషం "This platform doesn't support tracking CPU utilisation feature"
#అయితే_ముగింపు /* KERNEL_TIMER_IRQ */
#అయితే_ముగింపు /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    సమితి_IRQState(IRQIPI, CORE_IRQ_గమ్యం_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    సమితి_IRQState(IRQIPI, CORE_IRQ_గమ్యం_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
#అయితే_ముగింపు

    /* provide the IRQ control cap */
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

#నిర్వచితమైతే CONFIG_ARM_SMMU
BOOT_సంకేతం స్థిర శూన్య init_smmu(cap_t మూలం_cnode_cap)
{
    plat_smmu_init();
    /*provide the SID and CB control cap*/
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapSMMUSIDControl), cap_sid_control_cap_new());
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapSMMUCBControl), cap_cb_control_cap_new());
}

#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_ALLOW_SMC_CALLS
BOOT_సంకేతం స్థిర శూన్య init_smc(cap_t మూలం_cnode_cap)
{
    /* Provide the SMC cap*/
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapSMC), cap_smc_cap_new(0));
}
#అయితే_ముగింపు

/** This and only this function initialises the CPU.
 *
 * It does NOT initialise any kernel state.
 * @return For the verification build, this currently returns true always.
 */
BOOT_సంకేతం స్థిర bool_t init_cpu(శూన్య)
{
    bool_t haveHWFPU;

#నిర్వచితమైతే CONFIG_ARCH_AARCH64
    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        ఒకవేళ (!checkTCR_EL2()) {
            తిరిగి false;
        }
    }
#అయితే_ముగింపు

    activate_kernel_vspace();
    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_boot_init();
    }

#నిర్వచితమైతే CONFIG_ARCH_AARCH64
    /* After activate_kernel_vspace(), so printf() is available. */
    ఒకవేళ (!config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT) && !check16_బిట్టు_ASID()) {
        రాయి("ERROR: kernel requires 16-bit ASIDs\n");
        తిరిగి false;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API
    ఒకవేళ (!Arch_initHardwareBreakpoints()) {
        రాయి("Kernel built with CONFIG_HARDWARE_DEBUG_API, but this board doesn't "
               "reliably support it.\n");
        తిరిగి false;
    }
#అయితే_ముగింపు

    /* Setup kernel stack pointer.
     * On ARM SMP, the array index here is the CPU ID
     */
    మాట_t దొంతర_శిఖరం = ((మాట_t) kernel_దొంతర_alloc[CURRENT_CPU_సూచీ()]) + బిట్టు(CONFIG_KERNEL_దొంతర_బిట్లు);
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
#నిర్వచితమైతే CONFIG_ARCH_AARCH64
    /* the least 12 bits are used to store logical core ID */
    దొంతర_శిఖరం |= getCurrentCPUIndex();
#లేదావేళ defined(CONFIG_ARCH_AARCH32)
    /* Stack address encodes core ID, ensure it is in the right region */
    దొంతర_శిఖరం -= 8;
#అయితే_ముగింపు
#అయితే_ముగింపు
    సమితి_Kernel_దొంతర(దొంతర_శిఖరం);

#నిర్వచితమైతే CONFIG_ARCH_AARCH64
    /* initialise CPU's exception vector table */
    సమితి_Vtable((pptr_t)arm_సదిశ_పట్టిక);
#అయితే_ముగింపు /* CONFIG_ARCH_AARCH64 */

    haveHWFPU = fpsimd_HWCapTest();

    /* Disable FPU to avoid channels where a platform has an FPU but doesn't make use of it */
    ఒకవేళ (haveHWFPU) {
        disableFpu();
    }

#నిర్వచితమైతే CONFIG_HAVE_FPU
    ఒకవేళ (haveHWFPU) {
        ఒకవేళ (!fpsimd_init()) {
            తిరిగి false;
        }
    } లేకపోతే {
        రాయి("Platform claims to have FP hardware, but does not!\n");
        తిరిగి false;
    }
#అయితే_ముగింపు /* CONFIG_HAVE_FPU */

    cpu_initLocalIRQController();

#నిర్వచితమైతే CONFIG_ENABLE_BENCHMARKS
    arm_init_ccnt();
#అయితే_ముగింపు /* CONFIG_ENABLE_BENCHMARKS */

    /* Export selected CPU features for access by PL0 */
    armv_init_user_access();

    initTimer();

    తిరిగి true;
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_సంకేతం స్థిర శూన్య init_plat(శూన్య)
{
    initIRQController();
    initL2Cache();
#నిర్వచితమైతే CONFIG_ARM_SMMU
    plat_smmu_init();
#అయితే_ముగింపు
}

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
BOOT_సంకేతం స్థిర bool_t try_init_kernel_secondary_core(శూన్య)
{
    /* need to first wait until some kernel init has been done */
    అయితే (!కణుపు_boot_తాళం);

    /* Perform cpu init */
    init_cpu();

    కొరకు (గుర్తులేని పూర్ణాంకం i = 0; i < అంకె_PPI; i++) {
        ముసుగు_అంతరాయం(true, CORE_IRQ_గమ్యం_IRQT(getCurrentCPUIndex(), i));
    }
    సమితి_IRQState(IRQIPI, CORE_IRQ_గమ్యం_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    సమితి_IRQState(IRQIPI, CORE_IRQ_గమ్యం_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
    /* Enable per-CPU timer interrupts */
    సమితి_IRQState(IRQTimer, CORE_IRQ_గమ్యం_IRQT(getCurrentCPUIndex(), KERNEL_TIMER_IRQ));
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    సమితి_IRQState(IRQReserved, CORE_IRQ_గమ్యం_IRQT(getCurrentCPUIndex(), అంతరాయం_VGIC_MAINTENANCE));
    సమితి_IRQState(IRQReserved, CORE_IRQ_గమ్యం_IRQT(getCurrentCPUIndex(), అంతరాయం_VTIMER_EVENT));
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    కణుపు_తాళం_SYS;

    clock_sync_test();
    ks_అంకె_CPUs++;

    init_core_state(SchedulerAction_ResumeCurrentThread);

    తిరిగి true;
}

BOOT_సంకేతం స్థిర శూన్య release_secondary_cpus(శూన్య)
{
    /* release the cpus at the same time */
    ప్రతిజ్ఞా(0 == కణుపు_boot_తాళం); /* Sanity check for a proper lock state. */
    కణుపు_boot_తాళం = 1;

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
#నిర్వచితమైతే CONFIG_ARCH_AARCH32
    cleanInvalidateL1Caches();
    plat_cleanInvalidateL2Cache();
#అయితే_ముగింపు

    /* Wait until all the secondary cores are done initialising */
    అయితే (ks_అంకె_CPUs != CONFIG_MAX_అంకె_NODES) {
#నిర్వచితమైతే ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        కణుపు_STATE(ks_Cur_సమయం) = get_Current_సమయం();
#అయితే_ముగింపు
        /* perform a memory acquire to get new values of ksNumCPUs, release for ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

స్థిర BOOT_సంకేతం bool_t try_init_kernel(
    paddr_t ui_p_reg_ప్రారంభం,
    paddr_t ui_p_reg_చివర,
    sword_t pv_స్థానభ్రంశం,
    vptr_t  v_నమోదు,
    paddr_t dtb_phys_addr,
    మాట_t  dtb_కొలత
)
{
    cap_t మూలం_cnode_cap;
    cap_t it_ap_cap;
    cap_t it_pd_cap;
    cap_t ipcbuf_cap;
    p_region_t ui_p_reg = (p_region_t) {
        ui_p_reg_ప్రారంభం, ui_p_reg_చివర
    };
    region_t ui_reg = paddr_గమ్యం_pptr_reg(ui_p_reg);
    మాట_t extra_bi_కొలత = 0;
    pptr_t extra_bi_స్థానభ్రంశం = 0;
    vptr_t extra_bi_చట్రం_vptr;
    vptr_t bi_చట్రం_vptr;
    vptr_t ipcbuf_vptr;
    create_చట్రాలు_యొక్క_region_ret_t create_చట్రాలు_ret;
    create_చట్రాలు_యొక్క_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .ప్రారంభం = ui_p_reg_ప్రారంభం - pv_స్థానభ్రంశం,
        .చివర   = ui_p_reg_చివర   - pv_స్థానభ్రంశం
    };

    ipcbuf_vptr = ui_v_reg.చివర;
    bi_చట్రం_vptr = ipcbuf_vptr + బిట్టు(పుట_బిట్లు);
    extra_bi_చట్రం_vptr = bi_చట్రం_vptr + బిట్టు(se_L4_Boot_Info_చట్రం_బిట్లు);

    /* setup virtual memory for the kernel */
    map_kernel_కిటికీ();

    /* initialise the CPU */
    ఒకవేళ (!init_cpu()) {
        రాయి("ERROR: CPU init failed\n");
        తిరిగి false;
    }

    /* debug output via serial port is only available from here */
    రాయి("Bootstrapping kernel\n");

    /* initialise the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_ఖాళీలు;
    ఒకవేళ (dtb_కొలత > 0) {
        paddr_t dtb_phys_చివర = dtb_phys_addr + dtb_కొలత;
        ఒకవేళ (dtb_phys_చివర < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            రాయి("ERROR: DTB location at %"SEL4_PRIx_మాట
                   " len %"SEL4_PRIu_మాట" invalid\n",
                   dtb_phys_addr, dtb_కొలత);
            తిరిగి false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        ఒకవేళ (dtb_phys_చివర >= PADDR_శిఖరం) {
            రాయి("ERROR: DTB at [%"SEL4_PRIx_మాట"..%"SEL4_PRIx_మాట") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_మాట")\n",
                   dtb_phys_addr, dtb_phys_చివర, PADDR_శిఖరం);
            తిరిగి false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_కొలత += పరిమాణం(se_L4_Boot_Info_శీర్షిక) + dtb_కొలత;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .ప్రారంభం = dtb_phys_addr,
            .చివర   = dtb_phys_చివర
        };
    }

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    మాట_t extra_bi_కొలత_బిట్లు = calculate_extra_bi_కొలత_బిట్లు(extra_bi_కొలత);
    v_region_t it_v_reg = {
        .ప్రారంభం = ui_v_reg.ప్రారంభం,
        .చివర   = extra_bi_చట్రం_vptr + (extra_bi_కొలత_బిట్లు > 0 ? బిట్టు(extra_bi_కొలత_బిట్లు) : 0)
    };
    ఒకవేళ (it_v_reg.చివర >= USER_శిఖరం) {
        /* Variable arguments for printf() require well defined integer types to
         * work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        రాయి("ERROR: userland image virt [%"SEL4_PRIx_మాట"..%"SEL4_PRIx_మాట")"
               "exceeds USER_TOP (%"SEL4_PRIx_మాట")\n",
               it_v_reg.ప్రారంభం, it_v_reg.చివర, (మాట_t)USER_శిఖరం);
        తిరిగి false;
    }

    ఒకవేళ (!arch_init_freemem(ui_p_reg, dtb_p_reg, it_v_reg, extra_bi_కొలత_బిట్లు)) {
        రాయి("ERROR: free memory management initialization failed\n");
        తిరిగి false;
    }

    /* create the root cnode */
    మూలం_cnode_cap = create_మూలం_cnode();
    ఒకవేళ (cap_get_capType(మూలం_cnode_cap) == cap_null_cap) {
        రాయి("ERROR: root c-node creation failed\n");
        తిరిగి false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(మూలం_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(మూలం_cnode_cap);

#నిర్వచితమైతే CONFIG_ARM_SMMU
    /* initialise the SMMU and provide the SMMU control caps*/
    init_smmu(మూలం_cnode_cap);
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_ALLOW_SMC_CALLS
    init_smc(మూలం_cnode_cap);
#అయితే_ముగింపు

    populate_bi_చట్రం(0, CONFIG_MAX_అంకె_NODES, ipcbuf_vptr, extra_bi_కొలత);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_శీర్షిక శీర్షిక;
    ఒకవేళ (dtb_కొలత > 0) {
        శీర్షిక.id = SEL4_BOOTINFO_శీర్షిక_FDT;
        శీర్షిక.len = పరిమాణం(శీర్షిక) + dtb_కొలత;
        *(se_L4_Boot_Info_శీర్షిక *)(rootserver.extra_bi + extra_bi_స్థానభ్రంశం) = శీర్షిక;
        extra_bi_స్థానభ్రంశం += పరిమాణం(శీర్షిక);
        జ్ఞాపకం_నకలు((శూన్య *)(rootserver.extra_bi + extra_bi_స్థానభ్రంశం),
               paddr_గమ్యం_pptr(dtb_phys_addr),
               dtb_కొలత);
        extra_bi_స్థానభ్రంశం += dtb_కొలత;
    }

    ప్రతిజ్ఞా(extra_bi_కొలత == extra_bi_స్థానభ్రంశం);

    ఒకవేళ (config_సమితి(CONFIG_TK1_SMMU)) {
        ndks_boot.bi_చట్రం->ioSpaceCaps = create_iospace_caps(మూలం_cnode_cap);
        ఒకవేళ (ndks_boot.bi_చట్రం->ioSpaceCaps.ప్రారంభం == 0 &&
            ndks_boot.bi_చట్రం->ioSpaceCaps.చివర == 0) {
            రాయి("ERROR: SMMU I/O space creation failed\n");
            తిరిగి false;
        }
    } లేకపోతే {
        ndks_boot.bi_చట్రం->ioSpaceCaps = S_REG_ఖాళీలు;
    }

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_చిరునామా_space(మూలం_cnode_cap, it_v_reg);
    ఒకవేళ (cap_get_capType(it_pd_cap) == cap_null_cap) {
        రాయి("ERROR: address space creation for initial thread failed\n");
        తిరిగి false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_చట్రం_cap(
        మూలం_cnode_cap,
        it_pd_cap,
        bi_చట్రం_vptr
    );

    /* create and map extra bootinfo region */
    ఒకవేళ (extra_bi_కొలత > 0) {
        region_t extra_bi_region = {
            .ప్రారంభం = rootserver.extra_bi,
            .చివర = rootserver.extra_bi + extra_bi_కొలత
        };
        extra_bi_ret =
            create_చట్రాలు_యొక్క_region(
                మూలం_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_గమ్యం_paddr((శూన్య *)extra_bi_region.ప్రారంభం) - extra_bi_చట్రం_vptr
            );
        ఒకవేళ (!extra_bi_ret.success) {
            రాయి("ERROR: mapping extra boot info to initial thread failed\n");
            తిరిగి false;
        }
        ndks_boot.bi_చట్రం->extraBIPages = extra_bi_ret.region;
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    init_sched_control(మూలం_cnode_cap, CONFIG_MAX_అంకె_NODES);
#అయితే_ముగింపు

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_చట్రం_cap(మూలం_cnode_cap, it_pd_cap, ipcbuf_vptr);
    ఒకవేళ (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        రాయి("ERROR: could not create IPC buffer for initial thread\n");
        తిరిగి false;
    }

    /* create all userland image frames */
    create_చట్రాలు_ret =
        create_చట్రాలు_యొక్క_region(
            మూలం_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_స్థానభ్రంశం
        );
    ఒకవేళ (!create_చట్రాలు_ret.success) {
        రాయి("ERROR: could not create all userland image frames\n");
        తిరిగి false;
    }
    ndks_boot.bi_చట్రం->user_Image_చట్రాలు = create_చట్రాలు_ret.region;

    /* create/initialise the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(మూలం_cnode_cap);
    ఒకవేళ (cap_get_capType(it_ap_cap) == cap_null_cap) {
        రాయి("ERROR: could not create ASID pool for initial thread\n");
        తిరిగి false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    కణుపు_STATE(ks_Cur_సమయం) = get_Current_సమయం();
#అయితే_ముగింపు

    /* create the idle thread */
    create_idle_thread();

    /* Before creating the initial thread (which also switches to it)
     * we clean the cache so that any page table information written
     * as a result of calling create_frames_of_region will be correctly
     * read by the hardware page table walker */
    cleanInvalidateL1Caches();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         మూలం_cnode_cap,
                         it_pd_cap,
                         v_నమోదు,
                         bi_చట్రం_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    ఒకవేళ (initial == ఖాళీ) {
        రాయి("ERROR: could not create initial thread\n");
        తిరిగి false;
    }

    init_core_state(initial);

    /* create all of the untypeds. Both devices and kernel window memory */
    ఒకవేళ (!create_untypeds(మూలం_cnode_cap)) {
        రాయి("ERROR: could not create untypteds for kernel image boot memory\n");
        తిరిగి false;
    }

    /* no shared-frame caps (ARM has no multikernel support) */
    ndks_boot.bi_చట్రం->shared_చట్రాలు = S_REG_ఖాళీలు;

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
    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        invalidateHypTLB();
    }

    ks_అంకె_CPUs = 1;

    /* initialize BKL before booting up other cores */
    SMP_COND_STATEMENT(clh_తాళం_init());
    SMP_COND_STATEMENT(release_secondary_cpus());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    కణుపు_తాళం_SYS;

    రాయి("Booting all finished, dropped to user space\n");

    /* kernel successfully initialized */
    తిరిగి true;
}

BOOT_సంకేతం VISIBLE శూన్య init_kernel(
    paddr_t ui_p_reg_ప్రారంభం,
    paddr_t ui_p_reg_చివర,
    sword_t pv_స్థానభ్రంశం,
    vptr_t  v_నమోదు,
    paddr_t dtb_addr_p,
    uint32_t dtb_కొలత
)
{
    bool_t ఫలితం;

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    /* we assume there exists a cpu with id 0 and will use it for bootstrapping */
    ఒకవేళ (getCurrentCPUIndex() == 0) {
        ఫలితం = try_init_kernel(ui_p_reg_ప్రారంభం,
                                 ui_p_reg_చివర,
                                 pv_స్థానభ్రంశం,
                                 v_నమోదు,
                                 dtb_addr_p, dtb_కొలత);
    } లేకపోతే {
        ఫలితం = try_init_kernel_secondary_core();
    }

#లేకపోతే
    ఫలితం = try_init_kernel(ui_p_reg_ప్రారంభం,
                             ui_p_reg_చివర,
                             pv_స్థానభ్రంశం,
                             v_నమోదు,
                             dtb_addr_p, dtb_కొలత);

#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

    ఒకవేళ (!ఫలితం) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    కణుపు_STATE(ks_Cur_సమయం) = get_Current_సమయం();
    కణుపు_STATE(ksConsumed) = 0;
#అయితే_ముగింపు
    కాలపట్టిక();
    activateThread();
}
