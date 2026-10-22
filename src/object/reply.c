/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <object/reply.h>

व्योम reply_धकेलो(tcb_t *tcb_caller, tcb_t *tcb_callee, reply_t *reply, bool_t canDonate)
{
    sched_context_t *sc_donated = tcb_caller->tcbSchedContext;

    निश्चित(tcb_caller != शून्य);
    निश्चित(reply != शून्य);
    निश्चित(reply->replyTCB == शून्य);

    निश्चित(call_ढेर_get_call_ढेर_Ptr(reply->reply_पूर्व) == 0);
    निश्चित(call_ढेर_get_call_ढेर_Ptr(reply->reply_अग्र) == 0);

    /* caller state must be blocked on send or active, so cannot be in a existing call stack */
    निश्चित(thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_BlockedOnSend ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Restart);

    /* callee state must be simple at this point (inactive, restart, or running),
       so it also cannot have a reply reference associated with it. */
    निश्चित(thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Inactive ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Restart);

    /* link caller and reply */
    reply->replyTCB = tcb_caller;
    setThreadStateBlockedOnReply(tcb_caller, reply);

    अगर (sc_donated != शून्य && tcb_callee->tcbSchedContext == शून्य && canDonate) {
        reply_t *old_caller = sc_donated->scReply;

        /* check stack integrity */
        निश्चित(old_caller == शून्य ||
               SC_PTR(call_ढेर_get_call_ढेर_Ptr(old_caller->reply_अग्र)) == sc_donated);

        /* push on to stack */
        अगर (old_caller) {
            old_caller->reply_अग्र = call_ढेर_new(REPLY_REF(reply), false);
        }
        reply->reply_पूर्व = call_ढेर_new(REPLY_REF(old_caller), false);
        sc_donated->scReply = reply;
        reply->reply_अग्र = call_ढेर_new(SC_REF(sc_donated), true);

        /* now do the actual donation */
        schedContext_donate(sc_donated, tcb_callee);
    }
}

/* Pop the head reply from the call stack */
व्योम reply_निकालो(reply_t *reply, tcb_t *tcb)
{
    निश्चित(reply != शून्य);
    निश्चित(reply->replyTCB == tcb);
    निश्चित(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    निश्चित(thread_state_get_replyObject(tcb->tcbState) == REPLY_REF(reply));

    word_t अग्र_ptr = call_ढेर_get_call_ढेर_Ptr(reply->reply_अग्र);
    word_t पूर्व_ptr = call_ढेर_get_call_ढेर_Ptr(reply->reply_पूर्व);

    SC_PTR(अग्र_ptr)->scReply = REPLY_PTR(पूर्व_ptr);
    अगर (पूर्व_ptr != 0) {
        REPLY_PTR(पूर्व_ptr)->reply_अग्र = reply->reply_अग्र;
        निश्चित(call_ढेर_get_is_शीर्ष(REPLY_PTR(पूर्व_ptr)->reply_अग्र));
    }

    reply->reply_अग्र = call_ढेर_new(0, false);

    /* give it back */
    अगर (tcb->tcbSchedContext == शून्य) {
        /* only give the SC back if our SC is NULL. This prevents
         * strange behaviour when a thread is bound to an sc while it is
         * in the BlockedOnReply state. The semantics in this case are that the
         * SC cannot go back to the caller if the caller has received another one */
        schedContext_donate(SC_PTR(अग्र_ptr), tcb);
    }

    reply->reply_पूर्व = call_ढेर_new(0, false);
    reply_unlink(reply, tcb);
}

/* Remove a reply from the middle of the call stack */
व्योम reply_remove(reply_t *reply, tcb_t *tcb)
{
    निश्चित(reply->replyTCB == tcb);
    निश्चित(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    निश्चित(thread_state_get_replyObject(tcb->tcbState) == REPLY_REF(reply));

    word_t अग्र_ptr = call_ढेर_get_call_ढेर_Ptr(reply->reply_अग्र);
    word_t पूर्व_ptr = call_ढेर_get_call_ढेर_Ptr(reply->reply_पूर्व);

    अगर (likely(अग्र_ptr && call_ढेर_get_is_शीर्ष(reply->reply_अग्र))) {
        /* head of the call stack -> just pop */
        reply_निकालो(reply, tcb);
    } अन्यथा {
        अगर (अग्र_ptr) {
            /* not the head, remove from middle - break the chain */
            REPLY_PTR(अग्र_ptr)->reply_पूर्व = call_ढेर_new(0, false);
        }
        अगर (पूर्व_ptr) {
            REPLY_PTR(पूर्व_ptr)->reply_अग्र = call_ढेर_new(0, false);
        }
        reply->reply_पूर्व = call_ढेर_new(0, false);
        reply->reply_अग्र = call_ढेर_new(0, false);
        reply_unlink(reply, tcb);
    }
}

व्योम reply_remove_tcb(tcb_t *tcb)
{
    निश्चित(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    reply_t *reply = REPLY_PTR(thread_state_get_replyObject(tcb->tcbState));
    word_t अग्र_ptr = call_ढेर_get_call_ढेर_Ptr(reply->reply_अग्र);
    word_t पूर्व_ptr = call_ढेर_get_call_ढेर_Ptr(reply->reply_पूर्व);

    अगर (अग्र_ptr) {
        अगर (call_ढेर_get_is_शीर्ष(reply->reply_अग्र)) {
            SC_PTR(अग्र_ptr)->scReply = शून्य;
        } अन्यथा {
            REPLY_PTR(अग्र_ptr)->reply_पूर्व = call_ढेर_new(0, false);
        }
    }

    अगर (पूर्व_ptr) {
        REPLY_PTR(पूर्व_ptr)->reply_अग्र = call_ढेर_new(0, false);
    }

    reply->reply_पूर्व = call_ढेर_new(0, false);
    reply->reply_अग्र = call_ढेर_new(0, false);
    reply_unlink(reply, tcb);
}
