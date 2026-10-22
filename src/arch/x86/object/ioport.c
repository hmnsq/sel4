/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <kernel/thread.h>
#include <api/failures.h>
#include <api/syscall.h>
#include <machine/io.h>
#include <arch/object/ioport.h>
#include <arch/api/invocation.h>
#include <plat/machine/io.h>

static inline void apply_pattern(word_t_may_alias *w, word_t pattern, bool_t set)
{
    ако (set) {
        *w |= pattern;
    } иначе {
        *w &= ~pattern;
    }
}

static inline word_t make_pattern(цяло start, цяло end)
{
    // number of bits we want to have set
    цяло num_bits = end - start;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    върни (~(word_t)0) >> (CONFIG_WORD_SIZE - num_bits) << start;
}

static exception_t ensurePortOperationAllowed(cap_t cap, uint32_t start_port, uint32_t size)
{
    uint32_t first_allowed = cap_io_port_cap_get_capIOPortFirstPort(cap);
    uint32_t last_allowed = cap_io_port_cap_get_capIOPortLastPort(cap);
    uint32_t end_port = start_port + size - 1;
    assert(first_allowed <= last_allowed);
    assert(start_port <= end_port);

    ако ((start_port < first_allowed) || (end_port > last_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (цяло)start_port, (цяло)end_port,
                  (цяло)first_allowed, (цяло)last_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        върни EXCEPTION_SYSCALL_ERROR;
    }

    върни EXCEPTION_NONE;
}

void freeIOPortRange(uint16_t first_port, uint16_t last_port)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, false);
}

static bool_t isIOPortRangeFree(uint16_t first_port, uint16_t last_port)
{
    цяло low_word = first_port >> wordRadix;
    цяло high_word = last_port >> wordRadix;
    цяло low_index = first_port & MASK(wordRadix);
    цяло high_index = last_port & MASK(wordRadix);

    // check if we are operating on a partial word
    ако (low_word == high_word) {
        ако ((x86KSAllocatedIOPorts[low_word] & make_pattern(low_index, high_index + 1)) != 0) {
            върни false;
        }
        върни true;
    }
    // check the starting word
    ако ((x86KSAllocatedIOPorts[low_word] & make_pattern(low_index, CONFIG_WORD_SIZE)) != 0) {
        върни false;
    }
    low_word++;
    // check the rest of the whole words
    докато (low_word < high_word) {
        ако (x86KSAllocatedIOPorts[low_word] != 0) {
            върни false;
        }
        low_word++;
    }
    // check any trailing bits
    ако ((x86KSAllocatedIOPorts[low_word] & make_pattern(0, high_index + 1)) != 0) {
        върни false;
    }
    върни true;
}

static exception_t invokeX86PortControl(uint16_t first_port, uint16_t last_port, cte_t *ioportSlot, cte_t *controlSlot)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, true);
    cteInsert(cap_io_port_cap_new(first_port, last_port
#ifdef CONFIG_VTX
                                  , VPID_INVALID
#endif
                                 ),
              controlSlot, ioportSlot);

    върни EXCEPTION_NONE;
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

    ако (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        върни EXCEPTION_SYSCALL_ERROR;
    }

    ако (length < 4 || current_extra_caps.excaprefs[0] == NULL) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        върни EXCEPTION_SYSCALL_ERROR;
    }

    first_port = getSyscallArg(0, buffer) & 0xffff;
    last_port = getSyscallArg(1, buffer) & 0xffff;
    index = getSyscallArg(2, buffer);
    depth = getSyscallArg(3, buffer);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    ако (last_port < first_port) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalidArgumentNumber = 1;
        върни EXCEPTION_SYSCALL_ERROR;
    }

    ако (!isIOPortRangeFree(first_port, last_port)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = seL4_RevokeFirst;
        върни EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    ако (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(buffer, 0));
        върни lu_ret.status;
    }
    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    ако (status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(buffer, 0));
        върни status;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    върни invokeX86PortControl(first_port, last_port, destSlot, slot);
}

static exception_t invokeX86PortIn(word_t invLabel, uint16_t port, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = NODE_STATE(ksCurThread);

    switch (invLabel) {
    case X86IOPortIn8:
        res = in8(port);
        break;
    case X86IOPortIn16:
        res = in16(port);
        break;
    case X86IOPortIn32:
        res = in32(port);
        break;
    }

    ако (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned цяло length = setMR(thread, ipcBuffer, 0, res);
        setRegister(thread, msgInfoRegister, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Running);
    върни EXCEPTION_NONE;
}

static exception_t invokeX86PortOut(word_t invLabel, uint16_t port, uint32_t data)
{
    switch (invLabel) {
    case X86IOPortOut8:
        out8(port, data);
        break;
    case X86IOPortOut16:
        out16(port, data);
        break;
    case X86IOPortOut32:
        out32(port, data);
        break;
    }

    върни EXCEPTION_NONE;
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

    ако (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        ако (length < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            върни EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t port = getSyscallArg(0, buffer) & 0xffff;
        switch (invLabel) {
        case X86IOPortIn8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            break;
        case X86IOPortIn16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            break;
        case X86IOPortIn32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            break;
        }
        ако (ret != EXCEPTION_NONE) {
            върни ret;
        }
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        върни invokeX86PortIn(invLabel, port, call);
    } иначе ако (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        ако (length < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            върни EXCEPTION_SYSCALL_ERROR;
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

        switch (invLabel) {
        case X86IOPortOut8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            data = raw_data & 0xff;
            break;
        case X86IOPortOut16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            data = raw_data & 0xffff;
            break;
        case X86IOPortOut32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            data = raw_data & 0xffffffff;
            break;
        }
        ако (ret != EXCEPTION_NONE) {
            върни ret;
        }
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        върни invokeX86PortOut(invLabel, port, data);
    } иначе {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        върни EXCEPTION_SYSCALL_ERROR;
    }
}

void setIOPortMask(void *ioport_bitmap, uint16_t low, uint16_t high, bool_t set)
{
    //get an aliasing pointer
    word_t_may_alias *bitmap = ioport_bitmap;

    цяло low_word = low >> wordRadix;
    цяло high_word = high >> wordRadix;
    цяло low_index = low & MASK(wordRadix);
    цяло high_index = high & MASK(wordRadix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    ако (low_word == high_word) {
        apply_pattern(bitmap + low_word, make_pattern(low_index, high_index + 1), set);
    } иначе {
        // operate on the potentially partial first word
        apply_pattern(bitmap + low_word, make_pattern(low_index, CONFIG_WORD_SIZE), set);
        low_word++;
        // iterate over the whole words
        докато (low_word < high_word) {
            apply_pattern(bitmap + low_word, ~(word_t)0, set);
            low_word++;
        }
        // apply to any remaining bits
        apply_pattern(bitmap + low_word, make_pattern(0, high_index + 1), set);
    }
}
