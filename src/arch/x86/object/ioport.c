/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <arch/object/ioport.h>
#अन्तर्भाव <arch/api/invocation.h>
#अन्तर्भाव <plat/machine/io.h>

स्थिर अन्तःस्थ शून्य apply_आदर्शः(word_t_may_alias *w, word_t आदर्शः, bool_t समुच्चयः)
{
    यदि (समुच्चयः) {
        *w |= आदर्शः;
    } अन्यथा {
        *w &= ~आदर्शः;
    }
}

स्थिर अन्तःस्थ word_t make_आदर्शः(पूर्णाङ्क start, पूर्णाङ्क अन्तः_2)
{
    // number of bits we want to have set
    पूर्णाङ्क अङ्कः_द्वयङ्काः = अन्तः_2 - start;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    प्रत्यागम (~(word_t)0) >> (CONFIG_WORD_आकार - अङ्कः_द्वयङ्काः) << start;
}

स्थिर exception_t ensure_पत्तनम्_Operation_Allowed(cap_t cap, uint32_t start_पत्तनम्, uint32_t आकार)
{
    uint32_t first_allowed = cap_io_पत्तनम्_cap_get_cap_IOPort_First_पत्तनम्(cap);
    uint32_t चरमः_allowed = cap_io_पत्तनम्_cap_get_cap_IOPort_चरमः_पत्तनम्(cap);
    uint32_t अन्तः_पत्तनम् = start_पत्तनम् + आकार - 1;
    प्रतिज्ञा(first_allowed <= चरमः_allowed);
    प्रतिज्ञा(start_पत्तनम् <= अन्तः_पत्तनम्);

    यदि ((start_पत्तनम् < first_allowed) || (अन्तः_पत्तनम् > चरमः_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (पूर्णाङ्क)start_पत्तनम्, (पूर्णाङ्क)अन्तः_पत्तनम्,
                  (पूर्णाङ्क)first_allowed, (पूर्णाङ्क)चरमः_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    प्रत्यागम EXCEPTION_NONE;
}

शून्य freeIOPortRange(uint16_t first_पत्तनम्, uint16_t चरमः_पत्तनम्)
{
    समुच्चयः_IOPort_आवरणम्(x86KSAllocatedIOPorts, first_पत्तनम्, चरमः_पत्तनम्, false);
}

स्थिर bool_t isIOPortRangeFree(uint16_t first_पत्तनम्, uint16_t चरमः_पत्तनम्)
{
    पूर्णाङ्क नीचः_word = first_पत्तनम् >> wordRadix;
    पूर्णाङ्क उच्चः_word = चरमः_पत्तनम् >> wordRadix;
    पूर्णाङ्क नीचः_सूचकांक = first_पत्तनम् & आवरणम्(wordRadix);
    पूर्णाङ्क उच्चः_सूचकांक = चरमः_पत्तनम् & आवरणम्(wordRadix);

    // check if we are operating on a partial word
    यदि (नीचः_word == उच्चः_word) {
        यदि ((x86KSAllocatedIOPorts[नीचः_word] & make_आदर्शः(नीचः_सूचकांक, उच्चः_सूचकांक + 1)) != 0) {
            प्रत्यागम false;
        }
        प्रत्यागम true;
    }
    // check the starting word
    यदि ((x86KSAllocatedIOPorts[नीचः_word] & make_आदर्शः(नीचः_सूचकांक, CONFIG_WORD_आकार)) != 0) {
        प्रत्यागम false;
    }
    नीचः_word++;
    // check the rest of the whole words
    यावत् (नीचः_word < उच्चः_word) {
        यदि (x86KSAllocatedIOPorts[नीचः_word] != 0) {
            प्रत्यागम false;
        }
        नीचः_word++;
    }
    // check any trailing bits
    यदि ((x86KSAllocatedIOPorts[नीचः_word] & make_आदर्शः(0, उच्चः_सूचकांक + 1)) != 0) {
        प्रत्यागम false;
    }
    प्रत्यागम true;
}

स्थिर exception_t invoke_X86_पत्तनम्_Control(uint16_t first_पत्तनम्, uint16_t चरमः_पत्तनम्, cte_t *ioportSlot, cte_t *controlSlot)
{
    समुच्चयः_IOPort_आवरणम्(x86KSAllocatedIOPorts, first_पत्तनम्, चरमः_पत्तनम्, true);
    cte_निवेशय(cap_io_पत्तनम्_cap_new(first_पत्तनम्, चरमः_पत्तनम्
#यदि_परिभाषितम् CONFIG_VTX
                                  , VPID_INVALID
#यदि_अन्तः
                                 ),
              controlSlot, ioportSlot);

    प्रत्यागम EXCEPTION_NONE;
}

exception_t decode_X86_पत्तनम्_Control_Invocation(
    word_t invLabel,
    word_t दीर्घता,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    word_t *बफर
)
{
    uint16_t first_पत्तनम्;
    uint16_t चरमः_पत्तनम्;
    word_t सूचकांक, गभीरता;
    cap_t cnodeCap;
    cte_t *destSlot;
    lookupSlot_ret_t lu_ret;
    exception_t स्थितिः;

    यदि (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (दीर्घता < 4 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    first_पत्तनम् = getSyscallArg(0, बफर) & 0xffff;
    चरमः_पत्तनम् = getSyscallArg(1, बफर) & 0xffff;
    सूचकांक = getSyscallArg(2, बफर);
    गभीरता = getSyscallArg(3, बफर);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    यदि (चरमः_पत्तनम् < first_पत्तनम्) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (!isIOPortRangeFree(first_पत्तनम्, चरमः_पत्तनम्)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = seL4_RevokeFirst;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, सूचकांक, गभीरता);
    यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(बफर, 0));
        प्रत्यागम lu_ret.स्थितिः;
    }
    destSlot = lu_ret.slot;

    स्थितिः = ensure_रिक्तानि_Slot(destSlot);
    यदि (स्थितिः != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(बफर, 0));
        प्रत्यागम स्थितिः;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invoke_X86_पत्तनम्_Control(first_पत्तनम्, चरमः_पत्तनम्, destSlot, slot);
}

स्थिर exception_t invoke_X86_पत्तनम्_अन्तर्(word_t invLabel, uint16_t पत्तनम्, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = ग्रन्थिः_STATE(ksCurThread);

    विकल्प (invLabel) {
    पक्ष X86IOPortIn8:
        res = in8(पत्तनम्);
        भङ्ग;
    पक्ष X86IOPortIn16:
        res = in16(पत्तनम्);
        भङ्ग;
    पक्ष X86IOPortIn32:
        res = in32(पत्तनम्);
        भङ्ग;
    }

    यदि (call) {
        word_t *ipc_बफर = lookupIPCBuffer(true, thread);
        समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);
        अचिह्नित पूर्णाङ्क दीर्घता = समुच्चयः_MR(thread, ipc_बफर, 0, res);
        समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                        se_L4_सन्देशः_Info_new(0, 0, 0, दीर्घता)));
    }
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t invoke_X86_पत्तनम्_बहिर्गमनम्(word_t invLabel, uint16_t पत्तनम्, uint32_t दत्त)
{
    विकल्प (invLabel) {
    पक्ष X86IOPortOut8:
        out8(पत्तनम्, दत्त);
        भङ्ग;
    पक्ष X86IOPortOut16:
        out16(पत्तनम्, दत्त);
        भङ्ग;
    पक्ष X86IOPortOut32:
        out32(पत्तनम्, दत्त);
        भङ्ग;
    }

    प्रत्यागम EXCEPTION_NONE;
}

exception_t decode_X86_पत्तनम्_Invocation(
    word_t invLabel,
    word_t दीर्घता,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *बफर
)
{
    exception_t ret = EXCEPTION_NONE;

    यदि (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        यदि (दीर्घता < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_सन्देशः;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t पत्तनम् = getSyscallArg(0, बफर) & 0xffff;
        विकल्प (invLabel) {
        पक्ष X86IOPortIn8:
            ret = ensure_पत्तनम्_Operation_Allowed(cap, पत्तनम्, 1);
            भङ्ग;
        पक्ष X86IOPortIn16:
            ret = ensure_पत्तनम्_Operation_Allowed(cap, पत्तनम्, 2);
            भङ्ग;
        पक्ष X86IOPortIn32:
            ret = ensure_पत्तनम्_Operation_Allowed(cap, पत्तनम्, 4);
            भङ्ग;
        }
        यदि (ret != EXCEPTION_NONE) {
            प्रत्यागम ret;
        }
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम invoke_X86_पत्तनम्_अन्तर्(invLabel, पत्तनम्, call);
    } अन्यथा यदि (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        यदि (दीर्घता < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_सन्देशः;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to write to. */
        uint16_t पत्तनम् = getSyscallArg(0, बफर) & 0xffff;
        seL4_Word raw_दत्त = getSyscallArg(1, बफर);
        /* We construct the value for data from raw_data based on the actual size of the port
           operation. This ensures that there is no 'random' user data left over in the value
           passed to invokeX86PortOut. Whilst invokeX86PortOut will ignore any extra data and
           cast down to the correct word size removing the extra here is currently relied upon
           for verification */
        uint32_t दत्त = 0;

        विकल्प (invLabel) {
        पक्ष X86IOPortOut8:
            ret = ensure_पत्तनम्_Operation_Allowed(cap, पत्तनम्, 1);
            दत्त = raw_दत्त & 0xff;
            भङ्ग;
        पक्ष X86IOPortOut16:
            ret = ensure_पत्तनम्_Operation_Allowed(cap, पत्तनम्, 2);
            दत्त = raw_दत्त & 0xffff;
            भङ्ग;
        पक्ष X86IOPortOut32:
            ret = ensure_पत्तनम्_Operation_Allowed(cap, पत्तनम्, 4);
            दत्त = raw_दत्त & 0xffffffff;
            भङ्ग;
        }
        यदि (ret != EXCEPTION_NONE) {
            प्रत्यागम ret;
        }
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम invoke_X86_पत्तनम्_बहिर्गमनम्(invLabel, पत्तनम्, दत्त);
    } अन्यथा {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}

शून्य समुच्चयः_IOPort_आवरणम्(शून्य *ioport_bitmap, uint16_t नीचः, uint16_t उच्चः, bool_t समुच्चयः)
{
    //get an aliasing pointer
    word_t_may_alias *bitmap = ioport_bitmap;

    पूर्णाङ्क नीचः_word = नीचः >> wordRadix;
    पूर्णाङ्क उच्चः_word = उच्चः >> wordRadix;
    पूर्णाङ्क नीचः_सूचकांक = नीचः & आवरणम्(wordRadix);
    पूर्णाङ्क उच्चः_सूचकांक = उच्चः & आवरणम्(wordRadix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    यदि (नीचः_word == उच्चः_word) {
        apply_आदर्शः(bitmap + नीचः_word, make_आदर्शः(नीचः_सूचकांक, उच्चः_सूचकांक + 1), समुच्चयः);
    } अन्यथा {
        // operate on the potentially partial first word
        apply_आदर्शः(bitmap + नीचः_word, make_आदर्शः(नीचः_सूचकांक, CONFIG_WORD_आकार), समुच्चयः);
        नीचः_word++;
        // iterate over the whole words
        यावत् (नीचः_word < उच्चः_word) {
            apply_आदर्शः(bitmap + नीचः_word, ~(word_t)0, समुच्चयः);
            नीचः_word++;
        }
        // apply to any remaining bits
        apply_आदर्शः(bitmap + नीचः_word, make_आदर्शः(0, उच्चः_सूचकांक + 1), समुच्चयः);
    }
}
