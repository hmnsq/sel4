/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <machine/io.h>

#ifdef CONFIG_DEBUG_BUILD

शून्य _fail(
    अचर वर्ण  *s,
    अचर वर्ण  *file,
    अचिन्हित पूर्णाङ्क पङ्क्ति,
    अचर वर्ण  *function)
{
    म_लिखो(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        पङ्क्ति,
        function,
        s
    );
    halt();
}

शून्य _assert_fail(
    अचर वर्ण  *assertion,
    अचर वर्ण  *file,
    अचिन्हित पूर्णाङ्क पङ्क्ति,
    अचर वर्ण  *function)
{
    म_लिखो("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           पङ्क्ति,
           function
          );
    halt();
}

#endif
