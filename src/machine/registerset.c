/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <machine/registerset.h>

ثابت رجسٽر_t fault_messages[][MAX_نياپو_سائيز] = {
    [پيغام_ID_Syscall] = SYSCALL_پيغام,
    [پيغام_ID_Exception] = EXCEPTION_پيغام,
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    [پيغام_ID_Timeout_جواب] = TIMEOUT_جواب_پيغام,
#جيڪڏهن_پورو
};
