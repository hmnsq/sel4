/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <linker.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <plat/machine/hardware.h>
#அந்தர்பாவ <plat/machine/pic.h>

/* PIC (i8259) base registers */
#define PIC1_BASE 0x20
#define PIC2_BASE 0xa0

/* Program PIC (i8259) to remap IRQs 0-15 to interrupt vectors starting at 'interrupt' */
BOOT_CODE வெறுமை pic_remap_irqs(interrupt_t interrupt)
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

BOOT_CODE வெறுமை pic_disable(வெறுமை)
{
    /* We assume that pic_remap_irqs has already been called and
     * just mask all the irqs */
    out8(PIC1_BASE + 1, 0xff);
    out8(PIC2_BASE + 1, 0xff);
}

வெறுமை pic_mask_irq(bool_t mask, irq_t irq)
{
    uint8_t  bit_mask;
    uint16_t pic_port;

    ப்ரதிஜ்ஞா(irq >= irq_isa_min);
    ப்ரதிஜ்ஞா(irq <= irq_isa_max);

    எனில் (irq < 8) {
        bit_mask = BIT(irq);
        pic_port = PIC1_BASE + 1;
    } இல்லையெனில் {
        bit_mask = BIT(irq - 8);
        pic_port = PIC2_BASE + 1;
    }

    எனில் (mask) {
        /* Disables the interrupt */
        out8(pic_port, (in8(pic_port) | bit_mask));
    } இல்லையெனில் {
        /* Enables the interrupt */
        out8(pic_port, (in8(pic_port) & ~bit_mask));
    }
}

bool_t pic_is_irq_pending(வெறுமை)
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
    திரும்பு irr != 0;
}

நிலையான uint16_t pic_get_isr(வெறுமை)
{
    out8(PIC1_BASE, 0x0b);
    out8(PIC2_BASE, 0x0b);
    திரும்பு (((uint16_t)in8(PIC2_BASE)) << 8) | in8(PIC1_BASE);
}

வெறுமை pic_ack_active_irq(வெறுமை)
{
    irq_t irq = getActiveIRQ();
    எனில் (irq >= irq_isa_min + 8) {
        /* ack slave PIC, unless we got a spurious irq 15
         * It is spurious if the bit is not set in the ISR
         * Even if it was spurious we will still need to
         * acknowledge the master PIC */
        எனில் (irq != irq_isa_min + 15 || (pic_get_isr() & BIT(15))) {
            out8(PIC2_BASE, 0x20);
        }
    }
    /* ack master PIC, unless we got a spurious IRQ 7
     * It is spurious if the bit is not set in the ISR */
    எனில் (irq != irq_isa_min + 7 || (pic_get_isr() & BIT(7))) {
        out8(PIC1_BASE, 0x20);
    }
}
