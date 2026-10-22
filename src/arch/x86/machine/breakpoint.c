/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ifdef CONFIG_HARDWARE_DEBUG_API

#ಅನ್ತರ್ಭಾವ <arch/machine/debug.h>
#ಅನ್ತರ್ಭಾವ <mode/machine/debug.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <sel4/plat/api/constants.h> /* seL4_NumHWBReakpoints */

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

ಸ್ಥಿರ bool_t byte8_bps_supported = false;

bool_t byte8BreakpointsSupported(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ byte8_bps_supported;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ bitwiseAndDr6Reg(word_t mask)
{
    word_t tmp;

    tmp = readDr6Reg() & mask;
    writeDr6Reg(tmp);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ word_t readDr7Context(tcb_t *t)
{
    ಪ್ರತ್ಯಾಗಮ t->tcbArch.tcbContext.breakpointState.dr[5];
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ bitwiseOrDr7Context(tcb_t *t, word_t val)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] |= val;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ bitwiseAndDr7Context(tcb_t *t, word_t mask)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] &= mask;
}

ಸ್ಥಿರ ಶೂನ್ಯ unsetDr7BitsFor(tcb_t *t, uint16_t bp_ಅಂಕೆ)
{
    word_t mask;

    ವಿಕಲ್ಪ (bp_ಅಂಕೆ) {
    ಪಕ್ಷ 0:
        mask = (0x3u << X86_DEBUG_BP0_SIZE_SHIFT) | (0x3u << X86_DEBUG_BP0_TYPE_SHIFT);
        ಭಙ್ಗ;
    ಪಕ್ಷ 1:
        mask = (0x3u << X86_DEBUG_BP1_SIZE_SHIFT) | (0x3u << X86_DEBUG_BP1_TYPE_SHIFT);
        ಭಙ್ಗ;
    ಪಕ್ಷ 2:
        mask = (0x3u << X86_DEBUG_BP2_SIZE_SHIFT) | (0x3u << X86_DEBUG_BP2_TYPE_SHIFT);
        ಭಙ್ಗ;
    ಸ್ವಭಾವ: /* 3 */
        ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ == 3);
        mask = (0x3u << X86_DEBUG_BP3_SIZE_SHIFT) | (0x3u << X86_DEBUG_BP3_TYPE_SHIFT);
        ಭಙ್ಗ;
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
PURE ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ word_t convertTypeAndAccessToArch(uint16_t bp_ಅಂಕೆ, word_t type, word_t rw)
{
    ವಿಕಲ್ಪ (type) {
    ಪಕ್ಷ seL4_InstructionBreakpoint:
        type = X86_DEBUG_BP_TYPE_INSTR;
        ಭಙ್ಗ;
    ಸ್ವಭಾವ: /* seL4_DataBreakpoint */
        ಪ್ರತಿಜ್ಞಾ(type == seL4_DataBreakpoint);
        type = (rw == seL4_BreakOnWrite)
               ? X86_DEBUG_BP_TYPE_DATA_WRITE
               : X86_DEBUG_BP_TYPE_DATA_READWRITE;
    }

    ವಿಕಲ್ಪ (bp_ಅಂಕೆ) {
    ಪಕ್ಷ 0:
        ಪ್ರತ್ಯಾಗಮ type << X86_DEBUG_BP0_TYPE_SHIFT;
    ಪಕ್ಷ 1:
        ಪ್ರತ್ಯಾಗಮ type << X86_DEBUG_BP1_TYPE_SHIFT;
    ಪಕ್ಷ 2:
        ಪ್ರತ್ಯಾಗಮ type << X86_DEBUG_BP2_TYPE_SHIFT;
    ಸ್ವಭಾವ: /* 3 */
        ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ == 3);
        ಪ್ರತ್ಯಾಗಮ type << X86_DEBUG_BP3_TYPE_SHIFT;
    }
}

/** Reverse of convertTypeAndAccessToArch(): converts hardware values into
 * seL4 API values.
 * @param dr7 Hardware register value as input for conversion.
 * @param bp_num Breakpoint number.
 * @param type[out] Converted type value.
 * @param rw[out] Converted output access trigger value.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    word_t type, rw;
} convertedTypeAndAccess_t;

PURE ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ convertedTypeAndAccess_t convertArchToTypeAndAccess(word_t dr7, uint16_t bp_ಅಂಕೆ)
{
    convertedTypeAndAccess_t ret;

    ವಿಕಲ್ಪ (bp_ಅಂಕೆ) {
    ಪಕ್ಷ 0:
        dr7 &= 0x3u << X86_DEBUG_BP0_TYPE_SHIFT;
        dr7 >>= X86_DEBUG_BP0_TYPE_SHIFT;
        ಭಙ್ಗ;
    ಪಕ್ಷ 1:
        dr7 &= 0x3u << X86_DEBUG_BP1_TYPE_SHIFT;
        dr7 >>= X86_DEBUG_BP1_TYPE_SHIFT;
        ಭಙ್ಗ;
    ಪಕ್ಷ 2:
        dr7 &= 0x3u << X86_DEBUG_BP2_TYPE_SHIFT;
        dr7 >>= X86_DEBUG_BP2_TYPE_SHIFT;
        ಭಙ್ಗ;
    ಸ್ವಭಾವ: /* 3 */
        ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ == 3);
        dr7 &= 0x3u << X86_DEBUG_BP3_TYPE_SHIFT;
        dr7 >>= X86_DEBUG_BP3_TYPE_SHIFT;
    }

    ವಿಕಲ್ಪ (dr7) {
    ಪಕ್ಷ X86_DEBUG_BP_TYPE_INSTR:
        ret.type = seL4_InstructionBreakpoint;
        ret.rw = seL4_BreakOnRead;
        ಭಙ್ಗ;
    ಪಕ್ಷ X86_DEBUG_BP_TYPE_DATA_WRITE:
        ret.type = seL4_DataBreakpoint;
        ret.rw = seL4_BreakOnWrite;
        ಭಙ್ಗ;
    ಸ್ವಭಾವ: /* Read-write */
        ಪ್ರತಿಜ್ಞಾ(dr7 == X86_DEBUG_BP_TYPE_DATA_READWRITE);
        ret.type = seL4_DataBreakpoint;
        ret.rw = seL4_BreakOnReadWrite;
        ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ ret;
}

/** Converts an integer size number into an equivalent hardware register value.
 * @param n Breakpoint number.
 * @param type One value from seL4_BreakpointType.
 * @param size An integer for the operand size of the breakpoint.
 * @return Converted, hardware-specific value.
 */
PURE ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ word_t convertSizeToArch(uint16_t bp_ಅಂಕೆ, word_t type, word_t size)
{
    ಯದಿ (type == seL4_InstructionBreakpoint) {
        /* Intel manual vol3 17.2.4:
         * "If the corresponding RWn field in register DR7 is 00 (instruction
         * execution), then the LENn field should also be 00"
         */
        size = 0;
    } ಅನ್ಯಥಾ {
        ವಿಕಲ್ಪ (size) {
        ಪಕ್ಷ 1:
            size = X86_DEBUG_BP_SIZE_1B;
            ಭಙ್ಗ;
        ಪಕ್ಷ 2:
            size = X86_DEBUG_BP_SIZE_2B;
            ಭಙ್ಗ;
        ಪಕ್ಷ 8:
            size = X86_DEBUG_BP_SIZE_8B;
            ಭಙ್ಗ;
        ಸ್ವಭಾವ: /* 4B */
            ಪ್ರತಿಜ್ಞಾ(size == 4);
            size = X86_DEBUG_BP_SIZE_4B;
        }
    }

    ವಿಕಲ್ಪ (bp_ಅಂಕೆ) {
    ಪಕ್ಷ 0:
        ಪ್ರತ್ಯಾಗಮ size << X86_DEBUG_BP0_SIZE_SHIFT;
    ಪಕ್ಷ 1:
        ಪ್ರತ್ಯಾಗಮ size << X86_DEBUG_BP1_SIZE_SHIFT;
    ಪಕ್ಷ 2:
        ಪ್ರತ್ಯಾಗಮ size << X86_DEBUG_BP2_SIZE_SHIFT;
    ಸ್ವಭಾವ: /* 3 */
        ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ == 3);
        ಪ್ರತ್ಯಾಗಮ size << X86_DEBUG_BP3_SIZE_SHIFT;
    }
}

/** Reverse of convertSizeToArch(): converts a hardware-specific size value
 * into an integer representation.
 * @param dr7 Hardware register value as input.
 * @param n Breakpoint number.
 * @return Converted size value.
 */
PURE ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ word_t convertArchToSize(word_t dr7, uint16_t bp_ಅಂಕೆ)
{
    word_t type;

    ವಿಕಲ್ಪ (bp_ಅಂಕೆ) {
    ಪಕ್ಷ 0:
        type = dr7 & (0x3u << X86_DEBUG_BP0_TYPE_SHIFT);
        type >>= X86_DEBUG_BP0_TYPE_SHIFT;
        dr7 &= 0x3u << X86_DEBUG_BP0_SIZE_SHIFT;
        dr7 >>= X86_DEBUG_BP0_SIZE_SHIFT;
        ಭಙ್ಗ;
    ಪಕ್ಷ 1:
        type = dr7 & (0x3u << X86_DEBUG_BP1_TYPE_SHIFT);
        type >>= X86_DEBUG_BP1_TYPE_SHIFT;
        dr7 &= 0x3u << X86_DEBUG_BP1_SIZE_SHIFT;
        dr7 >>= X86_DEBUG_BP1_SIZE_SHIFT;
        ಭಙ್ಗ;
    ಪಕ್ಷ 2:
        type = dr7 & (0x3u << X86_DEBUG_BP2_TYPE_SHIFT);
        type >>= X86_DEBUG_BP2_TYPE_SHIFT;
        dr7 &= 0x3u << X86_DEBUG_BP2_SIZE_SHIFT;
        dr7 >>= X86_DEBUG_BP2_SIZE_SHIFT;
        ಭಙ್ಗ;
    ಸ್ವಭಾವ: /* 3 */
        ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ == 3);
        type = dr7 & (0x3u << X86_DEBUG_BP3_TYPE_SHIFT);
        type >>= X86_DEBUG_BP3_TYPE_SHIFT;
        dr7 &= 0x3u << X86_DEBUG_BP3_SIZE_SHIFT;
        dr7 >>= X86_DEBUG_BP3_SIZE_SHIFT;
    }

    /* Force size to 0 if type is instruction breakpoint. */
    ಯದಿ (type == X86_DEBUG_BP_TYPE_INSTR) {
        ಪ್ರತ್ಯಾಗಮ 0;
    }

    ವಿಕಲ್ಪ (dr7) {
    ಪಕ್ಷ X86_DEBUG_BP_SIZE_1B:
        ಪ್ರತ್ಯಾಗಮ 1;
    ಪಕ್ಷ X86_DEBUG_BP_SIZE_2B:
        ಪ್ರತ್ಯಾಗಮ 2;
    ಪಕ್ಷ X86_DEBUG_BP_SIZE_8B:
        ಪ್ರತ್ಯಾಗಮ 8;
    ಸ್ವಭಾವ: /* 4B */
        ಪ್ರತಿಜ್ಞಾ(dr7 == X86_DEBUG_BP_SIZE_4B);
        ಪ್ರತ್ಯಾಗಮ 4;
    }
}

/** Enables a breakpoint.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ಸ್ಥಿರ ಶೂನ್ಯ enableBreakpoint(tcb_t *t, uint16_t bp_ಅಂಕೆ)
{
    word_t enable_bit;

    ಪ್ರತಿಜ್ಞಾ(t != ರಿಕ್ತ);
    ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ < X86_DEBUG_BP_N_REGS);

    ವಿಕಲ್ಪ (bp_ಅಂಕೆ) {
    ಪಕ್ಷ 0:
        enable_bit = X86_DEBUG_BP0_ENABLE_BIT;
        ಭಙ್ಗ;
    ಪಕ್ಷ 1:
        enable_bit = X86_DEBUG_BP1_ENABLE_BIT;
        ಭಙ್ಗ;
    ಪಕ್ಷ 2:
        enable_bit = X86_DEBUG_BP2_ENABLE_BIT;
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        enable_bit = X86_DEBUG_BP3_ENABLE_BIT;
        ಭಙ್ಗ;
    }

    bitwiseOrDr7Context(t, enable_bit);
}

/** Disables a breakpoint without clearing its configuration.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ಸ್ಥಿರ ಶೂನ್ಯ disableBreakpoint(tcb_t *t, uint16_t bp_ಅಂಕೆ)
{
    word_t disable_mask;

    ಪ್ರತಿಜ್ಞಾ(t != ರಿಕ್ತ);
    ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ < X86_DEBUG_BP_N_REGS);

    ವಿಕಲ್ಪ (bp_ಅಂಕೆ) {
    ಪಕ್ಷ 0:
        disable_mask = ~X86_DEBUG_BP0_ENABLE_BIT;
        ಭಙ್ಗ;
    ಪಕ್ಷ 1:
        disable_mask = ~X86_DEBUG_BP1_ENABLE_BIT;
        ಭಙ್ಗ;
    ಪಕ್ಷ 2:
        disable_mask = ~X86_DEBUG_BP2_ENABLE_BIT;
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        disable_mask = ~X86_DEBUG_BP3_ENABLE_BIT;
        ಭಙ್ಗ;
    }

    bitwiseAndDr7Context(t, disable_mask);
}

/** Returns a boolean for whether or not a breakpoint is enabled.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ಸ್ಥಿರ bool_t breakpointIsEnabled(tcb_t *t, uint16_t bp_ಅಂಕೆ)
{
    word_t dr7;

    ಪ್ರತಿಜ್ಞಾ(t != ರಿಕ್ತ);
    ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ < X86_DEBUG_BP_N_REGS);

    dr7 = readDr7Context(t);
    ವಿಕಲ್ಪ (bp_ಅಂಕೆ) {
    ಪಕ್ಷ 0:
        ಪ್ರತ್ಯಾಗಮ !!(dr7 & X86_DEBUG_BP0_ENABLE_BIT);
    ಪಕ್ಷ 1:
        ಪ್ರತ್ಯಾಗಮ !!(dr7 & X86_DEBUG_BP1_ENABLE_BIT);
    ಪಕ್ಷ 2:
        ಪ್ರತ್ಯಾಗಮ !!(dr7 & X86_DEBUG_BP2_ENABLE_BIT);
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ !!(dr7 & X86_DEBUG_BP3_ENABLE_BIT);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ setBpVaddrContext(tcb_t *t, uint16_t bp_ಅಂಕೆ, word_t vaddr)
{
    ಪ್ರತಿಜ್ಞಾ(t != ರಿಕ್ತ);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    ವಿಕಲ್ಪ (bp_ಅಂಕೆ) {
    ಪಕ್ಷ 0:
        ubs->dr[0] = vaddr;
        ಭಙ್ಗ;
    ಪಕ್ಷ 1:
        ubs->dr[1] = vaddr;
        ಭಙ್ಗ;
    ಪಕ್ಷ 2:
        ubs->dr[2] = vaddr;
        ಭಙ್ಗ;
    ಸ್ವಭಾವ:
        ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ == 3);
        ubs->dr[3] = vaddr;
        ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ;
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
ಶೂನ್ಯ setBreakpoint(tcb_t *t,
                   uint16_t bp_ಅಂಕೆ, word_t vaddr, word_t types, word_t size, word_t rw)
{
    word_t dr7val;

    ಪ್ರತಿಜ್ಞಾ(t != ರಿಕ್ತ);

    dr7val = convertTypeAndAccessToArch(bp_ಅಂಕೆ, types, rw);
    dr7val |= convertSizeToArch(bp_ಅಂಕೆ, types, size);

    setBpVaddrContext(t, bp_ಅಂಕೆ, vaddr);
    unsetDr7BitsFor(t, bp_ಅಂಕೆ);
    bitwiseOrDr7Context(t, dr7val);
    enableBreakpoint(t, bp_ಅಂಕೆ);
}

ಸ್ಥಿರ word_t getBpVaddrContext(tcb_t *t, uint16_t bp_ಅಂಕೆ)
{
    ಪ್ರತಿಜ್ಞಾ(t != ರಿಕ್ತ);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    ವಿಕಲ್ಪ (bp_ಅಂಕೆ) {
    ಪಕ್ಷ 0:
        ಪ್ರತ್ಯಾಗಮ ubs->dr[0];
    ಪಕ್ಷ 1:
        ಪ್ರತ್ಯಾಗಮ ubs->dr[1];
    ಪಕ್ಷ 2:
        ಪ್ರತ್ಯಾಗಮ ubs->dr[2];
    ಸ್ವಭಾವ:
        ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ == 3);
        ಪ್ರತ್ಯಾಗಮ ubs->dr[3];
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
getBreakpoint_t getBreakpoint(tcb_t *t, uint16_t bp_ಅಂಕೆ)
{
    word_t dr7val;
    getBreakpoint_t ret;
    convertedTypeAndAccess_t res;

    dr7val = readDr7Context(t);
    ret.vaddr = getBpVaddrContext(t, bp_ಅಂಕೆ);
    ret.size = convertArchToSize(dr7val, bp_ಅಂಕೆ);
    res = convertArchToTypeAndAccess(dr7val, bp_ಅಂಕೆ);
    ret.type = res.type;
    ret.rw = res.rw;
    ret.is_enabled = breakpointIsEnabled(t, bp_ಅಂಕೆ);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/** Backend for the x86 seL4_TCB_UnsetBreakpoint invocation.
 *
 * Unsets and *clears* a hardware breakpoint.
 * @param uds Arch TCB register context pointer.
 * @param bp_num The hardware breakpoint ID you'd like to clear.
 */
ಶೂನ್ಯ unsetBreakpoint(tcb_t *t, uint16_t bp_ಅಂಕೆ)
{
    disableBreakpoint(t, bp_ಅಂಕೆ);
    unsetDr7BitsFor(t, bp_ಅಂಕೆ);
    setBpVaddrContext(t, bp_ಅಂಕೆ, 0);
}

/** Used in the exception path to determine if an exception was caused by
 * single-stepping being active.
 *
 * @param uc Arch TCB register context structure.
 * @return a structure stating whether or not the exception was caused by
 *          hardware single-stepping, and what the instruction vaddr was.
 */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    bool_t ret;
    word_t instr_vaddr;
} testAndResetSingleStepException_t;

ಸ್ಥಿರ testAndResetSingleStepException_t testAndResetSingleStepException(tcb_t *t)
{
    testAndResetSingleStepException_t ret;
    word_t dr6;

    dr6 = readDr6Reg();
    ಯದಿ (!(dr6 & X86_DEBUG_DR6_SINGLE_STEP_FLAG)) {
        ret.ret = false;
        ಪ್ರತ್ಯಾಗಮ ret;
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
    ಪ್ರತ್ಯಾಗಮ ret;
}

bool_t configureSingleStepping(tcb_t *t, uint16_t bp_ಅಂಕೆ, word_t n_instr,
                               UNUSED bool_t is_reply)
{
    /* On x86 no hardware breakpoints are needed for single stepping. */
    ಯದಿ (n_instr == 0) {
        /* If n_instr (number of instructions to single-step) is 0, that is the
          * same as requesting that single-stepping be disabled.
          */
        t->tcbArch.tcbContext.breakpointState.single_step_enabled = false;
        t->tcbArch.tcbContext.registers[FLAGS] &= ~X86_DEBUG_EFLAGS_TRAP_FLAG;
    } ಅನ್ಯಥಾ {
        t->tcbArch.tcbContext.breakpointState.single_step_enabled = true;
    }

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    ಪ್ರತ್ಯಾಗಮ false;
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ bp_ಅಂಕೆ;
    word_t vaddr, reason;
} getAndResetActiveBreakpoint_t;

ಸ್ಥಿರ getAndResetActiveBreakpoint_t getAndResetActiveBreakpoint(tcb_t *t)
{
    convertedTypeAndAccess_t tmp;
    getAndResetActiveBreakpoint_t ret;

    /* Read from the hardware regs, not user context */
    word_t dr6 = readDr6Reg();
    ಯದಿ (dr6 & BIT(0)) {
        ret.bp_ಅಂಕೆ = 0;
    } ಅನ್ಯಥಾ ಯದಿ (dr6 & BIT(1)) {
        ret.bp_ಅಂಕೆ = 1;
    } ಅನ್ಯಥಾ ಯದಿ (dr6 & BIT(2)) {
        ret.bp_ಅಂಕೆ = 2;
    } ಅನ್ಯಥಾ ಯದಿ (dr6 & BIT(3)) {
        ret.bp_ಅಂಕೆ = 3;
    } ಅನ್ಯಥಾ {
        ret.bp_ಅಂಕೆ = -1;
        ಪ್ರತ್ಯಾಗಮ ret;
    }

    tmp = convertArchToTypeAndAccess(readDr7Context(t), ret.bp_ಅಂಕೆ);
    ret.vaddr = getBpVaddrContext(t, ret.bp_ಅಂಕೆ);
    ret.reason = tmp.type;

    bitwiseAndDr6Reg(~BIT(ret.bp_ಅಂಕೆ));
    ಪ್ರತ್ಯಾಗಮ ret;
}

exception_t handleUserLevelDebugException(ಪೂರ್ಣಾಙ್ಕ int_vector)
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

    ct = ಗಂಟು_STATE(ksCurThread);

    /* Software break request (INT3) is detected by the vector number */
    ಯದಿ (int_vector == int_software_break_request) {
        current_fault = seL4_Fault_DebugException_new(getRestartPC(ಗಂಟು_STATE(ksCurThread)),
                                                      0, seL4_SoftwareBreakRequest);
    } ಅನ್ಯಥಾ {
        /* Hardware breakpoint trigger is detected using DR6 */
        active_bp = getAndResetActiveBreakpoint(ct);
        ಯದಿ (active_bp.bp_ಅಂಕೆ >= 0) {
            current_fault = seL4_Fault_DebugException_new(active_bp.vaddr,
                                                          active_bp.bp_ಅಂಕೆ,
                                                          active_bp.reason);
        } ಅನ್ಯಥಾ {
            single_step_info = testAndResetSingleStepException(ct);
            ಯದಿ (single_step_info.ret == true) {
                /* If the caller asked us to skip over N instructions before
                 * generating the next single-step breakpoint, we shouldn't
                 * bother to construct a fault message until we've skipped N
                 * instructions.
                 */
                ಯದಿ (singleStepFaultCounterReady(ct) == false) {
                    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
                }
                current_fault = seL4_Fault_DebugException_new(single_step_info.instr_vaddr,
                                                              0, seL4_SingleStep);
            } ಅನ್ಯಥಾ {
                ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
            }
        }
    }

    handleFault(ಗಂಟು_STATE(ksCurThread));

    schedule();
    activateThread();

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

BOOT_CODE bool_t Arch_initHardwareBreakpoints(ಶೂನ್ಯ)
{
    x86_cpu_identity_t *modelinfo;

    modelinfo = x86_cpuid_get_model_info();
    /* Intel manuals, vol3, section 17.2.4, "NOTES". */
    ಯದಿ (modelinfo->family == 15) {
        ಯದಿ (modelinfo->model == 3 || modelinfo->model == 4
            || modelinfo->model == 6) {
            byte8_bps_supported = true;
        }
    }
    ಯದಿ (modelinfo->family == 6) {
        ಯದಿ (modelinfo->model == 15 || modelinfo->model == 23
            || modelinfo->model == 0x1C) {
            byte8_bps_supported = true;
        }
    }
    ಪ್ರತ್ಯಾಗಮ true;
}

ಶೂನ್ಯ Arch_initBreakpointContext(user_breakpoint_state_t *uds)
{
    memset(uds, 0, ಪರಿಮಾಣ(*uds));

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
