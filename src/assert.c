/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <assert.h>
#অন্তর্ভাব <machine/io.h>

#ifdef CONFIG_DEBUG_BUILD

শূন্য _fail(
    ধ্রুবক অক্ষর  *s,
    ধ্রুবক অক্ষর  *file,
    unsigned পূর্ণসংখ্যা সারি,
    ধ্রুবক অক্ষর  *function)
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
    ধ্রুবক অক্ষর  *assertion,
    ধ্রুবক অক্ষর  *file,
    unsigned পূর্ণসংখ্যা সারি,
    ধ্রুবক অক্ষর  *function)
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
