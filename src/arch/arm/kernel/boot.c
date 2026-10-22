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

#اگر_تعریف_شده CONFIG_ARM_SMMU
#ضمن <drivers/smmu/smmuv2.h>
#پایان_اگر

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS ساكن متطاير _Atomic صحيح گره_boot_قفل;
#پایان_اگر /* ENABLE_SMP_SUPPORT */

BOOT_BSS ساكن region_t reserved[رقم_RESERVED_REGIONS];

BOOT_کد ساكن bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          کلمه_t extra_bi_اندازه_بیتها)
{
    /* reserve the kernel image region */
    reserved[0] = paddr_مقصد_pptr_reg(get_p_reg_kernel_img());

    صحيح شاخص = 1;

    /* add the dtb region, if it is not empty */
    إذا (dtb_p_reg.شروع) {
        إذا (شاخص >= ARRAY_اندازه(reserved)) {
            چاپ_کن("ERROR: no slot to add DTB to reserved regions\n");
            أرجع false;
        }
        reserved[شاخص].شروع = (pptr_t) paddr_مقصد_pptr(dtb_p_reg.شروع);
        reserved[شاخص].انتها = (pptr_t) paddr_مقصد_pptr(dtb_p_reg.انتها);
        شاخص++;
    }

    /* Reserve the user image region and the mode-reserved regions. For now,
     * only one mode-reserved region is supported, because this is all that is
     * needed.
     */
    إذا (نما_RESERVED > 1) {
        چاپ_کن("ERROR: MODE_RESERVED > 1 unsupported!\n");
        أرجع false;
    }
    إذا (ui_p_reg.شروع < PADDR_بالاترین) {
        region_t ui_reg = paddr_مقصد_pptr_reg(ui_p_reg);
        إذا (نما_RESERVED == 1) {
            إذا (شاخص + 1 >= ARRAY_اندازه(reserved)) {
                چاپ_کن("ERROR: no slot to add the user image and the "
                       "mode-reserved region to the reserved regions\n");
                أرجع false;
            }
            إذا (ui_reg.انتها > نما_reserved_region[0].شروع) {
                reserved[شاخص] = نما_reserved_region[0];
                شاخص++;
                reserved[شاخص] = ui_reg;
            } وإلا {
                reserved[شاخص] = ui_reg;
                شاخص++;
                reserved[شاخص] = نما_reserved_region[0];
            }
            شاخص++;
        } وإلا {
            إذا (شاخص >= ARRAY_اندازه(reserved)) {
                چاپ_کن("ERROR: no slot to add the user image to the reserved"
                       "regions\n");
                أرجع false;
            }
            reserved[شاخص] = ui_reg;
            شاخص++;
        }
    } وإلا {
        إذا (نما_RESERVED == 1) {
            إذا (شاخص >= ARRAY_اندازه(reserved)) {
                چاپ_کن("ERROR: no slot to add the mode-reserved region\n");
                أرجع false;
            }
            reserved[شاخص] = نما_reserved_region[0];
            شاخص++;
        }

        /* Reserve the ui_p_reg region still so it doesn't get turned into device UT. */
        reserve_region(ui_p_reg);
    }

    /* avail_p_regs comes from the auto-generated code */
    أرجع init_freemem(ARRAY_اندازه(avail_p_regs), avail_p_regs,
                        شاخص, reserved,
                        it_v_reg, extra_bi_اندازه_بیتها);
}


BOOT_کد ساكن فراغ init_irqs(cap_t ریشه_cnode_cap)
{
    بیعلامت i;

    لكل (i = 0; i <= maxIRQ ; i++) {
        مجموعه_IRQState(IRQInactive, CORE_IRQ_مقصد_IRQT(0, i));
    }
    مجموعه_IRQState(IRQTimer, CORE_IRQ_مقصد_IRQT(0, KERNEL_TIMER_IRQ));
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    مجموعه_IRQState(IRQReserved, CORE_IRQ_مقصد_IRQT(0, وقفه_VGIC_MAINTENANCE));
    مجموعه_IRQState(IRQReserved, CORE_IRQ_مقصد_IRQT(0, وقفه_VTIMER_EVENT));
#پایان_اگر
#اگر_تعریف_شده CONFIG_TK1_SMMU
    مجموعه_IRQState(IRQReserved, CORE_IRQ_مقصد_IRQT(0, وقفه_SMMU));
#پایان_اگر

#اگر_تعریف_شده CONFIG_ARM_ENABLE_PMU_سرریز_وقفه
#اگر_تعریف_شده KERNEL_PMU_IRQ
    مجموعه_IRQState(IRQReserved, CORE_IRQ_مقصد_IRQT(0, KERNEL_PMU_IRQ));
#إذا (defined CONFIG_PLAT_TX1 && defined ENABLE_SMP_SUPPORT)
//SELFOUR-1252
#خطا "This platform doesn't support tracking CPU utilisation on multicore"
#پایان_اگر /* CONFIG_PLAT_TX1 && ENABLE_SMP_SUPPORT */
#وإلا
#خطا "This platform doesn't support tracking CPU utilisation feature"
#پایان_اگر /* KERNEL_TIMER_IRQ */
#پایان_اگر /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    مجموعه_IRQState(IRQIPI, CORE_IRQ_مقصد_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    مجموعه_IRQState(IRQIPI, CORE_IRQ_مقصد_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
#پایان_اگر

    /* provide the IRQ control cap */
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

#اگر_تعریف_شده CONFIG_ARM_SMMU
BOOT_کد ساكن فراغ init_smmu(cap_t ریشه_cnode_cap)
{
    plat_smmu_init();
    /*provide the SID and CB control cap*/
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapSMMUSIDControl), cap_sid_control_cap_new());
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapSMMUCBControl), cap_cb_control_cap_new());
}

#پایان_اگر

#اگر_تعریف_شده CONFIG_ALLOW_SMC_CALLS
BOOT_کد ساكن فراغ init_smc(cap_t ریشه_cnode_cap)
{
    /* Provide the SMC cap*/
    write_جایگاه(جایگاه_PTR(pptr_از_cap(ریشه_cnode_cap), seL4_CapSMC), cap_smc_cap_new(0));
}
#پایان_اگر

/** This and only this function initialises the CPU.
 *
 * It does NOT initialise any kernel state.
 * @return For the verification build, this currently returns true always.
 */
BOOT_کد ساكن bool_t init_cpu(فراغ)
{
    bool_t haveHWFPU;

#اگر_تعریف_شده CONFIG_ARCH_AARCH64
    إذا (config_مجموعه(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        إذا (!checkTCR_EL2()) {
            أرجع false;
        }
    }
#پایان_اگر

    activate_kernel_vspace();
    إذا (config_مجموعه(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_boot_init();
    }

#اگر_تعریف_شده CONFIG_ARCH_AARCH64
    /* After activate_kernel_vspace(), so printf() is available. */
    إذا (!config_مجموعه(CONFIG_ARM_HYPERVISOR_SUPPORT) && !check16_بیت_ASID()) {
        چاپ_کن("ERROR: kernel requires 16-bit ASIDs\n");
        أرجع false;
    }
#پایان_اگر

#اگر_تعریف_شده CONFIG_HARDWARE_DEBUG_API
    إذا (!Arch_initHardwareBreakpoints()) {
        چاپ_کن("Kernel built with CONFIG_HARDWARE_DEBUG_API, but this board doesn't "
               "reliably support it.\n");
        أرجع false;
    }
#پایان_اگر

    /* Setup kernel stack pointer.
     * On ARM SMP, the array index here is the CPU ID
     */
    کلمه_t پشته_بالاترین = ((کلمه_t) kernel_پشته_alloc[CURRENT_CPU_شاخص()]) + بیت(CONFIG_KERNEL_پشته_بیتها);
#اگر_تعریف_شده ENABLE_SMP_SUPPORT
#اگر_تعریف_شده CONFIG_ARCH_AARCH64
    /* the least 12 bits are used to store logical core ID */
    پشته_بالاترین |= getCurrentCPUIndex();
#وإلاإذا defined(CONFIG_ARCH_AARCH32)
    /* Stack address encodes core ID, ensure it is in the right region */
    پشته_بالاترین -= 8;
#پایان_اگر
#پایان_اگر
    مجموعه_Kernel_پشته(پشته_بالاترین);

#اگر_تعریف_شده CONFIG_ARCH_AARCH64
    /* initialise CPU's exception vector table */
    مجموعه_Vtable((pptr_t)arm_وکتور_جدول);
#پایان_اگر /* CONFIG_ARCH_AARCH64 */

    haveHWFPU = fpsimd_HWCapTest();

    /* Disable FPU to avoid channels where a platform has an FPU but doesn't make use of it */
    إذا (haveHWFPU) {
        disableFpu();
    }

#اگر_تعریف_شده CONFIG_HAVE_FPU
    إذا (haveHWFPU) {
        إذا (!fpsimd_init()) {
            أرجع false;
        }
    } وإلا {
        چاپ_کن("Platform claims to have FP hardware, but does not!\n");
        أرجع false;
    }
#پایان_اگر /* CONFIG_HAVE_FPU */

    cpu_initLocalIRQController();

#اگر_تعریف_شده CONFIG_ENABLE_BENCHMARKS
    arm_init_ccnt();
#پایان_اگر /* CONFIG_ENABLE_BENCHMARKS */

    /* Export selected CPU features for access by PL0 */
    armv_init_user_access();

    initTimer();

    أرجع true;
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_کد ساكن فراغ init_plat(فراغ)
{
    initIRQController();
    initL2Cache();
#اگر_تعریف_شده CONFIG_ARM_SMMU
    plat_smmu_init();
#پایان_اگر
}

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
BOOT_کد ساكن bool_t try_init_kernel_secondary_core(فراغ)
{
    /* need to first wait until some kernel init has been done */
    طالما (!گره_boot_قفل);

    /* Perform cpu init */
    init_cpu();

    لكل (بیعلامت صحيح i = 0; i < رقم_PPI; i++) {
        ماسک_وقفه(true, CORE_IRQ_مقصد_IRQT(getCurrentCPUIndex(), i));
    }
    مجموعه_IRQState(IRQIPI, CORE_IRQ_مقصد_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    مجموعه_IRQState(IRQIPI, CORE_IRQ_مقصد_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
    /* Enable per-CPU timer interrupts */
    مجموعه_IRQState(IRQTimer, CORE_IRQ_مقصد_IRQT(getCurrentCPUIndex(), KERNEL_TIMER_IRQ));
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    مجموعه_IRQState(IRQReserved, CORE_IRQ_مقصد_IRQT(getCurrentCPUIndex(), وقفه_VGIC_MAINTENANCE));
    مجموعه_IRQState(IRQReserved, CORE_IRQ_مقصد_IRQT(getCurrentCPUIndex(), وقفه_VTIMER_EVENT));
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    گره_قفل_SYS;

    clock_sync_test();
    ks_رقم_CPUs++;

    init_core_state(SchedulerAction_ResumeCurrentThread);

    أرجع true;
}

BOOT_کد ساكن فراغ release_secondary_cpus(فراغ)
{
    /* release the cpus at the same time */
    أكد(0 == گره_boot_قفل); /* Sanity check for a proper lock state. */
    گره_boot_قفل = 1;

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
#اگر_تعریف_شده CONFIG_ARCH_AARCH32
    cleanInvalidateL1Caches();
    plat_cleanInvalidateL2Cache();
#پایان_اگر

    /* Wait until all the secondary cores are done initialising */
    طالما (ks_رقم_CPUs != CONFIG_MAX_رقم_NODES) {
#اگر_تعریف_شده ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        گره_STATE(ks_Cur_زمان) = get_Current_زمان();
#پایان_اگر
        /* perform a memory acquire to get new values of ksNumCPUs, release for ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#پایان_اگر /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

ساكن BOOT_کد bool_t try_init_kernel(
    paddr_t ui_p_reg_شروع,
    paddr_t ui_p_reg_انتها,
    sword_t pv_جابه_جایی,
    vptr_t  v_مدخل,
    paddr_t dtb_phys_addr,
    کلمه_t  dtb_اندازه
)
{
    cap_t ریشه_cnode_cap;
    cap_t it_ap_cap;
    cap_t it_pd_cap;
    cap_t ipcbuf_cap;
    p_region_t ui_p_reg = (p_region_t) {
        ui_p_reg_شروع, ui_p_reg_انتها
    };
    region_t ui_reg = paddr_مقصد_pptr_reg(ui_p_reg);
    کلمه_t extra_bi_اندازه = 0;
    pptr_t extra_bi_جابه_جایی = 0;
    vptr_t extra_bi_قاب_vptr;
    vptr_t bi_قاب_vptr;
    vptr_t ipcbuf_vptr;
    create_قابها_از_region_ret_t create_قابها_ret;
    create_قابها_از_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .شروع = ui_p_reg_شروع - pv_جابه_جایی,
        .انتها   = ui_p_reg_انتها   - pv_جابه_جایی
    };

    ipcbuf_vptr = ui_v_reg.انتها;
    bi_قاب_vptr = ipcbuf_vptr + بیت(صفحه_بیتها);
    extra_bi_قاب_vptr = bi_قاب_vptr + بیت(se_L4_Boot_Info_قاب_بیتها);

    /* setup virtual memory for the kernel */
    map_kernel_پنجره();

    /* initialise the CPU */
    إذا (!init_cpu()) {
        چاپ_کن("ERROR: CPU init failed\n");
        أرجع false;
    }

    /* debug output via serial port is only available from here */
    چاپ_کن("Bootstrapping kernel\n");

    /* initialise the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_خالی;
    إذا (dtb_اندازه > 0) {
        paddr_t dtb_phys_انتها = dtb_phys_addr + dtb_اندازه;
        إذا (dtb_phys_انتها < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            چاپ_کن("ERROR: DTB location at %"SEL4_PRIx_کلمه
                   " len %"SEL4_PRIu_کلمه" invalid\n",
                   dtb_phys_addr, dtb_اندازه);
            أرجع false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        إذا (dtb_phys_انتها >= PADDR_بالاترین) {
            چاپ_کن("ERROR: DTB at [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_کلمه")\n",
                   dtb_phys_addr, dtb_phys_انتها, PADDR_بالاترین);
            أرجع false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_اندازه += حجم(se_L4_Boot_Info_سرآیند) + dtb_اندازه;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .شروع = dtb_phys_addr,
            .انتها   = dtb_phys_انتها
        };
    }

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    کلمه_t extra_bi_اندازه_بیتها = calculate_extra_bi_اندازه_بیتها(extra_bi_اندازه);
    v_region_t it_v_reg = {
        .شروع = ui_v_reg.شروع,
        .انتها   = extra_bi_قاب_vptr + (extra_bi_اندازه_بیتها > 0 ? بیت(extra_bi_اندازه_بیتها) : 0)
    };
    إذا (it_v_reg.انتها >= USER_بالاترین) {
        /* Variable arguments for printf() require well defined integer types to
         * work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        چاپ_کن("ERROR: userland image virt [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه")"
               "exceeds USER_TOP (%"SEL4_PRIx_کلمه")\n",
               it_v_reg.شروع, it_v_reg.انتها, (کلمه_t)USER_بالاترین);
        أرجع false;
    }

    إذا (!arch_init_freemem(ui_p_reg, dtb_p_reg, it_v_reg, extra_bi_اندازه_بیتها)) {
        چاپ_کن("ERROR: free memory management initialization failed\n");
        أرجع false;
    }

    /* create the root cnode */
    ریشه_cnode_cap = create_ریشه_cnode();
    إذا (cap_get_capType(ریشه_cnode_cap) == cap_null_cap) {
        چاپ_کن("ERROR: root c-node creation failed\n");
        أرجع false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(ریشه_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(ریشه_cnode_cap);

#اگر_تعریف_شده CONFIG_ARM_SMMU
    /* initialise the SMMU and provide the SMMU control caps*/
    init_smmu(ریشه_cnode_cap);
#پایان_اگر
#اگر_تعریف_شده CONFIG_ALLOW_SMC_CALLS
    init_smc(ریشه_cnode_cap);
#پایان_اگر

    populate_bi_قاب(0, CONFIG_MAX_رقم_NODES, ipcbuf_vptr, extra_bi_اندازه);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_سرآیند سرآیند;
    إذا (dtb_اندازه > 0) {
        سرآیند.id = SEL4_BOOTINFO_سرآیند_FDT;
        سرآیند.len = حجم(سرآیند) + dtb_اندازه;
        *(se_L4_Boot_Info_سرآیند *)(rootserver.extra_bi + extra_bi_جابه_جایی) = سرآیند;
        extra_bi_جابه_جایی += حجم(سرآیند);
        کپی_حافظه((فراغ *)(rootserver.extra_bi + extra_bi_جابه_جایی),
               paddr_مقصد_pptr(dtb_phys_addr),
               dtb_اندازه);
        extra_bi_جابه_جایی += dtb_اندازه;
    }

    أكد(extra_bi_اندازه == extra_bi_جابه_جایی);

    إذا (config_مجموعه(CONFIG_TK1_SMMU)) {
        ndks_boot.bi_قاب->ioSpaceCaps = create_iospace_caps(ریشه_cnode_cap);
        إذا (ndks_boot.bi_قاب->ioSpaceCaps.شروع == 0 &&
            ndks_boot.bi_قاب->ioSpaceCaps.انتها == 0) {
            چاپ_کن("ERROR: SMMU I/O space creation failed\n");
            أرجع false;
        }
    } وإلا {
        ndks_boot.bi_قاب->ioSpaceCaps = S_REG_خالی;
    }

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_آدرس_space(ریشه_cnode_cap, it_v_reg);
    إذا (cap_get_capType(it_pd_cap) == cap_null_cap) {
        چاپ_کن("ERROR: address space creation for initial thread failed\n");
        أرجع false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_قاب_cap(
        ریشه_cnode_cap,
        it_pd_cap,
        bi_قاب_vptr
    );

    /* create and map extra bootinfo region */
    إذا (extra_bi_اندازه > 0) {
        region_t extra_bi_region = {
            .شروع = rootserver.extra_bi,
            .انتها = rootserver.extra_bi + extra_bi_اندازه
        };
        extra_bi_ret =
            create_قابها_از_region(
                ریشه_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_مقصد_paddr((فراغ *)extra_bi_region.شروع) - extra_bi_قاب_vptr
            );
        إذا (!extra_bi_ret.success) {
            چاپ_کن("ERROR: mapping extra boot info to initial thread failed\n");
            أرجع false;
        }
        ndks_boot.bi_قاب->extraBIPages = extra_bi_ret.region;
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    init_sched_control(ریشه_cnode_cap, CONFIG_MAX_رقم_NODES);
#پایان_اگر

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_قاب_cap(ریشه_cnode_cap, it_pd_cap, ipcbuf_vptr);
    إذا (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        چاپ_کن("ERROR: could not create IPC buffer for initial thread\n");
        أرجع false;
    }

    /* create all userland image frames */
    create_قابها_ret =
        create_قابها_از_region(
            ریشه_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_جابه_جایی
        );
    إذا (!create_قابها_ret.success) {
        چاپ_کن("ERROR: could not create all userland image frames\n");
        أرجع false;
    }
    ndks_boot.bi_قاب->user_Image_قابها = create_قابها_ret.region;

    /* create/initialise the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(ریشه_cnode_cap);
    إذا (cap_get_capType(it_ap_cap) == cap_null_cap) {
        چاپ_کن("ERROR: could not create ASID pool for initial thread\n");
        أرجع false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    گره_STATE(ks_Cur_زمان) = get_Current_زمان();
#پایان_اگر

    /* create the idle thread */
    create_idle_thread();

    /* Before creating the initial thread (which also switches to it)
     * we clean the cache so that any page table information written
     * as a result of calling create_frames_of_region will be correctly
     * read by the hardware page table walker */
    cleanInvalidateL1Caches();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         ریشه_cnode_cap,
                         it_pd_cap,
                         v_مدخل,
                         bi_قاب_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    إذا (initial == عدم) {
        چاپ_کن("ERROR: could not create initial thread\n");
        أرجع false;
    }

    init_core_state(initial);

    /* create all of the untypeds. Both devices and kernel window memory */
    إذا (!create_untypeds(ریشه_cnode_cap)) {
        چاپ_کن("ERROR: could not create untypteds for kernel image boot memory\n");
        أرجع false;
    }

    /* no shared-frame caps (ARM has no multikernel support) */
    ndks_boot.bi_قاب->shared_قابها = S_REG_خالی;

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
    إذا (config_مجموعه(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        invalidateHypTLB();
    }

    ks_رقم_CPUs = 1;

    /* initialize BKL before booting up other cores */
    SMP_COND_STATEMENT(clh_قفل_init());
    SMP_COND_STATEMENT(release_secondary_cpus());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    گره_قفل_SYS;

    چاپ_کن("Booting all finished, dropped to user space\n");

    /* kernel successfully initialized */
    أرجع true;
}

BOOT_کد VISIBLE فراغ init_kernel(
    paddr_t ui_p_reg_شروع,
    paddr_t ui_p_reg_انتها,
    sword_t pv_جابه_جایی,
    vptr_t  v_مدخل,
    paddr_t dtb_addr_p,
    uint32_t dtb_اندازه
)
{
    bool_t نتیجه;

#اگر_تعریف_شده ENABLE_SMP_SUPPORT
    /* we assume there exists a cpu with id 0 and will use it for bootstrapping */
    إذا (getCurrentCPUIndex() == 0) {
        نتیجه = try_init_kernel(ui_p_reg_شروع,
                                 ui_p_reg_انتها,
                                 pv_جابه_جایی,
                                 v_مدخل,
                                 dtb_addr_p, dtb_اندازه);
    } وإلا {
        نتیجه = try_init_kernel_secondary_core();
    }

#وإلا
    نتیجه = try_init_kernel(ui_p_reg_شروع,
                             ui_p_reg_انتها,
                             pv_جابه_جایی,
                             v_مدخل,
                             dtb_addr_p, dtb_اندازه);

#پایان_اگر /* ENABLE_SMP_SUPPORT */

    إذا (!نتیجه) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    گره_STATE(ks_Cur_زمان) = get_Current_زمان();
    گره_STATE(ksConsumed) = 0;
#پایان_اگر
    زمانبندی();
    activateThread();
}
