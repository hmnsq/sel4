/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/machine/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/registerset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sel4/plat/api/constants.h> /* seL4_NumHWBReakpoints */

/* Intel manual Vol3, 17.2.4 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_BP_ਮਾਪ_1_B                (0x0u)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_BP_ਮਾਪ_2_B                (0x1u)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_BP_ਮਾਪ_4_B                (0x3u)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_BP_ਮਾਪ_8_B                (0x2u)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_BP0_ਮਾਪ_ਖਿਸਕਾਓ            (18)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_BP1_ਮਾਪ_ਖਿਸਕਾਓ            (22)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_BP2_ਮਾਪ_ਖਿਸਕਾਓ            (26)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_BP3_ਮਾਪ_ਖਿਸਕਾਓ            (30)

/* NOTE: Intel manual 17.2.4:
 * I/O breakpoints are supported by every processor later than i486, but only
 * when CR4.DE=1.
 * When CR4.DE=0, or if processor is earlier than i586, this bit is "Undefined",
 * which is not the same as "Reserved", so it won't trigger an exception - it
 * will just cause an undefined reaction from the CPU.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_BP_TYPE_IO                (0x2u)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_BP_TYPE_INSTR             (0x0u)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_BP_TYPE_ਅੰਕੜੇ_WRITE        (0x1u)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_BP_TYPE_ਅੰਕੜੇ_READWRITE    (0x3u)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_BP0_TYPE_ਖਿਸਕਾਓ            (16)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_BP1_TYPE_ਖਿਸਕਾਓ            (20)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_BP2_TYPE_ਖਿਸਕਾਓ            (24)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_BP3_TYPE_ਖਿਸਕਾਓ            (28)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_EFLAGS_TRAP_FLAG    ((ਸ਼ਬਦ_t)ਦੋਆਧਾਰੀ_ਅੰਕ(8))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_EFLAGS_RESUME_FLAG    ((ਸ਼ਬਦ_t)ਦੋਆਧਾਰੀ_ਅੰਕ(16))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_DR6_SINGLE_ਕਦਮ_FLAG  ((ਸ਼ਬਦ_t)ਦੋਆਧਾਰੀ_ਅੰਕ(14))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ X86_DEBUG_DR6_BP_ਨਕਾਬ     (0xFu)

ਸਥਿਰ bool_t byte8_bps_supported = false;

bool_t byte8BreakpointsSupported(ਖਾਲੀ)
{
    ਵਾਪਸ byte8_bps_supported;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ bitwiseAndDr6Reg(ਸ਼ਬਦ_t ਨਕਾਬ_2)
{
    ਸ਼ਬਦ_t tmp;

    tmp = readDr6Reg() & ਨਕਾਬ_2;
    writeDr6Reg(tmp);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਸ਼ਬਦ_t readDr7Context(tcb_t *t)
{
    ਵਾਪਸ t->tcbArch.tcbContext.breakpointState.dr[5];
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ bitwiseOrDr7Context(tcb_t *t, ਸ਼ਬਦ_t val)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] |= val;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ bitwiseAndDr7Context(tcb_t *t, ਸ਼ਬਦ_t ਨਕਾਬ_2)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] &= ਨਕਾਬ_2;
}

ਸਥਿਰ ਖਾਲੀ unset_Dr7_ਬਿੱਟ_For(tcb_t *t, uint16_t bp_ਅੰਕ)
{
    ਸ਼ਬਦ_t ਨਕਾਬ_2;

    ਚੋਣ (bp_ਅੰਕ) {
    ਹਾਲਤ 0:
        ਨਕਾਬ_2 = (0x3u << X86_DEBUG_BP0_ਮਾਪ_ਖਿਸਕਾਓ) | (0x3u << X86_DEBUG_BP0_TYPE_ਖਿਸਕਾਓ);
        ਰੋਕੋ;
    ਹਾਲਤ 1:
        ਨਕਾਬ_2 = (0x3u << X86_DEBUG_BP1_ਮਾਪ_ਖਿਸਕਾਓ) | (0x3u << X86_DEBUG_BP1_TYPE_ਖਿਸਕਾਓ);
        ਰੋਕੋ;
    ਹਾਲਤ 2:
        ਨਕਾਬ_2 = (0x3u << X86_DEBUG_BP2_ਮਾਪ_ਖਿਸਕਾਓ) | (0x3u << X86_DEBUG_BP2_TYPE_ਖਿਸਕਾਓ);
        ਰੋਕੋ;
    ਮੂਲ: /* 3 */
        ਪ੍ਰਤਿਜ੍ਞਾ(bp_ਅੰਕ == 3);
        ਨਕਾਬ_2 = (0x3u << X86_DEBUG_BP3_ਮਾਪ_ਖਿਸਕਾਓ) | (0x3u << X86_DEBUG_BP3_TYPE_ਖਿਸਕਾਓ);
        ਰੋਕੋ;
    }

    ਨਕਾਬ_2 = ~ਨਕਾਬ_2;
    bitwiseAndDr7Context(t, ਨਕਾਬ_2);
}

/** Converts an seL4_BreakpointType value into the underlying hardware
 * equivalent.
 * @param bp_num Breakpoint number.
 * @param type One of the values of seL4_BreakpointType.
 * @param rw Access trigger condition (read/write).
 * @return Hardware specific register value representing the inputs.
 */
ਸ਼ੁੱਧ ਸਥਿਰ ਸਤਰਅੰਦਰ ਸ਼ਬਦ_t convert_Type_And_Access_ਮੰਜ਼ਿਲ_Arch(uint16_t bp_ਅੰਕ, ਸ਼ਬਦ_t type, ਸ਼ਬਦ_t rw)
{
    ਚੋਣ (type) {
    ਹਾਲਤ seL4_InstructionBreakpoint:
        type = X86_DEBUG_BP_TYPE_INSTR;
        ਰੋਕੋ;
    ਮੂਲ: /* seL4_DataBreakpoint */
        ਪ੍ਰਤਿਜ੍ਞਾ(type == se_L4_ਅੰਕੜੇ_Breakpoint);
        type = (rw == seL4_BreakOnWrite)
               ? X86_DEBUG_BP_TYPE_ਅੰਕੜੇ_WRITE
               : X86_DEBUG_BP_TYPE_ਅੰਕੜੇ_READWRITE;
    }

    ਚੋਣ (bp_ਅੰਕ) {
    ਹਾਲਤ 0:
        ਵਾਪਸ type << X86_DEBUG_BP0_TYPE_ਖਿਸਕਾਓ;
    ਹਾਲਤ 1:
        ਵਾਪਸ type << X86_DEBUG_BP1_TYPE_ਖਿਸਕਾਓ;
    ਹਾਲਤ 2:
        ਵਾਪਸ type << X86_DEBUG_BP2_TYPE_ਖਿਸਕਾਓ;
    ਮੂਲ: /* 3 */
        ਪ੍ਰਤਿਜ੍ਞਾ(bp_ਅੰਕ == 3);
        ਵਾਪਸ type << X86_DEBUG_BP3_TYPE_ਖਿਸਕਾਓ;
    }
}

/** Reverse of convertTypeAndAccessToArch(): converts hardware values into
 * seL4 API values.
 * @param dr7 Hardware register value as input for conversion.
 * @param bp_num Breakpoint number.
 * @param type[out] Converted type value.
 * @param rw[out] Converted output access trigger value.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਸ਼ਬਦ_t type, rw;
} convertedTypeAndAccess_t;

ਸ਼ੁੱਧ ਸਥਿਰ ਸਤਰਅੰਦਰ convertedTypeAndAccess_t convert_Arch_ਮੰਜ਼ਿਲ_Type_And_Access(ਸ਼ਬਦ_t dr7, uint16_t bp_ਅੰਕ)
{
    convertedTypeAndAccess_t ret;

    ਚੋਣ (bp_ਅੰਕ) {
    ਹਾਲਤ 0:
        dr7 &= 0x3u << X86_DEBUG_BP0_TYPE_ਖਿਸਕਾਓ;
        dr7 >>= X86_DEBUG_BP0_TYPE_ਖਿਸਕਾਓ;
        ਰੋਕੋ;
    ਹਾਲਤ 1:
        dr7 &= 0x3u << X86_DEBUG_BP1_TYPE_ਖਿਸਕਾਓ;
        dr7 >>= X86_DEBUG_BP1_TYPE_ਖਿਸਕਾਓ;
        ਰੋਕੋ;
    ਹਾਲਤ 2:
        dr7 &= 0x3u << X86_DEBUG_BP2_TYPE_ਖਿਸਕਾਓ;
        dr7 >>= X86_DEBUG_BP2_TYPE_ਖਿਸਕਾਓ;
        ਰੋਕੋ;
    ਮੂਲ: /* 3 */
        ਪ੍ਰਤਿਜ੍ਞਾ(bp_ਅੰਕ == 3);
        dr7 &= 0x3u << X86_DEBUG_BP3_TYPE_ਖਿਸਕਾਓ;
        dr7 >>= X86_DEBUG_BP3_TYPE_ਖਿਸਕਾਓ;
    }

    ਚੋਣ (dr7) {
    ਹਾਲਤ X86_DEBUG_BP_TYPE_INSTR:
        ret.type = seL4_InstructionBreakpoint;
        ret.rw = seL4_BreakOnRead;
        ਰੋਕੋ;
    ਹਾਲਤ X86_DEBUG_BP_TYPE_ਅੰਕੜੇ_WRITE:
        ret.type = se_L4_ਅੰਕੜੇ_Breakpoint;
        ret.rw = seL4_BreakOnWrite;
        ਰੋਕੋ;
    ਮੂਲ: /* Read-write */
        ਪ੍ਰਤਿਜ੍ਞਾ(dr7 == X86_DEBUG_BP_TYPE_ਅੰਕੜੇ_READWRITE);
        ret.type = se_L4_ਅੰਕੜੇ_Breakpoint;
        ret.rw = seL4_BreakOnReadWrite;
        ਰੋਕੋ;
    }
    ਵਾਪਸ ret;
}

/** Converts an integer size number into an equivalent hardware register value.
 * @param n Breakpoint number.
 * @param type One value from seL4_BreakpointType.
 * @param size An integer for the operand size of the breakpoint.
 * @return Converted, hardware-specific value.
 */
ਸ਼ੁੱਧ ਸਥਿਰ ਸਤਰਅੰਦਰ ਸ਼ਬਦ_t convert_ਮਾਪ_ਮੰਜ਼ਿਲ_Arch(uint16_t bp_ਅੰਕ, ਸ਼ਬਦ_t type, ਸ਼ਬਦ_t ਮਾਪ)
{
    ਜੇ (type == seL4_InstructionBreakpoint) {
        /* Intel manual vol3 17.2.4:
         * "If the corresponding RWn field in register DR7 is 00 (instruction
         * execution), then the LENn field should also be 00"
         */
        ਮਾਪ = 0;
    } ਨਹੀਂਤਾਂ {
        ਚੋਣ (ਮਾਪ) {
        ਹਾਲਤ 1:
            ਮਾਪ = X86_DEBUG_BP_ਮਾਪ_1_B;
            ਰੋਕੋ;
        ਹਾਲਤ 2:
            ਮਾਪ = X86_DEBUG_BP_ਮਾਪ_2_B;
            ਰੋਕੋ;
        ਹਾਲਤ 8:
            ਮਾਪ = X86_DEBUG_BP_ਮਾਪ_8_B;
            ਰੋਕੋ;
        ਮੂਲ: /* 4B */
            ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪ == 4);
            ਮਾਪ = X86_DEBUG_BP_ਮਾਪ_4_B;
        }
    }

    ਚੋਣ (bp_ਅੰਕ) {
    ਹਾਲਤ 0:
        ਵਾਪਸ ਮਾਪ << X86_DEBUG_BP0_ਮਾਪ_ਖਿਸਕਾਓ;
    ਹਾਲਤ 1:
        ਵਾਪਸ ਮਾਪ << X86_DEBUG_BP1_ਮਾਪ_ਖਿਸਕਾਓ;
    ਹਾਲਤ 2:
        ਵਾਪਸ ਮਾਪ << X86_DEBUG_BP2_ਮਾਪ_ਖਿਸਕਾਓ;
    ਮੂਲ: /* 3 */
        ਪ੍ਰਤਿਜ੍ਞਾ(bp_ਅੰਕ == 3);
        ਵਾਪਸ ਮਾਪ << X86_DEBUG_BP3_ਮਾਪ_ਖਿਸਕਾਓ;
    }
}

/** Reverse of convertSizeToArch(): converts a hardware-specific size value
 * into an integer representation.
 * @param dr7 Hardware register value as input.
 * @param n Breakpoint number.
 * @return Converted size value.
 */
ਸ਼ੁੱਧ ਸਥਿਰ ਸਤਰਅੰਦਰ ਸ਼ਬਦ_t convert_Arch_ਮੰਜ਼ਿਲ_ਮਾਪ(ਸ਼ਬਦ_t dr7, uint16_t bp_ਅੰਕ)
{
    ਸ਼ਬਦ_t type;

    ਚੋਣ (bp_ਅੰਕ) {
    ਹਾਲਤ 0:
        type = dr7 & (0x3u << X86_DEBUG_BP0_TYPE_ਖਿਸਕਾਓ);
        type >>= X86_DEBUG_BP0_TYPE_ਖਿਸਕਾਓ;
        dr7 &= 0x3u << X86_DEBUG_BP0_ਮਾਪ_ਖਿਸਕਾਓ;
        dr7 >>= X86_DEBUG_BP0_ਮਾਪ_ਖਿਸਕਾਓ;
        ਰੋਕੋ;
    ਹਾਲਤ 1:
        type = dr7 & (0x3u << X86_DEBUG_BP1_TYPE_ਖਿਸਕਾਓ);
        type >>= X86_DEBUG_BP1_TYPE_ਖਿਸਕਾਓ;
        dr7 &= 0x3u << X86_DEBUG_BP1_ਮਾਪ_ਖਿਸਕਾਓ;
        dr7 >>= X86_DEBUG_BP1_ਮਾਪ_ਖਿਸਕਾਓ;
        ਰੋਕੋ;
    ਹਾਲਤ 2:
        type = dr7 & (0x3u << X86_DEBUG_BP2_TYPE_ਖਿਸਕਾਓ);
        type >>= X86_DEBUG_BP2_TYPE_ਖਿਸਕਾਓ;
        dr7 &= 0x3u << X86_DEBUG_BP2_ਮਾਪ_ਖਿਸਕਾਓ;
        dr7 >>= X86_DEBUG_BP2_ਮਾਪ_ਖਿਸਕਾਓ;
        ਰੋਕੋ;
    ਮੂਲ: /* 3 */
        ਪ੍ਰਤਿਜ੍ਞਾ(bp_ਅੰਕ == 3);
        type = dr7 & (0x3u << X86_DEBUG_BP3_TYPE_ਖਿਸਕਾਓ);
        type >>= X86_DEBUG_BP3_TYPE_ਖਿਸਕਾਓ;
        dr7 &= 0x3u << X86_DEBUG_BP3_ਮਾਪ_ਖਿਸਕਾਓ;
        dr7 >>= X86_DEBUG_BP3_ਮਾਪ_ਖਿਸਕਾਓ;
    }

    /* Force size to 0 if type is instruction breakpoint. */
    ਜੇ (type == X86_DEBUG_BP_TYPE_INSTR) {
        ਵਾਪਸ 0;
    }

    ਚੋਣ (dr7) {
    ਹਾਲਤ X86_DEBUG_BP_ਮਾਪ_1_B:
        ਵਾਪਸ 1;
    ਹਾਲਤ X86_DEBUG_BP_ਮਾਪ_2_B:
        ਵਾਪਸ 2;
    ਹਾਲਤ X86_DEBUG_BP_ਮਾਪ_8_B:
        ਵਾਪਸ 8;
    ਮੂਲ: /* 4B */
        ਪ੍ਰਤਿਜ੍ਞਾ(dr7 == X86_DEBUG_BP_ਮਾਪ_4_B);
        ਵਾਪਸ 4;
    }
}

/** Enables a breakpoint.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ਸਥਿਰ ਖਾਲੀ enableBreakpoint(tcb_t *t, uint16_t bp_ਅੰਕ)
{
    ਸ਼ਬਦ_t enable_ਦੋਆਧਾਰੀ_ਅੰਕ;

    ਪ੍ਰਤਿਜ੍ਞਾ(t != ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(bp_ਅੰਕ < X86_DEBUG_BP_N_REGS);

    ਚੋਣ (bp_ਅੰਕ) {
    ਹਾਲਤ 0:
        enable_ਦੋਆਧਾਰੀ_ਅੰਕ = X86_DEBUG_BP0_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ;
        ਰੋਕੋ;
    ਹਾਲਤ 1:
        enable_ਦੋਆਧਾਰੀ_ਅੰਕ = X86_DEBUG_BP1_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ;
        ਰੋਕੋ;
    ਹਾਲਤ 2:
        enable_ਦੋਆਧਾਰੀ_ਅੰਕ = X86_DEBUG_BP2_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ;
        ਰੋਕੋ;
    ਮੂਲ:
        enable_ਦੋਆਧਾਰੀ_ਅੰਕ = X86_DEBUG_BP3_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ;
        ਰੋਕੋ;
    }

    bitwiseOrDr7Context(t, enable_ਦੋਆਧਾਰੀ_ਅੰਕ);
}

/** Disables a breakpoint without clearing its configuration.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ਸਥਿਰ ਖਾਲੀ disableBreakpoint(tcb_t *t, uint16_t bp_ਅੰਕ)
{
    ਸ਼ਬਦ_t disable_ਨਕਾਬ;

    ਪ੍ਰਤਿਜ੍ਞਾ(t != ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(bp_ਅੰਕ < X86_DEBUG_BP_N_REGS);

    ਚੋਣ (bp_ਅੰਕ) {
    ਹਾਲਤ 0:
        disable_ਨਕਾਬ = ~X86_DEBUG_BP0_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ;
        ਰੋਕੋ;
    ਹਾਲਤ 1:
        disable_ਨਕਾਬ = ~X86_DEBUG_BP1_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ;
        ਰੋਕੋ;
    ਹਾਲਤ 2:
        disable_ਨਕਾਬ = ~X86_DEBUG_BP2_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ;
        ਰੋਕੋ;
    ਮੂਲ:
        disable_ਨਕਾਬ = ~X86_DEBUG_BP3_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ;
        ਰੋਕੋ;
    }

    bitwiseAndDr7Context(t, disable_ਨਕਾਬ);
}

/** Returns a boolean for whether or not a breakpoint is enabled.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ਸਥਿਰ bool_t breakpointIsEnabled(tcb_t *t, uint16_t bp_ਅੰਕ)
{
    ਸ਼ਬਦ_t dr7;

    ਪ੍ਰਤਿਜ੍ਞਾ(t != ਨਿਰਾ);
    ਪ੍ਰਤਿਜ੍ਞਾ(bp_ਅੰਕ < X86_DEBUG_BP_N_REGS);

    dr7 = readDr7Context(t);
    ਚੋਣ (bp_ਅੰਕ) {
    ਹਾਲਤ 0:
        ਵਾਪਸ !!(dr7 & X86_DEBUG_BP0_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ);
    ਹਾਲਤ 1:
        ਵਾਪਸ !!(dr7 & X86_DEBUG_BP1_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ);
    ਹਾਲਤ 2:
        ਵਾਪਸ !!(dr7 & X86_DEBUG_BP2_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ);
    ਮੂਲ:
        ਵਾਪਸ !!(dr7 & X86_DEBUG_BP3_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ);
    }
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_Bp_Vaddr_Context(tcb_t *t, uint16_t bp_ਅੰਕ, ਸ਼ਬਦ_t vaddr)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(t != ਨਿਰਾ);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    ਚੋਣ (bp_ਅੰਕ) {
    ਹਾਲਤ 0:
        ubs->dr[0] = vaddr;
        ਰੋਕੋ;
    ਹਾਲਤ 1:
        ubs->dr[1] = vaddr;
        ਰੋਕੋ;
    ਹਾਲਤ 2:
        ubs->dr[2] = vaddr;
        ਰੋਕੋ;
    ਮੂਲ:
        ਪ੍ਰਤਿਜ੍ਞਾ(bp_ਅੰਕ == 3);
        ubs->dr[3] = vaddr;
        ਰੋਕੋ;
    }
    ਵਾਪਸ;
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
ਖਾਲੀ ਸੈੱਟ_Breakpoint(tcb_t *t,
                   uint16_t bp_ਅੰਕ, ਸ਼ਬਦ_t vaddr, ਸ਼ਬਦ_t types, ਸ਼ਬਦ_t ਮਾਪ, ਸ਼ਬਦ_t rw)
{
    ਸ਼ਬਦ_t dr7val;

    ਪ੍ਰਤਿਜ੍ਞਾ(t != ਨਿਰਾ);

    dr7val = convert_Type_And_Access_ਮੰਜ਼ਿਲ_Arch(bp_ਅੰਕ, types, rw);
    dr7val |= convert_ਮਾਪ_ਮੰਜ਼ਿਲ_Arch(bp_ਅੰਕ, types, ਮਾਪ);

    ਸੈੱਟ_Bp_Vaddr_Context(t, bp_ਅੰਕ, vaddr);
    unset_Dr7_ਬਿੱਟ_For(t, bp_ਅੰਕ);
    bitwiseOrDr7Context(t, dr7val);
    enableBreakpoint(t, bp_ਅੰਕ);
}

ਸਥਿਰ ਸ਼ਬਦ_t getBpVaddrContext(tcb_t *t, uint16_t bp_ਅੰਕ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(t != ਨਿਰਾ);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    ਚੋਣ (bp_ਅੰਕ) {
    ਹਾਲਤ 0:
        ਵਾਪਸ ubs->dr[0];
    ਹਾਲਤ 1:
        ਵਾਪਸ ubs->dr[1];
    ਹਾਲਤ 2:
        ਵਾਪਸ ubs->dr[2];
    ਮੂਲ:
        ਪ੍ਰਤਿਜ੍ਞਾ(bp_ਅੰਕ == 3);
        ਵਾਪਸ ubs->dr[3];
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
getBreakpoint_t getBreakpoint(tcb_t *t, uint16_t bp_ਅੰਕ)
{
    ਸ਼ਬਦ_t dr7val;
    getBreakpoint_t ret;
    convertedTypeAndAccess_t res;

    dr7val = readDr7Context(t);
    ret.vaddr = getBpVaddrContext(t, bp_ਅੰਕ);
    ret.ਮਾਪ = convert_Arch_ਮੰਜ਼ਿਲ_ਮਾਪ(dr7val, bp_ਅੰਕ);
    res = convert_Arch_ਮੰਜ਼ਿਲ_Type_And_Access(dr7val, bp_ਅੰਕ);
    ret.type = res.type;
    ret.rw = res.rw;
    ret.is_enabled = breakpointIsEnabled(t, bp_ਅੰਕ);
    ਵਾਪਸ ret;
}

/** Backend for the x86 seL4_TCB_UnsetBreakpoint invocation.
 *
 * Unsets and *clears* a hardware breakpoint.
 * @param uds Arch TCB register context pointer.
 * @param bp_num The hardware breakpoint ID you'd like to clear.
 */
ਖਾਲੀ unsetBreakpoint(tcb_t *t, uint16_t bp_ਅੰਕ)
{
    disableBreakpoint(t, bp_ਅੰਕ);
    unset_Dr7_ਬਿੱਟ_For(t, bp_ਅੰਕ);
    ਸੈੱਟ_Bp_Vaddr_Context(t, bp_ਅੰਕ, 0);
}

/** Used in the exception path to determine if an exception was caused by
 * single-stepping being active.
 *
 * @param uc Arch TCB register context structure.
 * @return a structure stating whether or not the exception was caused by
 *          hardware single-stepping, and what the instruction vaddr was.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    bool_t ret;
    ਸ਼ਬਦ_t instr_vaddr;
} test_And_Reset_Single_ਕਦਮ_Exception_t;

ਸਥਿਰ test_And_Reset_Single_ਕਦਮ_Exception_t test_And_Reset_Single_ਕਦਮ_Exception(tcb_t *t)
{
    test_And_Reset_Single_ਕਦਮ_Exception_t ret;
    ਸ਼ਬਦ_t dr6;

    dr6 = readDr6Reg();
    ਜੇ (!(dr6 & X86_DEBUG_DR6_SINGLE_ਕਦਮ_FLAG)) {
        ret.ret = false;
        ਵਾਪਸ ret;
    }

    ret.ret = true;
    ret.instr_vaddr = t->tcbArch.tcbContext.registers[FaultIP];
    bitwiseAndDr6Reg(~X86_DEBUG_DR6_SINGLE_ਕਦਮ_FLAG);

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
    ਵਾਪਸ ret;
}

bool_t configureSingleStepping(tcb_t *t, uint16_t bp_ਅੰਕ, ਸ਼ਬਦ_t n_instr,
                               ਅਣਵਰਤਿਆ bool_t is_ਜਵਾਬ)
{
    /* On x86 no hardware breakpoints are needed for single stepping. */
    ਜੇ (n_instr == 0) {
        /* If n_instr (number of instructions to single-step) is 0, that is the
          * same as requesting that single-stepping be disabled.
          */
        t->tcbArch.tcbContext.breakpointState.single_ਕਦਮ_enabled = false;
        t->tcbArch.tcbContext.registers[FLAGS] &= ~X86_DEBUG_EFLAGS_TRAP_FLAG;
    } ਨਹੀਂਤਾਂ {
        t->tcbArch.tcbContext.breakpointState.single_ਕਦਮ_enabled = true;
    }

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    ਵਾਪਸ false;
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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ bp_ਅੰਕ;
    ਸ਼ਬਦ_t vaddr, reason;
} get_And_Reset_ਸਰਗਰਮ_Breakpoint_t;

ਸਥਿਰ get_And_Reset_ਸਰਗਰਮ_Breakpoint_t get_And_Reset_ਸਰਗਰਮ_Breakpoint(tcb_t *t)
{
    convertedTypeAndAccess_t tmp;
    get_And_Reset_ਸਰਗਰਮ_Breakpoint_t ret;

    /* Read from the hardware regs, not user context */
    ਸ਼ਬਦ_t dr6 = readDr6Reg();
    ਜੇ (dr6 & ਦੋਆਧਾਰੀ_ਅੰਕ(0)) {
        ret.bp_ਅੰਕ = 0;
    } ਨਹੀਂਤਾਂ ਜੇ (dr6 & ਦੋਆਧਾਰੀ_ਅੰਕ(1)) {
        ret.bp_ਅੰਕ = 1;
    } ਨਹੀਂਤਾਂ ਜੇ (dr6 & ਦੋਆਧਾਰੀ_ਅੰਕ(2)) {
        ret.bp_ਅੰਕ = 2;
    } ਨਹੀਂਤਾਂ ਜੇ (dr6 & ਦੋਆਧਾਰੀ_ਅੰਕ(3)) {
        ret.bp_ਅੰਕ = 3;
    } ਨਹੀਂਤਾਂ {
        ret.bp_ਅੰਕ = -1;
        ਵਾਪਸ ret;
    }

    tmp = convert_Arch_ਮੰਜ਼ਿਲ_Type_And_Access(readDr7Context(t), ret.bp_ਅੰਕ);
    ret.vaddr = getBpVaddrContext(t, ret.bp_ਅੰਕ);
    ret.reason = tmp.type;

    bitwiseAndDr6Reg(~ਦੋਆਧਾਰੀ_ਅੰਕ(ret.bp_ਅੰਕ));
    ਵਾਪਸ ret;
}

exception_t handleUserLevelDebugException(ਪੂਰਨ int_ਸਦਿਸ਼)
{
    tcb_t *ct;
    get_And_Reset_ਸਰਗਰਮ_Breakpoint_t ਸਰਗਰਮ_bp;
    test_And_Reset_Single_ਕਦਮ_Exception_t single_ਕਦਮ_info;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
    ks_Kernel_ਇੰਦਰਾਜ.ਰਸਤਾ = ਇੰਦਰਾਜ_User_Level_Fault;
    ks_Kernel_ਇੰਦਰਾਜ.ਸ਼ਬਦ_2 = int_ਸਦਿਸ਼;
#ਜੇ_ਅੰਤ /* DEBUG */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    benchmark_track_ਸ਼ੁਰੂ();
#ਜੇ_ਅੰਤ

    ct = ਗੰਢ_STATE(ksCurThread);

    /* Software break request (INT3) is detected by the vector number */
    ਜੇ (int_ਸਦਿਸ਼ == int_software_break_request) {
        current_fault = seL4_Fault_DebugException_new(getRestartPC(ਗੰਢ_STATE(ksCurThread)),
                                                      0, seL4_SoftwareBreakRequest);
    } ਨਹੀਂਤਾਂ {
        /* Hardware breakpoint trigger is detected using DR6 */
        ਸਰਗਰਮ_bp = get_And_Reset_ਸਰਗਰਮ_Breakpoint(ct);
        ਜੇ (ਸਰਗਰਮ_bp.bp_ਅੰਕ >= 0) {
            current_fault = seL4_Fault_DebugException_new(ਸਰਗਰਮ_bp.vaddr,
                                                          ਸਰਗਰਮ_bp.bp_ਅੰਕ,
                                                          ਸਰਗਰਮ_bp.reason);
        } ਨਹੀਂਤਾਂ {
            single_ਕਦਮ_info = test_And_Reset_Single_ਕਦਮ_Exception(ct);
            ਜੇ (single_ਕਦਮ_info.ret == true) {
                /* If the caller asked us to skip over N instructions before
                 * generating the next single-step breakpoint, we shouldn't
                 * bother to construct a fault message until we've skipped N
                 * instructions.
                 */
                ਜੇ (single_ਕਦਮ_Fault_Counter_Ready(ct) == false) {
                    ਵਾਪਸ EXCEPTION_NONE;
                }
                current_fault = seL4_Fault_DebugException_new(single_ਕਦਮ_info.instr_vaddr,
                                                              0, se_L4_Single_ਕਦਮ);
            } ਨਹੀਂਤਾਂ {
                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }
        }
    }

    handleFault(ਗੰਢ_STATE(ksCurThread));

    ਸਮਾਂ_ਸਾਰਣੀ();
    activateThread();

    ਵਾਪਸ EXCEPTION_NONE;
}

BOOT_ਕੋਡ bool_t Arch_initHardwareBreakpoints(ਖਾਲੀ)
{
    x86_cpu_ਪਛਾਣ_ਤੱਤ_t *modelinfo;

    modelinfo = x86_cpuid_get_model_info();
    /* Intel manuals, vol3, section 17.2.4, "NOTES". */
    ਜੇ (modelinfo->family == 15) {
        ਜੇ (modelinfo->model == 3 || modelinfo->model == 4
            || modelinfo->model == 6) {
            byte8_bps_supported = true;
        }
    }
    ਜੇ (modelinfo->family == 6) {
        ਜੇ (modelinfo->model == 15 || modelinfo->model == 23
            || modelinfo->model == 0x1C) {
            byte8_bps_supported = true;
        }
    }
    ਵਾਪਸ true;
}

ਖਾਲੀ Arch_initBreakpointContext(user_breakpoint_state_t *uds)
{
    memset(uds, 0, ਆਕਾਰ(*uds));

    /* Preload reserved values into register context */
    uds->dr[4] = readDr6Reg() &
                 ~(ਦੋਆਧਾਰੀ_ਅੰਕ(0)
                   | ਦੋਆਧਾਰੀ_ਅੰਕ(1)
                   | ਦੋਆਧਾਰੀ_ਅੰਕ(2)
                   | ਦੋਆਧਾਰੀ_ਅੰਕ(3)
                   | X86_DEBUG_DR6_SINGLE_ਕਦਮ_FLAG);

    uds->dr[5] = readDr7Reg() &
                 ~(X86_DEBUG_BP0_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ | X86_DEBUG_BP1_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ
                   | X86_DEBUG_BP2_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ
                   | X86_DEBUG_BP3_ENABLE_ਦੋਆਧਾਰੀ_ਅੰਕ);
}

#ਜੇ_ਅੰਤ
