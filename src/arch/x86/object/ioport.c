/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <arch/object/ioport.h>
#ಅನ್ತರ್ಭಾವ <arch/api/invocation.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/io.h>

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ apply_pattern(word_t_may_alias *w, word_t pattern, bool_t set)
{
    ಯದಿ (set) {
        *w |= pattern;
    } ಅನ್ಯಥಾ {
        *w &= ~pattern;
    }
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ word_t make_pattern(ಪೂರ್ಣಾಙ್ಕ start, ಪೂರ್ಣಾಙ್ಕ end)
{
    // number of bits we want to have set
    ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_bits = end - start;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    ಪ್ರತ್ಯಾಗಮ (~(word_t)0) >> (CONFIG_WORD_SIZE - ಅಂಕೆ_bits) << start;
}

ಸ್ಥಿರ exception_t ensurePortOperationAllowed(cap_t cap, uint32_t start_port, uint32_t size)
{
    uint32_t first_allowed = cap_io_port_cap_get_capIOPortFirstPort(cap);
    uint32_t last_allowed = cap_io_port_cap_get_capIOPortLastPort(cap);
    uint32_t end_port = start_port + size - 1;
    ಪ್ರತಿಜ್ಞಾ(first_allowed <= last_allowed);
    ಪ್ರತಿಜ್ಞಾ(start_port <= end_port);

    ಯದಿ ((start_port < first_allowed) || (end_port > last_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (ಪೂರ್ಣಾಙ್ಕ)start_port, (ಪೂರ್ಣಾಙ್ಕ)end_port,
                  (ಪೂರ್ಣಾಙ್ಕ)first_allowed, (ಪೂರ್ಣಾಙ್ಕ)last_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಶೂನ್ಯ freeIOPortRange(uint16_t first_port, uint16_t last_port)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, false);
}

ಸ್ಥಿರ bool_t isIOPortRangeFree(uint16_t first_port, uint16_t last_port)
{
    ಪೂರ್ಣಾಙ್ಕ ಕೆಳಗಿನ_word = first_port >> wordRadix;
    ಪೂರ್ಣಾಙ್ಕ ಮೇಲಿನ_word = last_port >> wordRadix;
    ಪೂರ್ಣಾಙ್ಕ ಕೆಳಗಿನ_index = first_port & MASK(wordRadix);
    ಪೂರ್ಣಾಙ್ಕ ಮೇಲಿನ_index = last_port & MASK(wordRadix);

    // check if we are operating on a partial word
    ಯದಿ (ಕೆಳಗಿನ_word == ಮೇಲಿನ_word) {
        ಯದಿ ((x86KSAllocatedIOPorts[ಕೆಳಗಿನ_word] & make_pattern(ಕೆಳಗಿನ_index, ಮೇಲಿನ_index + 1)) != 0) {
            ಪ್ರತ್ಯಾಗಮ false;
        }
        ಪ್ರತ್ಯಾಗಮ true;
    }
    // check the starting word
    ಯದಿ ((x86KSAllocatedIOPorts[ಕೆಳಗಿನ_word] & make_pattern(ಕೆಳಗಿನ_index, CONFIG_WORD_SIZE)) != 0) {
        ಪ್ರತ್ಯಾಗಮ false;
    }
    ಕೆಳಗಿನ_word++;
    // check the rest of the whole words
    ಯಾವತ್ (ಕೆಳಗಿನ_word < ಮೇಲಿನ_word) {
        ಯದಿ (x86KSAllocatedIOPorts[ಕೆಳಗಿನ_word] != 0) {
            ಪ್ರತ್ಯಾಗಮ false;
        }
        ಕೆಳಗಿನ_word++;
    }
    // check any trailing bits
    ಯದಿ ((x86KSAllocatedIOPorts[ಕೆಳಗಿನ_word] & make_pattern(0, ಮೇಲಿನ_index + 1)) != 0) {
        ಪ್ರತ್ಯಾಗಮ false;
    }
    ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ exception_t invokeX86PortControl(uint16_t first_port, uint16_t last_port, cte_t *ioportSlot, cte_t *controlSlot)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, true);
    cte_ಸೇರಿಸು(cap_io_port_cap_new(first_port, last_port
#ifdef CONFIG_VTX
                                  , VPID_INVALID
#endif
                                 ),
              controlSlot, ioportSlot);

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
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

    ಯದಿ (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (length < 4 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    first_port = getSyscallArg(0, buffer) & 0xffff;
    last_port = getSyscallArg(1, buffer) & 0xffff;
    index = getSyscallArg(2, buffer);
    depth = getSyscallArg(3, buffer);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    ಯದಿ (last_port < first_port) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    ಯದಿ (!isIOPortRangeFree(first_port, last_port)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = seL4_RevokeFirst;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    ಯದಿ (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(buffer, 0));
        ಪ್ರತ್ಯಾಗಮ lu_ret.status;
    }
    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    ಯದಿ (status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(buffer, 0));
        ಪ್ರತ್ಯಾಗಮ status;
    }

    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeX86PortControl(first_port, last_port, destSlot, slot);
}

ಸ್ಥಿರ exception_t invokeX86PortIn(word_t invLabel, uint16_t port, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);

    ವಿಕಲ್ಪ (invLabel) {
    ಪಕ್ಷ X86IOPortIn8:
        res = in8(port);
        ಭಙ್ಗ;
    ಪಕ್ಷ X86IOPortIn16:
        res = in16(port);
        ಭಙ್ಗ;
    ಪಕ್ಷ X86IOPortIn32:
        res = in32(port);
        ಭಙ್ಗ;
    }

    ಯದಿ (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned ಪೂರ್ಣಾಙ್ಕ length = setMR(thread, ipcBuffer, 0, res);
        setRegister(thread, ಸಂದೇಶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t invokeX86PortOut(word_t invLabel, uint16_t port, uint32_t data)
{
    ವಿಕಲ್ಪ (invLabel) {
    ಪಕ್ಷ X86IOPortOut8:
        out8(port, data);
        ಭಙ್ಗ;
    ಪಕ್ಷ X86IOPortOut16:
        out16(port, data);
        ಭಙ್ಗ;
    ಪಕ್ಷ X86IOPortOut32:
        out32(port, data);
        ಭಙ್ಗ;
    }

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
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

    ಯದಿ (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        ಯದಿ (length < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t port = getSyscallArg(0, buffer) & 0xffff;
        ವಿಕಲ್ಪ (invLabel) {
        ಪಕ್ಷ X86IOPortIn8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            ಭಙ್ಗ;
        ಪಕ್ಷ X86IOPortIn16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            ಭಙ್ಗ;
        ಪಕ್ಷ X86IOPortIn32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            ಭಙ್ಗ;
        }
        ಯದಿ (ret != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ ret;
        }
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ invokeX86PortIn(invLabel, port, call);
    } ಅನ್ಯಥಾ ಯದಿ (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        ಯದಿ (length < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
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

        ವಿಕಲ್ಪ (invLabel) {
        ಪಕ್ಷ X86IOPortOut8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            data = raw_data & 0xff;
            ಭಙ್ಗ;
        ಪಕ್ಷ X86IOPortOut16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            data = raw_data & 0xffff;
            ಭಙ್ಗ;
        ಪಕ್ಷ X86IOPortOut32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            data = raw_data & 0xffffffff;
            ಭಙ್ಗ;
        }
        ಯದಿ (ret != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ ret;
        }
        setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಪ್ರತ್ಯಾಗಮ invokeX86PortOut(invLabel, port, data);
    } ಅನ್ಯಥಾ {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
}

ಶೂನ್ಯ setIOPortMask(ಶೂನ್ಯ *ioport_bitmap, uint16_t ಕೆಳಗಿನ, uint16_t ಮೇಲಿನ, bool_t set)
{
    //get an aliasing pointer
    word_t_may_alias *bitmap = ioport_bitmap;

    ಪೂರ್ಣಾಙ್ಕ ಕೆಳಗಿನ_word = ಕೆಳಗಿನ >> wordRadix;
    ಪೂರ್ಣಾಙ್ಕ ಮೇಲಿನ_word = ಮೇಲಿನ >> wordRadix;
    ಪೂರ್ಣಾಙ್ಕ ಕೆಳಗಿನ_index = ಕೆಳಗಿನ & MASK(wordRadix);
    ಪೂರ್ಣಾಙ್ಕ ಮೇಲಿನ_index = ಮೇಲಿನ & MASK(wordRadix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    ಯದಿ (ಕೆಳಗಿನ_word == ಮೇಲಿನ_word) {
        apply_pattern(bitmap + ಕೆಳಗಿನ_word, make_pattern(ಕೆಳಗಿನ_index, ಮೇಲಿನ_index + 1), set);
    } ಅನ್ಯಥಾ {
        // operate on the potentially partial first word
        apply_pattern(bitmap + ಕೆಳಗಿನ_word, make_pattern(ಕೆಳಗಿನ_index, CONFIG_WORD_SIZE), set);
        ಕೆಳಗಿನ_word++;
        // iterate over the whole words
        ಯಾವತ್ (ಕೆಳಗಿನ_word < ಮೇಲಿನ_word) {
            apply_pattern(bitmap + ಕೆಳಗಿನ_word, ~(word_t)0, set);
            ಕೆಳಗಿನ_word++;
        }
        // apply to any remaining bits
        apply_pattern(bitmap + ಕೆಳಗಿನ_word, make_pattern(0, ಮೇಲಿನ_index + 1), set);
    }
}
