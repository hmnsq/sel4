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

स्थिर अन्तःस्थ शून्य apply_pattern(word_t_may_alias *w, word_t pattern, bool_t set)
{
    यदि (set) {
        *w |= pattern;
    } अन्यथा {
        *w &= ~pattern;
    }
}

स्थिर अन्तःस्थ word_t make_pattern(पूर्णाङ्क start, पूर्णाङ्क end)
{
    // number of bits we want to have set
    पूर्णाङ्क अङ्क_bits = end - start;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    फर्काऊ (~(word_t)0) >> (CONFIG_WORD_SIZE - अङ्क_bits) << start;
}

स्थिर exception_t ensurePortOperationAllowed(cap_t cap, uint32_t start_port, uint32_t size)
{
    uint32_t first_allowed = cap_io_port_cap_get_capIOPortFirstPort(cap);
    uint32_t last_allowed = cap_io_port_cap_get_capIOPortLastPort(cap);
    uint32_t end_port = start_port + size - 1;
    प्रतिज्ञा(first_allowed <= last_allowed);
    प्रतिज्ञा(start_port <= end_port);

    यदि ((start_port < first_allowed) || (end_port > last_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (पूर्णाङ्क)start_port, (पूर्णाङ्क)end_port,
                  (पूर्णाङ्क)first_allowed, (पूर्णाङ्क)last_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    फर्काऊ EXCEPTION_NONE;
}

शून्य freeIOPortRange(uint16_t first_port, uint16_t last_port)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, false);
}

स्थिर bool_t isIOPortRangeFree(uint16_t first_port, uint16_t last_port)
{
    पूर्णाङ्क तल्लो_word = first_port >> wordRadix;
    पूर्णाङ्क माथिल्लो_word = last_port >> wordRadix;
    पूर्णाङ्क तल्लो_index = first_port & MASK(wordRadix);
    पूर्णाङ्क माथिल्लो_index = last_port & MASK(wordRadix);

    // check if we are operating on a partial word
    यदि (तल्लो_word == माथिल्लो_word) {
        यदि ((x86KSAllocatedIOPorts[तल्लो_word] & make_pattern(तल्लो_index, माथिल्लो_index + 1)) != 0) {
            फर्काऊ false;
        }
        फर्काऊ true;
    }
    // check the starting word
    यदि ((x86KSAllocatedIOPorts[तल्लो_word] & make_pattern(तल्लो_index, CONFIG_WORD_SIZE)) != 0) {
        फर्काऊ false;
    }
    तल्लो_word++;
    // check the rest of the whole words
    जबसम्म (तल्लो_word < माथिल्लो_word) {
        यदि (x86KSAllocatedIOPorts[तल्लो_word] != 0) {
            फर्काऊ false;
        }
        तल्लो_word++;
    }
    // check any trailing bits
    यदि ((x86KSAllocatedIOPorts[तल्लो_word] & make_pattern(0, माथिल्लो_index + 1)) != 0) {
        फर्काऊ false;
    }
    फर्काऊ true;
}

स्थिर exception_t invokeX86PortControl(uint16_t first_port, uint16_t last_port, cte_t *ioportSlot, cte_t *controlSlot)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, true);
    cte_घुसाउ(cap_io_port_cap_new(first_port, last_port
#ifdef CONFIG_VTX
                                  , VPID_INVALID
#endif
                                 ),
              controlSlot, ioportSlot);

    फर्काऊ EXCEPTION_NONE;
}

exception_t decodeX86PortControlInvocation(
    word_t invLabel,
    word_t length,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    word_t *buffer
)
{
    uint16_t first_port;
    uint16_t last_port;
    word_t index, depth;
    cap_t cnodeCap;
    cte_t *destSlot;
    lookupSlot_ret_t lu_ret;
    exception_t status;

    यदि (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (length < 4 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    first_port = getSyscallArg(0, buffer) & 0xffff;
    last_port = getSyscallArg(1, buffer) & 0xffff;
    index = getSyscallArg(2, buffer);
    depth = getSyscallArg(3, buffer);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    यदि (last_port < first_port) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 1;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    यदि (!isIOPortRangeFree(first_port, last_port)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = seL4_RevokeFirst;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    यदि (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(buffer, 0));
        फर्काऊ lu_ret.status;
    }
    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    यदि (status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(buffer, 0));
        फर्काऊ status;
    }

    setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
    फर्काऊ invokeX86PortControl(first_port, last_port, destSlot, slot);
}

स्थिर exception_t invokeX86PortIn(word_t invLabel, uint16_t port, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = गाँठो_STATE(ksCurThread);

    विकल्प (invLabel) {
    पक्ष X86IOPortIn8:
        res = in8(port);
        रोक;
    पक्ष X86IOPortIn16:
        res = in16(port);
        रोक;
    पक्ष X86IOPortIn32:
        res = in32(port);
        रोक;
    }

    यदि (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        अचिन्हित पूर्णाङ्क length = setMR(thread, ipcBuffer, 0, res);
        setRegister(thread, सन्देश_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Running);
    फर्काऊ EXCEPTION_NONE;
}

स्थिर exception_t invokeX86PortOut(word_t invLabel, uint16_t port, uint32_t data)
{
    विकल्प (invLabel) {
    पक्ष X86IOPortOut8:
        out8(port, data);
        रोक;
    पक्ष X86IOPortOut16:
        out16(port, data);
        रोक;
    पक्ष X86IOPortOut32:
        out32(port, data);
        रोक;
    }

    फर्काऊ EXCEPTION_NONE;
}

exception_t decodeX86PortInvocation(
    word_t invLabel,
    word_t length,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *buffer
)
{
    exception_t ret = EXCEPTION_NONE;

    यदि (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        यदि (length < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t port = getSyscallArg(0, buffer) & 0xffff;
        विकल्प (invLabel) {
        पक्ष X86IOPortIn8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            रोक;
        पक्ष X86IOPortIn16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            रोक;
        पक्ष X86IOPortIn32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            रोक;
        }
        यदि (ret != EXCEPTION_NONE) {
            फर्काऊ ret;
        }
        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        फर्काऊ invokeX86PortIn(invLabel, port, call);
    } अन्यथा यदि (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        यदि (length < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            फर्काऊ EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to write to. */
        uint16_t port = getSyscallArg(0, buffer) & 0xffff;
        seL4_Word raw_data = getSyscallArg(1, buffer);
        /* We construct the value for data from raw_data based on the actual size of the port
           operation. This ensures that there is no 'random' user data left over in the value
           passed to invokeX86PortOut. Whilst invokeX86PortOut will ignore any extra data and
           cast down to the correct word size removing the extra here is currently relied upon
           for verification */
        uint32_t data = 0;

        विकल्प (invLabel) {
        पक्ष X86IOPortOut8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            data = raw_data & 0xff;
            रोक;
        पक्ष X86IOPortOut16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            data = raw_data & 0xffff;
            रोक;
        पक्ष X86IOPortOut32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            data = raw_data & 0xffffffff;
            रोक;
        }
        यदि (ret != EXCEPTION_NONE) {
            फर्काऊ ret;
        }
        setThreadState(गाँठो_STATE(ksCurThread), ThreadState_Restart);
        फर्काऊ invokeX86PortOut(invLabel, port, data);
    } अन्यथा {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        फर्काऊ EXCEPTION_SYSCALL_ERROR;
    }
}

शून्य setIOPortMask(शून्य *ioport_bitmap, uint16_t तल्लो, uint16_t माथिल्लो, bool_t set)
{
    //get an aliasing pointer
    word_t_may_alias *bitmap = ioport_bitmap;

    पूर्णाङ्क तल्लो_word = तल्लो >> wordRadix;
    पूर्णाङ्क माथिल्लो_word = माथिल्लो >> wordRadix;
    पूर्णाङ्क तल्लो_index = तल्लो & MASK(wordRadix);
    पूर्णाङ्क माथिल्लो_index = माथिल्लो & MASK(wordRadix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    यदि (तल्लो_word == माथिल्लो_word) {
        apply_pattern(bitmap + तल्लो_word, make_pattern(तल्लो_index, माथिल्लो_index + 1), set);
    } अन्यथा {
        // operate on the potentially partial first word
        apply_pattern(bitmap + तल्लो_word, make_pattern(तल्लो_index, CONFIG_WORD_SIZE), set);
        तल्लो_word++;
        // iterate over the whole words
        जबसम्म (तल्लो_word < माथिल्लो_word) {
            apply_pattern(bitmap + तल्लो_word, ~(word_t)0, set);
            तल्लो_word++;
        }
        // apply to any remaining bits
        apply_pattern(bitmap + तल्लो_word, make_pattern(0, माथिल्लो_index + 1), set);
    }
}
