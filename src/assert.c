/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <machine/io.h>

#ifdef CONFIG_DEBUG_BUILD

ശൂന്യം _fail(
    സ്ഥിരം അക്ഷരം  *s,
    സ്ഥിരം അക്ഷരം  *file,
    unsigned പൂർണ്ണസംഖ്യ വരി,
    സ്ഥിരം അക്ഷരം  *function)
{
    printf(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        വരി,
        function,
        s
    );
    halt();
}

ശൂന്യം _assert_fail(
    സ്ഥിരം അക്ഷരം  *assertion,
    സ്ഥിരം അക്ഷരം  *file,
    unsigned പൂർണ്ണസംഖ്യ വരി,
    സ്ഥിരം അക്ഷരം  *function)
{
    printf("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           വരി,
           function
          );
    halt();
}

#endif
