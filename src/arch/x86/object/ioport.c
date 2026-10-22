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
    ᨀᨘ (set) {
        *w |= pattern;
    } ᨀᨘᨁ {
        *w &= ~pattern;
    }
}

static anta_hstha word_t make_pattern(puur_naa_nka start, puur_naa_nka end)
{
    // number of bits we want to have set
    puur_naa_nka num_bits = end - start;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    ᨀᨘᨒ (~(word_t)0) >> (CONFIG_WORD_SIZE - num_bits) << start;
}

static exception_t ensurePortOperationAllowed(cap_t cap, uint32_t start_port, uint32_t size)
{
    uint32_t first_allowed = cap_io_port_cap_get_capIOPortFirstPort(cap);
    uint32_t last_allowed = cap_io_port_cap_get_capIOPortLastPort(cap);
    uint32_t end_port = start_port + size - 1;
    assert(first_allowed <= last_allowed);
    assert(start_port <= end_port);

    ᨀᨘ ((start_port < first_allowed) || (end_port > last_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (puur_naa_nka)start_port, (puur_naa_nka)end_port,
                  (puur_naa_nka)first_allowed, (puur_naa_nka)last_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        ᨀᨘᨒ EXCEPTION_SYSCALL_ERROR;
    }

    ᨀᨘᨒ EXCEPTION_NONE;
}

void freeIOPortRange(uint16_t first_port, uint16_t last_port)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, false);
}

static bool_t isIOPortRangeFree(uint16_t first_port, uint16_t last_port)
{
    puur_naa_nka low_word = first_port >> wordRadix;
    puur_naa_nka high_word = last_port >> wordRadix;
    puur_naa_nka low_index = first_port & MASK(wordRadix);
    puur_naa_nka high_index = last_port & MASK(wordRadix);

    // check if we are operating on a partial word
    ᨀᨘ (low_word == high_word) {
        ᨀᨘ ((x86KSAllocatedIOPorts[low_word] & make_pattern(low_index, high_index + 1)) != 0) {
            ᨀᨘᨒ false;
        }
        ᨀᨘᨒ true;
    }
    // check the starting word
    ᨀᨘ ((x86KSAllocatedIOPorts[low_word] & make_pattern(low_index, CONFIG_WORD_SIZE)) != 0) {
        ᨀᨘᨒ false;
    }
    low_word++;
    // check the rest of the whole words
    ᨀᨛ (low_word < high_word) {
        ᨀᨘ (x86KSAllocatedIOPorts[low_word] != 0) {
            ᨀᨘᨒ false;
        }
        low_word++;
    }
    // check any trailing bits
    ᨀᨘ ((x86KSAllocatedIOPorts[low_word] & make_pattern(0, high_index + 1)) != 0) {
        ᨀᨘᨒ false;
    }
    ᨀᨘᨒ true;
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

    ᨀᨘᨒ EXCEPTION_NONE;
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

    ᨀᨘ (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ᨀᨘᨒ EXCEPTION_SYSCALL_ERROR;
    }

    ᨀᨘ (length < 4 || current_extra_caps.excaprefs[0] == NULL) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ᨀᨘᨒ EXCEPTION_SYSCALL_ERROR;
    }

    first_port = getSyscallArg(0, buffer) & 0xffff;
    last_port = getSyscallArg(1, buffer) & 0xffff;
    index = getSyscallArg(2, buffer);
    depth = getSyscallArg(3, buffer);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    ᨀᨘ (last_port < first_port) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalidArgumentNumber = 1;
        ᨀᨘᨒ EXCEPTION_SYSCALL_ERROR;
    }

    ᨀᨘ (!isIOPortRangeFree(first_port, last_port)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = seL4_RevokeFirst;
        ᨀᨘᨒ EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    ᨀᨘ (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(buffer, 0));
        ᨀᨘᨒ lu_ret.status;
    }
    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    ᨀᨘ (status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(buffer, 0));
        ᨀᨘᨒ status;
    }

    setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
    ᨀᨘᨒ invokeX86PortControl(first_port, last_port, destSlot, slot);
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

    ᨀᨘ (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned puur_naa_nka length = setMR(thread, ipcBuffer, 0, res);
        setRegister(thread, msgInfoRegister, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(NODE_STATE(ksCurThread), ThreadState_Running);
    ᨀᨘᨒ EXCEPTION_NONE;
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

    ᨀᨘᨒ EXCEPTION_NONE;
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

    ᨀᨘ (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        ᨀᨘ (length < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ᨀᨘᨒ EXCEPTION_SYSCALL_ERROR;
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
        ᨀᨘ (ret != EXCEPTION_NONE) {
            ᨀᨘᨒ ret;
        }
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        ᨀᨘᨒ invokeX86PortIn(invLabel, port, call);
    } ᨀᨘᨁ ᨀᨘ (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        ᨀᨘ (length < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ᨀᨘᨒ EXCEPTION_SYSCALL_ERROR;
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
        ᨀᨘ (ret != EXCEPTION_NONE) {
            ᨀᨘᨒ ret;
        }
        setThreadState(NODE_STATE(ksCurThread), ThreadState_Restart);
        ᨀᨘᨒ invokeX86PortOut(invLabel, port, data);
    } ᨀᨘᨁ {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ᨀᨘᨒ EXCEPTION_SYSCALL_ERROR;
    }
}

void setIOPortMask(void *ioport_bitmap, uint16_t low, uint16_t high, bool_t set)
{
    //get an aliasing pointer
    word_t_may_alias *bitmap = ioport_bitmap;

    puur_naa_nka low_word = low >> wordRadix;
    puur_naa_nka high_word = high >> wordRadix;
    puur_naa_nka low_index = low & MASK(wordRadix);
    puur_naa_nka high_index = high & MASK(wordRadix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    ᨀᨘ (low_word == high_word) {
        apply_pattern(bitmap + low_word, make_pattern(low_index, high_index + 1), set);
    } ᨀᨘᨁ {
        // operate on the potentially partial first word
        apply_pattern(bitmap + low_word, make_pattern(low_index, CONFIG_WORD_SIZE), set);
        low_word++;
        // iterate over the whole words
        ᨀᨛ (low_word < high_word) {
            apply_pattern(bitmap + low_word, ~(word_t)0, set);
            low_word++;
        }
        // apply to any remaining bits
        apply_pattern(bitmap + low_word, make_pattern(0, high_index + 1), set);
    }
}
