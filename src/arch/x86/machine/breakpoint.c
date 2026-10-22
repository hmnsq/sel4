/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>

#ifdef CONFIG_HARDWARE_DEBUG_API

#ଅନ୍ତର୍ଭାଵ <arch/machine/debug.h>
#ଅନ୍ତର୍ଭାଵ <mode/machine/debug.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine.h>
#ଅନ୍ତର୍ଭାଵ <machine/registerset.h>
#ଅନ୍ତର୍ଭାଵ <sel4/plat/api/constants.h> /* seL4_NumHWBReakpoints */

/* Intel manual Vol3, 17.2.4 */
#define X86_DEBUG_BP_SIZE_1B                (0x0u)
#define X86_DEBUG_BP_SIZE_2B                (0x1u)
#define X86_DEBUG_BP_SIZE_4B                (0x3u)
#define X86_DEBUG_BP_SIZE_8B                (0x2u)

#define X86_DEBUG_BP0_SIZE_SHIFT            (18)
#define X86_DEBUG_BP1_SIZE_SHIFT            (22)
#define X86_DEBUG_BP2_SIZE_SHIFT            (26)
#define X86_DEBUG_BP3_SIZE_SHIFT            (30)

/* NOTE: Intel manual 17.2.4:
 * I/O breakpoints are supported by every processor later than i486, but only
 * when CR4.DE=1.
 * When CR4.DE=0, or if processor is earlier than i586, this bit is "Undefined",
 * which is not the same as "Reserved", so it won't trigger an exception - it
 * will just cause an undefined reaction from the CPU.
 */
#define X86_DEBUG_BP_TYPE_IO                (0x2u)
#define X86_DEBUG_BP_TYPE_INSTR             (0x0u)
#define X86_DEBUG_BP_TYPE_DATA_WRITE        (0x1u)
#define X86_DEBUG_BP_TYPE_DATA_READWRITE    (0x3u)

#define X86_DEBUG_BP0_TYPE_SHIFT            (16)
#define X86_DEBUG_BP1_TYPE_SHIFT            (20)
#define X86_DEBUG_BP2_TYPE_SHIFT            (24)
#define X86_DEBUG_BP3_TYPE_SHIFT            (28)

#define X86_DEBUG_EFLAGS_TRAP_FLAG    ((word_t)BIT(8))
#define X86_DEBUG_EFLAGS_RESUME_FLAG    ((word_t)BIT(16))
#define X86_DEBUG_DR6_SINGLE_STEP_FLAG  ((word_t)BIT(14))

#define X86_DEBUG_DR6_BP_MASK     (0xFu)

ସ୍ଥିର bool_t byte8_bps_supported = false;

bool_t byte8BreakpointsSupported(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ byte8_bps_supported;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ bitwiseAndDr6Reg(word_t mask)
{
    word_t tmp;

    tmp = readDr6Reg() & mask;
    writeDr6Reg(tmp);
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ word_t readDr7Context(tcb_t *t)
{
    ଫେରନ୍ତୁ t->tcbArch.tcbContext.breakpointState.dr[5];
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ bitwiseOrDr7Context(tcb_t *t, word_t val)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] |= val;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ bitwiseAndDr7Context(tcb_t *t, word_t mask)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] &= mask;
}

ସ୍ଥିର ଶୂନ୍ୟ unsetDr7BitsFor(tcb_t *t, uint16_t bp_ଅଙ୍କ)
{
    word_t mask;

    ବାଛ (bp_ଅଙ୍କ) {
    ସ୍ଥିତି 0:
        mask = (0x3u << X86_DEBUG_BP0_SIZE_SHIFT) | (0x3u << X86_DEBUG_BP0_TYPE_SHIFT);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 1:
        mask = (0x3u << X86_DEBUG_BP1_SIZE_SHIFT) | (0x3u << X86_DEBUG_BP1_TYPE_SHIFT);
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 2:
        mask = (0x3u << X86_DEBUG_BP2_SIZE_SHIFT) | (0x3u << X86_DEBUG_BP2_TYPE_SHIFT);
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* 3 */
        ପ୍ରତିଜ୍ଞା(bp_ଅଙ୍କ == 3);
        mask = (0x3u << X86_DEBUG_BP3_SIZE_SHIFT) | (0x3u << X86_DEBUG_BP3_TYPE_SHIFT);
        ଭାଙ୍ଗ;
    }

    mask = ~mask;
    bitwiseAndDr7Context(t, mask);
}

/** Converts an seL4_BreakpointType value into the underlying hardware
 * equivalent.
 * @param bp_num Breakpoint number.
 * @param type One of the values of seL4_BreakpointType.
 * @param rw Access trigger condition (read/write).
 * @return Hardware specific register value representing the inputs.
 */
PURE ସ୍ଥିର ଧାଡ଼ିଭିତରେ word_t convertTypeAndAccessToArch(uint16_t bp_ଅଙ୍କ, word_t type, word_t rw)
{
    ବାଛ (type) {
    ସ୍ଥିତି seL4_InstructionBreakpoint:
        type = X86_DEBUG_BP_TYPE_INSTR;
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* seL4_DataBreakpoint */
        ପ୍ରତିଜ୍ଞା(type == seL4_DataBreakpoint);
        type = (rw == seL4_BreakOnWrite)
               ? X86_DEBUG_BP_TYPE_DATA_WRITE
               : X86_DEBUG_BP_TYPE_DATA_READWRITE;
    }

    ବାଛ (bp_ଅଙ୍କ) {
    ସ୍ଥିତି 0:
        ଫେରନ୍ତୁ type << X86_DEBUG_BP0_TYPE_SHIFT;
    ସ୍ଥିତି 1:
        ଫେରନ୍ତୁ type << X86_DEBUG_BP1_TYPE_SHIFT;
    ସ୍ଥିତି 2:
        ଫେରନ୍ତୁ type << X86_DEBUG_BP2_TYPE_SHIFT;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* 3 */
        ପ୍ରତିଜ୍ଞା(bp_ଅଙ୍କ == 3);
        ଫେରନ୍ତୁ type << X86_DEBUG_BP3_TYPE_SHIFT;
    }
}

/** Reverse of convertTypeAndAccessToArch(): converts hardware values into
 * seL4 API values.
 * @param dr7 Hardware register value as input for conversion.
 * @param bp_num Breakpoint number.
 * @param type[out] Converted type value.
 * @param rw[out] Converted output access trigger value.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    word_t type, rw;
} convertedTypeAndAccess_t;

PURE ସ୍ଥିର ଧାଡ଼ିଭିତରେ convertedTypeAndAccess_t convertArchToTypeAndAccess(word_t dr7, uint16_t bp_ଅଙ୍କ)
{
    convertedTypeAndAccess_t ret;

    ବାଛ (bp_ଅଙ୍କ) {
    ସ୍ଥିତି 0:
        dr7 &= 0x3u << X86_DEBUG_BP0_TYPE_SHIFT;
        dr7 >>= X86_DEBUG_BP0_TYPE_SHIFT;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 1:
        dr7 &= 0x3u << X86_DEBUG_BP1_TYPE_SHIFT;
        dr7 >>= X86_DEBUG_BP1_TYPE_SHIFT;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 2:
        dr7 &= 0x3u << X86_DEBUG_BP2_TYPE_SHIFT;
        dr7 >>= X86_DEBUG_BP2_TYPE_SHIFT;
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* 3 */
        ପ୍ରତିଜ୍ଞା(bp_ଅଙ୍କ == 3);
        dr7 &= 0x3u << X86_DEBUG_BP3_TYPE_SHIFT;
        dr7 >>= X86_DEBUG_BP3_TYPE_SHIFT;
    }

    ବାଛ (dr7) {
    ସ୍ଥିତି X86_DEBUG_BP_TYPE_INSTR:
        ret.type = seL4_InstructionBreakpoint;
        ret.rw = seL4_BreakOnRead;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି X86_DEBUG_BP_TYPE_DATA_WRITE:
        ret.type = seL4_DataBreakpoint;
        ret.rw = seL4_BreakOnWrite;
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* Read-write */
        ପ୍ରତିଜ୍ଞା(dr7 == X86_DEBUG_BP_TYPE_DATA_READWRITE);
        ret.type = seL4_DataBreakpoint;
        ret.rw = seL4_BreakOnReadWrite;
        ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ ret;
}

/** Converts an integer size number into an equivalent hardware register value.
 * @param n Breakpoint number.
 * @param type One value from seL4_BreakpointType.
 * @param size An integer for the operand size of the breakpoint.
 * @return Converted, hardware-specific value.
 */
PURE ସ୍ଥିର ଧାଡ଼ିଭିତରେ word_t convertSizeToArch(uint16_t bp_ଅଙ୍କ, word_t type, word_t size)
{
    ଯଦି (type == seL4_InstructionBreakpoint) {
        /* Intel manual vol3 17.2.4:
         * "If the corresponding RWn field in register DR7 is 00 (instruction
         * execution), then the LENn field should also be 00"
         */
        size = 0;
    } ଅନ୍ୟଥା {
        ବାଛ (size) {
        ସ୍ଥିତି 1:
            size = X86_DEBUG_BP_SIZE_1B;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି 2:
            size = X86_DEBUG_BP_SIZE_2B;
            ଭାଙ୍ଗ;
        ସ୍ଥିତି 8:
            size = X86_DEBUG_BP_SIZE_8B;
            ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* 4B */
            ପ୍ରତିଜ୍ଞା(size == 4);
            size = X86_DEBUG_BP_SIZE_4B;
        }
    }

    ବାଛ (bp_ଅଙ୍କ) {
    ସ୍ଥିତି 0:
        ଫେରନ୍ତୁ size << X86_DEBUG_BP0_SIZE_SHIFT;
    ସ୍ଥିତି 1:
        ଫେରନ୍ତୁ size << X86_DEBUG_BP1_SIZE_SHIFT;
    ସ୍ଥିତି 2:
        ଫେରନ୍ତୁ size << X86_DEBUG_BP2_SIZE_SHIFT;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* 3 */
        ପ୍ରତିଜ୍ଞା(bp_ଅଙ୍କ == 3);
        ଫେରନ୍ତୁ size << X86_DEBUG_BP3_SIZE_SHIFT;
    }
}

/** Reverse of convertSizeToArch(): converts a hardware-specific size value
 * into an integer representation.
 * @param dr7 Hardware register value as input.
 * @param n Breakpoint number.
 * @return Converted size value.
 */
PURE ସ୍ଥିର ଧାଡ଼ିଭିତରେ word_t convertArchToSize(word_t dr7, uint16_t bp_ଅଙ୍କ)
{
    word_t type;

    ବାଛ (bp_ଅଙ୍କ) {
    ସ୍ଥିତି 0:
        type = dr7 & (0x3u << X86_DEBUG_BP0_TYPE_SHIFT);
        type >>= X86_DEBUG_BP0_TYPE_SHIFT;
        dr7 &= 0x3u << X86_DEBUG_BP0_SIZE_SHIFT;
        dr7 >>= X86_DEBUG_BP0_SIZE_SHIFT;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 1:
        type = dr7 & (0x3u << X86_DEBUG_BP1_TYPE_SHIFT);
        type >>= X86_DEBUG_BP1_TYPE_SHIFT;
        dr7 &= 0x3u << X86_DEBUG_BP1_SIZE_SHIFT;
        dr7 >>= X86_DEBUG_BP1_SIZE_SHIFT;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 2:
        type = dr7 & (0x3u << X86_DEBUG_BP2_TYPE_SHIFT);
        type >>= X86_DEBUG_BP2_TYPE_SHIFT;
        dr7 &= 0x3u << X86_DEBUG_BP2_SIZE_SHIFT;
        dr7 >>= X86_DEBUG_BP2_SIZE_SHIFT;
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* 3 */
        ପ୍ରତିଜ୍ଞା(bp_ଅଙ୍କ == 3);
        type = dr7 & (0x3u << X86_DEBUG_BP3_TYPE_SHIFT);
        type >>= X86_DEBUG_BP3_TYPE_SHIFT;
        dr7 &= 0x3u << X86_DEBUG_BP3_SIZE_SHIFT;
        dr7 >>= X86_DEBUG_BP3_SIZE_SHIFT;
    }

    /* Force size to 0 if type is instruction breakpoint. */
    ଯଦି (type == X86_DEBUG_BP_TYPE_INSTR) {
        ଫେରନ୍ତୁ 0;
    }

    ବାଛ (dr7) {
    ସ୍ଥିତି X86_DEBUG_BP_SIZE_1B:
        ଫେରନ୍ତୁ 1;
    ସ୍ଥିତି X86_DEBUG_BP_SIZE_2B:
        ଫେରନ୍ତୁ 2;
    ସ୍ଥିତି X86_DEBUG_BP_SIZE_8B:
        ଫେରନ୍ତୁ 8;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* 4B */
        ପ୍ରତିଜ୍ଞା(dr7 == X86_DEBUG_BP_SIZE_4B);
        ଫେରନ୍ତୁ 4;
    }
}

/** Enables a breakpoint.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ସ୍ଥିର ଶୂନ୍ୟ enableBreakpoint(tcb_t *t, uint16_t bp_ଅଙ୍କ)
{
    word_t enable_bit;

    ପ୍ରତିଜ୍ଞା(t != ଖାଲି);
    ପ୍ରତିଜ୍ଞା(bp_ଅଙ୍କ < X86_DEBUG_BP_N_REGS);

    ବାଛ (bp_ଅଙ୍କ) {
    ସ୍ଥିତି 0:
        enable_bit = X86_DEBUG_BP0_ENABLE_BIT;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 1:
        enable_bit = X86_DEBUG_BP1_ENABLE_BIT;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 2:
        enable_bit = X86_DEBUG_BP2_ENABLE_BIT;
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        enable_bit = X86_DEBUG_BP3_ENABLE_BIT;
        ଭାଙ୍ଗ;
    }

    bitwiseOrDr7Context(t, enable_bit);
}

/** Disables a breakpoint without clearing its configuration.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ସ୍ଥିର ଶୂନ୍ୟ disableBreakpoint(tcb_t *t, uint16_t bp_ଅଙ୍କ)
{
    word_t disable_mask;

    ପ୍ରତିଜ୍ଞା(t != ଖାଲି);
    ପ୍ରତିଜ୍ଞା(bp_ଅଙ୍କ < X86_DEBUG_BP_N_REGS);

    ବାଛ (bp_ଅଙ୍କ) {
    ସ୍ଥିତି 0:
        disable_mask = ~X86_DEBUG_BP0_ENABLE_BIT;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 1:
        disable_mask = ~X86_DEBUG_BP1_ENABLE_BIT;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 2:
        disable_mask = ~X86_DEBUG_BP2_ENABLE_BIT;
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        disable_mask = ~X86_DEBUG_BP3_ENABLE_BIT;
        ଭାଙ୍ଗ;
    }

    bitwiseAndDr7Context(t, disable_mask);
}

/** Returns a boolean for whether or not a breakpoint is enabled.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ସ୍ଥିର bool_t breakpointIsEnabled(tcb_t *t, uint16_t bp_ଅଙ୍କ)
{
    word_t dr7;

    ପ୍ରତିଜ୍ଞା(t != ଖାଲି);
    ପ୍ରତିଜ୍ଞା(bp_ଅଙ୍କ < X86_DEBUG_BP_N_REGS);

    dr7 = readDr7Context(t);
    ବାଛ (bp_ଅଙ୍କ) {
    ସ୍ଥିତି 0:
        ଫେରନ୍ତୁ !!(dr7 & X86_DEBUG_BP0_ENABLE_BIT);
    ସ୍ଥିତି 1:
        ଫେରନ୍ତୁ !!(dr7 & X86_DEBUG_BP1_ENABLE_BIT);
    ସ୍ଥିତି 2:
        ଫେରନ୍ତୁ !!(dr7 & X86_DEBUG_BP2_ENABLE_BIT);
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ !!(dr7 & X86_DEBUG_BP3_ENABLE_BIT);
    }
}

ସ୍ଥିର ଶୂନ୍ୟ setBpVaddrContext(tcb_t *t, uint16_t bp_ଅଙ୍କ, word_t vaddr)
{
    ପ୍ରତିଜ୍ଞା(t != ଖାଲି);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    ବାଛ (bp_ଅଙ୍କ) {
    ସ୍ଥିତି 0:
        ubs->dr[0] = vaddr;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 1:
        ubs->dr[1] = vaddr;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି 2:
        ubs->dr[2] = vaddr;
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ପ୍ରତିଜ୍ଞା(bp_ଅଙ୍କ == 3);
        ubs->dr[3] = vaddr;
        ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ;
}

/** Backend for the seL4_TCB_SetBreakpoint invocation.
 *
 * @param uds Arch TCB register context structure.
 * @param bp_num Hardware breakpoint ID.
 * @param vaddr USerspace virtual address on which you'd like this breakpoint
 *        to trigger.
 * @param types One of the seL4_BreakpointType values.
 * @param size positive integer indicating the byte-range size that should
 *        trigger the breakpoint. 0 is valid for Instruction breakpoints.
 * @param rw Access type that should trigger the BP (read/write).
 */
ଶୂନ୍ୟ setBreakpoint(tcb_t *t,
                   uint16_t bp_ଅଙ୍କ, word_t vaddr, word_t types, word_t size, word_t rw)
{
    word_t dr7val;

    ପ୍ରତିଜ୍ଞା(t != ଖାଲି);

    dr7val = convertTypeAndAccessToArch(bp_ଅଙ୍କ, types, rw);
    dr7val |= convertSizeToArch(bp_ଅଙ୍କ, types, size);

    setBpVaddrContext(t, bp_ଅଙ୍କ, vaddr);
    unsetDr7BitsFor(t, bp_ଅଙ୍କ);
    bitwiseOrDr7Context(t, dr7val);
    enableBreakpoint(t, bp_ଅଙ୍କ);
}

ସ୍ଥିର word_t getBpVaddrContext(tcb_t *t, uint16_t bp_ଅଙ୍କ)
{
    ପ୍ରତିଜ୍ଞା(t != ଖାଲି);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    ବାଛ (bp_ଅଙ୍କ) {
    ସ୍ଥିତି 0:
        ଫେରନ୍ତୁ ubs->dr[0];
    ସ୍ଥିତି 1:
        ଫେରନ୍ତୁ ubs->dr[1];
    ସ୍ଥିତି 2:
        ଫେରନ୍ତୁ ubs->dr[2];
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ପ୍ରତିଜ୍ଞା(bp_ଅଙ୍କ == 3);
        ଫେରନ୍ତୁ ubs->dr[3];
    }
}

/** Backend for the x86 seL4_TCB_GetBreakpoint invocation.
 *
 * Returns information about a particular breakpoint ID, including whether or
 * not it's enabled.
 *
 * @param uds Arch TCB register context pointer.
 * @param bp_num Hardware breakpoint ID of the BP you'd like to query.
 * @return Structure containing information about the status of the breakpoint.
 */
getBreakpoint_t getBreakpoint(tcb_t *t, uint16_t bp_ଅଙ୍କ)
{
    word_t dr7val;
    getBreakpoint_t ret;
    convertedTypeAndAccess_t res;

    dr7val = readDr7Context(t);
    ret.vaddr = getBpVaddrContext(t, bp_ଅଙ୍କ);
    ret.size = convertArchToSize(dr7val, bp_ଅଙ୍କ);
    res = convertArchToTypeAndAccess(dr7val, bp_ଅଙ୍କ);
    ret.type = res.type;
    ret.rw = res.rw;
    ret.is_enabled = breakpointIsEnabled(t, bp_ଅଙ୍କ);
    ଫେରନ୍ତୁ ret;
}

/** Backend for the x86 seL4_TCB_UnsetBreakpoint invocation.
 *
 * Unsets and *clears* a hardware breakpoint.
 * @param uds Arch TCB register context pointer.
 * @param bp_num The hardware breakpoint ID you'd like to clear.
 */
ଶୂନ୍ୟ unsetBreakpoint(tcb_t *t, uint16_t bp_ଅଙ୍କ)
{
    disableBreakpoint(t, bp_ଅଙ୍କ);
    unsetDr7BitsFor(t, bp_ଅଙ୍କ);
    setBpVaddrContext(t, bp_ଅଙ୍କ, 0);
}

/** Used in the exception path to determine if an exception was caused by
 * single-stepping being active.
 *
 * @param uc Arch TCB register context structure.
 * @return a structure stating whether or not the exception was caused by
 *          hardware single-stepping, and what the instruction vaddr was.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    bool_t ret;
    word_t instr_vaddr;
} testAndResetSingleStepException_t;

ସ୍ଥିର testAndResetSingleStepException_t testAndResetSingleStepException(tcb_t *t)
{
    testAndResetSingleStepException_t ret;
    word_t dr6;

    dr6 = readDr6Reg();
    ଯଦି (!(dr6 & X86_DEBUG_DR6_SINGLE_STEP_FLAG)) {
        ret.ret = false;
        ଫେରନ୍ତୁ ret;
    }

    ret.ret = true;
    ret.instr_vaddr = t->tcbArch.tcbContext.registers[FaultIP];
    bitwiseAndDr6Reg(~X86_DEBUG_DR6_SINGLE_STEP_FLAG);

    /* And that's not all: if the breakpoint is an instruction breakpoint, we
     * also need to set EFLAGS.RF. The processor raises the #DB exception BEFORE
     * the instruction executes. This means that when we IRET to userspace, the
     * SAME breakpoint will trigger again, and so on ad infinitum. EFLAGS.RF
     * solves this problem:
     *
     * When EFLAGS.RF is set, the processor will ignore instruction breakpoints
     * that should be raised, for one instruction. After that instruction
     * executes, the processor will also automatically unset EFLAGS.RF. See
     * Intel manuals, vol3, section 17.3.1.1.
     */
    /* This will automatically be popped by restore_user_context() */
    t->tcbArch.tcbContext.registers[FLAGS] |= X86_DEBUG_EFLAGS_RESUME_FLAG;
    ଫେରନ୍ତୁ ret;
}

bool_t configureSingleStepping(tcb_t *t, uint16_t bp_ଅଙ୍କ, word_t n_instr,
                               UNUSED bool_t is_reply)
{
    /* On x86 no hardware breakpoints are needed for single stepping. */
    ଯଦି (n_instr == 0) {
        /* If n_instr (number of instructions to single-step) is 0, that is the
          * same as requesting that single-stepping be disabled.
          */
        t->tcbArch.tcbContext.breakpointState.single_step_enabled = false;
        t->tcbArch.tcbContext.registers[FLAGS] &= ~X86_DEBUG_EFLAGS_TRAP_FLAG;
    } ଅନ୍ୟଥା {
        t->tcbArch.tcbContext.breakpointState.single_step_enabled = true;
    }

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    ଫେରନ୍ତୁ false;
}

/** Used in the exception path to determine which breakpoint triggered the
 * exception.
 *
 * First, checks to see which hardware breakpoint was triggered, and saves
 * the ID of that breakpoint. Secondly, resets that breakpoint such that its
 * "triggered" bit is no longer in the asserted state -- whatever that means
 * for the arch. So on x86, that means clearing the indicator bit in DR6.
 *
 * Aside from the ID of the breakpoint that was raised, also returns
 * information about the breakpoint (vaddr, access, type, etc).
 *
 * @param uc Arch TCB register context pointer.
 * @return Structure with a "bp_num" member that states which hardware
 *          breakpoint was triggered, and gives information describing the
 *          breakpoint.
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା bp_ଅଙ୍କ;
    word_t vaddr, reason;
} getAndResetActiveBreakpoint_t;

ସ୍ଥିର getAndResetActiveBreakpoint_t getAndResetActiveBreakpoint(tcb_t *t)
{
    convertedTypeAndAccess_t tmp;
    getAndResetActiveBreakpoint_t ret;

    /* Read from the hardware regs, not user context */
    word_t dr6 = readDr6Reg();
    ଯଦି (dr6 & BIT(0)) {
        ret.bp_ଅଙ୍କ = 0;
    } ଅନ୍ୟଥା ଯଦି (dr6 & BIT(1)) {
        ret.bp_ଅଙ୍କ = 1;
    } ଅନ୍ୟଥା ଯଦି (dr6 & BIT(2)) {
        ret.bp_ଅଙ୍କ = 2;
    } ଅନ୍ୟଥା ଯଦି (dr6 & BIT(3)) {
        ret.bp_ଅଙ୍କ = 3;
    } ଅନ୍ୟଥା {
        ret.bp_ଅଙ୍କ = -1;
        ଫେରନ୍ତୁ ret;
    }

    tmp = convertArchToTypeAndAccess(readDr7Context(t), ret.bp_ଅଙ୍କ);
    ret.vaddr = getBpVaddrContext(t, ret.bp_ଅଙ୍କ);
    ret.reason = tmp.type;

    bitwiseAndDr6Reg(~BIT(ret.bp_ଅଙ୍କ));
    ଫେରନ୍ତୁ ret;
}

exception_t handleUserLevelDebugException(ପୂର୍ଣ୍ଣସଂଖ୍ୟା int_vector)
{
    tcb_t *ct;
    getAndResetActiveBreakpoint_t active_bp;
    testAndResetSingleStepException_t single_step_info;

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_UserLevelFault;
    ksKernelEntry.word = int_vector;
#endif /* DEBUG */

#ifdef CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    benchmark_track_start();
#endif

    ct = ଗଣ୍ଠି_STATE(ksCurThread);

    /* Software break request (INT3) is detected by the vector number */
    ଯଦି (int_vector == int_software_break_request) {
        current_fault = seL4_Fault_DebugException_new(getRestartPC(ଗଣ୍ଠି_STATE(ksCurThread)),
                                                      0, seL4_SoftwareBreakRequest);
    } ଅନ୍ୟଥା {
        /* Hardware breakpoint trigger is detected using DR6 */
        active_bp = getAndResetActiveBreakpoint(ct);
        ଯଦି (active_bp.bp_ଅଙ୍କ >= 0) {
            current_fault = seL4_Fault_DebugException_new(active_bp.vaddr,
                                                          active_bp.bp_ଅଙ୍କ,
                                                          active_bp.reason);
        } ଅନ୍ୟଥା {
            single_step_info = testAndResetSingleStepException(ct);
            ଯଦି (single_step_info.ret == true) {
                /* If the caller asked us to skip over N instructions before
                 * generating the next single-step breakpoint, we shouldn't
                 * bother to construct a fault message until we've skipped N
                 * instructions.
                 */
                ଯଦି (singleStepFaultCounterReady(ct) == false) {
                    ଫେରନ୍ତୁ EXCEPTION_NONE;
                }
                current_fault = seL4_Fault_DebugException_new(single_step_info.instr_vaddr,
                                                              0, seL4_SingleStep);
            } ଅନ୍ୟଥା {
                ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
            }
        }
    }

    handleFault(ଗଣ୍ଠି_STATE(ksCurThread));

    schedule();
    activateThread();

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

BOOT_CODE bool_t Arch_initHardwareBreakpoints(ଶୂନ୍ୟ)
{
    x86_cpu_identity_t *modelinfo;

    modelinfo = x86_cpuid_get_model_info();
    /* Intel manuals, vol3, section 17.2.4, "NOTES". */
    ଯଦି (modelinfo->family == 15) {
        ଯଦି (modelinfo->model == 3 || modelinfo->model == 4
            || modelinfo->model == 6) {
            byte8_bps_supported = true;
        }
    }
    ଯଦି (modelinfo->family == 6) {
        ଯଦି (modelinfo->model == 15 || modelinfo->model == 23
            || modelinfo->model == 0x1C) {
            byte8_bps_supported = true;
        }
    }
    ଫେରନ୍ତୁ true;
}

ଶୂନ୍ୟ Arch_initBreakpointContext(user_breakpoint_state_t *uds)
{
    memset(uds, 0, ଆକାର(*uds));

    /* Preload reserved values into register context */
    uds->dr[4] = readDr6Reg() &
                 ~(BIT(0)
                   | BIT(1)
                   | BIT(2)
                   | BIT(3)
                   | X86_DEBUG_DR6_SINGLE_STEP_FLAG);

    uds->dr[5] = readDr7Reg() &
                 ~(X86_DEBUG_BP0_ENABLE_BIT | X86_DEBUG_BP1_ENABLE_BIT
                   | X86_DEBUG_BP2_ENABLE_BIT
                   | X86_DEBUG_BP3_ENABLE_BIT);
}

#endif
