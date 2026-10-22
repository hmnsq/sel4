/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <object/reply.h>

ଶୂନ୍ୟ reply_ଠେଲ(tcb_t *tcb_caller, tcb_t *tcb_callee, reply_t *reply, bool_t canDonate)
{
    sched_context_t *sc_donated = tcb_caller->tcbSchedContext;

    ପ୍ରତିଜ୍ଞା(tcb_caller != ଖାଲି);
    ପ୍ରତିଜ୍ଞା(reply != ଖାଲି);
    ପ୍ରତିଜ୍ଞା(reply->replyTCB == ଖାଲି);

    ପ୍ରତିଜ୍ଞା(call_ଗଦା_get_call_ଗଦା_Ptr(reply->replyPrev) == 0);
    ପ୍ରତିଜ୍ଞା(call_ଗଦା_get_call_ଗଦା_Ptr(reply->replyNext) == 0);

    /* caller state must be blocked on send or active, so cannot be in a existing call stack */
    ପ୍ରତିଜ୍ଞା(thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_BlockedOnSend ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Restart);

    /* callee state must be simple at this point (inactive, restart, or running),
       so it also cannot have a reply reference associated with it. */
    ପ୍ରତିଜ୍ଞା(thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Inactive ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Restart);

    /* link caller and reply */
    reply->replyTCB = tcb_caller;
    setThreadStateBlockedOnReply(tcb_caller, reply);

    ଯଦି (sc_donated != ଖାଲି && tcb_callee->tcbSchedContext == ଖାଲି && canDonate) {
        reply_t *old_caller = sc_donated->scReply;

        /* check stack integrity */
        ପ୍ରତିଜ୍ଞା(old_caller == ଖାଲି ||
               SC_PTR(call_ଗଦା_get_call_ଗଦା_Ptr(old_caller->replyNext)) == sc_donated);

        /* push on to stack */
        ଯଦି (old_caller) {
            old_caller->replyNext = call_ଗଦା_new(REPLY_REF(reply), false);
        }
        reply->replyPrev = call_ଗଦା_new(REPLY_REF(old_caller), false);
        sc_donated->scReply = reply;
        reply->replyNext = call_ଗଦା_new(SC_REF(sc_donated), true);

        /* now do the actual donation */
        schedContext_donate(sc_donated, tcb_callee);
    }
}

/* Pop the head reply from the call stack */
ଶୂନ୍ୟ reply_କାଢ଼(reply_t *reply, tcb_t *tcb)
{
    ପ୍ରତିଜ୍ଞା(reply != ଖାଲି);
    ପ୍ରତିଜ୍ଞା(reply->replyTCB == tcb);
    ପ୍ରତିଜ୍ଞା(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    ପ୍ରତିଜ୍ଞା(thread_state_get_replyObject(tcb->tcbState) == REPLY_REF(reply));

    word_t next_ptr = call_ଗଦା_get_call_ଗଦା_Ptr(reply->replyNext);
    word_t prev_ptr = call_ଗଦା_get_call_ଗଦା_Ptr(reply->replyPrev);

    SC_PTR(next_ptr)->scReply = REPLY_PTR(prev_ptr);
    ଯଦି (prev_ptr != 0) {
        REPLY_PTR(prev_ptr)->replyNext = reply->replyNext;
        ପ୍ରତିଜ୍ଞା(call_ଗଦା_get_is_Head(REPLY_PTR(prev_ptr)->replyNext));
    }

    reply->replyNext = call_ଗଦା_new(0, false);

    /* give it back */
    ଯଦି (tcb->tcbSchedContext == ଖାଲି) {
        /* only give the SC back if our SC is NULL. This prevents
         * strange behaviour when a thread is bound to an sc while it is
         * in the BlockedOnReply state. The semantics in this case are that the
         * SC cannot go back to the caller if the caller has received another one */
        schedContext_donate(SC_PTR(next_ptr), tcb);
    }

    reply->replyPrev = call_ଗଦା_new(0, false);
    reply_unlink(reply, tcb);
}

/* Remove a reply from the middle of the call stack */
ଶୂନ୍ୟ reply_remove(reply_t *reply, tcb_t *tcb)
{
    ପ୍ରତିଜ୍ଞା(reply->replyTCB == tcb);
    ପ୍ରତିଜ୍ଞା(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    ପ୍ରତିଜ୍ଞା(thread_state_get_replyObject(tcb->tcbState) == REPLY_REF(reply));

    word_t next_ptr = call_ଗଦା_get_call_ଗଦା_Ptr(reply->replyNext);
    word_t prev_ptr = call_ଗଦା_get_call_ଗଦା_Ptr(reply->replyPrev);

    ଯଦି (likely(next_ptr && call_ଗଦା_get_is_Head(reply->replyNext))) {
        /* head of the call stack -> just pop */
        reply_କାଢ଼(reply, tcb);
    } ଅନ୍ୟଥା {
        ଯଦି (next_ptr) {
            /* not the head, remove from middle - break the chain */
            REPLY_PTR(next_ptr)->replyPrev = call_ଗଦା_new(0, false);
        }
        ଯଦି (prev_ptr) {
            REPLY_PTR(prev_ptr)->replyNext = call_ଗଦା_new(0, false);
        }
        reply->replyPrev = call_ଗଦା_new(0, false);
        reply->replyNext = call_ଗଦା_new(0, false);
        reply_unlink(reply, tcb);
    }
}

ଶୂନ୍ୟ reply_remove_tcb(tcb_t *tcb)
{
    ପ୍ରତିଜ୍ଞା(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    reply_t *reply = REPLY_PTR(thread_state_get_replyObject(tcb->tcbState));
    word_t next_ptr = call_ଗଦା_get_call_ଗଦା_Ptr(reply->replyNext);
    word_t prev_ptr = call_ଗଦା_get_call_ଗଦା_Ptr(reply->replyPrev);

    ଯଦି (next_ptr) {
        ଯଦି (call_ଗଦା_get_is_Head(reply->replyNext)) {
            SC_PTR(next_ptr)->scReply = ଖାଲି;
        } ଅନ୍ୟଥା {
            REPLY_PTR(next_ptr)->replyPrev = call_ଗଦା_new(0, false);
        }
    }

    ଯଦି (prev_ptr) {
        REPLY_PTR(prev_ptr)->replyNext = call_ଗଦା_new(0, false);
    }

    reply->replyPrev = call_ଗଦା_new(0, false);
    reply->replyNext = call_ଗଦା_new(0, false);
    reply_unlink(reply, tcb);
}
