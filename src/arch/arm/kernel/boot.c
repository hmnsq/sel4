/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <assert.h>
#شامل <kernel/boot.h>
#شامل <machine/io.h>
#شامل <model/statedata.h>
#شامل <object/interrupt.h>
#شامل <arch/machine.h>
#شامل <arch/kernel/boot.h>
#شامل <arch/kernel/vspace.h>
#شامل <arch/benchmark.h>
#شامل <arch/user_access.h>
#شامل <arch/object/iospace.h>
#شامل <linker.h>
#شامل <plat/machine/hardware.h>
#شامل <machine.h>
#شامل <arch/machine/timer.h>
#شامل <arch/machine/fpu.h>
#شامل <arch/machine/tlb.h>

#اگر_تعریف_شدہ CONFIG_ARM_SMMU
#شامل <drivers/smmu/smmuv2.h>
#اگر_ختم

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS ساکن متلون _Atomic صحیح گرہ_boot_تالا;
#اگر_ختم /* ENABLE_SMP_SUPPORT */

BOOT_BSS ساکن region_t reserved[عدد_RESERVED_REGIONS];

BOOT_رمز ساکن bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          لفظ_t extra_bi_سائز_بٹ)
{
    /* reserve the kernel image region */
    reserved[0] = paddr_مقصد_pptr_reg(get_p_reg_kernel_img());

    صحیح اشاریہ = 1;

    /* add the dtb region, if it is not empty */
    اگر (dtb_p_reg.آغاز) {
        اگر (اشاریہ >= ARRAY_سائز(reserved)) {
            لکھو("ERROR: no slot to add DTB to reserved regions\n");
            واپس false;
        }
        reserved[اشاریہ].آغاز = (pptr_t) paddr_مقصد_pptr(dtb_p_reg.آغاز);
        reserved[اشاریہ].انتہا = (pptr_t) paddr_مقصد_pptr(dtb_p_reg.انتہا);
        اشاریہ++;
    }

    /* Reserve the user image region and the mode-reserved regions. For now,
     * only one mode-reserved region is supported, because this is all that is
     * needed.
     */
    اگر (منوال_RESERVED > 1) {
        لکھو("ERROR: MODE_RESERVED > 1 unsupported!\n");
        واپس false;
    }
    اگر (ui_p_reg.آغاز < PADDR_چوٹی) {
        region_t ui_reg = paddr_مقصد_pptr_reg(ui_p_reg);
        اگر (منوال_RESERVED == 1) {
            اگر (اشاریہ + 1 >= ARRAY_سائز(reserved)) {
                لکھو("ERROR: no slot to add the user image and the "
                       "mode-reserved region to the reserved regions\n");
                واپس false;
            }
            اگر (ui_reg.انتہا > منوال_reserved_region[0].آغاز) {
                reserved[اشاریہ] = منوال_reserved_region[0];
                اشاریہ++;
                reserved[اشاریہ] = ui_reg;
            } ورنہ {
                reserved[اشاریہ] = ui_reg;
                اشاریہ++;
                reserved[اشاریہ] = منوال_reserved_region[0];
            }
            اشاریہ++;
        } ورنہ {
            اگر (اشاریہ >= ARRAY_سائز(reserved)) {
                لکھو("ERROR: no slot to add the user image to the reserved"
                       "regions\n");
                واپس false;
            }
            reserved[اشاریہ] = ui_reg;
            اشاریہ++;
        }
    } ورنہ {
        اگر (منوال_RESERVED == 1) {
            اگر (اشاریہ >= ARRAY_سائز(reserved)) {
                لکھو("ERROR: no slot to add the mode-reserved region\n");
                واپس false;
            }
            reserved[اشاریہ] = منوال_reserved_region[0];
            اشاریہ++;
        }

        /* Reserve the ui_p_reg region still so it doesn't get turned into device UT. */
        reserve_region(ui_p_reg);
    }

    /* avail_p_regs comes from the auto-generated code */
    واپس init_freemem(ARRAY_سائز(avail_p_regs), avail_p_regs,
                        اشاریہ, reserved,
                        it_v_reg, extra_bi_سائز_بٹ);
}


BOOT_رمز ساکن خالی init_irqs(cap_t جڑ_cnode_cap)
{
    غیرعلامتی i;

    برائے (i = 0; i <= maxIRQ ; i++) {
        سیٹ_IRQState(IRQInactive, CORE_IRQ_مقصد_IRQT(0, i));
    }
    سیٹ_IRQState(IRQTimer, CORE_IRQ_مقصد_IRQT(0, KERNEL_TIMER_IRQ));
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    سیٹ_IRQState(IRQReserved, CORE_IRQ_مقصد_IRQT(0, مداخلت_VGIC_MAINTENANCE));
    سیٹ_IRQState(IRQReserved, CORE_IRQ_مقصد_IRQT(0, مداخلت_VTIMER_EVENT));
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_TK1_SMMU
    سیٹ_IRQState(IRQReserved, CORE_IRQ_مقصد_IRQT(0, مداخلت_SMMU));
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_ARM_ENABLE_PMU_حد_سے_تجاوز_مداخلت
#اگر_تعریف_شدہ KERNEL_PMU_IRQ
    سیٹ_IRQState(IRQReserved, CORE_IRQ_مقصد_IRQT(0, KERNEL_PMU_IRQ));
#اگر (defined CONFIG_PLAT_TX1 && defined ENABLE_SMP_SUPPORT)
//SELFOUR-1252
#غلطی "This platform doesn't support tracking CPU utilisation on multicore"
#اگر_ختم /* CONFIG_PLAT_TX1 && ENABLE_SMP_SUPPORT */
#ورنہ
#غلطی "This platform doesn't support tracking CPU utilisation feature"
#اگر_ختم /* KERNEL_TIMER_IRQ */
#اگر_ختم /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    سیٹ_IRQState(IRQIPI, CORE_IRQ_مقصد_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    سیٹ_IRQState(IRQIPI, CORE_IRQ_مقصد_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
#اگر_ختم

    /* provide the IRQ control cap */
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

#اگر_تعریف_شدہ CONFIG_ARM_SMMU
BOOT_رمز ساکن خالی init_smmu(cap_t جڑ_cnode_cap)
{
    plat_smmu_init();
    /*provide the SID and CB control cap*/
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapSMMUSIDControl), cap_sid_control_cap_new());
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapSMMUCBControl), cap_cb_control_cap_new());
}

#اگر_ختم

#اگر_تعریف_شدہ CONFIG_ALLOW_SMC_CALLS
BOOT_رمز ساکن خالی init_smc(cap_t جڑ_cnode_cap)
{
    /* Provide the SMC cap*/
    write_خانہ(خانہ_PTR(pptr_کا_cap(جڑ_cnode_cap), seL4_CapSMC), cap_smc_cap_new(0));
}
#اگر_ختم

/** This and only this function initialises the CPU.
 *
 * It does NOT initialise any kernel state.
 * @return For the verification build, this currently returns true always.
 */
BOOT_رمز ساکن bool_t init_cpu(خالی)
{
    bool_t haveHWFPU;

#اگر_تعریف_شدہ CONFIG_ARCH_AARCH64
    اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        اگر (!checkTCR_EL2()) {
            واپس false;
        }
    }
#اگر_ختم

    activate_kernel_vspace();
    اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_boot_init();
    }

#اگر_تعریف_شدہ CONFIG_ARCH_AARCH64
    /* After activate_kernel_vspace(), so printf() is available. */
    اگر (!config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT) && !check16_ثنائی_ہندسہ_ASID()) {
        لکھو("ERROR: kernel requires 16-bit ASIDs\n");
        واپس false;
    }
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API
    اگر (!Arch_initHardwareBreakpoints()) {
        لکھو("Kernel built with CONFIG_HARDWARE_DEBUG_API, but this board doesn't "
               "reliably support it.\n");
        واپس false;
    }
#اگر_ختم

    /* Setup kernel stack pointer.
     * On ARM SMP, the array index here is the CPU ID
     */
    لفظ_t ڈھیر_چوٹی = ((لفظ_t) kernel_ڈھیر_alloc[CURRENT_CPU_اشاریہ()]) + ثنائی_ہندسہ(CONFIG_KERNEL_ڈھیر_بٹ);
#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
#اگر_تعریف_شدہ CONFIG_ARCH_AARCH64
    /* the least 12 bits are used to store logical core ID */
    ڈھیر_چوٹی |= getCurrentCPUIndex();
#وگرنہ_اگر defined(CONFIG_ARCH_AARCH32)
    /* Stack address encodes core ID, ensure it is in the right region */
    ڈھیر_چوٹی -= 8;
#اگر_ختم
#اگر_ختم
    سیٹ_Kernel_ڈھیر(ڈھیر_چوٹی);

#اگر_تعریف_شدہ CONFIG_ARCH_AARCH64
    /* initialise CPU's exception vector table */
    سیٹ_Vtable((pptr_t)arm_سمتیہ_جدول);
#اگر_ختم /* CONFIG_ARCH_AARCH64 */

    haveHWFPU = fpsimd_HWCapTest();

    /* Disable FPU to avoid channels where a platform has an FPU but doesn't make use of it */
    اگر (haveHWFPU) {
        disableFpu();
    }

#اگر_تعریف_شدہ CONFIG_HAVE_FPU
    اگر (haveHWFPU) {
        اگر (!fpsimd_init()) {
            واپس false;
        }
    } ورنہ {
        لکھو("Platform claims to have FP hardware, but does not!\n");
        واپس false;
    }
#اگر_ختم /* CONFIG_HAVE_FPU */

    cpu_initLocalIRQController();

#اگر_تعریف_شدہ CONFIG_ENABLE_BENCHMARKS
    arm_init_ccnt();
#اگر_ختم /* CONFIG_ENABLE_BENCHMARKS */

    /* Export selected CPU features for access by PL0 */
    armv_init_user_access();

    initTimer();

    واپس true;
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_رمز ساکن خالی init_plat(خالی)
{
    initIRQController();
    initL2Cache();
#اگر_تعریف_شدہ CONFIG_ARM_SMMU
    plat_smmu_init();
#اگر_ختم
}

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
BOOT_رمز ساکن bool_t try_init_kernel_secondary_core(خالی)
{
    /* need to first wait until some kernel init has been done */
    جب (!گرہ_boot_تالا);

    /* Perform cpu init */
    init_cpu();

    برائے (غیرعلامتی صحیح i = 0; i < عدد_PPI; i++) {
        نقاب_مداخلت(true, CORE_IRQ_مقصد_IRQT(getCurrentCPUIndex(), i));
    }
    سیٹ_IRQState(IRQIPI, CORE_IRQ_مقصد_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    سیٹ_IRQState(IRQIPI, CORE_IRQ_مقصد_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
    /* Enable per-CPU timer interrupts */
    سیٹ_IRQState(IRQTimer, CORE_IRQ_مقصد_IRQT(getCurrentCPUIndex(), KERNEL_TIMER_IRQ));
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    سیٹ_IRQState(IRQReserved, CORE_IRQ_مقصد_IRQT(getCurrentCPUIndex(), مداخلت_VGIC_MAINTENANCE));
    سیٹ_IRQState(IRQReserved, CORE_IRQ_مقصد_IRQT(getCurrentCPUIndex(), مداخلت_VTIMER_EVENT));
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    گرہ_تالا_SYS;

    clock_sync_test();
    ks_عدد_CPUs++;

    init_core_state(SchedulerAction_ResumeCurrentThread);

    واپس true;
}

BOOT_رمز ساکن خالی release_secondary_cpus(خالی)
{
    /* release the cpus at the same time */
    أكد(0 == گرہ_boot_تالا); /* Sanity check for a proper lock state. */
    گرہ_boot_تالا = 1;

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
#اگر_تعریف_شدہ CONFIG_ARCH_AARCH32
    cleanInvalidateL1Caches();
    plat_cleanInvalidateL2Cache();
#اگر_ختم

    /* Wait until all the secondary cores are done initialising */
    جب (ks_عدد_CPUs != CONFIG_MAX_عدد_NODES) {
#اگر_تعریف_شدہ ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        گرہ_STATE(ks_Cur_وقت) = get_Current_وقت();
#اگر_ختم
        /* perform a memory acquire to get new values of ksNumCPUs, release for ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#اگر_ختم /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

ساکن BOOT_رمز bool_t try_init_kernel(
    paddr_t ui_p_reg_آغاز,
    paddr_t ui_p_reg_انتہا,
    sword_t pv_انحراف,
    vptr_t  v_اندراج,
    paddr_t dtb_phys_addr,
    لفظ_t  dtb_سائز
)
{
    cap_t جڑ_cnode_cap;
    cap_t it_ap_cap;
    cap_t it_pd_cap;
    cap_t ipcbuf_cap;
    p_region_t ui_p_reg = (p_region_t) {
        ui_p_reg_آغاز, ui_p_reg_انتہا
    };
    region_t ui_reg = paddr_مقصد_pptr_reg(ui_p_reg);
    لفظ_t extra_bi_سائز = 0;
    pptr_t extra_bi_انحراف = 0;
    vptr_t extra_bi_چوکھٹا_vptr;
    vptr_t bi_چوکھٹا_vptr;
    vptr_t ipcbuf_vptr;
    create_چوکھٹے_کا_region_ret_t create_چوکھٹے_ret;
    create_چوکھٹے_کا_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .آغاز = ui_p_reg_آغاز - pv_انحراف,
        .انتہا   = ui_p_reg_انتہا   - pv_انحراف
    };

    ipcbuf_vptr = ui_v_reg.انتہا;
    bi_چوکھٹا_vptr = ipcbuf_vptr + ثنائی_ہندسہ(صفحہ_بٹ);
    extra_bi_چوکھٹا_vptr = bi_چوکھٹا_vptr + ثنائی_ہندسہ(se_L4_Boot_Info_چوکھٹا_بٹ);

    /* setup virtual memory for the kernel */
    map_kernel_کھڑکی();

    /* initialise the CPU */
    اگر (!init_cpu()) {
        لکھو("ERROR: CPU init failed\n");
        واپس false;
    }

    /* debug output via serial port is only available from here */
    لکھو("Bootstrapping kernel\n");

    /* initialise the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_خالی_خانے;
    اگر (dtb_سائز > 0) {
        paddr_t dtb_phys_انتہا = dtb_phys_addr + dtb_سائز;
        اگر (dtb_phys_انتہا < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            لکھو("ERROR: DTB location at %"SEL4_PRIx_لفظ
                   " len %"SEL4_PRIu_لفظ" invalid\n",
                   dtb_phys_addr, dtb_سائز);
            واپس false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        اگر (dtb_phys_انتہا >= PADDR_چوٹی) {
            لکھو("ERROR: DTB at [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_لفظ")\n",
                   dtb_phys_addr, dtb_phys_انتہا, PADDR_چوٹی);
            واپس false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_سائز += حجم(se_L4_Boot_Info_سرخی) + dtb_سائز;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .آغاز = dtb_phys_addr,
            .انتہا   = dtb_phys_انتہا
        };
    }

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    لفظ_t extra_bi_سائز_بٹ = calculate_extra_bi_سائز_بٹ(extra_bi_سائز);
    v_region_t it_v_reg = {
        .آغاز = ui_v_reg.آغاز,
        .انتہا   = extra_bi_چوکھٹا_vptr + (extra_bi_سائز_بٹ > 0 ? ثنائی_ہندسہ(extra_bi_سائز_بٹ) : 0)
    };
    اگر (it_v_reg.انتہا >= USER_چوٹی) {
        /* Variable arguments for printf() require well defined integer types to
         * work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        لکھو("ERROR: userland image virt [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ")"
               "exceeds USER_TOP (%"SEL4_PRIx_لفظ")\n",
               it_v_reg.آغاز, it_v_reg.انتہا, (لفظ_t)USER_چوٹی);
        واپس false;
    }

    اگر (!arch_init_freemem(ui_p_reg, dtb_p_reg, it_v_reg, extra_bi_سائز_بٹ)) {
        لکھو("ERROR: free memory management initialization failed\n");
        واپس false;
    }

    /* create the root cnode */
    جڑ_cnode_cap = create_جڑ_cnode();
    اگر (cap_get_capType(جڑ_cnode_cap) == cap_null_cap) {
        لکھو("ERROR: root c-node creation failed\n");
        واپس false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(جڑ_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(جڑ_cnode_cap);

#اگر_تعریف_شدہ CONFIG_ARM_SMMU
    /* initialise the SMMU and provide the SMMU control caps*/
    init_smmu(جڑ_cnode_cap);
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_ALLOW_SMC_CALLS
    init_smc(جڑ_cnode_cap);
#اگر_ختم

    populate_bi_چوکھٹا(0, CONFIG_MAX_عدد_NODES, ipcbuf_vptr, extra_bi_سائز);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_سرخی سرخی;
    اگر (dtb_سائز > 0) {
        سرخی.id = SEL4_BOOTINFO_سرخی_FDT;
        سرخی.len = حجم(سرخی) + dtb_سائز;
        *(se_L4_Boot_Info_سرخی *)(rootserver.extra_bi + extra_bi_انحراف) = سرخی;
        extra_bi_انحراف += حجم(سرخی);
        حافظہ_نقل((خالی *)(rootserver.extra_bi + extra_bi_انحراف),
               paddr_مقصد_pptr(dtb_phys_addr),
               dtb_سائز);
        extra_bi_انحراف += dtb_سائز;
    }

    أكد(extra_bi_سائز == extra_bi_انحراف);

    اگر (config_سیٹ(CONFIG_TK1_SMMU)) {
        ndks_boot.bi_چوکھٹا->ioSpaceCaps = create_iospace_caps(جڑ_cnode_cap);
        اگر (ndks_boot.bi_چوکھٹا->ioSpaceCaps.آغاز == 0 &&
            ndks_boot.bi_چوکھٹا->ioSpaceCaps.انتہا == 0) {
            لکھو("ERROR: SMMU I/O space creation failed\n");
            واپس false;
        }
    } ورنہ {
        ndks_boot.bi_چوکھٹا->ioSpaceCaps = S_REG_خالی_خانے;
    }

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_پتہ_space(جڑ_cnode_cap, it_v_reg);
    اگر (cap_get_capType(it_pd_cap) == cap_null_cap) {
        لکھو("ERROR: address space creation for initial thread failed\n");
        واپس false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_چوکھٹا_cap(
        جڑ_cnode_cap,
        it_pd_cap,
        bi_چوکھٹا_vptr
    );

    /* create and map extra bootinfo region */
    اگر (extra_bi_سائز > 0) {
        region_t extra_bi_region = {
            .آغاز = rootserver.extra_bi,
            .انتہا = rootserver.extra_bi + extra_bi_سائز
        };
        extra_bi_ret =
            create_چوکھٹے_کا_region(
                جڑ_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_مقصد_paddr((خالی *)extra_bi_region.آغاز) - extra_bi_چوکھٹا_vptr
            );
        اگر (!extra_bi_ret.success) {
            لکھو("ERROR: mapping extra boot info to initial thread failed\n");
            واپس false;
        }
        ndks_boot.bi_چوکھٹا->extraBIPages = extra_bi_ret.region;
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    init_sched_control(جڑ_cnode_cap, CONFIG_MAX_عدد_NODES);
#اگر_ختم

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_چوکھٹا_cap(جڑ_cnode_cap, it_pd_cap, ipcbuf_vptr);
    اگر (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        لکھو("ERROR: could not create IPC buffer for initial thread\n");
        واپس false;
    }

    /* create all userland image frames */
    create_چوکھٹے_ret =
        create_چوکھٹے_کا_region(
            جڑ_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_انحراف
        );
    اگر (!create_چوکھٹے_ret.success) {
        لکھو("ERROR: could not create all userland image frames\n");
        واپس false;
    }
    ndks_boot.bi_چوکھٹا->user_Image_چوکھٹے = create_چوکھٹے_ret.region;

    /* create/initialise the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(جڑ_cnode_cap);
    اگر (cap_get_capType(it_ap_cap) == cap_null_cap) {
        لکھو("ERROR: could not create ASID pool for initial thread\n");
        واپس false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    گرہ_STATE(ks_Cur_وقت) = get_Current_وقت();
#اگر_ختم

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

    اگر (initial == تہی) {
        لکھو("ERROR: could not create initial thread\n");
        واپس false;
    }

    init_core_state(initial);

    /* create all of the untypeds. Both devices and kernel window memory */
    اگر (!create_untypeds(جڑ_cnode_cap)) {
        لکھو("ERROR: could not create untypteds for kernel image boot memory\n");
        واپس false;
    }

    /* no shared-frame caps (ARM has no multikernel support) */
    ndks_boot.bi_چوکھٹا->shared_چوکھٹے = S_REG_خالی_خانے;

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
    اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        invalidateHypTLB();
    }

    ks_عدد_CPUs = 1;

    /* initialize BKL before booting up other cores */
    SMP_COND_STATEMENT(clh_تالا_init());
    SMP_COND_STATEMENT(release_secondary_cpus());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    گرہ_تالا_SYS;

    لکھو("Booting all finished, dropped to user space\n");

    /* kernel successfully initialized */
    واپس true;
}

BOOT_رمز VISIBLE خالی init_kernel(
    paddr_t ui_p_reg_آغاز,
    paddr_t ui_p_reg_انتہا,
    sword_t pv_انحراف,
    vptr_t  v_اندراج,
    paddr_t dtb_addr_p,
    uint32_t dtb_سائز
)
{
    bool_t نتیجہ;

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    /* we assume there exists a cpu with id 0 and will use it for bootstrapping */
    اگر (getCurrentCPUIndex() == 0) {
        نتیجہ = try_init_kernel(ui_p_reg_آغاز,
                                 ui_p_reg_انتہا,
                                 pv_انحراف,
                                 v_اندراج,
                                 dtb_addr_p, dtb_سائز);
    } ورنہ {
        نتیجہ = try_init_kernel_secondary_core();
    }

#ورنہ
    نتیجہ = try_init_kernel(ui_p_reg_آغاز,
                             ui_p_reg_انتہا,
                             pv_انحراف,
                             v_اندراج,
                             dtb_addr_p, dtb_سائز);

#اگر_ختم /* ENABLE_SMP_SUPPORT */

    اگر (!نتیجہ) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    گرہ_STATE(ks_Cur_وقت) = get_Current_وقت();
    گرہ_STATE(ksConsumed) = 0;
#اگر_ختم
    جدول_وقت();
    activateThread();
}
