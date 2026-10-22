/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <object/reply.h>

فراغ جواب_ڈالو(tcb_t *tcb_caller, tcb_t *tcb_callee, جواب_t *جواب, bool_t canDonate)
{
    sched_context_t *sc_donated = tcb_caller->tcbSchedContext;

    أكد(tcb_caller != عدم);
    أكد(جواب != عدم);
    أكد(جواب->جواب_TCB == عدم);

    أكد(call_ڈھیر_get_call_ڈھیر_Ptr(جواب->جواب_Prev) == 0);
    أكد(call_ڈھیر_get_call_ڈھیر_Ptr(جواب->جواب_اگلا) == 0);

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
    جواب->جواب_TCB = tcb_caller;
    سیٹ_Thread_State_Blocked_On_جواب(tcb_caller, جواب);

    إذا (sc_donated != عدم && tcb_callee->tcbSchedContext == عدم && canDonate) {
        جواب_t *old_caller = sc_donated->sc_جواب;

        /* check stack integrity */
        أكد(old_caller == عدم ||
               SC_PTR(call_ڈھیر_get_call_ڈھیر_Ptr(old_caller->جواب_اگلا)) == sc_donated);

        /* push on to stack */
        إذا (old_caller) {
            old_caller->جواب_اگلا = call_ڈھیر_new(جواب_REF(جواب), false);
        }
        جواب->جواب_Prev = call_ڈھیر_new(جواب_REF(old_caller), false);
        sc_donated->sc_جواب = جواب;
        جواب->جواب_اگلا = call_ڈھیر_new(SC_REF(sc_donated), true);

        /* now do the actual donation */
        schedContext_donate(sc_donated, tcb_callee);
    }
}

/* Pop the head reply from the call stack */
فراغ جواب_نکالو(جواب_t *جواب, tcb_t *tcb)
{
    أكد(جواب != عدم);
    أكد(جواب->جواب_TCB == tcb);
    أكد(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_جواب);
    أكد(thread_state_get_جواب_Object(tcb->tcbState) == جواب_REF(جواب));

    لفظ_t اگلا_ptr = call_ڈھیر_get_call_ڈھیر_Ptr(جواب->جواب_اگلا);
    لفظ_t prev_ptr = call_ڈھیر_get_call_ڈھیر_Ptr(جواب->جواب_Prev);

    SC_PTR(اگلا_ptr)->sc_جواب = جواب_PTR(prev_ptr);
    إذا (prev_ptr != 0) {
        جواب_PTR(prev_ptr)->جواب_اگلا = جواب->جواب_اگلا;
        أكد(call_ڈھیر_get_is_سر(جواب_PTR(prev_ptr)->جواب_اگلا));
    }

    جواب->جواب_اگلا = call_ڈھیر_new(0, false);

    /* give it back */
    إذا (tcb->tcbSchedContext == عدم) {
        /* only give the SC back if our SC is NULL. This prevents
         * strange behaviour when a thread is bound to an sc while it is
         * in the BlockedOnReply state. The semantics in this case are that the
         * SC cannot go back to the caller if the caller has received another one */
        schedContext_donate(SC_PTR(اگلا_ptr), tcb);
    }

    جواب->جواب_Prev = call_ڈھیر_new(0, false);
    جواب_unlink(جواب, tcb);
}

/* Remove a reply from the middle of the call stack */
فراغ جواب_remove(جواب_t *جواب, tcb_t *tcb)
{
    أكد(جواب->جواب_TCB == tcb);
    أكد(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_جواب);
    أكد(thread_state_get_جواب_Object(tcb->tcbState) == جواب_REF(جواب));

    لفظ_t اگلا_ptr = call_ڈھیر_get_call_ڈھیر_Ptr(جواب->جواب_اگلا);
    لفظ_t prev_ptr = call_ڈھیر_get_call_ڈھیر_Ptr(جواب->جواب_Prev);

    إذا (likely(اگلا_ptr && call_ڈھیر_get_is_سر(جواب->جواب_اگلا))) {
        /* head of the call stack -> just pop */
        جواب_نکالو(جواب, tcb);
    } وإلا {
        إذا (اگلا_ptr) {
            /* not the head, remove from middle - break the chain */
            جواب_PTR(اگلا_ptr)->جواب_Prev = call_ڈھیر_new(0, false);
        }
        إذا (prev_ptr) {
            جواب_PTR(prev_ptr)->جواب_اگلا = call_ڈھیر_new(0, false);
        }
        جواب->جواب_Prev = call_ڈھیر_new(0, false);
        جواب->جواب_اگلا = call_ڈھیر_new(0, false);
        جواب_unlink(جواب, tcb);
    }
}

فراغ جواب_remove_tcb(tcb_t *tcb)
{
    أكد(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_جواب);
    جواب_t *جواب = جواب_PTR(thread_state_get_جواب_Object(tcb->tcbState));
    لفظ_t اگلا_ptr = call_ڈھیر_get_call_ڈھیر_Ptr(جواب->جواب_اگلا);
    لفظ_t prev_ptr = call_ڈھیر_get_call_ڈھیر_Ptr(جواب->جواب_Prev);

    إذا (اگلا_ptr) {
        إذا (call_ڈھیر_get_is_سر(جواب->جواب_اگلا)) {
            SC_PTR(اگلا_ptr)->sc_جواب = عدم;
        } وإلا {
            جواب_PTR(اگلا_ptr)->جواب_Prev = call_ڈھیر_new(0, false);
        }
    }

    إذا (prev_ptr) {
        جواب_PTR(prev_ptr)->جواب_اگلا = call_ڈھیر_new(0, false);
    }

    جواب->جواب_Prev = call_ڈھیر_new(0, false);
    جواب->جواب_اگلا = call_ڈھیر_new(0, false);
    جواب_unlink(جواب, tcb);
}
