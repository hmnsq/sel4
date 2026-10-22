/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <machine/registerset.h>

ثابت رجسٹر_t fault_messages[][MAX_پیغام_سائز] = {
    [پیغام_ID_Syscall] = SYSCALL_پیغام,
    [پیغام_ID_Exception] = EXCEPTION_پیغام,
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    [پیغام_ID_Timeout_جواب] = TIMEOUT_جواب_پیغام,
#اگر_ختم
};
