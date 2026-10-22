/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <machine/io.h>

#که_تعریف_شوی CONFIG_DEBUG_BUILD

فراغ _fail(
    ثابت حرف  *s,
    ثابت حرف  *file,
    بینښه صحيح کرښه,
    ثابت حرف  *function)
{
    چاپ_کړه(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        کرښه,
        function,
        s
    );
    halt();
}

فراغ _assert_fail(
    ثابت حرف  *assertion,
    ثابت حرف  *file,
    بینښه صحيح کرښه,
    ثابت حرف  *function)
{
    چاپ_کړه("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           کرښه,
           function
          );
    halt();
}

#د_که_پای
