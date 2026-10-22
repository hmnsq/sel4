/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD

ਖਾਲੀ _fail(
    ਅਚਲ ਅੱਖਰ  *s,
    ਅਚਲ ਅੱਖਰ  *file,
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸਤਰ,
    ਅਚਲ ਅੱਖਰ  *function)
{
    ਲਿਖੋ(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        ਸਤਰ,
        function,
        s
    );
    halt();
}

ਖਾਲੀ _assert_fail(
    ਅਚਲ ਅੱਖਰ  *assertion,
    ਅਚਲ ਅੱਖਰ  *file,
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਸਤਰ,
    ਅਚਲ ਅੱਖਰ  *function)
{
    ਲਿਖੋ("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           ਸਤਰ,
           function
          );
    halt();
}

#ਜੇ_ਅੰਤ
