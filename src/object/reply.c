/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <object/reply.h>

فراغ ځواب_ټېل(tcb_t *tcb_caller, tcb_t *tcb_callee, ځواب_t *ځواب, bool_t canDonate)
{
    sched_context_t *sc_donated = tcb_caller->tcbSchedContext;

    أكد(tcb_caller != عدم);
    أكد(ځواب != عدم);
    أكد(ځواب->ځواب_TCB == عدم);

    أكد(call_ډېری_get_call_ډېری_Ptr(ځواب->ځواب_Prev) == 0);
    أكد(call_ډېری_get_call_ډېری_Ptr(ځواب->ځواب_بل) == 0);

    /* caller state must be blocked on send or active, so cannot be in a existing call stack */
    أكد(thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_BlockedOnSend ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_caller->tcbState) == ThreadState_Restart);

    /* callee state must be simple at this point (inactive, restart, or running),
       so it also cannot have a reply reference associated with it. */
    أكد(thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Inactive ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Running ||
           thread_state_get_tsType(tcb_callee->tcbState) == ThreadState_Restart);

    /* link caller and reply */
    ځواب->ځواب_TCB = tcb_caller;
    سېټ_Thread_State_Blocked_On_ځواب(tcb_caller, ځواب);

    إذا (sc_donated != عدم && tcb_callee->tcbSchedContext == عدم && canDonate) {
        ځواب_t *old_caller = sc_donated->sc_ځواب;

        /* check stack integrity */
        أكد(old_caller == عدم ||
               SC_PTR(call_ډېری_get_call_ډېری_Ptr(old_caller->ځواب_بل)) == sc_donated);

        /* push on to stack */
        إذا (old_caller) {
            old_caller->ځواب_بل = call_ډېری_new(ځواب_REF(ځواب), false);
        }
        ځواب->ځواب_Prev = call_ډېری_new(ځواب_REF(old_caller), false);
        sc_donated->sc_ځواب = ځواب;
        ځواب->ځواب_بل = call_ډېری_new(SC_REF(sc_donated), true);

        /* now do the actual donation */
        schedContext_donate(sc_donated, tcb_callee);
    }
}

/* Pop the head reply from the call stack */
فراغ ځواب_راوباسه(ځواب_t *ځواب, tcb_t *tcb)
{
    أكد(ځواب != عدم);
    أكد(ځواب->ځواب_TCB == tcb);
    أكد(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_ځواب);
    أكد(thread_state_get_ځواب_Object(tcb->tcbState) == ځواب_REF(ځواب));

    کلمه_t بل_ptr = call_ډېری_get_call_ډېری_Ptr(ځواب->ځواب_بل);
    کلمه_t prev_ptr = call_ډېری_get_call_ډېری_Ptr(ځواب->ځواب_Prev);

    SC_PTR(بل_ptr)->sc_ځواب = ځواب_PTR(prev_ptr);
    إذا (prev_ptr != 0) {
        ځواب_PTR(prev_ptr)->ځواب_بل = ځواب->ځواب_بل;
        أكد(call_ډېری_get_is_سر(ځواب_PTR(prev_ptr)->ځواب_بل));
    }

    ځواب->ځواب_بل = call_ډېری_new(0, false);

    /* give it back */
    إذا (tcb->tcbSchedContext == عدم) {
        /* only give the SC back if our SC is NULL. This prevents
         * strange behaviour when a thread is bound to an sc while it is
         * in the BlockedOnReply state. The semantics in this case are that the
         * SC cannot go back to the caller if the caller has received another one */
        schedContext_donate(SC_PTR(بل_ptr), tcb);
    }

    ځواب->ځواب_Prev = call_ډېری_new(0, false);
    ځواب_unlink(ځواب, tcb);
}

/* Remove a reply from the middle of the call stack */
فراغ ځواب_remove(ځواب_t *ځواب, tcb_t *tcb)
{
    أكد(ځواب->ځواب_TCB == tcb);
    أكد(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_ځواب);
    أكد(thread_state_get_ځواب_Object(tcb->tcbState) == ځواب_REF(ځواب));

    کلمه_t بل_ptr = call_ډېری_get_call_ډېری_Ptr(ځواب->ځواب_بل);
    کلمه_t prev_ptr = call_ډېری_get_call_ډېری_Ptr(ځواب->ځواب_Prev);

    إذا (likely(بل_ptr && call_ډېری_get_is_سر(ځواب->ځواب_بل))) {
        /* head of the call stack -> just pop */
        ځواب_راوباسه(ځواب, tcb);
    } وإلا {
        إذا (بل_ptr) {
            /* not the head, remove from middle - break the chain */
            ځواب_PTR(بل_ptr)->ځواب_Prev = call_ډېری_new(0, false);
        }
        إذا (prev_ptr) {
            ځواب_PTR(prev_ptr)->ځواب_بل = call_ډېری_new(0, false);
        }
        ځواب->ځواب_Prev = call_ډېری_new(0, false);
        ځواب->ځواب_بل = call_ډېری_new(0, false);
        ځواب_unlink(ځواب, tcb);
    }
}

فراغ ځواب_remove_tcb(tcb_t *tcb)
{
    أكد(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_ځواب);
    ځواب_t *ځواب = ځواب_PTR(thread_state_get_ځواب_Object(tcb->tcbState));
    کلمه_t بل_ptr = call_ډېری_get_call_ډېری_Ptr(ځواب->ځواب_بل);
    کلمه_t prev_ptr = call_ډېری_get_call_ډېری_Ptr(ځواب->ځواب_Prev);

    إذا (بل_ptr) {
        إذا (call_ډېری_get_is_سر(ځواب->ځواب_بل)) {
            SC_PTR(بل_ptr)->sc_ځواب = عدم;
        } وإلا {
            ځواب_PTR(بل_ptr)->ځواب_Prev = call_ډېری_new(0, false);
        }
    }

    إذا (prev_ptr) {
        ځواب_PTR(prev_ptr)->ځواب_بل = call_ډېری_new(0, false);
    }

    ځواب->ځواب_Prev = call_ډېری_new(0, false);
    ځواب->ځواب_بل = call_ډېری_new(0, false);
    ځواب_unlink(ځواب, tcb);
}
