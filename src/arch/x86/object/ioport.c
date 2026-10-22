/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <arch/object/ioport.h>
#ಅನ್ತರ್ಭಾವ <arch/api/invocation.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/io.h>

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ apply_ನಮೂನೆ(ಶಬ್ದ_t_may_alias *w, ಶಬ್ದ_t ನಮೂನೆ, bool_t ಗಣ)
{
    ಆದರೆ (ಗಣ) {
        *w |= ನಮೂನೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        *w &= ~ನಮೂನೆ;
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶಬ್ದ_t make_ನಮೂನೆ(ಪೂರ್ಣಾಂಕ ಆರಂಭ, ಪೂರ್ಣಾಂಕ ಕೊನೆ)
{
    // number of bits we want to have set
    ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಬಿಟ್ಟುಗಳು = ಕೊನೆ - ಆರಂಭ;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    ಹಿಂತಿರುಗಿ (~(ಶಬ್ದ_t)0) >> (CONFIG_ಶಬ್ದ_ಗಾತ್ರ - ಅಂಕೆ_ಬಿಟ್ಟುಗಳು) << ಆರಂಭ;
}

ಸ್ಥಿರ exception_t ensure_ಪೋರ್ಟ್_Operation_Allowed(cap_t cap, uint32_t ಆರಂಭ_ಪೋರ್ಟ್, uint32_t ಗಾತ್ರ_2)
{
    uint32_t ಮೊದಲ_allowed = cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಮೊದಲ_ಪೋರ್ಟ್(cap);
    uint32_t ಕೊನೆಯ_allowed = cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಕೊನೆಯ_ಪೋರ್ಟ್(cap);
    uint32_t ಕೊನೆ_ಪೋರ್ಟ್ = ಆರಂಭ_ಪೋರ್ಟ್ + ಗಾತ್ರ_2 - 1;
    ಪ್ರತಿಜ್ಞಾ(ಮೊದಲ_allowed <= ಕೊನೆಯ_allowed);
    ಪ್ರತಿಜ್ಞಾ(ಆರಂಭ_ಪೋರ್ಟ್ <= ಕೊನೆ_ಪೋರ್ಟ್);

    ಆದರೆ ((ಆರಂಭ_ಪೋರ್ಟ್ < ಮೊದಲ_allowed) || (ಕೊನೆ_ಪೋರ್ಟ್ > ಕೊನೆಯ_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (ಪೂರ್ಣಾಂಕ)ಆರಂಭ_ಪೋರ್ಟ್, (ಪೂರ್ಣಾಂಕ)ಕೊನೆ_ಪೋರ್ಟ್,
                  (ಪೂರ್ಣಾಂಕ)ಮೊದಲ_allowed, (ಪೂರ್ಣಾಂಕ)ಕೊನೆಯ_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಶೂನ್ಯ freeIOPortRange(uint16_t ಮೊದಲ_ಪೋರ್ಟ್, uint16_t ಕೊನೆಯ_ಪೋರ್ಟ್)
{
    ಗಣ_IOPort_ಮುಖವಾಡ(x86KSAllocatedIOPorts, ಮೊದಲ_ಪೋರ್ಟ್, ಕೊನೆಯ_ಪೋರ್ಟ್, false);
}

ಸ್ಥಿರ bool_t isIOPortRangeFree(uint16_t ಮೊದಲ_ಪೋರ್ಟ್, uint16_t ಕೊನೆಯ_ಪೋರ್ಟ್)
{
    ಪೂರ್ಣಾಂಕ ಕೆಳಗಿನ_ಶಬ್ದ = ಮೊದಲ_ಪೋರ್ಟ್ >> ಶಬ್ದ_Radix;
    ಪೂರ್ಣಾಂಕ ಮೇಲಿನ_ಶಬ್ದ = ಕೊನೆಯ_ಪೋರ್ಟ್ >> ಶಬ್ದ_Radix;
    ಪೂರ್ಣಾಂಕ ಕೆಳಗಿನ_ಸೂಚ್ಯಂಕ = ಮೊದಲ_ಪೋರ್ಟ್ & ಮುಖವಾಡ(ಶಬ್ದ_Radix);
    ಪೂರ್ಣಾಂಕ ಮೇಲಿನ_ಸೂಚ್ಯಂಕ = ಕೊನೆಯ_ಪೋರ್ಟ್ & ಮುಖವಾಡ(ಶಬ್ದ_Radix);

    // check if we are operating on a partial word
    ಆದರೆ (ಕೆಳಗಿನ_ಶಬ್ದ == ಮೇಲಿನ_ಶಬ್ದ) {
        ಆದರೆ ((x86KSAllocatedIOPorts[ಕೆಳಗಿನ_ಶಬ್ದ] & make_ನಮೂನೆ(ಕೆಳಗಿನ_ಸೂಚ್ಯಂಕ, ಮೇಲಿನ_ಸೂಚ್ಯಂಕ + 1)) != 0) {
            ಹಿಂತಿರುಗಿ false;
        }
        ಹಿಂತಿರುಗಿ true;
    }
    // check the starting word
    ಆದರೆ ((x86KSAllocatedIOPorts[ಕೆಳಗಿನ_ಶಬ್ದ] & make_ನಮೂನೆ(ಕೆಳಗಿನ_ಸೂಚ್ಯಂಕ, CONFIG_ಶಬ್ದ_ಗಾತ್ರ)) != 0) {
        ಹಿಂತಿರುಗಿ false;
    }
    ಕೆಳಗಿನ_ಶಬ್ದ++;
    // check the rest of the whole words
    ಆಗ (ಕೆಳಗಿನ_ಶಬ್ದ < ಮೇಲಿನ_ಶಬ್ದ) {
        ಆದರೆ (x86KSAllocatedIOPorts[ಕೆಳಗಿನ_ಶಬ್ದ] != 0) {
            ಹಿಂತಿರುಗಿ false;
        }
        ಕೆಳಗಿನ_ಶಬ್ದ++;
    }
    // check any trailing bits
    ಆದರೆ ((x86KSAllocatedIOPorts[ಕೆಳಗಿನ_ಶಬ್ದ] & make_ನಮೂನೆ(0, ಮೇಲಿನ_ಸೂಚ್ಯಂಕ + 1)) != 0) {
        ಹಿಂತಿರುಗಿ false;
    }
    ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ exception_t invoke_X86_ಪೋರ್ಟ್_Control(uint16_t ಮೊದಲ_ಪೋರ್ಟ್, uint16_t ಕೊನೆಯ_ಪೋರ್ಟ್, cte_t *ioport_ಸ್ಥಾನ, cte_t *control_ಸ್ಥಾನ)
{
    ಗಣ_IOPort_ಮುಖವಾಡ(x86KSAllocatedIOPorts, ಮೊದಲ_ಪೋರ್ಟ್, ಕೊನೆಯ_ಪೋರ್ಟ್, true);
    cte_ಸೇರಿಸು(cap_io_ಪೋರ್ಟ್_cap_new(ಮೊದಲ_ಪೋರ್ಟ್, ಕೊನೆಯ_ಪೋರ್ಟ್
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
                                  , VPID_INVALID
#ಆದರೆ_ಅಂತ್ಯ
                                 ),
              control_ಸ್ಥಾನ, ioport_ಸ್ಥಾನ);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decode_X86_ಪೋರ್ಟ್_Control_Invocation(
    ಶಬ್ದ_t invLabel,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cptr_t cptr,
    cte_t *ಸ್ಥಾನ,
    cap_t cap,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    uint16_t ಮೊದಲ_ಪೋರ್ಟ್;
    uint16_t ಕೊನೆಯ_ಪೋರ್ಟ್;
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ, ಆಳ;
    cap_t cnodeCap;
    cte_t *dest_ಸ್ಥಾನ;
    lookup_ಸ್ಥಾನ_ret_t lu_ret;
    exception_t ಸ್ಥಿತಿ_2;

    ಆದರೆ (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ಉದ್ದಳತೆ < 4 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಮೊದಲ_ಪೋರ್ಟ್ = getSyscallArg(0, ಬಫರ್) & 0xffff;
    ಕೊನೆಯ_ಪೋರ್ಟ್ = getSyscallArg(1, ಬಫರ್) & 0xffff;
    ಸೂಚ್ಯಂಕ = getSyscallArg(2, ಬಫರ್);
    ಆಳ = getSyscallArg(3, ಬಫರ್);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (ಕೊನೆಯ_ಪೋರ್ಟ್ < ಮೊದಲ_ಪೋರ್ಟ್) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (!isIOPortRangeFree(ಮೊದಲ_ಪೋರ್ಟ್, ಕೊನೆಯ_ಪೋರ್ಟ್)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_ಗುರಿ_ಸ್ಥಾನ(cnodeCap, ಸೂಚ್ಯಂಕ, ಆಳ);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(ಬಫರ್, 0));
        ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
    }
    dest_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;

    ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(dest_ಸ್ಥಾನ);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(ಬಫರ್, 0));
        ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invoke_X86_ಪೋರ್ಟ್_Control(ಮೊದಲ_ಪೋರ್ಟ್, ಕೊನೆಯ_ಪೋರ್ಟ್, dest_ಸ್ಥಾನ, ಸ್ಥಾನ);
}

ಸ್ಥಿರ exception_t invoke_X86_ಪೋರ್ಟ್_ಒಳ(ಶಬ್ದ_t invLabel, uint16_t ಪೋರ್ಟ್, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);

    ಆಯ್ಕೆ (invLabel) {
    ಸ್ಥಿತಿ X86IOPortIn8:
        res = in8(ಪೋರ್ಟ್);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ X86IOPortIn16:
        res = in16(ಪೋರ್ಟ್);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ X86IOPortIn32:
        res = in32(ಪೋರ್ಟ್);
        ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (call) {
        ಶಬ್ದ_t *ipc_ಬಫರ್ = lookupIPCBuffer(true, thread);
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ = ಗಣ_MR(thread, ipc_ಬಫರ್, 0, res);
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                        se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, ಉದ್ದಳತೆ)));
    }
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t invoke_X86_ಪೋರ್ಟ್_ಹೊರಹೋಗು(ಶಬ್ದ_t invLabel, uint16_t ಪೋರ್ಟ್, uint32_t ದತ್ತಾಂಶ)
{
    ಆಯ್ಕೆ (invLabel) {
    ಸ್ಥಿತಿ X86IOPortOut8:
        out8(ಪೋರ್ಟ್, ದತ್ತಾಂಶ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ X86IOPortOut16:
        out16(ಪೋರ್ಟ್, ದತ್ತಾಂಶ);
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ X86IOPortOut32:
        out32(ಪೋರ್ಟ್, ದತ್ತಾಂಶ);
        ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decode_X86_ಪೋರ್ಟ್_Invocation(
    ಶಬ್ದ_t invLabel,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cptr_t cptr,
    cte_t *ಸ್ಥಾನ,
    cap_t cap,
    bool_t call,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    exception_t ret = EXCEPTION_NONE;

    ಆದರೆ (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        ಆದರೆ (ಉದ್ದಳತೆ < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t ಪೋರ್ಟ್ = getSyscallArg(0, ಬಫರ್) & 0xffff;
        ಆಯ್ಕೆ (invLabel) {
        ಸ್ಥಿತಿ X86IOPortIn8:
            ret = ensure_ಪೋರ್ಟ್_Operation_Allowed(cap, ಪೋರ್ಟ್, 1);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ X86IOPortIn16:
            ret = ensure_ಪೋರ್ಟ್_Operation_Allowed(cap, ಪೋರ್ಟ್, 2);
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ X86IOPortIn32:
            ret = ensure_ಪೋರ್ಟ್_Operation_Allowed(cap, ಪೋರ್ಟ್, 4);
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ (ret != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ ret;
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ invoke_X86_ಪೋರ್ಟ್_ಒಳ(invLabel, ಪೋರ್ಟ್, call);
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        ಆದರೆ (ಉದ್ದಳತೆ < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to write to. */
        uint16_t ಪೋರ್ಟ್ = getSyscallArg(0, ಬಫರ್) & 0xffff;
        se_L4_ಶಬ್ದ raw_ದತ್ತಾಂಶ = getSyscallArg(1, ಬಫರ್);
        /* We construct the value for data from raw_data based on the actual size of the port
           operation. This ensures that there is no 'random' user data left over in the value
           passed to invokeX86PortOut. Whilst invokeX86PortOut will ignore any extra data and
           cast down to the correct word size removing the extra here is currently relied upon
           for verification */
        uint32_t ದತ್ತಾಂಶ = 0;

        ಆಯ್ಕೆ (invLabel) {
        ಸ್ಥಿತಿ X86IOPortOut8:
            ret = ensure_ಪೋರ್ಟ್_Operation_Allowed(cap, ಪೋರ್ಟ್, 1);
            ದತ್ತಾಂಶ = raw_ದತ್ತಾಂಶ & 0xff;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ X86IOPortOut16:
            ret = ensure_ಪೋರ್ಟ್_Operation_Allowed(cap, ಪೋರ್ಟ್, 2);
            ದತ್ತಾಂಶ = raw_ದತ್ತಾಂಶ & 0xffff;
            ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ X86IOPortOut32:
            ret = ensure_ಪೋರ್ಟ್_Operation_Allowed(cap, ಪೋರ್ಟ್, 4);
            ದತ್ತಾಂಶ = raw_ದತ್ತಾಂಶ & 0xffffffff;
            ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ (ret != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ ret;
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ invoke_X86_ಪೋರ್ಟ್_ಹೊರಹೋಗು(invLabel, ಪೋರ್ಟ್, ದತ್ತಾಂಶ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}

ಶೂನ್ಯ ಗಣ_IOPort_ಮುಖವಾಡ(ಶೂನ್ಯ *ioport_bitmap, uint16_t ಕೆಳಗಿನ, uint16_t ಮೇಲಿನ, bool_t ಗಣ)
{
    //get an aliasing pointer
    ಶಬ್ದ_t_may_alias *bitmap = ioport_bitmap;

    ಪೂರ್ಣಾಂಕ ಕೆಳಗಿನ_ಶಬ್ದ = ಕೆಳಗಿನ >> ಶಬ್ದ_Radix;
    ಪೂರ್ಣಾಂಕ ಮೇಲಿನ_ಶಬ್ದ = ಮೇಲಿನ >> ಶಬ್ದ_Radix;
    ಪೂರ್ಣಾಂಕ ಕೆಳಗಿನ_ಸೂಚ್ಯಂಕ = ಕೆಳಗಿನ & ಮುಖವಾಡ(ಶಬ್ದ_Radix);
    ಪೂರ್ಣಾಂಕ ಮೇಲಿನ_ಸೂಚ್ಯಂಕ = ಮೇಲಿನ & ಮುಖವಾಡ(ಶಬ್ದ_Radix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    ಆದರೆ (ಕೆಳಗಿನ_ಶಬ್ದ == ಮೇಲಿನ_ಶಬ್ದ) {
        apply_ನಮೂನೆ(bitmap + ಕೆಳಗಿನ_ಶಬ್ದ, make_ನಮೂನೆ(ಕೆಳಗಿನ_ಸೂಚ್ಯಂಕ, ಮೇಲಿನ_ಸೂಚ್ಯಂಕ + 1), ಗಣ);
    } ಇಲ್ಲದಿದ್ದರೆ {
        // operate on the potentially partial first word
        apply_ನಮೂನೆ(bitmap + ಕೆಳಗಿನ_ಶಬ್ದ, make_ನಮೂನೆ(ಕೆಳಗಿನ_ಸೂಚ್ಯಂಕ, CONFIG_ಶಬ್ದ_ಗಾತ್ರ), ಗಣ);
        ಕೆಳಗಿನ_ಶಬ್ದ++;
        // iterate over the whole words
        ಆಗ (ಕೆಳಗಿನ_ಶಬ್ದ < ಮೇಲಿನ_ಶಬ್ದ) {
            apply_ನಮೂನೆ(bitmap + ಕೆಳಗಿನ_ಶಬ್ದ, ~(ಶಬ್ದ_t)0, ಗಣ);
            ಕೆಳಗಿನ_ಶಬ್ದ++;
        }
        // apply to any remaining bits
        apply_ನಮೂನೆ(bitmap + ಕೆಳಗಿನ_ಶಬ್ದ, make_ನಮೂನೆ(0, ಮೇಲಿನ_ಸೂಚ್ಯಂಕ + 1), ಗಣ);
    }
}
