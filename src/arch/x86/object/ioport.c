/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <api/syscall.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/ioport.h>
#ଅନ୍ତର୍ଭାଵ <arch/api/invocation.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/io.h>

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ apply_pattern(word_t_may_alias *w, word_t pattern, bool_t set)
{
    ଯଦି (set) {
        *w |= pattern;
    } ଅନ୍ୟଥା {
        *w &= ~pattern;
    }
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ word_t make_pattern(ପୂର୍ଣ୍ଣସଂଖ୍ୟା start, ପୂର୍ଣ୍ଣସଂଖ୍ୟା end)
{
    // number of bits we want to have set
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_bits = end - start;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    ଫେରନ୍ତୁ (~(word_t)0) >> (CONFIG_WORD_SIZE - ଅଙ୍କ_bits) << start;
}

ସ୍ଥିର exception_t ensurePortOperationAllowed(cap_t cap, uint32_t start_port, uint32_t size)
{
    uint32_t first_allowed = cap_io_port_cap_get_capIOPortFirstPort(cap);
    uint32_t last_allowed = cap_io_port_cap_get_capIOPortLastPort(cap);
    uint32_t end_port = start_port + size - 1;
    ପ୍ରତିଜ୍ଞା(first_allowed <= last_allowed);
    ପ୍ରତିଜ୍ଞା(start_port <= end_port);

    ଯଦି ((start_port < first_allowed) || (end_port > last_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)start_port, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)end_port,
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)first_allowed, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)last_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ଶୂନ୍ୟ freeIOPortRange(uint16_t first_port, uint16_t last_port)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, false);
}

ସ୍ଥିର bool_t isIOPortRangeFree(uint16_t first_port, uint16_t last_port)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ତଳ_word = first_port >> wordRadix;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଉପର_word = last_port >> wordRadix;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ତଳ_index = first_port & MASK(wordRadix);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଉପର_index = last_port & MASK(wordRadix);

    // check if we are operating on a partial word
    ଯଦି (ତଳ_word == ଉପର_word) {
        ଯଦି ((x86KSAllocatedIOPorts[ତଳ_word] & make_pattern(ତଳ_index, ଉପର_index + 1)) != 0) {
            ଫେରନ୍ତୁ false;
        }
        ଫେରନ୍ତୁ true;
    }
    // check the starting word
    ଯଦି ((x86KSAllocatedIOPorts[ତଳ_word] & make_pattern(ତଳ_index, CONFIG_WORD_SIZE)) != 0) {
        ଫେରନ୍ତୁ false;
    }
    ତଳ_word++;
    // check the rest of the whole words
    ଯେତେବେଳେ (ତଳ_word < ଉପର_word) {
        ଯଦି (x86KSAllocatedIOPorts[ତଳ_word] != 0) {
            ଫେରନ୍ତୁ false;
        }
        ତଳ_word++;
    }
    // check any trailing bits
    ଯଦି ((x86KSAllocatedIOPorts[ତଳ_word] & make_pattern(0, ଉପର_index + 1)) != 0) {
        ଫେରନ୍ତୁ false;
    }
    ଫେରନ୍ତୁ true;
}

ସ୍ଥିର exception_t invokeX86PortControl(uint16_t first_port, uint16_t last_port, cte_t *ioportSlot, cte_t *controlSlot)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, true);
    cte_ଭର୍ତ୍ତି(cap_io_port_cap_new(first_port, last_port
#ifdef CONFIG_VTX
                                  , VPID_INVALID
#endif
                                 ),
              controlSlot, ioportSlot);

    ଫେରନ୍ତୁ EXCEPTION_NONE;
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

    ଯଦି (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଯଦି (length < 4 || current_extra_caps.excaprefs[0] == ଖାଲି) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    first_port = getSyscallArg(0, buffer) & 0xffff;
    last_port = getSyscallArg(1, buffer) & 0xffff;
    index = getSyscallArg(2, buffer);
    depth = getSyscallArg(3, buffer);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    ଯଦି (last_port < first_port) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    ଯଦି (!isIOPortRangeFree(first_port, last_port)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = seL4_RevokeFirst;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    ଯଦି (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(buffer, 0));
        ଫେରନ୍ତୁ lu_ret.status;
    }
    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    ଯଦି (status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(buffer, 0));
        ଫେରନ୍ତୁ status;
    }

    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeX86PortControl(first_port, last_port, destSlot, slot);
}

ସ୍ଥିର exception_t invokeX86PortIn(word_t invLabel, uint16_t port, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = ଗଣ୍ଠି_STATE(ksCurThread);

    ବାଛ (invLabel) {
    ସ୍ଥିତି X86IOPortIn8:
        res = in8(port);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି X86IOPortIn16:
        res = in16(port);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି X86IOPortIn32:
        res = in32(port);
        ଭାଙ୍ଗ;
    }

    ଯଦି (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା length = setMR(thread, ipcBuffer, 0, res);
        setRegister(thread, ସନ୍ଦେଶ_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Running);
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

ସ୍ଥିର exception_t invokeX86PortOut(word_t invLabel, uint16_t port, uint32_t data)
{
    ବାଛ (invLabel) {
    ସ୍ଥିତି X86IOPortOut8:
        out8(port, data);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି X86IOPortOut16:
        out16(port, data);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି X86IOPortOut32:
        out32(port, data);
        ଭାଙ୍ଗ;
    }

    ଫେରନ୍ତୁ EXCEPTION_NONE;
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

    ଯଦି (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        ଯଦି (length < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t port = getSyscallArg(0, buffer) & 0xffff;
        ବାଛ (invLabel) {
        ସ୍ଥିତି X86IOPortIn8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି X86IOPortIn16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            ଭାଙ୍ଗ;
        ସ୍ଥିତି X86IOPortIn32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            ଭାଙ୍ଗ;
        }
        ଯଦି (ret != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ ret;
        }
        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ invokeX86PortIn(invLabel, port, call);
    } ଅନ୍ୟଥା ଯଦି (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        ଯଦି (length < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
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

        ବାଛ (invLabel) {
        ସ୍ଥିତି X86IOPortOut8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            data = raw_data & 0xff;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି X86IOPortOut16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            data = raw_data & 0xffff;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି X86IOPortOut32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            data = raw_data & 0xffffffff;
            ଭାଙ୍ଗ;
        }
        ଯଦି (ret != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ ret;
        }
        setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
        ଫେରନ୍ତୁ invokeX86PortOut(invLabel, port, data);
    } ଅନ୍ୟଥା {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
}

ଶୂନ୍ୟ setIOPortMask(ଶୂନ୍ୟ *ioport_bitmap, uint16_t ତଳ, uint16_t ଉପର, bool_t set)
{
    //get an aliasing pointer
    word_t_may_alias *bitmap = ioport_bitmap;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ତଳ_word = ତଳ >> wordRadix;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଉପର_word = ଉପର >> wordRadix;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ତଳ_index = ତଳ & MASK(wordRadix);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଉପର_index = ଉପର & MASK(wordRadix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    ଯଦି (ତଳ_word == ଉପର_word) {
        apply_pattern(bitmap + ତଳ_word, make_pattern(ତଳ_index, ଉପର_index + 1), set);
    } ଅନ୍ୟଥା {
        // operate on the potentially partial first word
        apply_pattern(bitmap + ତଳ_word, make_pattern(ତଳ_index, CONFIG_WORD_SIZE), set);
        ତଳ_word++;
        // iterate over the whole words
        ଯେତେବେଳେ (ତଳ_word < ଉପର_word) {
            apply_pattern(bitmap + ତଳ_word, ~(word_t)0, set);
            ତଳ_word++;
        }
        // apply to any remaining bits
        apply_pattern(bitmap + ତଳ_word, make_pattern(0, ଉପର_index + 1), set);
    }
}
