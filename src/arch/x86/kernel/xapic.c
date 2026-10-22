/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/xapic.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/x2apic.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_XAPIC
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_USE_LOGICAL_IDS
/* using flat cluster mode we only support 8 cores */
compile_assert(ಸಂಖ್ಯೆ_ದ_cores_invalid_for_logical_ids, CONFIG_MAX_ಅಂಕೆ_NODES <= 8)

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ
init_xapic_ldr(ಶೂನ್ಯ)
{
    uint32_t ldr;

    apic_write_reg(APIC_DEST_FORMAT, XAPIC_DFR_FLAT);
    ldr = apic_read_reg(APIC_LOGICAL_DEST) & ಮುಖವಾಡ(XAPIC_LDR_ಸರಿಸು);
    ldr |= (ಬಿಟ್(getCurrentCPUIndex()) << XAPIC_LDR_ಸರಿಸು);
    apic_write_reg(APIC_LOGICAL_DEST, ldr);
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_USE_LOGICAL_IDS */

BOOT_ಸಂಕೇತ bool_t apic_enable(ಶೂನ್ಯ)
{
    apic_ತಳ_msr_t apic_ತಳ_msr;
    apic_ತಳ_msr.ಪದಗಳು[0] = x86_rdmsr_ಕೆಳಗಿನ(IA32_APIC_ತಳ_MSR);

    ಆದರೆ (!apic_ತಳ_msr_get_enabled(apic_ತಳ_msr)) {
        ಬರೆ("APIC: Enabled bit not set\n");
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (x2apic_is_enabled()) {
        ಬರೆ("x2APIC enabled in BIOS but kernel does not support that\n");
        ಹಿಂತಿರುಗಿ false;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_USE_LOGICAL_IDS
    init_xapic_ldr();
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_USE_LOGICAL_IDS */

    ಹಿಂತಿರುಗಿ true;
}

bool_t apic_is_ಅಡಚಣೆ_pending(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t i;

    /* read 256-bit register: each 32-bit word is 16 byte aligned */
    ಪ್ರತಿಜ್ಞಾ(int_irq_min % 32 == 0);
    ಗಾಗಿ (i = int_irq_min; i <= int_irq_max; i += 32) {
        ಆದರೆ (apic_read_reg(APIC_IRR_ತಳ + i / 2) != 0) {
            ಹಿಂತಿರುಗಿ true;
        }
    }
    ಹಿಂತಿರುಗಿ false;
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).ಪದಗಳು[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*ಸದಿಶ*/
        ).ಪದಗಳು[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).ಪದಗಳು[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
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
        apic_icr2_new(
            cpu_id       /* dest */
        ).ಪದಗಳು[0],
        apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* delivery_status */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /*ಸದಿಶ*/
        ).ಪದಗಳು[0]
    );
}

ಶೂನ್ಯ apic_send_ipi_core(irq_t ಸದಿಶ, cpu_id_t cpu_id)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    ಮಾಡು {
        icr1.ಪದಗಳು[0] = apic_read_reg(APIC_ICR1);
    } ಆಗ (apic_icr1_get_delivery_ಸ್ಥಿತಿ(icr1));

    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).ಪದಗಳು[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            ಸದಿಶ      /*ಸದಿಶ*/
        ).ಪದಗಳು[0]
    );
}

ಶೂನ್ಯ apic_send_ipi_cluster(irq_t ಸದಿಶ, ಶಬ್ದ_t mda)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    ಮಾಡು {
        icr1.ಪದಗಳು[0] = apic_read_reg(APIC_ICR1);
    } ಆಗ (apic_icr1_get_delivery_ಸ್ಥಿತಿ(icr1));

    apic_write_icr(
        apic_icr2_new(
            mda         /* message destination address */
        ).ಪದಗಳು[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            ಸದಿಶ      /*ಸದಿಶ*/
        ).ಪದಗಳು[0]
    );
}
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_XAPIC */
