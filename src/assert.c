/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <assert.h>
#شامل <machine/io.h>

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD

خالی _fail(
    ثابت حرف  *s,
    ثابت حرف  *file,
    غیرعلامتی صحیح سطر,
    ثابت حرف  *function)
{
    لکھو(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        سطر,
        function,
        s
    );
    halt();
}

خالی _assert_fail(
    ثابت حرف  *assertion,
    ثابت حرف  *file,
    غیرعلامتی صحیح سطر,
    ثابت حرف  *function)
{
    لکھو("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           سطر,
           function
          );
    halt();
}

#اگر_ختم
