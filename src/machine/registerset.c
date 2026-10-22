/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <machine/registerset.h>

ध्रुव register_t fault_messages[][MAX_सन्देशः_SIZE] = {
    [MessageID_Syscall] = SYSCALL_MESSAGE,
    [MessageID_Exception] = EXCEPTION_MESSAGE,
#ifdef CONFIG_KERNEL_MCS
    [MessageID_TimeoutReply] = TIMEOUT_REPLY_MESSAGE,
#endif
};
