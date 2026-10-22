/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <object/reply.h>

فراغ پاسخ_افزودن(tcb_t *tcb_caller, tcb_t *tcb_callee, پاسخ_t *پاسخ, bool_t canDonate)
{
    sched_context_t *sc_donated = tcb_caller->tcbSchedContext;

    أكد(tcb_caller != عدم);
    أكد(پاسخ != عدم);
    أكد(پاسخ->پاسخ_TCB == عدم);

    أكد(call_پشته_get_call_پشته_Ptr(پاسخ->پاسخ_Prev) == 0);
    أكد(call_پشته_get_call_پشته_Ptr(پاسخ->پاسخ_بعدی) == 0);

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
    پاسخ->پاسخ_TCB = tcb_caller;
    مجموعه_Thread_State_Blocked_On_پاسخ(tcb_caller, پاسخ);

    إذا (sc_donated != عدم && tcb_callee->tcbSchedContext == عدم && canDonate) {
        پاسخ_t *old_caller = sc_donated->sc_پاسخ;

        /* check stack integrity */
        أكد(old_caller == عدم ||
               SC_PTR(call_پشته_get_call_پشته_Ptr(old_caller->پاسخ_بعدی)) == sc_donated);

        /* push on to stack */
        إذا (old_caller) {
            old_caller->پاسخ_بعدی = call_پشته_new(پاسخ_REF(پاسخ), false);
        }
        پاسخ->پاسخ_Prev = call_پشته_new(پاسخ_REF(old_caller), false);
        sc_donated->sc_پاسخ = پاسخ;
        پاسخ->پاسخ_بعدی = call_پشته_new(SC_REF(sc_donated), true);

        /* now do the actual donation */
        schedContext_donate(sc_donated, tcb_callee);
    }
}

/* Pop the head reply from the call stack */
فراغ پاسخ_برداشت(پاسخ_t *پاسخ, tcb_t *tcb)
{
    أكد(پاسخ != عدم);
    أكد(پاسخ->پاسخ_TCB == tcb);
    أكد(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_پاسخ);
    أكد(thread_state_get_پاسخ_Object(tcb->tcbState) == پاسخ_REF(پاسخ));

    کلمه_t بعدی_ptr = call_پشته_get_call_پشته_Ptr(پاسخ->پاسخ_بعدی);
    کلمه_t prev_ptr = call_پشته_get_call_پشته_Ptr(پاسخ->پاسخ_Prev);

    SC_PTR(بعدی_ptr)->sc_پاسخ = پاسخ_PTR(prev_ptr);
    إذا (prev_ptr != 0) {
        پاسخ_PTR(prev_ptr)->پاسخ_بعدی = پاسخ->پاسخ_بعدی;
        أكد(call_پشته_get_is_سر(پاسخ_PTR(prev_ptr)->پاسخ_بعدی));
    }

    پاسخ->پاسخ_بعدی = call_پشته_new(0, false);

    /* give it back */
    إذا (tcb->tcbSchedContext == عدم) {
        /* only give the SC back if our SC is NULL. This prevents
         * strange behaviour when a thread is bound to an sc while it is
         * in the BlockedOnReply state. The semantics in this case are that the
         * SC cannot go back to the caller if the caller has received another one */
        schedContext_donate(SC_PTR(بعدی_ptr), tcb);
    }

    پاسخ->پاسخ_Prev = call_پشته_new(0, false);
    پاسخ_unlink(پاسخ, tcb);
}

/* Remove a reply from the middle of the call stack */
فراغ پاسخ_remove(پاسخ_t *پاسخ, tcb_t *tcb)
{
    أكد(پاسخ->پاسخ_TCB == tcb);
    أكد(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_پاسخ);
    أكد(thread_state_get_پاسخ_Object(tcb->tcbState) == پاسخ_REF(پاسخ));

    کلمه_t بعدی_ptr = call_پشته_get_call_پشته_Ptr(پاسخ->پاسخ_بعدی);
    کلمه_t prev_ptr = call_پشته_get_call_پشته_Ptr(پاسخ->پاسخ_Prev);

    إذا (likely(بعدی_ptr && call_پشته_get_is_سر(پاسخ->پاسخ_بعدی))) {
        /* head of the call stack -> just pop */
        پاسخ_برداشت(پاسخ, tcb);
    } وإلا {
        إذا (بعدی_ptr) {
            /* not the head, remove from middle - break the chain */
            پاسخ_PTR(بعدی_ptr)->پاسخ_Prev = call_پشته_new(0, false);
        }
        إذا (prev_ptr) {
            پاسخ_PTR(prev_ptr)->پاسخ_بعدی = call_پشته_new(0, false);
        }
        پاسخ->پاسخ_Prev = call_پشته_new(0, false);
        پاسخ->پاسخ_بعدی = call_پشته_new(0, false);
        پاسخ_unlink(پاسخ, tcb);
    }
}

فراغ پاسخ_remove_tcb(tcb_t *tcb)
{
    أكد(thread_state_get_tsType(tcb->tcbState) == Thread_State_Blocked_On_پاسخ);
    پاسخ_t *پاسخ = پاسخ_PTR(thread_state_get_پاسخ_Object(tcb->tcbState));
    کلمه_t بعدی_ptr = call_پشته_get_call_پشته_Ptr(پاسخ->پاسخ_بعدی);
    کلمه_t prev_ptr = call_پشته_get_call_پشته_Ptr(پاسخ->پاسخ_Prev);

    إذا (بعدی_ptr) {
        إذا (call_پشته_get_is_سر(پاسخ->پاسخ_بعدی)) {
            SC_PTR(بعدی_ptr)->sc_پاسخ = عدم;
        } وإلا {
            پاسخ_PTR(بعدی_ptr)->پاسخ_Prev = call_پشته_new(0, false);
        }
    }

    إذا (prev_ptr) {
        پاسخ_PTR(prev_ptr)->پاسخ_بعدی = call_پشته_new(0, false);
    }

    پاسخ->پاسخ_Prev = call_پشته_new(0, false);
    پاسخ->پاسخ_بعدی = call_پشته_new(0, false);
    پاسخ_unlink(پاسخ, tcb);
}
