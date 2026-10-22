/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <machine/io.h>

#ifdef CONFIG_DEBUG_BUILD

शून्य _fail(
    ध्रुव वर्ण  *s,
    ध्रुव वर्ण  *file,
    अचिन्हित पूर्णाङ्क पंक्ति,
    ध्रुव वर्ण  *function)
{
    म_लिखो(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        पंक्ति,
        function,
        s
    );
    halt();
}

शून्य _assert_fail(
    ध्रुव वर्ण  *assertion,
    ध्रुव वर्ण  *file,
    अचिन्हित पूर्णाङ्क पंक्ति,
    ध्रुव वर्ण  *function)
{
    म_लिखो("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           पंक्ति,
           function
          );
    halt();
}

#endif
