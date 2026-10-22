/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2021, HENSOLDT Cyber
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <object/interrupt.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <arch/benchmark.h>
#ಅನ್ತರ್ಭಾವ <arch/user_access.h>
#ಅನ್ತರ್ಭಾವ <arch/object/iospace.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <machine.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/timer.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/tlb.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
#ಅನ್ತರ್ಭಾವ <drivers/smmu/smmuv2.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
/* SMP boot synchronization works based on a global variable with the initial
 * value 0, as the loader must zero all BSS variables. Secondary cores keep
 * spinning until the primary core has initialized all kernel structures and
 * then set it to 1.
 */
BOOT_BSS ಸ್ಥಿರ ಅಸ್ಥಿರ _Atomic ಪೂರ್ಣಾಂಕ ಗಂಟು_boot_ಬೀಗ;
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

BOOT_BSS ಸ್ಥಿರ region_t reserved[ಅಂಕೆ_RESERVED_REGIONS];

BOOT_ಸಂಕೇತ ಸ್ಥಿರ bool_t arch_init_freemem(p_region_t ui_p_reg,
                                          p_region_t dtb_p_reg,
                                          v_region_t it_v_reg,
                                          ಶಬ್ದ_t extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)
{
    /* reserve the kernel image region */
    reserved[0] = paddr_ಗಮ್ಯ_pptr_reg(get_p_reg_kernel_img());

    ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = 1;

    /* add the dtb region, if it is not empty */
    ಆದರೆ (dtb_p_reg.ಆರಂಭ) {
        ಆದರೆ (ಸೂಚ್ಯಂಕ >= ARRAY_ಗಾತ್ರ(reserved)) {
            ಬರೆ("ERROR: no slot to add DTB to reserved regions\n");
            ಹಿಂತಿರುಗಿ false;
        }
        reserved[ಸೂಚ್ಯಂಕ].ಆರಂಭ = (pptr_t) paddr_ಗಮ್ಯ_pptr(dtb_p_reg.ಆರಂಭ);
        reserved[ಸೂಚ್ಯಂಕ].ಕೊನೆ = (pptr_t) paddr_ಗಮ್ಯ_pptr(dtb_p_reg.ಕೊನೆ);
        ಸೂಚ್ಯಂಕ++;
    }

    /* Reserve the user image region and the mode-reserved regions. For now,
     * only one mode-reserved region is supported, because this is all that is
     * needed.
     */
    ಆದರೆ (ಬಹುಲಕ_RESERVED > 1) {
        ಬರೆ("ERROR: MODE_RESERVED > 1 unsupported!\n");
        ಹಿಂತಿರುಗಿ false;
    }
    ಆದರೆ (ui_p_reg.ಆರಂಭ < PADDR_ತುದಿ) {
        region_t ui_reg = paddr_ಗಮ್ಯ_pptr_reg(ui_p_reg);
        ಆದರೆ (ಬಹುಲಕ_RESERVED == 1) {
            ಆದರೆ (ಸೂಚ್ಯಂಕ + 1 >= ARRAY_ಗಾತ್ರ(reserved)) {
                ಬರೆ("ERROR: no slot to add the user image and the "
                       "mode-reserved region to the reserved regions\n");
                ಹಿಂತಿರುಗಿ false;
            }
            ಆದರೆ (ui_reg.ಕೊನೆ > ಬಹುಲಕ_reserved_region[0].ಆರಂಭ) {
                reserved[ಸೂಚ್ಯಂಕ] = ಬಹುಲಕ_reserved_region[0];
                ಸೂಚ್ಯಂಕ++;
                reserved[ಸೂಚ್ಯಂಕ] = ui_reg;
            } ಇಲ್ಲದಿದ್ದರೆ {
                reserved[ಸೂಚ್ಯಂಕ] = ui_reg;
                ಸೂಚ್ಯಂಕ++;
                reserved[ಸೂಚ್ಯಂಕ] = ಬಹುಲಕ_reserved_region[0];
            }
            ಸೂಚ್ಯಂಕ++;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಆದರೆ (ಸೂಚ್ಯಂಕ >= ARRAY_ಗಾತ್ರ(reserved)) {
                ಬರೆ("ERROR: no slot to add the user image to the reserved"
                       "regions\n");
                ಹಿಂತಿರುಗಿ false;
            }
            reserved[ಸೂಚ್ಯಂಕ] = ui_reg;
            ಸೂಚ್ಯಂಕ++;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (ಬಹುಲಕ_RESERVED == 1) {
            ಆದರೆ (ಸೂಚ್ಯಂಕ >= ARRAY_ಗಾತ್ರ(reserved)) {
                ಬರೆ("ERROR: no slot to add the mode-reserved region\n");
                ಹಿಂತಿರುಗಿ false;
            }
            reserved[ಸೂಚ್ಯಂಕ] = ಬಹುಲಕ_reserved_region[0];
            ಸೂಚ್ಯಂಕ++;
        }

        /* Reserve the ui_p_reg region still so it doesn't get turned into device UT. */
        reserve_region(ui_p_reg);
    }

    /* avail_p_regs comes from the auto-generated code */
    ಹಿಂತಿರುಗಿ init_freemem(ARRAY_ಗಾತ್ರ(avail_p_regs), avail_p_regs,
                        ಸೂಚ್ಯಂಕ, reserved,
                        it_v_reg, extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
}


BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ init_irqs(cap_t ಬೇರು_cnode_cap)
{
    ಚಿಹ್ನೆರಹಿತ i;

    ಗಾಗಿ (i = 0; i <= maxIRQ ; i++) {
        ಗಣ_IRQState(IRQInactive, CORE_IRQ_ಗಮ್ಯ_IRQT(0, i));
    }
    ಗಣ_IRQState(IRQTimer, CORE_IRQ_ಗಮ್ಯ_IRQT(0, KERNEL_TIMER_IRQ));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಗಣ_IRQState(IRQReserved, CORE_IRQ_ಗಮ್ಯ_IRQT(0, ಅಡಚಣೆ_VGIC_MAINTENANCE));
    ಗಣ_IRQState(IRQReserved, CORE_IRQ_ಗಮ್ಯ_IRQT(0, ಅಡಚಣೆ_VTIMER_EVENT));
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU
    ಗಣ_IRQState(IRQReserved, CORE_IRQ_ಗಮ್ಯ_IRQT(0, ಅಡಚಣೆ_SMMU));
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_ENABLE_PMU_ಅತಿಪ್ರವಾಹ_ಅಡಚಣೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ KERNEL_PMU_IRQ
    ಗಣ_IRQState(IRQReserved, CORE_IRQ_ಗಮ್ಯ_IRQT(0, KERNEL_PMU_IRQ));
#ಆದರೆ (defined CONFIG_PLAT_TX1 && defined ENABLE_SMP_SUPPORT)
//SELFOUR-1252
#ದೋಷ "This platform doesn't support tracking CPU utilisation on multicore"
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_PLAT_TX1 && ENABLE_SMP_SUPPORT */
#ಇಲ್ಲದಿದ್ದರೆ
#ದೋಷ "This platform doesn't support tracking CPU utilisation feature"
#ಆದರೆ_ಅಂತ್ಯ /* KERNEL_TIMER_IRQ */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_ENABLE_PMU_OVERFLOW_INTERRUPT */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ಗಣ_IRQState(IRQIPI, CORE_IRQ_ಗಮ್ಯ_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    ಗಣ_IRQState(IRQIPI, CORE_IRQ_ಗಮ್ಯ_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
#ಆದರೆ_ಅಂತ್ಯ

    /* provide the IRQ control cap */
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapIRQControl), cap_irq_control_cap_new());
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ init_smmu(cap_t ಬೇರು_cnode_cap)
{
    plat_smmu_init();
    /*provide the SID and CB control cap*/
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapSMMUSIDControl), cap_sid_control_cap_new());
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapSMMUCBControl), cap_cb_control_cap_new());
}

#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ALLOW_SMC_CALLS
BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ init_smc(cap_t ಬೇರು_cnode_cap)
{
    /* Provide the SMC cap*/
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapSMC), cap_smc_cap_new(0));
}
#ಆದರೆ_ಅಂತ್ಯ

/** This and only this function initialises the CPU.
 *
 * It does NOT initialise any kernel state.
 * @return For the verification build, this currently returns true always.
 */
BOOT_ಸಂಕೇತ ಸ್ಥಿರ bool_t init_cpu(ಶೂನ್ಯ)
{
    bool_t haveHWFPU;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH64
    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        ಆದರೆ (!checkTCR_EL2()) {
            ಹಿಂತಿರುಗಿ false;
        }
    }
#ಆದರೆ_ಅಂತ್ಯ

    activate_kernel_vspace();
    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        vcpu_boot_init();
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH64
    /* After activate_kernel_vspace(), so printf() is available. */
    ಆದರೆ (!config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT) && !check16_ಬಿಟ್_ASID()) {
        ಬರೆ("ERROR: kernel requires 16-bit ASIDs\n");
        ಹಿಂತಿರುಗಿ false;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API
    ಆದರೆ (!Arch_initHardwareBreakpoints()) {
        ಬರೆ("Kernel built with CONFIG_HARDWARE_DEBUG_API, but this board doesn't "
               "reliably support it.\n");
        ಹಿಂತಿರುಗಿ false;
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* Setup kernel stack pointer.
     * On ARM SMP, the array index here is the CPU ID
     */
    ಶಬ್ದ_t ರಾಶಿ_ತುದಿ = ((ಶಬ್ದ_t) kernel_ರಾಶಿ_alloc[CURRENT_CPU_ಸೂಚ್ಯಂಕ()]) + ಬಿಟ್(CONFIG_KERNEL_ರಾಶಿ_ಬಿಟ್ಟುಗಳು);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH64
    /* the least 12 bits are used to store logical core ID */
    ರಾಶಿ_ತುದಿ |= getCurrentCPUIndex();
#ಅಥವಾಆದರೆ defined(CONFIG_ARCH_AARCH32)
    /* Stack address encodes core ID, ensure it is in the right region */
    ರಾಶಿ_ತುದಿ -= 8;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
    ಗಣ_Kernel_ರಾಶಿ(ರಾಶಿ_ತುದಿ);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH64
    /* initialise CPU's exception vector table */
    ಗಣ_Vtable((pptr_t)arm_ಸದಿಶ_ಕೋಷ್ಟಕ);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARCH_AARCH64 */

    haveHWFPU = fpsimd_HWCapTest();

    /* Disable FPU to avoid channels where a platform has an FPU but doesn't make use of it */
    ಆದರೆ (haveHWFPU) {
        disableFpu();
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
    ಆದರೆ (haveHWFPU) {
        ಆದರೆ (!fpsimd_init()) {
            ಹಿಂತಿರುಗಿ false;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಬರೆ("Platform claims to have FP hardware, but does not!\n");
        ಹಿಂತಿರುಗಿ false;
    }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_HAVE_FPU */

    cpu_initLocalIRQController();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ENABLE_BENCHMARKS
    arm_init_ccnt();
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ENABLE_BENCHMARKS */

    /* Export selected CPU features for access by PL0 */
    armv_init_user_access();

    initTimer();

    ಹಿಂತಿರುಗಿ true;
}

/* This and only this function initialises the platform. It does NOT initialise any kernel state. */

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ init_plat(ಶೂನ್ಯ)
{
    initIRQController();
    initL2Cache();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
    plat_smmu_init();
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
BOOT_ಸಂಕೇತ ಸ್ಥಿರ bool_t try_init_kernel_secondary_core(ಶೂನ್ಯ)
{
    /* need to first wait until some kernel init has been done */
    ಆಗ (!ಗಂಟು_boot_ಬೀಗ);

    /* Perform cpu init */
    init_cpu();

    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_PPI; i++) {
        ಮುಖವಾಡ_ಅಡಚಣೆ(true, CORE_IRQ_ಗಮ್ಯ_IRQT(getCurrentCPUIndex(), i));
    }
    ಗಣ_IRQState(IRQIPI, CORE_IRQ_ಗಮ್ಯ_IRQT(getCurrentCPUIndex(), irq_remote_call_ipi));
    ಗಣ_IRQState(IRQIPI, CORE_IRQ_ಗಮ್ಯ_IRQT(getCurrentCPUIndex(), irq_reschedule_ipi));
    /* Enable per-CPU timer interrupts */
    ಗಣ_IRQState(IRQTimer, CORE_IRQ_ಗಮ್ಯ_IRQT(getCurrentCPUIndex(), KERNEL_TIMER_IRQ));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಗಣ_IRQState(IRQReserved, CORE_IRQ_ಗಮ್ಯ_IRQT(getCurrentCPUIndex(), ಅಡಚಣೆ_VGIC_MAINTENANCE));
    ಗಣ_IRQState(IRQReserved, CORE_IRQ_ಗಮ್ಯ_IRQT(getCurrentCPUIndex(), ಅಡಚಣೆ_VTIMER_EVENT));
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    ಗಂಟು_ಬೀಗ_SYS;

    clock_sync_test();
    ks_ಅಂಕೆ_CPUs++;

    init_core_state(SchedulerAction_ResumeCurrentThread);

    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ release_secondary_cpus(ಶೂನ್ಯ)
{
    /* release the cpus at the same time */
    ಪ್ರತಿಜ್ಞಾ(0 == ಗಂಟು_boot_ಬೀಗ); /* Sanity check for a proper lock state. */
    ಗಂಟು_boot_ಬೀಗ = 1;

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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH32
    cleanInvalidateL1Caches();
    plat_cleanInvalidateL2Cache();
#ಆದರೆ_ಅಂತ್ಯ

    /* Wait until all the secondary cores are done initialising */
    ಆಗ (ks_ಅಂಕೆ_CPUs != CONFIG_MAX_ಅಂಕೆ_NODES) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
        ಗಂಟು_STATE(ks_Cur_ಸಮಯ) = get_Current_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ
        /* perform a memory acquire to get new values of ksNumCPUs, release for ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQ_REL);
    }
}
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

/* Main kernel initialisation function. */

ಸ್ಥಿರ BOOT_ಸಂಕೇತ bool_t try_init_kernel(
    paddr_t ui_p_reg_ಆರಂಭ,
    paddr_t ui_p_reg_ಕೊನೆ,
    sword_t pv_ಸ್ಥಾನಾಂತರ,
    vptr_t  v_ನಮೂದು,
    paddr_t dtb_phys_addr,
    ಶಬ್ದ_t  dtb_ಗಾತ್ರ
)
{
    cap_t ಬೇರು_cnode_cap;
    cap_t it_ap_cap;
    cap_t it_pd_cap;
    cap_t ipcbuf_cap;
    p_region_t ui_p_reg = (p_region_t) {
        ui_p_reg_ಆರಂಭ, ui_p_reg_ಕೊನೆ
    };
    region_t ui_reg = paddr_ಗಮ್ಯ_pptr_reg(ui_p_reg);
    ಶಬ್ದ_t extra_bi_ಗಾತ್ರ = 0;
    pptr_t extra_bi_ಸ್ಥಾನಾಂತರ = 0;
    vptr_t extra_bi_ಚೌಕಟ್ಟು_vptr;
    vptr_t bi_ಚೌಕಟ್ಟು_vptr;
    vptr_t ipcbuf_vptr;
    create_ಚೌಕಟ್ಟುಗಳು_ದ_region_ret_t create_ಚೌಕಟ್ಟುಗಳು_ret;
    create_ಚೌಕಟ್ಟುಗಳು_ದ_region_ret_t extra_bi_ret;

    /* convert from physical addresses to userland vptrs */
    v_region_t ui_v_reg = {
        .ಆರಂಭ = ui_p_reg_ಆರಂಭ - pv_ಸ್ಥಾನಾಂತರ,
        .ಕೊನೆ   = ui_p_reg_ಕೊನೆ   - pv_ಸ್ಥಾನಾಂತರ
    };

    ipcbuf_vptr = ui_v_reg.ಕೊನೆ;
    bi_ಚೌಕಟ್ಟು_vptr = ipcbuf_vptr + ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು);
    extra_bi_ಚೌಕಟ್ಟು_vptr = bi_ಚೌಕಟ್ಟು_vptr + ಬಿಟ್(se_L4_Boot_Info_ಚೌಕಟ್ಟು_ಬಿಟ್ಟುಗಳು);

    /* setup virtual memory for the kernel */
    map_kernel_ಕಿಟಕಿ();

    /* initialise the CPU */
    ಆದರೆ (!init_cpu()) {
        ಬರೆ("ERROR: CPU init failed\n");
        ಹಿಂತಿರುಗಿ false;
    }

    /* debug output via serial port is only available from here */
    ಬರೆ("Bootstrapping kernel\n");

    /* initialise the platform */
    init_plat();

    /* If a DTB was provided, pass the data on as extra bootinfo */
    p_region_t dtb_p_reg = P_REG_ಖಾಲಿಗಳು;
    ಆದರೆ (dtb_ಗಾತ್ರ > 0) {
        paddr_t dtb_phys_ಕೊನೆ = dtb_phys_addr + dtb_ಗಾತ್ರ;
        ಆದರೆ (dtb_phys_ಕೊನೆ < dtb_phys_addr) {
            /* An integer overflow happened in DTB end address calculation, the
             * location or size passed seems invalid.
             */
            ಬರೆ("ERROR: DTB location at %"SEL4_PRIx_ಶಬ್ದ
                   " len %"SEL4_PRIu_ಶಬ್ದ" invalid\n",
                   dtb_phys_addr, dtb_ಗಾತ್ರ);
            ಹಿಂತಿರುಗಿ false;
        }
        /* If the DTB is located in physical memory that is not mapped in the
         * kernel window we cannot access it.
         */
        ಆದರೆ (dtb_phys_ಕೊನೆ >= PADDR_ತುದಿ) {
            ಬರೆ("ERROR: DTB at [%"SEL4_PRIx_ಶಬ್ದ"..%"SEL4_PRIx_ಶಬ್ದ") "
                   "exceeds PADDR_TOP (%"SEL4_PRIx_ಶಬ್ದ")\n",
                   dtb_phys_addr, dtb_phys_ಕೊನೆ, PADDR_ತುದಿ);
            ಹಿಂತಿರುಗಿ false;
        }
        /* DTB seems valid and accessible, pass it on in bootinfo. */
        extra_bi_ಗಾತ್ರ += ಗಾತ್ರ(se_L4_Boot_Info_ಶೀರ್ಷಿಕೆ) + dtb_ಗಾತ್ರ;
        /* Remember the memory region it uses. */
        dtb_p_reg = (p_region_t) {
            .ಆರಂಭ = dtb_phys_addr,
            .ಕೊನೆ   = dtb_phys_ಕೊನೆ
        };
    }

    /* The region of the initial thread is the user image + ipcbuf and boot info */
    ಶಬ್ದ_t extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು = calculate_extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(extra_bi_ಗಾತ್ರ);
    v_region_t it_v_reg = {
        .ಆರಂಭ = ui_v_reg.ಆರಂಭ,
        .ಕೊನೆ   = extra_bi_ಚೌಕಟ್ಟು_vptr + (extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು > 0 ? ಬಿಟ್(extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು) : 0)
    };
    ಆದರೆ (it_v_reg.ಕೊನೆ >= USER_ತುದಿ) {
        /* Variable arguments for printf() require well defined integer types to
         * work properly. Unfortunately, the definition of USER_TOP differs
         * between platforms (int, long), so we have to cast here to play safe.
         */
        ಬರೆ("ERROR: userland image virt [%"SEL4_PRIx_ಶಬ್ದ"..%"SEL4_PRIx_ಶಬ್ದ")"
               "exceeds USER_TOP (%"SEL4_PRIx_ಶಬ್ದ")\n",
               it_v_reg.ಆರಂಭ, it_v_reg.ಕೊನೆ, (ಶಬ್ದ_t)USER_ತುದಿ);
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (!arch_init_freemem(ui_p_reg, dtb_p_reg, it_v_reg, extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)) {
        ಬರೆ("ERROR: free memory management initialization failed\n");
        ಹಿಂತಿರುಗಿ false;
    }

    /* create the root cnode */
    ಬೇರು_cnode_cap = create_ಬೇರು_cnode();
    ಆದರೆ (cap_get_capType(ಬೇರು_cnode_cap) == cap_null_cap) {
        ಬರೆ("ERROR: root c-node creation failed\n");
        ಹಿಂತಿರುಗಿ false;
    }

    /* create the cap for managing thread domains */
    create_domain_cap(ಬೇರು_cnode_cap);

    /* initialise the IRQ states and provide the IRQ control cap */
    init_irqs(ಬೇರು_cnode_cap);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
    /* initialise the SMMU and provide the SMMU control caps*/
    init_smmu(ಬೇರು_cnode_cap);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ALLOW_SMC_CALLS
    init_smc(ಬೇರು_cnode_cap);
#ಆದರೆ_ಅಂತ್ಯ

    populate_bi_ಚೌಕಟ್ಟು(0, CONFIG_MAX_ಅಂಕೆ_NODES, ipcbuf_vptr, extra_bi_ಗಾತ್ರ);

    /* put DTB in the bootinfo block, if present. */
    se_L4_Boot_Info_ಶೀರ್ಷಿಕೆ ಶೀರ್ಷಿಕೆ;
    ಆದರೆ (dtb_ಗಾತ್ರ > 0) {
        ಶೀರ್ಷಿಕೆ.id = SEL4_BOOTINFO_ಶೀರ್ಷಿಕೆ_FDT;
        ಶೀರ್ಷಿಕೆ.len = ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ) + dtb_ಗಾತ್ರ;
        *(se_L4_Boot_Info_ಶೀರ್ಷಿಕೆ *)(rootserver.extra_bi + extra_bi_ಸ್ಥಾನಾಂತರ) = ಶೀರ್ಷಿಕೆ;
        extra_bi_ಸ್ಥಾನಾಂತರ += ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ);
        ಸ್ಮರಣೆ_ನಕಲು((ಶೂನ್ಯ *)(rootserver.extra_bi + extra_bi_ಸ್ಥಾನಾಂತರ),
               paddr_ಗಮ್ಯ_pptr(dtb_phys_addr),
               dtb_ಗಾತ್ರ);
        extra_bi_ಸ್ಥಾನಾಂತರ += dtb_ಗಾತ್ರ;
    }

    ಪ್ರತಿಜ್ಞಾ(extra_bi_ಗಾತ್ರ == extra_bi_ಸ್ಥಾನಾಂತರ);

    ಆದರೆ (config_ಗಣ(CONFIG_TK1_SMMU)) {
        ndks_boot.bi_ಚೌಕಟ್ಟು->ioSpaceCaps = create_iospace_caps(ಬೇರು_cnode_cap);
        ಆದರೆ (ndks_boot.bi_ಚೌಕಟ್ಟು->ioSpaceCaps.ಆರಂಭ == 0 &&
            ndks_boot.bi_ಚೌಕಟ್ಟು->ioSpaceCaps.ಕೊನೆ == 0) {
            ಬರೆ("ERROR: SMMU I/O space creation failed\n");
            ಹಿಂತಿರುಗಿ false;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ndks_boot.bi_ಚೌಕಟ್ಟು->ioSpaceCaps = S_REG_ಖಾಲಿಗಳು;
    }

    /* Construct an initial address space with enough virtual addresses
     * to cover the user image + ipc buffer and bootinfo frames */
    it_pd_cap = create_it_ವಿಳಾಸ_space(ಬೇರು_cnode_cap, it_v_reg);
    ಆದರೆ (cap_get_capType(it_pd_cap) == cap_null_cap) {
        ಬರೆ("ERROR: address space creation for initial thread failed\n");
        ಹಿಂತಿರುಗಿ false;
    }

    /* Create and map bootinfo frame cap */
    create_bi_ಚೌಕಟ್ಟು_cap(
        ಬೇರು_cnode_cap,
        it_pd_cap,
        bi_ಚೌಕಟ್ಟು_vptr
    );

    /* create and map extra bootinfo region */
    ಆದರೆ (extra_bi_ಗಾತ್ರ > 0) {
        region_t extra_bi_region = {
            .ಆರಂಭ = rootserver.extra_bi,
            .ಕೊನೆ = rootserver.extra_bi + extra_bi_ಗಾತ್ರ
        };
        extra_bi_ret =
            create_ಚೌಕಟ್ಟುಗಳು_ದ_region(
                ಬೇರು_cnode_cap,
                it_pd_cap,
                extra_bi_region,
                true,
                pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)extra_bi_region.ಆರಂಭ) - extra_bi_ಚೌಕಟ್ಟು_vptr
            );
        ಆದರೆ (!extra_bi_ret.success) {
            ಬರೆ("ERROR: mapping extra boot info to initial thread failed\n");
            ಹಿಂತಿರುಗಿ false;
        }
        ndks_boot.bi_ಚೌಕಟ್ಟು->extraBIPages = extra_bi_ret.region;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    init_sched_control(ಬೇರು_cnode_cap, CONFIG_MAX_ಅಂಕೆ_NODES);
#ಆದರೆ_ಅಂತ್ಯ

    /* create the initial thread's IPC buffer */
    ipcbuf_cap = create_ipcbuf_ಚೌಕಟ್ಟು_cap(ಬೇರು_cnode_cap, it_pd_cap, ipcbuf_vptr);
    ಆದರೆ (cap_get_capType(ipcbuf_cap) == cap_null_cap) {
        ಬರೆ("ERROR: could not create IPC buffer for initial thread\n");
        ಹಿಂತಿರುಗಿ false;
    }

    /* create all userland image frames */
    create_ಚೌಕಟ್ಟುಗಳು_ret =
        create_ಚೌಕಟ್ಟುಗಳು_ದ_region(
            ಬೇರು_cnode_cap,
            it_pd_cap,
            ui_reg,
            true,
            pv_ಸ್ಥಾನಾಂತರ
        );
    ಆದರೆ (!create_ಚೌಕಟ್ಟುಗಳು_ret.success) {
        ಬರೆ("ERROR: could not create all userland image frames\n");
        ಹಿಂತಿರುಗಿ false;
    }
    ndks_boot.bi_ಚೌಕಟ್ಟು->user_Image_ಚೌಕಟ್ಟುಗಳು = create_ಚೌಕಟ್ಟುಗಳು_ret.region;

    /* create/initialise the initial thread's ASID pool */
    it_ap_cap = create_it_asid_pool(ಬೇರು_cnode_cap);
    ಆದರೆ (cap_get_capType(it_ap_cap) == cap_null_cap) {
        ಬರೆ("ERROR: could not create ASID pool for initial thread\n");
        ಹಿಂತಿರುಗಿ false;
    }
    write_it_asid_pool(it_ap_cap, it_pd_cap);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಗಂಟು_STATE(ks_Cur_ಸಮಯ) = get_Current_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ

    /* create the idle thread */
    create_idle_thread();

    /* Before creating the initial thread (which also switches to it)
     * we clean the cache so that any page table information written
     * as a result of calling create_frames_of_region will be correctly
     * read by the hardware page table walker */
    cleanInvalidateL1Caches();

    /* create the initial thread */
    tcb_t *initial = create_initial_thread(
                         ಬೇರು_cnode_cap,
                         it_pd_cap,
                         v_ನಮೂದು,
                         bi_ಚೌಕಟ್ಟು_vptr,
                         ipcbuf_vptr,
                         ipcbuf_cap
                     );

    ಆದರೆ (initial == ಖಾಲಿ) {
        ಬರೆ("ERROR: could not create initial thread\n");
        ಹಿಂತಿರುಗಿ false;
    }

    init_core_state(initial);

    /* create all of the untypeds. Both devices and kernel window memory */
    ಆದರೆ (!create_untypeds(ಬೇರು_cnode_cap)) {
        ಬರೆ("ERROR: could not create untypteds for kernel image boot memory\n");
        ಹಿಂತಿರುಗಿ false;
    }

    /* no shared-frame caps (ARM has no multikernel support) */
    ndks_boot.bi_ಚೌಕಟ್ಟು->shared_ಚೌಕಟ್ಟುಗಳು = S_REG_ಖಾಲಿಗಳು;

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
    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        invalidateHypTLB();
    }

    ks_ಅಂಕೆ_CPUs = 1;

    /* initialize BKL before booting up other cores */
    SMP_COND_STATEMENT(clh_ಬೀಗ_init());
    SMP_COND_STATEMENT(release_secondary_cpus());

    /* All cores are up now, so there can be concurrency. The kernel booting is
     * supposed to be finished before the secondary cores are released, all the
     * primary has to do now is schedule the initial thread. Currently there is
     * nothing that touches any global data structures, nevertheless we grab the
     * BKL here to play safe. It is released when the kernel is left. */
    ಗಂಟು_ಬೀಗ_SYS;

    ಬರೆ("Booting all finished, dropped to user space\n");

    /* kernel successfully initialized */
    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ VISIBLE ಶೂನ್ಯ init_kernel(
    paddr_t ui_p_reg_ಆರಂಭ,
    paddr_t ui_p_reg_ಕೊನೆ,
    sword_t pv_ಸ್ಥಾನಾಂತರ,
    vptr_t  v_ನಮೂದು,
    paddr_t dtb_addr_p,
    uint32_t dtb_ಗಾತ್ರ
)
{
    bool_t ಫಲಿತಾಂಶ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    /* we assume there exists a cpu with id 0 and will use it for bootstrapping */
    ಆದರೆ (getCurrentCPUIndex() == 0) {
        ಫಲಿತಾಂಶ = try_init_kernel(ui_p_reg_ಆರಂಭ,
                                 ui_p_reg_ಕೊನೆ,
                                 pv_ಸ್ಥಾನಾಂತರ,
                                 v_ನಮೂದು,
                                 dtb_addr_p, dtb_ಗಾತ್ರ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಫಲಿತಾಂಶ = try_init_kernel_secondary_core();
    }

#ಇಲ್ಲದಿದ್ದರೆ
    ಫಲಿತಾಂಶ = try_init_kernel(ui_p_reg_ಆರಂಭ,
                             ui_p_reg_ಕೊನೆ,
                             pv_ಸ್ಥಾನಾಂತರ,
                             v_ನಮೂದು,
                             dtb_addr_p, dtb_ಗಾತ್ರ);

#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

    ಆದರೆ (!ಫಲಿತಾಂಶ) {
        fail("ERROR: kernel init failed");
        UNREACHABLE();
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಗಂಟು_STATE(ks_Cur_ಸಮಯ) = get_Current_ಸಮಯ();
    ಗಂಟು_STATE(ksConsumed) = 0;
#ಆದರೆ_ಅಂತ್ಯ
    ವೇಳಾಪಟ್ಟಿ();
    activateThread();
}
