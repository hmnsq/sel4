/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <linker.h>
#ضمن <machine/io.h>
#ضمن <plat/machine/hardware.h>
#ضمن <plat/machine/pic.h>

/* PIC (i8259) base registers */
#تعریف_کن PIC1_پایه 0x20
#تعریف_کن PIC2_پایه 0xa0

/* Program PIC (i8259) to remap IRQs 0-15 to interrupt vectors starting at 'interrupt' */
BOOT_کد فراغ pic_remap_irqs(وقفه_t وقفه)
{
    out8(PIC1_پایه, 0x11);
    out8(PIC2_پایه, 0x11);
    out8(PIC1_پایه + 1, وقفه);
    out8(PIC2_پایه + 1, وقفه + 8);
    out8(PIC1_پایه + 1, 0x04);
    out8(PIC2_پایه + 1, 0x02);
    out8(PIC1_پایه + 1, 0x01);
    out8(PIC2_پایه + 1, 0x01);
    out8(PIC1_پایه + 1, 0x0);
    out8(PIC2_پایه + 1, 0x0);
}

BOOT_کد فراغ pic_disable(فراغ)
{
    /* We assume that pic_remap_irqs has already been called and
     * just mask all the irqs */
    out8(PIC1_پایه + 1, 0xff);
    out8(PIC2_پایه + 1, 0xff);
}

فراغ pic_ماسک_irq(bool_t ماسک_2, irq_t irq)
{
    uint8_t  بیت_ماسک;
    uint16_t pic_پورت;

    أكد(irq >= irq_isa_min);
    أكد(irq <= irq_isa_max);

    إذا (irq < 8) {
        بیت_ماسک = بیت(irq);
        pic_پورت = PIC1_پایه + 1;
    } وإلا {
        بیت_ماسک = بیت(irq - 8);
        pic_پورت = PIC2_پایه + 1;
    }

    إذا (ماسک_2) {
        /* Disables the interrupt */
        out8(pic_پورت, (in8(pic_پورت) | بیت_ماسک));
    } وإلا {
        /* Enables the interrupt */
        out8(pic_پورت, (in8(pic_پورت) & ~بیت_ماسک));
    }
}

bool_t pic_is_irq_pending(فراغ)
{
    /* Interrupt Request Register (IRR) - holds pending IRQs */
    uint8_t irr;

    /* Send to PIC1's OCW3, in order to read IRR from next inb instruction */
    out8(PIC1_پایه, 0x0a);

    /* Read IRR */
    irr = in8(PIC1_پایه);

    /* Since slave PIC is connected to IRQ2 of master PIC,
     * there is no need to check IRR of slave PIC.
     */
    أرجع irr != 0;
}

ساكن uint16_t pic_get_isr(فراغ)
{
    out8(PIC1_پایه, 0x0b);
    out8(PIC2_پایه, 0x0b);
    أرجع (((uint16_t)in8(PIC2_پایه)) << 8) | in8(PIC1_پایه);
}

فراغ pic_ack_فعال_irq(فراغ)
{
    irq_t irq = get_فعال_IRQ();
    إذا (irq >= irq_isa_min + 8) {
        /* ack slave PIC, unless we got a spurious irq 15
         * It is spurious if the bit is not set in the ISR
         * Even if it was spurious we will still need to
         * acknowledge the master PIC */
        إذا (irq != irq_isa_min + 15 || (pic_get_isr() & بیت(15))) {
            out8(PIC2_پایه, 0x20);
        }
    }
    /* ack master PIC, unless we got a spurious IRQ 7
     * It is spurious if the bit is not set in the ISR */
    إذا (irq != irq_isa_min + 7 || (pic_get_isr() & بیت(7))) {
        out8(PIC1_پایه, 0x20);
    }
}
