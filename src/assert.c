/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <machine/io.h>

#اگر_تعریف_شده CONFIG_DEBUG_BUILD

فراغ _fail(
    ثابت حرف  *s,
    ثابت حرف  *file,
    بیعلامت صحيح سطر,
    ثابت حرف  *function)
{
    چاپ_کن(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        سطر,
        function,
        s
    );
    halt();
}

فراغ _assert_fail(
    ثابت حرف  *assertion,
    ثابت حرف  *file,
    بیعلامت صحيح سطر,
    ثابت حرف  *function)
{
    چاپ_کن("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           سطر,
           function
          );
    halt();
}

#پایان_اگر
