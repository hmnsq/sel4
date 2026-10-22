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
BOOT_کد ساكن مضمن bool_t supportsAsyncExceptions(فراغ)
{
    کلمه_t fpexc;

    /* Set FPEXC.EX=1 */
    VMRS(FPEXC, fpexc);
    fpexc |= بیت(FPEXC_EX_بیت);
    VMSR(FPEXC, fpexc);

    /* Read back the FPEXC register*/
    VMRS(FPEXC, fpexc);

    أرجع !!(fpexc & بیت(FPEXC_EX_بیت));
}

#اگر_تعریف_شده CONFIG_HAVE_FPU
/* This variable is set at boot/init time to true if the FPU supports 32 registers (d0-d31).
 * otherwise it only supports 16 registers (d0-d15).
 * We cache this value in the following variable to avoid reading the coprocessor
 * on every FPU context switch, since it shouldn't change for one platform on run-time.
 */
bool_t isFPUD32SupportedCached;

BOOT_کد ساكن مضمن bool_t isFPUD32Supported(فراغ)
{
    کلمه_t mvfr0;
    asm متطاير(".word 0xeef73a10 \n"   /* vmrs    r3, mvfr0 */
                 "mov %0, r3       \n"
                 : "=r"(mvfr0)
                 :
                 : "r3");
    أرجع ((mvfr0 & 0xf) == 2);
}

/* Initialise the FP/SIMD for this machine. */
BOOT_کد bool_t fpsimd_init(فراغ)
{
    /* We are inside CONFIG_HAVE_FPU, so CPACR_DEFAULT enables CP10 and CP11 access. */
    مجموعه_CPACR(CPACR_DEFAULT);
    isb();

    إذا (config_مجموعه(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* Allow hyp mode FPU accesses for the rest of the initialisation. */
        enable_Fpu_Inst_درون_Hyp();
    }

    إذا (supportsAsyncExceptions()) {
        /* In the future, when we've targets that support asynchronous FPU exceptions, we've to support them */
        چاپ_کن("Error: seL4 doesn't support FPU subarchitectures that support asynchronous exceptions\n");
        أرجع false;
    }

    isFPUD32SupportedCached = isFPUD32Supported();
    إذا (config_مجموعه(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* FPEXC is VCPU state and native threads always have EN set. Setting EN
         * must happen before disableFpu() traps hyp mode (kernel) FPU accesses
         * again. */
        مجموعه_En_FPEXC();
    }
    /* Set the FPU to lazy switch mode */
    disableFpu();

    أرجع true;
}
#پایان_اگر /* CONFIG_HAVE_FPU */

/* Probe the FPU hardware. Requires CP10 and CP11 access in CPACR and, in hyp
 * mode, no CP10 and CP11 traps in HCPTR. */
BOOT_کد ساكن bool_t fpsimd_HWCapProbe(فراغ)
{
    کلمه_t fpsid;

    /* Check of this platform supports HW FP instructions */
    asm متطاير(".word 0xeef00a10  \n"  /* vmrs    r0, fpsid */
                 "mov %0, r0        \n"
                 : "=r"(fpsid) :
                 : "r0");
    إذا (fpsid & بیت(FPSID_SW_بیت)) {
        أرجع false;
    }

    کلمه_t fpsid_subarch;

    إذا (supportsAsyncExceptions()) {
        /* In the future, when we've targets that support asynchronous FPU exceptions, we've to support them */
        إذا (config_مجموعه(CONFIG_HAVE_FPU)) {
            چاپ_کن("Error: seL4 doesn't support FPU subarchitectures that support asynchronous exceptions\n");
            أرجع false;
        } وإلا {
            // if we aren't using the fpu then we have detected an fpu that we cannot use, but that is fine
            أرجع true;
        }
    }
    /* Check for subarchitectures we support */
    fpsid_subarch = (fpsid >> FPSID_SUBARCH_لغزش_POS) & 0x7f;

    بدل (fpsid_subarch) {
    /* We only support the following subarch values */
    حالة 0x2:
    حالة 0x3:
    حالة 0x4:
        اكسر;
    افتراضي: {
        إذا (config_مجموعه(CONFIG_HAVE_FPU)) {
            چاپ_کن("Error: seL4 doesn't support this VFP subarchitecture\n");
            أرجع false;
        } وإلا {
            // if we aren't using the fpu then we have detected an fpu that we cannot use, but that is fine
            أرجع true;
        }
    }

    }

    إذا (!config_مجموعه(CONFIG_HAVE_FPU)) {
        چاپ_کن("Info: Not using supported FPU as FPU is disabled in the build configuration\n");
    }
    أرجع true;
}

BOOT_کد bool_t fpsimd_HWCapTest(فراغ)
{
    bool_t نتیجه;

    /* Enable CP10 and CP11 access to read the FPU control/status registers. */
    مجموعه_CPACR(CPACR_FPU_ENABLE);
    isb();

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Allow hyp mode (kernel) FPU accesses, then restore HCPTR afterwards. */
    کلمه_t hcptr = getHCPTR();
    مجموعه_HCPTR(hcptr & ~HCPTR_FPU_TRAPS);
#پایان_اگر

    نتیجه = fpsimd_HWCapProbe();

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    مجموعه_HCPTR(hcptr);
#پایان_اگر

    /* Restore the CPACR value for native threads. Without CONFIG_HAVE_FPU this
     * disables CP10 and CP11 access again. */
    مجموعه_CPACR(CPACR_DEFAULT);
    isb();

    أرجع نتیجه;
}
