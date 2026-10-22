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

#جے_تعریف_شدہ CONFIG_ARM_SMMU
#ضمن <drivers/smmu/smmuv2.h>
#جے_انت

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS ساكن متطاير _Atomic صحيح گرہ_boot_جندرا;
#جے_انت /* ENABLE_SMP_SUPPORT */

BOOT_BSS ساكن region_t reserved[عدد_RESERVED_REGIONS];

BOOT_کوڈ ساكن bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          لفظ_t extra_bi_ناپ_بٹ)
{
    /* reserve the kernel image region */
    reserved[0] = paddr_منزل_pptr_reg(get_p_reg_kernel_img());

    صحيح اشاریہ = 1;

    /* add the dtb region, if it is not empty */
    إذا (dtb_p_reg.شروع) {
        إذا (اشاریہ >= ARRAY_ناپ(reserved)) {
            لکھو("ERROR: no slot to add DTB to reserved regions\n");
            أرجع false;
        }
        reserved[اشاریہ].شروع = (pptr_t) paddr_منزل_pptr(dtb_p_reg.شروع);
        reserved[اشاریہ].سرا = (pptr_t) paddr_منزل_pptr(dtb_p_reg.سرا);
        اشاریہ++;
    }

    /* Reserve the user image region and the mode-reserved regions. For now,
     * only one mode-reserved region is supported, because this is all that is
     * needed.
     */
    إذا (منوال_RESERVED > 1) {
        لکھو("ERROR: MODE_RESERVED > 1 unsupported!\n");
        أرجع false;
    }
    إذا (ui_p_reg.شروع < PADDR_چوٹی) {
        region_t ui_reg = paddr_منزل_pptr_reg(ui_p_reg);
        إذا (منوال_RESERVED == 1) {
            إذا (اشاریہ + 1 >= ARRAY_ناپ(reserved)) {
                لکھو("ERROR: no slot to add the user image and the "
                       "mode-reserved region to the reserved regions\n");
                أرجع false;
            }
            إذا (ui_reg.سرا > منوال_reserved_region[0].شروع) {
                reserved[اشاریہ] = منوال_reserved_region[0];
                اشاریہ++;
                reserved[اشاریہ] = ui_reg;
            } وإلا {
                reserved[اشاریہ] = ui_reg;
                اشاریہ++;
                reserved[اشاریہ] = منوال_reserved_region[0];
            }
            اشاریہ++;
        } وإلا {
            إذا (اشاریہ >= ARRAY_ناپ(reserved)) {
                لکھو("ERROR: no slot to add the user image to the reserved"
                       "regions\n");
                أرجع false;
            }
            reserved[اشاریہ] = ui_reg;
            اشاریہ++;
        }
    } وإلا {
        إذا (منوال_RESERVED == 1) {
            إذا (اشاریہ >= ARRAY_ناپ(reserved)) {
                لکھو("ERROR: no slot to add the mode-reserved region\n");
                أرجع false;
            }
            reserved[اشاریہ] = منوال_reserved_region[0];
            اشاریہ++;
        }

        /* Reserve the ui_p_reg region still so it doesn't get turned into device UT. */
        reserve_region(ui_p_reg);
    }

    /* avail_p_regs comes from the auto-generated code */
    أرجع init_freemem(ARRAY_ناپ(avail_p_regs), avail_p_regs,
                        اشاریہ, reserved,
                        it_v_reg, extra_bi_ناپ_بٹ);
}


BOOT_کوڈ ساكن فراغ init_irqs(cap_t جڑ_cnode_cap)
{
    بنانشانی i;

    لكل (i = 0; i <= maxIRQ ; i++) {
        سیٹ_IRQState(IRQInactive, CORE_IRQ_منزل_IRQT(0, i));
    }
    سیٹ_IRQState(IRQTimer, CORE_IRQ_منزل_IRQT(0, KERNEL_TIMER_IRQ));
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    سیٹ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(0, رکاوٹ_VGIC_MAINTENANCE));
    سیٹ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(0, رکاوٹ_VTIMER_EVENT));
#جے_انت
#جے_تعریف_شدہ CONFIG_TK1_SMMU
    سیٹ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(0, رکاوٹ_SMMU));
#جے_انت

#جے_تعریف_شدہ CONFIG_ARM_ENABLE_PMU_حد_توں_ودھ_رکاوٹ
#جے_تعریف_شدہ KERNEL_PMU_IRQ
    سیٹ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(0, KERNEL_PMU_IRQ));
#إذا (defined CONFIG_PLAT_TX1 && defined ENABLE_SMP_SUPPORT)
//SELFOUR-1252
#غلطی "This platform doesn't support tracking CPU utilisation on multicore"
#جے_انت /* CONFIG_PLAT_TX1 && ENABLE_SMP_SUPPORT */
#وإلا
#غلطی "This platform doesn't support tracking CPU utilisation feature"
#جے_انت /* KERNEL_TIMER_IRQ */
#جے_انت /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    سیٹ_IRQState(IRQIPI, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    سیٹ_IRQState(IRQIPI, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
#جے_انت

    /* provide the IRQ control cap */
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

#جے_تعریف_شدہ CONFIG_ARM_SMMU
BOOT_کوڈ ساكن فراغ init_smmu(cap_t جڑ_cnode_cap)
{
    plat_smmu_init();
    /*provide the SID and CB control cap*/
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapSMMUSIDControl), cap_sid_control_cap_new());
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapSMMUCBControl), cap_cb_control_cap_new());
}

#جے_انت

#جے_تعریف_شدہ CONFIG_ALLOW_SMC_CALLS
BOOT_کوڈ ساكن فراغ init_smc(cap_t جڑ_cnode_cap)
{
    /* Provide the SMC cap*/
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapSMC), cap_smc_cap_new(0));
}
#جے_انت

/** This and only this function initialises the CPU.
 *
 * It does NOT initialise any kernel state.
 * @return For the verification build, this currently returns true always.
 */
BOOT_کوڈ ساكن bool_t init_cpu(فراغ)
{
    bool_t haveHWFPU;

#جے_تعریف_شدہ CONFIG_ARCH_AARCH64
    إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        إذا (!checkTCR_EL2()) {
            أرجع false;
        }
    }
#جے_انت

    activate_kernel_vspace();
    إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_boot_init();
    }

#جے_تعریف_شدہ CONFIG_ARCH_AARCH64
    /* After activate_kernel_vspace(), so printf() is available. */
    إذا (!config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT) && !check16_ثنائی_ہندسہ_ASID()) {
        لکھو("ERROR: kernel requires 16-bit ASIDs\n");
        أرجع false;
    }
#جے_انت

#جے_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
    إذا (!Arch_initHardwareBreakpoints()) {
        لکھو("Kernel built with CONFIG_HARDWARE_DEBUG_API, but this board doesn't "
               "reliably support it.\n");
        أرجع false;
    }
#جے_انت

    /* Setup kernel stack pointer.
     * On ARM SMP, the array index here is the CPU ID
     */
    لفظ_t ڈھیر_چوٹی = ((لفظ_t) kernel_ڈھیر_alloc[CURRENT_CPU_اشاریہ()]) + ثنائی_ہندسہ(CONFIG_KERNEL_ڈھیر_بٹ);
#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
#جے_تعریف_شدہ CONFIG_ARCH_AARCH64
    /* the least 12 bits are used to store logical core ID */
    ڈھیر_چوٹی |= getCurrentCPUIndex();
#وإلاإذا defined(CONFIG_ARCH_AARCH32)
    /* Stack address encodes core ID, ensure it is in the right region */
    ڈھیر_چوٹی -= 8;
#جے_انت
#جے_انت
    سیٹ_Kernel_ڈھیر(ڈھیر_چوٹی);

#جے_تعریف_شدہ CONFIG_ARCH_AARCH64
    /* initialise CPU's exception vector table */
    سیٹ_Vtable((pptr_t)arm_سمتیہ_جدول);
#جے_انت /* CONFIG_ARCH_AARCH64 */

    haveHWFPU = fpsimd_HWCapTest();

    /* Disable FPU to avoid channels where a platform has an FPU but doesn't make use of it */
    إذا (haveHWFPU) {
        disableFpu();
    }

#جے_تعریف_شدہ CONFIG_HAVE_FPU
    إذا (haveHWFPU) {
        إذا (!fpsimd_init()) {
            أرجع false;
        }
    } وإلا {
        لکھو("Platform claims to have FP hardware, but does not!\n");
        أرجع false;
    }
#جے_انت /* CONFIG_HAVE_FPU */

    cpu_initLocalIRQController();

#جے_تعریف_شدہ CONFIG_ENABLE_BENCHMARKS
    arm_init_ccnt();
#جے_انت /* CONFIG_ENABLE_BENCHMARKS */

    /* Export selected CPU features for access by PL0 */
    armv_init_user_access();

    initTimer();

    أرجع true;
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_کوڈ ساكن فراغ init_plat(فراغ)
{
    initIRQController();
    initL2Cache();
#جے_تعریف_شدہ CONFIG_ARM_SMMU
    plat_smmu_init();
#جے_انت
}

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
BOOT_کوڈ ساكن bool_t try_init_kernel_secondary_core(فراغ)
{
    /* need to first wait until some kernel init has been done */
    طالما (!گرہ_boot_جندرا);

    /* Perform cpu init */
    init_cpu();

    لكل (بنانشانی صحيح i = 0; i < عدد_PPI; i++) {
        نقاب_رکاوٹ(true, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), i));
    }
    سیٹ_IRQState(IRQIPI, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    سیٹ_IRQState(IRQIPI, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
    /* Enable per-CPU timer interrupts */
    سیٹ_IRQState(IRQTimer, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), KERNEL_TIMER_IRQ));
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    سیٹ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), رکاوٹ_VGIC_MAINTENANCE));
    سیٹ_IRQState(IRQReserved, CORE_IRQ_منزل_IRQT(getCurrentCPUIndex(), رکاوٹ_VTIMER_EVENT));
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    گرہ_جندرا_SYS;

    clock_sync_test();
    ks_عدد_CPUs++;

    init_core_state(SchedulerAction_ResumeCurrentThread);

    أرجع true;
}

BOOT_کوڈ ساكن فراغ release_secondary_cpus(فراغ)
{
    /* release the cpus at the same time */
    أكد(0 == گرہ_boot_جندرا); /* Sanity check for a proper lock state. */
    گرہ_boot_جندرا = 1;

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
#جے_تعریف_شدہ CONFIG_ARCH_AARCH32
    cleanInvalidateL1Caches();
    plat_cleanInvalidateL2Cache();
#جے_انت

    /* Wait until all the secondary cores are done initialising */
    طالما (ks_عدد_CPUs != CONFIG_MAX_عدد_NODES) {
#جے_تعریف_شدہ ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        گرہ_STATE(ks_Cur_ویلا) = get_Current_ویلا();
#جے_انت
        /* perform a memory acquire to get new values of ksNumCPUs, release for ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#جے_انت /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

ساكن BOOT_کوڈ bool_t try_init_kernel(
    paddr_t ui_p_reg_شروع,
    paddr_t ui_p_reg_سرا,
    sword_t pv_وتھ,
    vptr_t  v_اندراج,
    paddr_t dtb_phys_addr,
    لفظ_t  dtb_ناپ
)
{
    cap_t جڑ_cnode_cap;
    cap_t it_ap_cap;
    cap_t it_pd_cap;
    cap_t ipcbuf_cap;
    p_region_t ui_p_reg = (p_region_t) {
        ui_p_reg_شروع, ui_p_reg_سرا
    };
    region_t ui_reg = paddr_منزل_pptr_reg(ui_p_reg);
    لفظ_t extra_bi_ناپ = 0;
    pptr_t extra_bi_وتھ = 0;
    vptr_t extra_bi_چوکھٹا_vptr;
    vptr_t bi_چوکھٹا_vptr;
    vptr_t ipcbuf_vptr;
    create_چوکھٹے_دا_region_ret_t create_چوکھٹے_ret;
    create_چوکھٹے_دا_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .شروع = ui_p_reg_شروع - pv_وتھ,
        .سرا   = ui_p_reg_سرا   - pv_وتھ
    };

    ipcbuf_vptr = ui_v_reg.سرا;
    bi_چوکھٹا_vptr = ipcbuf_vptr + ثنائی_ہندسہ(ورقہ_بٹ);
    extra_bi_چوکھٹا_vptr = bi_چوکھٹا_vptr + ثنائی_ہندسہ(se_L4_Boot_Info_چوکھٹا_بٹ);

    /* setup virtual memory for the kernel */
    map_kernel_کھڑکی();

    /* initialise the CPU */
    إذا (!init_cpu()) {
        لکھو("ERROR: CPU init failed\n");
        أرجع false;
    }

    /* debug output via serial port is only available from here */
    لکھو("Bootstrapping kernel\n");

    /* initialise the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_خالی_تھاواں;
    إذا (dtb_ناپ > 0) {
        paddr_t dtb_phys_سرا = dtb_phys_addr + dtb_ناپ;
        إذا (dtb_phys_سرا < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            لکھو("ERROR: DTB location at %"SEL4_PRIx_لفظ
                   " len %"SEL4_PRIu_لفظ" invalid\n",
                   dtb_phys_addr, dtb_ناپ);
            أرجع false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        إذا (dtb_phys_سرا >= PADDR_چوٹی) {
            لکھو("ERROR: DTB at [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_لفظ")\n",
                   dtb_phys_addr, dtb_phys_سرا, PADDR_چوٹی);
            أرجع false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_ناپ += حجم(se_L4_Boot_Info_سرخی) + dtb_ناپ;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .شروع = dtb_phys_addr,
            .سرا   = dtb_phys_سرا
        };
    }

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    لفظ_t extra_bi_ناپ_بٹ = calculate_extra_bi_ناپ_بٹ(extra_bi_ناپ);
    v_region_t it_v_reg = {
        .شروع = ui_v_reg.شروع,
        .سرا   = extra_bi_چوکھٹا_vptr + (extra_bi_ناپ_بٹ > 0 ? ثنائی_ہندسہ(extra_bi_ناپ_بٹ) : 0)
    };
    إذا (it_v_reg.سرا >= USER_چوٹی) {
        /* Variable arguments for printf() require well defined integer types to
         * work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        لکھو("ERROR: userland image virt [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ")"
               "exceeds USER_TOP (%"SEL4_PRIx_لفظ")\n",
               it_v_reg.شروع, it_v_reg.سرا, (لفظ_t)USER_چوٹی);
        أرجع false;
    }

    إذا (!arch_init_freemem(ui_p_reg, dtb_p_reg, it_v_reg, extra_bi_ناپ_بٹ)) {
        لکھو("ERROR: free memory management initialization failed\n");
        أرجع false;
    }

    /* create the root cnode */
    جڑ_cnode_cap = create_جڑ_cnode();
    إذا (cap_get_capType(جڑ_cnode_cap) == cap_null_cap) {
        لکھو("ERROR: root c-node creation failed\n");
        أرجع false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(جڑ_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(جڑ_cnode_cap);

#جے_تعریف_شدہ CONFIG_ARM_SMMU
    /* initialise the SMMU and provide the SMMU control caps*/
    init_smmu(جڑ_cnode_cap);
#جے_انت
#جے_تعریف_شدہ CONFIG_ALLOW_SMC_CALLS
    init_smc(جڑ_cnode_cap);
#جے_انت

    populate_bi_چوکھٹا(0, CONFIG_MAX_عدد_NODES, ipcbuf_vptr, extra_bi_ناپ);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_سرخی سرخی;
    إذا (dtb_ناپ > 0) {
        سرخی.id = SEL4_BOOTINFO_سرخی_FDT;
        سرخی.len = حجم(سرخی) + dtb_ناپ;
        *(se_L4_Boot_Info_سرخی *)(rootserver.extra_bi + extra_bi_وتھ) = سرخی;
        extra_bi_وتھ += حجم(سرخی);
        حافظہ_نقل((فراغ *)(rootserver.extra_bi + extra_bi_وتھ),
               paddr_منزل_pptr(dtb_phys_addr),
               dtb_ناپ);
        extra_bi_وتھ += dtb_ناپ;
    }

    أكد(extra_bi_ناپ == extra_bi_وتھ);

    إذا (config_سیٹ(CONFIG_TK1_SMMU)) {
        ndks_boot.bi_چوکھٹا->ioSpaceCaps = create_iospace_caps(جڑ_cnode_cap);
        إذا (ndks_boot.bi_چوکھٹا->ioSpaceCaps.شروع == 0 &&
            ndks_boot.bi_چوکھٹا->ioSpaceCaps.سرا == 0) {
            لکھو("ERROR: SMMU I/O space creation failed\n");
            أرجع false;
        }
    } وإلا {
        ndks_boot.bi_چوکھٹا->ioSpaceCaps = S_REG_خالی_تھاواں;
    }

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_پتہ_space(جڑ_cnode_cap, it_v_reg);
    إذا (cap_get_capType(it_pd_cap) == cap_null_cap) {
        لکھو("ERROR: address space creation for initial thread failed\n");
        أرجع false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_چوکھٹا_cap(
        جڑ_cnode_cap,
        it_pd_cap,
        bi_چوکھٹا_vptr
    );

    /* create and map extra bootinfo region */
    إذا (extra_bi_ناپ > 0) {
        region_t extra_bi_region = {
            .شروع = rootserver.extra_bi,
            .سرا = rootserver.extra_bi + extra_bi_ناپ
        };
        extra_bi_ret =
            create_چوکھٹے_دا_region(
                جڑ_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_منزل_paddr((فراغ *)extra_bi_region.شروع) - extra_bi_چوکھٹا_vptr
            );
        إذا (!extra_bi_ret.success) {
            لکھو("ERROR: mapping extra boot info to initial thread failed\n");
            أرجع false;
        }
        ndks_boot.bi_چوکھٹا->extraBIPages = extra_bi_ret.region;
    }

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    init_sched_control(جڑ_cnode_cap, CONFIG_MAX_عدد_NODES);
#جے_انت

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_چوکھٹا_cap(جڑ_cnode_cap, it_pd_cap, ipcbuf_vptr);
    إذا (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        لکھو("ERROR: could not create IPC buffer for initial thread\n");
        أرجع false;
    }

    /* create all userland image frames */
    create_چوکھٹے_ret =
        create_چوکھٹے_دا_region(
            جڑ_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_وتھ
        );
    إذا (!create_چوکھٹے_ret.success) {
        لکھو("ERROR: could not create all userland image frames\n");
        أرجع false;
    }
    ndks_boot.bi_چوکھٹا->user_Image_چوکھٹے = create_چوکھٹے_ret.region;

    /* create/initialise the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(جڑ_cnode_cap);
    إذا (cap_get_capType(it_ap_cap) == cap_null_cap) {
        لکھو("ERROR: could not create ASID pool for initial thread\n");
        أرجع false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    گرہ_STATE(ks_Cur_ویلا) = get_Current_ویلا();
#جے_انت

    /* create the idle thread */
    create_idle_thread();

    /* Before creating the initial thread (which also switches to it)
     * we clean the cache so that any page table information written
     * as a result of calling create_frames_of_region will be correctly
     * read by the hardware page table walker */
    cleanInvalidateL1Caches();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         جڑ_cnode_cap,
                         it_pd_cap,
                         v_اندراج,
                         bi_چوکھٹا_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    إذا (initial == عدم) {
        لکھو("ERROR: could not create initial thread\n");
        أرجع false;
    }

    init_core_state(initial);

    /* create all of the untypeds. Both devices and kernel window memory */
    إذا (!create_untypeds(جڑ_cnode_cap)) {
        لکھو("ERROR: could not create untypteds for kernel image boot memory\n");
        أرجع false;
    }

    /* no shared-frame caps (ARM has no multikernel support) */
    ndks_boot.bi_چوکھٹا->shared_چوکھٹے = S_REG_خالی_تھاواں;

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
    إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        invalidateHypTLB();
    }

    ks_عدد_CPUs = 1;

    /* initialize BKL before booting up other cores */
    SMP_COND_STATEMENT(clh_جندرا_init());
    SMP_COND_STATEMENT(release_secondary_cpus());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    گرہ_جندرا_SYS;

    لکھو("Booting all finished, dropped to user space\n");

    /* kernel successfully initialized */
    أرجع true;
}

BOOT_کوڈ VISIBLE فراغ init_kernel(
    paddr_t ui_p_reg_شروع,
    paddr_t ui_p_reg_سرا,
    sword_t pv_وتھ,
    vptr_t  v_اندراج,
    paddr_t dtb_addr_p,
    uint32_t dtb_ناپ
)
{
    bool_t نتیجہ;

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    /* we assume there exists a cpu with id 0 and will use it for bootstrapping */
    إذا (getCurrentCPUIndex() == 0) {
        نتیجہ = try_init_kernel(ui_p_reg_شروع,
                                 ui_p_reg_سرا,
                                 pv_وتھ,
                                 v_اندراج,
                                 dtb_addr_p, dtb_ناپ);
    } وإلا {
        نتیجہ = try_init_kernel_secondary_core();
    }

#وإلا
    نتیجہ = try_init_kernel(ui_p_reg_شروع,
                             ui_p_reg_سرا,
                             pv_وتھ,
                             v_اندراج,
                             dtb_addr_p, dtb_ناپ);

#جے_انت /* ENABLE_SMP_SUPPORT */

    إذا (!نتیجہ) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    گرہ_STATE(ks_Cur_ویلا) = get_Current_ویلا();
    گرہ_STATE(ksConsumed) = 0;
#جے_انت
    ویلا_سارنی();
    activateThread();
}
