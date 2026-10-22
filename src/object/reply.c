/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <object/reply.h>

శూన్య జవాబు_నెట్టు(tcb_t *tcb_caller, tcb_t *tcb_callee, జవాబు_t *జవాబు, bool_t canDonate)
{
    sched_context_t *sc_donated = tcb_caller->tcbSchedContext;

    ప్రతిజ్ఞా(tcb_caller != ఖాళీ);
    ప్రతిజ్ఞా(జవాబు != ఖాళీ);
    ప్రతిజ్ఞా(జవాబు->జవాబు_TCB == ఖాళీ);

    ప్రతిజ్ఞా(call_దొంతర_get_call_దొంతర_Ptr(జవాబు->జవాబు_Prev) == 0);
    ప్రతిజ్ఞా(call_దొంతర_get_call_దొంతర_Ptr(జవాబు->జవాబు_తదుపరి) == 0);

    /* caller state must be blocked on send or active, so cannot be in a existing call stack */
    ప్రతిజ్ఞా(thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_BlockedOnSend ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Restart);

    /* callee state must be simple at this point (inactive, restart, or running),
       so it also cannot have a reply reference associated with it. */
    ప్రతిజ్ఞా(thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Inactive ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Restart);

    /* link caller and reply */
    జవాబు->జవాబు_TCB = tcb_caller;
    సమితి_Thread_State_Blocked_On_జవాబు(tcb_caller, జవాబు);

    ఒకవేళ (sc_donated != ఖాళీ && tcb_callee->tcbSchedContext == ఖాళీ && canDonate) {
        జవాబు_t *old_caller = sc_donated->sc_జవాబు;

        /* check stack integrity */
        ప్రతిజ్ఞా(old_caller == ఖాళీ ||
               SC_PTR(call_దొంతర_get_call_దొంతర_Ptr(old_caller->జవాబు_తదుపరి)) == sc_donated);

        /* push on to stack */
        ఒకవేళ (old_caller) {
            old_caller->జవాబు_తదుపరి = call_దొంతర_new(జవాబు_REF(జవాబు), false);
        }
        జవాబు->జవాబు_Prev = call_దొంతర_new(జవాబు_REF(old_caller), false);
        sc_donated->sc_జవాబు = జవాబు;
        జవాబు->జవాబు_తదుపరి = call_దొంతర_new(SC_REF(sc_donated), true);

        /* now do the actual donation */
        schedContext_donate(sc_donated, tcb_callee);
    }
}

/* Pop the head reply from the call stack */
శూన్య జవాబు_తీయి(జవాబు_t *జవాబు, tcb_t *tcb)
{
    ప్రతిజ్ఞా(జవాబు != ఖాళీ);
    ప్రతిజ్ఞా(జవాబు->జవాబు_TCB == tcb);
    ప్రతిజ్ఞా(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_జవాబు);
    ప్రతిజ్ఞా(thread_state_get_జవాబు_Object(tcb->tcbState) == జవాబు_REF(జవాబు));

    మాట_t తదుపరి_ptr = call_దొంతర_get_call_దొంతర_Ptr(జవాబు->జవాబు_తదుపరి);
    మాట_t prev_ptr = call_దొంతర_get_call_దొంతర_Ptr(జవాబు->జవాబు_Prev);

    SC_PTR(తదుపరి_ptr)->sc_జవాబు = జవాబు_PTR(prev_ptr);
    ఒకవేళ (prev_ptr != 0) {
        జవాబు_PTR(prev_ptr)->జవాబు_తదుపరి = జవాబు->జవాబు_తదుపరి;
        ప్రతిజ్ఞా(call_దొంతర_get_is_తల(జవాబు_PTR(prev_ptr)->జవాబు_తదుపరి));
    }

    జవాబు->జవాబు_తదుపరి = call_దొంతర_new(0, false);

    /* give it back */
    ఒకవేళ (tcb->tcbSchedContext == ఖాళీ) {
        /* only give the SC back if our SC is NULL. This prevents
         * strange behaviour when a thread is bound to an sc while it is
         * in the BlockedOnReply state. The semantics in this case are that the
         * SC cannot go back to the caller if the caller has received another one */
        schedContext_donate(SC_PTR(తదుపరి_ptr), tcb);
    }

    జవాబు->జవాబు_Prev = call_దొంతర_new(0, false);
    జవాబు_unlink(జవాబు, tcb);
}

/* Remove a reply from the middle of the call stack */
శూన్య జవాబు_remove(జవాబు_t *జవాబు, tcb_t *tcb)
{
    ప్రతిజ్ఞా(జవాబు->జవాబు_TCB == tcb);
    ప్రతిజ్ఞా(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_జవాబు);
    ప్రతిజ్ఞా(thread_state_get_జవాబు_Object(tcb->tcbState) == జవాబు_REF(జవాబు));

    మాట_t తదుపరి_ptr = call_దొంతర_get_call_దొంతర_Ptr(జవాబు->జవాబు_తదుపరి);
    మాట_t prev_ptr = call_దొంతర_get_call_దొంతర_Ptr(జవాబు->జవాబు_Prev);

    ఒకవేళ (likely(తదుపరి_ptr && call_దొంతర_get_is_తల(జవాబు->జవాబు_తదుపరి))) {
        /* head of the call stack -> just pop */
        జవాబు_తీయి(జవాబు, tcb);
    } లేకపోతే {
        ఒకవేళ (తదుపరి_ptr) {
            /* not the head, remove from middle - break the chain */
            జవాబు_PTR(తదుపరి_ptr)->జవాబు_Prev = call_దొంతర_new(0, false);
        }
        ఒకవేళ (prev_ptr) {
            జవాబు_PTR(prev_ptr)->జవాబు_తదుపరి = call_దొంతర_new(0, false);
        }
        జవాబు->జవాబు_Prev = call_దొంతర_new(0, false);
        జవాబు->జవాబు_తదుపరి = call_దొంతర_new(0, false);
        జవాబు_unlink(జవాబు, tcb);
    }
}

శూన్య జవాబు_remove_tcb(tcb_t *tcb)
{
    ప్రతిజ్ఞా(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_జవాబు);
    జవాబు_t *జవాబు = జవాబు_PTR(thread_state_get_జవాబు_Object(tcb->tcbState));
    మాట_t తదుపరి_ptr = call_దొంతర_get_call_దొంతర_Ptr(జవాబు->జవాబు_తదుపరి);
    మాట_t prev_ptr = call_దొంతర_get_call_దొంతర_Ptr(జవాబు->జవాబు_Prev);

    ఒకవేళ (తదుపరి_ptr) {
        ఒకవేళ (call_దొంతర_get_is_తల(జవాబు->జవాబు_తదుపరి)) {
            SC_PTR(తదుపరి_ptr)->sc_జవాబు = ఖాళీ;
        } లేకపోతే {
            జవాబు_PTR(తదుపరి_ptr)->జవాబు_Prev = call_దొంతర_new(0, false);
        }
    }

    ఒకవేళ (prev_ptr) {
        జవాబు_PTR(prev_ptr)->జవాబు_తదుపరి = call_దొంతర_new(0, false);
    }

    జవాబు->జవాబు_Prev = call_దొంతర_new(0, false);
    జవాబు->జవాబు_తదుపరి = call_దొంతర_new(0, false);
    జవాబు_unlink(జవాబు, tcb);
}
