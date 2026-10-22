/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <kernel/thread.h>
#ضمن <api/failures.h>
#ضمن <api/syscall.h>
#ضمن <machine/io.h>
#ضمن <arch/object/ioport.h>
#ضمن <arch/api/invocation.h>
#ضمن <plat/machine/io.h>

ساكن مضمن فراغ apply_pattern(word_t_may_alias *w, word_t pattern, bool_t set)
{
    إذا (set) {
        *w |= pattern;
    } وإلا {
        *w &= ~pattern;
    }
}

ساكن مضمن word_t make_pattern(صحيح start, صحيح end)
{
    // number of bits we want to have set
    صحيح رقم_bits = end - start;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    إرجاع (~(word_t)0) >> (CONFIG_WORD_SIZE - رقم_bits) << start;
}

ساكن exception_t ensurePortOperationAllowed(cap_t cap, uint32_t start_port, uint32_t size)
{
    uint32_t first_allowed = cap_io_port_cap_get_capIOPortFirstPort(cap);
    uint32_t last_allowed = cap_io_port_cap_get_capIOPortLastPort(cap);
    uint32_t end_port = start_port + size - 1;
    أكد(first_allowed <= last_allowed);
    أكد(start_port <= end_port);

    إذا ((start_port < first_allowed) || (end_port > last_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (صحيح)start_port, (صحيح)end_port,
                  (صحيح)first_allowed, (صحيح)last_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        إرجاع EXCEPTION_SYSCALL_ERROR;
    }

    إرجاع EXCEPTION_NONE;
}

فراغ freeIOPortRange(uint16_t first_port, uint16_t last_port)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, false);
}

ساكن bool_t isIOPortRangeFree(uint16_t first_port, uint16_t last_port)
{
    صحيح أدنى_word = first_port >> wordRadix;
    صحيح أعلى_word = last_port >> wordRadix;
    صحيح أدنى_index = first_port & MASK(wordRadix);
    صحيح أعلى_index = last_port & MASK(wordRadix);

    // check if we are operating on a partial word
    إذا (أدنى_word == أعلى_word) {
        إذا ((x86KSAllocatedIOPorts[أدنى_word] & make_pattern(أدنى_index, أعلى_index + 1)) != 0) {
            إرجاع false;
        }
        إرجاع true;
    }
    // check the starting word
    إذا ((x86KSAllocatedIOPorts[أدنى_word] & make_pattern(أدنى_index, CONFIG_WORD_SIZE)) != 0) {
        إرجاع false;
    }
    أدنى_word++;
    // check the rest of the whole words
    بينما (أدنى_word < أعلى_word) {
        إذا (x86KSAllocatedIOPorts[أدنى_word] != 0) {
            إرجاع false;
        }
        أدنى_word++;
    }
    // check any trailing bits
    إذا ((x86KSAllocatedIOPorts[أدنى_word] & make_pattern(0, أعلى_index + 1)) != 0) {
        إرجاع false;
    }
    إرجاع true;
}

ساكن exception_t invokeX86PortControl(uint16_t first_port, uint16_t last_port, cte_t *ioportSlot, cte_t *controlSlot)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, true);
    cte_أدرج(cap_io_port_cap_new(first_port, last_port
#ifdef CONFIG_VTX
                                  , VPID_INVALID
#endif
                                 ),
              controlSlot, ioportSlot);

    إرجاع EXCEPTION_NONE;
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

    إذا (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        إرجاع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (length < 4 || current_extra_caps.excaprefs[0] == عدم) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        إرجاع EXCEPTION_SYSCALL_ERROR;
    }

    first_port = getSyscallArg(0, buffer) & 0xffff;
    last_port = getSyscallArg(1, buffer) & 0xffff;
    index = getSyscallArg(2, buffer);
    depth = getSyscallArg(3, buffer);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    إذا (last_port < first_port) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_العدد = 1;
        إرجاع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!isIOPortRangeFree(first_port, last_port)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = seL4_RevokeFirst;
        إرجاع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    إذا (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(buffer, 0));
        إرجاع lu_ret.status;
    }
    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    إذا (status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(buffer, 0));
        إرجاع status;
    }

    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
    إرجاع invokeX86PortControl(first_port, last_port, destSlot, slot);
}

ساكن exception_t invokeX86PortIn(word_t invLabel, uint16_t port, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = عقدة_STATE(ksCurThread);

    بدل (invLabel) {
    حالة X86IOPortIn8:
        res = in8(port);
        اكسر;
    حالة X86IOPortIn16:
        res = in16(port);
        اكسر;
    حالة X86IOPortIn32:
        res = in32(port);
        اكسر;
    }

    إذا (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned صحيح length = setMR(thread, ipcBuffer, 0, res);
        setRegister(thread, رسالة_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(عقدة_STATE(ksCurThread), ThreadState_Running);
    إرجاع EXCEPTION_NONE;
}

ساكن exception_t invokeX86PortOut(word_t invLabel, uint16_t port, uint32_t data)
{
    بدل (invLabel) {
    حالة X86IOPortOut8:
        out8(port, data);
        اكسر;
    حالة X86IOPortOut16:
        out16(port, data);
        اكسر;
    حالة X86IOPortOut32:
        out32(port, data);
        اكسر;
    }

    إرجاع EXCEPTION_NONE;
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

    إذا (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        إذا (length < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            إرجاع EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t port = getSyscallArg(0, buffer) & 0xffff;
        بدل (invLabel) {
        حالة X86IOPortIn8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            اكسر;
        حالة X86IOPortIn16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            اكسر;
        حالة X86IOPortIn32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            اكسر;
        }
        إذا (ret != EXCEPTION_NONE) {
            إرجاع ret;
        }
        setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
        إرجاع invokeX86PortIn(invLabel, port, call);
    } وإلا إذا (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        إذا (length < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            إرجاع EXCEPTION_SYSCALL_ERROR;
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

        بدل (invLabel) {
        حالة X86IOPortOut8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            data = raw_data & 0xff;
            اكسر;
        حالة X86IOPortOut16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            data = raw_data & 0xffff;
            اكسر;
        حالة X86IOPortOut32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            data = raw_data & 0xffffffff;
            اكسر;
        }
        إذا (ret != EXCEPTION_NONE) {
            إرجاع ret;
        }
        setThreadState(عقدة_STATE(ksCurThread), ThreadState_Restart);
        إرجاع invokeX86PortOut(invLabel, port, data);
    } وإلا {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        إرجاع EXCEPTION_SYSCALL_ERROR;
    }
}

فراغ setIOPortMask(فراغ *ioport_bitmap, uint16_t أدنى, uint16_t أعلى, bool_t set)
{
    //get an aliasing pointer
    word_t_may_alias *bitmap = ioport_bitmap;

    صحيح أدنى_word = أدنى >> wordRadix;
    صحيح أعلى_word = أعلى >> wordRadix;
    صحيح أدنى_index = أدنى & MASK(wordRadix);
    صحيح أعلى_index = أعلى & MASK(wordRadix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    إذا (أدنى_word == أعلى_word) {
        apply_pattern(bitmap + أدنى_word, make_pattern(أدنى_index, أعلى_index + 1), set);
    } وإلا {
        // operate on the potentially partial first word
        apply_pattern(bitmap + أدنى_word, make_pattern(أدنى_index, CONFIG_WORD_SIZE), set);
        أدنى_word++;
        // iterate over the whole words
        بينما (أدنى_word < أعلى_word) {
            apply_pattern(bitmap + أدنى_word, ~(word_t)0, set);
            أدنى_word++;
        }
        // apply to any remaining bits
        apply_pattern(bitmap + أدنى_word, make_pattern(0, أعلى_index + 1), set);
    }
}
