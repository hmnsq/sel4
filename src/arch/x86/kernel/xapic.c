/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <arch/kernel/xapic.h>
#ضمن <arch/kernel/x2apic.h>

#که_تعریف_شوی CONFIG_XAPIC
#که_تعریف_شوی CONFIG_USE_LOGICAL_IDS
/* using flat cluster mode we only support 8 cores */
compile_assert(شمېره_د_cores_invalid_for_logical_ids, CONFIG_MAX_عدد_NODES <= 8)

BOOT_کوډ ساكن فراغ
init_xapic_ldr(فراغ)
{
    uint32_t ldr;

    apic_write_reg(APIC_DEST_FORMAT, XAPIC_DFR_FLAT);
    ldr = apic_read_reg(APIC_LOGICAL_DEST) & ماسک(XAPIC_LDR_ښويدنه);
    ldr |= (بټ(getCurrentCPUIndex()) << XAPIC_LDR_ښويدنه);
    apic_write_reg(APIC_LOGICAL_DEST, ldr);
}
#د_که_پای /* CONFIG_USE_LOGICAL_IDS */

BOOT_کوډ bool_t apic_enable(فراغ)
{
    apic_بنسټ_msr_t apic_بنسټ_msr;
    apic_بنسټ_msr.ټکي[0] = x86_rdmsr_ټیټ(IA32_APIC_بنسټ_MSR);

    إذا (!apic_بنسټ_msr_get_enabled(apic_بنسټ_msr)) {
        چاپ_کړه("APIC: Enabled bit not set\n");
        أرجع false;
    }

    إذا (x2apic_is_enabled()) {
        چاپ_کړه("x2APIC enabled in BIOS but kernel does not support that\n");
        أرجع false;
    }

#که_تعریف_شوی CONFIG_USE_LOGICAL_IDS
    init_xapic_ldr();
#د_که_پای /* CONFIG_USE_LOGICAL_IDS */

    أرجع true;
}

bool_t apic_is_مداخله_pending(فراغ)
{
    کلمه_t i;

    /* read 256-bit register: each 32-bit word is 16 byte aligned */
    أكد(int_irq_min % 32 == 0);
    لكل (i = int_irq_min; i <= int_irq_max; i += 32) {
        إذا (apic_read_reg(APIC_IRR_بنسټ + i / 2) != 0) {
            أرجع true;
        }
    }
    أرجع false;
}

BOOT_کوډ فراغ apic_send_init_ipi(cpu_id_t cpu_id)
{
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).ټکي[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            1,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*ویکټور*/
        ).ټکي[0]
    );
    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).ټکي[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            1,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            5,          /* delivery_mode   */
            0           /*ویکټور*/
        ).ټکي[0]
    );
}

BOOT_کوډ فراغ apic_send_startup_ipi(cpu_id_t cpu_id, paddr_t startup_addr)
{
    /* check if 4K aligned */
    أكد(IS_ALIGNED(startup_addr, پاڼه_بټونه));
    /* check if startup_addr < 640K */
    أكد(startup_addr < 0xa0000);
    startup_addr >>= پاڼه_بټونه;

    apic_write_icr(
        apic_icr2_new(
            cpu_id       /* dest */
        ).ټکي[0],
        apic_icr1_new(
            0,           /* dest_shorthand  */
            0,           /* trigger_mode    */
            0,           /* level           */
            0,           /* delivery_status */
            0,           /* dest_mode       */
            6,           /* delivery_mode   */
            startup_addr /*ویکټور*/
        ).ټکي[0]
    );
}

فراغ apic_send_ipi_core(irq_t ویکټور, cpu_id_t cpu_id)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    افعل {
        icr1.ټکي[0] = apic_read_reg(APIC_ICR1);
    } طالما (apic_icr1_get_delivery_حالت(icr1));

    apic_write_icr(
        apic_icr2_new(
            cpu_id      /* dest */
        ).ټکي[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            0,          /* dest_mode       */
            0,          /* delivery_mode   */
            ویکټور      /*ویکټور*/
        ).ټکي[0]
    );
}

فراغ apic_send_ipi_cluster(irq_t ویکټور, کلمه_t mda)
{
    apic_icr1_t icr1;
    /* wait till we can send an IPI */
    افعل {
        icr1.ټکي[0] = apic_read_reg(APIC_ICR1);
    } طالما (apic_icr1_get_delivery_حالت(icr1));

    apic_write_icr(
        apic_icr2_new(
            mda         /* message destination address */
        ).ټکي[0],
        apic_icr1_new(
            0,          /* dest_shorthand  */
            0,          /* trigger_mode    */
            0,          /* level           */
            0,          /* delivery_status */
            1,          /* dest_mode       */
            0,          /* delivery_mode   */
            ویکټور      /*ویکټور*/
        ).ټکي[0]
    );
}
#د_که_پای /* CONFIG_XAPIC */
