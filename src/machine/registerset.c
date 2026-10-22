/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <machine/registerset.h>

ثابت ثبات_t fault_messages[][MAX_پیام_اندازه] = {
    [پیام_ID_Syscall] = SYSCALL_پیام,
    [پیام_ID_Exception] = EXCEPTION_پیام,
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    [پیام_ID_Timeout_پاسخ] = TIMEOUT_پاسخ_پیام,
#پایان_اگر
};
