/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <linker.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <plat/machine/hardware.h>
#अन्तर्भाव <plat/machine/pic.h>

/* PIC (i8259) base registers */
#परिभाषय PIC1_आधारः 0x20
#परिभाषय PIC2_आधारः 0xa0

/* Program PIC (i8259) to remap IRQs 0-15 to interrupt vectors starting at 'interrupt' */
BOOT_संहिता शून्य pic_remap_irqs(व्यवधानम्_t व्यवधानम्)
{
    out8(PIC1_आधारः, 0x11);
    out8(PIC2_आधारः, 0x11);
    out8(PIC1_आधारः + 1, व्यवधानम्);
    out8(PIC2_आधारः + 1, व्यवधानम् + 8);
    out8(PIC1_आधारः + 1, 0x04);
    out8(PIC2_आधारः + 1, 0x02);
    out8(PIC1_आधारः + 1, 0x01);
    out8(PIC2_आधारः + 1, 0x01);
    out8(PIC1_आधारः + 1, 0x0);
    out8(PIC2_आधारः + 1, 0x0);
}

BOOT_संहिता शून्य pic_disable(शून्य)
{
    /* We assume that pic_remap_irqs has already been called and
     * just mask all the irqs */
    out8(PIC1_आधारः + 1, 0xff);
    out8(PIC2_आधारः + 1, 0xff);
}

शून्य pic_आवरणम्_irq(bool_t आवरणम्_2, irq_t irq)
{
    uint8_t  bit_आवरणम्;
    uint16_t pic_पत्तनम्;

    प्रतिज्ञा(irq >= irq_isa_min);
    प्रतिज्ञा(irq <= irq_isa_max);

    यदि (irq < 8) {
        bit_आवरणम् = BIT(irq);
        pic_पत्तनम् = PIC1_आधारः + 1;
    } अन्यथा {
        bit_आवरणम् = BIT(irq - 8);
        pic_पत्तनम् = PIC2_आधारः + 1;
    }

    यदि (आवरणम्_2) {
        /* Disables the interrupt */
        out8(pic_पत्तनम्, (in8(pic_पत्तनम्) | bit_आवरणम्));
    } अन्यथा {
        /* Enables the interrupt */
        out8(pic_पत्तनम्, (in8(pic_पत्तनम्) & ~bit_आवरणम्));
    }
}

bool_t pic_is_irq_pending(शून्य)
{
    /* Interrupt Request Register (IRR) - holds pending IRQs */
    uint8_t irr;

    /* Send to PIC1's OCW3, in order to read IRR from next inb instruction */
    out8(PIC1_आधारः, 0x0a);

    /* Read IRR */
    irr = in8(PIC1_आधारः);

    /* Since slave PIC is connected to IRQ2 of master PIC,
     * there is no need to check IRR of slave PIC.
     */
    प्रत्यागम irr != 0;
}

स्थिर uint16_t pic_get_isr(शून्य)
{
    out8(PIC1_आधारः, 0x0b);
    out8(PIC2_आधारः, 0x0b);
    प्रत्यागम (((uint16_t)in8(PIC2_आधारः)) << 8) | in8(PIC1_आधारः);
}

शून्य pic_ack_सक्रिय_irq(शून्य)
{
    irq_t irq = get_सक्रिय_IRQ();
    यदि (irq >= irq_isa_min + 8) {
        /* ack slave PIC, unless we got a spurious irq 15
         * It is spurious if the bit is not set in the ISR
         * Even if it was spurious we will still need to
         * acknowledge the master PIC */
        यदि (irq != irq_isa_min + 15 || (pic_get_isr() & BIT(15))) {
            out8(PIC2_आधारः, 0x20);
        }
    }
    /* ack master PIC, unless we got a spurious IRQ 7
     * It is spurious if the bit is not set in the ISR */
    यदि (irq != irq_isa_min + 7 || (pic_get_isr() & BIT(7))) {
        out8(PIC1_आधारः, 0x20);
    }
}
