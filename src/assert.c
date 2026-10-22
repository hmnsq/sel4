/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <machine/io.h>

#ifdef CONFIG_DEBUG_BUILD

शून्य _fail(
    स्थिरांक अक्षर  *s,
    स्थिरांक अक्षर  *file,
    अचिन्हित पूर्णांक ओळ,
    स्थिरांक अक्षर  *function)
{
    म_लिखो(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        ओळ,
        function,
        s
    );
    halt();
}

शून्य _assert_fail(
    स्थिरांक अक्षर  *assertion,
    स्थिरांक अक्षर  *file,
    अचिन्हित पूर्णांक ओळ,
    स्थिरांक अक्षर  *function)
{
    म_लिखो("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           ओळ,
           function
          );
    halt();
}

#endif
