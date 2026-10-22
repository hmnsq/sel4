/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <assert.h>
#شامل <machine/io.h>

#ifdef CONFIG_DEBUG_BUILD

تهی _fail(
    ثابت نویسه  *s,
    ثابت نویسه  *file,
    unsigned صحیح سطر,
    ثابت نویسه  *function)
{
    printf(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        سطر,
        function,
        s
    );
    halt();
}

تهی _assert_fail(
    ثابت نویسه  *assertion,
    ثابت نویسه  *file,
    unsigned صحیح سطر,
    ثابت نویسه  *function)
{
    printf("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           سطر,
           function
          );
    halt();
}

#endif
