/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <object/reply.h>

ਖਾਲੀ ਜਵਾਬ_ਧੱਕੋ(tcb_t *tcb_caller, tcb_t *tcb_callee, ਜਵਾਬ_t *ਜਵਾਬ, bool_t canDonate)
{
    sched_context_t *sc_donated = tcb_caller->tcbSchedContext;

    ਪ੍ਰਤਿਜ੍ਞਾ(tcb_caller != ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(ਜਵਾਬ != ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(ਜਵਾਬ->ਜਵਾਬ_TCB == ਨਿਰਾ);

    ਪ੍ਰਤਿਜ੍ਞਾ(call_ਢੇਰ_get_call_ਢੇਰ_Ptr(ਜਵਾਬ->ਜਵਾਬ_Prev) == 0);
    ਪ੍ਰਤਿਜ੍ਞਾ(call_ਢੇਰ_get_call_ਢੇਰ_Ptr(ਜਵਾਬ->ਜਵਾਬ_ਅਗਲਾ) == 0);

    /* caller state must be blocked on send or active, so cannot be in a existing call stack */
    ਪ੍ਰਤਿਜ੍ਞਾ(thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_BlockedOnSend ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Restart);

    /* callee state must be simple at this point (inactive, restart, or running),
       so it also cannot have a reply reference associated with it. */
    ਪ੍ਰਤਿਜ੍ਞਾ(thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Inactive ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Restart);

    /* link caller and reply */
    ਜਵਾਬ->ਜਵਾਬ_TCB = tcb_caller;
    ਸੈੱਟ_Thread_State_Blocked_On_ਜਵਾਬ(tcb_caller, ਜਵਾਬ);

    ਜੇ (sc_donated != ਨਿਰਾ && tcb_callee->tcbSchedContext == ਨਿਰਾ && canDonate) {
        ਜਵਾਬ_t *old_caller = sc_donated->sc_ਜਵਾਬ;

        /* check stack integrity */
        ਪ੍ਰਤਿਜ੍ਞਾ(old_caller == ਨਿਰਾ ||
               SC_PTR(call_ਢੇਰ_get_call_ਢੇਰ_Ptr(old_caller->ਜਵਾਬ_ਅਗਲਾ)) == sc_donated);

        /* push on to stack */
        ਜੇ (old_caller) {
            old_caller->ਜਵਾਬ_ਅਗਲਾ = call_ਢੇਰ_new(ਜਵਾਬ_REF(ਜਵਾਬ), false);
        }
        ਜਵਾਬ->ਜਵਾਬ_Prev = call_ਢੇਰ_new(ਜਵਾਬ_REF(old_caller), false);
        sc_donated->sc_ਜਵਾਬ = ਜਵਾਬ;
        ਜਵਾਬ->ਜਵਾਬ_ਅਗਲਾ = call_ਢੇਰ_new(SC_REF(sc_donated), true);

        /* now do the actual donation */
        schedContext_donate(sc_donated, tcb_callee);
    }
}

/* Pop the head reply from the call stack */
ਖਾਲੀ ਜਵਾਬ_ਕੱਢੋ(ਜਵਾਬ_t *ਜਵਾਬ, tcb_t *tcb)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(ਜਵਾਬ != ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(ਜਵਾਬ->ਜਵਾਬ_TCB == tcb);
    ਪ੍ਰਤਿਜ੍ਞਾ(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_ਜਵਾਬ);
    ਪ੍ਰਤਿਜ੍ਞਾ(thread_state_get_ਜਵਾਬ_Object(tcb->tcbState) == ਜਵਾਬ_REF(ਜਵਾਬ));

    ਸ਼ਬਦ_t ਅਗਲਾ_ptr = call_ਢੇਰ_get_call_ਢੇਰ_Ptr(ਜਵਾਬ->ਜਵਾਬ_ਅਗਲਾ);
    ਸ਼ਬਦ_t prev_ptr = call_ਢੇਰ_get_call_ਢੇਰ_Ptr(ਜਵਾਬ->ਜਵਾਬ_Prev);

    SC_PTR(ਅਗਲਾ_ptr)->sc_ਜਵਾਬ = ਜਵਾਬ_PTR(prev_ptr);
    ਜੇ (prev_ptr != 0) {
        ਜਵਾਬ_PTR(prev_ptr)->ਜਵਾਬ_ਅਗਲਾ = ਜਵਾਬ->ਜਵਾਬ_ਅਗਲਾ;
        ਪ੍ਰਤਿਜ੍ਞਾ(call_ਢੇਰ_get_is_ਸਿਰ(ਜਵਾਬ_PTR(prev_ptr)->ਜਵਾਬ_ਅਗਲਾ));
    }

    ਜਵਾਬ->ਜਵਾਬ_ਅਗਲਾ = call_ਢੇਰ_new(0, false);

    /* give it back */
    ਜੇ (tcb->tcbSchedContext == ਨਿਰਾ) {
        /* only give the SC back if our SC is NULL. This prevents
         * strange behaviour when a thread is bound to an sc while it is
         * in the BlockedOnReply state. The semantics in this case are that the
         * SC cannot go back to the caller if the caller has received another one */
        schedContext_donate(SC_PTR(ਅਗਲਾ_ptr), tcb);
    }

    ਜਵਾਬ->ਜਵਾਬ_Prev = call_ਢੇਰ_new(0, false);
    ਜਵਾਬ_unlink(ਜਵਾਬ, tcb);
}

/* Remove a reply from the middle of the call stack */
ਖਾਲੀ ਜਵਾਬ_remove(ਜਵਾਬ_t *ਜਵਾਬ, tcb_t *tcb)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(ਜਵਾਬ->ਜਵਾਬ_TCB == tcb);
    ਪ੍ਰਤਿਜ੍ਞਾ(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_ਜਵਾਬ);
    ਪ੍ਰਤਿਜ੍ਞਾ(thread_state_get_ਜਵਾਬ_Object(tcb->tcbState) == ਜਵਾਬ_REF(ਜਵਾਬ));

    ਸ਼ਬਦ_t ਅਗਲਾ_ptr = call_ਢੇਰ_get_call_ਢੇਰ_Ptr(ਜਵਾਬ->ਜਵਾਬ_ਅਗਲਾ);
    ਸ਼ਬਦ_t prev_ptr = call_ਢੇਰ_get_call_ਢੇਰ_Ptr(ਜਵਾਬ->ਜਵਾਬ_Prev);

    ਜੇ (likely(ਅਗਲਾ_ptr && call_ਢੇਰ_get_is_ਸਿਰ(ਜਵਾਬ->ਜਵਾਬ_ਅਗਲਾ))) {
        /* head of the call stack -> just pop */
        ਜਵਾਬ_ਕੱਢੋ(ਜਵਾਬ, tcb);
    } ਨਹੀਂਤਾਂ {
        ਜੇ (ਅਗਲਾ_ptr) {
            /* not the head, remove from middle - break the chain */
            ਜਵਾਬ_PTR(ਅਗਲਾ_ptr)->ਜਵਾਬ_Prev = call_ਢੇਰ_new(0, false);
        }
        ਜੇ (prev_ptr) {
            ਜਵਾਬ_PTR(prev_ptr)->ਜਵਾਬ_ਅਗਲਾ = call_ਢੇਰ_new(0, false);
        }
        ਜਵਾਬ->ਜਵਾਬ_Prev = call_ਢੇਰ_new(0, false);
        ਜਵਾਬ->ਜਵਾਬ_ਅਗਲਾ = call_ਢੇਰ_new(0, false);
        ਜਵਾਬ_unlink(ਜਵਾਬ, tcb);
    }
}

ਖਾਲੀ ਜਵਾਬ_remove_tcb(tcb_t *tcb)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_ਜਵਾਬ);
    ਜਵਾਬ_t *ਜਵਾਬ = ਜਵਾਬ_PTR(thread_state_get_ਜਵਾਬ_Object(tcb->tcbState));
    ਸ਼ਬਦ_t ਅਗਲਾ_ptr = call_ਢੇਰ_get_call_ਢੇਰ_Ptr(ਜਵਾਬ->ਜਵਾਬ_ਅਗਲਾ);
    ਸ਼ਬਦ_t prev_ptr = call_ਢੇਰ_get_call_ਢੇਰ_Ptr(ਜਵਾਬ->ਜਵਾਬ_Prev);

    ਜੇ (ਅਗਲਾ_ptr) {
        ਜੇ (call_ਢੇਰ_get_is_ਸਿਰ(ਜਵਾਬ->ਜਵਾਬ_ਅਗਲਾ)) {
            SC_PTR(ਅਗਲਾ_ptr)->sc_ਜਵਾਬ = ਨਿਰਾ;
        } ਨਹੀਂਤਾਂ {
            ਜਵਾਬ_PTR(ਅਗਲਾ_ptr)->ਜਵਾਬ_Prev = call_ਢੇਰ_new(0, false);
        }
    }

    ਜੇ (prev_ptr) {
        ਜਵਾਬ_PTR(prev_ptr)->ਜਵਾਬ_ਅਗਲਾ = call_ਢੇਰ_new(0, false);
    }

    ਜਵਾਬ->ਜਵਾਬ_Prev = call_ਢੇਰ_new(0, false);
    ਜਵਾਬ->ਜਵਾਬ_ਅਗਲਾ = call_ਢੇਰ_new(0, false);
    ਜਵਾਬ_unlink(ਜਵਾਬ, tcb);
}
