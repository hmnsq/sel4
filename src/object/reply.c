/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <object/reply.h>

শূন্য reply_ঠেলো(tcb_t *tcb_caller, tcb_t *tcb_callee, reply_t *reply, bool_t canDonate)
{
    sched_context_t *sc_donated = tcb_caller->tcbSchedContext;

    প্রতিজ্ঞা(tcb_caller != ফাঁকা);
    প্রতিজ্ঞা(reply != ফাঁকা);
    প্রতিজ্ঞা(reply->replyTCB == ফাঁকা);

    প্রতিজ্ঞা(call_স্তূপ_get_call_স্তূপ_Ptr(reply->reply_পূর্ব) == 0);
    প্রতিজ্ঞা(call_স্তূপ_get_call_স্তূপ_Ptr(reply->reply_পরবর্তী) == 0);

    /* caller state must be blocked on send or active, so cannot be in a existing call stack */
    প্রতিজ্ঞা(thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_BlockedOnSend ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Restart);

    /* callee state must be simple at this point (inactive, restart, or running),
       so it also cannot have a reply reference associated with it. */
    প্রতিজ্ঞা(thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Inactive ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Restart);

    /* link caller and reply */
    reply->replyTCB = tcb_caller;
    setThreadStateBlockedOnReply(tcb_caller, reply);

    যদি (sc_donated != ফাঁকা && tcb_callee->tcbSchedContext == ফাঁকা && canDonate) {
        reply_t *old_caller = sc_donated->scReply;

        /* check stack integrity */
        প্রতিজ্ঞা(old_caller == ফাঁকা ||
               SC_PTR(call_স্তূপ_get_call_স্তূপ_Ptr(old_caller->reply_পরবর্তী)) == sc_donated);

        /* push on to stack */
        যদি (old_caller) {
            old_caller->reply_পরবর্তী = call_স্তূপ_new(REPLY_REF(reply), false);
        }
        reply->reply_পূর্ব = call_স্তূপ_new(REPLY_REF(old_caller), false);
        sc_donated->scReply = reply;
        reply->reply_পরবর্তী = call_স্তূপ_new(SC_REF(sc_donated), true);

        /* now do the actual donation */
        schedContext_donate(sc_donated, tcb_callee);
    }
}

/* Pop the head reply from the call stack */
শূন্য reply_বের_করো(reply_t *reply, tcb_t *tcb)
{
    প্রতিজ্ঞা(reply != ফাঁকা);
    প্রতিজ্ঞা(reply->replyTCB == tcb);
    প্রতিজ্ঞা(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    প্রতিজ্ঞা(thread_state_get_replyObject(tcb->tcbState) == REPLY_REF(reply));

    word_t পরবর্তী_ptr = call_স্তূপ_get_call_স্তূপ_Ptr(reply->reply_পরবর্তী);
    word_t পূর্ব_ptr = call_স্তূপ_get_call_স্তূপ_Ptr(reply->reply_পূর্ব);

    SC_PTR(পরবর্তী_ptr)->scReply = REPLY_PTR(পূর্ব_ptr);
    যদি (পূর্ব_ptr != 0) {
        REPLY_PTR(পূর্ব_ptr)->reply_পরবর্তী = reply->reply_পরবর্তী;
        প্রতিজ্ঞা(call_স্তূপ_get_is_শীর্ষ(REPLY_PTR(পূর্ব_ptr)->reply_পরবর্তী));
    }

    reply->reply_পরবর্তী = call_স্তূপ_new(0, false);

    /* give it back */
    যদি (tcb->tcbSchedContext == ফাঁকা) {
        /* only give the SC back if our SC is NULL. This prevents
         * strange behaviour when a thread is bound to an sc while it is
         * in the BlockedOnReply state. The semantics in this case are that the
         * SC cannot go back to the caller if the caller has received another one */
        schedContext_donate(SC_PTR(পরবর্তী_ptr), tcb);
    }

    reply->reply_পূর্ব = call_স্তূপ_new(0, false);
    reply_unlink(reply, tcb);
}

/* Remove a reply from the middle of the call stack */
শূন্য reply_remove(reply_t *reply, tcb_t *tcb)
{
    প্রতিজ্ঞা(reply->replyTCB == tcb);
    প্রতিজ্ঞা(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    প্রতিজ্ঞা(thread_state_get_replyObject(tcb->tcbState) == REPLY_REF(reply));

    word_t পরবর্তী_ptr = call_স্তূপ_get_call_স্তূপ_Ptr(reply->reply_পরবর্তী);
    word_t পূর্ব_ptr = call_স্তূপ_get_call_স্তূপ_Ptr(reply->reply_পূর্ব);

    যদি (likely(পরবর্তী_ptr && call_স্তূপ_get_is_শীর্ষ(reply->reply_পরবর্তী))) {
        /* head of the call stack -> just pop */
        reply_বের_করো(reply, tcb);
    } অন্যথা {
        যদি (পরবর্তী_ptr) {
            /* not the head, remove from middle - break the chain */
            REPLY_PTR(পরবর্তী_ptr)->reply_পূর্ব = call_স্তূপ_new(0, false);
        }
        যদি (পূর্ব_ptr) {
            REPLY_PTR(পূর্ব_ptr)->reply_পরবর্তী = call_স্তূপ_new(0, false);
        }
        reply->reply_পূর্ব = call_স্তূপ_new(0, false);
        reply->reply_পরবর্তী = call_স্তূপ_new(0, false);
        reply_unlink(reply, tcb);
    }
}

শূন্য reply_remove_tcb(tcb_t *tcb)
{
    প্রতিজ্ঞা(thread_state_get_tsType(tcb->tcbState) == ThreadState_BlockedOnReply);
    reply_t *reply = REPLY_PTR(thread_state_get_replyObject(tcb->tcbState));
    word_t পরবর্তী_ptr = call_স্তূপ_get_call_স্তূপ_Ptr(reply->reply_পরবর্তী);
    word_t পূর্ব_ptr = call_স্তূপ_get_call_স্তূপ_Ptr(reply->reply_পূর্ব);

    যদি (পরবর্তী_ptr) {
        যদি (call_স্তূপ_get_is_শীর্ষ(reply->reply_পরবর্তী)) {
            SC_PTR(পরবর্তী_ptr)->scReply = ফাঁকা;
        } অন্যথা {
            REPLY_PTR(পরবর্তী_ptr)->reply_পূর্ব = call_স্তূপ_new(0, false);
        }
    }

    যদি (পূর্ব_ptr) {
        REPLY_PTR(পূর্ব_ptr)->reply_পরবর্তী = call_স্তূপ_new(0, false);
    }

    reply->reply_পূর্ব = call_স্তূপ_new(0, false);
    reply->reply_পরবর্তী = call_স্তূপ_new(0, false);
    reply_unlink(reply, tcb);
}
