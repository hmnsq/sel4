/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <mode/machine.h>
#समावेश <arch/machine/fpu.h>
#समावेश <mode/model/statedata.h>
#समावेश <config.h>
#समावेश <util.h>

/*
 * The following function checks if the subarchitecture support asynchronous exceptions
 */
BOOT_CODE अटल अंतरभूत bool_t supportsAsyncExceptions(व्योम)
{
    word_t fpexc;

    /* Set FPEXC.EX=1 */
    VMRS(FPEXC, fpexc);
    fpexc |= BIT(FPEXC_EX_BIT);
    VMSR(FPEXC, fpexc);

    /* Read back the FPEXC register*/
    VMRS(FPEXC, fpexc);

    वापस !!(fpexc & BIT(FPEXC_EX_BIT));
}

#अगर_घोषित CONFIG_HAVE_FPU
/* This variable is set at boot/init time to true if the FPU supports 32 registers (d0-d31).
 * otherwise it only supports 16 registers (d0-d15).
 * We cache this value in the following variable to avoid reading the coprocessor
 * on every FPU context switch, since it shouldn't change for one platform on run-time.
 */
bool_t isFPUD32SupportedCached;

BOOT_CODE अटल अंतरभूत bool_t isFPUD32Supported(व्योम)
{
    word_t mvfr0;
    यंत्र अस्थिर(".word 0xeef73a10 \n"   /* vmrs    r3, mvfr0 */
                 "mov %0, r3       \n"
                 : "=r"(mvfr0)
                 :
                 : "r3");
    वापस ((mvfr0 & 0xf) == 2);
}

/* Initialise the FP/SIMD for this machine. */
BOOT_CODE bool_t fpsimd_init(व्योम)
{
    /* We are inside CONFIG_HAVE_FPU, so CPACR_DEFAULT enables CP10 and CP11 access. */
    setCPACR(CPACR_DEFAULT);
    isb();

    अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* Allow hyp mode FPU accesses for the rest of the initialisation. */
        enableFpuInstInHyp();
    }

    अगर (supportsAsyncExceptions()) {
        /* In the future, when we've targets that support asynchronous FPU exceptions, we've to support them */
        म_लिखो("Error: seL4 doesn't support FPU subarchitectures that support asynchronous exceptions\n");
        वापस false;
    }

    isFPUD32SupportedCached = isFPUD32Supported();
    अगर (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* FPEXC is VCPU state and native threads always have EN set. Setting EN
         * must happen before disableFpu() traps hyp mode (kernel) FPU accesses
         * again. */
        setEnFPEXC();
    }
    /* Set the FPU to lazy switch mode */
    disableFpu();

    वापस true;
}
#पूर्ण_अगर /* CONFIG_HAVE_FPU */

/* Probe the FPU hardware. Requires CP10 and CP11 access in CPACR and, in hyp
 * mode, no CP10 and CP11 traps in HCPTR. */
BOOT_CODE अटल bool_t fpsimd_HWCapProbe(व्योम)
{
    word_t fpsid;

    /* Check of this platform supports HW FP instructions */
    यंत्र अस्थिर(".word 0xeef00a10  \n"  /* vmrs    r0, fpsid */
                 "mov %0, r0        \n"
                 : "=r"(fpsid) :
                 : "r0");
    अगर (fpsid & BIT(FPSID_SW_BIT)) {
        वापस false;
    }

    word_t fpsid_subarch;

    अगर (supportsAsyncExceptions()) {
        /* In the future, when we've targets that support asynchronous FPU exceptions, we've to support them */
        अगर (config_set(CONFIG_HAVE_FPU)) {
            म_लिखो("Error: seL4 doesn't support FPU subarchitectures that support asynchronous exceptions\n");
            वापस false;
        } अन्यथा {
            // if we aren't using the fpu then we have detected an fpu that we cannot use, but that is fine
            वापस true;
        }
    }
    /* Check for subarchitectures we support */
    fpsid_subarch = (fpsid >> FPSID_SUBARCH_SHIFT_POS) & 0x7f;

    चयन (fpsid_subarch) {
    /* We only support the following subarch values */
    हाल 0x2:
    हाल 0x3:
    हाल 0x4:
        अवरोध;
    शेष: {
        अगर (config_set(CONFIG_HAVE_FPU)) {
            म_लिखो("Error: seL4 doesn't support this VFP subarchitecture\n");
            वापस false;
        } अन्यथा {
            // if we aren't using the fpu then we have detected an fpu that we cannot use, but that is fine
            वापस true;
        }
    }

    }

    अगर (!config_set(CONFIG_HAVE_FPU)) {
        म_लिखो("Info: Not using supported FPU as FPU is disabled in the build configuration\n");
    }
    वापस true;
}

BOOT_CODE bool_t fpsimd_HWCapTest(व्योम)
{
    bool_t परिणाम;

    /* Enable CP10 and CP11 access to read the FPU control/status registers. */
    setCPACR(CPACR_FPU_ENABLE);
    isb();

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Allow hyp mode (kernel) FPU accesses, then restore HCPTR afterwards. */
    word_t hcptr = getHCPTR();
    setHCPTR(hcptr & ~HCPTR_FPU_TRAPS);
#पूर्ण_अगर

    परिणाम = fpsimd_HWCapProbe();

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    setHCPTR(hcptr);
#पूर्ण_अगर

    /* Restore the CPACR value for native threads. Without CONFIG_HAVE_FPU this
     * disables CP10 and CP11 access again. */
    setCPACR(CPACR_DEFAULT);
    isb();

    वापस परिणाम;
}
