/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <linker.h>
#شامل <machine/io.h>
#شامل <plat/machine/hardware.h>
#شامل <plat/machine/pic.h>

/* PIC (i8259) base registers */
#تعریف_کرو PIC1_بنیاد 0x20
#تعریف_کرو PIC2_بنیاد 0xa0

/* Program PIC (i8259) to remap IRQs 0-15 to interrupt vectors starting at 'interrupt' */
BOOT_رمز خالی pic_remap_irqs(مداخلت_t مداخلت)
{
    out8(PIC1_بنیاد, 0x11);
    out8(PIC2_بنیاد, 0x11);
    out8(PIC1_بنیاد + 1, مداخلت);
    out8(PIC2_بنیاد + 1, مداخلت + 8);
    out8(PIC1_بنیاد + 1, 0x04);
    out8(PIC2_بنیاد + 1, 0x02);
    out8(PIC1_بنیاد + 1, 0x01);
    out8(PIC2_بنیاد + 1, 0x01);
    out8(PIC1_بنیاد + 1, 0x0);
    out8(PIC2_بنیاد + 1, 0x0);
}

BOOT_رمز خالی pic_disable(خالی)
{
    /* We assume that pic_remap_irqs has already been called and
     * just mask all the irqs */
    out8(PIC1_بنیاد + 1, 0xff);
    out8(PIC2_بنیاد + 1, 0xff);
}

خالی pic_نقاب_irq(bool_t نقاب_2, irq_t irq)
{
    uint8_t  ثنائی_ہندسہ_نقاب;
    uint16_t pic_پورٹ;

    أكد(irq >= irq_isa_min);
    أكد(irq <= irq_isa_max);

    اگر (irq < 8) {
        ثنائی_ہندسہ_نقاب = ثنائی_ہندسہ(irq);
        pic_پورٹ = PIC1_بنیاد + 1;
    } ورنہ {
        ثنائی_ہندسہ_نقاب = ثنائی_ہندسہ(irq - 8);
        pic_پورٹ = PIC2_بنیاد + 1;
    }

    اگر (نقاب_2) {
        /* Disables the interrupt */
        out8(pic_پورٹ, (in8(pic_پورٹ) | ثنائی_ہندسہ_نقاب));
    } ورنہ {
        /* Enables the interrupt */
        out8(pic_پورٹ, (in8(pic_پورٹ) & ~ثنائی_ہندسہ_نقاب));
    }
}

bool_t pic_is_irq_pending(خالی)
{
    /* Interrupt Request Register (IRR) - holds pending IRQs */
    uint8_t irr;

    /* Send to PIC1's OCW3, in order to read IRR from next inb instruction */
    out8(PIC1_بنیاد, 0x0a);

    /* Read IRR */
    irr = in8(PIC1_بنیاد);

    /* Since slave PIC is connected to IRQ2 of master PIC,
     * there is no need to check IRR of slave PIC.
     */
    واپس irr != 0;
}

ساکن uint16_t pic_get_isr(خالی)
{
    out8(PIC1_بنیاد, 0x0b);
    out8(PIC2_بنیاد, 0x0b);
    واپس (((uint16_t)in8(PIC2_بنیاد)) << 8) | in8(PIC1_بنیاد);
}

خالی pic_ack_فعال_irq(خالی)
{
    irq_t irq = get_فعال_IRQ();
    اگر (irq >= irq_isa_min + 8) {
        /* ack slave PIC, unless we got a spurious irq 15
         * It is spurious if the bit is not set in the ISR
         * Even if it was spurious we will still need to
         * acknowledge the master PIC */
        اگر (irq != irq_isa_min + 15 || (pic_get_isr() & ثنائی_ہندسہ(15))) {
            out8(PIC2_بنیاد, 0x20);
        }
    }
    /* ack master PIC, unless we got a spurious IRQ 7
     * It is spurious if the bit is not set in the ISR */
    اگر (irq != irq_isa_min + 7 || (pic_get_isr() & ثنائی_ہندسہ(7))) {
        out8(PIC1_بنیاد, 0x20);
    }
}
