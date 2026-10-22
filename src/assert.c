/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <machine/io.h>

#ifdef CONFIG_DEBUG_BUILD

فراغ _fail(
    ثابت حرف  *s,
    ثابت حرف  *file,
    unsigned صحيح هێڵ,
    ثابت حرف  *function)
{
    printf(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        هێڵ,
        function,
        s
    );
    halt();
}

فراغ _assert_fail(
    ثابت حرف  *assertion,
    ثابت حرف  *file,
    unsigned صحيح هێڵ,
    ثابت حرف  *function)
{
    printf("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           هێڵ,
           function
          );
    halt();
}

#endif
