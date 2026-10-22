/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <assert.h>
#அந்தர்பாவ <machine/io.h>

#ifdef CONFIG_DEBUG_BUILD

வெறுமை _fail(
    மாறிலி எழுத்து  *s,
    மாறிலி எழுத்து  *file,
    unsigned முழு வரி,
    மாறிலி எழுத்து  *function)
{
    printf(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        வரி,
        function,
        s
    );
    halt();
}

வெறுமை _assert_fail(
    மாறிலி எழுத்து  *assertion,
    மாறிலி எழுத்து  *file,
    unsigned முழு வரி,
    மாறிலி எழுத்து  *function)
{
    printf("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           வரி,
           function
          );
    halt();
}

#endif
