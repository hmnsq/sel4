/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <mode/machine.h>
#అన్తర్భావ <arch/machine/fpu.h>
#అన్తర్భావ <mode/model/statedata.h>
#అన్తర్భావ <config.h>
#అన్తర్భావ <util.h>

/*
 * The following function checks if the subarchitecture support asynchronous exceptions
 */
BOOT_సంకేతం స్థిర అంతఃవరుస bool_t supportsAsyncExceptions(శూన్య)
{
    మాట_t fpexc;

    /* Set FPEXC.EX=1 */
    VMRS(FPEXC, fpexc);
    fpexc |= బిట్టు(FPEXC_EX_బిట్టు);
    VMSR(FPEXC, fpexc);

    /* Read back the FPEXC register*/
    VMRS(FPEXC, fpexc);

    తిరిగి !!(fpexc & బిట్టు(FPEXC_EX_బిట్టు));
}

#నిర్వచితమైతే CONFIG_HAVE_FPU
/* This variable is set at boot/init time to true if the FPU supports 32 registers (d0-d31).
 * otherwise it only supports 16 registers (d0-d15).
 * We cache this value in the following variable to avoid reading the coprocessor
 * on every FPU context switch, since it shouldn't change for one platform on run-time.
 */
bool_t isFPUD32SupportedCached;

BOOT_సంకేతం స్థిర అంతఃవరుస bool_t isFPUD32Supported(శూన్య)
{
    మాట_t mvfr0;
    asm అస్థిర(".word 0xeef73a10 \n"   /* vmrs    r3, mvfr0 */
                 "mov %0, r3       \n"
                 : "=r"(mvfr0)
                 :
                 : "r3");
    తిరిగి ((mvfr0 & 0xf) == 2);
}

/* Initialise the FP/SIMD for this machine. */
BOOT_సంకేతం bool_t fpsimd_init(శూన్య)
{
    /* We are inside CONFIG_HAVE_FPU, so CPACR_DEFAULT enables CP10 and CP11 access. */
    సమితి_CPACR(CPACR_DEFAULT);
    isb();

    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* Allow hyp mode FPU accesses for the rest of the initialisation. */
        enable_Fpu_Inst_లోపల_Hyp();
    }

    ఒకవేళ (supportsAsyncExceptions()) {
        /* In the future, when we've targets that support asynchronous FPU exceptions, we've to support them */
        రాయి("Error: seL4 doesn't support FPU subarchitectures that support asynchronous exceptions\n");
        తిరిగి false;
    }

    isFPUD32SupportedCached = isFPUD32Supported();
    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
        /* FPEXC is VCPU state and native threads always have EN set. Setting EN
         * must happen before disableFpu() traps hyp mode (kernel) FPU accesses
         * again. */
        సమితి_En_FPEXC();
    }
    /* Set the FPU to lazy switch mode */
    disableFpu();

    తిరిగి true;
}
#అయితే_ముగింపు /* CONFIG_HAVE_FPU */

/* Probe the FPU hardware. Requires CP10 and CP11 access in CPACR and, in hyp
 * mode, no CP10 and CP11 traps in HCPTR. */
BOOT_సంకేతం స్థిర bool_t fpsimd_HWCapProbe(శూన్య)
{
    మాట_t fpsid;

    /* Check of this platform supports HW FP instructions */
    asm అస్థిర(".word 0xeef00a10  \n"  /* vmrs    r0, fpsid */
                 "mov %0, r0        \n"
                 : "=r"(fpsid) :
                 : "r0");
    ఒకవేళ (fpsid & బిట్టు(FPSID_SW_బిట్టు)) {
        తిరిగి false;
    }

    మాట_t fpsid_subarch;

    ఒకవేళ (supportsAsyncExceptions()) {
        /* In the future, when we've targets that support asynchronous FPU exceptions, we've to support them */
        ఒకవేళ (config_సమితి(CONFIG_HAVE_FPU)) {
            రాయి("Error: seL4 doesn't support FPU subarchitectures that support asynchronous exceptions\n");
            తిరిగి false;
        } లేకపోతే {
            // if we aren't using the fpu then we have detected an fpu that we cannot use, but that is fine
            తిరిగి true;
        }
    }
    /* Check for subarchitectures we support */
    fpsid_subarch = (fpsid >> FPSID_SUBARCH_జరుపు_POS) & 0x7f;

    ఎంపిక (fpsid_subarch) {
    /* We only support the following subarch values */
    స్థితి 0x2:
    స్థితి 0x3:
    స్థితి 0x4:
        ఆపు;
    అప్రమేయ: {
        ఒకవేళ (config_సమితి(CONFIG_HAVE_FPU)) {
            రాయి("Error: seL4 doesn't support this VFP subarchitecture\n");
            తిరిగి false;
        } లేకపోతే {
            // if we aren't using the fpu then we have detected an fpu that we cannot use, but that is fine
            తిరిగి true;
        }
    }

    }

    ఒకవేళ (!config_సమితి(CONFIG_HAVE_FPU)) {
        రాయి("Info: Not using supported FPU as FPU is disabled in the build configuration\n");
    }
    తిరిగి true;
}

BOOT_సంకేతం bool_t fpsimd_HWCapTest(శూన్య)
{
    bool_t ఫలితం;

    /* Enable CP10 and CP11 access to read the FPU control/status registers. */
    సమితి_CPACR(CPACR_FPU_ENABLE);
    isb();

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Allow hyp mode (kernel) FPU accesses, then restore HCPTR afterwards. */
    మాట_t hcptr = getHCPTR();
    సమితి_HCPTR(hcptr & ~HCPTR_FPU_TRAPS);
#అయితే_ముగింపు

    ఫలితం = fpsimd_HWCapProbe();

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    సమితి_HCPTR(hcptr);
#అయితే_ముగింపు

    /* Restore the CPACR value for native threads. Without CONFIG_HAVE_FPU this
     * disables CP10 and CP11 access again. */
    సమితి_CPACR(CPACR_DEFAULT);
    isb();

    తిరిగి ఫలితం;
}
