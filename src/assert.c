/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD

ಶೂನ್ಯ _fail(
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  *s,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  *file,
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸಾಲು,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  *function)
{
    ಬರೆ(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        ಸಾಲು,
        function,
        s
    );
    halt();
}

ಶೂನ್ಯ _assert_fail(
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  *assertion,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  *file,
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸಾಲು,
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  *function)
{
    ಬರೆ("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           ಸಾಲು,
           function
          );
    halt();
}

#ಆದರೆ_ಅಂತ್ಯ
