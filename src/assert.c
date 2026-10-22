/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <machine/io.h>

#यदि_परिभाषितम् CONFIG_DEBUG_BUILD

शून्य _fail(
    ध्रुव वर्ण  *s,
    ध्रुव वर्ण  *file,
    अचिह्नित पूर्णाङ्क पङ्क्तिः,
    ध्रुव वर्ण  *function)
{
    प्रारूप_लिख(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        पङ्क्तिः,
        function,
        s
    );
    halt();
}

शून्य _assert_fail(
    ध्रुव वर्ण  *assertion,
    ध्रुव वर्ण  *file,
    अचिह्नित पूर्णाङ्क पङ्क्तिः,
    ध्रुव वर्ण  *function)
{
    प्रारूप_लिख("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           पङ्क्तिः,
           function
          );
    halt();
}

#यदि_अन्तः
