/*
 * Copyright 2014, General Dynamics C4 Systems
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
#ضمن <arch/kernel/vspace.h>
#ضمن <arch/benchmark.h>
#ضمن <arch/user_access.h>
#ضمن <arch/object/iospace.h>
#ضمن <linker.h>
#ضمن <plat/machine/hardware.h>
#ضمن <machine.h>
#ضمن <arch/machine/timer.h>
#ضمن <arch/machine/fpu.h>
#ضمن <arch/machine/tlb.h>

#که_تعریف_شوی CONFIG_ARM_SMMU
#ضمن <drivers/smmu/smmuv2.h>
#د_که_پای

#که_تعریف_شوی ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS ساكن متطاير _Atomic صحيح غوټه_boot_قلف;
#د_که_پای /* ENABLE_SMP_SUPPORT */

BOOT_BSS ساكن region_t reserved[عدد_RESERVED_REGIONS];

BOOT_کوډ ساكن bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          کلمه_t extra_bi_اندازه_بټونه)
{
    /* reserve the kernel image region */
    reserved[0] = paddr_منزل_pptr_reg(get_p_reg_kernel_img());

    صحيح شاخص = 1;

    /* add the dtb region, if it is not empty */
    إذا (dtb_p_reg.پیل) {
        إذا (شاخص >= ARRAY_اندازه(reserved)) {
            چاپ_کړه("ERROR: no slot to add DTB to reserved regions\n");
            أرجع false;
        }
        reserved[شاخص].پیل = (pptr_t) paddr_منزل_pptr(dtb_p_reg.پیل);
        reserved[شاخص].پای_ټکی = (pptr_t) paddr_منزل_pptr(dtb_p_reg.پای_ټکی);
        شاخص++;
    }

    /* Reserve the user image region and the mode-reserved regions. For now,
     * only one mode-reserved region is supported, because this is all that is
     * needed.
     */
    إذا (ډېرپېښ_RESERVED > 1) {
        چاپ_کړه("ERROR: MODE_RESERVED > 1 unsupported!\n");
        أرجع false;
    }
    إذا (ui_p_reg.پیل < PADDR_سر) {
        region_t ui_reg = paddr_منزل_pptr_reg(ui_p_reg);
        إذا (ډېرپېښ_RESERVED == 1) {
            إذا (شاخص + 1 >= ARRAY_اندازه(reserved)) {
                چاپ_کړه("ERROR: no slot to add the user image and the "
                       "mode-reserved region to the reserved regions\n");
                أرجع false;
            }
            إذا (ui_reg.پای_ټکی > ډېرپېښ_reserved_region[0].پیل) {
                reserved[شاخص] = ډېرپېښ_reserved_region[0];
                شاخص++;
                reserved[شاخص] = ui_reg;
            } وإلا {
                reserved[شاخص] = ui_reg;
                شاخص++;
                reserved[شاخص] = ډېرپېښ_reserved_region[0];
            }
            شاخص++;
        } وإلا {
            إذا (شاخص >= ARRAY_اندازه(reserved)) {
                چاپ_کړه("ERROR: no slot to add the user image to the reserved"
                       "regions\n");
                أرجع false;
            }
            reserved[شاخص] = ui_reg;
            شاخص++;
        }
    } وإلا {
        إذا (ډېرپېښ_RESERVED == 1) {
            إذا (شاخص >= ARRAY_اندازه(reserved)) {
                چاپ_کړه("ERROR: no slot to add the mode-reserved region\n");
                أرجع false;
            }
            reserved[شاخص] = ډېرپېښ_reserved_region[0];
            شاخص++;
        }

        /* Reserve the ui_p_reg region still so it doesn't get turned into device UT. */
        reserve_region(ui_p_reg);
    }

    /* avail_p_regs comes from the auto-generated code */
    أرجع init_freemem(ARRAY_اندازه(avail_p_regs), avail_p_regs,
                        شاخص, reserved,
                        it_v_reg, extra_bi_اندازه_بټونه);
}


BOOT_کوډ ساكن فراغ init_irqs(cap_t ریښه_cnode_cap)
{
    بینښه i;

    لكل (i = 0; i <= maxIRQ ; i++) {
        سېټ_IRQState(IRQInactive, CORE_IRQ_منزل_IRQT(0, i));
    }
    سېټ_IRQState(IRQTimer, CORE_IRQ_منزل_IRQT(0, KERNEL_TIMER_IRQ));
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    سېټ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(0, مداخله_VGIC_MAINTENANCE));
    سېټ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(0, مداخله_VTIMER_EVENT));
#د_که_پای
#که_تعریف_شوی CONFIG_TK1_SMMU
    سېټ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(0, مداخله_SMMU));
#د_که_پای

#که_تعریف_شوی CONFIG_ARM_ENABLE_PMU_ډکېدنه_مداخله
#که_تعریف_شوی KERNEL_PMU_IRQ
    سېټ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(0, KERNEL_PMU_IRQ));
#إذا (defined CONFIG_PLAT_TX1 && defined ENABLE_SMP_SUPPORT)
//SELFOUR-1252
#تېروتنه "This platform doesn't support tracking CPU utilisation on multicore"
#د_که_پای /* CONFIG_PLAT_TX1 && ENABLE_SMP_SUPPORT */
#وإلا
#تېروتنه "This platform doesn't support tracking CPU utilisation feature"
#د_که_پای /* KERNEL_TIMER_IRQ */
#د_که_پای /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#که_تعریف_شوی ENABLE_SMP_SUPPORT
    سېټ_IRQState(IRQIPI, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    سېټ_IRQState(IRQIPI, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
#د_که_پای

    /* provide the IRQ control cap */
    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

#که_تعریف_شوی CONFIG_ARM_SMMU
BOOT_کوډ ساكن فراغ init_smmu(cap_t ریښه_cnode_cap)
{
    plat_smmu_init();
    /*provide the SID and CB control cap*/
    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapSMMUSIDControl), cap_sid_control_cap_new());
    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapSMMUCBControl), cap_cb_control_cap_new());
}

#د_که_پای

#که_تعریف_شوی CONFIG_ALLOW_SMC_CALLS
BOOT_کوډ ساكن فراغ init_smc(cap_t ریښه_cnode_cap)
{
    /* Provide the SMC cap*/
    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapSMC), cap_smc_cap_new(0));
}
#د_که_پای

/** This and only this function initialises the CPU.
 *
 * It does NOT initialise any kernel state.
 * @return For the verification build, this currently returns true always.
 */
BOOT_کوډ ساكن bool_t init_cpu(فراغ)
{
    bool_t haveHWFPU;

#که_تعریف_شوی CONFIG_ARCH_AARCH64
    إذا (config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        إذا (!checkTCR_EL2()) {
            أرجع false;
        }
    }
#د_که_پای

    activate_kernel_vspace();
    إذا (config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_boot_init();
    }

#که_تعریف_شوی CONFIG_ARCH_AARCH64
    /* After activate_kernel_vspace(), so printf() is available. */
    إذا (!config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT) && !check16_بټ_ASID()) {
        چاپ_کړه("ERROR: kernel requires 16-bit ASIDs\n");
        أرجع false;
    }
#د_که_پای

#که_تعریف_شوی CONFIG_HARDWARE_DEBUG_API
    إذا (!Arch_initHardwareBreakpoints()) {
        چاپ_کړه("Kernel built with CONFIG_HARDWARE_DEBUG_API, but this board doesn't "
               "reliably support it.\n");
        أرجع false;
    }
#د_که_پای

    /* Setup kernel stack pointer.
     * On ARM SMP, the array index here is the CPU ID
     */
    کلمه_t ډېری_سر = ((کلمه_t) kernel_ډېری_alloc[CURRENT_CPU_شاخص()]) + بټ(CONFIG_KERNEL_ډېری_بټونه);
#که_تعریف_شوی ENABLE_SMP_SUPPORT
#که_تعریف_شوی CONFIG_ARCH_AARCH64
    /* the least 12 bits are used to store logical core ID */
    ډېری_سر |= getCurrentCPUIndex();
#وإلاإذا defined(CONFIG_ARCH_AARCH32)
    /* Stack address encodes core ID, ensure it is in the right region */
    ډېری_سر -= 8;
#د_که_پای
#د_که_پای
    سېټ_Kernel_ډېری(ډېری_سر);

#که_تعریف_شوی CONFIG_ARCH_AARCH64
    /* initialise CPU's exception vector table */
    سېټ_Vtable((pptr_t)arm_ویکټور_جدول);
#د_که_پای /* CONFIG_ARCH_AARCH64 */

    haveHWFPU = fpsimd_HWCapTest();

    /* Disable FPU to avoid channels where a platform has an FPU but doesn't make use of it */
    إذا (haveHWFPU) {
        disableFpu();
    }

#که_تعریف_شوی CONFIG_HAVE_FPU
    إذا (haveHWFPU) {
        إذا (!fpsimd_init()) {
            أرجع false;
        }
    } وإلا {
        چاپ_کړه("Platform claims to have FP hardware, but does not!\n");
        أرجع false;
    }
#د_که_پای /* CONFIG_HAVE_FPU */

    cpu_initLocalIRQController();

#که_تعریف_شوی CONFIG_ENABLE_BENCHMARKS
    arm_init_ccnt();
#د_که_پای /* CONFIG_ENABLE_BENCHMARKS */

    /* Export selected CPU features for access by PL0 */
    armv_init_user_access();

    initTimer();

    أرجع true;
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_کوډ ساكن فراغ init_plat(فراغ)
{
    initIRQController();
    initL2Cache();
#که_تعریف_شوی CONFIG_ARM_SMMU
    plat_smmu_init();
#د_که_پای
}

#که_تعریف_شوی ENABLE_SMP_SUPPORT
BOOT_کوډ ساكن bool_t try_init_kernel_secondary_core(فراغ)
{
    /* need to first wait until some kernel init has been done */
    طالما (!غوټه_boot_قلف);

    /* Perform cpu init */
    init_cpu();

    لكل (بینښه صحيح i = 0; i < عدد_PPI; i++) {
        ماسک_مداخله(true, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), i));
    }
    سېټ_IRQState(IRQIPI, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    سېټ_IRQState(IRQIPI, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
    /* Enable per-CPU timer interrupts */
    سېټ_IRQState(IRQTimer, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), KERNEL_TIMER_IRQ));
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    سېټ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), مداخله_VGIC_MAINTENANCE));
    سېټ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), مداخله_VTIMER_EVENT));
#د_که_پای /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    غوټه_قلف_SYS;

    clock_sync_test();
    ks_عدد_CPUs++;

    init_core_state(SchedulerAction_ResumeCurrentThread);

    أرجع true;
}

BOOT_کوډ ساكن فراغ release_secondary_cpus(فراغ)
{
    /* release the cpus at the same time */
    أكد(0 == غوټه_boot_قلف); /* Sanity check for a proper lock state. */
    غوټه_boot_قلف = 1;

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
#که_تعریف_شوی CONFIG_ARCH_AARCH32
    cleanInvalidateL1Caches();
    plat_cleanInvalidateL2Cache();
#د_که_پای

    /* Wait until all the secondary cores are done initialising */
    طالما (ks_عدد_CPUs != CONFIG_MAX_عدد_NODES) {
#که_تعریف_شوی ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        غوټه_STATE(ks_Cur_وخت) = get_Current_وخت();
#د_که_پای
        /* perform a memory acquire to get new values of ksNumCPUs, release for ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#د_که_پای /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

ساكن BOOT_کوډ bool_t try_init_kernel(
    paddr_t ui_p_reg_پیل,
    paddr_t ui_p_reg_پای_ټکی,
    sword_t pv_دننه_واټن,
    vptr_t  v_ثبت,
    paddr_t dtb_phys_addr,
    کلمه_t  dtb_اندازه
)
{
    cap_t ریښه_cnode_cap;
    cap_t it_ap_cap;
    cap_t it_pd_cap;
    cap_t ipcbuf_cap;
    p_region_t ui_p_reg = (p_region_t) {
        ui_p_reg_پیل, ui_p_reg_پای_ټکی
    };
    region_t ui_reg = paddr_منزل_pptr_reg(ui_p_reg);
    کلمه_t extra_bi_اندازه = 0;
    pptr_t extra_bi_دننه_واټن = 0;
    vptr_t extra_bi_چوکاټ_vptr;
    vptr_t bi_چوکاټ_vptr;
    vptr_t ipcbuf_vptr;
    create_چوکاټونه_د_region_ret_t create_چوکاټونه_ret;
    create_چوکاټونه_د_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .پیل = ui_p_reg_پیل - pv_دننه_واټن,
        .پای_ټکی   = ui_p_reg_پای_ټکی   - pv_دننه_واټن
    };

    ipcbuf_vptr = ui_v_reg.پای_ټکی;
    bi_چوکاټ_vptr = ipcbuf_vptr + بټ(پاڼه_بټونه);
    extra_bi_چوکاټ_vptr = bi_چوکاټ_vptr + بټ(se_L4_Boot_Info_چوکاټ_بټونه);

    /* setup virtual memory for the kernel */
    map_kernel_کړکۍ();

    /* initialise the CPU */
    إذا (!init_cpu()) {
        چاپ_کړه("ERROR: CPU init failed\n");
        أرجع false;
    }

    /* debug output via serial port is only available from here */
    چاپ_کړه("Bootstrapping kernel\n");

    /* initialise the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_تشه;
    إذا (dtb_اندازه > 0) {
        paddr_t dtb_phys_پای_ټکی = dtb_phys_addr + dtb_اندازه;
        إذا (dtb_phys_پای_ټکی < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            چاپ_کړه("ERROR: DTB location at %"SEL4_PRIx_کلمه
                   " len %"SEL4_PRIu_کلمه" invalid\n",
                   dtb_phys_addr, dtb_اندازه);
            أرجع false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        إذا (dtb_phys_پای_ټکی >= PADDR_سر) {
            چاپ_کړه("ERROR: DTB at [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_کلمه")\n",
                   dtb_phys_addr, dtb_phys_پای_ټکی, PADDR_سر);
            أرجع false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_اندازه += حجم(se_L4_Boot_Info_سرلیک) + dtb_اندازه;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .پیل = dtb_phys_addr,
            .پای_ټکی   = dtb_phys_پای_ټکی
        };
    }

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    کلمه_t extra_bi_اندازه_بټونه = calculate_extra_bi_اندازه_بټونه(extra_bi_اندازه);
    v_region_t it_v_reg = {
        .پیل = ui_v_reg.پیل,
        .پای_ټکی   = extra_bi_چوکاټ_vptr + (extra_bi_اندازه_بټونه > 0 ? بټ(extra_bi_اندازه_بټونه) : 0)
    };
    إذا (it_v_reg.پای_ټکی >= USER_سر) {
        /* Variable arguments for printf() require well defined integer types to
         * work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        چاپ_کړه("ERROR: userland image virt [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه")"
               "exceeds USER_TOP (%"SEL4_PRIx_کلمه")\n",
               it_v_reg.پیل, it_v_reg.پای_ټکی, (کلمه_t)USER_سر);
        أرجع false;
    }

    إذا (!arch_init_freemem(ui_p_reg, dtb_p_reg, it_v_reg, extra_bi_اندازه_بټونه)) {
        چاپ_کړه("ERROR: free memory management initialization failed\n");
        أرجع false;
    }

    /* create the root cnode */
    ریښه_cnode_cap = create_ریښه_cnode();
    إذا (cap_get_capType(ریښه_cnode_cap) == cap_null_cap) {
        چاپ_کړه("ERROR: root c-node creation failed\n");
        أرجع false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(ریښه_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(ریښه_cnode_cap);

#که_تعریف_شوی CONFIG_ARM_SMMU
    /* initialise the SMMU and provide the SMMU control caps*/
    init_smmu(ریښه_cnode_cap);
#د_که_پای
#که_تعریف_شوی CONFIG_ALLOW_SMC_CALLS
    init_smc(ریښه_cnode_cap);
#د_که_پای

    populate_bi_چوکاټ(0, CONFIG_MAX_عدد_NODES, ipcbuf_vptr, extra_bi_اندازه);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_سرلیک سرلیک;
    إذا (dtb_اندازه > 0) {
        سرلیک.id = SEL4_BOOTINFO_سرلیک_FDT;
        سرلیک.len = حجم(سرلیک) + dtb_اندازه;
        *(se_L4_Boot_Info_سرلیک *)(rootserver.extra_bi + extra_bi_دننه_واټن) = سرلیک;
        extra_bi_دننه_واټن += حجم(سرلیک);
        حافظه_نقل((فراغ *)(rootserver.extra_bi + extra_bi_دننه_واټن),
               paddr_منزل_pptr(dtb_phys_addr),
               dtb_اندازه);
        extra_bi_دننه_واټن += dtb_اندازه;
    }

    أكد(extra_bi_اندازه == extra_bi_دننه_واټن);

    إذا (config_سېټ(CONFIG_TK1_SMMU)) {
        ndks_boot.bi_چوکاټ->ioSpaceCaps = create_iospace_caps(ریښه_cnode_cap);
        إذا (ndks_boot.bi_چوکاټ->ioSpaceCaps.پیل == 0 &&
            ndks_boot.bi_چوکاټ->ioSpaceCaps.پای_ټکی == 0) {
            چاپ_کړه("ERROR: SMMU I/O space creation failed\n");
            أرجع false;
        }
    } وإلا {
        ndks_boot.bi_چوکاټ->ioSpaceCaps = S_REG_تشه;
    }

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_پته_space(ریښه_cnode_cap, it_v_reg);
    إذا (cap_get_capType(it_pd_cap) == cap_null_cap) {
        چاپ_کړه("ERROR: address space creation for initial thread failed\n");
        أرجع false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_چوکاټ_cap(
        ریښه_cnode_cap,
        it_pd_cap,
        bi_چوکاټ_vptr
    );

    /* create and map extra bootinfo region */
    إذا (extra_bi_اندازه > 0) {
        region_t extra_bi_region = {
            .پیل = rootserver.extra_bi,
            .پای_ټکی = rootserver.extra_bi + extra_bi_اندازه
        };
        extra_bi_ret =
            create_چوکاټونه_د_region(
                ریښه_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_منزل_paddr((فراغ *)extra_bi_region.پیل) - extra_bi_چوکاټ_vptr
            );
        إذا (!extra_bi_ret.success) {
            چاپ_کړه("ERROR: mapping extra boot info to initial thread failed\n");
            أرجع false;
        }
        ndks_boot.bi_چوکاټ->extraBIPages = extra_bi_ret.region;
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    init_sched_control(ریښه_cnode_cap, CONFIG_MAX_عدد_NODES);
#د_که_پای

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_چوکاټ_cap(ریښه_cnode_cap, it_pd_cap, ipcbuf_vptr);
    إذا (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        چاپ_کړه("ERROR: could not create IPC buffer for initial thread\n");
        أرجع false;
    }

    /* create all userland image frames */
    create_چوکاټونه_ret =
        create_چوکاټونه_د_region(
            ریښه_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_دننه_واټن
        );
    إذا (!create_چوکاټونه_ret.success) {
        چاپ_کړه("ERROR: could not create all userland image frames\n");
        أرجع false;
    }
    ndks_boot.bi_چوکاټ->user_Image_چوکاټونه = create_چوکاټونه_ret.region;

    /* create/initialise the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(ریښه_cnode_cap);
    إذا (cap_get_capType(it_ap_cap) == cap_null_cap) {
        چاپ_کړه("ERROR: could not create ASID pool for initial thread\n");
        أرجع false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#که_تعریف_شوی CONFIG_KERNEL_MCS
    غوټه_STATE(ks_Cur_وخت) = get_Current_وخت();
#د_که_پای

    /* create the idle thread */
    create_idle_thread();

    /* Before creating the initial thread (which also switches to it)
     * we clean the cache so that any page table information written
     * as a result of calling create_frames_of_region will be correctly
     * read by the hardware page table walker */
    cleanInvalidateL1Caches();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         ریښه_cnode_cap,
                         it_pd_cap,
                         v_ثبت,
                         bi_چوکاټ_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    إذا (initial == عدم) {
        چاپ_کړه("ERROR: could not create initial thread\n");
        أرجع false;
    }

    init_core_state(initial);

    /* create all of the untypeds. Both devices and kernel window memory */
    إذا (!create_untypeds(ریښه_cnode_cap)) {
        چاپ_کړه("ERROR: could not create untypteds for kernel image boot memory\n");
        أرجع false;
    }

    /* no shared-frame caps (ARM has no multikernel support) */
    ndks_boot.bi_چوکاټ->shared_چوکاټونه = S_REG_تشه;

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
    إذا (config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        invalidateHypTLB();
    }

    ks_عدد_CPUs = 1;

    /* initialize BKL before booting up other cores */
    SMP_COND_STATEMENT(clh_قلف_init());
    SMP_COND_STATEMENT(release_secondary_cpus());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    غوټه_قلف_SYS;

    چاپ_کړه("Booting all finished, dropped to user space\n");

    /* kernel successfully initialized */
    أرجع true;
}

BOOT_کوډ VISIBLE فراغ init_kernel(
    paddr_t ui_p_reg_پیل,
    paddr_t ui_p_reg_پای_ټکی,
    sword_t pv_دننه_واټن,
    vptr_t  v_ثبت,
    paddr_t dtb_addr_p,
    uint32_t dtb_اندازه
)
{
    bool_t پایله;

#که_تعریف_شوی ENABLE_SMP_SUPPORT
    /* we assume there exists a cpu with id 0 and will use it for bootstrapping */
    إذا (getCurrentCPUIndex() == 0) {
        پایله = try_init_kernel(ui_p_reg_پیل,
                                 ui_p_reg_پای_ټکی,
                                 pv_دننه_واټن,
                                 v_ثبت,
                                 dtb_addr_p, dtb_اندازه);
    } وإلا {
        پایله = try_init_kernel_secondary_core();
    }

#وإلا
    پایله = try_init_kernel(ui_p_reg_پیل,
                             ui_p_reg_پای_ټکی,
                             pv_دننه_واټن,
                             v_ثبت,
                             dtb_addr_p, dtb_اندازه);

#د_که_پای /* ENABLE_SMP_SUPPORT */

    إذا (!پایله) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    غوټه_STATE(ks_Cur_وخت) = get_Current_وخت();
    غوټه_STATE(ksConsumed) = 0;
#د_که_پای
    مهالوېش();
    activateThread();
}
