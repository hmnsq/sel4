/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/interrupt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/benchmark.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/user_access.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/iospace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/timer.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/fpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/tlb.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/smmu/smmuv2.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS ਸਥਿਰ ਅਸਥਿਰ _Atomic ਪੂਰਨ ਗੰਢ_boot_ਤਾਲਾ;
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

BOOT_BSS ਸਥਿਰ region_t reserved[ਅੰਕ_RESERVED_REGIONS];

BOOT_ਕੋਡ ਸਥਿਰ bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          ਸ਼ਬਦ_t extra_bi_ਮਾਪ_ਬਿੱਟ)
{
    /* reserve the kernel image region */
    reserved[0] = paddr_ਮੰਜ਼ਿਲ_pptr_reg(get_p_reg_kernel_img());

    ਪੂਰਨ ਸੂਚਕਾਂਕ = 1;

    /* add the dtb region, if it is not empty */
    ਜੇ (dtb_p_reg.ਸ਼ੁਰੂ) {
        ਜੇ (ਸੂਚਕਾਂਕ >= ARRAY_ਮਾਪ(reserved)) {
            ਲਿਖੋ("ERROR: no slot to add DTB to reserved regions\n");
            ਵਾਪਸ false;
        }
        reserved[ਸੂਚਕਾਂਕ].ਸ਼ੁਰੂ = (pptr_t) paddr_ਮੰਜ਼ਿਲ_pptr(dtb_p_reg.ਸ਼ੁਰੂ);
        reserved[ਸੂਚਕਾਂਕ].ਸਿਰਾ = (pptr_t) paddr_ਮੰਜ਼ਿਲ_pptr(dtb_p_reg.ਸਿਰਾ);
        ਸੂਚਕਾਂਕ++;
    }

    /* Reserve the user image region and the mode-reserved regions. For now,
     * only one mode-reserved region is supported, because this is all that is
     * needed.
     */
    ਜੇ (ਬਹੁਲਕ_RESERVED > 1) {
        ਲਿਖੋ("ERROR: MODE_RESERVED > 1 unsupported!\n");
        ਵਾਪਸ false;
    }
    ਜੇ (ui_p_reg.ਸ਼ੁਰੂ < PADDR_ਸਿਖਰ) {
        region_t ui_reg = paddr_ਮੰਜ਼ਿਲ_pptr_reg(ui_p_reg);
        ਜੇ (ਬਹੁਲਕ_RESERVED == 1) {
            ਜੇ (ਸੂਚਕਾਂਕ + 1 >= ARRAY_ਮਾਪ(reserved)) {
                ਲਿਖੋ("ERROR: no slot to add the user image and the "
                       "mode-reserved region to the reserved regions\n");
                ਵਾਪਸ false;
            }
            ਜੇ (ui_reg.ਸਿਰਾ > ਬਹੁਲਕ_reserved_region[0].ਸ਼ੁਰੂ) {
                reserved[ਸੂਚਕਾਂਕ] = ਬਹੁਲਕ_reserved_region[0];
                ਸੂਚਕਾਂਕ++;
                reserved[ਸੂਚਕਾਂਕ] = ui_reg;
            } ਨਹੀਂਤਾਂ {
                reserved[ਸੂਚਕਾਂਕ] = ui_reg;
                ਸੂਚਕਾਂਕ++;
                reserved[ਸੂਚਕਾਂਕ] = ਬਹੁਲਕ_reserved_region[0];
            }
            ਸੂਚਕਾਂਕ++;
        } ਨਹੀਂਤਾਂ {
            ਜੇ (ਸੂਚਕਾਂਕ >= ARRAY_ਮਾਪ(reserved)) {
                ਲਿਖੋ("ERROR: no slot to add the user image to the reserved"
                       "regions\n");
                ਵਾਪਸ false;
            }
            reserved[ਸੂਚਕਾਂਕ] = ui_reg;
            ਸੂਚਕਾਂਕ++;
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ (ਬਹੁਲਕ_RESERVED == 1) {
            ਜੇ (ਸੂਚਕਾਂਕ >= ARRAY_ਮਾਪ(reserved)) {
                ਲਿਖੋ("ERROR: no slot to add the mode-reserved region\n");
                ਵਾਪਸ false;
            }
            reserved[ਸੂਚਕਾਂਕ] = ਬਹੁਲਕ_reserved_region[0];
            ਸੂਚਕਾਂਕ++;
        }

        /* Reserve the ui_p_reg region still so it doesn't get turned into device UT. */
        reserve_region(ui_p_reg);
    }

    /* avail_p_regs comes from the auto-generated code */
    ਵਾਪਸ init_freemem(ARRAY_ਮਾਪ(avail_p_regs), avail_p_regs,
                        ਸੂਚਕਾਂਕ, reserved,
                        it_v_reg, extra_bi_ਮਾਪ_ਬਿੱਟ);
}


BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ init_irqs(cap_t ਜੜ੍ਹ_cnode_cap)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ i;

    ਲਈ (i = 0; i <= maxIRQ ; i++) {
        ਸੈੱਟ_IRQState(IRQInactive, CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(0, i));
    }
    ਸੈੱਟ_IRQState(IRQTimer, CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(0, KERNEL_TIMER_IRQ));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਸੈੱਟ_IRQState(IRQReserved, CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(0, ਰੁਕਾਵਟ_VGIC_MAINTENANCE));
    ਸੈੱਟ_IRQState(IRQReserved, CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(0, ਰੁਕਾਵਟ_VTIMER_EVENT));
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
    ਸੈੱਟ_IRQState(IRQReserved, CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(0, ਰੁਕਾਵਟ_SMMU));
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_ENABLE_PMU_ਉਛਾਲ_ਰੁਕਾਵਟ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ KERNEL_PMU_IRQ
    ਸੈੱਟ_IRQState(IRQReserved, CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(0, KERNEL_PMU_IRQ));
#ਜੇ (defined CONFIG_PLAT_TX1 && defined ENABLE_SMP_SUPPORT)
//SELFOUR-1252
#ਗਲਤੀ "This platform doesn't support tracking CPU utilisation on multicore"
#ਜੇ_ਅੰਤ /* CONFIG_PLAT_TX1 && ENABLE_SMP_SUPPORT */
#ਨਹੀਂਤਾਂ
#ਗਲਤੀ "This platform doesn't support tracking CPU utilisation feature"
#ਜੇ_ਅੰਤ /* KERNEL_TIMER_IRQ */
#ਜੇ_ਅੰਤ /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ਸੈੱਟ_IRQState(IRQIPI, CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    ਸੈੱਟ_IRQState(IRQIPI, CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
#ਜੇ_ਅੰਤ

    /* provide the IRQ control cap */
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ init_smmu(cap_t ਜੜ੍ਹ_cnode_cap)
{
    plat_smmu_init();
    /*provide the SID and CB control cap*/
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapSMMUSIDControl), cap_sid_control_cap_new());
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapSMMUCBControl), cap_cb_control_cap_new());
}

#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ALLOW_SMC_CALLS
BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ init_smc(cap_t ਜੜ੍ਹ_cnode_cap)
{
    /* Provide the SMC cap*/
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapSMC), cap_smc_cap_new(0));
}
#ਜੇ_ਅੰਤ

/** This and only this function initialises the CPU.
 *
 * It does NOT initialise any kernel state.
 * @return For the verification build, this currently returns true always.
 */
BOOT_ਕੋਡ ਸਥਿਰ bool_t init_cpu(ਖਾਲੀ)
{
    bool_t haveHWFPU;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_AARCH64
    ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        ਜੇ (!checkTCR_EL2()) {
            ਵਾਪਸ false;
        }
    }
#ਜੇ_ਅੰਤ

    activate_kernel_vspace();
    ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_boot_init();
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_AARCH64
    /* After activate_kernel_vspace(), so printf() is available. */
    ਜੇ (!config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT) && !check16_ਦੋਆਧਾਰੀ_ਅੰਕ_ASID()) {
        ਲਿਖੋ("ERROR: kernel requires 16-bit ASIDs\n");
        ਵਾਪਸ false;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API
    ਜੇ (!Arch_initHardwareBreakpoints()) {
        ਲਿਖੋ("Kernel built with CONFIG_HARDWARE_DEBUG_API, but this board doesn't "
               "reliably support it.\n");
        ਵਾਪਸ false;
    }
#ਜੇ_ਅੰਤ

    /* Setup kernel stack pointer.
     * On ARM SMP, the array index here is the CPU ID
     */
    ਸ਼ਬਦ_t ਢੇਰ_ਸਿਖਰ = ((ਸ਼ਬਦ_t) kernel_ਢੇਰ_alloc[CURRENT_CPU_ਸੂਚਕਾਂਕ()]) + ਦੋਆਧਾਰੀ_ਅੰਕ(CONFIG_KERNEL_ਢੇਰ_ਬਿੱਟ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_AARCH64
    /* the least 12 bits are used to store logical core ID */
    ਢੇਰ_ਸਿਖਰ |= getCurrentCPUIndex();
#ਜਾਂਜੇ defined(CONFIG_ARCH_AARCH32)
    /* Stack address encodes core ID, ensure it is in the right region */
    ਢੇਰ_ਸਿਖਰ -= 8;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅੰਤ
    ਸੈੱਟ_Kernel_ਢੇਰ(ਢੇਰ_ਸਿਖਰ);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_AARCH64
    /* initialise CPU's exception vector table */
    ਸੈੱਟ_Vtable((pptr_t)arm_ਸਦਿਸ਼_ਸਾਰਣੀ);
#ਜੇ_ਅੰਤ /* CONFIG_ARCH_AARCH64 */

    haveHWFPU = fpsimd_HWCapTest();

    /* Disable FPU to avoid channels where a platform has an FPU but doesn't make use of it */
    ਜੇ (haveHWFPU) {
        disableFpu();
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
    ਜੇ (haveHWFPU) {
        ਜੇ (!fpsimd_init()) {
            ਵਾਪਸ false;
        }
    } ਨਹੀਂਤਾਂ {
        ਲਿਖੋ("Platform claims to have FP hardware, but does not!\n");
        ਵਾਪਸ false;
    }
#ਜੇ_ਅੰਤ /* CONFIG_HAVE_FPU */

    cpu_initLocalIRQController();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ENABLE_BENCHMARKS
    arm_init_ccnt();
#ਜੇ_ਅੰਤ /* CONFIG_ENABLE_BENCHMARKS */

    /* Export selected CPU features for access by PL0 */
    armv_init_user_access();

    initTimer();

    ਵਾਪਸ true;
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ init_plat(ਖਾਲੀ)
{
    initIRQController();
    initL2Cache();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
    plat_smmu_init();
#ਜੇ_ਅੰਤ
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
BOOT_ਕੋਡ ਸਥਿਰ bool_t try_init_kernel_secondary_core(ਖਾਲੀ)
{
    /* need to first wait until some kernel init has been done */
    ਜਦੋਂ (!ਗੰਢ_boot_ਤਾਲਾ);

    /* Perform cpu init */
    init_cpu();

    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i = 0; i < ਅੰਕ_PPI; i++) {
        ਨਕਾਬ_ਰੁਕਾਵਟ(true, CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(getCurrentCPUIndex(), i));
    }
    ਸੈੱਟ_IRQState(IRQIPI, CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    ਸੈੱਟ_IRQState(IRQIPI, CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
    /* Enable per-CPU timer interrupts */
    ਸੈੱਟ_IRQState(IRQTimer, CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(getCurrentCPUIndex(), KERNEL_TIMER_IRQ));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਸੈੱਟ_IRQState(IRQReserved, CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(getCurrentCPUIndex(), ਰੁਕਾਵਟ_VGIC_MAINTENANCE));
    ਸੈੱਟ_IRQState(IRQReserved, CORE_IRQ_ਮੰਜ਼ਿਲ_IRQT(getCurrentCPUIndex(), ਰੁਕਾਵਟ_VTIMER_EVENT));
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    ਗੰਢ_ਤਾਲਾ_SYS;

    clock_sync_test();
    ks_ਅੰਕ_CPUs++;

    init_core_state(SchedulerAction_ResumeCurrentThread);

    ਵਾਪਸ true;
}

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ release_secondary_cpus(ਖਾਲੀ)
{
    /* release the cpus at the same time */
    ਪ੍ਰਤਿਜ੍ਞਾ(0 == ਗੰਢ_boot_ਤਾਲਾ); /* Sanity check for a proper lock state. */
    ਗੰਢ_boot_ਤਾਲਾ = 1;

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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_AARCH32
    cleanInvalidateL1Caches();
    plat_cleanInvalidateL2Cache();
#ਜੇ_ਅੰਤ

    /* Wait until all the secondary cores are done initialising */
    ਜਦੋਂ (ks_ਅੰਕ_CPUs != CONFIG_MAX_ਅੰਕ_NODES) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        ਗੰਢ_STATE(ks_Cur_ਸਮਾਂ) = get_Current_ਸਮਾਂ();
#ਜੇ_ਅੰਤ
        /* perform a memory acquire to get new values of ksNumCPUs, release for ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

ਸਥਿਰ BOOT_ਕੋਡ bool_t try_init_kernel(
    paddr_t ui_p_reg_ਸ਼ੁਰੂ,
    paddr_t ui_p_reg_ਸਿਰਾ,
    sword_t pv_ਵਿੱਥ,
    vptr_t  v_ਇੰਦਰਾਜ,
    paddr_t dtb_phys_addr,
    ਸ਼ਬਦ_t  dtb_ਮਾਪ
)
{
    cap_t ਜੜ੍ਹ_cnode_cap;
    cap_t it_ap_cap;
    cap_t it_pd_cap;
    cap_t ipcbuf_cap;
    p_region_t ui_p_reg = (p_region_t) {
        ui_p_reg_ਸ਼ੁਰੂ, ui_p_reg_ਸਿਰਾ
    };
    region_t ui_reg = paddr_ਮੰਜ਼ਿਲ_pptr_reg(ui_p_reg);
    ਸ਼ਬਦ_t extra_bi_ਮਾਪ = 0;
    pptr_t extra_bi_ਵਿੱਥ = 0;
    vptr_t extra_bi_ਚੌਖਟਾ_vptr;
    vptr_t bi_ਚੌਖਟਾ_vptr;
    vptr_t ipcbuf_vptr;
    create_ਚੌਖਟੇ_ਦਾ_region_ret_t create_ਚੌਖਟੇ_ret;
    create_ਚੌਖਟੇ_ਦਾ_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .ਸ਼ੁਰੂ = ui_p_reg_ਸ਼ੁਰੂ - pv_ਵਿੱਥ,
        .ਸਿਰਾ   = ui_p_reg_ਸਿਰਾ   - pv_ਵਿੱਥ
    };

    ipcbuf_vptr = ui_v_reg.ਸਿਰਾ;
    bi_ਚੌਖਟਾ_vptr = ipcbuf_vptr + ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ);
    extra_bi_ਚੌਖਟਾ_vptr = bi_ਚੌਖਟਾ_vptr + ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Boot_Info_ਚੌਖਟਾ_ਬਿੱਟ);

    /* setup virtual memory for the kernel */
    map_kernel_ਖਿੜਕੀ();

    /* initialise the CPU */
    ਜੇ (!init_cpu()) {
        ਲਿਖੋ("ERROR: CPU init failed\n");
        ਵਾਪਸ false;
    }

    /* debug output via serial port is only available from here */
    ਲਿਖੋ("Bootstrapping kernel\n");

    /* initialise the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_ਖਾਲੀ_ਥਾਵਾਂ;
    ਜੇ (dtb_ਮਾਪ > 0) {
        paddr_t dtb_phys_ਸਿਰਾ = dtb_phys_addr + dtb_ਮਾਪ;
        ਜੇ (dtb_phys_ਸਿਰਾ < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            ਲਿਖੋ("ERROR: DTB location at %"SEL4_PRIx_ਸ਼ਬਦ
                   " len %"SEL4_PRIu_ਸ਼ਬਦ" invalid\n",
                   dtb_phys_addr, dtb_ਮਾਪ);
            ਵਾਪਸ false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        ਜੇ (dtb_phys_ਸਿਰਾ >= PADDR_ਸਿਖਰ) {
            ਲਿਖੋ("ERROR: DTB at [%"SEL4_PRIx_ਸ਼ਬਦ"..%"SEL4_PRIx_ਸ਼ਬਦ") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_ਸ਼ਬਦ")\n",
                   dtb_phys_addr, dtb_phys_ਸਿਰਾ, PADDR_ਸਿਖਰ);
            ਵਾਪਸ false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_ਮਾਪ += ਆਕਾਰ(se_L4_Boot_Info_ਸਿਰਲੇਖ) + dtb_ਮਾਪ;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .ਸ਼ੁਰੂ = dtb_phys_addr,
            .ਸਿਰਾ   = dtb_phys_ਸਿਰਾ
        };
    }

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    ਸ਼ਬਦ_t extra_bi_ਮਾਪ_ਬਿੱਟ = calculate_extra_bi_ਮਾਪ_ਬਿੱਟ(extra_bi_ਮਾਪ);
    v_region_t it_v_reg = {
        .ਸ਼ੁਰੂ = ui_v_reg.ਸ਼ੁਰੂ,
        .ਸਿਰਾ   = extra_bi_ਚੌਖਟਾ_vptr + (extra_bi_ਮਾਪ_ਬਿੱਟ > 0 ? ਦੋਆਧਾਰੀ_ਅੰਕ(extra_bi_ਮਾਪ_ਬਿੱਟ) : 0)
    };
    ਜੇ (it_v_reg.ਸਿਰਾ >= USER_ਸਿਖਰ) {
        /* Variable arguments for printf() require well defined integer types to
         * work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        ਲਿਖੋ("ERROR: userland image virt [%"SEL4_PRIx_ਸ਼ਬਦ"..%"SEL4_PRIx_ਸ਼ਬਦ")"
               "exceeds USER_TOP (%"SEL4_PRIx_ਸ਼ਬਦ")\n",
               it_v_reg.ਸ਼ੁਰੂ, it_v_reg.ਸਿਰਾ, (ਸ਼ਬਦ_t)USER_ਸਿਖਰ);
        ਵਾਪਸ false;
    }

    ਜੇ (!arch_init_freemem(ui_p_reg, dtb_p_reg, it_v_reg, extra_bi_ਮਾਪ_ਬਿੱਟ)) {
        ਲਿਖੋ("ERROR: free memory management initialization failed\n");
        ਵਾਪਸ false;
    }

    /* create the root cnode */
    ਜੜ੍ਹ_cnode_cap = create_ਜੜ੍ਹ_cnode();
    ਜੇ (cap_get_capType(ਜੜ੍ਹ_cnode_cap) == cap_null_cap) {
        ਲਿਖੋ("ERROR: root c-node creation failed\n");
        ਵਾਪਸ false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(ਜੜ੍ਹ_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(ਜੜ੍ਹ_cnode_cap);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
    /* initialise the SMMU and provide the SMMU control caps*/
    init_smmu(ਜੜ੍ਹ_cnode_cap);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ALLOW_SMC_CALLS
    init_smc(ਜੜ੍ਹ_cnode_cap);
#ਜੇ_ਅੰਤ

    populate_bi_ਚੌਖਟਾ(0, CONFIG_MAX_ਅੰਕ_NODES, ipcbuf_vptr, extra_bi_ਮਾਪ);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_ਸਿਰਲੇਖ ਸਿਰਲੇਖ;
    ਜੇ (dtb_ਮਾਪ > 0) {
        ਸਿਰਲੇਖ.id = SEL4_BOOTINFO_ਸਿਰਲੇਖ_FDT;
        ਸਿਰਲੇਖ.len = ਆਕਾਰ(ਸਿਰਲੇਖ) + dtb_ਮਾਪ;
        *(se_L4_Boot_Info_ਸਿਰਲੇਖ *)(rootserver.extra_bi + extra_bi_ਵਿੱਥ) = ਸਿਰਲੇਖ;
        extra_bi_ਵਿੱਥ += ਆਕਾਰ(ਸਿਰਲੇਖ);
        ਸਮ੍ਰਿਤੀ_ਨਕਲ((ਖਾਲੀ *)(rootserver.extra_bi + extra_bi_ਵਿੱਥ),
               paddr_ਮੰਜ਼ਿਲ_pptr(dtb_phys_addr),
               dtb_ਮਾਪ);
        extra_bi_ਵਿੱਥ += dtb_ਮਾਪ;
    }

    ਪ੍ਰਤਿਜ੍ਞਾ(extra_bi_ਮਾਪ == extra_bi_ਵਿੱਥ);

    ਜੇ (config_ਸੈੱਟ(CONFIG_TK1_SMMU)) {
        ndks_boot.bi_ਚੌਖਟਾ->ioSpaceCaps = create_iospace_caps(ਜੜ੍ਹ_cnode_cap);
        ਜੇ (ndks_boot.bi_ਚੌਖਟਾ->ioSpaceCaps.ਸ਼ੁਰੂ == 0 &&
            ndks_boot.bi_ਚੌਖਟਾ->ioSpaceCaps.ਸਿਰਾ == 0) {
            ਲਿਖੋ("ERROR: SMMU I/O space creation failed\n");
            ਵਾਪਸ false;
        }
    } ਨਹੀਂਤਾਂ {
        ndks_boot.bi_ਚੌਖਟਾ->ioSpaceCaps = S_REG_ਖਾਲੀ_ਥਾਵਾਂ;
    }

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_ਪਤਾ_space(ਜੜ੍ਹ_cnode_cap, it_v_reg);
    ਜੇ (cap_get_capType(it_pd_cap) == cap_null_cap) {
        ਲਿਖੋ("ERROR: address space creation for initial thread failed\n");
        ਵਾਪਸ false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_ਚੌਖਟਾ_cap(
        ਜੜ੍ਹ_cnode_cap,
        it_pd_cap,
        bi_ਚੌਖਟਾ_vptr
    );

    /* create and map extra bootinfo region */
    ਜੇ (extra_bi_ਮਾਪ > 0) {
        region_t extra_bi_region = {
            .ਸ਼ੁਰੂ = rootserver.extra_bi,
            .ਸਿਰਾ = rootserver.extra_bi + extra_bi_ਮਾਪ
        };
        extra_bi_ret =
            create_ਚੌਖਟੇ_ਦਾ_region(
                ਜੜ੍ਹ_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)extra_bi_region.ਸ਼ੁਰੂ) - extra_bi_ਚੌਖਟਾ_vptr
            );
        ਜੇ (!extra_bi_ret.success) {
            ਲਿਖੋ("ERROR: mapping extra boot info to initial thread failed\n");
            ਵਾਪਸ false;
        }
        ndks_boot.bi_ਚੌਖਟਾ->extraBIPages = extra_bi_ret.region;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    init_sched_control(ਜੜ੍ਹ_cnode_cap, CONFIG_MAX_ਅੰਕ_NODES);
#ਜੇ_ਅੰਤ

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_ਚੌਖਟਾ_cap(ਜੜ੍ਹ_cnode_cap, it_pd_cap, ipcbuf_vptr);
    ਜੇ (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        ਲਿਖੋ("ERROR: could not create IPC buffer for initial thread\n");
        ਵਾਪਸ false;
    }

    /* create all userland image frames */
    create_ਚੌਖਟੇ_ret =
        create_ਚੌਖਟੇ_ਦਾ_region(
            ਜੜ੍ਹ_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_ਵਿੱਥ
        );
    ਜੇ (!create_ਚੌਖਟੇ_ret.success) {
        ਲਿਖੋ("ERROR: could not create all userland image frames\n");
        ਵਾਪਸ false;
    }
    ndks_boot.bi_ਚੌਖਟਾ->user_Image_ਚੌਖਟੇ = create_ਚੌਖਟੇ_ret.region;

    /* create/initialise the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(ਜੜ੍ਹ_cnode_cap);
    ਜੇ (cap_get_capType(it_ap_cap) == cap_null_cap) {
        ਲਿਖੋ("ERROR: could not create ASID pool for initial thread\n");
        ਵਾਪਸ false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਗੰਢ_STATE(ks_Cur_ਸਮਾਂ) = get_Current_ਸਮਾਂ();
#ਜੇ_ਅੰਤ

    /* create the idle thread */
    create_idle_thread();

    /* Before creating the initial thread (which also switches to it)
     * we clean the cache so that any page table information written
     * as a result of calling create_frames_of_region will be correctly
     * read by the hardware page table walker */
    cleanInvalidateL1Caches();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         ਜੜ੍ਹ_cnode_cap,
                         it_pd_cap,
                         v_ਇੰਦਰਾਜ,
                         bi_ਚੌਖਟਾ_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    ਜੇ (initial == ਨਿਰਾ) {
        ਲਿਖੋ("ERROR: could not create initial thread\n");
        ਵਾਪਸ false;
    }

    init_core_state(initial);

    /* create all of the untypeds. Both devices and kernel window memory */
    ਜੇ (!create_untypeds(ਜੜ੍ਹ_cnode_cap)) {
        ਲਿਖੋ("ERROR: could not create untypteds for kernel image boot memory\n");
        ਵਾਪਸ false;
    }

    /* no shared-frame caps (ARM has no multikernel support) */
    ndks_boot.bi_ਚੌਖਟਾ->shared_ਚੌਖਟੇ = S_REG_ਖਾਲੀ_ਥਾਵਾਂ;

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
    ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        invalidateHypTLB();
    }

    ks_ਅੰਕ_CPUs = 1;

    /* initialize BKL before booting up other cores */
    SMP_COND_STATEMENT(clh_ਤਾਲਾ_init());
    SMP_COND_STATEMENT(release_secondary_cpus());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    ਗੰਢ_ਤਾਲਾ_SYS;

    ਲਿਖੋ("Booting all finished, dropped to user space\n");

    /* kernel successfully initialized */
    ਵਾਪਸ true;
}

BOOT_ਕੋਡ VISIBLE ਖਾਲੀ init_kernel(
    paddr_t ui_p_reg_ਸ਼ੁਰੂ,
    paddr_t ui_p_reg_ਸਿਰਾ,
    sword_t pv_ਵਿੱਥ,
    vptr_t  v_ਇੰਦਰਾਜ,
    paddr_t dtb_addr_p,
    uint32_t dtb_ਮਾਪ
)
{
    bool_t ਨਤੀਜਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    /* we assume there exists a cpu with id 0 and will use it for bootstrapping */
    ਜੇ (getCurrentCPUIndex() == 0) {
        ਨਤੀਜਾ = try_init_kernel(ui_p_reg_ਸ਼ੁਰੂ,
                                 ui_p_reg_ਸਿਰਾ,
                                 pv_ਵਿੱਥ,
                                 v_ਇੰਦਰਾਜ,
                                 dtb_addr_p, dtb_ਮਾਪ);
    } ਨਹੀਂਤਾਂ {
        ਨਤੀਜਾ = try_init_kernel_secondary_core();
    }

#ਨਹੀਂਤਾਂ
    ਨਤੀਜਾ = try_init_kernel(ui_p_reg_ਸ਼ੁਰੂ,
                             ui_p_reg_ਸਿਰਾ,
                             pv_ਵਿੱਥ,
                             v_ਇੰਦਰਾਜ,
                             dtb_addr_p, dtb_ਮਾਪ);

#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

    ਜੇ (!ਨਤੀਜਾ) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਗੰਢ_STATE(ks_Cur_ਸਮਾਂ) = get_Current_ਸਮਾਂ();
    ਗੰਢ_STATE(ksConsumed) = 0;
#ਜੇ_ਅੰਤ
    ਸਮਾਂ_ਸਾਰਣੀ();
    activateThread();
}
