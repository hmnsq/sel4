/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <mode/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <mode/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <util.h>

/*
 * The following function checks if the subarchitecture support asynchronous exceptions
 */
BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool_t supportsAsyncExceptions(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t fpexc;

    /* Set FPEXC.EX=1 */
    VMRS(FPEXC, fpexc);
    fpexc |= ಬಿಟ್(FPEXC_EX_ಬಿಟ್);
    VMSR(FPEXC, fpexc);

    /* Read back the FPEXC register*/
    VMRS(FPEXC, fpexc);

    ಹಿಂತಿರುಗಿ !!(fpexc & ಬಿಟ್(FPEXC_EX_ಬಿಟ್));
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
/* This variable is set at boot/init time to true if the FPU supports 32 registers (d0-d31).
 * otherwise it only supports 16 registers (d0-d15).
 * We cache this value in the following variable to avoid reading the coprocessor
 * on every FPU context switch, since it shouldn't change for one platform on run-time.
 */
bool_t isFPUD32SupportedCached;

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool_t isFPUD32Supported(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t mvfr0;
    asm ಅಸ್ಥಿರ(".word 0xeef73a10 \n"   /* vmrs    r3, mvfr0 */
                 "mov %0, r3       \n"
                 : "=r"(mvfr0)
                 :
                 : "r3");
    ಹಿಂತಿರುಗಿ ((mvfr0 & 0xf) == 2);
}

/* Initialise the FP/SIMD for this machine. */
BOOT_ಸಂಕೇತ bool_t fpsimd_init(ಶೂನ್ಯ)
{
    /* We are inside CONFIG_HAVE_FPU, so CPACR_DEFAULT enables CP10 and CP11 access. */
    ಗಣ_CPACR(CPACR_DEFAULT);
    isb();

    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* Allow hyp mode FPU accesses for the rest of the initialisation. */
        enable_Fpu_Inst_ಒಳ_Hyp();
    }

    ಆದರೆ (supportsAsyncExceptions()) {
        /* In the future, when we've targets that support asynchronous FPU exceptions, we've to support them */
        ಬರೆ("Error: seL4 doesn't support FPU subarchitectures that support asynchronous exceptions\n");
        ಹಿಂತಿರುಗಿ false;
    }

    isFPUD32SupportedCached = isFPUD32Supported();
    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* FPEXC is VCPU state and native threads always have EN set. Setting EN
         * must happen before disableFpu() traps hyp mode (kernel) FPU accesses
         * again. */
        ಗಣ_En_FPEXC();
    }
    /* Set the FPU to lazy switch mode */
    disableFpu();

    ಹಿಂತಿರುಗಿ true;
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_HAVE_FPU */

/* Probe the FPU hardware. Requires CP10 and CP11 access in CPACR and, in hyp
 * mode, no CP10 and CP11 traps in HCPTR. */
BOOT_ಸಂಕೇತ ಸ್ಥಿರ bool_t fpsimd_HWCapProbe(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t fpsid;

    /* Check of this platform supports HW FP instructions */
    asm ಅಸ್ಥಿರ(".word 0xeef00a10  \n"  /* vmrs    r0, fpsid */
                 "mov %0, r0        \n"
                 : "=r"(fpsid) :
                 : "r0");
    ಆದರೆ (fpsid & ಬಿಟ್(FPSID_SW_ಬಿಟ್)) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಶಬ್ದ_t fpsid_subarch;

    ಆದರೆ (supportsAsyncExceptions()) {
        /* In the future, when we've targets that support asynchronous FPU exceptions, we've to support them */
        ಆದರೆ (config_ಗಣ(CONFIG_HAVE_FPU)) {
            ಬರೆ("Error: seL4 doesn't support FPU subarchitectures that support asynchronous exceptions\n");
            ಹಿಂತಿರುಗಿ false;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // if we aren't using the fpu then we have detected an fpu that we cannot use, but that is fine
            ಹಿಂತಿರುಗಿ true;
        }
    }
    /* Check for subarchitectures we support */
    fpsid_subarch = (fpsid >> FPSID_SUBARCH_ಸರಿಸು_POS) & 0x7f;

    ಆಯ್ಕೆ (fpsid_subarch) {
    /* We only support the following subarch values */
    ಸ್ಥಿತಿ 0x2:
    ಸ್ಥಿತಿ 0x3:
    ಸ್ಥಿತಿ 0x4:
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ: {
        ಆದರೆ (config_ಗಣ(CONFIG_HAVE_FPU)) {
            ಬರೆ("Error: seL4 doesn't support this VFP subarchitecture\n");
            ಹಿಂತಿರುಗಿ false;
        } ಇಲ್ಲದಿದ್ದರೆ {
            // if we aren't using the fpu then we have detected an fpu that we cannot use, but that is fine
            ಹಿಂತಿರುಗಿ true;
        }
    }

    }

    ಆದರೆ (!config_ಗಣ(CONFIG_HAVE_FPU)) {
        ಬರೆ("Info: Not using supported FPU as FPU is disabled in the build configuration\n");
    }
    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ bool_t fpsimd_HWCapTest(ಶೂನ್ಯ)
{
    bool_t ಫಲಿತಾಂಶ;

    /* Enable CP10 and CP11 access to read the FPU control/status registers. */
    ಗಣ_CPACR(CPACR_FPU_ENABLE);
    isb();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Allow hyp mode (kernel) FPU accesses, then restore HCPTR afterwards. */
    ಶಬ್ದ_t hcptr = getHCPTR();
    ಗಣ_HCPTR(hcptr & ~HCPTR_FPU_TRAPS);
#ಆದರೆ_ಅಂತ್ಯ

    ಫಲಿತಾಂಶ = fpsimd_HWCapProbe();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಗಣ_HCPTR(hcptr);
#ಆದರೆ_ಅಂತ್ಯ

    /* Restore the CPACR value for native threads. Without CONFIG_HAVE_FPU this
     * disables CP10 and CP11 access again. */
    ಗಣ_CPACR(CPACR_DEFAULT);
    isb();

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}
