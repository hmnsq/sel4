/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2025, Indan Zupancic
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ضمن <util.h>
#ضمن <api/syscall.h>
#ضمن <kernel/thread.h>
#ضمن <mode/api/ipc_buffer.h>
#ضمن <model/statedata.h>
#ضمن <object/domain.h>
#ضمن <object/structures.h>

/* Domain schedules. The duration is in kernel ticks for non-MCS and timer ticks for MCS. */
dschedule_t ks_Dom_مهالوېش[CONFIG_عدد_DOMAIN_SCHEDULES];

ساكن فراغ invoke_Domain_سېټ_سېټ(tcb_t *tcb, dom_t domain)
{
    prepare_سېټ_Domain(tcb, domain);
    سېټ_Domain(tcb, domain);
}

ساكن exception_t decode_Domain_سېټ_سېټ_Invocation(کلمه_t اوږدتیا, کلمه_t *بفر)
{
    dom_t domain;
    cap_t tcap;

    إذا (unlikely(اوږدتیا == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    } وإلا {
        domain = getSyscallArg(0, بفر);
        إذا (domain >= عدد_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, عدد_Domains);
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    إذا (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_سېټ_سېټ(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    أرجع EXCEPTION_NONE;
}

ساكن فراغ invoke_Domain_مهالوېش_Configure(کلمه_t شاخص, dom_t domain, ticks_t duration)
{
    ks_Dom_مهالوېش[شاخص] = dschedule_make(domain, duration);
}

ساكن exception_t decode_Domain_مهالوېش_Configure(کلمه_t اوږدتیا, کلمه_t *بفر)
{
    کلمه_t شاخص;
    dom_t domain;
    ticks_t duration;

    إذا (unlikely(اوږدتیا < 2 + وخت_ARG_اندازه)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    شاخص = getSyscallArg(0, بفر);
    domain = getSyscallArg(1, بفر);
    duration = ډېرپېښ_parse_وخت_Arg(2, بفر);

    /* Last entry must stay an end marker, hence the -1: */
    إذا (شاخص >= dom_مهالوېش_اوږدتیا - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_مهالوېش_اوږدتیا - 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (domain >= عدد_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = عدد_Domains - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_دلیل_شمېره = 2;
        current_syscall_error.type = se_L4_Invalid_دلیل;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_دلیل_شمېره = 1;
        current_syscall_error.type = se_L4_Invalid_دلیل;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (شاخص == ks_Dom_مهالوېش_پیل && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_دلیل_شمېره = 2;
        current_syscall_error.type = se_L4_Invalid_دلیل;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_مهالوېش_Configure(شاخص, domain, duration);
    أرجع EXCEPTION_NONE;
}

ساكن فراغ invoke_Domain_مهالوېش_سېټ_پیل(کلمه_t شاخص)
{
    ks_Dom_مهالوېش_پیل = شاخص;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ks_Domain_وخت = 0;
    ks_Dom_مهالوېش_Idx = dom_مهالوېش_اوږدتیا - 2;
    rescheduleRequired();
}

ساكن exception_t decode_Domain_مهالوېش_سېټ_پیل(کلمه_t اوږدتیا, کلمه_t *بفر)
{
    کلمه_t شاخص;

    إذا (unlikely(اوږدتیا < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    شاخص = getSyscallArg(0, بفر);

    إذا (شاخص >= dom_مهالوېش_اوږدتیا) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_مهالوېش_اوږدتیا - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (dschedule_is_پای_ټکی_marker(شاخص)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_دلیل_شمېره = 0;
        current_syscall_error.type = se_L4_Invalid_دلیل;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_مهالوېش_سېټ_پیل(شاخص);
    أرجع EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(کلمه_t invLabel, کلمه_t اوږدتیا, کلمه_t *بفر)
{
    بدل (invLabel) {
    حالة Domain_سېټ_سېټ:
        أرجع decode_Domain_سېټ_سېټ_Invocation(اوږدتیا, بفر);
    حالة Domain_مهالوېش_Configure:
        أرجع decode_Domain_مهالوېش_Configure(اوږدتیا, بفر);
    حالة Domain_مهالوېش_سېټ_پیل:
        أرجع decode_Domain_مهالوېش_سېټ_پیل(اوږدتیا, بفر);
    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
