/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <machine/registerset.h>

स्थिर register_t fault_messages[][MAX_संदेश_आकार] = {
    [MessageID_Syscall] = SYSCALL_MESSAGE,
    [MessageID_Exception] = EXCEPTION_MESSAGE,
#अगर_घोषित CONFIG_KERNEL_MCS
    [MessageID_TimeoutReply] = TIMEOUT_REPLY_MESSAGE,
#पूर्ण_अगर
};
