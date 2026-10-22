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
    जइ (set) {
        *w |= pattern;
    } अण्णहा {
        *w &= ~pattern;
    }
}

स्थिर अन्तःस्थ word_t make_pattern(पूर्णाङ्क start, पूर्णाङ्क end)
{
    // number of bits we want to have set
    पूर्णाङ्क अङ्कः_bits = end - start;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    प्रत्यागम (~(word_t)0) >> (CONFIG_WORD_SIZE - अङ्कः_bits) << start;
}

स्थिर exception_t ensurePortOperationAllowed(cap_t cap, uint32_t start_port, uint32_t size)
{
    uint32_t first_allowed = cap_io_port_cap_get_capIOPortFirstPort(cap);
    uint32_t last_allowed = cap_io_port_cap_get_capIOPortLastPort(cap);
    uint32_t end_port = start_port + size - 1;
    प्रतिज्ञा(first_allowed <= last_allowed);
    प्रतिज्ञा(start_port <= end_port);

    जइ ((start_port < first_allowed) || (end_port > last_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (पूर्णाङ्क)start_port, (पूर्णाङ्क)end_port,
                  (पूर्णाङ्क)first_allowed, (पूर्णाङ्क)last_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    प्रत्यागम EXCEPTION_NONE;
}

शून्य freeIOPortRange(uint16_t first_port, uint16_t last_port)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, false);
}

स्थिर bool_t isIOPortRangeFree(uint16_t first_port, uint16_t last_port)
{
    पूर्णाङ्क नीचः_word = first_port >> wordRadix;
    पूर्णाङ्क उच्चः_word = last_port >> wordRadix;
    पूर्णाङ्क नीचः_index = first_port & MASK(wordRadix);
    पूर्णाङ्क उच्चः_index = last_port & MASK(wordRadix);

    // check if we are operating on a partial word
    जइ (नीचः_word == उच्चः_word) {
        जइ ((x86KSAllocatedIOPorts[नीचः_word] & make_pattern(नीचः_index, उच्चः_index + 1)) != 0) {
            प्रत्यागम false;
        }
        प्रत्यागम true;
    }
    // check the starting word
    जइ ((x86KSAllocatedIOPorts[नीचः_word] & make_pattern(नीचः_index, CONFIG_WORD_SIZE)) != 0) {
        प्रत्यागम false;
    }
    नीचः_word++;
    // check the rest of the whole words
    जाव (नीचः_word < उच्चः_word) {
        जइ (x86KSAllocatedIOPorts[नीचः_word] != 0) {
            प्रत्यागम false;
        }
        नीचः_word++;
    }
    // check any trailing bits
    जइ ((x86KSAllocatedIOPorts[नीचः_word] & make_pattern(0, उच्चः_index + 1)) != 0) {
        प्रत्यागम false;
    }
    प्रत्यागम true;
}

स्थिर exception_t invokeX86PortControl(uint16_t first_port, uint16_t last_port, cte_t *ioportSlot, cte_t *controlSlot)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, true);
    cte_निवेशय(cap_io_port_cap_new(first_port, last_port
#ifdef CONFIG_VTX
                                  , VPID_INVALID
#endif
                                 ),
              controlSlot, ioportSlot);

    प्रत्यागम EXCEPTION_NONE;
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

    जइ (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    जइ (length < 4 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    first_port = getSyscallArg(0, buffer) & 0xffff;
    last_port = getSyscallArg(1, buffer) & 0xffff;
    index = getSyscallArg(2, buffer);
    depth = getSyscallArg(3, buffer);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    जइ (last_port < first_port) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    जइ (!isIOPortRangeFree(first_port, last_port)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = seL4_RevokeFirst;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    जइ (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(buffer, 0));
        प्रत्यागम lu_ret.status;
    }
    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    जइ (status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(buffer, 0));
        प्रत्यागम status;
    }

    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeX86PortControl(first_port, last_port, destSlot, slot);
}

स्थिर exception_t invokeX86PortIn(word_t invLabel, uint16_t port, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = ग्रन्थिः_STATE(ksCurThread);

    विकल्प (invLabel) {
    पक्ष X86IOPortIn8:
        res = in8(port);
        भङ्ग;
    पक्ष X86IOPortIn16:
        res = in16(port);
        भङ्ग;
    पक्ष X86IOPortIn32:
        res = in32(port);
        भङ्ग;
    }

    जइ (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned पूर्णाङ्क length = setMR(thread, ipcBuffer, 0, res);
        setRegister(thread, सन्देशः_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर exception_t invokeX86PortOut(word_t invLabel, uint16_t port, uint32_t data)
{
    विकल्प (invLabel) {
    पक्ष X86IOPortOut8:
        out8(port, data);
        भङ्ग;
    पक्ष X86IOPortOut16:
        out16(port, data);
        भङ्ग;
    पक्ष X86IOPortOut32:
        out32(port, data);
        भङ्ग;
    }

    प्रत्यागम EXCEPTION_NONE;
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

    जइ (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        जइ (length < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t port = getSyscallArg(0, buffer) & 0xffff;
        विकल्प (invLabel) {
        पक्ष X86IOPortIn8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            भङ्ग;
        पक्ष X86IOPortIn16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            भङ्ग;
        पक्ष X86IOPortIn32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            भङ्ग;
        }
        जइ (ret != EXCEPTION_NONE) {
            प्रत्यागम ret;
        }
        setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम invokeX86PortIn(invLabel, port, call);
    } अण्णहा जइ (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        जइ (length < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
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
            भङ्ग;
        पक्ष X86IOPortOut16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            data = raw_data & 0xffff;
            भङ्ग;
        पक्ष X86IOPortOut32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            data = raw_data & 0xffffffff;
            भङ्ग;
        }
        जइ (ret != EXCEPTION_NONE) {
            प्रत्यागम ret;
        }
        setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम invokeX86PortOut(invLabel, port, data);
    } अण्णहा {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}

शून्य setIOPortMask(शून्य *ioport_bitmap, uint16_t नीचः, uint16_t उच्चः, bool_t set)
{
    //get an aliasing pointer
    word_t_may_alias *bitmap = ioport_bitmap;

    पूर्णाङ्क नीचः_word = नीचः >> wordRadix;
    पूर्णाङ्क उच्चः_word = उच्चः >> wordRadix;
    पूर्णाङ्क नीचः_index = नीचः & MASK(wordRadix);
    पूर्णाङ्क उच्चः_index = उच्चः & MASK(wordRadix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    जइ (नीचः_word == उच्चः_word) {
        apply_pattern(bitmap + नीचः_word, make_pattern(नीचः_index, उच्चः_index + 1), set);
    } अण्णहा {
        // operate on the potentially partial first word
        apply_pattern(bitmap + नीचः_word, make_pattern(नीचः_index, CONFIG_WORD_SIZE), set);
        नीचः_word++;
        // iterate over the whole words
        जाव (नीचः_word < उच्चः_word) {
            apply_pattern(bitmap + नीचः_word, ~(word_t)0, set);
            नीचः_word++;
        }
        // apply to any remaining bits
        apply_pattern(bitmap + नीचः_word, make_pattern(0, उच्चः_index + 1), set);
    }
}
