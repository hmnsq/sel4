/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <kernel/thread.h>
#অন্তর্ভাৱ <api/failures.h>
#অন্তর্ভাৱ <api/syscall.h>
#অন্তর্ভাৱ <machine/io.h>
#অন্তর্ভাৱ <arch/object/ioport.h>
#অন্তর্ভাৱ <arch/api/invocation.h>
#অন্তর্ভাৱ <plat/machine/io.h>

স্থির অন্তঃস্থ শূন্য apply_pattern(word_t_may_alias *w, word_t pattern, bool_t set)
{
    যদি (set) {
        *w |= pattern;
    } অন্যথা {
        *w &= ~pattern;
    }
}

স্থির অন্তঃস্থ word_t make_pattern(পূর্ণাঙ্ক start, পূর্ণাঙ্ক end)
{
    // number of bits we want to have set
    পূর্ণাঙ্ক অঙ্ক_bits = end - start;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    ঘূৰাই (~(word_t)0) >> (CONFIG_WORD_SIZE - অঙ্ক_bits) << start;
}

স্থির exception_t ensurePortOperationAllowed(cap_t cap, uint32_t start_port, uint32_t size)
{
    uint32_t first_allowed = cap_io_port_cap_get_capIOPortFirstPort(cap);
    uint32_t last_allowed = cap_io_port_cap_get_capIOPortLastPort(cap);
    uint32_t end_port = start_port + size - 1;
    প্রতিজ্ঞা(first_allowed <= last_allowed);
    প্রতিজ্ঞা(start_port <= end_port);

    যদি ((start_port < first_allowed) || (end_port > last_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (পূর্ণাঙ্ক)start_port, (পূর্ণাঙ্ক)end_port,
                  (পূর্ণাঙ্ক)first_allowed, (পূর্ণাঙ্ক)last_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    ঘূৰাই EXCEPTION_NONE;
}

শূন্য freeIOPortRange(uint16_t first_port, uint16_t last_port)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, false);
}

স্থির bool_t isIOPortRangeFree(uint16_t first_port, uint16_t last_port)
{
    পূর্ণাঙ্ক নিচেৰ_word = first_port >> wordRadix;
    পূর্ণাঙ্ক উপৰেৰ_word = last_port >> wordRadix;
    পূর্ণাঙ্ক নিচেৰ_index = first_port & MASK(wordRadix);
    পূর্ণাঙ্ক উপৰেৰ_index = last_port & MASK(wordRadix);

    // check if we are operating on a partial word
    যদি (নিচেৰ_word == উপৰেৰ_word) {
        যদি ((x86KSAllocatedIOPorts[নিচেৰ_word] & make_pattern(নিচেৰ_index, উপৰেৰ_index + 1)) != 0) {
            ঘূৰাই false;
        }
        ঘূৰাই true;
    }
    // check the starting word
    যদি ((x86KSAllocatedIOPorts[নিচেৰ_word] & make_pattern(নিচেৰ_index, CONFIG_WORD_SIZE)) != 0) {
        ঘূৰাই false;
    }
    নিচেৰ_word++;
    // check the rest of the whole words
    যেতিয়ালৈ (নিচেৰ_word < উপৰেৰ_word) {
        যদি (x86KSAllocatedIOPorts[নিচেৰ_word] != 0) {
            ঘূৰাই false;
        }
        নিচেৰ_word++;
    }
    // check any trailing bits
    যদি ((x86KSAllocatedIOPorts[নিচেৰ_word] & make_pattern(0, উপৰেৰ_index + 1)) != 0) {
        ঘূৰাই false;
    }
    ঘূৰাই true;
}

স্থির exception_t invokeX86PortControl(uint16_t first_port, uint16_t last_port, cte_t *ioportSlot, cte_t *controlSlot)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, true);
    cte_ঢোকাও(cap_io_port_cap_new(first_port, last_port
#ifdef CONFIG_VTX
                                  , VPID_INVALID
#endif
                                 ),
              controlSlot, ioportSlot);

    ঘূৰাই EXCEPTION_NONE;
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

    যদি (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    যদি (length < 4 || current_extra_caps.excaprefs[0] == রিক্ত) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    first_port = getSyscallArg(0, buffer) & 0xffff;
    last_port = getSyscallArg(1, buffer) & 0xffff;
    index = getSyscallArg(2, buffer);
    depth = getSyscallArg(3, buffer);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    যদি (last_port < first_port) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 1;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    যদি (!isIOPortRangeFree(first_port, last_port)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = seL4_RevokeFirst;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    যদি (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(buffer, 0));
        ঘূৰাই lu_ret.status;
    }
    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    যদি (status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(buffer, 0));
        ঘূৰাই status;
    }

    setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
    ঘূৰাই invokeX86PortControl(first_port, last_port, destSlot, slot);
}

স্থির exception_t invokeX86PortIn(word_t invLabel, uint16_t port, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = গ্ৰন্থি_STATE(ksCurThread);

    ৱিকল্প (invLabel) {
    পক্ষ X86IOPortIn8:
        res = in8(port);
        ভঙ্গ;
    পক্ষ X86IOPortIn16:
        res = in16(port);
        ভঙ্গ;
    পক্ষ X86IOPortIn32:
        res = in32(port);
        ভঙ্গ;
    }

    যদি (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned পূর্ণাঙ্ক length = setMR(thread, ipcBuffer, 0, res);
        setRegister(thread, বাৰ্তা_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Running);
    ঘূৰাই EXCEPTION_NONE;
}

স্থির exception_t invokeX86PortOut(word_t invLabel, uint16_t port, uint32_t data)
{
    ৱিকল্প (invLabel) {
    পক্ষ X86IOPortOut8:
        out8(port, data);
        ভঙ্গ;
    পক্ষ X86IOPortOut16:
        out16(port, data);
        ভঙ্গ;
    পক্ষ X86IOPortOut32:
        out32(port, data);
        ভঙ্গ;
    }

    ঘূৰাই EXCEPTION_NONE;
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

    যদি (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        যদি (length < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t port = getSyscallArg(0, buffer) & 0xffff;
        ৱিকল্প (invLabel) {
        পক্ষ X86IOPortIn8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            ভঙ্গ;
        পক্ষ X86IOPortIn16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            ভঙ্গ;
        পক্ষ X86IOPortIn32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            ভঙ্গ;
        }
        যদি (ret != EXCEPTION_NONE) {
            ঘূৰাই ret;
        }
        setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
        ঘূৰাই invokeX86PortIn(invLabel, port, call);
    } অন্যথা যদি (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        যদি (length < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ঘূৰাই EXCEPTION_SYSCALL_ERROR;
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

        ৱিকল্প (invLabel) {
        পক্ষ X86IOPortOut8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            data = raw_data & 0xff;
            ভঙ্গ;
        পক্ষ X86IOPortOut16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            data = raw_data & 0xffff;
            ভঙ্গ;
        পক্ষ X86IOPortOut32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            data = raw_data & 0xffffffff;
            ভঙ্গ;
        }
        যদি (ret != EXCEPTION_NONE) {
            ঘূৰাই ret;
        }
        setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
        ঘূৰাই invokeX86PortOut(invLabel, port, data);
    } অন্যথা {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
}

শূন্য setIOPortMask(শূন্য *ioport_bitmap, uint16_t নিচেৰ, uint16_t উপৰেৰ, bool_t set)
{
    //get an aliasing pointer
    word_t_may_alias *bitmap = ioport_bitmap;

    পূর্ণাঙ্ক নিচেৰ_word = নিচেৰ >> wordRadix;
    পূর্ণাঙ্ক উপৰেৰ_word = উপৰেৰ >> wordRadix;
    পূর্ণাঙ্ক নিচেৰ_index = নিচেৰ & MASK(wordRadix);
    পূর্ণাঙ্ক উপৰেৰ_index = উপৰেৰ & MASK(wordRadix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    যদি (নিচেৰ_word == উপৰেৰ_word) {
        apply_pattern(bitmap + নিচেৰ_word, make_pattern(নিচেৰ_index, উপৰেৰ_index + 1), set);
    } অন্যথা {
        // operate on the potentially partial first word
        apply_pattern(bitmap + নিচেৰ_word, make_pattern(নিচেৰ_index, CONFIG_WORD_SIZE), set);
        নিচেৰ_word++;
        // iterate over the whole words
        যেতিয়ালৈ (নিচেৰ_word < উপৰেৰ_word) {
            apply_pattern(bitmap + নিচেৰ_word, ~(word_t)0, set);
            নিচেৰ_word++;
        }
        // apply to any remaining bits
        apply_pattern(bitmap + নিচেৰ_word, make_pattern(0, উপৰেৰ_index + 1), set);
    }
}
