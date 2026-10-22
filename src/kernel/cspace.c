/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <types.h>
#అన్తర్భావ <object.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <kernel/cspace.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <arch/machine.h>

lookupCap_ret_t lookupCap(tcb_t *thread, cptr_t cPtr)
{
    lookup_స్థానం_raw_ret_t lu_ret;
    lookupCap_ret_t ret;

    lu_ret = lookup_స్థానం(thread, cPtr);
    ఒకవేళ (unlikely(lu_ret.స్థితి_2 != EXCEPTION_NONE)) {
        ret.స్థితి_2 = lu_ret.స్థితి_2;
        ret.cap = cap_null_cap_new();
        తిరిగి ret;
    }

    ret.స్థితి_2 = EXCEPTION_NONE;
    ret.cap = lu_ret.స్థానం->cap;
    తిరిగి ret;
}

lookup_Cap_And_స్థానం_ret_t lookup_Cap_And_స్థానం(tcb_t *thread, cptr_t cPtr)
{
    lookup_స్థానం_raw_ret_t lu_ret;
    lookup_Cap_And_స్థానం_ret_t ret;

    lu_ret = lookup_స్థానం(thread, cPtr);
    ఒకవేళ (unlikely(lu_ret.స్థితి_2 != EXCEPTION_NONE)) {
        ret.స్థితి_2 = lu_ret.స్థితి_2;
        ret.స్థానం = ఖాళీ;
        ret.cap = cap_null_cap_new();
        తిరిగి ret;
    }

    ret.స్థితి_2 = EXCEPTION_NONE;
    ret.స్థానం = lu_ret.స్థానం;
    ret.cap = lu_ret.స్థానం->cap;
    తిరిగి ret;
}

lookup_స్థానం_raw_ret_t lookup_స్థానం(tcb_t *thread, cptr_t capptr)
{
    cap_t thread_మూలం;
    resolve_చిరునామా_బిట్లు_ret_t res_ret;
    lookup_స్థానం_raw_ret_t ret;

    thread_మూలం = TCB_PTR_CTE_PTR(thread, tcbCTable)->cap;
    res_ret = resolve_చిరునామా_బిట్లు(thread_మూలం, capptr, మాట_బిట్లు);

    ret.స్థితి_2 = res_ret.స్థితి_2;
    ret.స్థానం = res_ret.స్థానం;
    తిరిగి ret;
}

lookup_స్థానం_ret_t lookup_స్థానం_For_CNode_Op(bool_t is_ఆధారం, cap_t మూలం, cptr_t capptr,
                                      మాట_t లోతు)
{
    resolve_చిరునామా_బిట్లు_ret_t res_ret;
    lookup_స్థానం_ret_t ret;

    ret.స్థానం = ఖాళీ;

    ఒకవేళ (unlikely(cap_get_capType(మూలం) != cap_cnode_cap)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = is_ఆధారం;
        current_lookup_fault = lookup_fault_invalid_మూలం_new();
        ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        తిరిగి ret;
    }

    ఒకవేళ (unlikely(లోతు < 1 || లోతు > మాట_బిట్లు)) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = మాట_బిట్లు;
        ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        తిరిగి ret;
    }
    res_ret = resolve_చిరునామా_బిట్లు(మూలం, capptr, లోతు);
    ఒకవేళ (unlikely(res_ret.స్థితి_2 != EXCEPTION_NONE)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = is_ఆధారం;
        /* current_lookup_fault will have been set by resolveAddressBits */
        ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        తిరిగి ret;
    }

    ఒకవేళ (unlikely(res_ret.బిట్లు_మిగిలిన != 0)) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = is_ఆధారం;
        current_lookup_fault =
            lookup_fault_లోతు_mismatch_new(0, res_ret.బిట్లు_మిగిలిన);
        ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        తిరిగి ret;
    }

    ret.స్థానం = res_ret.స్థానం;
    ret.స్థితి_2 = EXCEPTION_NONE;
    తిరిగి ret;
}

lookup_స్థానం_ret_t lookup_ఆధారం_స్థానం(cap_t మూలం, cptr_t capptr, మాట_t లోతు)
{
    తిరిగి lookup_స్థానం_For_CNode_Op(true, మూలం, capptr, లోతు);
}

lookup_స్థానం_ret_t lookup_లక్ష్యం_స్థానం(cap_t మూలం, cptr_t capptr, మాట_t లోతు)
{
    తిరిగి lookup_స్థానం_For_CNode_Op(false, మూలం, capptr, లోతు);
}

lookup_స్థానం_ret_t lookup_ఇరుసు_స్థానం(cap_t మూలం, cptr_t capptr, మాట_t లోతు)
{
    తిరిగి lookup_స్థానం_For_CNode_Op(true, మూలం, capptr, లోతు);
}

resolve_చిరునామా_బిట్లు_ret_t resolve_చిరునామా_బిట్లు(cap_t కణుపు_Cap, cptr_t capptr, మాట_t n_బిట్లు)
{
    resolve_చిరునామా_బిట్లు_ret_t ret;
    మాట_t radix_బిట్లు, guard_బిట్లు, level_బిట్లు, guard;
    మాట_t capGuard, స్థానభ్రంశం;
    cte_t *స్థానం;

    ret.బిట్లు_మిగిలిన = n_బిట్లు;
    ret.స్థానం = ఖాళీ;

    ఒకవేళ (unlikely(cap_get_capType(కణుపు_Cap) != cap_cnode_cap)) {
        current_lookup_fault = lookup_fault_invalid_మూలం_new();
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }

    అయితే (1) {
        radix_బిట్లు = cap_cnode_cap_get_capCNodeRadix(కణుపు_Cap);
        guard_బిట్లు = cap_cnode_cap_get_cap_CNode_Guard_కొలత(కణుపు_Cap);
        level_బిట్లు = radix_బిట్లు + guard_బిట్లు;

        /* Haskell error: "All CNodes must resolve bits" */
        ప్రతిజ్ఞా(level_బిట్లు != 0);

        capGuard = cap_cnode_cap_get_capCNodeGuard(కణుపు_Cap);

        /* The MASK(wordRadix) here is to avoid the case where
         * n_bits = wordBits (=2^wordRadix) and guardBits = 0, as it violates
         * the C spec to shift right by more than wordBits-1.
         */
        guard = (capptr >> ((n_బిట్లు - guard_బిట్లు) & ముసుగు(మాట_Radix))) & ముసుగు(guard_బిట్లు);
        ఒకవేళ (unlikely(guard_బిట్లు > n_బిట్లు || guard != capGuard)) {
            current_lookup_fault =
                lookup_fault_guard_mismatch_new(capGuard, n_బిట్లు, guard_బిట్లు);
            ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
            తిరిగి ret;
        }

        ఒకవేళ (unlikely(level_బిట్లు > n_బిట్లు)) {
            current_lookup_fault =
                lookup_fault_లోతు_mismatch_new(level_బిట్లు, n_బిట్లు);
            ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
            తిరిగి ret;
        }

        స్థానభ్రంశం = (capptr >> (n_బిట్లు - level_బిట్లు)) & ముసుగు(radix_బిట్లు);
        స్థానం = CTE_PTR(cap_cnode_cap_get_capCNodePtr(కణుపు_Cap)) + స్థానభ్రంశం;

        ఒకవేళ (likely(n_బిట్లు == level_బిట్లు)) {
            ret.స్థితి_2 = EXCEPTION_NONE;
            ret.స్థానం = స్థానం;
            ret.బిట్లు_మిగిలిన = 0;
            తిరిగి ret;
        }

        /** GHOSTUPD: "(\<acute>levelBits > 0, id)" */

        n_బిట్లు -= level_బిట్లు;
        కణుపు_Cap = స్థానం->cap;

        ఒకవేళ (unlikely(cap_get_capType(కణుపు_Cap) != cap_cnode_cap)) {
            ret.స్థితి_2 = EXCEPTION_NONE;
            ret.స్థానం = స్థానం;
            ret.బిట్లు_మిగిలిన = n_బిట్లు;
            తిరిగి ret;
        }
    }
}
