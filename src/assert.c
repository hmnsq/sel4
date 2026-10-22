/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <assert.h>
#समावेश <machine/io.h>

#अगर_घोषित CONFIG_DEBUG_BUILD

व्योम _fail(
    स्थिर अक्षर  *s,
    स्थिर अक्षर  *file,
    अचिन्हित पूर्णांक पंक्ति_2,
    स्थिर अक्षर  *function)
{
    म_लिखो(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        पंक्ति_2,
        function,
        s
    );
    halt();
}

व्योम _assert_fail(
    स्थिर अक्षर  *assertion,
    स्थिर अक्षर  *file,
    अचिन्हित पूर्णांक पंक्ति_2,
    स्थिर अक्षर  *function)
{
    म_लिखो("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           पंक्ति_2,
           function
          );
    halt();
}

#पूर्ण_अगर
