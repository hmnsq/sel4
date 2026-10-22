/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <model/smp.h>
#समावेश <object/tcb.h>

#अगर_घोषित ENABLE_SMP_SUPPORT

व्योम migrateTCB(tcb_t *tcb, word_t new_core)
{
    अगर (new_core == tcb->tcbAffinity) {
        वापस;
    }
#अगर_घोषित CONFIG_DEBUG_BUILD
    tcbDebugRemove(tcb);
#पूर्ण_अगर
#अगर_घोषित CONFIG_HAVE_FPU
    /* If the thread owns the FPU of the core it is currently running on (which
     * is not necessarily the core, that we are now running on), then release
     * that cores's FPU.
     */
    fpuRelease(tcb);
#पूर्ण_अगर /* CONFIG_HAVE_FPU */
    tcb->tcbAffinity = new_core;
#अगर_घोषित CONFIG_DEBUG_BUILD
    tcbDebugAppend(tcb);
#पूर्ण_अगर
}

#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */
