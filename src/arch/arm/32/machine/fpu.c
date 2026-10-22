/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <mode/machine.h>
#شامل <arch/machine/fpu.h>
#شامل <mode/model/statedata.h>
#شامل <config.h>
#شامل <util.h>

/*
 * The following function checks if the subarchitecture support asynchronous exceptions
 */
BOOT_رمز ساکن اندرونی bool_t supportsAsyncExceptions(خالی)
{
    لفظ_t fpexc;

    /* Set FPEXC.EX=1 */
    VMRS(FPEXC, fpexc);
    fpexc |= ثنائی_ہندسہ(FPEXC_EX_ثنائی_ہندسہ);
    VMSR(FPEXC, fpexc);

    /* Read back the FPEXC register*/
    VMRS(FPEXC, fpexc);

    واپس !!(fpexc & ثنائی_ہندسہ(FPEXC_EX_ثنائی_ہندسہ));
}

#اگر_تعریف_شدہ CONFIG_HAVE_FPU
/* This variable is set at boot/init time to true if the FPU supports 32 registers (d0-d31).
 * otherwise it only supports 16 registers (d0-d15).
 * We cache this value in the following variable to avoid reading the coprocessor
 * on every FPU context switch, since it shouldn't change for one platform on run-time.
 */
bool_t isFPUD32SupportedCached;

BOOT_رمز ساکن اندرونی bool_t isFPUD32Supported(خالی)
{
    لفظ_t mvfr0;
    asm متلون(".word 0xeef73a10 \n"   /* vmrs    r3, mvfr0 */
                 "mov %0, r3       \n"
                 : "=r"(mvfr0)
                 :
                 : "r3");
    واپس ((mvfr0 & 0xf) == 2);
}

/* Initialise the FP/SIMD for this machine. */
BOOT_رمز bool_t fpsimd_init(خالی)
{
    /* We are inside CONFIG_HAVE_FPU, so CPACR_DEFAULT enables CP10 and CP11 access. */
    سیٹ_CPACR(CPACR_DEFAULT);
    isb();

    اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* Allow hyp mode FPU accesses for the rest of the initialisation. */
        enable_Fpu_Inst_اندر_Hyp();
    }

    اگر (supportsAsyncExceptions()) {
        /* In the future, when we've targets that support asynchronous FPU exceptions, we've to support them */
        لکھو("Error: seL4 doesn't support FPU subarchitectures that support asynchronous exceptions\n");
        واپس false;
    }

    isFPUD32SupportedCached = isFPUD32Supported();
    اگر (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* FPEXC is VCPU state and native threads always have EN set. Setting EN
         * must happen before disableFpu() traps hyp mode (kernel) FPU accesses
         * again. */
        سیٹ_En_FPEXC();
    }
    /* Set the FPU to lazy switch mode */
    disableFpu();

    واپس true;
}
#اگر_ختم /* CONFIG_HAVE_FPU */

/* Probe the FPU hardware. Requires CP10 and CP11 access in CPACR and, in hyp
 * mode, no CP10 and CP11 traps in HCPTR. */
BOOT_رمز ساکن bool_t fpsimd_HWCapProbe(خالی)
{
    لفظ_t fpsid;

    /* Check of this platform supports HW FP instructions */
    asm متلون(".word 0xeef00a10  \n"  /* vmrs    r0, fpsid */
                 "mov %0, r0        \n"
                 : "=r"(fpsid) :
                 : "r0");
    اگر (fpsid & ثنائی_ہندسہ(FPSID_SW_ثنائی_ہندسہ)) {
        واپس false;
    }

    لفظ_t fpsid_subarch;

    اگر (supportsAsyncExceptions()) {
        /* In the future, when we've targets that support asynchronous FPU exceptions, we've to support them */
        اگر (config_سیٹ(CONFIG_HAVE_FPU)) {
            لکھو("Error: seL4 doesn't support FPU subarchitectures that support asynchronous exceptions\n");
            واپس false;
        } ورنہ {
            // if we aren't using the fpu then we have detected an fpu that we cannot use, but that is fine
            واپس true;
        }
    }
    /* Check for subarchitectures we support */
    fpsid_subarch = (fpsid >> FPSID_SUBARCH_سرکاؤ_POS) & 0x7f;

    انتخاب (fpsid_subarch) {
    /* We only support the following subarch values */
    حالت 0x2:
    حالت 0x3:
    حالت 0x4:
        توڑو;
    طےشدہ: {
        اگر (config_سیٹ(CONFIG_HAVE_FPU)) {
            لکھو("Error: seL4 doesn't support this VFP subarchitecture\n");
            واپس false;
        } ورنہ {
            // if we aren't using the fpu then we have detected an fpu that we cannot use, but that is fine
            واپس true;
        }
    }

    }

    اگر (!config_سیٹ(CONFIG_HAVE_FPU)) {
        لکھو("Info: Not using supported FPU as FPU is disabled in the build configuration\n");
    }
    واپس true;
}

BOOT_رمز bool_t fpsimd_HWCapTest(خالی)
{
    bool_t نتیجہ;

    /* Enable CP10 and CP11 access to read the FPU control/status registers. */
    سیٹ_CPACR(CPACR_FPU_ENABLE);
    isb();

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Allow hyp mode (kernel) FPU accesses, then restore HCPTR afterwards. */
    لفظ_t hcptr = getHCPTR();
    سیٹ_HCPTR(hcptr & ~HCPTR_FPU_TRAPS);
#اگر_ختم

    نتیجہ = fpsimd_HWCapProbe();

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    سیٹ_HCPTR(hcptr);
#اگر_ختم

    /* Restore the CPACR value for native threads. Without CONFIG_HAVE_FPU this
     * disables CP10 and CP11 access again. */
    سیٹ_CPACR(CPACR_DEFAULT);
    isb();

    واپس نتیجہ;
}
