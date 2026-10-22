/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/pic.h>

/* PIC (i8259) base registers */
#ವ್ಯಾಖ್ಯಾನಿಸು PIC1_ತಳ 0x20
#ವ್ಯಾಖ್ಯಾನಿಸು PIC2_ತಳ 0xa0

/* Program PIC (i8259) to remap IRQs 0-15 to interrupt vectors starting at 'interrupt' */
BOOT_ಸಂಕೇತ ಶೂನ್ಯ pic_remap_irqs(ಅಡಚಣೆ_t ಅಡಚಣೆ)
{
    out8(PIC1_ತಳ, 0x11);
    out8(PIC2_ತಳ, 0x11);
    out8(PIC1_ತಳ + 1, ಅಡಚಣೆ);
    out8(PIC2_ತಳ + 1, ಅಡಚಣೆ + 8);
    out8(PIC1_ತಳ + 1, 0x04);
    out8(PIC2_ತಳ + 1, 0x02);
    out8(PIC1_ತಳ + 1, 0x01);
    out8(PIC2_ತಳ + 1, 0x01);
    out8(PIC1_ತಳ + 1, 0x0);
    out8(PIC2_ತಳ + 1, 0x0);
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ pic_disable(ಶೂನ್ಯ)
{
    /* We assume that pic_remap_irqs has already been called and
     * just mask all the irqs */
    out8(PIC1_ತಳ + 1, 0xff);
    out8(PIC2_ತಳ + 1, 0xff);
}

ಶೂನ್ಯ pic_ಮುಖವಾಡ_irq(bool_t ಮುಖವಾಡ_2, irq_t irq)
{
    uint8_t  ಬಿಟ್_ಮುಖವಾಡ;
    uint16_t pic_ಪೋರ್ಟ್;

    ಪ್ರತಿಜ್ಞಾ(irq >= irq_isa_min);
    ಪ್ರತಿಜ್ಞಾ(irq <= irq_isa_max);

    ಆದರೆ (irq < 8) {
        ಬಿಟ್_ಮುಖವಾಡ = ಬಿಟ್(irq);
        pic_ಪೋರ್ಟ್ = PIC1_ತಳ + 1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಬಿಟ್_ಮುಖವಾಡ = ಬಿಟ್(irq - 8);
        pic_ಪೋರ್ಟ್ = PIC2_ತಳ + 1;
    }

    ಆದರೆ (ಮುಖವಾಡ_2) {
        /* Disables the interrupt */
        out8(pic_ಪೋರ್ಟ್, (in8(pic_ಪೋರ್ಟ್) | ಬಿಟ್_ಮುಖವಾಡ));
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Enables the interrupt */
        out8(pic_ಪೋರ್ಟ್, (in8(pic_ಪೋರ್ಟ್) & ~ಬಿಟ್_ಮುಖವಾಡ));
    }
}

bool_t pic_is_irq_pending(ಶೂನ್ಯ)
{
    /* Interrupt Request Register (IRR) - holds pending IRQs */
    uint8_t irr;

    /* Send to PIC1's OCW3, in order to read IRR from next inb instruction */
    out8(PIC1_ತಳ, 0x0a);

    /* Read IRR */
    irr = in8(PIC1_ತಳ);

    /* Since slave PIC is connected to IRQ2 of master PIC,
     * there is no need to check IRR of slave PIC.
     */
    ಹಿಂತಿರುಗಿ irr != 0;
}

ಸ್ಥಿರ uint16_t pic_get_isr(ಶೂನ್ಯ)
{
    out8(PIC1_ತಳ, 0x0b);
    out8(PIC2_ತಳ, 0x0b);
    ಹಿಂತಿರುಗಿ (((uint16_t)in8(PIC2_ತಳ)) << 8) | in8(PIC1_ತಳ);
}

ಶೂನ್ಯ pic_ack_ಸಕ್ರಿಯ_irq(ಶೂನ್ಯ)
{
    irq_t irq = get_ಸಕ್ರಿಯ_IRQ();
    ಆದರೆ (irq >= irq_isa_min + 8) {
        /* ack slave PIC, unless we got a spurious irq 15
         * It is spurious if the bit is not set in the ISR
         * Even if it was spurious we will still need to
         * acknowledge the master PIC */
        ಆದರೆ (irq != irq_isa_min + 15 || (pic_get_isr() & ಬಿಟ್(15))) {
            out8(PIC2_ತಳ, 0x20);
        }
    }
    /* ack master PIC, unless we got a spurious IRQ 7
     * It is spurious if the bit is not set in the ISR */
    ಆದರೆ (irq != irq_isa_min + 7 || (pic_get_isr() & ಬಿಟ್(7))) {
        out8(PIC1_ತಳ, 0x20);
    }
}
