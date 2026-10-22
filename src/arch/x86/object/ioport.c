/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <kernel/thread.h>
#شامل <api/failures.h>
#شامل <api/syscall.h>
#شامل <machine/io.h>
#شامل <arch/object/ioport.h>
#شامل <arch/api/invocation.h>
#شامل <plat/machine/io.h>

ایستا درونخطی تهی apply_pattern(word_t_may_alias *w, word_t pattern, bool_t set)
{
    اگر (set) {
        *w |= pattern;
    } وگرنه {
        *w &= ~pattern;
    }
}

ایستا درونخطی word_t make_pattern(صحیح start, صحیح end)
{
    // number of bits we want to have set
    صحیح رقم_bits = end - start;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    بازگشت (~(word_t)0) >> (CONFIG_WORD_SIZE - رقم_bits) << start;
}

ایستا exception_t ensurePortOperationAllowed(cap_t cap, uint32_t start_port, uint32_t size)
{
    uint32_t first_allowed = cap_io_port_cap_get_capIOPortFirstPort(cap);
    uint32_t last_allowed = cap_io_port_cap_get_capIOPortLastPort(cap);
    uint32_t end_port = start_port + size - 1;
    أكد(first_allowed <= last_allowed);
    أكد(start_port <= end_port);

    اگر ((start_port < first_allowed) || (end_port > last_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (صحیح)start_port, (صحیح)end_port,
                  (صحیح)first_allowed, (صحیح)last_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    بازگشت EXCEPTION_NONE;
}

تهی freeIOPortRange(uint16_t first_port, uint16_t last_port)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, false);
}

ایستا bool_t isIOPortRangeFree(uint16_t first_port, uint16_t last_port)
{
    صحیح کم_word = first_port >> wordRadix;
    صحیح بالا_word = last_port >> wordRadix;
    صحیح کم_index = first_port & MASK(wordRadix);
    صحیح بالا_index = last_port & MASK(wordRadix);

    // check if we are operating on a partial word
    اگر (کم_word == بالا_word) {
        اگر ((x86KSAllocatedIOPorts[کم_word] & make_pattern(کم_index, بالا_index + 1)) != 0) {
            بازگشت false;
        }
        بازگشت true;
    }
    // check the starting word
    اگر ((x86KSAllocatedIOPorts[کم_word] & make_pattern(کم_index, CONFIG_WORD_SIZE)) != 0) {
        بازگشت false;
    }
    کم_word++;
    // check the rest of the whole words
    وقتی (کم_word < بالا_word) {
        اگر (x86KSAllocatedIOPorts[کم_word] != 0) {
            بازگشت false;
        }
        کم_word++;
    }
    // check any trailing bits
    اگر ((x86KSAllocatedIOPorts[کم_word] & make_pattern(0, بالا_index + 1)) != 0) {
        بازگشت false;
    }
    بازگشت true;
}

ایستا exception_t invokeX86PortControl(uint16_t first_port, uint16_t last_port, cte_t *ioportSlot, cte_t *controlSlot)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, true);
    cte_درج(cap_io_port_cap_new(first_port, last_port
#ifdef CONFIG_VTX
                                  , VPID_INVALID
#endif
                                 ),
              controlSlot, ioportSlot);

    بازگشت EXCEPTION_NONE;
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

    اگر (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    اگر (length < 4 || current_extra_caps.excaprefs[0] == پوچ) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    first_port = getSyscallArg(0, buffer) & 0xffff;
    last_port = getSyscallArg(1, buffer) & 0xffff;
    index = getSyscallArg(2, buffer);
    depth = getSyscallArg(3, buffer);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    اگر (last_port < first_port) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_عدد = 1;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    اگر (!isIOPortRangeFree(first_port, last_port)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = seL4_RevokeFirst;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    اگر (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(buffer, 0));
        بازگشت lu_ret.status;
    }
    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    اگر (status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(buffer, 0));
        بازگشت status;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    بازگشت invokeX86PortControl(first_port, last_port, destSlot, slot);
}

ایستا exception_t invokeX86PortIn(word_t invLabel, uint16_t port, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = گره_STATE(ksCurThread);

    انتخاب (invLabel) {
    حالت X86IOPortIn8:
        res = in8(port);
        بشکن;
    حالت X86IOPortIn16:
        res = in16(port);
        بشکن;
    حالت X86IOPortIn32:
        res = in32(port);
        بشکن;
    }

    اگر (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned صحیح length = setMR(thread, ipcBuffer, 0, res);
        setRegister(thread, پیام_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(گره_STATE(ksCurThread), ThreadState_Running);
    بازگشت EXCEPTION_NONE;
}

ایستا exception_t invokeX86PortOut(word_t invLabel, uint16_t port, uint32_t data)
{
    انتخاب (invLabel) {
    حالت X86IOPortOut8:
        out8(port, data);
        بشکن;
    حالت X86IOPortOut16:
        out16(port, data);
        بشکن;
    حالت X86IOPortOut32:
        out32(port, data);
        بشکن;
    }

    بازگشت EXCEPTION_NONE;
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

    اگر (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        اگر (length < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t port = getSyscallArg(0, buffer) & 0xffff;
        انتخاب (invLabel) {
        حالت X86IOPortIn8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            بشکن;
        حالت X86IOPortIn16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            بشکن;
        حالت X86IOPortIn32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            بشکن;
        }
        اگر (ret != EXCEPTION_NONE) {
            بازگشت ret;
        }
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        بازگشت invokeX86PortIn(invLabel, port, call);
    } وگرنه اگر (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        اگر (length < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            بازگشت EXCEPTION_SYSCALL_ERROR;
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

        انتخاب (invLabel) {
        حالت X86IOPortOut8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            data = raw_data & 0xff;
            بشکن;
        حالت X86IOPortOut16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            data = raw_data & 0xffff;
            بشکن;
        حالت X86IOPortOut32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            data = raw_data & 0xffffffff;
            بشکن;
        }
        اگر (ret != EXCEPTION_NONE) {
            بازگشت ret;
        }
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        بازگشت invokeX86PortOut(invLabel, port, data);
    } وگرنه {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
}

تهی setIOPortMask(تهی *ioport_bitmap, uint16_t کم, uint16_t بالا, bool_t set)
{
    //get an aliasing pointer
    word_t_may_alias *bitmap = ioport_bitmap;

    صحیح کم_word = کم >> wordRadix;
    صحیح بالا_word = بالا >> wordRadix;
    صحیح کم_index = کم & MASK(wordRadix);
    صحیح بالا_index = بالا & MASK(wordRadix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    اگر (کم_word == بالا_word) {
        apply_pattern(bitmap + کم_word, make_pattern(کم_index, بالا_index + 1), set);
    } وگرنه {
        // operate on the potentially partial first word
        apply_pattern(bitmap + کم_word, make_pattern(کم_index, CONFIG_WORD_SIZE), set);
        کم_word++;
        // iterate over the whole words
        وقتی (کم_word < بالا_word) {
            apply_pattern(bitmap + کم_word, ~(word_t)0, set);
            کم_word++;
        }
        // apply to any remaining bits
        apply_pattern(bitmap + کم_word, make_pattern(0, بالا_index + 1), set);
    }
}
