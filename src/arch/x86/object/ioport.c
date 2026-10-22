/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <kernel/thread.h>
#समावेश <api/failures.h>
#समावेश <api/syscall.h>
#समावेश <machine/io.h>
#समावेश <arch/object/ioport.h>
#समावेश <arch/api/invocation.h>
#समावेश <plat/machine/io.h>

अटल अंतरभूत व्योम apply_pattern(word_t_may_alias *w, word_t pattern, bool_t set)
{
    अगर (set) {
        *w |= pattern;
    } अन्यथा {
        *w &= ~pattern;
    }
}

अटल अंतरभूत word_t make_pattern(पूर्णांक start, पूर्णांक end)
{
    // number of bits we want to have set
    पूर्णांक अंक_bits = end - start;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    वापस (~(word_t)0) >> (CONFIG_WORD_आकार - अंक_bits) << start;
}

अटल exception_t ensurePortOperationAllowed(cap_t cap, uint32_t start_port, uint32_t आकार)
{
    uint32_t first_allowed = cap_io_port_cap_get_capIOPortFirstPort(cap);
    uint32_t last_allowed = cap_io_port_cap_get_capIOPortLastPort(cap);
    uint32_t end_port = start_port + आकार - 1;
    निश्चित(first_allowed <= last_allowed);
    निश्चित(start_port <= end_port);

    अगर ((start_port < first_allowed) || (end_port > last_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (पूर्णांक)start_port, (पूर्णांक)end_port,
                  (पूर्णांक)first_allowed, (पूर्णांक)last_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    वापस EXCEPTION_NONE;
}

व्योम freeIOPortRange(uint16_t first_port, uint16_t last_port)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, false);
}

अटल bool_t isIOPortRangeFree(uint16_t first_port, uint16_t last_port)
{
    पूर्णांक निचला_word = first_port >> wordRadix;
    पूर्णांक ऊपरी_word = last_port >> wordRadix;
    पूर्णांक निचला_सूचकांक = first_port & MASK(wordRadix);
    पूर्णांक ऊपरी_सूचकांक = last_port & MASK(wordRadix);

    // check if we are operating on a partial word
    अगर (निचला_word == ऊपरी_word) {
        अगर ((x86KSAllocatedIOPorts[निचला_word] & make_pattern(निचला_सूचकांक, ऊपरी_सूचकांक + 1)) != 0) {
            वापस false;
        }
        वापस true;
    }
    // check the starting word
    अगर ((x86KSAllocatedIOPorts[निचला_word] & make_pattern(निचला_सूचकांक, CONFIG_WORD_आकार)) != 0) {
        वापस false;
    }
    निचला_word++;
    // check the rest of the whole words
    जबतक (निचला_word < ऊपरी_word) {
        अगर (x86KSAllocatedIOPorts[निचला_word] != 0) {
            वापस false;
        }
        निचला_word++;
    }
    // check any trailing bits
    अगर ((x86KSAllocatedIOPorts[निचला_word] & make_pattern(0, ऊपरी_सूचकांक + 1)) != 0) {
        वापस false;
    }
    वापस true;
}

अटल exception_t invokeX86PortControl(uint16_t first_port, uint16_t last_port, cte_t *ioportSlot, cte_t *controlSlot)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, true);
    cte_जोड़ो(cap_io_port_cap_new(first_port, last_port
#अगर_घोषित CONFIG_VTX
                                  , VPID_INVALID
#पूर्ण_अगर
                                 ),
              controlSlot, ioportSlot);

    वापस EXCEPTION_NONE;
}

exception_t decodeX86PortControlInvocation(
    word_t invLabel,
    word_t लंबाई,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    word_t *बफर
)
{
    uint16_t first_port;
    uint16_t last_port;
    word_t सूचकांक, depth;
    cap_t cnodeCap;
    cte_t *destSlot;
    lookupSlot_ret_t lu_ret;
    exception_t status;

    अगर (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (लंबाई < 4 || current_extra_caps.excaprefs[0] == शून्य) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    first_port = getSyscallArg(0, बफर) & 0xffff;
    last_port = getSyscallArg(1, बफर) & 0xffff;
    सूचकांक = getSyscallArg(2, बफर);
    depth = getSyscallArg(3, बफर);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    अगर (last_port < first_port) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (!isIOPortRangeFree(first_port, last_port)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = seL4_RevokeFirst;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, सूचकांक, depth);
    अगर (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(बफर, 0));
        वापस lu_ret.status;
    }
    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    अगर (status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(बफर, 0));
        वापस status;
    }

    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeX86PortControl(first_port, last_port, destSlot, slot);
}

अटल exception_t invokeX86PortIn(word_t invLabel, uint16_t port, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = गाँठ_STATE(ksCurThread);

    चयन (invLabel) {
    हाल X86IOPortIn8:
        res = in8(port);
        अवरोध;
    हाल X86IOPortIn16:
        res = in16(port);
        अवरोध;
    हाल X86IOPortIn32:
        res = in32(port);
        अवरोध;
    }

    अगर (call) {
        word_t *ipc_बफर = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        अचिन्हित पूर्णांक लंबाई = setMR(thread, ipc_बफर, 0, res);
        setRegister(thread, संदेश_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, लंबाई)));
    }
    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Running);
    वापस EXCEPTION_NONE;
}

अटल exception_t invokeX86PortOut(word_t invLabel, uint16_t port, uint32_t तथ्य)
{
    चयन (invLabel) {
    हाल X86IOPortOut8:
        out8(port, तथ्य);
        अवरोध;
    हाल X86IOPortOut16:
        out16(port, तथ्य);
        अवरोध;
    हाल X86IOPortOut32:
        out32(port, तथ्य);
        अवरोध;
    }

    वापस EXCEPTION_NONE;
}

exception_t decodeX86PortInvocation(
    word_t invLabel,
    word_t लंबाई,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *बफर
)
{
    exception_t ret = EXCEPTION_NONE;

    अगर (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        अगर (लंबाई < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t port = getSyscallArg(0, बफर) & 0xffff;
        चयन (invLabel) {
        हाल X86IOPortIn8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            अवरोध;
        हाल X86IOPortIn16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            अवरोध;
        हाल X86IOPortIn32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            अवरोध;
        }
        अगर (ret != EXCEPTION_NONE) {
            वापस ret;
        }
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस invokeX86PortIn(invLabel, port, call);
    } अन्यथा अगर (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        अगर (लंबाई < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to write to. */
        uint16_t port = getSyscallArg(0, बफर) & 0xffff;
        seL4_Word raw_तथ्य = getSyscallArg(1, बफर);
        /* We construct the value for data from raw_data based on the actual size of the port
           operation. This ensures that there is no 'random' user data left over in the value
           passed to invokeX86PortOut. Whilst invokeX86PortOut will ignore any extra data and
           cast down to the correct word size removing the extra here is currently relied upon
           for verification */
        uint32_t तथ्य = 0;

        चयन (invLabel) {
        हाल X86IOPortOut8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            तथ्य = raw_तथ्य & 0xff;
            अवरोध;
        हाल X86IOPortOut16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            तथ्य = raw_तथ्य & 0xffff;
            अवरोध;
        हाल X86IOPortOut32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            तथ्य = raw_तथ्य & 0xffffffff;
            अवरोध;
        }
        अगर (ret != EXCEPTION_NONE) {
            वापस ret;
        }
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस invokeX86PortOut(invLabel, port, तथ्य);
    } अन्यथा {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
}

व्योम setIOPortMask(व्योम *ioport_bitmap, uint16_t निचला, uint16_t ऊपरी, bool_t set)
{
    //get an aliasing pointer
    word_t_may_alias *bitmap = ioport_bitmap;

    पूर्णांक निचला_word = निचला >> wordRadix;
    पूर्णांक ऊपरी_word = ऊपरी >> wordRadix;
    पूर्णांक निचला_सूचकांक = निचला & MASK(wordRadix);
    पूर्णांक ऊपरी_सूचकांक = ऊपरी & MASK(wordRadix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    अगर (निचला_word == ऊपरी_word) {
        apply_pattern(bitmap + निचला_word, make_pattern(निचला_सूचकांक, ऊपरी_सूचकांक + 1), set);
    } अन्यथा {
        // operate on the potentially partial first word
        apply_pattern(bitmap + निचला_word, make_pattern(निचला_सूचकांक, CONFIG_WORD_आकार), set);
        निचला_word++;
        // iterate over the whole words
        जबतक (निचला_word < ऊपरी_word) {
            apply_pattern(bitmap + निचला_word, ~(word_t)0, set);
            निचला_word++;
        }
        // apply to any remaining bits
        apply_pattern(bitmap + निचला_word, make_pattern(0, ऊपरी_सूचकांक + 1), set);
    }
}
