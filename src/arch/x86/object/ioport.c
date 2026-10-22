/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <arch/object/ioport.h>
#அந்தர்பாவ <arch/api/invocation.h>
#அந்தர்பாவ <plat/machine/io.h>

நிலையான உள்வரி வெறுமை apply_pattern(word_t_may_alias *w, word_t pattern, bool_t set)
{
    எனில் (set) {
        *w |= pattern;
    } இல்லையெனில் {
        *w &= ~pattern;
    }
}

நிலையான உள்வரி word_t make_pattern(முழு start, முழு end)
{
    // number of bits we want to have set
    முழு இலக்கம்_bits = end - start;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    திரும்பு (~(word_t)0) >> (CONFIG_WORD_SIZE - இலக்கம்_bits) << start;
}

நிலையான exception_t ensurePortOperationAllowed(cap_t cap, uint32_t start_port, uint32_t size)
{
    uint32_t first_allowed = cap_io_port_cap_get_capIOPortFirstPort(cap);
    uint32_t last_allowed = cap_io_port_cap_get_capIOPortLastPort(cap);
    uint32_t end_port = start_port + size - 1;
    ப்ரதிஜ்ஞா(first_allowed <= last_allowed);
    ப்ரதிஜ்ஞா(start_port <= end_port);

    எனில் ((start_port < first_allowed) || (end_port > last_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (முழு)start_port, (முழு)end_port,
                  (முழு)first_allowed, (முழு)last_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    திரும்பு EXCEPTION_NONE;
}

வெறுமை freeIOPortRange(uint16_t first_port, uint16_t last_port)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, false);
}

நிலையான bool_t isIOPortRangeFree(uint16_t first_port, uint16_t last_port)
{
    முழு கீழ்_word = first_port >> wordRadix;
    முழு உயர்வு_word = last_port >> wordRadix;
    முழு கீழ்_index = first_port & MASK(wordRadix);
    முழு உயர்வு_index = last_port & MASK(wordRadix);

    // check if we are operating on a partial word
    எனில் (கீழ்_word == உயர்வு_word) {
        எனில் ((x86KSAllocatedIOPorts[கீழ்_word] & make_pattern(கீழ்_index, உயர்வு_index + 1)) != 0) {
            திரும்பு false;
        }
        திரும்பு true;
    }
    // check the starting word
    எனில் ((x86KSAllocatedIOPorts[கீழ்_word] & make_pattern(கீழ்_index, CONFIG_WORD_SIZE)) != 0) {
        திரும்பு false;
    }
    கீழ்_word++;
    // check the rest of the whole words
    போது (கீழ்_word < உயர்வு_word) {
        எனில் (x86KSAllocatedIOPorts[கீழ்_word] != 0) {
            திரும்பு false;
        }
        கீழ்_word++;
    }
    // check any trailing bits
    எனில் ((x86KSAllocatedIOPorts[கீழ்_word] & make_pattern(0, உயர்வு_index + 1)) != 0) {
        திரும்பு false;
    }
    திரும்பு true;
}

நிலையான exception_t invokeX86PortControl(uint16_t first_port, uint16_t last_port, cte_t *ioportSlot, cte_t *controlSlot)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, true);
    cte_செருகு(cap_io_port_cap_new(first_port, last_port
#ifdef CONFIG_VTX
                                  , VPID_INVALID
#endif
                                 ),
              controlSlot, ioportSlot);

    திரும்பு EXCEPTION_NONE;
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

    எனில் (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (length < 4 || current_extra_caps.excaprefs[0] == வெற்று) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    first_port = getSyscallArg(0, buffer) & 0xffff;
    last_port = getSyscallArg(1, buffer) & 0xffff;
    index = getSyscallArg(2, buffer);
    depth = getSyscallArg(3, buffer);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    எனில் (last_port < first_port) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (!isIOPortRangeFree(first_port, last_port)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = seL4_RevokeFirst;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    எனில் (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(buffer, 0));
        திரும்பு lu_ret.status;
    }
    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    எனில் (status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(buffer, 0));
        திரும்பு status;
    }

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeX86PortControl(first_port, last_port, destSlot, slot);
}

நிலையான exception_t invokeX86PortIn(word_t invLabel, uint16_t port, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = முனை_STATE(ksCurThread);

    தேர்வு (invLabel) {
    நிலை X86IOPortIn8:
        res = in8(port);
        நிறுத்து;
    நிலை X86IOPortIn16:
        res = in16(port);
        நிறுத்து;
    நிலை X86IOPortIn32:
        res = in32(port);
        நிறுத்து;
    }

    எனில் (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned முழு length = setMR(thread, ipcBuffer, 0, res);
        setRegister(thread, செய்தி_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Running);
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t invokeX86PortOut(word_t invLabel, uint16_t port, uint32_t data)
{
    தேர்வு (invLabel) {
    நிலை X86IOPortOut8:
        out8(port, data);
        நிறுத்து;
    நிலை X86IOPortOut16:
        out16(port, data);
        நிறுத்து;
    நிலை X86IOPortOut32:
        out32(port, data);
        நிறுத்து;
    }

    திரும்பு EXCEPTION_NONE;
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

    எனில் (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        எனில் (length < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t port = getSyscallArg(0, buffer) & 0xffff;
        தேர்வு (invLabel) {
        நிலை X86IOPortIn8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            நிறுத்து;
        நிலை X86IOPortIn16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            நிறுத்து;
        நிலை X86IOPortIn32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            நிறுத்து;
        }
        எனில் (ret != EXCEPTION_NONE) {
            திரும்பு ret;
        }
        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு invokeX86PortIn(invLabel, port, call);
    } இல்லையெனில் எனில் (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        எனில் (length < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
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

        தேர்வு (invLabel) {
        நிலை X86IOPortOut8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            data = raw_data & 0xff;
            நிறுத்து;
        நிலை X86IOPortOut16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            data = raw_data & 0xffff;
            நிறுத்து;
        நிலை X86IOPortOut32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            data = raw_data & 0xffffffff;
            நிறுத்து;
        }
        எனில் (ret != EXCEPTION_NONE) {
            திரும்பு ret;
        }
        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு invokeX86PortOut(invLabel, port, data);
    } இல்லையெனில் {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
}

வெறுமை setIOPortMask(வெறுமை *ioport_bitmap, uint16_t கீழ், uint16_t உயர்வு, bool_t set)
{
    //get an aliasing pointer
    word_t_may_alias *bitmap = ioport_bitmap;

    முழு கீழ்_word = கீழ் >> wordRadix;
    முழு உயர்வு_word = உயர்வு >> wordRadix;
    முழு கீழ்_index = கீழ் & MASK(wordRadix);
    முழு உயர்வு_index = உயர்வு & MASK(wordRadix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    எனில் (கீழ்_word == உயர்வு_word) {
        apply_pattern(bitmap + கீழ்_word, make_pattern(கீழ்_index, உயர்வு_index + 1), set);
    } இல்லையெனில் {
        // operate on the potentially partial first word
        apply_pattern(bitmap + கீழ்_word, make_pattern(கீழ்_index, CONFIG_WORD_SIZE), set);
        கீழ்_word++;
        // iterate over the whole words
        போது (கீழ்_word < உயர்வு_word) {
            apply_pattern(bitmap + கீழ்_word, ~(word_t)0, set);
            கீழ்_word++;
        }
        // apply to any remaining bits
        apply_pattern(bitmap + கீழ்_word, make_pattern(0, உயர்வு_index + 1), set);
    }
}
