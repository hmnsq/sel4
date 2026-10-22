/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <kernel/thread.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <api/syscall.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <arch/object/ioport.h>
#અન્તર્ભાવ <arch/api/invocation.h>
#અન્તર્ભાવ <plat/machine/io.h>

સ્થિર અંતઃરેખ શૂન્ય apply_pattern(word_t_may_alias *w, word_t pattern, bool_t set)
{
    જો (set) {
        *w |= pattern;
    } નહીંતર {
        *w &= ~pattern;
    }
}

સ્થિર અંતઃરેખ word_t make_pattern(પૂર્ણાંક start, પૂર્ણાંક end)
{
    // number of bits we want to have set
    પૂર્ણાંક અંક_bits = end - start;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    પાછા (~(word_t)0) >> (CONFIG_WORD_SIZE - અંક_bits) << start;
}

સ્થિર exception_t ensurePortOperationAllowed(cap_t cap, uint32_t start_port, uint32_t size)
{
    uint32_t first_allowed = cap_io_port_cap_get_capIOPortFirstPort(cap);
    uint32_t last_allowed = cap_io_port_cap_get_capIOPortLastPort(cap);
    uint32_t end_port = start_port + size - 1;
    પ્રતિજ્ઞા(first_allowed <= last_allowed);
    પ્રતિજ્ઞા(start_port <= end_port);

    જો ((start_port < first_allowed) || (end_port > last_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (પૂર્ણાંક)start_port, (પૂર્ણાંક)end_port,
                  (પૂર્ણાંક)first_allowed, (પૂર્ણાંક)last_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    પાછા EXCEPTION_NONE;
}

શૂન્ય freeIOPortRange(uint16_t first_port, uint16_t last_port)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, false);
}

સ્થિર bool_t isIOPortRangeFree(uint16_t first_port, uint16_t last_port)
{
    પૂર્ણાંક નીચલું_word = first_port >> wordRadix;
    પૂર્ણાંક ઉપલું_word = last_port >> wordRadix;
    પૂર્ણાંક નીચલું_index = first_port & MASK(wordRadix);
    પૂર્ણાંક ઉપલું_index = last_port & MASK(wordRadix);

    // check if we are operating on a partial word
    જો (નીચલું_word == ઉપલું_word) {
        જો ((x86KSAllocatedIOPorts[નીચલું_word] & make_pattern(નીચલું_index, ઉપલું_index + 1)) != 0) {
            પાછા false;
        }
        પાછા true;
    }
    // check the starting word
    જો ((x86KSAllocatedIOPorts[નીચલું_word] & make_pattern(નીચલું_index, CONFIG_WORD_SIZE)) != 0) {
        પાછા false;
    }
    નીચલું_word++;
    // check the rest of the whole words
    જ્યારે (નીચલું_word < ઉપલું_word) {
        જો (x86KSAllocatedIOPorts[નીચલું_word] != 0) {
            પાછા false;
        }
        નીચલું_word++;
    }
    // check any trailing bits
    જો ((x86KSAllocatedIOPorts[નીચલું_word] & make_pattern(0, ઉપલું_index + 1)) != 0) {
        પાછા false;
    }
    પાછા true;
}

સ્થિર exception_t invokeX86PortControl(uint16_t first_port, uint16_t last_port, cte_t *ioportSlot, cte_t *controlSlot)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, true);
    cte_ઉમેરો(cap_io_port_cap_new(first_port, last_port
#ifdef CONFIG_VTX
                                  , VPID_INVALID
#endif
                                 ),
              controlSlot, ioportSlot);

    પાછા EXCEPTION_NONE;
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

    જો (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (length < 4 || current_extra_caps.excaprefs[0] == ખાલી) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    first_port = getSyscallArg(0, buffer) & 0xffff;
    last_port = getSyscallArg(1, buffer) & 0xffff;
    index = getSyscallArg(2, buffer);
    depth = getSyscallArg(3, buffer);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    જો (last_port < first_port) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_સંખ્યા = 1;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    જો (!isIOPortRangeFree(first_port, last_port)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = seL4_RevokeFirst;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    જો (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(buffer, 0));
        પાછા lu_ret.status;
    }
    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    જો (status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(buffer, 0));
        પાછા status;
    }

    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
    પાછા invokeX86PortControl(first_port, last_port, destSlot, slot);
}

સ્થિર exception_t invokeX86PortIn(word_t invLabel, uint16_t port, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = ગાંઠ_STATE(ksCurThread);

    પસંદગી (invLabel) {
    સ્થિતિ X86IOPortIn8:
        res = in8(port);
        તોડો;
    સ્થિતિ X86IOPortIn16:
        res = in16(port);
        તોડો;
    સ્થિતિ X86IOPortIn32:
        res = in32(port);
        તોડો;
    }

    જો (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned પૂર્ણાંક length = setMR(thread, ipcBuffer, 0, res);
        setRegister(thread, સંદેશ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Running);
    પાછા EXCEPTION_NONE;
}

સ્થિર exception_t invokeX86PortOut(word_t invLabel, uint16_t port, uint32_t data)
{
    પસંદગી (invLabel) {
    સ્થિતિ X86IOPortOut8:
        out8(port, data);
        તોડો;
    સ્થિતિ X86IOPortOut16:
        out16(port, data);
        તોડો;
    સ્થિતિ X86IOPortOut32:
        out32(port, data);
        તોડો;
    }

    પાછા EXCEPTION_NONE;
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

    જો (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        જો (length < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            પાછા EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t port = getSyscallArg(0, buffer) & 0xffff;
        પસંદગી (invLabel) {
        સ્થિતિ X86IOPortIn8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            તોડો;
        સ્થિતિ X86IOPortIn16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            તોડો;
        સ્થિતિ X86IOPortIn32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            તોડો;
        }
        જો (ret != EXCEPTION_NONE) {
            પાછા ret;
        }
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા invokeX86PortIn(invLabel, port, call);
    } નહીંતર જો (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        જો (length < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            પાછા EXCEPTION_SYSCALL_ERROR;
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

        પસંદગી (invLabel) {
        સ્થિતિ X86IOPortOut8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            data = raw_data & 0xff;
            તોડો;
        સ્થિતિ X86IOPortOut16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            data = raw_data & 0xffff;
            તોડો;
        સ્થિતિ X86IOPortOut32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            data = raw_data & 0xffffffff;
            તોડો;
        }
        જો (ret != EXCEPTION_NONE) {
            પાછા ret;
        }
        setThreadState(ગાંઠ_STATE(ksCurThread), ThreadState_Restart);
        પાછા invokeX86PortOut(invLabel, port, data);
    } નહીંતર {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        પાછા EXCEPTION_SYSCALL_ERROR;
    }
}

શૂન્ય setIOPortMask(શૂન્ય *ioport_bitmap, uint16_t નીચલું, uint16_t ઉપલું, bool_t set)
{
    //get an aliasing pointer
    word_t_may_alias *bitmap = ioport_bitmap;

    પૂર્ણાંક નીચલું_word = નીચલું >> wordRadix;
    પૂર્ણાંક ઉપલું_word = ઉપલું >> wordRadix;
    પૂર્ણાંક નીચલું_index = નીચલું & MASK(wordRadix);
    પૂર્ણાંક ઉપલું_index = ઉપલું & MASK(wordRadix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    જો (નીચલું_word == ઉપલું_word) {
        apply_pattern(bitmap + નીચલું_word, make_pattern(નીચલું_index, ઉપલું_index + 1), set);
    } નહીંતર {
        // operate on the potentially partial first word
        apply_pattern(bitmap + નીચલું_word, make_pattern(નીચલું_index, CONFIG_WORD_SIZE), set);
        નીચલું_word++;
        // iterate over the whole words
        જ્યારે (નીચલું_word < ઉપલું_word) {
            apply_pattern(bitmap + નીચલું_word, ~(word_t)0, set);
            નીચલું_word++;
        }
        // apply to any remaining bits
        apply_pattern(bitmap + નીચલું_word, make_pattern(0, ઉપલું_index + 1), set);
    }
}
