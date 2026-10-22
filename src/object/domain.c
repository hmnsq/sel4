/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2025, Indan Zupancic
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#شامل <util.h>
#شامل <api/syscall.h>
#شامل <kernel/thread.h>
#شامل <mode/api/ipc_buffer.h>
#شامل <model/statedata.h>
#شامل <object/domain.h>
#شامل <object/structures.h>

/* Domain schedules. The duration is in kernel ticks for non-MCS and timer ticks for MCS. */
dschedule_t ks_Dom_جدول_وقت[CONFIG_عدد_DOMAIN_SCHEDULES];

ساکن خالی invoke_Domain_سیٹ_سیٹ(tcb_t *tcb, dom_t domain)
{
    prepare_سیٹ_Domain(tcb, domain);
    سیٹ_Domain(tcb, domain);
}

ساکن exception_t decode_Domain_سیٹ_سیٹ_Invocation(لفظ_t لمبائی, لفظ_t *بفر)
{
    dom_t domain;
    cap_t tcap;

    اگر (unlikely(لمبائی == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    } ورنہ {
        domain = getSyscallArg(0, بفر);
        اگر (domain >= عدد_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, عدد_Domains);
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 0;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
    }

    اگر (unlikely(current_extra_caps.excaprefs[0] == تہی)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    اگر (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_سیٹ_سیٹ(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    واپس EXCEPTION_NONE;
}

ساکن خالی invoke_Domain_جدول_وقت_Configure(لفظ_t اشاریہ, dom_t domain, ticks_t duration)
{
    ks_Dom_جدول_وقت[اشاریہ] = dschedule_make(domain, duration);
}

ساکن exception_t decode_Domain_جدول_وقت_Configure(لفظ_t لمبائی, لفظ_t *بفر)
{
    لفظ_t اشاریہ;
    dom_t domain;
    ticks_t duration;

    اگر (unlikely(لمبائی < 2 + وقت_ARG_سائز)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اشاریہ = getSyscallArg(0, بفر);
    domain = getSyscallArg(1, بفر);
    duration = منوال_parse_وقت_Arg(2, بفر);

    /* Last entry must stay an end marker, hence the -1: */
    اگر (اشاریہ >= dom_جدول_وقت_لمبائی - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_جدول_وقت_لمبائی - 2;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اگر (domain >= عدد_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = عدد_Domains - 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اگر (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_دلیل_نمبر = 2;
        current_syscall_error.type = se_L4_Invalid_دلیل;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اگر (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_دلیل_نمبر = 1;
        current_syscall_error.type = se_L4_Invalid_دلیل;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اگر (اشاریہ == ks_Dom_جدول_وقت_آغاز && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_دلیل_نمبر = 2;
        current_syscall_error.type = se_L4_Invalid_دلیل;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_جدول_وقت_Configure(اشاریہ, domain, duration);
    واپس EXCEPTION_NONE;
}

ساکن خالی invoke_Domain_جدول_وقت_سیٹ_آغاز(لفظ_t اشاریہ)
{
    ks_Dom_جدول_وقت_آغاز = اشاریہ;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ks_Domain_وقت = 0;
    ks_Dom_جدول_وقت_Idx = dom_جدول_وقت_لمبائی - 2;
    rescheduleRequired();
}

ساکن exception_t decode_Domain_جدول_وقت_سیٹ_آغاز(لفظ_t لمبائی, لفظ_t *بفر)
{
    لفظ_t اشاریہ;

    اگر (unlikely(لمبائی < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اشاریہ = getSyscallArg(0, بفر);

    اگر (اشاریہ >= dom_جدول_وقت_لمبائی) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_جدول_وقت_لمبائی - 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اگر (dschedule_is_انتہا_marker(اشاریہ)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_دلیل_نمبر = 0;
        current_syscall_error.type = se_L4_Invalid_دلیل;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_جدول_وقت_سیٹ_آغاز(اشاریہ);
    واپس EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(لفظ_t invLabel, لفظ_t لمبائی, لفظ_t *بفر)
{
    انتخاب (invLabel) {
    حالت Domain_سیٹ_سیٹ:
        واپس decode_Domain_سیٹ_سیٹ_Invocation(لمبائی, بفر);
    حالت Domain_جدول_وقت_Configure:
        واپس decode_Domain_جدول_وقت_Configure(لمبائی, بفر);
    حالت Domain_جدول_وقت_سیٹ_آغاز:
        واپس decode_Domain_جدول_وقت_سیٹ_آغاز(لمبائی, بفر);
    طےشدہ:
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
}
