/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <api/syscall.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <arch/object/ioport.h>
#অন্তর্ভাব <arch/api/invocation.h>
#অন্তর্ভাব <plat/machine/io.h>

স্থির অন্তঃরেখ শূন্য apply_pattern(word_t_may_alias *w, word_t pattern, bool_t set)
{
    যদি (set) {
        *w |= pattern;
    } অন্যথা {
        *w &= ~pattern;
    }
}

স্থির অন্তঃরেখ word_t make_pattern(পূর্ণসংখ্যা start, পূর্ণসংখ্যা end)
{
    // number of bits we want to have set
    পূর্ণসংখ্যা অঙ্ক_bits = end - start;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    ফেরত (~(word_t)0) >> (CONFIG_WORD_আকার - অঙ্ক_bits) << start;
}

স্থির exception_t ensurePortOperationAllowed(cap_t cap, uint32_t start_port, uint32_t আকার_2)
{
    uint32_t first_allowed = cap_io_port_cap_get_capIOPortFirstPort(cap);
    uint32_t last_allowed = cap_io_port_cap_get_capIOPortLastPort(cap);
    uint32_t end_port = start_port + আকার_2 - 1;
    প্রতিজ্ঞা(first_allowed <= last_allowed);
    প্রতিজ্ঞা(start_port <= end_port);

    যদি ((start_port < first_allowed) || (end_port > last_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (পূর্ণসংখ্যা)start_port, (পূর্ণসংখ্যা)end_port,
                  (পূর্ণসংখ্যা)first_allowed, (পূর্ণসংখ্যা)last_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    ফেরত EXCEPTION_NONE;
}

শূন্য freeIOPortRange(uint16_t first_port, uint16_t last_port)
{
    setIOPortMask(x86KSAllocatedIOPorts, first_port, last_port, false);
}

স্থির bool_t isIOPortRangeFree(uint16_t first_port, uint16_t last_port)
{
    পূর্ণসংখ্যা নিচের_word = first_port >> wordRadix;
    পূর্ণসংখ্যা উপরের_word = last_port >> wordRadix;
    পূর্ণসংখ্যা নিচের_সূচক = first_port & MASK(wordRadix);
    পূর্ণসংখ্যা উপরের_সূচক = last_port & MASK(wordRadix);

    // check if we are operating on a partial word
    যদি (নিচের_word == উপরের_word) {
        যদি ((x86KSAllocatedIOPorts[নিচের_word] & make_pattern(নিচের_সূচক, উপরের_সূচক + 1)) != 0) {
            ফেরত false;
        }
        ফেরত true;
    }
    // check the starting word
    যদি ((x86KSAllocatedIOPorts[নিচের_word] & make_pattern(নিচের_সূচক, CONFIG_WORD_আকার)) != 0) {
        ফেরত false;
    }
    নিচের_word++;
    // check the rest of the whole words
    যখন (নিচের_word < উপরের_word) {
        যদি (x86KSAllocatedIOPorts[নিচের_word] != 0) {
            ফেরত false;
        }
        নিচের_word++;
    }
    // check any trailing bits
    যদি ((x86KSAllocatedIOPorts[নিচের_word] & make_pattern(0, উপরের_সূচক + 1)) != 0) {
        ফেরত false;
    }
    ফেরত true;
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

    ফেরত EXCEPTION_NONE;
}

exception_t decodeX86PortControlInvocation(
    word_t invLabel,
    word_t দৈর্ঘ্য,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    word_t *বাফার
)
{
    uint16_t first_port;
    uint16_t last_port;
    word_t সূচক, depth;
    cap_t cnodeCap;
    cte_t *destSlot;
    lookupSlot_ret_t lu_ret;
    exception_t status;

    যদি (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (দৈর্ঘ্য < 4 || current_extra_caps.excaprefs[0] == ফাঁকা) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    first_port = getSyscallArg(0, বাফার) & 0xffff;
    last_port = getSyscallArg(1, বাফার) & 0xffff;
    সূচক = getSyscallArg(2, বাফার);
    depth = getSyscallArg(3, বাফার);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    যদি (last_port < first_port) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 1;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    যদি (!isIOPortRangeFree(first_port, last_port)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = seL4_RevokeFirst;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, সূচক, depth);
    যদি (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(বাফার, 0));
        ফেরত lu_ret.status;
    }
    destSlot = lu_ret.slot;

    status = ensureEmptySlot(destSlot);
    যদি (status != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(বাফার, 0));
        ফেরত status;
    }

    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত invokeX86PortControl(first_port, last_port, destSlot, slot);
}

স্থির exception_t invokeX86PortIn(word_t invLabel, uint16_t port, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = গ্রন্থি_STATE(ksCurThread);

    নির্বাচন (invLabel) {
    ক্ষেত্র X86IOPortIn8:
        res = in8(port);
        বিরতি;
    ক্ষেত্র X86IOPortIn16:
        res = in16(port);
        বিরতি;
    ক্ষেত্র X86IOPortIn32:
        res = in32(port);
        বিরতি;
    }

    যদি (call) {
        word_t *ipc_বাফার = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned পূর্ণসংখ্যা দৈর্ঘ্য = setMR(thread, ipc_বাফার, 0, res);
        setRegister(thread, বার্তা_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, দৈর্ঘ্য)));
    }
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Running);
    ফেরত EXCEPTION_NONE;
}

স্থির exception_t invokeX86PortOut(word_t invLabel, uint16_t port, uint32_t তথ্য)
{
    নির্বাচন (invLabel) {
    ক্ষেত্র X86IOPortOut8:
        out8(port, তথ্য);
        বিরতি;
    ক্ষেত্র X86IOPortOut16:
        out16(port, তথ্য);
        বিরতি;
    ক্ষেত্র X86IOPortOut32:
        out32(port, তথ্য);
        বিরতি;
    }

    ফেরত EXCEPTION_NONE;
}

exception_t decodeX86PortInvocation(
    word_t invLabel,
    word_t দৈর্ঘ্য,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *বাফার
)
{
    exception_t ret = EXCEPTION_NONE;

    যদি (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        যদি (দৈর্ঘ্য < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t port = getSyscallArg(0, বাফার) & 0xffff;
        নির্বাচন (invLabel) {
        ক্ষেত্র X86IOPortIn8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            বিরতি;
        ক্ষেত্র X86IOPortIn16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            বিরতি;
        ক্ষেত্র X86IOPortIn32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            বিরতি;
        }
        যদি (ret != EXCEPTION_NONE) {
            ফেরত ret;
        }
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত invokeX86PortIn(invLabel, port, call);
    } অন্যথা যদি (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        যদি (দৈর্ঘ্য < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            ফেরত EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to write to. */
        uint16_t port = getSyscallArg(0, বাফার) & 0xffff;
        seL4_Word raw_তথ্য = getSyscallArg(1, বাফার);
        /* We construct the value for data from raw_data based on the actual size of the port
           operation. This ensures that there is no 'random' user data left over in the value
           passed to invokeX86PortOut. Whilst invokeX86PortOut will ignore any extra data and
           cast down to the correct word size removing the extra here is currently relied upon
           for verification */
        uint32_t তথ্য = 0;

        নির্বাচন (invLabel) {
        ক্ষেত্র X86IOPortOut8:
            ret = ensurePortOperationAllowed(cap, port, 1);
            তথ্য = raw_তথ্য & 0xff;
            বিরতি;
        ক্ষেত্র X86IOPortOut16:
            ret = ensurePortOperationAllowed(cap, port, 2);
            তথ্য = raw_তথ্য & 0xffff;
            বিরতি;
        ক্ষেত্র X86IOPortOut32:
            ret = ensurePortOperationAllowed(cap, port, 4);
            তথ্য = raw_তথ্য & 0xffffffff;
            বিরতি;
        }
        যদি (ret != EXCEPTION_NONE) {
            ফেরত ret;
        }
        setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
        ফেরত invokeX86PortOut(invLabel, port, তথ্য);
    } অন্যথা {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
}

শূন্য setIOPortMask(শূন্য *ioport_bitmap, uint16_t নিচের, uint16_t উপরের, bool_t set)
{
    //get an aliasing pointer
    word_t_may_alias *bitmap = ioport_bitmap;

    পূর্ণসংখ্যা নিচের_word = নিচের >> wordRadix;
    পূর্ণসংখ্যা উপরের_word = উপরের >> wordRadix;
    পূর্ণসংখ্যা নিচের_সূচক = নিচের & MASK(wordRadix);
    পূর্ণসংখ্যা উপরের_সূচক = উপরের & MASK(wordRadix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    যদি (নিচের_word == উপরের_word) {
        apply_pattern(bitmap + নিচের_word, make_pattern(নিচের_সূচক, উপরের_সূচক + 1), set);
    } অন্যথা {
        // operate on the potentially partial first word
        apply_pattern(bitmap + নিচের_word, make_pattern(নিচের_সূচক, CONFIG_WORD_আকার), set);
        নিচের_word++;
        // iterate over the whole words
        যখন (নিচের_word < উপরের_word) {
            apply_pattern(bitmap + নিচের_word, ~(word_t)0, set);
            নিচের_word++;
        }
        // apply to any remaining bits
        apply_pattern(bitmap + নিচের_word, make_pattern(0, উপরের_সূচক + 1), set);
    }
}
