/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <mode/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/fpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>

/*
 * The following function checks if the subarchitecture support asynchronous exceptions
 */
BOOT_ਕੋਡ ਸਥਿਰ ਸਤਰਅੰਦਰ bool_t supportsAsyncExceptions(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t fpexc;

    /* Set FPEXC.EX=1 */
    VMRS(FPEXC, fpexc);
    fpexc |= ਦੋਆਧਾਰੀ_ਅੰਕ(FPEXC_EX_ਦੋਆਧਾਰੀ_ਅੰਕ);
    VMSR(FPEXC, fpexc);

    /* Read back the FPEXC register*/
    VMRS(FPEXC, fpexc);

    ਵਾਪਸ !!(fpexc & ਦੋਆਧਾਰੀ_ਅੰਕ(FPEXC_EX_ਦੋਆਧਾਰੀ_ਅੰਕ));
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
/* This variable is set at boot/init time to true if the FPU supports 32 registers (d0-d31).
 * otherwise it only supports 16 registers (d0-d15).
 * We cache this value in the following variable to avoid reading the coprocessor
 * on every FPU context switch, since it shouldn't change for one platform on run-time.
 */
bool_t isFPUD32SupportedCached;

BOOT_ਕੋਡ ਸਥਿਰ ਸਤਰਅੰਦਰ bool_t isFPUD32Supported(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t mvfr0;
    asm ਅਸਥਿਰ(".word 0xeef73a10 \n"   /* vmrs    r3, mvfr0 */
                 "mov %0, r3       \n"
                 : "=r"(mvfr0)
                 :
                 : "r3");
    ਵਾਪਸ ((mvfr0 & 0xf) == 2);
}

/* Initialise the FP/SIMD for this machine. */
BOOT_ਕੋਡ bool_t fpsimd_init(ਖਾਲੀ)
{
    /* We are inside CONFIG_HAVE_FPU, so CPACR_DEFAULT enables CP10 and CP11 access. */
    ਸੈੱਟ_CPACR(CPACR_DEFAULT);
    isb();

    ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* Allow hyp mode FPU accesses for the rest of the initialisation. */
        enable_Fpu_Inst_ਵਿੱਚ_Hyp();
    }

    ਜੇ (supportsAsyncExceptions()) {
        /* In the future, when we've targets that support asynchronous FPU exceptions, we've to support them */
        ਲਿਖੋ("Error: seL4 doesn't support FPU subarchitectures that support asynchronous exceptions\n");
        ਵਾਪਸ false;
    }

    isFPUD32SupportedCached = isFPUD32Supported();
    ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* FPEXC is VCPU state and native threads always have EN set. Setting EN
         * must happen before disableFpu() traps hyp mode (kernel) FPU accesses
         * again. */
        ਸੈੱਟ_En_FPEXC();
    }
    /* Set the FPU to lazy switch mode */
    disableFpu();

    ਵਾਪਸ true;
}
#ਜੇ_ਅੰਤ /* CONFIG_HAVE_FPU */

/* Probe the FPU hardware. Requires CP10 and CP11 access in CPACR and, in hyp
 * mode, no CP10 and CP11 traps in HCPTR. */
BOOT_ਕੋਡ ਸਥਿਰ bool_t fpsimd_HWCapProbe(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t fpsid;

    /* Check of this platform supports HW FP instructions */
    asm ਅਸਥਿਰ(".word 0xeef00a10  \n"  /* vmrs    r0, fpsid */
                 "mov %0, r0        \n"
                 : "=r"(fpsid) :
                 : "r0");
    ਜੇ (fpsid & ਦੋਆਧਾਰੀ_ਅੰਕ(FPSID_SW_ਦੋਆਧਾਰੀ_ਅੰਕ)) {
        ਵਾਪਸ false;
    }

    ਸ਼ਬਦ_t fpsid_subarch;

    ਜੇ (supportsAsyncExceptions()) {
        /* In the future, when we've targets that support asynchronous FPU exceptions, we've to support them */
        ਜੇ (config_ਸੈੱਟ(CONFIG_HAVE_FPU)) {
            ਲਿਖੋ("Error: seL4 doesn't support FPU subarchitectures that support asynchronous exceptions\n");
            ਵਾਪਸ false;
        } ਨਹੀਂਤਾਂ {
            // if we aren't using the fpu then we have detected an fpu that we cannot use, but that is fine
            ਵਾਪਸ true;
        }
    }
    /* Check for subarchitectures we support */
    fpsid_subarch = (fpsid >> FPSID_SUBARCH_ਖਿਸਕਾਓ_POS) & 0x7f;

    ਚੋਣ (fpsid_subarch) {
    /* We only support the following subarch values */
    ਹਾਲਤ 0x2:
    ਹਾਲਤ 0x3:
    ਹਾਲਤ 0x4:
        ਰੋਕੋ;
    ਮੂਲ: {
        ਜੇ (config_ਸੈੱਟ(CONFIG_HAVE_FPU)) {
            ਲਿਖੋ("Error: seL4 doesn't support this VFP subarchitecture\n");
            ਵਾਪਸ false;
        } ਨਹੀਂਤਾਂ {
            // if we aren't using the fpu then we have detected an fpu that we cannot use, but that is fine
            ਵਾਪਸ true;
        }
    }

    }

    ਜੇ (!config_ਸੈੱਟ(CONFIG_HAVE_FPU)) {
        ਲਿਖੋ("Info: Not using supported FPU as FPU is disabled in the build configuration\n");
    }
    ਵਾਪਸ true;
}

BOOT_ਕੋਡ bool_t fpsimd_HWCapTest(ਖਾਲੀ)
{
    bool_t ਨਤੀਜਾ;

    /* Enable CP10 and CP11 access to read the FPU control/status registers. */
    ਸੈੱਟ_CPACR(CPACR_FPU_ENABLE);
    isb();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Allow hyp mode (kernel) FPU accesses, then restore HCPTR afterwards. */
    ਸ਼ਬਦ_t hcptr = getHCPTR();
    ਸੈੱਟ_HCPTR(hcptr & ~HCPTR_FPU_TRAPS);
#ਜੇ_ਅੰਤ

    ਨਤੀਜਾ = fpsimd_HWCapProbe();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਸੈੱਟ_HCPTR(hcptr);
#ਜੇ_ਅੰਤ

    /* Restore the CPACR value for native threads. Without CONFIG_HAVE_FPU this
     * disables CP10 and CP11 access again. */
    ਸੈੱਟ_CPACR(CPACR_DEFAULT);
    isb();

    ਵਾਪਸ ਨਤੀਜਾ;
}
