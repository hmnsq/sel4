/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <plat/machine/devices_gen.h>

#define URXD  0x00 /* UART Receiver Register */
#define UTXD  0x40 /* UART Transmitter Register */
#define UCR1  0x80 /* UART Control Register 1 */
#define UCR2  0x84 /* UART Control Register 2 */
#define UCR3  0x88 /* UART Control Register 3 */
#define UCR4  0x8c /* UART Control Register 4 */
#define UFCR  0x90 /* UART FIFO Control Register */
#define USR1  0x94 /* UART Status Register 1 */
#define USR2  0x98 /* UART Status Register 2 */
#define UESC  0x9c /* UART Escape Character Register */
#define UTIM  0xa0 /* UART Escape Timer Register */
#define UBIR  0xa4 /* UART BRM Incremental Register */
#define UBMR  0xa8 /* UART BRM Modulator Register */
#define UBRC  0xac /* UART Baud Rate Counter Register */
#define ONEMS 0xb0 /* UART One Millisecond Register */
#define UTS   0xb4 /* UART Test Register */

#define UART_SR1_TRDY         BIT(13)
#define UART_SR1_RRDY         BIT(9)
#define UART_SR2_TXFIFO_EMPTY BIT(14)
#define UART_SR2_RXFIFO_RDR   BIT(0)

#define UART_REG(x) ((चञ्चल uint32_t *)(UART_PPTR + (x)))

#ifdef CONFIG_PRINTING
शून्य uart_drv_putchar(अचिन्हित वर्ण c)
{
    जबसम्म (!(*UART_REG(USR2) & UART_SR2_TXFIFO_EMPTY));
    *UART_REG(UTXD) = c;
}
#endif /* CONFIG_PRINTING */

#ifdef CONFIG_DEBUG_BUILD
अचिन्हित वर्ण uart_drv_getchar(शून्य)
{
    जबसम्म (!(*UART_REG(USR2) & UART_SR2_RXFIFO_RDR));
    फर्काऊ *UART_REG(URXD);
}
#endif /* CONFIG_DEBUG_BUILD */
