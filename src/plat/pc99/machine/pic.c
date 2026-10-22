/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <linker.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <plat/machine/hardware.h>
#અન્તર્ભાવ <plat/machine/pic.h>

/* PIC (i8259) base registers */
#define PIC1_BASE 0x20
#define PIC2_BASE 0xa0

/* Program PIC (i8259) to remap IRQs 0-15 to interrupt vectors starting at 'interrupt' */
BOOT_CODE શૂન્ય pic_remap_irqs(interrupt_t interrupt)
{
    out8(PIC1_BASE, 0x11);
    out8(PIC2_BASE, 0x11);
    out8(PIC1_BASE + 1, interrupt);
    out8(PIC2_BASE + 1, interrupt + 8);
    out8(PIC1_BASE + 1, 0x04);
    out8(PIC2_BASE + 1, 0x02);
    out8(PIC1_BASE + 1, 0x01);
    out8(PIC2_BASE + 1, 0x01);
    out8(PIC1_BASE + 1, 0x0);
    out8(PIC2_BASE + 1, 0x0);
}

BOOT_CODE શૂન્ય pic_disable(શૂન્ય)
{
    /* We assume that pic_remap_irqs has already been called and
     * just mask all the irqs */
    out8(PIC1_BASE + 1, 0xff);
    out8(PIC2_BASE + 1, 0xff);
}

શૂન્ય pic_mask_irq(bool_t mask, irq_t irq)
{
    uint8_t  bit_mask;
    uint16_t pic_port;

    પ્રતિજ્ઞા(irq >= irq_isa_min);
    પ્રતિજ્ઞા(irq <= irq_isa_max);

    જો (irq < 8) {
        bit_mask = BIT(irq);
        pic_port = PIC1_BASE + 1;
    } નહીંતર {
        bit_mask = BIT(irq - 8);
        pic_port = PIC2_BASE + 1;
    }

    જો (mask) {
        /* Disables the interrupt */
        out8(pic_port, (in8(pic_port) | bit_mask));
    } નહીંતર {
        /* Enables the interrupt */
        out8(pic_port, (in8(pic_port) & ~bit_mask));
    }
}

bool_t pic_is_irq_pending(શૂન્ય)
{
    /* Interrupt Request Register (IRR) - holds pending IRQs */
    uint8_t irr;

    /* Send to PIC1's OCW3, in order to read IRR from next inb instruction */
    out8(PIC1_BASE, 0x0a);

    /* Read IRR */
    irr = in8(PIC1_BASE);

    /* Since slave PIC is connected to IRQ2 of master PIC,
     * there is no need to check IRR of slave PIC.
     */
    પાછા irr != 0;
}

સ્થિર uint16_t pic_get_isr(શૂન્ય)
{
    out8(PIC1_BASE, 0x0b);
    out8(PIC2_BASE, 0x0b);
    પાછા (((uint16_t)in8(PIC2_BASE)) << 8) | in8(PIC1_BASE);
}

શૂન્ય pic_ack_active_irq(શૂન્ય)
{
    irq_t irq = getActiveIRQ();
    જો (irq >= irq_isa_min + 8) {
        /* ack slave PIC, unless we got a spurious irq 15
         * It is spurious if the bit is not set in the ISR
         * Even if it was spurious we will still need to
         * acknowledge the master PIC */
        જો (irq != irq_isa_min + 15 || (pic_get_isr() & BIT(15))) {
            out8(PIC2_BASE, 0x20);
        }
    }
    /* ack master PIC, unless we got a spurious IRQ 7
     * It is spurious if the bit is not set in the ISR */
    જો (irq != irq_isa_min + 7 || (pic_get_isr() & BIT(7))) {
        out8(PIC1_BASE, 0x20);
    }
}
