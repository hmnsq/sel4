/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <machine/registerset.h>

ثابت رجسٹر_t fault_messages[][MAX_پیغام_ناپ] = {
    [سنیہا_ID_Syscall] = SYSCALL_سنیہا,
    [سنیہا_ID_Exception] = EXCEPTION_سنیہا,
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    [سنیہا_ID_Timeout_جواب] = TIMEOUT_جواب_سنیہا,
#جے_انت
};
