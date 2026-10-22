/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <assert.h>
#અન્તર્ભાવ <machine/io.h>

#ifdef CONFIG_DEBUG_BUILD

શૂન્ય _fail(
    અચલ અક્ષર  *s,
    અચલ અક્ષર  *file,
    unsigned પૂર્ણાંક લીટી,
    અચલ અક્ષર  *function)
{
    printf(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        લીટી,
        function,
        s
    );
    halt();
}

શૂન્ય _assert_fail(
    અચલ અક્ષર  *assertion,
    અચલ અક્ષર  *file,
    unsigned પૂર્ણાંક લીટી,
    અચલ અક્ષર  *function)
{
    printf("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           લીટી,
           function
          );
    halt();
}

#endif
