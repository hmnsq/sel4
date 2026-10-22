/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <machine/io.h>

#ifdef CONFIG_DEBUG_BUILD

শূন্য _fail(
    ধ্রুব বর্ণ  *s,
    ধ্রুব বর্ণ  *file,
    unsigned পূর্ণাঙ্ক সারি,
    ধ্রুব বর্ণ  *function)
{
    printf(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        সারি,
        function,
        s
    );
    halt();
}

শূন্য _assert_fail(
    ধ্রুব বর্ণ  *assertion,
    ধ্রুব বর্ণ  *file,
    unsigned পূর্ণাঙ্ক সারি,
    ধ্রুব বর্ণ  *function)
{
    printf("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           সারি,
           function
          );
    halt();
}

#endif
