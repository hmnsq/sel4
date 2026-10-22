/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <mode/machine.h>
#ضمن <arch/machine/fpu.h>
#ضمن <mode/model/statedata.h>
#ضمن <config.h>
#ضمن <util.h>

/*
 * The following function checks if the subarchitecture support asynchronous exceptions
 */
BOOT_CODE ساكن مضمن bool_t supportsAsyncExceptions(فراغ)
{
    word_t fpexc;

    /* Set FPEXC.EX=1 */
    VMRS(FPEXC, fpexc);
    fpexc |= BIT(FPEXC_EX_BIT);
    VMSR(FPEXC, fpexc);

    /* Read back the FPEXC register*/
    VMRS(FPEXC, fpexc);

    أرجع !!(fpexc & BIT(FPEXC_EX_BIT));
}

#ifdef CONFIG_HAVE_FPU
/* This variable is set at boot/init time to true if the FPU supports 32 registers (d0-d31).
 * otherwise it only supports 16 registers (d0-d15).
 * We cache this value in the following variable to avoid reading the coprocessor
 * on every FPU context switch, since it shouldn't change for one platform on run-time.
 */
bool_t isFPUD32SupportedCached;

BOOT_CODE ساكن مضمن bool_t isFPUD32Supported(فراغ)
{
    word_t mvfr0;
    asm متطاير(".word 0xeef73a10 \n"   /* vmrs    r3, mvfr0 */
                 "mov %0, r3       \n"
                 : "=r"(mvfr0)
                 :
                 : "r3");
    أرجع ((mvfr0 & 0xf) == 2);
}

/* Initialise the FP/SIMD for this machine. */
BOOT_CODE bool_t fpsimd_init(فراغ)
{
    /* We are inside CONFIG_HAVE_FPU, so CPACR_DEFAULT enables CP10 and CP11 access. */
    setCPACR(CPACR_DEFAULT);
    isb();

    𐤀𐤌 (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* Allow hyp mode FPU accesses for the rest of the initialisation. */
        enableFpuInstInHyp();
    }

    𐤀𐤌 (supportsAsyncExceptions()) {
        /* In the future, when we've targets that support asynchronous FPU exceptions, we've to support them */
        printf("Error: seL4 doesn't support FPU subarchitectures that support asynchronous exceptions\n");
        أرجع false;
    }

    isFPUD32SupportedCached = isFPUD32Supported();
    𐤀𐤌 (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* FPEXC is VCPU state and native threads always have EN set. Setting EN
         * must happen before disableFpu() traps hyp mode (kernel) FPU accesses
         * again. */
        setEnFPEXC();
    }
    /* Set the FPU to lazy switch mode */
    disableFpu();

    أرجع true;
}
#endif /* CONFIG_HAVE_FPU */

/* Probe the FPU hardware. Requires CP10 and CP11 access in CPACR and, in hyp
 * mode, no CP10 and CP11 traps in HCPTR. */
BOOT_CODE ساكن bool_t fpsimd_HWCapProbe(فراغ)
{
    word_t fpsid;

    /* Check of this platform supports HW FP instructions */
    asm متطاير(".word 0xeef00a10  \n"  /* vmrs    r0, fpsid */
                 "mov %0, r0        \n"
                 : "=r"(fpsid) :
                 : "r0");
    𐤀𐤌 (fpsid & BIT(FPSID_SW_BIT)) {
        أرجع false;
    }

    word_t fpsid_subarch;

    𐤀𐤌 (supportsAsyncExceptions()) {
        /* In the future, when we've targets that support asynchronous FPU exceptions, we've to support them */
        𐤀𐤌 (config_set(CONFIG_HAVE_FPU)) {
            printf("Error: seL4 doesn't support FPU subarchitectures that support asynchronous exceptions\n");
            أرجع false;
        } وإلا {
            // if we aren't using the fpu then we have detected an fpu that we cannot use, but that is fine
            أرجع true;
        }
    }
    /* Check for subarchitectures we support */
    fpsid_subarch = (fpsid >> FPSID_SUBARCH_SHIFT_POS) & 0x7f;

    بدل (fpsid_subarch) {
    /* We only support the following subarch values */
    حالة 0x2:
    حالة 0x3:
    حالة 0x4:
        اكسر;
    افتراضي: {
        𐤀𐤌 (config_set(CONFIG_HAVE_FPU)) {
            printf("Error: seL4 doesn't support this VFP subarchitecture\n");
            أرجع false;
        } وإلا {
            // if we aren't using the fpu then we have detected an fpu that we cannot use, but that is fine
            أرجع true;
        }
    }

    }

    𐤀𐤌 (!config_set(CONFIG_HAVE_FPU)) {
        printf("Info: Not using supported FPU as FPU is disabled in the build configuration\n");
    }
    أرجع true;
}

BOOT_CODE bool_t fpsimd_HWCapTest(فراغ)
{
    bool_t result;

    /* Enable CP10 and CP11 access to read the FPU control/status registers. */
    setCPACR(CPACR_FPU_ENABLE);
    isb();

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Allow hyp mode (kernel) FPU accesses, then restore HCPTR afterwards. */
    word_t hcptr = getHCPTR();
    setHCPTR(hcptr & ~HCPTR_FPU_TRAPS);
#endif

    result = fpsimd_HWCapProbe();

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    setHCPTR(hcptr);
#endif

    /* Restore the CPACR value for native threads. Without CONFIG_HAVE_FPU this
     * disables CP10 and CP11 access again. */
    setCPACR(CPACR_DEFAULT);
    isb();

    أرجع result;
}
