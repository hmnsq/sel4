/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/pic.h>

/* PIC (i8259) base registers */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PIC1_ਅਧਾਰ 0x20
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PIC2_ਅਧਾਰ 0xa0

/* Program PIC (i8259) to remap IRQs 0-15 to interrupt vectors starting at 'interrupt' */
BOOT_ਕੋਡ ਖਾਲੀ pic_remap_irqs(ਰੁਕਾਵਟ_t ਰੁਕਾਵਟ)
{
    out8(PIC1_ਅਧਾਰ, 0x11);
    out8(PIC2_ਅਧਾਰ, 0x11);
    out8(PIC1_ਅਧਾਰ + 1, ਰੁਕਾਵਟ);
    out8(PIC2_ਅਧਾਰ + 1, ਰੁਕਾਵਟ + 8);
    out8(PIC1_ਅਧਾਰ + 1, 0x04);
    out8(PIC2_ਅਧਾਰ + 1, 0x02);
    out8(PIC1_ਅਧਾਰ + 1, 0x01);
    out8(PIC2_ਅਧਾਰ + 1, 0x01);
    out8(PIC1_ਅਧਾਰ + 1, 0x0);
    out8(PIC2_ਅਧਾਰ + 1, 0x0);
}

BOOT_ਕੋਡ ਖਾਲੀ pic_disable(ਖਾਲੀ)
{
    /* We assume that pic_remap_irqs has already been called and
     * just mask all the irqs */
    out8(PIC1_ਅਧਾਰ + 1, 0xff);
    out8(PIC2_ਅਧਾਰ + 1, 0xff);
}

ਖਾਲੀ pic_ਨਕਾਬ_irq(bool_t ਨਕਾਬ_2, irq_t irq)
{
    uint8_t  ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ;
    uint16_t pic_ਪੋਰਟ;

    ਪ੍ਰਤਿਜ੍ਞਾ(irq >= irq_isa_min);
    ਪ੍ਰਤਿਜ੍ਞਾ(irq <= irq_isa_max);

    ਜੇ (irq < 8) {
        ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ = ਦੋਆਧਾਰੀ_ਅੰਕ(irq);
        pic_ਪੋਰਟ = PIC1_ਅਧਾਰ + 1;
    } ਨਹੀਂਤਾਂ {
        ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ = ਦੋਆਧਾਰੀ_ਅੰਕ(irq - 8);
        pic_ਪੋਰਟ = PIC2_ਅਧਾਰ + 1;
    }

    ਜੇ (ਨਕਾਬ_2) {
        /* Disables the interrupt */
        out8(pic_ਪੋਰਟ, (in8(pic_ਪੋਰਟ) | ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ));
    } ਨਹੀਂਤਾਂ {
        /* Enables the interrupt */
        out8(pic_ਪੋਰਟ, (in8(pic_ਪੋਰਟ) & ~ਦੋਆਧਾਰੀ_ਅੰਕ_ਨਕਾਬ));
    }
}

bool_t pic_is_irq_pending(ਖਾਲੀ)
{
    /* Interrupt Request Register (IRR) - holds pending IRQs */
    uint8_t irr;

    /* Send to PIC1's OCW3, in order to read IRR from next inb instruction */
    out8(PIC1_ਅਧਾਰ, 0x0a);

    /* Read IRR */
    irr = in8(PIC1_ਅਧਾਰ);

    /* Since slave PIC is connected to IRQ2 of master PIC,
     * there is no need to check IRR of slave PIC.
     */
    ਵਾਪਸ irr != 0;
}

ਸਥਿਰ uint16_t pic_get_isr(ਖਾਲੀ)
{
    out8(PIC1_ਅਧਾਰ, 0x0b);
    out8(PIC2_ਅਧਾਰ, 0x0b);
    ਵਾਪਸ (((uint16_t)in8(PIC2_ਅਧਾਰ)) << 8) | in8(PIC1_ਅਧਾਰ);
}

ਖਾਲੀ pic_ack_ਸਰਗਰਮ_irq(ਖਾਲੀ)
{
    irq_t irq = get_ਸਰਗਰਮ_IRQ();
    ਜੇ (irq >= irq_isa_min + 8) {
        /* ack slave PIC, unless we got a spurious irq 15
         * It is spurious if the bit is not set in the ISR
         * Even if it was spurious we will still need to
         * acknowledge the master PIC */
        ਜੇ (irq != irq_isa_min + 15 || (pic_get_isr() & ਦੋਆਧਾਰੀ_ਅੰਕ(15))) {
            out8(PIC2_ਅਧਾਰ, 0x20);
        }
    }
    /* ack master PIC, unless we got a spurious IRQ 7
     * It is spurious if the bit is not set in the ISR */
    ਜੇ (irq != irq_isa_min + 7 || (pic_get_isr() & ਦੋਆਧਾਰੀ_ਅੰਕ(7))) {
        out8(PIC1_ਅਧਾਰ, 0x20);
    }
}
