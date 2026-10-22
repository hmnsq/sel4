/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <linker.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <plat/machine/hardware.h>
#అన్తర్భావ <plat/machine/pic.h>

/* PIC (i8259) base registers */
#నిర్వచించు PIC1_పునాది 0x20
#నిర్వచించు PIC2_పునాది 0xa0

/* Program PIC (i8259) to remap IRQs 0-15 to interrupt vectors starting at 'interrupt' */
BOOT_సంకేతం శూన్య pic_remap_irqs(అంతరాయం_t అంతరాయం)
{
    out8(PIC1_పునాది, 0x11);
    out8(PIC2_పునాది, 0x11);
    out8(PIC1_పునాది + 1, అంతరాయం);
    out8(PIC2_పునాది + 1, అంతరాయం + 8);
    out8(PIC1_పునాది + 1, 0x04);
    out8(PIC2_పునాది + 1, 0x02);
    out8(PIC1_పునాది + 1, 0x01);
    out8(PIC2_పునాది + 1, 0x01);
    out8(PIC1_పునాది + 1, 0x0);
    out8(PIC2_పునాది + 1, 0x0);
}

BOOT_సంకేతం శూన్య pic_disable(శూన్య)
{
    /* We assume that pic_remap_irqs has already been called and
     * just mask all the irqs */
    out8(PIC1_పునాది + 1, 0xff);
    out8(PIC2_పునాది + 1, 0xff);
}

శూన్య pic_ముసుగు_irq(bool_t ముసుగు_2, irq_t irq)
{
    uint8_t  బిట్టు_ముసుగు;
    uint16_t pic_పోర్టు;

    ప్రతిజ్ఞా(irq >= irq_isa_min);
    ప్రతిజ్ఞా(irq <= irq_isa_max);

    ఒకవేళ (irq < 8) {
        బిట్టు_ముసుగు = బిట్టు(irq);
        pic_పోర్టు = PIC1_పునాది + 1;
    } లేకపోతే {
        బిట్టు_ముసుగు = బిట్టు(irq - 8);
        pic_పోర్టు = PIC2_పునాది + 1;
    }

    ఒకవేళ (ముసుగు_2) {
        /* Disables the interrupt */
        out8(pic_పోర్టు, (in8(pic_పోర్టు) | బిట్టు_ముసుగు));
    } లేకపోతే {
        /* Enables the interrupt */
        out8(pic_పోర్టు, (in8(pic_పోర్టు) & ~బిట్టు_ముసుగు));
    }
}

bool_t pic_is_irq_pending(శూన్య)
{
    /* Interrupt Request Register (IRR) - holds pending IRQs */
    uint8_t irr;

    /* Send to PIC1's OCW3, in order to read IRR from next inb instruction */
    out8(PIC1_పునాది, 0x0a);

    /* Read IRR */
    irr = in8(PIC1_పునాది);

    /* Since slave PIC is connected to IRQ2 of master PIC,
     * there is no need to check IRR of slave PIC.
     */
    తిరిగి irr != 0;
}

స్థిర uint16_t pic_get_isr(శూన్య)
{
    out8(PIC1_పునాది, 0x0b);
    out8(PIC2_పునాది, 0x0b);
    తిరిగి (((uint16_t)in8(PIC2_పునాది)) << 8) | in8(PIC1_పునాది);
}

శూన్య pic_ack_చురుకైన_irq(శూన్య)
{
    irq_t irq = get_చురుకైన_IRQ();
    ఒకవేళ (irq >= irq_isa_min + 8) {
        /* ack slave PIC, unless we got a spurious irq 15
         * It is spurious if the bit is not set in the ISR
         * Even if it was spurious we will still need to
         * acknowledge the master PIC */
        ఒకవేళ (irq != irq_isa_min + 15 || (pic_get_isr() & బిట్టు(15))) {
            out8(PIC2_పునాది, 0x20);
        }
    }
    /* ack master PIC, unless we got a spurious IRQ 7
     * It is spurious if the bit is not set in the ISR */
    ఒకవేళ (irq != irq_isa_min + 7 || (pic_get_isr() & బిట్టు(7))) {
        out8(PIC1_పునాది, 0x20);
    }
}
