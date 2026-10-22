/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API

#ಅನ್ತರ್ಭಾವ <arch/machine/debug.h>
#ಅನ್ತರ್ಭಾವ <mode/machine/debug.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <sel4/plat/api/constants.h> /* seL4_NumHWBReakpoints */

/* Intel manual Vol3, 17.2.4 */
#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_BP_ಗಾತ್ರ_1_B                (0x0u)
#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_BP_ಗಾತ್ರ_2_B                (0x1u)
#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_BP_ಗಾತ್ರ_4_B                (0x3u)
#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_BP_ಗಾತ್ರ_8_B                (0x2u)

#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_BP0_ಗಾತ್ರ_ಸರಿಸು            (18)
#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_BP1_ಗಾತ್ರ_ಸರಿಸು            (22)
#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_BP2_ಗಾತ್ರ_ಸರಿಸು            (26)
#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_BP3_ಗಾತ್ರ_ಸರಿಸು            (30)

/* NOTE: Intel manual 17.2.4:
 * I/O breakpoints are supported by every processor later than i486, but only
 * when CR4.DE=1.
 * When CR4.DE=0, or if processor is earlier than i586, this bit is "Undefined",
 * which is not the same as "Reserved", so it won't trigger an exception - it
 * will just cause an undefined reaction from the CPU.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_BP_TYPE_IO                (0x2u)
#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_BP_TYPE_INSTR             (0x0u)
#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_BP_TYPE_ದತ್ತಾಂಶ_WRITE        (0x1u)
#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_BP_TYPE_ದತ್ತಾಂಶ_READWRITE    (0x3u)

#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_BP0_TYPE_ಸರಿಸು            (16)
#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_BP1_TYPE_ಸರಿಸು            (20)
#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_BP2_TYPE_ಸರಿಸು            (24)
#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_BP3_TYPE_ಸರಿಸು            (28)

#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_EFLAGS_TRAP_FLAG    ((ಶಬ್ದ_t)ಬಿಟ್(8))
#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_EFLAGS_RESUME_FLAG    ((ಶಬ್ದ_t)ಬಿಟ್(16))
#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_DR6_SINGLE_ಹೆಜ್ಜೆ_FLAG  ((ಶಬ್ದ_t)ಬಿಟ್(14))

#ವ್ಯಾಖ್ಯಾನಿಸು X86_DEBUG_DR6_BP_ಮುಖವಾಡ     (0xFu)

ಸ್ಥಿರ bool_t byte8_bps_supported = false;

bool_t byte8BreakpointsSupported(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ byte8_bps_supported;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ bitwiseAndDr6Reg(ಶಬ್ದ_t ಮುಖವಾಡ_2)
{
    ಶಬ್ದ_t tmp;

    tmp = readDr6Reg() & ಮುಖವಾಡ_2;
    writeDr6Reg(tmp);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶಬ್ದ_t readDr7Context(tcb_t *t)
{
    ಹಿಂತಿರುಗಿ t->tcbArch.tcbContext.breakpointState.dr[5];
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ bitwiseOrDr7Context(tcb_t *t, ಶಬ್ದ_t val)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] |= val;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ bitwiseAndDr7Context(tcb_t *t, ಶಬ್ದ_t ಮುಖವಾಡ_2)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] &= ಮುಖವಾಡ_2;
}

ಸ್ಥಿರ ಶೂನ್ಯ unset_Dr7_ಬಿಟ್ಟುಗಳು_For(tcb_t *t, uint16_t bp_ಅಂಕೆ)
{
    ಶಬ್ದ_t ಮುಖವಾಡ_2;

    ಆಯ್ಕೆ (bp_ಅಂಕೆ) {
    ಸ್ಥಿತಿ 0:
        ಮುಖವಾಡ_2 = (0x3u << X86_DEBUG_BP0_ಗಾತ್ರ_ಸರಿಸು) | (0x3u << X86_DEBUG_BP0_TYPE_ಸರಿಸು);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 1:
        ಮುಖವಾಡ_2 = (0x3u << X86_DEBUG_BP1_ಗಾತ್ರ_ಸರಿಸು) | (0x3u << X86_DEBUG_BP1_TYPE_ಸರಿಸು);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 2:
        ಮುಖವಾಡ_2 = (0x3u << X86_DEBUG_BP2_ಗಾತ್ರ_ಸರಿಸು) | (0x3u << X86_DEBUG_BP2_TYPE_ಸರಿಸು);
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ: /* 3 */
        ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ == 3);
        ಮುಖವಾಡ_2 = (0x3u << X86_DEBUG_BP3_ಗಾತ್ರ_ಸರಿಸು) | (0x3u << X86_DEBUG_BP3_TYPE_ಸರಿಸು);
        ನಿಲ್ಲಿಸು;
    }

    ಮುಖವಾಡ_2 = ~ಮುಖವಾಡ_2;
    bitwiseAndDr7Context(t, ಮುಖವಾಡ_2);
}

/** Converts an seL4_BreakpointType value into the underlying hardware
 * equivalent.
 * @param bp_num Breakpoint number.
 * @param type One of the values of seL4_BreakpointType.
 * @param rw Access trigger condition (read/write).
 * @return Hardware specific register value representing the inputs.
 */
ಶುದ್ಧ ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶಬ್ದ_t convert_Type_And_Access_ಗಮ್ಯ_Arch(uint16_t bp_ಅಂಕೆ, ಶಬ್ದ_t type, ಶಬ್ದ_t rw)
{
    ಆಯ್ಕೆ (type) {
    ಸ್ಥಿತಿ seL4_InstructionBreakpoint:
        type = X86_DEBUG_BP_TYPE_INSTR;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ: /* seL4_DataBreakpoint */
        ಪ್ರತಿಜ್ಞಾ(type == se_L4_ದತ್ತಾಂಶ_Breakpoint);
        type = (rw == seL4_BreakOnWrite)
               ? X86_DEBUG_BP_TYPE_ದತ್ತಾಂಶ_WRITE
               : X86_DEBUG_BP_TYPE_ದತ್ತಾಂಶ_READWRITE;
    }

    ಆಯ್ಕೆ (bp_ಅಂಕೆ) {
    ಸ್ಥಿತಿ 0:
        ಹಿಂತಿರುಗಿ type << X86_DEBUG_BP0_TYPE_ಸರಿಸು;
    ಸ್ಥಿತಿ 1:
        ಹಿಂತಿರುಗಿ type << X86_DEBUG_BP1_TYPE_ಸರಿಸು;
    ಸ್ಥಿತಿ 2:
        ಹಿಂತಿರುಗಿ type << X86_DEBUG_BP2_TYPE_ಸರಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ: /* 3 */
        ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ == 3);
        ಹಿಂತಿರುಗಿ type << X86_DEBUG_BP3_TYPE_ಸರಿಸು;
    }
}

/** Reverse of convertTypeAndAccessToArch(): converts hardware values into
 * seL4 API values.
 * @param dr7 Hardware register value as input for conversion.
 * @param bp_num Breakpoint number.
 * @param type[out] Converted type value.
 * @param rw[out] Converted output access trigger value.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಶಬ್ದ_t type, rw;
} convertedTypeAndAccess_t;

ಶುದ್ಧ ಸ್ಥಿರ ಸಾಲೊಳಗೆ convertedTypeAndAccess_t convert_Arch_ಗಮ್ಯ_Type_And_Access(ಶಬ್ದ_t dr7, uint16_t bp_ಅಂಕೆ)
{
    convertedTypeAndAccess_t ret;

    ಆಯ್ಕೆ (bp_ಅಂಕೆ) {
    ಸ್ಥಿತಿ 0:
        dr7 &= 0x3u << X86_DEBUG_BP0_TYPE_ಸರಿಸು;
        dr7 >>= X86_DEBUG_BP0_TYPE_ಸರಿಸು;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 1:
        dr7 &= 0x3u << X86_DEBUG_BP1_TYPE_ಸರಿಸು;
        dr7 >>= X86_DEBUG_BP1_TYPE_ಸರಿಸು;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 2:
        dr7 &= 0x3u << X86_DEBUG_BP2_TYPE_ಸರಿಸು;
        dr7 >>= X86_DEBUG_BP2_TYPE_ಸರಿಸು;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ: /* 3 */
        ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ == 3);
        dr7 &= 0x3u << X86_DEBUG_BP3_TYPE_ಸರಿಸು;
        dr7 >>= X86_DEBUG_BP3_TYPE_ಸರಿಸು;
    }

    ಆಯ್ಕೆ (dr7) {
    ಸ್ಥಿತಿ X86_DEBUG_BP_TYPE_INSTR:
        ret.type = seL4_InstructionBreakpoint;
        ret.rw = seL4_BreakOnRead;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ X86_DEBUG_BP_TYPE_ದತ್ತಾಂಶ_WRITE:
        ret.type = se_L4_ದತ್ತಾಂಶ_Breakpoint;
        ret.rw = seL4_BreakOnWrite;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ: /* Read-write */
        ಪ್ರತಿಜ್ಞಾ(dr7 == X86_DEBUG_BP_TYPE_ದತ್ತಾಂಶ_READWRITE);
        ret.type = se_L4_ದತ್ತಾಂಶ_Breakpoint;
        ret.rw = seL4_BreakOnReadWrite;
        ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ ret;
}

/** Converts an integer size number into an equivalent hardware register value.
 * @param n Breakpoint number.
 * @param type One value from seL4_BreakpointType.
 * @param size An integer for the operand size of the breakpoint.
 * @return Converted, hardware-specific value.
 */
ಶುದ್ಧ ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶಬ್ದ_t convert_ಗಾತ್ರ_ಗಮ್ಯ_Arch(uint16_t bp_ಅಂಕೆ, ಶಬ್ದ_t type, ಶಬ್ದ_t ಗಾತ್ರ_2)
{
    ಆದರೆ (type == seL4_InstructionBreakpoint) {
        /* Intel manual vol3 17.2.4:
         * "If the corresponding RWn field in register DR7 is 00 (instruction
         * execution), then the LENn field should also be 00"
         */
        ಗಾತ್ರ_2 = 0;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆಯ್ಕೆ (ಗಾತ್ರ_2) {
        ಸ್ಥಿತಿ 1:
            ಗಾತ್ರ_2 = X86_DEBUG_BP_ಗಾತ್ರ_1_B;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 2:
            ಗಾತ್ರ_2 = X86_DEBUG_BP_ಗಾತ್ರ_2_B;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ 8:
            ಗಾತ್ರ_2 = X86_DEBUG_BP_ಗಾತ್ರ_8_B;
            ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ: /* 4B */
            ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ_2 == 4);
            ಗಾತ್ರ_2 = X86_DEBUG_BP_ಗಾತ್ರ_4_B;
        }
    }

    ಆಯ್ಕೆ (bp_ಅಂಕೆ) {
    ಸ್ಥಿತಿ 0:
        ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2 << X86_DEBUG_BP0_ಗಾತ್ರ_ಸರಿಸು;
    ಸ್ಥಿತಿ 1:
        ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2 << X86_DEBUG_BP1_ಗಾತ್ರ_ಸರಿಸು;
    ಸ್ಥಿತಿ 2:
        ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2 << X86_DEBUG_BP2_ಗಾತ್ರ_ಸರಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ: /* 3 */
        ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ == 3);
        ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2 << X86_DEBUG_BP3_ಗಾತ್ರ_ಸರಿಸು;
    }
}

/** Reverse of convertSizeToArch(): converts a hardware-specific size value
 * into an integer representation.
 * @param dr7 Hardware register value as input.
 * @param n Breakpoint number.
 * @return Converted size value.
 */
ಶುದ್ಧ ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶಬ್ದ_t convert_Arch_ಗಮ್ಯ_ಗಾತ್ರ(ಶಬ್ದ_t dr7, uint16_t bp_ಅಂಕೆ)
{
    ಶಬ್ದ_t type;

    ಆಯ್ಕೆ (bp_ಅಂಕೆ) {
    ಸ್ಥಿತಿ 0:
        type = dr7 & (0x3u << X86_DEBUG_BP0_TYPE_ಸರಿಸು);
        type >>= X86_DEBUG_BP0_TYPE_ಸರಿಸು;
        dr7 &= 0x3u << X86_DEBUG_BP0_ಗಾತ್ರ_ಸರಿಸು;
        dr7 >>= X86_DEBUG_BP0_ಗಾತ್ರ_ಸರಿಸು;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 1:
        type = dr7 & (0x3u << X86_DEBUG_BP1_TYPE_ಸರಿಸು);
        type >>= X86_DEBUG_BP1_TYPE_ಸರಿಸು;
        dr7 &= 0x3u << X86_DEBUG_BP1_ಗಾತ್ರ_ಸರಿಸು;
        dr7 >>= X86_DEBUG_BP1_ಗಾತ್ರ_ಸರಿಸು;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 2:
        type = dr7 & (0x3u << X86_DEBUG_BP2_TYPE_ಸರಿಸು);
        type >>= X86_DEBUG_BP2_TYPE_ಸರಿಸು;
        dr7 &= 0x3u << X86_DEBUG_BP2_ಗಾತ್ರ_ಸರಿಸು;
        dr7 >>= X86_DEBUG_BP2_ಗಾತ್ರ_ಸರಿಸು;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ: /* 3 */
        ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ == 3);
        type = dr7 & (0x3u << X86_DEBUG_BP3_TYPE_ಸರಿಸು);
        type >>= X86_DEBUG_BP3_TYPE_ಸರಿಸು;
        dr7 &= 0x3u << X86_DEBUG_BP3_ಗಾತ್ರ_ಸರಿಸು;
        dr7 >>= X86_DEBUG_BP3_ಗಾತ್ರ_ಸರಿಸು;
    }

    /* Force size to 0 if type is instruction breakpoint. */
    ಆದರೆ (type == X86_DEBUG_BP_TYPE_INSTR) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಆಯ್ಕೆ (dr7) {
    ಸ್ಥಿತಿ X86_DEBUG_BP_ಗಾತ್ರ_1_B:
        ಹಿಂತಿರುಗಿ 1;
    ಸ್ಥಿತಿ X86_DEBUG_BP_ಗಾತ್ರ_2_B:
        ಹಿಂತಿರುಗಿ 2;
    ಸ್ಥಿತಿ X86_DEBUG_BP_ಗಾತ್ರ_8_B:
        ಹಿಂತಿರುಗಿ 8;
    ಪೂರ್ವನಿಯೋಜಿತ: /* 4B */
        ಪ್ರತಿಜ್ಞಾ(dr7 == X86_DEBUG_BP_ಗಾತ್ರ_4_B);
        ಹಿಂತಿರುಗಿ 4;
    }
}

/** Enables a breakpoint.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ಸ್ಥಿರ ಶೂನ್ಯ enableBreakpoint(tcb_t *t, uint16_t bp_ಅಂಕೆ)
{
    ಶಬ್ದ_t enable_ಬಿಟ್;

    ಪ್ರತಿಜ್ಞಾ(t != ಖಾಲಿ);
    ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ < X86_DEBUG_BP_N_REGS);

    ಆಯ್ಕೆ (bp_ಅಂಕೆ) {
    ಸ್ಥಿತಿ 0:
        enable_ಬಿಟ್ = X86_DEBUG_BP0_ENABLE_ಬಿಟ್;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 1:
        enable_ಬಿಟ್ = X86_DEBUG_BP1_ENABLE_ಬಿಟ್;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 2:
        enable_ಬಿಟ್ = X86_DEBUG_BP2_ENABLE_ಬಿಟ್;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        enable_ಬಿಟ್ = X86_DEBUG_BP3_ENABLE_ಬಿಟ್;
        ನಿಲ್ಲಿಸು;
    }

    bitwiseOrDr7Context(t, enable_ಬಿಟ್);
}

/** Disables a breakpoint without clearing its configuration.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ಸ್ಥಿರ ಶೂನ್ಯ disableBreakpoint(tcb_t *t, uint16_t bp_ಅಂಕೆ)
{
    ಶಬ್ದ_t disable_ಮುಖವಾಡ;

    ಪ್ರತಿಜ್ಞಾ(t != ಖಾಲಿ);
    ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ < X86_DEBUG_BP_N_REGS);

    ಆಯ್ಕೆ (bp_ಅಂಕೆ) {
    ಸ್ಥಿತಿ 0:
        disable_ಮುಖವಾಡ = ~X86_DEBUG_BP0_ENABLE_ಬಿಟ್;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 1:
        disable_ಮುಖವಾಡ = ~X86_DEBUG_BP1_ENABLE_ಬಿಟ್;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 2:
        disable_ಮುಖವಾಡ = ~X86_DEBUG_BP2_ENABLE_ಬಿಟ್;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        disable_ಮುಖವಾಡ = ~X86_DEBUG_BP3_ENABLE_ಬಿಟ್;
        ನಿಲ್ಲಿಸು;
    }

    bitwiseAndDr7Context(t, disable_ಮುಖವಾಡ);
}

/** Returns a boolean for whether or not a breakpoint is enabled.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ಸ್ಥಿರ bool_t breakpointIsEnabled(tcb_t *t, uint16_t bp_ಅಂಕೆ)
{
    ಶಬ್ದ_t dr7;

    ಪ್ರತಿಜ್ಞಾ(t != ಖಾಲಿ);
    ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ < X86_DEBUG_BP_N_REGS);

    dr7 = readDr7Context(t);
    ಆಯ್ಕೆ (bp_ಅಂಕೆ) {
    ಸ್ಥಿತಿ 0:
        ಹಿಂತಿರುಗಿ !!(dr7 & X86_DEBUG_BP0_ENABLE_ಬಿಟ್);
    ಸ್ಥಿತಿ 1:
        ಹಿಂತಿರುಗಿ !!(dr7 & X86_DEBUG_BP1_ENABLE_ಬಿಟ್);
    ಸ್ಥಿತಿ 2:
        ಹಿಂತಿರುಗಿ !!(dr7 & X86_DEBUG_BP2_ENABLE_ಬಿಟ್);
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ !!(dr7 & X86_DEBUG_BP3_ENABLE_ಬಿಟ್);
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_Bp_Vaddr_Context(tcb_t *t, uint16_t bp_ಅಂಕೆ, ಶಬ್ದ_t vaddr)
{
    ಪ್ರತಿಜ್ಞಾ(t != ಖಾಲಿ);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    ಆಯ್ಕೆ (bp_ಅಂಕೆ) {
    ಸ್ಥಿತಿ 0:
        ubs->dr[0] = vaddr;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 1:
        ubs->dr[1] = vaddr;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 2:
        ubs->dr[2] = vaddr;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ == 3);
        ubs->dr[3] = vaddr;
        ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ;
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
ಶೂನ್ಯ ಗಣ_Breakpoint(tcb_t *t,
                   uint16_t bp_ಅಂಕೆ, ಶಬ್ದ_t vaddr, ಶಬ್ದ_t types, ಶಬ್ದ_t ಗಾತ್ರ_2, ಶಬ್ದ_t rw)
{
    ಶಬ್ದ_t dr7val;

    ಪ್ರತಿಜ್ಞಾ(t != ಖಾಲಿ);

    dr7val = convert_Type_And_Access_ಗಮ್ಯ_Arch(bp_ಅಂಕೆ, types, rw);
    dr7val |= convert_ಗಾತ್ರ_ಗಮ್ಯ_Arch(bp_ಅಂಕೆ, types, ಗಾತ್ರ_2);

    ಗಣ_Bp_Vaddr_Context(t, bp_ಅಂಕೆ, vaddr);
    unset_Dr7_ಬಿಟ್ಟುಗಳು_For(t, bp_ಅಂಕೆ);
    bitwiseOrDr7Context(t, dr7val);
    enableBreakpoint(t, bp_ಅಂಕೆ);
}

ಸ್ಥಿರ ಶಬ್ದ_t getBpVaddrContext(tcb_t *t, uint16_t bp_ಅಂಕೆ)
{
    ಪ್ರತಿಜ್ಞಾ(t != ಖಾಲಿ);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    ಆಯ್ಕೆ (bp_ಅಂಕೆ) {
    ಸ್ಥಿತಿ 0:
        ಹಿಂತಿರುಗಿ ubs->dr[0];
    ಸ್ಥಿತಿ 1:
        ಹಿಂತಿರುಗಿ ubs->dr[1];
    ಸ್ಥಿತಿ 2:
        ಹಿಂತಿರುಗಿ ubs->dr[2];
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಪ್ರತಿಜ್ಞಾ(bp_ಅಂಕೆ == 3);
        ಹಿಂತಿರುಗಿ ubs->dr[3];
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
    ಶಬ್ದ_t dr7val;
    getBreakpoint_t ret;
    convertedTypeAndAccess_t res;

    dr7val = readDr7Context(t);
    ret.vaddr = getBpVaddrContext(t, bp_ಅಂಕೆ);
    ret.ಗಾತ್ರ_2 = convert_Arch_ಗಮ್ಯ_ಗಾತ್ರ(dr7val, bp_ಅಂಕೆ);
    res = convert_Arch_ಗಮ್ಯ_Type_And_Access(dr7val, bp_ಅಂಕೆ);
    ret.type = res.type;
    ret.rw = res.rw;
    ret.is_enabled = breakpointIsEnabled(t, bp_ಅಂಕೆ);
    ಹಿಂತಿರುಗಿ ret;
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
    unset_Dr7_ಬಿಟ್ಟುಗಳು_For(t, bp_ಅಂಕೆ);
    ಗಣ_Bp_Vaddr_Context(t, bp_ಅಂಕೆ, 0);
}

/** Used in the exception path to determine if an exception was caused by
 * single-stepping being active.
 *
 * @param uc Arch TCB register context structure.
 * @return a structure stating whether or not the exception was caused by
 *          hardware single-stepping, and what the instruction vaddr was.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    bool_t ret;
    ಶಬ್ದ_t instr_vaddr;
} test_And_Reset_Single_ಹೆಜ್ಜೆ_Exception_t;

ಸ್ಥಿರ test_And_Reset_Single_ಹೆಜ್ಜೆ_Exception_t test_And_Reset_Single_ಹೆಜ್ಜೆ_Exception(tcb_t *t)
{
    test_And_Reset_Single_ಹೆಜ್ಜೆ_Exception_t ret;
    ಶಬ್ದ_t dr6;

    dr6 = readDr6Reg();
    ಆದರೆ (!(dr6 & X86_DEBUG_DR6_SINGLE_ಹೆಜ್ಜೆ_FLAG)) {
        ret.ret = false;
        ಹಿಂತಿರುಗಿ ret;
    }

    ret.ret = true;
    ret.instr_vaddr = t->tcbArch.tcbContext.registers[FaultIP];
    bitwiseAndDr6Reg(~X86_DEBUG_DR6_SINGLE_ಹೆಜ್ಜೆ_FLAG);

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
    ಹಿಂತಿರುಗಿ ret;
}

bool_t configureSingleStepping(tcb_t *t, uint16_t bp_ಅಂಕೆ, ಶಬ್ದ_t n_instr,
                               ಬಳಸದ bool_t is_ಉತ್ತರ)
{
    /* On x86 no hardware breakpoints are needed for single stepping. */
    ಆದರೆ (n_instr == 0) {
        /* If n_instr (number of instructions to single-step) is 0, that is the
          * same as requesting that single-stepping be disabled.
          */
        t->tcbArch.tcbContext.breakpointState.single_ಹೆಜ್ಜೆ_enabled = false;
        t->tcbArch.tcbContext.registers[FLAGS] &= ~X86_DEBUG_EFLAGS_TRAP_FLAG;
    } ಇಲ್ಲದಿದ್ದರೆ {
        t->tcbArch.tcbContext.breakpointState.single_ಹೆಜ್ಜೆ_enabled = true;
    }

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    ಹಿಂತಿರುಗಿ false;
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ bp_ಅಂಕೆ;
    ಶಬ್ದ_t vaddr, reason;
} get_And_Reset_ಸಕ್ರಿಯ_Breakpoint_t;

ಸ್ಥಿರ get_And_Reset_ಸಕ್ರಿಯ_Breakpoint_t get_And_Reset_ಸಕ್ರಿಯ_Breakpoint(tcb_t *t)
{
    convertedTypeAndAccess_t tmp;
    get_And_Reset_ಸಕ್ರಿಯ_Breakpoint_t ret;

    /* Read from the hardware regs, not user context */
    ಶಬ್ದ_t dr6 = readDr6Reg();
    ಆದರೆ (dr6 & ಬಿಟ್(0)) {
        ret.bp_ಅಂಕೆ = 0;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dr6 & ಬಿಟ್(1)) {
        ret.bp_ಅಂಕೆ = 1;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dr6 & ಬಿಟ್(2)) {
        ret.bp_ಅಂಕೆ = 2;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dr6 & ಬಿಟ್(3)) {
        ret.bp_ಅಂಕೆ = 3;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ret.bp_ಅಂಕೆ = -1;
        ಹಿಂತಿರುಗಿ ret;
    }

    tmp = convert_Arch_ಗಮ್ಯ_Type_And_Access(readDr7Context(t), ret.bp_ಅಂಕೆ);
    ret.vaddr = getBpVaddrContext(t, ret.bp_ಅಂಕೆ);
    ret.reason = tmp.type;

    bitwiseAndDr6Reg(~ಬಿಟ್(ret.bp_ಅಂಕೆ));
    ಹಿಂತಿರುಗಿ ret;
}

exception_t handleUserLevelDebugException(ಪೂರ್ಣಾಂಕ int_ಸದಿಶ)
{
    tcb_t *ct;
    get_And_Reset_ಸಕ್ರಿಯ_Breakpoint_t ಸಕ್ರಿಯ_bp;
    test_And_Reset_Single_ಹೆಜ್ಜೆ_Exception_t single_ಹೆಜ್ಜೆ_info;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
    ks_Kernel_ನಮೂದು.ಮಾರ್ಗ = ನಮೂದು_User_Level_Fault;
    ks_Kernel_ನಮೂದು.ಶಬ್ದ = int_ಸದಿಶ;
#ಆದರೆ_ಅಂತ್ಯ /* DEBUG */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    benchmark_track_ಆರಂಭ();
#ಆದರೆ_ಅಂತ್ಯ

    ct = ಗಂಟು_STATE(ksCurThread);

    /* Software break request (INT3) is detected by the vector number */
    ಆದರೆ (int_ಸದಿಶ == int_software_break_request) {
        current_fault = seL4_Fault_DebugException_new(getRestartPC(ಗಂಟು_STATE(ksCurThread)),
                                                      0, seL4_SoftwareBreakRequest);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Hardware breakpoint trigger is detected using DR6 */
        ಸಕ್ರಿಯ_bp = get_And_Reset_ಸಕ್ರಿಯ_Breakpoint(ct);
        ಆದರೆ (ಸಕ್ರಿಯ_bp.bp_ಅಂಕೆ >= 0) {
            current_fault = seL4_Fault_DebugException_new(ಸಕ್ರಿಯ_bp.vaddr,
                                                          ಸಕ್ರಿಯ_bp.bp_ಅಂಕೆ,
                                                          ಸಕ್ರಿಯ_bp.reason);
        } ಇಲ್ಲದಿದ್ದರೆ {
            single_ಹೆಜ್ಜೆ_info = test_And_Reset_Single_ಹೆಜ್ಜೆ_Exception(ct);
            ಆದರೆ (single_ಹೆಜ್ಜೆ_info.ret == true) {
                /* If the caller asked us to skip over N instructions before
                 * generating the next single-step breakpoint, we shouldn't
                 * bother to construct a fault message until we've skipped N
                 * instructions.
                 */
                ಆದರೆ (single_ಹೆಜ್ಜೆ_Fault_Counter_Ready(ct) == false) {
                    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
                }
                current_fault = seL4_Fault_DebugException_new(single_ಹೆಜ್ಜೆ_info.instr_vaddr,
                                                              0, se_L4_Single_ಹೆಜ್ಜೆ);
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }
        }
    }

    handleFault(ಗಂಟು_STATE(ksCurThread));

    ವೇಳಾಪಟ್ಟಿ();
    activateThread();

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

BOOT_ಸಂಕೇತ bool_t Arch_initHardwareBreakpoints(ಶೂನ್ಯ)
{
    x86_cpu_ತಟಸ್ಥ_t *modelinfo;

    modelinfo = x86_cpuid_get_model_info();
    /* Intel manuals, vol3, section 17.2.4, "NOTES". */
    ಆದರೆ (modelinfo->family == 15) {
        ಆದರೆ (modelinfo->model == 3 || modelinfo->model == 4
            || modelinfo->model == 6) {
            byte8_bps_supported = true;
        }
    }
    ಆದರೆ (modelinfo->family == 6) {
        ಆದರೆ (modelinfo->model == 15 || modelinfo->model == 23
            || modelinfo->model == 0x1C) {
            byte8_bps_supported = true;
        }
    }
    ಹಿಂತಿರುಗಿ true;
}

ಶೂನ್ಯ Arch_initBreakpointContext(user_breakpoint_state_t *uds)
{
    memset(uds, 0, ಗಾತ್ರ(*uds));

    /* Preload reserved values into register context */
    uds->dr[4] = readDr6Reg() &
                 ~(ಬಿಟ್(0)
                   | ಬಿಟ್(1)
                   | ಬಿಟ್(2)
                   | ಬಿಟ್(3)
                   | X86_DEBUG_DR6_SINGLE_ಹೆಜ್ಜೆ_FLAG);

    uds->dr[5] = readDr7Reg() &
                 ~(X86_DEBUG_BP0_ENABLE_ಬಿಟ್ | X86_DEBUG_BP1_ENABLE_ಬಿಟ್
                   | X86_DEBUG_BP2_ENABLE_ಬಿಟ್
                   | X86_DEBUG_BP3_ENABLE_ಬಿಟ್);
}

#ಆದರೆ_ಅಂತ್ಯ
