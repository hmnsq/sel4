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
dschedule_t ks_Dom_وقت_جدول[CONFIG_انگ_DOMAIN_SCHEDULES];

ساكن فراغ invoke_Domain_سيٽ_سيٽ(tcb_t *tcb, dom_t domain)
{
    prepare_سيٽ_Domain(tcb, domain);
    سيٽ_Domain(tcb, domain);
}

ساكن exception_t decode_Domain_سيٽ_سيٽ_Invocation(لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    dom_t domain;
    cap_t tcap;

    إذا (unlikely(ڊيگهائي == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    } وإلا {
        domain = getSyscallArg(0, بفر);
        إذا (domain >= انگ_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, انگ_Domains);
            current_syscall_error.type = se_L4_Invalid_دليل;
            current_syscall_error.invalid_دليل_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    إذا (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_سيٽ_سيٽ(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    أرجع EXCEPTION_NONE;
}

ساكن فراغ invoke_Domain_وقت_جدول_Configure(لفظ_t اشاريو, dom_t domain, ticks_t duration)
{
    ks_Dom_وقت_جدول[اشاريو] = dschedule_make(domain, duration);
}

ساكن exception_t decode_Domain_وقت_جدول_Configure(لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    لفظ_t اشاريو;
    dom_t domain;
    ticks_t duration;

    إذا (unlikely(ڊيگهائي < 2 + وقت_ARG_سائيز)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    اشاريو = getSyscallArg(0, بفر);
    domain = getSyscallArg(1, بفر);
    duration = گهڻائي_parse_وقت_Arg(2, بفر);

    /* Last entry must stay an end marker, hence the -1: */
    إذا (اشاريو >= dom_وقت_جدول_ڊيگهائي - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_وقت_جدول_ڊيگهائي - 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (domain >= انگ_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = انگ_Domains - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_دليل_عدد = 2;
        current_syscall_error.type = se_L4_Invalid_دليل;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_دليل_عدد = 1;
        current_syscall_error.type = se_L4_Invalid_دليل;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (اشاريو == ks_Dom_وقت_جدول_شروعات && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_دليل_عدد = 2;
        current_syscall_error.type = se_L4_Invalid_دليل;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_وقت_جدول_Configure(اشاريو, domain, duration);
    أرجع EXCEPTION_NONE;
}

ساكن فراغ invoke_Domain_وقت_جدول_سيٽ_شروعات(لفظ_t اشاريو)
{
    ks_Dom_وقت_جدول_شروعات = اشاريو;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ks_Domain_وقت = 0;
    ks_Dom_وقت_جدول_Idx = dom_وقت_جدول_ڊيگهائي - 2;
    rescheduleRequired();
}

ساكن exception_t decode_Domain_وقت_جدول_سيٽ_شروعات(لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    لفظ_t اشاريو;

    إذا (unlikely(ڊيگهائي < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    اشاريو = getSyscallArg(0, بفر);

    إذا (اشاريو >= dom_وقت_جدول_ڊيگهائي) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_وقت_جدول_ڊيگهائي - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (dschedule_is_پڇاڙي_marker(اشاريو)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_دليل_عدد = 0;
        current_syscall_error.type = se_L4_Invalid_دليل;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_وقت_جدول_سيٽ_شروعات(اشاريو);
    أرجع EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(لفظ_t invLabel, لفظ_t ڊيگهائي, لفظ_t *بفر)
{
    بدل (invLabel) {
    حالة Domain_سيٽ_سيٽ:
        أرجع decode_Domain_سيٽ_سيٽ_Invocation(ڊيگهائي, بفر);
    حالة Domain_وقت_جدول_Configure:
        أرجع decode_Domain_وقت_جدول_Configure(ڊيگهائي, بفر);
    حالة Domain_وقت_جدول_سيٽ_شروعات:
        أرجع decode_Domain_وقت_جدول_سيٽ_شروعات(ڊيگهائي, بفر);
    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
