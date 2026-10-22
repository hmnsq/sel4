/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <machine/io.h>

#ifdef CONFIG_DEBUG_BUILD

শূন্য _fail(
    ধ্রুৱ ৱর্ণ  *s,
    ধ্রুৱ ৱর্ণ  *file,
    unsigned পূর্ণাঙ্ক সাৰি,
    ধ্রুৱ ৱর্ণ  *function)
{
    printf(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        সাৰি,
        function,
        s
    );
    halt();
}

শূন্য _assert_fail(
    ধ্রুৱ ৱর্ণ  *assertion,
    ধ্রুৱ ৱর্ণ  *file,
    unsigned পূর্ণাঙ্ক সাৰি,
    ধ্রুৱ ৱর্ণ  *function)
{
    printf("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           সাৰি,
           function
          );
    halt();
}

#endif
