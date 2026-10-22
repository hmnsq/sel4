/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <object/reply.h>

शून्य प्रत्युत्तरम्_प्रक्षिप(tcb_t *tcb_caller, tcb_t *tcb_callee, प्रत्युत्तरम्_t *प्रत्युत्तरम्, bool_t canDonate)
{
    sched_context_t *sc_donated = tcb_caller->tcbSchedContext;

    प्रतिज्ञा(tcb_caller != रिक्त);
    प्रतिज्ञा(प्रत्युत्तरम् != रिक्त);
    प्रतिज्ञा(प्रत्युत्तरम्->प्रत्युत्तरम्_TCB == रिक्त);

    प्रतिज्ञा(call_राशिः_get_call_राशिः_Ptr(प्रत्युत्तरम्->प्रत्युत्तरम्_पूर्व) == 0);
    प्रतिज्ञा(call_राशिः_get_call_राशिः_Ptr(प्रत्युत्तरम्->प्रत्युत्तरम्_अग्र) == 0);

    /* caller state must be blocked on send or active, so cannot be in a existing call stack */
    प्रतिज्ञा(thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_BlockedOnSend ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Restart);

    /* callee state must be simple at this point (inactive, restart, or running),
       so it also cannot have a reply reference associated with it. */
    प्रतिज्ञा(thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Inactive ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Restart);

    /* link caller and reply */
    प्रत्युत्तरम्->प्रत्युत्तरम्_TCB = tcb_caller;
    समुच्चयः_Thread_State_Blocked_On_प्रत्युत्तरम्(tcb_caller, प्रत्युत्तरम्);

    यदि (sc_donated != रिक्त && tcb_callee->tcbSchedContext == रिक्त && canDonate) {
        प्रत्युत्तरम्_t *old_caller = sc_donated->sc_प्रत्युत्तरम्;

        /* check stack integrity */
        प्रतिज्ञा(old_caller == रिक्त ||
               SC_PTR(call_राशिः_get_call_राशिः_Ptr(old_caller->प्रत्युत्तरम्_अग्र)) == sc_donated);

        /* push on to stack */
        यदि (old_caller) {
            old_caller->प्रत्युत्तरम्_अग्र = call_राशिः_new(प्रत्युत्तरम्_REF(प्रत्युत्तरम्), false);
        }
        प्रत्युत्तरम्->प्रत्युत्तरम्_पूर्व = call_राशिः_new(प्रत्युत्तरम्_REF(old_caller), false);
        sc_donated->sc_प्रत्युत्तरम् = प्रत्युत्तरम्;
        प्रत्युत्तरम्->प्रत्युत्तरम्_अग्र = call_राशिः_new(SC_REF(sc_donated), true);

        /* now do the actual donation */
        schedContext_donate(sc_donated, tcb_callee);
    }
}

/* Pop the head reply from the call stack */
शून्य प्रत्युत्तरम्_निष्कासय(प्रत्युत्तरम्_t *प्रत्युत्तरम्, tcb_t *tcb)
{
    प्रतिज्ञा(प्रत्युत्तरम् != रिक्त);
    प्रतिज्ञा(प्रत्युत्तरम्->प्रत्युत्तरम्_TCB == tcb);
    प्रतिज्ञा(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_प्रत्युत्तरम्);
    प्रतिज्ञा(thread_state_get_प्रत्युत्तरम्_Object(tcb->tcbState) == प्रत्युत्तरम्_REF(प्रत्युत्तरम्));

    word_t अग्र_ptr = call_राशिः_get_call_राशिः_Ptr(प्रत्युत्तरम्->प्रत्युत्तरम्_अग्र);
    word_t पूर्व_ptr = call_राशिः_get_call_राशिः_Ptr(प्रत्युत्तरम्->प्रत्युत्तरम्_पूर्व);

    SC_PTR(अग्र_ptr)->sc_प्रत्युत्तरम् = प्रत्युत्तरम्_PTR(पूर्व_ptr);
    यदि (पूर्व_ptr != 0) {
        प्रत्युत्तरम्_PTR(पूर्व_ptr)->प्रत्युत्तरम्_अग्र = प्रत्युत्तरम्->प्रत्युत्तरम्_अग्र;
        प्रतिज्ञा(call_राशिः_get_is_शीर्ष(प्रत्युत्तरम्_PTR(पूर्व_ptr)->प्रत्युत्तरम्_अग्र));
    }

    प्रत्युत्तरम्->प्रत्युत्तरम्_अग्र = call_राशिः_new(0, false);

    /* give it back */
    यदि (tcb->tcbSchedContext == रिक्त) {
        /* only give the SC back if our SC is NULL. This prevents
         * strange behaviour when a thread is bound to an sc while it is
         * in the BlockedOnReply state. The semantics in this case are that the
         * SC cannot go back to the caller if the caller has received another one */
        schedContext_donate(SC_PTR(अग्र_ptr), tcb);
    }

    प्रत्युत्तरम्->प्रत्युत्तरम्_पूर्व = call_राशिः_new(0, false);
    प्रत्युत्तरम्_unlink(प्रत्युत्तरम्, tcb);
}

/* Remove a reply from the middle of the call stack */
शून्य प्रत्युत्तरम्_remove(प्रत्युत्तरम्_t *प्रत्युत्तरम्, tcb_t *tcb)
{
    प्रतिज्ञा(प्रत्युत्तरम्->प्रत्युत्तरम्_TCB == tcb);
    प्रतिज्ञा(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_प्रत्युत्तरम्);
    प्रतिज्ञा(thread_state_get_प्रत्युत्तरम्_Object(tcb->tcbState) == प्रत्युत्तरम्_REF(प्रत्युत्तरम्));

    word_t अग्र_ptr = call_राशिः_get_call_राशिः_Ptr(प्रत्युत्तरम्->प्रत्युत्तरम्_अग्र);
    word_t पूर्व_ptr = call_राशिः_get_call_राशिः_Ptr(प्रत्युत्तरम्->प्रत्युत्तरम्_पूर्व);

    यदि (likely(अग्र_ptr && call_राशिः_get_is_शीर्ष(प्रत्युत्तरम्->प्रत्युत्तरम्_अग्र))) {
        /* head of the call stack -> just pop */
        प्रत्युत्तरम्_निष्कासय(प्रत्युत्तरम्, tcb);
    } अन्यथा {
        यदि (अग्र_ptr) {
            /* not the head, remove from middle - break the chain */
            प्रत्युत्तरम्_PTR(अग्र_ptr)->प्रत्युत्तरम्_पूर्व = call_राशिः_new(0, false);
        }
        यदि (पूर्व_ptr) {
            प्रत्युत्तरम्_PTR(पूर्व_ptr)->प्रत्युत्तरम्_अग्र = call_राशिः_new(0, false);
        }
        प्रत्युत्तरम्->प्रत्युत्तरम्_पूर्व = call_राशिः_new(0, false);
        प्रत्युत्तरम्->प्रत्युत्तरम्_अग्र = call_राशिः_new(0, false);
        प्रत्युत्तरम्_unlink(प्रत्युत्तरम्, tcb);
    }
}

शून्य प्रत्युत्तरम्_remove_tcb(tcb_t *tcb)
{
    प्रतिज्ञा(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_प्रत्युत्तरम्);
    प्रत्युत्तरम्_t *प्रत्युत्तरम् = प्रत्युत्तरम्_PTR(thread_state_get_प्रत्युत्तरम्_Object(tcb->tcbState));
    word_t अग्र_ptr = call_राशिः_get_call_राशिः_Ptr(प्रत्युत्तरम्->प्रत्युत्तरम्_अग्र);
    word_t पूर्व_ptr = call_राशिः_get_call_राशिः_Ptr(प्रत्युत्तरम्->प्रत्युत्तरम्_पूर्व);

    यदि (अग्र_ptr) {
        यदि (call_राशिः_get_is_शीर्ष(प्रत्युत्तरम्->प्रत्युत्तरम्_अग्र)) {
            SC_PTR(अग्र_ptr)->sc_प्रत्युत्तरम् = रिक्त;
        } अन्यथा {
            प्रत्युत्तरम्_PTR(अग्र_ptr)->प्रत्युत्तरम्_पूर्व = call_राशिः_new(0, false);
        }
    }

    यदि (पूर्व_ptr) {
        प्रत्युत्तरम्_PTR(पूर्व_ptr)->प्रत्युत्तरम्_अग्र = call_राशिः_new(0, false);
    }

    प्रत्युत्तरम्->प्रत्युत्तरम्_पूर्व = call_राशिः_new(0, false);
    प्रत्युत्तरम्->प्रत्युत्तरम्_अग्र = call_राशिः_new(0, false);
    प्रत्युत्तरम्_unlink(प्रत्युत्तरम्, tcb);
}
