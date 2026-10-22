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
dschedule_t ks_Dom_ویلا_سارنی[CONFIG_عدد_DOMAIN_SCHEDULES];

ساكن فراغ invoke_Domain_سیٹ_سیٹ(tcb_t *tcb, dom_t domain)
{
    prepare_سیٹ_Domain(tcb, domain);
    سیٹ_Domain(tcb, domain);
}

ساكن exception_t decode_Domain_سیٹ_سیٹ_Invocation(لفظ_t لمائی, لفظ_t *بفر)
{
    dom_t domain;
    cap_t tcap;

    إذا (unlikely(لمائی == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    } وإلا {
        domain = getSyscallArg(0, بفر);
        إذا (domain >= عدد_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, عدد_Domains);
            current_syscall_error.type = se_L4_Invalid_دلیل;
            current_syscall_error.invalid_دلیل_نمبر = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    إذا (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_سیٹ_سیٹ(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    أرجع EXCEPTION_NONE;
}

ساكن فراغ invoke_Domain_ویلا_سارنی_Configure(لفظ_t اشاریہ, dom_t domain, ticks_t duration)
{
    ks_Dom_ویلا_سارنی[اشاریہ] = dschedule_make(domain, duration);
}

ساكن exception_t decode_Domain_ویلا_سارنی_Configure(لفظ_t لمائی, لفظ_t *بفر)
{
    لفظ_t اشاریہ;
    dom_t domain;
    ticks_t duration;

    إذا (unlikely(لمائی < 2 + ویلا_ARG_ناپ)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    اشاریہ = getSyscallArg(0, بفر);
    domain = getSyscallArg(1, بفر);
    duration = منوال_parse_ویلا_Arg(2, بفر);

    /* Last entry must stay an end marker, hence the -1: */
    إذا (اشاریہ >= dom_ویلا_سارنی_لمائی - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_ویلا_سارنی_لمائی - 2;
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
        current_syscall_error.invalid_دلیل_نمبر = 2;
        current_syscall_error.type = se_L4_Invalid_دلیل;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_دلیل_نمبر = 1;
        current_syscall_error.type = se_L4_Invalid_دلیل;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (اشاریہ == ks_Dom_ویلا_سارنی_شروع && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_دلیل_نمبر = 2;
        current_syscall_error.type = se_L4_Invalid_دلیل;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_ویلا_سارنی_Configure(اشاریہ, domain, duration);
    أرجع EXCEPTION_NONE;
}

ساكن فراغ invoke_Domain_ویلا_سارنی_سیٹ_شروع(لفظ_t اشاریہ)
{
    ks_Dom_ویلا_سارنی_شروع = اشاریہ;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ks_Domain_ویلا = 0;
    ks_Dom_ویلا_سارنی_Idx = dom_ویلا_سارنی_لمائی - 2;
    rescheduleRequired();
}

ساكن exception_t decode_Domain_ویلا_سارنی_سیٹ_شروع(لفظ_t لمائی, لفظ_t *بفر)
{
    لفظ_t اشاریہ;

    إذا (unlikely(لمائی < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    اشاریہ = getSyscallArg(0, بفر);

    إذا (اشاریہ >= dom_ویلا_سارنی_لمائی) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_ویلا_سارنی_لمائی - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (dschedule_is_سرا_marker(اشاریہ)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_دلیل_نمبر = 0;
        current_syscall_error.type = se_L4_Invalid_دلیل;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_ویلا_سارنی_سیٹ_شروع(اشاریہ);
    أرجع EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(لفظ_t invLabel, لفظ_t لمائی, لفظ_t *بفر)
{
    بدل (invLabel) {
    حالة Domain_سیٹ_سیٹ:
        أرجع decode_Domain_سیٹ_سیٹ_Invocation(لمائی, بفر);
    حالة Domain_ویلا_سارنی_Configure:
        أرجع decode_Domain_ویلا_سارنی_Configure(لمائی, بفر);
    حالة Domain_ویلا_سارنی_سیٹ_شروع:
        أرجع decode_Domain_ویلا_سارنی_سیٹ_شروع(لمائی, بفر);
    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
