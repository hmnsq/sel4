/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <object/reply.h>

શૂન્ય reply_ધકેલો(tcb_t *tcb_caller, tcb_t *tcb_callee, reply_t *reply, bool_t canDonate)
{
    sched_context_t *sc_donated = tcb_caller->tcbSchedContext;

    પ્રતિજ્ઞા(tcb_caller != ખાલી);
    પ્રતિજ્ઞા(reply != ખાલી);
    પ્રતિજ્ઞા(reply->replyTCB == ખાલી);

    પ્રતિજ્ઞા(call_ઢગલો_get_call_ઢગલો_Ptr(reply->replyPrev) == 0);
    પ્રતિજ્ઞા(call_ઢગલો_get_call_ઢગલો_Ptr(reply->replyNext) == 0);

    /* caller state must be blocked on send or active, so cannot be in a existing call stack */
    પ્રતિજ્ઞા(thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_BlockedOnSend ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Restart);

    /* callee state must be simple at this point (inactive, restart, or running),
       so it also cannot have a reply reference associated with it. */
    પ્રતિજ્ઞા(thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Inactive ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Restart);

    /* link caller and reply */
    reply->replyTCB = tcb_caller;
    setThreadStateBlockedOnReply(tcb_caller, reply);

    જો (sc_donated != ખાલી && tcb_callee->tcbSchedContext == ખાલી && canDonate) {
        reply_t *old_caller = sc_donated->scReply;

        /* check stack integrity */
        પ્રતિજ્ઞા(old_caller == ખાલી ||
               SC_PTR(call_ઢગલો_get_call_ઢગલો_Ptr(old_caller->replyNext)) == sc_donated);

        /* push on to stack */
        જો (old_caller) {
            old_caller->replyNext = call_ઢગલો_new(REPLY_REF(reply), false);
        }
        reply->replyPrev = call_ઢગલો_new(REPLY_REF(old_caller), false);
        sc_donated->scReply = reply;
        reply->replyNext = call_ઢગલો_new(SC_REF(sc_donated), true);

        /* now do the actual donation */
        schedContext_donate(sc_donated, tcb_callee);
    }
}

/* Pop the head reply from the call stack */
શૂન્ય reply_કાઢો(reply_t *reply, tcb_t *tcb)
{
    પ્રતિજ્ઞા(reply != ખાલી);
    પ્રતિજ્ઞા(reply->replyTCB == tcb);
    પ્રતિજ્ઞા(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    પ્રતિજ્ઞા(thread_state_get_replyObject(tcb->tcbState) == REPLY_REF(reply));

    word_t next_ptr = call_ઢગલો_get_call_ઢગલો_Ptr(reply->replyNext);
    word_t prev_ptr = call_ઢગલો_get_call_ઢગલો_Ptr(reply->replyPrev);

    SC_PTR(next_ptr)->scReply = REPLY_PTR(prev_ptr);
    જો (prev_ptr != 0) {
        REPLY_PTR(prev_ptr)->replyNext = reply->replyNext;
        પ્રતિજ્ઞા(call_ઢગલો_get_is_Head(REPLY_PTR(prev_ptr)->replyNext));
    }

    reply->replyNext = call_ઢગલો_new(0, false);

    /* give it back */
    જો (tcb->tcbSchedContext == ખાલી) {
        /* only give the SC back if our SC is NULL. This prevents
         * strange behaviour when a thread is bound to an sc while it is
         * in the BlockedOnReply state. The semantics in this case are that the
         * SC cannot go back to the caller if the caller has received another one */
        schedContext_donate(SC_PTR(next_ptr), tcb);
    }

    reply->replyPrev = call_ઢગલો_new(0, false);
    reply_unlink(reply, tcb);
}

/* Remove a reply from the middle of the call stack */
શૂન્ય reply_remove(reply_t *reply, tcb_t *tcb)
{
    પ્રતિજ્ઞા(reply->replyTCB == tcb);
    પ્રતિજ્ઞા(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    પ્રતિજ્ઞા(thread_state_get_replyObject(tcb->tcbState) == REPLY_REF(reply));

    word_t next_ptr = call_ઢગલો_get_call_ઢગલો_Ptr(reply->replyNext);
    word_t prev_ptr = call_ઢગલો_get_call_ઢગલો_Ptr(reply->replyPrev);

    જો (likely(next_ptr && call_ઢગલો_get_is_Head(reply->replyNext))) {
        /* head of the call stack -> just pop */
        reply_કાઢો(reply, tcb);
    } નહીંતર {
        જો (next_ptr) {
            /* not the head, remove from middle - break the chain */
            REPLY_PTR(next_ptr)->replyPrev = call_ઢગલો_new(0, false);
        }
        જો (prev_ptr) {
            REPLY_PTR(prev_ptr)->replyNext = call_ઢગલો_new(0, false);
        }
        reply->replyPrev = call_ઢગલો_new(0, false);
        reply->replyNext = call_ઢગલો_new(0, false);
        reply_unlink(reply, tcb);
    }
}

શૂન્ય reply_remove_tcb(tcb_t *tcb)
{
    પ્રતિજ્ઞા(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    reply_t *reply = REPLY_PTR(thread_state_get_replyObject(tcb->tcbState));
    word_t next_ptr = call_ઢગલો_get_call_ઢગલો_Ptr(reply->replyNext);
    word_t prev_ptr = call_ઢગલો_get_call_ઢગલો_Ptr(reply->replyPrev);

    જો (next_ptr) {
        જો (call_ઢગલો_get_is_Head(reply->replyNext)) {
            SC_PTR(next_ptr)->scReply = ખાલી;
        } નહીંતર {
            REPLY_PTR(next_ptr)->replyPrev = call_ઢગલો_new(0, false);
        }
    }

    જો (prev_ptr) {
        REPLY_PTR(prev_ptr)->replyNext = call_ઢગલો_new(0, false);
    }

    reply->replyPrev = call_ઢગલો_new(0, false);
    reply->replyNext = call_ઢગલો_new(0, false);
    reply_unlink(reply, tcb);
}
