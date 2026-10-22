/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/xapic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/x2apic.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_XAPIC
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_USE_LOGICAL_IDS
/* using flat cluster mode we only support 8 cores */
compile_assert(ਸੰਖਿਆ_ਦਾ_cores_invalid_for_logical_ids, CONFIG_MAX_ਅੰਕ_NODES <= 8)

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ
init_xapic_ldr(ਖਾਲੀ)
{
    uint32_t ldr;

    apic_write_reg(APIC_DEST_FORMAT, XAPIC_DFR_FLAT);
    ldr = apic_read_reg(APIC_LOGICAL_DEST) & ਨਕਾਬ(XAPIC_LDR_ਖਿਸਕਾਓ);
    ldr |= (ਦੋਆਧਾਰੀ_ਅੰਕ(getCurrentCPUIndex()) << XAPIC_LDR_ਖਿਸਕਾਓ);
    apic_write_reg(APIC_LOGICAL_DEST, ldr);
}
#ਜੇ_ਅੰਤ /* CONFIG_USE_LOGICAL_IDS */

BOOT_ਕੋਡ bool_t apic_enable(ਖਾਲੀ)
{
    apic_ਅਧਾਰ_msr_t apic_ਅਧਾਰ_msr;
    apic_ਅਧਾਰ_msr.ਸ਼ਬਦ[0] = x86_rdmsr_ਨੀਵਾਂ(IA32_APIC_ਅਧਾਰ_MSR);

    ਜੇ (!apic_ਅਧਾਰ_msr_get_enabled(apic_ਅਧਾਰ_msr)) {
        ਲਿਖੋ("APIC: Enabled bit not set\n");
        ਵਾਪਸ false;
    }

    ਜੇ (x2apic_is_enabled()) {
        ਲਿਖੋ("x2APIC enabled in BIOS but kernel does not support that\n");
        ਵਾਪਸ false;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_USE_LOGICAL_IDS
    init_xapic_ldr();
#ਜੇ_ਅੰਤ /* CONFIG_USE_LOGICAL_IDS */

    ਵਾਪਸ true;
}

bool_t apic_is_ਰੁਕਾਵਟ_pending(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t i;

    /* read 256-bit register: each 32-bit word is 16 byte aligned */
    ਪ੍ਰਤਿਜ੍ਞਾ(int_irq_min % 32 == 0);
    ਲਈ (i = int_irq_min; i <= int_irq_max; i += 32) {
        ਜੇ (apic_read_reg(APIC_IRR_ਅਧਾਰ + i / 2) != 0) {
            ਵਾਪਸ true;
        }
    }
    ਵਾਪਸ false;
}

BOOT_ਕੋਡ ਖਾਲੀ apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).ਸ਼ਬਦ[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*ਸਦਿਸ਼*/
        ).ਸ਼ਬਦ[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).ਸ਼ਬਦ[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*ਸਦਿਸ਼*/
        ).ਸ਼ਬਦ[0]
    );
}

BOOT_ਕੋਡ ਖਾਲੀ apic_send_startup_ipi(cpu_id_t cpu_id, paddr_t startup_addr)
{
    /* check if 4K aligned */
    ਪ੍ਰਤਿਜ੍ਞਾ(IS_ALIGNED(startup_addr, ਪੰਨਾ_ਬਿੱਟ));
    /* check if startup_addr < 640K */
    ਪ੍ਰਤਿਜ੍ਞਾ(startup_addr < 0xa0000);
    startup_addr >>= ਪੰਨਾ_ਬਿੱਟ;

    apic_write_icr(
        apic_icr2_new(
            cpu_id       /* dest */
        ).ਸ਼ਬਦ[0],
        apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* delivery_status */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /*ਸਦਿਸ਼*/
        ).ਸ਼ਬਦ[0]
    );
}

ਖਾਲੀ apic_send_ipi_core(irq_t ਸਦਿਸ਼, cpu_id_t cpu_id)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    ਕਰੋ {
        icr1.ਸ਼ਬਦ[0] = apic_read_reg(APIC_ICR1);
    } ਜਦੋਂ (apic_icr1_get_delivery_ਸਥਿਤੀ(icr1));

    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).ਸ਼ਬਦ[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            ਸਦਿਸ਼      /*ਸਦਿਸ਼*/
        ).ਸ਼ਬਦ[0]
    );
}

ਖਾਲੀ apic_send_ipi_cluster(irq_t ਸਦਿਸ਼, ਸ਼ਬਦ_t mda)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    ਕਰੋ {
        icr1.ਸ਼ਬਦ[0] = apic_read_reg(APIC_ICR1);
    } ਜਦੋਂ (apic_icr1_get_delivery_ਸਥਿਤੀ(icr1));

    apic_write_icr(
        apic_icr2_new(
            mda         /* message destination address */
        ).ਸ਼ਬਦ[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            ਸਦਿਸ਼      /*ਸਦਿਸ਼*/
        ).ਸ਼ਬਦ[0]
    );
}
#ਜੇ_ਅੰਤ /* CONFIG_XAPIC */
