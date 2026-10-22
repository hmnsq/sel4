/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <object/reply.h>

வெறுமை reply_தள்ளு(tcb_t *tcb_caller, tcb_t *tcb_callee, reply_t *reply, bool_t canDonate)
{
    sched_context_t *sc_donated = tcb_caller->tcbSchedContext;

    ப்ரதிஜ்ஞா(tcb_caller != வெற்று);
    ப்ரதிஜ்ஞா(reply != வெற்று);
    ப்ரதிஜ்ஞா(reply->replyTCB == வெற்று);

    ப்ரதிஜ்ஞா(call_அடுக்கு_get_call_அடுக்கு_Ptr(reply->replyPrev) == 0);
    ப்ரதிஜ்ஞா(call_அடுக்கு_get_call_அடுக்கு_Ptr(reply->replyNext) == 0);

    /* caller state must be blocked on send or active, so cannot be in a existing call stack */
    ப்ரதிஜ்ஞா(thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_BlockedOnSend ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Restart);

    /* callee state must be simple at this point (inactive, restart, or running),
       so it also cannot have a reply reference associated with it. */
    ப்ரதிஜ்ஞா(thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Inactive ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Restart);

    /* link caller and reply */
    reply->replyTCB = tcb_caller;
    setThreadStateBlockedOnReply(tcb_caller, reply);

    எனில் (sc_donated != வெற்று && tcb_callee->tcbSchedContext == வெற்று && canDonate) {
        reply_t *old_caller = sc_donated->scReply;

        /* check stack integrity */
        ப்ரதிஜ்ஞா(old_caller == வெற்று ||
               SC_PTR(call_அடுக்கு_get_call_அடுக்கு_Ptr(old_caller->replyNext)) == sc_donated);

        /* push on to stack */
        எனில் (old_caller) {
            old_caller->replyNext = call_அடுக்கு_new(REPLY_REF(reply), false);
        }
        reply->replyPrev = call_அடுக்கு_new(REPLY_REF(old_caller), false);
        sc_donated->scReply = reply;
        reply->replyNext = call_அடுக்கு_new(SC_REF(sc_donated), true);

        /* now do the actual donation */
        schedContext_donate(sc_donated, tcb_callee);
    }
}

/* Pop the head reply from the call stack */
வெறுமை reply_எடு(reply_t *reply, tcb_t *tcb)
{
    ப்ரதிஜ்ஞா(reply != வெற்று);
    ப்ரதிஜ்ஞா(reply->replyTCB == tcb);
    ப்ரதிஜ்ஞா(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    ப்ரதிஜ்ஞா(thread_state_get_replyObject(tcb->tcbState) == REPLY_REF(reply));

    word_t next_ptr = call_அடுக்கு_get_call_அடுக்கு_Ptr(reply->replyNext);
    word_t prev_ptr = call_அடுக்கு_get_call_அடுக்கு_Ptr(reply->replyPrev);

    SC_PTR(next_ptr)->scReply = REPLY_PTR(prev_ptr);
    எனில் (prev_ptr != 0) {
        REPLY_PTR(prev_ptr)->replyNext = reply->replyNext;
        ப்ரதிஜ்ஞா(call_அடுக்கு_get_is_Head(REPLY_PTR(prev_ptr)->replyNext));
    }

    reply->replyNext = call_அடுக்கு_new(0, false);

    /* give it back */
    எனில் (tcb->tcbSchedContext == வெற்று) {
        /* only give the SC back if our SC is NULL. This prevents
         * strange behaviour when a thread is bound to an sc while it is
         * in the BlockedOnReply state. The semantics in this case are that the
         * SC cannot go back to the caller if the caller has received another one */
        schedContext_donate(SC_PTR(next_ptr), tcb);
    }

    reply->replyPrev = call_அடுக்கு_new(0, false);
    reply_unlink(reply, tcb);
}

/* Remove a reply from the middle of the call stack */
வெறுமை reply_remove(reply_t *reply, tcb_t *tcb)
{
    ப்ரதிஜ்ஞா(reply->replyTCB == tcb);
    ப்ரதிஜ்ஞா(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    ப்ரதிஜ்ஞா(thread_state_get_replyObject(tcb->tcbState) == REPLY_REF(reply));

    word_t next_ptr = call_அடுக்கு_get_call_அடுக்கு_Ptr(reply->replyNext);
    word_t prev_ptr = call_அடுக்கு_get_call_அடுக்கு_Ptr(reply->replyPrev);

    எனில் (likely(next_ptr && call_அடுக்கு_get_is_Head(reply->replyNext))) {
        /* head of the call stack -> just pop */
        reply_எடு(reply, tcb);
    } இல்லையெனில் {
        எனில் (next_ptr) {
            /* not the head, remove from middle - break the chain */
            REPLY_PTR(next_ptr)->replyPrev = call_அடுக்கு_new(0, false);
        }
        எனில் (prev_ptr) {
            REPLY_PTR(prev_ptr)->replyNext = call_அடுக்கு_new(0, false);
        }
        reply->replyPrev = call_அடுக்கு_new(0, false);
        reply->replyNext = call_அடுக்கு_new(0, false);
        reply_unlink(reply, tcb);
    }
}

வெறுமை reply_remove_tcb(tcb_t *tcb)
{
    ப்ரதிஜ்ஞா(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    reply_t *reply = REPLY_PTR(thread_state_get_replyObject(tcb->tcbState));
    word_t next_ptr = call_அடுக்கு_get_call_அடுக்கு_Ptr(reply->replyNext);
    word_t prev_ptr = call_அடுக்கு_get_call_அடுக்கு_Ptr(reply->replyPrev);

    எனில் (next_ptr) {
        எனில் (call_அடுக்கு_get_is_Head(reply->replyNext)) {
            SC_PTR(next_ptr)->scReply = வெற்று;
        } இல்லையெனில் {
            REPLY_PTR(next_ptr)->replyPrev = call_அடுக்கு_new(0, false);
        }
    }

    எனில் (prev_ptr) {
        REPLY_PTR(prev_ptr)->replyNext = call_அடுக்கு_new(0, false);
    }

    reply->replyPrev = call_அடுக்கு_new(0, false);
    reply->replyNext = call_அடுக்கு_new(0, false);
    reply_unlink(reply, tcb);
}
