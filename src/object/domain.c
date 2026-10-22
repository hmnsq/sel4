/*
 * Copyright 2014, General Dynamics C4 Systems
 * Copyright 2025, Indan Zupancic
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#अन्तर्भाव <util.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <mode/api/ipc_buffer.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <object/domain.h>
#अन्तर्भाव <object/structures.h>

/* Domain schedules. The duration is in kernel ticks for non-MCS and timer ticks for MCS. */
dschedule_t ks_Dom_कालसारणी[CONFIG_अङ्कः_DOMAIN_SCHEDULES];

स्थिर शून्य invoke_Domain_समुच्चयः_समुच्चयः(tcb_t *tcb, dom_t domain)
{
    prepare_समुच्चयः_Domain(tcb, domain);
    समुच्चयः_Domain(tcb, domain);
}

स्थिर exception_t decode_Domain_समुच्चयः_समुच्चयः_Invocation(word_t दीर्घता, word_t *बफर)
{
    dom_t domain;
    cap_t tcap;

    यदि (unlikely(दीर्घता == 0)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    } अन्यथा {
        domain = getSyscallArg(0, बफर);
        यदि (domain >= अङ्कः_Domains) {
            userError("Domain Configure: invalid domain (%lu >= %u).",
                      domain, अङ्कः_Domains);
            current_syscall_error.type = se_L4_Invalid_तर्कः;
            current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
    }

    यदि (unlikely(current_extra_caps.excaprefs[0] == रिक्त)) {
        userError("Domain Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    tcap = current_extra_caps.excaprefs[0]->cap;
    यदि (unlikely(cap_get_capType(tcap) != cap_thread_cap)) {
        userError("Domain Configure: thread cap required.");
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_समुच्चयः_समुच्चयः(TCB_PTR(cap_thread_cap_get_capTCBPtr(tcap)), domain);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर शून्य invoke_Domain_कालसारणी_Configure(word_t सूचकांक, dom_t domain, ticks_t duration)
{
    ks_Dom_कालसारणी[सूचकांक] = dschedule_make(domain, duration);
}

स्थिर exception_t decode_Domain_कालसारणी_Configure(word_t दीर्घता, word_t *बफर)
{
    word_t सूचकांक;
    dom_t domain;
    ticks_t duration;

    यदि (unlikely(दीर्घता < 2 + TIME_ARG_आकार)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    सूचकांक = getSyscallArg(0, बफर);
    domain = getSyscallArg(1, बफर);
    duration = बहुलकम्_parse_Time_Arg(2, बफर);

    /* Last entry must stay an end marker, hence the -1: */
    यदि (सूचकांक >= dom_कालसारणी_दीर्घता - 1) {
        userError("Domain Schedule Configure: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_कालसारणी_दीर्घता - 2;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (domain >= अङ्कः_Domains) {
        userError("Domain Schedule Configure: Invalid domain.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = अङ्कः_Domains - 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (duration > DSCHED_MAX_DURATION) {
        userError("Domain Schedule Configure: Duration must fit in 56 bits.");
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 2;
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (duration == 0 && domain != 0) {
        userError("Domain Schedule Configure: Both domain and duration must be zero for end markers.");
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 1;
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (सूचकांक == ks_Dom_कालसारणी_Start && duration == 0) {
        userError("Domain Schedule Configure: Starting schedule's duration must not be zero.");
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 2;
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_कालसारणी_Configure(सूचकांक, domain, duration);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर शून्य invoke_Domain_कालसारणी_समुच्चयः_Start(word_t सूचकांक)
{
    ks_Dom_कालसारणी_Start = सूचकांक;
    /* End the current domain schedule and force an
     * immediate switch to the new starting index: */
    ksDomainTime = 0;
    ks_Dom_कालसारणी_Idx = dom_कालसारणी_दीर्घता - 2;
    rescheduleRequired();
}

स्थिर exception_t decode_Domain_कालसारणी_समुच्चयः_Start(word_t दीर्घता, word_t *बफर)
{
    word_t सूचकांक;

    यदि (unlikely(दीर्घता < 1)) {
        userError("Domain Schedule Configure: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    सूचकांक = getSyscallArg(0, बफर);

    यदि (सूचकांक >= dom_कालसारणी_दीर्घता) {
        userError("Domain Schedule Set Start: Invalid index.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = dom_कालसारणी_दीर्घता - 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (dschedule_is_अन्तः_marker(सूचकांक)) {
        userError("Domain Schedule Set Start: Starting schedule must not be an end marker.");
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    invoke_Domain_कालसारणी_समुच्चयः_Start(सूचकांक);
    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeDomainInvocation(word_t invLabel, word_t दीर्घता, word_t *बफर)
{
    विकल्प (invLabel) {
    पक्ष Domain_समुच्चयः_समुच्चयः:
        प्रत्यागम decode_Domain_समुच्चयः_समुच्चयः_Invocation(दीर्घता, बफर);
    पक्ष Domain_कालसारणी_Configure:
        प्रत्यागम decode_Domain_कालसारणी_Configure(दीर्घता, बफर);
    पक्ष Domain_कालसारणी_समुच्चयः_Start:
        प्रत्यागम decode_Domain_कालसारणी_समुच्चयः_Start(दीर्घता, बफर);
    स्वभाव:
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}
