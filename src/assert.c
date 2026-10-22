/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <assert.h>
#include <machine/io.h>

#ifdef CONFIG_DEBUG_BUILD

void _fail(
    const char  *s,
    const char  *file,
    unsigned int लिन्या,
    const char  *function)
{
    printf(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        लिन्या,
        function,
        s
    );
    halt();
}

void _assert_fail(
    const char  *assertion,
    const char  *file,
    unsigned int लिन्या,
    const char  *function)
{
    printf("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           लिन्या,
           function
          );
    halt();
}

#endif
