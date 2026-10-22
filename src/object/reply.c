/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <object/reply.h>

शून्य reply_ढकला(tcb_t *tcb_caller, tcb_t *tcb_callee, reply_t *reply, bool_t canDonate)
{
    sched_context_t *sc_donated = tcb_caller->tcbSchedContext;

    प्रतिज्ञा(tcb_caller != रिक्त);
    प्रतिज्ञा(reply != रिक्त);
    प्रतिज्ञा(reply->replyTCB == रिक्त);

    प्रतिज्ञा(call_चळत_get_call_चळत_Ptr(reply->replyPrev) == 0);
    प्रतिज्ञा(call_चळत_get_call_चळत_Ptr(reply->replyNext) == 0);

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
    reply->replyTCB = tcb_caller;
    setThreadStateBlockedOnReply(tcb_caller, reply);

    जर (sc_donated != रिक्त && tcb_callee->tcbSchedContext == रिक्त && canDonate) {
        reply_t *old_caller = sc_donated->scReply;

        /* check stack integrity */
        प्रतिज्ञा(old_caller == रिक्त ||
               SC_PTR(call_चळत_get_call_चळत_Ptr(old_caller->replyNext)) == sc_donated);

        /* push on to stack */
        जर (old_caller) {
            old_caller->replyNext = call_चळत_new(REPLY_REF(reply), false);
        }
        reply->replyPrev = call_चळत_new(REPLY_REF(old_caller), false);
        sc_donated->scReply = reply;
        reply->replyNext = call_चळत_new(SC_REF(sc_donated), true);

        /* now do the actual donation */
        schedContext_donate(sc_donated, tcb_callee);
    }
}

/* Pop the head reply from the call stack */
शून्य reply_काढा(reply_t *reply, tcb_t *tcb)
{
    प्रतिज्ञा(reply != रिक्त);
    प्रतिज्ञा(reply->replyTCB == tcb);
    प्रतिज्ञा(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    प्रतिज्ञा(thread_state_get_replyObject(tcb->tcbState) == REPLY_REF(reply));

    word_t next_ptr = call_चळत_get_call_चळत_Ptr(reply->replyNext);
    word_t prev_ptr = call_चळत_get_call_चळत_Ptr(reply->replyPrev);

    SC_PTR(next_ptr)->scReply = REPLY_PTR(prev_ptr);
    जर (prev_ptr != 0) {
        REPLY_PTR(prev_ptr)->replyNext = reply->replyNext;
        प्रतिज्ञा(call_चळत_get_is_Head(REPLY_PTR(prev_ptr)->replyNext));
    }

    reply->replyNext = call_चळत_new(0, false);

    /* give it back */
    जर (tcb->tcbSchedContext == रिक्त) {
        /* only give the SC back if our SC is NULL. This prevents
         * strange behaviour when a thread is bound to an sc while it is
         * in the BlockedOnReply state. The semantics in this case are that the
         * SC cannot go back to the caller if the caller has received another one */
        schedContext_donate(SC_PTR(next_ptr), tcb);
    }

    reply->replyPrev = call_चळत_new(0, false);
    reply_unlink(reply, tcb);
}

/* Remove a reply from the middle of the call stack */
शून्य reply_remove(reply_t *reply, tcb_t *tcb)
{
    प्रतिज्ञा(reply->replyTCB == tcb);
    प्रतिज्ञा(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    प्रतिज्ञा(thread_state_get_replyObject(tcb->tcbState) == REPLY_REF(reply));

    word_t next_ptr = call_चळत_get_call_चळत_Ptr(reply->replyNext);
    word_t prev_ptr = call_चळत_get_call_चळत_Ptr(reply->replyPrev);

    जर (likely(next_ptr && call_चळत_get_is_Head(reply->replyNext))) {
        /* head of the call stack -> just pop */
        reply_काढा(reply, tcb);
    } अन्यथा {
        जर (next_ptr) {
            /* not the head, remove from middle - break the chain */
            REPLY_PTR(next_ptr)->replyPrev = call_चळत_new(0, false);
        }
        जर (prev_ptr) {
            REPLY_PTR(prev_ptr)->replyNext = call_चळत_new(0, false);
        }
        reply->replyPrev = call_चळत_new(0, false);
        reply->replyNext = call_चळत_new(0, false);
        reply_unlink(reply, tcb);
    }
}

शून्य reply_remove_tcb(tcb_t *tcb)
{
    प्रतिज्ञा(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    reply_t *reply = REPLY_PTR(thread_state_get_replyObject(tcb->tcbState));
    word_t next_ptr = call_चळत_get_call_चळत_Ptr(reply->replyNext);
    word_t prev_ptr = call_चळत_get_call_चळत_Ptr(reply->replyPrev);

    जर (next_ptr) {
        जर (call_चळत_get_is_Head(reply->replyNext)) {
            SC_PTR(next_ptr)->scReply = रिक्त;
        } अन्यथा {
            REPLY_PTR(next_ptr)->replyPrev = call_चळत_new(0, false);
        }
    }

    जर (prev_ptr) {
        REPLY_PTR(prev_ptr)->replyNext = call_चळत_new(0, false);
    }

    reply->replyPrev = call_चळत_new(0, false);
    reply->replyNext = call_चळत_new(0, false);
    reply_unlink(reply, tcb);
}
