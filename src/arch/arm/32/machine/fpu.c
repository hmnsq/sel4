/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <mode/machine.h>
#अन्तर्भाव <arch/machine/fpu.h>
#अन्तर्भाव <mode/model/statedata.h>
#अन्तर्भाव <config.h>
#अन्तर्भाव <util.h>

/*
 * The following function checks if the subarchitecture support asynchronous exceptions
 */
BOOT_संहिता स्थिर अन्तःस्थ bool_t supportsAsyncExceptions(शून्य)
{
    word_t fpexc;

    /* Set FPEXC.EX=1 */
    VMRS(FPEXC, fpexc);
    fpexc |= BIT(FPEXC_EX_BIT);
    VMSR(FPEXC, fpexc);

    /* Read back the FPEXC register*/
    VMRS(FPEXC, fpexc);

    प्रत्यागम !!(fpexc & BIT(FPEXC_EX_BIT));
}

#यदि_परिभाषितम् CONFIG_HAVE_FPU
/* This variable is set at boot/init time to true if the FPU supports 32 registers (d0-d31).
 * otherwise it only supports 16 registers (d0-d15).
 * We cache this value in the following variable to avoid reading the coprocessor
 * on every FPU context switch, since it shouldn't change for one platform on run-time.
 */
bool_t isFPUD32SupportedCached;

BOOT_संहिता स्थिर अन्तःस्थ bool_t isFPUD32Supported(शून्य)
{
    word_t mvfr0;
    asm चञ्चल(".word 0xeef73a10 \n"   /* vmrs    r3, mvfr0 */
                 "mov %0, r3       \n"
                 : "=r"(mvfr0)
                 :
                 : "r3");
    प्रत्यागम ((mvfr0 & 0xf) == 2);
}

/* Initialise the FP/SIMD for this machine. */
BOOT_संहिता bool_t fpsimd_init(शून्य)
{
    /* We are inside CONFIG_HAVE_FPU, so CPACR_DEFAULT enables CP10 and CP11 access. */
    समुच्चयः_CPACR(CPACR_DEFAULT);
    isb();

    यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* Allow hyp mode FPU accesses for the rest of the initialisation. */
        enable_Fpu_Inst_अन्तर्_Hyp();
    }

    यदि (supportsAsyncExceptions()) {
        /* In the future, when we've targets that support asynchronous FPU exceptions, we've to support them */
        प्रारूप_लिख("Error: seL4 doesn't support FPU subarchitectures that support asynchronous exceptions\n");
        प्रत्यागम false;
    }

    isFPUD32SupportedCached = isFPUD32Supported();
    यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* FPEXC is VCPU state and native threads always have EN set. Setting EN
         * must happen before disableFpu() traps hyp mode (kernel) FPU accesses
         * again. */
        समुच्चयः_En_FPEXC();
    }
    /* Set the FPU to lazy switch mode */
    disableFpu();

    प्रत्यागम true;
}
#यदि_अन्तः /* CONFIG_HAVE_FPU */

/* Probe the FPU hardware. Requires CP10 and CP11 access in CPACR and, in hyp
 * mode, no CP10 and CP11 traps in HCPTR. */
BOOT_संहिता स्थिर bool_t fpsimd_HWCapProbe(शून्य)
{
    word_t fpsid;

    /* Check of this platform supports HW FP instructions */
    asm चञ्चल(".word 0xeef00a10  \n"  /* vmrs    r0, fpsid */
                 "mov %0, r0        \n"
                 : "=r"(fpsid) :
                 : "r0");
    यदि (fpsid & BIT(FPSID_SW_BIT)) {
        प्रत्यागम false;
    }

    word_t fpsid_subarch;

    यदि (supportsAsyncExceptions()) {
        /* In the future, when we've targets that support asynchronous FPU exceptions, we've to support them */
        यदि (config_समुच्चयः(CONFIG_HAVE_FPU)) {
            प्रारूप_लिख("Error: seL4 doesn't support FPU subarchitectures that support asynchronous exceptions\n");
            प्रत्यागम false;
        } अन्यथा {
            // if we aren't using the fpu then we have detected an fpu that we cannot use, but that is fine
            प्रत्यागम true;
        }
    }
    /* Check for subarchitectures we support */
    fpsid_subarch = (fpsid >> FPSID_SUBARCH_सरणम्_POS) & 0x7f;

    विकल्प (fpsid_subarch) {
    /* We only support the following subarch values */
    पक्ष 0x2:
    पक्ष 0x3:
    पक्ष 0x4:
        भङ्ग;
    स्वभाव: {
        यदि (config_समुच्चयः(CONFIG_HAVE_FPU)) {
            प्रारूप_लिख("Error: seL4 doesn't support this VFP subarchitecture\n");
            प्रत्यागम false;
        } अन्यथा {
            // if we aren't using the fpu then we have detected an fpu that we cannot use, but that is fine
            प्रत्यागम true;
        }
    }

    }

    यदि (!config_समुच्चयः(CONFIG_HAVE_FPU)) {
        प्रारूप_लिख("Info: Not using supported FPU as FPU is disabled in the build configuration\n");
    }
    प्रत्यागम true;
}

BOOT_संहिता bool_t fpsimd_HWCapTest(शून्य)
{
    bool_t परिणाम;

    /* Enable CP10 and CP11 access to read the FPU control/status registers. */
    समुच्चयः_CPACR(CPACR_FPU_ENABLE);
    isb();

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Allow hyp mode (kernel) FPU accesses, then restore HCPTR afterwards. */
    word_t hcptr = getHCPTR();
    समुच्चयः_HCPTR(hcptr & ~HCPTR_FPU_TRAPS);
#यदि_अन्तः

    परिणाम = fpsimd_HWCapProbe();

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    समुच्चयः_HCPTR(hcptr);
#यदि_अन्तः

    /* Restore the CPACR value for native threads. Without CONFIG_HAVE_FPU this
     * disables CP10 and CP11 access again. */
    समुच्चयः_CPACR(CPACR_DEFAULT);
    isb();

    प्रत्यागम परिणाम;
}
