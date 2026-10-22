/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <kernel/thread.h>
#അന്തര്ഭാവ <api/failures.h>
#അന്തര്ഭാവ <api/syscall.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <arch/object/ioport.h>
#അന്തര്ഭാവ <arch/api/invocation.h>
#അന്തര്ഭാവ <plat/machine/io.h>

സ്ഥിതം വരിയിൽ ശൂന്യം apply_pattern(word_t_may_alias *w, word_t pattern, bool_t set)
{
    എങ്കിൽ (set) {
        *w |= pattern;
    } അല്ലെങ്കിൽ {
        *w &= ~pattern;
    }
}

സ്ഥിതം വരിയിൽ word_t make_pattern(പൂർണ്ണസംഖ്യ start, പൂർണ്ണസംഖ്യ end)
{
    // number of bits we want to have set
    പൂർണ്ണസംഖ്യ അക്കം_bits = end - start;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    തിരികെ (~(word_t)0) >> (CONFIG_WORD_SIZE - അക്കം_bits) << start;
}

സ്ഥിതം exception_t ensurePortOperationAllowed(cap_t cap, uint32_t start_port, uint32_t size)
{
    uint32_t first_allowed = cap_io_port_cap_get_capIOPortFirstPort(cap);
    uint32_t last_allowed = cap_io_port_cap_get_capIOPortLastPort(cap);
    uint32_t end_port = start_port + size - 1;
    പ്രതിജ്ഞാ(first_allowed <= last_allowed);
    പ്രതിജ്ഞാ(start_port <= end_port);

    എങ്കിൽ ((start_port < first_allowed) || (end_port > last_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (പൂർണ്ണസംഖ്യ)start_port, (പൂർണ്ണസംഖ്യ)end_port,
                  (പൂർണ്ണസംഖ്യ)first_allowed, (പൂർണ്ണസംഖ്യ)last_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    തിരികെ EXCEPTION_NONE;
}

ശൂന്യം freeIOPortRange(uint16_t first_port, uint16_t last_port)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, false);
}

സ്ഥിതം bool_t isIOPortRangeFree(uint16_t first_port, uint16_t last_port)
{
    പൂർണ്ണസംഖ്യ താഴെയുള്ള_word = first_port >> wordRadix;
    പൂർണ്ണസംഖ്യ മുകളിലെ_word = last_port >> wordRadix;
    പൂർണ്ണസംഖ്യ താഴെയുള്ള_index = first_port & MASK(wordRadix);
    പൂർണ്ണസംഖ്യ മുകളിലെ_index = last_port & MASK(wordRadix);

    // check if we are operating on a partial word
    എങ്കിൽ (താഴെയുള്ള_word == മുകളിലെ_word) {
        എങ്കിൽ ((x86KSAllocatedIOPorts[താഴെയുള്ള_word] & make_pattern(താഴെയുള്ള_index, മുകളിലെ_index + 1)) != 0) {
            തിരികെ false;
        }
        തിരികെ true;
    }
    // check the starting word
    എങ്കിൽ ((x86KSAllocatedIOPorts[താഴെയുള്ള_word] & make_pattern(താഴെയുള്ള_index, CONFIG_WORD_SIZE)) != 0) {
        തിരികെ false;
    }
    താഴെയുള്ള_word++;
    // check the rest of the whole words
    ആകുമ്പോൾ (താഴെയുള്ള_word < മുകളിലെ_word) {
        എങ്കിൽ (x86KSAllocatedIOPorts[താഴെയുള്ള_word] != 0) {
            തിരികെ false;
        }
        താഴെയുള്ള_word++;
    }
    // check any trailing bits
    എങ്കിൽ ((x86KSAllocatedIOPorts[താഴെയുള്ള_word] & make_pattern(0, മുകളിലെ_index + 1)) != 0) {
        തിരികെ false;
    }
    തിരികെ true;
}

സ്ഥിതം exception_t invokeX86PortControl(uint16_t first_port, uint16_t last_port, cte_t *ioportSlot, cte_t *controlSlot)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, true);
    cte_തിരുകുക(cap_io_port_cap_new(first_port, last_port
#ifdef CONFIG_VTX
                                  , VPID_INVALID
#endif
                                 ),
              controlSlot, ioportSlot);

    തിരികെ EXCEPTION_NONE;
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

    എങ്കിൽ (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    എങ്കിൽ (length < 4 || current_extra_caps.excaprefs[0] == ശൂന്യ) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    first_port = getSyscallArg(0, buffer) & 0xffff;
    last_port = getSyscallArg(1, buffer) & 0xffff;
    index = getSyscallArg(2, buffer);
    depth = getSyscallArg(3, buffer);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    എങ്കിൽ (last_port < first_port) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_സംഖ്യ = 1;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    എങ്കിൽ (!isIOPortRangeFree(first_port, last_port)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = seL4_RevokeFirst;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    എങ്കിൽ (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(buffer, 0));
        തിരികെ lu_ret.status;
    }
    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    എങ്കിൽ (status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(buffer, 0));
        തിരികെ status;
    }

    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ invokeX86PortControl(first_port, last_port, destSlot, slot);
}

സ്ഥിതം exception_t invokeX86PortIn(word_t invLabel, uint16_t port, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = കണ്ണി_STATE(ksCurThread);

    തിരഞ്ഞെടുപ്പ് (invLabel) {
    അവസ്ഥ X86IOPortIn8:
        res = in8(port);
        നിർത്തുക;
    അവസ്ഥ X86IOPortIn16:
        res = in16(port);
        നിർത്തുക;
    അവസ്ഥ X86IOPortIn32:
        res = in32(port);
        നിർത്തുക;
    }

    എങ്കിൽ (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned പൂർണ്ണസംഖ്യ length = setMR(thread, ipcBuffer, 0, res);
        setRegister(thread, സന്ദേശം_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Running);
    തിരികെ EXCEPTION_NONE;
}

സ്ഥിതം exception_t invokeX86PortOut(word_t invLabel, uint16_t port, uint32_t data)
{
    തിരഞ്ഞെടുപ്പ് (invLabel) {
    അവസ്ഥ X86IOPortOut8:
        out8(port, data);
        നിർത്തുക;
    അവസ്ഥ X86IOPortOut16:
        out16(port, data);
        നിർത്തുക;
    അവസ്ഥ X86IOPortOut32:
        out32(port, data);
        നിർത്തുക;
    }

    തിരികെ EXCEPTION_NONE;
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

    എങ്കിൽ (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        എങ്കിൽ (length < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t port = getSyscallArg(0, buffer) & 0xffff;
        തിരഞ്ഞെടുപ്പ് (invLabel) {
        അവസ്ഥ X86IOPortIn8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            നിർത്തുക;
        അവസ്ഥ X86IOPortIn16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            നിർത്തുക;
        അവസ്ഥ X86IOPortIn32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            നിർത്തുക;
        }
        എങ്കിൽ (ret != EXCEPTION_NONE) {
            തിരികെ ret;
        }
        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ invokeX86PortIn(invLabel, port, call);
    } അല്ലെങ്കിൽ എങ്കിൽ (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        എങ്കിൽ (length < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            തിരികെ EXCEPTION_SYSCALL_ERROR;
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

        തിരഞ്ഞെടുപ്പ് (invLabel) {
        അവസ്ഥ X86IOPortOut8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            data = raw_data & 0xff;
            നിർത്തുക;
        അവസ്ഥ X86IOPortOut16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            data = raw_data & 0xffff;
            നിർത്തുക;
        അവസ്ഥ X86IOPortOut32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            data = raw_data & 0xffffffff;
            നിർത്തുക;
        }
        എങ്കിൽ (ret != EXCEPTION_NONE) {
            തിരികെ ret;
        }
        setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
        തിരികെ invokeX86PortOut(invLabel, port, data);
    } അല്ലെങ്കിൽ {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
}

ശൂന്യം setIOPortMask(ശൂന്യം *ioport_bitmap, uint16_t താഴെയുള്ള, uint16_t മുകളിലെ, bool_t set)
{
    //get an aliasing pointer
    word_t_may_alias *bitmap = ioport_bitmap;

    പൂർണ്ണസംഖ്യ താഴെയുള്ള_word = താഴെയുള്ള >> wordRadix;
    പൂർണ്ണസംഖ്യ മുകളിലെ_word = മുകളിലെ >> wordRadix;
    പൂർണ്ണസംഖ്യ താഴെയുള്ള_index = താഴെയുള്ള & MASK(wordRadix);
    പൂർണ്ണസംഖ്യ മുകളിലെ_index = മുകളിലെ & MASK(wordRadix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    എങ്കിൽ (താഴെയുള്ള_word == മുകളിലെ_word) {
        apply_pattern(bitmap + താഴെയുള്ള_word, make_pattern(താഴെയുള്ള_index, മുകളിലെ_index + 1), set);
    } അല്ലെങ്കിൽ {
        // operate on the potentially partial first word
        apply_pattern(bitmap + താഴെയുള്ള_word, make_pattern(താഴെയുള്ള_index, CONFIG_WORD_SIZE), set);
        താഴെയുള്ള_word++;
        // iterate over the whole words
        ആകുമ്പോൾ (താഴെയുള്ള_word < മുകളിലെ_word) {
            apply_pattern(bitmap + താഴെയുള്ള_word, ~(word_t)0, set);
            താഴെയുള്ള_word++;
        }
        // apply to any remaining bits
        apply_pattern(bitmap + താഴെയുള്ള_word, make_pattern(0, മുകളിലെ_index + 1), set);
    }
}
