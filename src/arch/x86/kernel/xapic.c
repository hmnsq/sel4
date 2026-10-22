/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <arch/kernel/xapic.h>
#شامل <arch/kernel/x2apic.h>

#اگر_تعریف_شدہ CONFIG_XAPIC
#اگر_تعریف_شدہ CONFIG_USE_LOGICAL_IDS
/* using flat cluster mode we only support 8 cores */
compile_assert(نمبر_کا_cores_invalid_for_logical_ids, CONFIG_MAX_عدد_NODES <= 8)

BOOT_رمز ساکن خالی
init_xapic_ldr(خالی)
{
    uint32_t ldr;

    apic_write_reg(APIC_DEST_FORMAT, XAPIC_DFR_FLAT);
    ldr = apic_read_reg(APIC_LOGICAL_DEST) & نقاب(XAPIC_LDR_سرکاؤ);
    ldr |= (ثنائی_ہندسہ(getCurrentCPUIndex()) << XAPIC_LDR_سرکاؤ);
    apic_write_reg(APIC_LOGICAL_DEST, ldr);
}
#اگر_ختم /* CONFIG_USE_LOGICAL_IDS */

BOOT_رمز bool_t apic_enable(خالی)
{
    apic_بنیاد_msr_t apic_بنیاد_msr;
    apic_بنیاد_msr.الفاظ[0] = x86_rdmsr_کم(IA32_APIC_بنیاد_MSR);

    اگر (!apic_بنیاد_msr_get_enabled(apic_بنیاد_msr)) {
        لکھو("APIC: Enabled bit not set\n");
        واپس false;
    }

    اگر (x2apic_is_enabled()) {
        لکھو("x2APIC enabled in BIOS but kernel does not support that\n");
        واپس false;
    }

#اگر_تعریف_شدہ CONFIG_USE_LOGICAL_IDS
    init_xapic_ldr();
#اگر_ختم /* CONFIG_USE_LOGICAL_IDS */

    واپس true;
}

bool_t apic_is_مداخلت_pending(خالی)
{
    لفظ_t i;

    /* read 256-bit register: each 32-bit word is 16 byte aligned */
    أكد(int_irq_min % 32 == 0);
    برائے (i = int_irq_min; i <= int_irq_max; i += 32) {
        اگر (apic_read_reg(APIC_IRR_بنیاد + i / 2) != 0) {
            واپس true;
        }
    }
    واپس false;
}

BOOT_رمز خالی apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).الفاظ[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*سمتیہ*/
        ).الفاظ[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).الفاظ[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*سمتیہ*/
        ).الفاظ[0]
    );
}

BOOT_رمز خالی apic_send_startup_ipi(cpu_id_t cpu_id, paddr_t startup_addr)
{
    /* check if 4K aligned */
    أكد(IS_ALIGNED(startup_addr, صفحہ_بٹ));
    /* check if startup_addr < 640K */
    أكد(startup_addr < 0xa0000);
    startup_addr >>= صفحہ_بٹ;

    apic_write_icr(
        apic_icr2_new(
            cpu_id       /* dest */
        ).الفاظ[0],
        apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* delivery_status */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /*سمتیہ*/
        ).الفاظ[0]
    );
}

خالی apic_send_ipi_core(irq_t سمتیہ, cpu_id_t cpu_id)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    کرو {
        icr1.الفاظ[0] = apic_read_reg(APIC_ICR1);
    } جب (apic_icr1_get_delivery_حالت(icr1));

    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).الفاظ[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            سمتیہ      /*سمتیہ*/
        ).الفاظ[0]
    );
}

خالی apic_send_ipi_cluster(irq_t سمتیہ, لفظ_t mda)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    کرو {
        icr1.الفاظ[0] = apic_read_reg(APIC_ICR1);
    } جب (apic_icr1_get_delivery_حالت(icr1));

    apic_write_icr(
        apic_icr2_new(
            mda         /* message destination address */
        ).الفاظ[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            سمتیہ      /*سمتیہ*/
        ).الفاظ[0]
    );
}
#اگر_ختم /* CONFIG_XAPIC */
