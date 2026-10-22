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

static anta_hstha void apply_pattern(word_t_may_alias *w, word_t pattern, bool_t set)
{
    නම් (set) {
        *w |= pattern;
    } නැතහොත් {
        *w &= ~pattern;
    }
}

static anta_hstha word_t make_pattern(පූර්ණ_සංඛ්‍යා start, පූර්ණ_සංඛ්‍යා end)
{
    // number of bits we want to have set
    පූර්ණ_සංඛ්‍යා num_bits = end - start;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    ආපසු (~(word_t)0) >> (CONFIG_WORD_SIZE - num_bits) << start;
}

static exception_t ensurePortOperationAllowed(cap_t cap, uint32_t start_port, uint32_t size)
{
    uint32_t first_allowed = cap_io_port_cap_get_capIOPortFirstPort(cap);
    uint32_t last_allowed = cap_io_port_cap_get_capIOPortLastPort(cap);
    uint32_t end_port = start_port + size - 1;
    assert(first_allowed <= last_allowed);
    assert(start_port <= end_port);

    නම් ((start_port < first_allowed) || (end_port > last_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (පූර්ණ_සංඛ්‍යා)start_port, (පූර්ණ_සංඛ්‍යා)end_port,
                  (පූර්ණ_සංඛ්‍යා)first_allowed, (පූර්ණ_සංඛ්‍යා)last_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        ආපසු EXCEPTION_SYSCALL_ERROR;
    }

    ආපසු EXCEPTION_NONE;
}

void freeIOPortRange(uint16_t first_port, uint16_t last_port)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, false);
}

static bool_t isIOPortRangeFree(uint16_t first_port, uint16_t last_port)
{
    පූර්ණ_සංඛ්‍යා low_word = first_port >> wordRadix;
    පූර්ණ_සංඛ්‍යා high_word = last_port >> wordRadix;
    පූර්ණ_සංඛ්‍යා low_index = first_port & MASK(wordRadix);
    පූර්ණ_සංඛ්‍යා high_index = last_port & MASK(wordRadix);

    // check if we are operating on a partial word
    නම් (low_word == high_word) {
        නම් ((x86KSAllocatedIOPorts[low_word] & make_pattern(low_index, high_index + 1)) != 0) {
            ආපසු false;
        }
        ආපසු true;
    }
    // check the starting word
    නම් ((x86KSAllocatedIOPorts[low_word] & make_pattern(low_index, CONFIG_WORD_SIZE)) != 0) {
        ආපසු false;
    }
    low_word++;
    // check the rest of the whole words
    අතරතුර (low_word < high_word) {
        නම් (x86KSAllocatedIOPorts[low_word] != 0) {
            ආපසු false;
        }
        low_word++;
    }
    // check any trailing bits
    නම් ((x86KSAllocatedIOPorts[low_word] & make_pattern(0, high_index + 1)) != 0) {
        ආපසු false;
    }
    ආපසු true;
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

    ආපසු EXCEPTION_NONE;
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

    නම් (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ආපසු EXCEPTION_SYSCALL_ERROR;
    }

    නම් (length < 4 || current_extra_caps.excaprefs[0] == NULL) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ආපසු EXCEPTION_SYSCALL_ERROR;
    }

    first_port = getSyscallArg(0, buffer) & 0xffff;
    last_port = getSyscallArg(1, buffer) & 0xffff;
    index = getSyscallArg(2, buffer);
    depth = getSyscallArg(3, buffer);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    නම් (last_port < first_port) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalidArgumentNumber = 1;
        ආපසු EXCEPTION_SYSCALL_ERROR;
    }

    නම් (!isIOPortRangeFree(first_port, last_port)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = seL4_RevokeFirst;
        ආපසු EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    නම් (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(buffer, 0));
        ආපසු lu_ret.status;
    }
    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    නම් (status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(buffer, 0));
        ආපසු status;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    ආපසු invokeX86PortControl(first_port, last_port, destSlot, slot);
}

static exception_t invokeX86PortIn(word_t invLabel, uint16_t port, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = NODE_STATE(ksCurThread);

    switch (invLabel) {
    pak_sa X86IOPortIn8:
        res = in8(port);
        bha_nga;
    pak_sa X86IOPortIn16:
        res = in16(port);
        bha_nga;
    pak_sa X86IOPortIn32:
        res = in32(port);
        bha_nga;
    }

    නම් (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned පූර්ණ_සංඛ්‍යා length = setMR(thread, ipcBuffer, 0, res);
        setRegister(thread, msgInfoRegister, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Running);
    ආපසු EXCEPTION_NONE;
}

static exception_t invokeX86PortOut(word_t invLabel, uint16_t port, uint32_t data)
{
    switch (invLabel) {
    pak_sa X86IOPortOut8:
        out8(port, data);
        bha_nga;
    pak_sa X86IOPortOut16:
        out16(port, data);
        bha_nga;
    pak_sa X86IOPortOut32:
        out32(port, data);
        bha_nga;
    }

    ආපසු EXCEPTION_NONE;
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

    නම් (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        නම් (length < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ආපසු EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t port = getSyscallArg(0, buffer) & 0xffff;
        switch (invLabel) {
        pak_sa X86IOPortIn8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            bha_nga;
        pak_sa X86IOPortIn16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            bha_nga;
        pak_sa X86IOPortIn32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            bha_nga;
        }
        නම් (ret != EXCEPTION_NONE) {
            ආපසු ret;
        }
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        ආපසු invokeX86PortIn(invLabel, port, call);
    } නැතහොත් නම් (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        නම් (length < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ආපසු EXCEPTION_SYSCALL_ERROR;
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
        pak_sa X86IOPortOut8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            data = raw_data & 0xff;
            bha_nga;
        pak_sa X86IOPortOut16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            data = raw_data & 0xffff;
            bha_nga;
        pak_sa X86IOPortOut32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            data = raw_data & 0xffffffff;
            bha_nga;
        }
        නම් (ret != EXCEPTION_NONE) {
            ආපසු ret;
        }
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        ආපසු invokeX86PortOut(invLabel, port, data);
    } නැතහොත් {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ආපසු EXCEPTION_SYSCALL_ERROR;
    }
}

void setIOPortMask(void *ioport_bitmap, uint16_t low, uint16_t high, bool_t set)
{
    //get an aliasing pointer
    word_t_may_alias *bitmap = ioport_bitmap;

    පූර්ණ_සංඛ්‍යා low_word = low >> wordRadix;
    පූර්ණ_සංඛ්‍යා high_word = high >> wordRadix;
    පූර්ණ_සංඛ්‍යා low_index = low & MASK(wordRadix);
    පූර්ණ_සංඛ්‍යා high_index = high & MASK(wordRadix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    නම් (low_word == high_word) {
        apply_pattern(bitmap + low_word, make_pattern(low_index, high_index + 1), set);
    } නැතහොත් {
        // operate on the potentially partial first word
        apply_pattern(bitmap + low_word, make_pattern(low_index, CONFIG_WORD_SIZE), set);
        low_word++;
        // iterate over the whole words
        අතරතුර (low_word < high_word) {
            apply_pattern(bitmap + low_word, ~(word_t)0, set);
            low_word++;
        }
        // apply to any remaining bits
        apply_pattern(bitmap + low_word, make_pattern(0, high_index + 1), set);
    }
}
