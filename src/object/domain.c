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
dschedule_t ks_Dom_زمانبندی[CONFIG_رقم_DOMAIN_SCHEDULES];

ساكن فراغ invoke_Domain_مجموعه_مجموعه(tcb_t *tcb, dom_t domain)
{
    prepare_مجموعه_Domain(tcb, domain);
    مجموعه_Domain(tcb, domain);
}

ساكن exception_t decode_Domain_مجموعه_مجموعه_Invocation(کلمه_t درازی, کلمه_t *میانگیر)
{
    dom_t domain;
    cap_t tcap;

    إذا (unlikely(درازی == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    } وإلا {
        domain = getSyscallArg(0, میانگیر);
        إذا (domain >= رقم_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, رقم_Domains);
            current_syscall_error.type = se_L4_Invalid_آرگومان;
            current_syscall_error.invalid_آرگومان_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    إذا (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_مجموعه_مجموعه(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    أرجع EXCEPTION_NONE;
}

ساكن فراغ invoke_Domain_زمانبندی_Configure(کلمه_t شاخص, dom_t domain, ticks_t duration)
{
    ks_Dom_زمانبندی[شاخص] = dschedule_make(domain, duration);
}

ساكن exception_t decode_Domain_زمانبندی_Configure(کلمه_t درازی, کلمه_t *میانگیر)
{
    کلمه_t شاخص;
    dom_t domain;
    ticks_t duration;

    إذا (unlikely(درازی < 2 + زمان_ARG_اندازه)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    شاخص = getSyscallArg(0, میانگیر);
    domain = getSyscallArg(1, میانگیر);
    duration = نما_parse_زمان_Arg(2, میانگیر);

    /* Last entry must stay an end marker, hence the -1: */
    إذا (شاخص >= dom_زمانبندی_درازی - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_زمانبندی_درازی - 2;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (domain >= رقم_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = رقم_Domains - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_آرگومان_عدد = 2;
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_آرگومان_عدد = 1;
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (شاخص == ks_Dom_زمانبندی_شروع && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_آرگومان_عدد = 2;
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_زمانبندی_Configure(شاخص, domain, duration);
    أرجع EXCEPTION_NONE;
}

ساكن فراغ invoke_Domain_زمانبندی_مجموعه_شروع(کلمه_t شاخص)
{
    ks_Dom_زمانبندی_شروع = شاخص;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ks_Domain_زمان = 0;
    ks_Dom_زمانبندی_Idx = dom_زمانبندی_درازی - 2;
    rescheduleRequired();
}

ساكن exception_t decode_Domain_زمانبندی_مجموعه_شروع(کلمه_t درازی, کلمه_t *میانگیر)
{
    کلمه_t شاخص;

    إذا (unlikely(درازی < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    شاخص = getSyscallArg(0, میانگیر);

    إذا (شاخص >= dom_زمانبندی_درازی) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_زمانبندی_درازی - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (dschedule_is_انتها_marker(شاخص)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_آرگومان_عدد = 0;
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_زمانبندی_مجموعه_شروع(شاخص);
    أرجع EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(کلمه_t invLabel, کلمه_t درازی, کلمه_t *میانگیر)
{
    بدل (invLabel) {
    حالة Domain_مجموعه_مجموعه:
        أرجع decode_Domain_مجموعه_مجموعه_Invocation(درازی, میانگیر);
    حالة Domain_زمانبندی_Configure:
        أرجع decode_Domain_زمانبندی_Configure(درازی, میانگیر);
    حالة Domain_زمانبندی_مجموعه_شروع:
        أرجع decode_Domain_زمانبندی_مجموعه_شروع(درازی, میانگیر);
    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
