/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <assert.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>

#ifdef CONFIG_DEBUG_BUILD

ଶୂନ୍ୟ _fail(
    ଅଚଳ ଅକ୍ଷର  *s,
    ଅଚଳ ଅକ୍ଷର  *file,
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
    ଅଚଳ ଅକ୍ଷର  *function)
{
    printf(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        ଧାଡ଼ି,
        function,
        s
    );
    halt();
}

ଶୂନ୍ୟ _assert_fail(
    ଅଚଳ ଅକ୍ଷର  *assertion,
    ଅଚଳ ଅକ୍ଷର  *file,
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
    ଅଚଳ ଅକ୍ଷର  *function)
{
    printf("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           ଧାଡ଼ି,
           function
          );
    halt();
}

#endif
