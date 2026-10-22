/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>

#ifdef CONFIG_DEBUG_BUILD

ಶೂನ್ಯ _fail(
    ಧ್ರುವ ವರ್ಣ  *s,
    ಧ್ರುವ ವರ್ಣ  *file,
    unsigned ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
    ಧ್ರುವ ವರ್ಣ  *function)
{
    printf(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        ಸಾಲು,
        function,
        s
    );
    halt();
}

ಶೂನ್ಯ _assert_fail(
    ಧ್ರುವ ವರ್ಣ  *assertion,
    ಧ್ರುವ ವರ್ಣ  *file,
    unsigned ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
    ಧ್ರುವ ವರ್ಣ  *function)
{
    printf("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           ಸಾಲು,
           function
          );
    halt();
}

#endif
