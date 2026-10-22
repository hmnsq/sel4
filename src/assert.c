/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <assert.h>
#అన్తర్భావ <machine/io.h>

#నిర్వచితమైతే CONFIG_DEBUG_BUILD

శూన్య _fail(
    స్థిరరాశి అక్షరం  *s,
    స్థిరరాశి అక్షరం  *file,
    గుర్తులేని పూర్ణాంకం పంక్తి,
    స్థిరరాశి అక్షరం  *function)
{
    రాయి(
        "seL4 called fail at %s:%u in function %s, saying \"%s\"\n",
        file,
        పంక్తి,
        function,
        s
    );
    halt();
}

శూన్య _assert_fail(
    స్థిరరాశి అక్షరం  *assertion,
    స్థిరరాశి అక్షరం  *file,
    గుర్తులేని పూర్ణాంకం పంక్తి,
    స్థిరరాశి అక్షరం  *function)
{
    రాయి("seL4 failed assertion '%s' at %s:%u in function %s\n",
           assertion,
           file,
           పంక్తి,
           function
          );
    halt();
}

#అయితే_ముగింపు
