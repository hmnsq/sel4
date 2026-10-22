/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/x2apic.h>

BOOT_ಸಂಕೇತ bool_t x2apic_is_enabled(ಶೂನ್ಯ)
{
    apic_ತಳ_msr_t apic_ತಳ_msr;
    apic_ತಳ_msr.ಪದಗಳು[0] = x86_rdmsr_ಕೆಳಗಿನ(IA32_APIC_ತಳ_MSR);

    ಆದರೆ ((x86_cpuid_ecx(1, 0) & ಬಿಟ್(21)) &&
        apic_ತಳ_msr_get_enabled(apic_ತಳ_msr) &&
        apic_ತಳ_msr_get_x2apic(apic_ತಳ_msr)) {
        ಹಿಂತಿರುಗಿ true;
    }
    ಹಿಂತಿರುಗಿ false;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_X2APIC
BOOT_ಸಂಕೇತ bool_t apic_enable(ಶೂನ್ಯ)
{
    apic_ತಳ_msr_t apic_ತಳ_msr;
    apic_ತಳ_msr.ಪದಗಳು[0] = x86_rdmsr_ಕೆಳಗಿನ(IA32_APIC_ತಳ_MSR);

    ಆದರೆ (!apic_ತಳ_msr_get_enabled(apic_ತಳ_msr)) {
        ಬರೆ("APIC: Enabled bit not set\n");
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (x86_cpuid_ecx(1, 0) & ಬಿಟ್(21)) {
        apic_ತಳ_msr = apic_ತಳ_msr_ಗಣ_x2apic(apic_ತಳ_msr, 1);
        x86_wrmsr(IA32_APIC_ತಳ_MSR, apic_ತಳ_msr.ಪದಗಳು[0]);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಬರೆ("APIC: x2APIC is not supported on this machine\n");
        ಹಿಂತಿರುಗಿ false;
    }

    ಹಿಂತಿರುಗಿ true;
}

bool_t apic_is_ಅಡಚಣೆ_pending(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t i;

    ಪ್ರತಿಜ್ಞಾ(int_irq_min % 32 == 0);
    ಗಾಗಿ (i = int_irq_min; i <= int_irq_max; i += 32) {
        ಆದರೆ (apic_read_reg(APIC_IRR_ತಳ + ((i / 32) - 1)) != 0) {
            ಹಿಂತಿರುಗಿ true;
        }
    }
    ಹಿಂತಿರುಗಿ false;
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).ಪದಗಳು[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*ಸದಿಶ*/
        ).ಪದಗಳು[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).ಪದಗಳು[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*ಸದಿಶ*/
        ).ಪದಗಳು[0]
    );
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ apic_send_startup_ipi(cpu_id_t cpu_id, paddr_t startup_addr)
{
    /* check if 4K aligned */
    ಪ್ರತಿಜ್ಞಾ(IS_ALIGNED(startup_addr, ಪುಟ_ಬಿಟ್ಟುಗಳು));
    /* check if startup_addr < 640K */
    ಪ್ರತಿಜ್ಞಾ(startup_addr < 0xa0000);
    startup_addr >>= ಪುಟ_ಬಿಟ್ಟುಗಳು;

    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).ಪದಗಳು[0],
        x2apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /*ಸದಿಶ*/
        ).ಪದಗಳು[0]
    );
}

ಶೂನ್ಯ apic_send_ipi_core(irq_t ಸದಿಶ, cpu_id_t cpu_id)
{
    apic_write_icr(
        x2apic_icr2_new(
            cpu_id      /* dest */
        ).ಪದಗಳು[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            ಸದಿಶ      /*ಸದಿಶ*/
        ).ಪದಗಳು[0]
    );
}

ಶೂನ್ಯ apic_send_ipi_cluster(irq_t ಸದಿಶ, ಶಬ್ದ_t mda)
{
    apic_write_icr(
        x2apic_icr2_new(
            mda         /* message destination address */
        ).ಪದಗಳು[0],
        x2apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            ಸದಿಶ      /*ಸದಿಶ*/
        ).ಪದಗಳು[0]
    );
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_X2APIC */
