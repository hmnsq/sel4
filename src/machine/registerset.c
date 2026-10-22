/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <machine/registerset.h>

ثابت ثبتوونکی_t fault_messages[][MAX_پیغام_اندازه] = {
    [پیغام_ID_Syscall] = SYSCALL_پیغام,
    [پیغام_ID_Exception] = EXCEPTION_پیغام,
#که_تعریف_شوی CONFIG_KERNEL_MCS
    [پیغام_ID_Timeout_ځواب] = TIMEOUT_ځواب_پیغام,
#د_که_پای
};
