/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <object/reply.h>

ಶೂನ್ಯ ಉತ್ತರ_ತಳ್ಳು(tcb_t *tcb_caller, tcb_t *tcb_callee, ಉತ್ತರ_t *ಉತ್ತರ, bool_t canDonate)
{
    sched_context_t *sc_donated = tcb_caller->tcbSchedContext;

    ಪ್ರತಿಜ್ಞಾ(tcb_caller != ಖಾಲಿ);
    ಪ್ರತಿಜ್ಞಾ(ಉತ್ತರ != ಖಾಲಿ);
    ಪ್ರತಿಜ್ಞಾ(ಉತ್ತರ->ಉತ್ತರ_TCB == ಖಾಲಿ);

    ಪ್ರತಿಜ್ಞಾ(call_ರಾಶಿ_get_call_ರಾಶಿ_Ptr(ಉತ್ತರ->ಉತ್ತರ_Prev) == 0);
    ಪ್ರತಿಜ್ಞಾ(call_ರಾಶಿ_get_call_ರಾಶಿ_Ptr(ಉತ್ತರ->ಉತ್ತರ_ಮುಂದಿನ) == 0);

    /* caller state must be blocked on send or active, so cannot be in a existing call stack */
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_BlockedOnSend ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Restart);

    /* callee state must be simple at this point (inactive, restart, or running),
       so it also cannot have a reply reference associated with it. */
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Inactive ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Restart);

    /* link caller and reply */
    ಉತ್ತರ->ಉತ್ತರ_TCB = tcb_caller;
    ಗಣ_Thread_State_Blocked_On_ಉತ್ತರ(tcb_caller, ಉತ್ತರ);

    ಆದರೆ (sc_donated != ಖಾಲಿ && tcb_callee->tcbSchedContext == ಖಾಲಿ && canDonate) {
        ಉತ್ತರ_t *old_caller = sc_donated->sc_ಉತ್ತರ;

        /* check stack integrity */
        ಪ್ರತಿಜ್ಞಾ(old_caller == ಖಾಲಿ ||
               SC_PTR(call_ರಾಶಿ_get_call_ರಾಶಿ_Ptr(old_caller->ಉತ್ತರ_ಮುಂದಿನ)) == sc_donated);

        /* push on to stack */
        ಆದರೆ (old_caller) {
            old_caller->ಉತ್ತರ_ಮುಂದಿನ = call_ರಾಶಿ_new(ಉತ್ತರ_REF(ಉತ್ತರ), false);
        }
        ಉತ್ತರ->ಉತ್ತರ_Prev = call_ರಾಶಿ_new(ಉತ್ತರ_REF(old_caller), false);
        sc_donated->sc_ಉತ್ತರ = ಉತ್ತರ;
        ಉತ್ತರ->ಉತ್ತರ_ಮುಂದಿನ = call_ರಾಶಿ_new(SC_REF(sc_donated), true);

        /* now do the actual donation */
        schedContext_donate(sc_donated, tcb_callee);
    }
}

/* Pop the head reply from the call stack */
ಶೂನ್ಯ ಉತ್ತರ_ತೆಗೆ(ಉತ್ತರ_t *ಉತ್ತರ, tcb_t *tcb)
{
    ಪ್ರತಿಜ್ಞಾ(ಉತ್ತರ != ಖಾಲಿ);
    ಪ್ರತಿಜ್ಞಾ(ಉತ್ತರ->ಉತ್ತರ_TCB == tcb);
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_ಉತ್ತರ);
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_ಉತ್ತರ_Object(tcb->tcbState) == ಉತ್ತರ_REF(ಉತ್ತರ));

    ಶಬ್ದ_t ಮುಂದಿನ_ptr = call_ರಾಶಿ_get_call_ರಾಶಿ_Ptr(ಉತ್ತರ->ಉತ್ತರ_ಮುಂದಿನ);
    ಶಬ್ದ_t prev_ptr = call_ರಾಶಿ_get_call_ರಾಶಿ_Ptr(ಉತ್ತರ->ಉತ್ತರ_Prev);

    SC_PTR(ಮುಂದಿನ_ptr)->sc_ಉತ್ತರ = ಉತ್ತರ_PTR(prev_ptr);
    ಆದರೆ (prev_ptr != 0) {
        ಉತ್ತರ_PTR(prev_ptr)->ಉತ್ತರ_ಮುಂದಿನ = ಉತ್ತರ->ಉತ್ತರ_ಮುಂದಿನ;
        ಪ್ರತಿಜ್ಞಾ(call_ರಾಶಿ_get_is_ತಲೆ(ಉತ್ತರ_PTR(prev_ptr)->ಉತ್ತರ_ಮುಂದಿನ));
    }

    ಉತ್ತರ->ಉತ್ತರ_ಮುಂದಿನ = call_ರಾಶಿ_new(0, false);

    /* give it back */
    ಆದರೆ (tcb->tcbSchedContext == ಖಾಲಿ) {
        /* only give the SC back if our SC is NULL. This prevents
         * strange behaviour when a thread is bound to an sc while it is
         * in the BlockedOnReply state. The semantics in this case are that the
         * SC cannot go back to the caller if the caller has received another one */
        schedContext_donate(SC_PTR(ಮುಂದಿನ_ptr), tcb);
    }

    ಉತ್ತರ->ಉತ್ತರ_Prev = call_ರಾಶಿ_new(0, false);
    ಉತ್ತರ_unlink(ಉತ್ತರ, tcb);
}

/* Remove a reply from the middle of the call stack */
ಶೂನ್ಯ ಉತ್ತರ_remove(ಉತ್ತರ_t *ಉತ್ತರ, tcb_t *tcb)
{
    ಪ್ರತಿಜ್ಞಾ(ಉತ್ತರ->ಉತ್ತರ_TCB == tcb);
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_ಉತ್ತರ);
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_ಉತ್ತರ_Object(tcb->tcbState) == ಉತ್ತರ_REF(ಉತ್ತರ));

    ಶಬ್ದ_t ಮುಂದಿನ_ptr = call_ರಾಶಿ_get_call_ರಾಶಿ_Ptr(ಉತ್ತರ->ಉತ್ತರ_ಮುಂದಿನ);
    ಶಬ್ದ_t prev_ptr = call_ರಾಶಿ_get_call_ರಾಶಿ_Ptr(ಉತ್ತರ->ಉತ್ತರ_Prev);

    ಆದರೆ (likely(ಮುಂದಿನ_ptr && call_ರಾಶಿ_get_is_ತಲೆ(ಉತ್ತರ->ಉತ್ತರ_ಮುಂದಿನ))) {
        /* head of the call stack -> just pop */
        ಉತ್ತರ_ತೆಗೆ(ಉತ್ತರ, tcb);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (ಮುಂದಿನ_ptr) {
            /* not the head, remove from middle - break the chain */
            ಉತ್ತರ_PTR(ಮುಂದಿನ_ptr)->ಉತ್ತರ_Prev = call_ರಾಶಿ_new(0, false);
        }
        ಆದರೆ (prev_ptr) {
            ಉತ್ತರ_PTR(prev_ptr)->ಉತ್ತರ_ಮುಂದಿನ = call_ರಾಶಿ_new(0, false);
        }
        ಉತ್ತರ->ಉತ್ತರ_Prev = call_ರಾಶಿ_new(0, false);
        ಉತ್ತರ->ಉತ್ತರ_ಮುಂದಿನ = call_ರಾಶಿ_new(0, false);
        ಉತ್ತರ_unlink(ಉತ್ತರ, tcb);
    }
}

ಶೂನ್ಯ ಉತ್ತರ_remove_tcb(tcb_t *tcb)
{
    ಪ್ರತಿಜ್ಞಾ(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_ಉತ್ತರ);
    ಉತ್ತರ_t *ಉತ್ತರ = ಉತ್ತರ_PTR(thread_state_get_ಉತ್ತರ_Object(tcb->tcbState));
    ಶಬ್ದ_t ಮುಂದಿನ_ptr = call_ರಾಶಿ_get_call_ರಾಶಿ_Ptr(ಉತ್ತರ->ಉತ್ತರ_ಮುಂದಿನ);
    ಶಬ್ದ_t prev_ptr = call_ರಾಶಿ_get_call_ರಾಶಿ_Ptr(ಉತ್ತರ->ಉತ್ತರ_Prev);

    ಆದರೆ (ಮುಂದಿನ_ptr) {
        ಆದರೆ (call_ರಾಶಿ_get_is_ತಲೆ(ಉತ್ತರ->ಉತ್ತರ_ಮುಂದಿನ)) {
            SC_PTR(ಮುಂದಿನ_ptr)->sc_ಉತ್ತರ = ಖಾಲಿ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಉತ್ತರ_PTR(ಮುಂದಿನ_ptr)->ಉತ್ತರ_Prev = call_ರಾಶಿ_new(0, false);
        }
    }

    ಆದರೆ (prev_ptr) {
        ಉತ್ತರ_PTR(prev_ptr)->ಉತ್ತರ_ಮುಂದಿನ = call_ರಾಶಿ_new(0, false);
    }

    ಉತ್ತರ->ಉತ್ತರ_Prev = call_ರಾಶಿ_new(0, false);
    ಉತ್ತರ->ಉತ್ತರ_ಮುಂದಿನ = call_ರಾಶಿ_new(0, false);
    ಉತ್ತರ_unlink(ಉತ್ತರ, tcb);
}
