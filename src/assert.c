/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <machine/io.h>

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD

فراغ _fail(
    ثابت حرف  *s,
    ثابت حرف  *file,
    بنانشان صحيح سٽ,
    ثابت حرف  *function)
{
    لکو(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        سٽ,
        function,
        s
    );
    halt();
}

فراغ _assert_fail(
    ثابت حرف  *assertion,
    ثابت حرف  *file,
    بنانشان صحيح سٽ,
    ثابت حرف  *function)
{
    لکو("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           سٽ,
           function
          );
    halt();
}

#جيڪڏهن_پورو
