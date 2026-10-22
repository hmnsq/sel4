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

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_SMMU
#ضمن <drivers/smmu/smmuv2.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS ساكن متطاير _Atomic صحيح ڳنڍ_boot_تالو;
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

BOOT_BSS ساكن region_t reserved[انگ_RESERVED_REGIONS];

BOOT_ڪوڊ ساكن bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          لفظ_t extra_bi_سائيز_بٽ)
{
    /* reserve the kernel image region */
    reserved[0] = paddr_منزل_pptr_reg(get_p_reg_kernel_img());

    صحيح اشاريو = 1;

    /* add the dtb region, if it is not empty */
    إذا (dtb_p_reg.شروعات) {
        إذا (اشاريو >= ARRAY_سائيز(reserved)) {
            لکو("ERROR: no slot to add DTB to reserved regions\n");
            أرجع false;
        }
        reserved[اشاريو].شروعات = (pptr_t) paddr_منزل_pptr(dtb_p_reg.شروعات);
        reserved[اشاريو].پڇاڙي = (pptr_t) paddr_منزل_pptr(dtb_p_reg.پڇاڙي);
        اشاريو++;
    }

    /* Reserve the user image region and the mode-reserved regions. For now,
     * only one mode-reserved region is supported, because this is all that is
     * needed.
     */
    إذا (گهڻائي_RESERVED > 1) {
        لکو("ERROR: MODE_RESERVED > 1 unsupported!\n");
        أرجع false;
    }
    إذا (ui_p_reg.شروعات < PADDR_چوٽي) {
        region_t ui_reg = paddr_منزل_pptr_reg(ui_p_reg);
        إذا (گهڻائي_RESERVED == 1) {
            إذا (اشاريو + 1 >= ARRAY_سائيز(reserved)) {
                لکو("ERROR: no slot to add the user image and the "
                       "mode-reserved region to the reserved regions\n");
                أرجع false;
            }
            إذا (ui_reg.پڇاڙي > گهڻائي_reserved_region[0].شروعات) {
                reserved[اشاريو] = گهڻائي_reserved_region[0];
                اشاريو++;
                reserved[اشاريو] = ui_reg;
            } وإلا {
                reserved[اشاريو] = ui_reg;
                اشاريو++;
                reserved[اشاريو] = گهڻائي_reserved_region[0];
            }
            اشاريو++;
        } وإلا {
            إذا (اشاريو >= ARRAY_سائيز(reserved)) {
                لکو("ERROR: no slot to add the user image to the reserved"
                       "regions\n");
                أرجع false;
            }
            reserved[اشاريو] = ui_reg;
            اشاريو++;
        }
    } وإلا {
        إذا (گهڻائي_RESERVED == 1) {
            إذا (اشاريو >= ARRAY_سائيز(reserved)) {
                لکو("ERROR: no slot to add the mode-reserved region\n");
                أرجع false;
            }
            reserved[اشاريو] = گهڻائي_reserved_region[0];
            اشاريو++;
        }

        /* Reserve the ui_p_reg region still so it doesn't get turned into device UT. */
        reserve_region(ui_p_reg);
    }

    /* avail_p_regs comes from the auto-generated code */
    أرجع init_freemem(ARRAY_سائيز(avail_p_regs), avail_p_regs,
                        اشاريو, reserved,
                        it_v_reg, extra_bi_سائيز_بٽ);
}


BOOT_ڪوڊ ساكن فراغ init_irqs(cap_t پاڙ_cnode_cap)
{
    بنانشان i;

    لكل (i = 0; i <= maxIRQ ; i++) {
        سيٽ_IRQState(IRQInactive, CORE_IRQ_منزل_IRQT(0, i));
    }
    سيٽ_IRQState(IRQTimer, CORE_IRQ_منزل_IRQT(0, KERNEL_TIMER_IRQ));
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    سيٽ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(0, مداخلت_VGIC_MAINTENANCE));
    سيٽ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(0, مداخلت_VTIMER_EVENT));
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
    سيٽ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(0, مداخلت_SMMU));
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_ENABLE_PMU_حد_کان_مٿي_مداخلت
#جيڪڏهن_وصف_ٿيل KERNEL_PMU_IRQ
    سيٽ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(0, KERNEL_PMU_IRQ));
#إذا (defined CONFIG_PLAT_TX1 && defined ENABLE_SMP_SUPPORT)
//SELFOUR-1252
#غلطي "This platform doesn't support tracking CPU utilisation on multicore"
#جيڪڏهن_پورو /* CONFIG_PLAT_TX1 && ENABLE_SMP_SUPPORT */
#وإلا
#غلطي "This platform doesn't support tracking CPU utilisation feature"
#جيڪڏهن_پورو /* KERNEL_TIMER_IRQ */
#جيڪڏهن_پورو /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    سيٽ_IRQState(IRQIPI, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    سيٽ_IRQState(IRQIPI, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
#جيڪڏهن_پورو

    /* provide the IRQ control cap */
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_SMMU
BOOT_ڪوڊ ساكن فراغ init_smmu(cap_t پاڙ_cnode_cap)
{
    plat_smmu_init();
    /*provide the SID and CB control cap*/
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapSMMUSIDControl), cap_sid_control_cap_new());
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapSMMUCBControl), cap_cb_control_cap_new());
}

#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_ALLOW_SMC_CALLS
BOOT_ڪوڊ ساكن فراغ init_smc(cap_t پاڙ_cnode_cap)
{
    /* Provide the SMC cap*/
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapSMC), cap_smc_cap_new(0));
}
#جيڪڏهن_پورو

/** This and only this function initialises the CPU.
 *
 * It does NOT initialise any kernel state.
 * @return For the verification build, this currently returns true always.
 */
BOOT_ڪوڊ ساكن bool_t init_cpu(فراغ)
{
    bool_t haveHWFPU;

#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH64
    إذا (config_سيٽ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        إذا (!checkTCR_EL2()) {
            أرجع false;
        }
    }
#جيڪڏهن_پورو

    activate_kernel_vspace();
    إذا (config_سيٽ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_boot_init();
    }

#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH64
    /* After activate_kernel_vspace(), so printf() is available. */
    إذا (!config_سيٽ(CONFIG_ARM_HYPERVISOR_SUPPORT) && !check16_ٻٽو_انگ_ASID()) {
        لکو("ERROR: kernel requires 16-bit ASIDs\n");
        أرجع false;
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_HARDWARE_DEBUG_API
    إذا (!Arch_initHardwareBreakpoints()) {
        لکو("Kernel built with CONFIG_HARDWARE_DEBUG_API, but this board doesn't "
               "reliably support it.\n");
        أرجع false;
    }
#جيڪڏهن_پورو

    /* Setup kernel stack pointer.
     * On ARM SMP, the array index here is the CPU ID
     */
    لفظ_t ڍير_چوٽي = ((لفظ_t) kernel_ڍير_alloc[CURRENT_CPU_اشاريو()]) + ٻٽو_انگ(CONFIG_KERNEL_ڍير_بٽ);
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH64
    /* the least 12 bits are used to store logical core ID */
    ڍير_چوٽي |= getCurrentCPUIndex();
#وإلاإذا defined(CONFIG_ARCH_AARCH32)
    /* Stack address encodes core ID, ensure it is in the right region */
    ڍير_چوٽي -= 8;
#جيڪڏهن_پورو
#جيڪڏهن_پورو
    سيٽ_Kernel_ڍير(ڍير_چوٽي);

#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH64
    /* initialise CPU's exception vector table */
    سيٽ_Vtable((pptr_t)arm_ويڪٽر_جدول);
#جيڪڏهن_پورو /* CONFIG_ARCH_AARCH64 */

    haveHWFPU = fpsimd_HWCapTest();

    /* Disable FPU to avoid channels where a platform has an FPU but doesn't make use of it */
    إذا (haveHWFPU) {
        disableFpu();
    }

#جيڪڏهن_وصف_ٿيل CONFIG_HAVE_FPU
    إذا (haveHWFPU) {
        إذا (!fpsimd_init()) {
            أرجع false;
        }
    } وإلا {
        لکو("Platform claims to have FP hardware, but does not!\n");
        أرجع false;
    }
#جيڪڏهن_پورو /* CONFIG_HAVE_FPU */

    cpu_initLocalIRQController();

#جيڪڏهن_وصف_ٿيل CONFIG_ENABLE_BENCHMARKS
    arm_init_ccnt();
#جيڪڏهن_پورو /* CONFIG_ENABLE_BENCHMARKS */

    /* Export selected CPU features for access by PL0 */
    armv_init_user_access();

    initTimer();

    أرجع true;
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_ڪوڊ ساكن فراغ init_plat(فراغ)
{
    initIRQController();
    initL2Cache();
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_SMMU
    plat_smmu_init();
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
BOOT_ڪوڊ ساكن bool_t try_init_kernel_secondary_core(فراغ)
{
    /* need to first wait until some kernel init has been done */
    طالما (!ڳنڍ_boot_تالو);

    /* Perform cpu init */
    init_cpu();

    لكل (بنانشان صحيح i = 0; i < انگ_PPI; i++) {
        نقاب_مداخلت(true, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), i));
    }
    سيٽ_IRQState(IRQIPI, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    سيٽ_IRQState(IRQIPI, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
    /* Enable per-CPU timer interrupts */
    سيٽ_IRQState(IRQTimer, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), KERNEL_TIMER_IRQ));
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    سيٽ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), مداخلت_VGIC_MAINTENANCE));
    سيٽ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), مداخلت_VTIMER_EVENT));
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    ڳنڍ_تالو_SYS;

    clock_sync_test();
    ks_انگ_CPUs++;

    init_core_state(SchedulerAction_ResumeCurrentThread);

    أرجع true;
}

BOOT_ڪوڊ ساكن فراغ release_secondary_cpus(فراغ)
{
    /* release the cpus at the same time */
    أكد(0 == ڳنڍ_boot_تالو); /* Sanity check for a proper lock state. */
    ڳنڍ_boot_تالو = 1;

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
#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH32
    cleanInvalidateL1Caches();
    plat_cleanInvalidateL2Cache();
#جيڪڏهن_پورو

    /* Wait until all the secondary cores are done initialising */
    طالما (ks_انگ_CPUs != CONFIG_MAX_انگ_NODES) {
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        ڳنڍ_STATE(ks_Cur_وقت) = get_Current_وقت();
#جيڪڏهن_پورو
        /* perform a memory acquire to get new values of ksNumCPUs, release for ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

ساكن BOOT_ڪوڊ bool_t try_init_kernel(
    paddr_t ui_p_reg_شروعات,
    paddr_t ui_p_reg_پڇاڙي,
    sword_t pv_وٿي,
    vptr_t  v_داخلا,
    paddr_t dtb_phys_addr,
    لفظ_t  dtb_سائيز
)
{
    cap_t پاڙ_cnode_cap;
    cap_t it_ap_cap;
    cap_t it_pd_cap;
    cap_t ipcbuf_cap;
    p_region_t ui_p_reg = (p_region_t) {
        ui_p_reg_شروعات, ui_p_reg_پڇاڙي
    };
    region_t ui_reg = paddr_منزل_pptr_reg(ui_p_reg);
    لفظ_t extra_bi_سائيز = 0;
    pptr_t extra_bi_وٿي = 0;
    vptr_t extra_bi_چوکٺو_vptr;
    vptr_t bi_چوکٺو_vptr;
    vptr_t ipcbuf_vptr;
    create_چوکٺا_جو_region_ret_t create_چوکٺا_ret;
    create_چوکٺا_جو_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .شروعات = ui_p_reg_شروعات - pv_وٿي,
        .پڇاڙي   = ui_p_reg_پڇاڙي   - pv_وٿي
    };

    ipcbuf_vptr = ui_v_reg.پڇاڙي;
    bi_چوکٺو_vptr = ipcbuf_vptr + ٻٽو_انگ(صفحو_بٽ);
    extra_bi_چوکٺو_vptr = bi_چوکٺو_vptr + ٻٽو_انگ(se_L4_Boot_Info_چوکٺو_بٽ);

    /* setup virtual memory for the kernel */
    map_kernel_دري();

    /* initialise the CPU */
    إذا (!init_cpu()) {
        لکو("ERROR: CPU init failed\n");
        أرجع false;
    }

    /* debug output via serial port is only available from here */
    لکو("Bootstrapping kernel\n");

    /* initialise the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_خالي_جايون;
    إذا (dtb_سائيز > 0) {
        paddr_t dtb_phys_پڇاڙي = dtb_phys_addr + dtb_سائيز;
        إذا (dtb_phys_پڇاڙي < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            لکو("ERROR: DTB location at %"SEL4_PRIx_لفظ
                   " len %"SEL4_PRIu_لفظ" invalid\n",
                   dtb_phys_addr, dtb_سائيز);
            أرجع false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        إذا (dtb_phys_پڇاڙي >= PADDR_چوٽي) {
            لکو("ERROR: DTB at [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_لفظ")\n",
                   dtb_phys_addr, dtb_phys_پڇاڙي, PADDR_چوٽي);
            أرجع false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_سائيز += حجم(se_L4_Boot_Info_سرو) + dtb_سائيز;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .شروعات = dtb_phys_addr,
            .پڇاڙي   = dtb_phys_پڇاڙي
        };
    }

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    لفظ_t extra_bi_سائيز_بٽ = calculate_extra_bi_سائيز_بٽ(extra_bi_سائيز);
    v_region_t it_v_reg = {
        .شروعات = ui_v_reg.شروعات,
        .پڇاڙي   = extra_bi_چوکٺو_vptr + (extra_bi_سائيز_بٽ > 0 ? ٻٽو_انگ(extra_bi_سائيز_بٽ) : 0)
    };
    إذا (it_v_reg.پڇاڙي >= USER_چوٽي) {
        /* Variable arguments for printf() require well defined integer types to
         * work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        لکو("ERROR: userland image virt [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ")"
               "exceeds USER_TOP (%"SEL4_PRIx_لفظ")\n",
               it_v_reg.شروعات, it_v_reg.پڇاڙي, (لفظ_t)USER_چوٽي);
        أرجع false;
    }

    إذا (!arch_init_freemem(ui_p_reg, dtb_p_reg, it_v_reg, extra_bi_سائيز_بٽ)) {
        لکو("ERROR: free memory management initialization failed\n");
        أرجع false;
    }

    /* create the root cnode */
    پاڙ_cnode_cap = create_پاڙ_cnode();
    إذا (cap_get_capType(پاڙ_cnode_cap) == cap_null_cap) {
        لکو("ERROR: root c-node creation failed\n");
        أرجع false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(پاڙ_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(پاڙ_cnode_cap);

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_SMMU
    /* initialise the SMMU and provide the SMMU control caps*/
    init_smmu(پاڙ_cnode_cap);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_ALLOW_SMC_CALLS
    init_smc(پاڙ_cnode_cap);
#جيڪڏهن_پورو

    populate_bi_چوکٺو(0, CONFIG_MAX_انگ_NODES, ipcbuf_vptr, extra_bi_سائيز);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_سرو سرو;
    إذا (dtb_سائيز > 0) {
        سرو.id = SEL4_BOOTINFO_سرو_FDT;
        سرو.len = حجم(سرو) + dtb_سائيز;
        *(se_L4_Boot_Info_سرو *)(rootserver.extra_bi + extra_bi_وٿي) = سرو;
        extra_bi_وٿي += حجم(سرو);
        يادگيري_نقل((فراغ *)(rootserver.extra_bi + extra_bi_وٿي),
               paddr_منزل_pptr(dtb_phys_addr),
               dtb_سائيز);
        extra_bi_وٿي += dtb_سائيز;
    }

    أكد(extra_bi_سائيز == extra_bi_وٿي);

    إذا (config_سيٽ(CONFIG_TK1_SMMU)) {
        ndks_boot.bi_چوکٺو->ioSpaceCaps = create_iospace_caps(پاڙ_cnode_cap);
        إذا (ndks_boot.bi_چوکٺو->ioSpaceCaps.شروعات == 0 &&
            ndks_boot.bi_چوکٺو->ioSpaceCaps.پڇاڙي == 0) {
            لکو("ERROR: SMMU I/O space creation failed\n");
            أرجع false;
        }
    } وإلا {
        ndks_boot.bi_چوکٺو->ioSpaceCaps = S_REG_خالي_جايون;
    }

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_پتو_space(پاڙ_cnode_cap, it_v_reg);
    إذا (cap_get_capType(it_pd_cap) == cap_null_cap) {
        لکو("ERROR: address space creation for initial thread failed\n");
        أرجع false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_چوکٺو_cap(
        پاڙ_cnode_cap,
        it_pd_cap,
        bi_چوکٺو_vptr
    );

    /* create and map extra bootinfo region */
    إذا (extra_bi_سائيز > 0) {
        region_t extra_bi_region = {
            .شروعات = rootserver.extra_bi,
            .پڇاڙي = rootserver.extra_bi + extra_bi_سائيز
        };
        extra_bi_ret =
            create_چوکٺا_جو_region(
                پاڙ_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_منزل_paddr((فراغ *)extra_bi_region.شروعات) - extra_bi_چوکٺو_vptr
            );
        إذا (!extra_bi_ret.success) {
            لکو("ERROR: mapping extra boot info to initial thread failed\n");
            أرجع false;
        }
        ndks_boot.bi_چوکٺو->extraBIPages = extra_bi_ret.region;
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    init_sched_control(پاڙ_cnode_cap, CONFIG_MAX_انگ_NODES);
#جيڪڏهن_پورو

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_چوکٺو_cap(پاڙ_cnode_cap, it_pd_cap, ipcbuf_vptr);
    إذا (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        لکو("ERROR: could not create IPC buffer for initial thread\n");
        أرجع false;
    }

    /* create all userland image frames */
    create_چوکٺا_ret =
        create_چوکٺا_جو_region(
            پاڙ_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_وٿي
        );
    إذا (!create_چوکٺا_ret.success) {
        لکو("ERROR: could not create all userland image frames\n");
        أرجع false;
    }
    ndks_boot.bi_چوکٺو->user_Image_چوکٺا = create_چوکٺا_ret.region;

    /* create/initialise the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(پاڙ_cnode_cap);
    إذا (cap_get_capType(it_ap_cap) == cap_null_cap) {
        لکو("ERROR: could not create ASID pool for initial thread\n");
        أرجع false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    ڳنڍ_STATE(ks_Cur_وقت) = get_Current_وقت();
#جيڪڏهن_پورو

    /* create the idle thread */
    create_idle_thread();

    /* Before creating the initial thread (which also switches to it)
     * we clean the cache so that any page table information written
     * as a result of calling create_frames_of_region will be correctly
     * read by the hardware page table walker */
    cleanInvalidateL1Caches();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         پاڙ_cnode_cap,
                         it_pd_cap,
                         v_داخلا,
                         bi_چوکٺو_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    إذا (initial == عدم) {
        لکو("ERROR: could not create initial thread\n");
        أرجع false;
    }

    init_core_state(initial);

    /* create all of the untypeds. Both devices and kernel window memory */
    إذا (!create_untypeds(پاڙ_cnode_cap)) {
        لکو("ERROR: could not create untypteds for kernel image boot memory\n");
        أرجع false;
    }

    /* no shared-frame caps (ARM has no multikernel support) */
    ndks_boot.bi_چوکٺو->shared_چوکٺا = S_REG_خالي_جايون;

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
    إذا (config_سيٽ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        invalidateHypTLB();
    }

    ks_انگ_CPUs = 1;

    /* initialize BKL before booting up other cores */
    SMP_COND_STATEMENT(clh_تالو_init());
    SMP_COND_STATEMENT(release_secondary_cpus());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    ڳنڍ_تالو_SYS;

    لکو("Booting all finished, dropped to user space\n");

    /* kernel successfully initialized */
    أرجع true;
}

BOOT_ڪوڊ VISIBLE فراغ init_kernel(
    paddr_t ui_p_reg_شروعات,
    paddr_t ui_p_reg_پڇاڙي,
    sword_t pv_وٿي,
    vptr_t  v_داخلا,
    paddr_t dtb_addr_p,
    uint32_t dtb_سائيز
)
{
    bool_t نتيجو;

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    /* we assume there exists a cpu with id 0 and will use it for bootstrapping */
    إذا (getCurrentCPUIndex() == 0) {
        نتيجو = try_init_kernel(ui_p_reg_شروعات,
                                 ui_p_reg_پڇاڙي,
                                 pv_وٿي,
                                 v_داخلا,
                                 dtb_addr_p, dtb_سائيز);
    } وإلا {
        نتيجو = try_init_kernel_secondary_core();
    }

#وإلا
    نتيجو = try_init_kernel(ui_p_reg_شروعات,
                             ui_p_reg_پڇاڙي,
                             pv_وٿي,
                             v_داخلا,
                             dtb_addr_p, dtb_سائيز);

#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

    إذا (!نتيجو) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    ڳنڍ_STATE(ks_Cur_وقت) = get_Current_وقت();
    ڳنڍ_STATE(ksConsumed) = 0;
#جيڪڏهن_پورو
    وقت_جدول();
    activateThread();
}
