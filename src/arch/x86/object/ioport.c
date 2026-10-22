/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/ioport.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/api/invocation.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/io.h>

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ apply_ਨਮੂਨਾ(ਸ਼ਬਦ_t_may_alias *w, ਸ਼ਬਦ_t ਨਮੂਨਾ, bool_t ਸੈੱਟ)
{
    ਜੇ (ਸੈੱਟ) {
        *w |= ਨਮੂਨਾ;
    } ਨਹੀਂਤਾਂ {
        *w &= ~ਨਮੂਨਾ;
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਸ਼ਬਦ_t make_ਨਮੂਨਾ(ਪੂਰਨ ਸ਼ੁਰੂ, ਪੂਰਨ ਸਿਰਾ)
{
    // number of bits we want to have set
    ਪੂਰਨ ਅੰਕ_ਬਿੱਟ = ਸਿਰਾ - ਸ਼ੁਰੂ;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    ਵਾਪਸ (~(ਸ਼ਬਦ_t)0) >> (CONFIG_ਸ਼ਬਦ_ਮਾਪ - ਅੰਕ_ਬਿੱਟ) << ਸ਼ੁਰੂ;
}

ਸਥਿਰ exception_t ensure_ਪੋਰਟ_Operation_Allowed(cap_t cap, uint32_t ਸ਼ੁਰੂ_ਪੋਰਟ, uint32_t ਮਾਪ)
{
    uint32_t ਪਹਿਲਾ_allowed = cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਪਹਿਲਾ_ਪੋਰਟ(cap);
    uint32_t ਆਖਰੀ_allowed = cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਆਖਰੀ_ਪੋਰਟ(cap);
    uint32_t ਸਿਰਾ_ਪੋਰਟ = ਸ਼ੁਰੂ_ਪੋਰਟ + ਮਾਪ - 1;
    ਪ੍ਰਤਿਜ੍ਞਾ(ਪਹਿਲਾ_allowed <= ਆਖਰੀ_allowed);
    ਪ੍ਰਤਿਜ੍ਞਾ(ਸ਼ੁਰੂ_ਪੋਰਟ <= ਸਿਰਾ_ਪੋਰਟ);

    ਜੇ ((ਸ਼ੁਰੂ_ਪੋਰਟ < ਪਹਿਲਾ_allowed) || (ਸਿਰਾ_ਪੋਰਟ > ਆਖਰੀ_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (ਪੂਰਨ)ਸ਼ੁਰੂ_ਪੋਰਟ, (ਪੂਰਨ)ਸਿਰਾ_ਪੋਰਟ,
                  (ਪੂਰਨ)ਪਹਿਲਾ_allowed, (ਪੂਰਨ)ਆਖਰੀ_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

ਖਾਲੀ freeIOPortRange(uint16_t ਪਹਿਲਾ_ਪੋਰਟ, uint16_t ਆਖਰੀ_ਪੋਰਟ)
{
    ਸੈੱਟ_IOPort_ਨਕਾਬ(x86KSAllocatedIOPorts, ਪਹਿਲਾ_ਪੋਰਟ, ਆਖਰੀ_ਪੋਰਟ, false);
}

ਸਥਿਰ bool_t isIOPortRangeFree(uint16_t ਪਹਿਲਾ_ਪੋਰਟ, uint16_t ਆਖਰੀ_ਪੋਰਟ)
{
    ਪੂਰਨ ਨੀਵਾਂ_ਸ਼ਬਦ = ਪਹਿਲਾ_ਪੋਰਟ >> ਸ਼ਬਦ_Radix;
    ਪੂਰਨ ਉੱਚਾ_ਸ਼ਬਦ = ਆਖਰੀ_ਪੋਰਟ >> ਸ਼ਬਦ_Radix;
    ਪੂਰਨ ਨੀਵਾਂ_ਸੂਚਕਾਂਕ = ਪਹਿਲਾ_ਪੋਰਟ & ਨਕਾਬ(ਸ਼ਬਦ_Radix);
    ਪੂਰਨ ਉੱਚਾ_ਸੂਚਕਾਂਕ = ਆਖਰੀ_ਪੋਰਟ & ਨਕਾਬ(ਸ਼ਬਦ_Radix);

    // check if we are operating on a partial word
    ਜੇ (ਨੀਵਾਂ_ਸ਼ਬਦ == ਉੱਚਾ_ਸ਼ਬਦ) {
        ਜੇ ((x86KSAllocatedIOPorts[ਨੀਵਾਂ_ਸ਼ਬਦ] & make_ਨਮੂਨਾ(ਨੀਵਾਂ_ਸੂਚਕਾਂਕ, ਉੱਚਾ_ਸੂਚਕਾਂਕ + 1)) != 0) {
            ਵਾਪਸ false;
        }
        ਵਾਪਸ true;
    }
    // check the starting word
    ਜੇ ((x86KSAllocatedIOPorts[ਨੀਵਾਂ_ਸ਼ਬਦ] & make_ਨਮੂਨਾ(ਨੀਵਾਂ_ਸੂਚਕਾਂਕ, CONFIG_ਸ਼ਬਦ_ਮਾਪ)) != 0) {
        ਵਾਪਸ false;
    }
    ਨੀਵਾਂ_ਸ਼ਬਦ++;
    // check the rest of the whole words
    ਜਦੋਂ (ਨੀਵਾਂ_ਸ਼ਬਦ < ਉੱਚਾ_ਸ਼ਬਦ) {
        ਜੇ (x86KSAllocatedIOPorts[ਨੀਵਾਂ_ਸ਼ਬਦ] != 0) {
            ਵਾਪਸ false;
        }
        ਨੀਵਾਂ_ਸ਼ਬਦ++;
    }
    // check any trailing bits
    ਜੇ ((x86KSAllocatedIOPorts[ਨੀਵਾਂ_ਸ਼ਬਦ] & make_ਨਮੂਨਾ(0, ਉੱਚਾ_ਸੂਚਕਾਂਕ + 1)) != 0) {
        ਵਾਪਸ false;
    }
    ਵਾਪਸ true;
}

ਸਥਿਰ exception_t invoke_X86_ਪੋਰਟ_Control(uint16_t ਪਹਿਲਾ_ਪੋਰਟ, uint16_t ਆਖਰੀ_ਪੋਰਟ, cte_t *ioport_ਥਾਂ, cte_t *control_ਥਾਂ)
{
    ਸੈੱਟ_IOPort_ਨਕਾਬ(x86KSAllocatedIOPorts, ਪਹਿਲਾ_ਪੋਰਟ, ਆਖਰੀ_ਪੋਰਟ, true);
    cte_ਪਾਓ(cap_io_ਪੋਰਟ_cap_new(ਪਹਿਲਾ_ਪੋਰਟ, ਆਖਰੀ_ਪੋਰਟ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
                                  , VPID_INVALID
#ਜੇ_ਅੰਤ
                                 ),
              control_ਥਾਂ, ioport_ਥਾਂ);

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decode_X86_ਪੋਰਟ_Control_Invocation(
    ਸ਼ਬਦ_t invLabel,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cptr_t cptr,
    cte_t *ਥਾਂ,
    cap_t cap,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    uint16_t ਪਹਿਲਾ_ਪੋਰਟ;
    uint16_t ਆਖਰੀ_ਪੋਰਟ;
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ;
    cap_t cnodeCap;
    cte_t *dest_ਥਾਂ;
    lookup_ਥਾਂ_ret_t lu_ret;
    exception_t ਸਥਿਤੀ;

    ਜੇ (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ਲੰਮਾਈ < 4 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਪਹਿਲਾ_ਪੋਰਟ = getSyscallArg(0, ਬਫਰ) & 0xffff;
    ਆਖਰੀ_ਪੋਰਟ = getSyscallArg(1, ਬਫਰ) & 0xffff;
    ਸੂਚਕਾਂਕ = getSyscallArg(2, ਬਫਰ);
    ਡੂੰਘਾਈ = getSyscallArg(3, ਬਫਰ);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (ਆਖਰੀ_ਪੋਰਟ < ਪਹਿਲਾ_ਪੋਰਟ) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (!isIOPortRangeFree(ਪਹਿਲਾ_ਪੋਰਟ, ਆਖਰੀ_ਪੋਰਟ)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(cnodeCap, ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(ਬਫਰ, 0));
        ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
    }
    dest_ਥਾਂ = lu_ret.ਥਾਂ;

    ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(dest_ਥਾਂ);
    ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(ਬਫਰ, 0));
        ਵਾਪਸ ਸਥਿਤੀ;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invoke_X86_ਪੋਰਟ_Control(ਪਹਿਲਾ_ਪੋਰਟ, ਆਖਰੀ_ਪੋਰਟ, dest_ਥਾਂ, ਥਾਂ);
}

ਸਥਿਰ exception_t invoke_X86_ਪੋਰਟ_ਵਿੱਚ(ਸ਼ਬਦ_t invLabel, uint16_t ਪੋਰਟ, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = ਗੰਢ_STATE(ksCurThread);

    ਚੋਣ (invLabel) {
    ਹਾਲਤ X86IOPortIn8:
        res = in8(ਪੋਰਟ);
        ਰੋਕੋ;
    ਹਾਲਤ X86IOPortIn16:
        res = in16(ਪੋਰਟ);
        ਰੋਕੋ;
    ਹਾਲਤ X86IOPortIn32:
        res = in32(ਪੋਰਟ);
        ਰੋਕੋ;
    }

    ਜੇ (call) {
        ਸ਼ਬਦ_t *ipc_ਬਫਰ = lookupIPCBuffer(true, thread);
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ = ਸੈੱਟ_MR(thread, ipc_ਬਫਰ, 0, res);
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                        se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, ਲੰਮਾਈ)));
    }
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Running);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t invoke_X86_ਪੋਰਟ_ਬਾਹਰਲਾ(ਸ਼ਬਦ_t invLabel, uint16_t ਪੋਰਟ, uint32_t ਅੰਕੜੇ)
{
    ਚੋਣ (invLabel) {
    ਹਾਲਤ X86IOPortOut8:
        out8(ਪੋਰਟ, ਅੰਕੜੇ);
        ਰੋਕੋ;
    ਹਾਲਤ X86IOPortOut16:
        out16(ਪੋਰਟ, ਅੰਕੜੇ);
        ਰੋਕੋ;
    ਹਾਲਤ X86IOPortOut32:
        out32(ਪੋਰਟ, ਅੰਕੜੇ);
        ਰੋਕੋ;
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decode_X86_ਪੋਰਟ_Invocation(
    ਸ਼ਬਦ_t invLabel,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cptr_t cptr,
    cte_t *ਥਾਂ,
    cap_t cap,
    bool_t call,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    exception_t ret = EXCEPTION_NONE;

    ਜੇ (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        ਜੇ (ਲੰਮਾਈ < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t ਪੋਰਟ = getSyscallArg(0, ਬਫਰ) & 0xffff;
        ਚੋਣ (invLabel) {
        ਹਾਲਤ X86IOPortIn8:
            ret = ensure_ਪੋਰਟ_Operation_Allowed(cap, ਪੋਰਟ, 1);
            ਰੋਕੋ;
        ਹਾਲਤ X86IOPortIn16:
            ret = ensure_ਪੋਰਟ_Operation_Allowed(cap, ਪੋਰਟ, 2);
            ਰੋਕੋ;
        ਹਾਲਤ X86IOPortIn32:
            ret = ensure_ਪੋਰਟ_Operation_Allowed(cap, ਪੋਰਟ, 4);
            ਰੋਕੋ;
        }
        ਜੇ (ret != EXCEPTION_NONE) {
            ਵਾਪਸ ret;
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ invoke_X86_ਪੋਰਟ_ਵਿੱਚ(invLabel, ਪੋਰਟ, call);
    } ਨਹੀਂਤਾਂ ਜੇ (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        ਜੇ (ਲੰਮਾਈ < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to write to. */
        uint16_t ਪੋਰਟ = getSyscallArg(0, ਬਫਰ) & 0xffff;
        se_L4_ਸ਼ਬਦ raw_ਅੰਕੜੇ = getSyscallArg(1, ਬਫਰ);
        /* We construct the value for data from raw_data based on the actual size of the port
           operation. This ensures that there is no 'random' user data left over in the value
           passed to invokeX86PortOut. Whilst invokeX86PortOut will ignore any extra data and
           cast down to the correct word size removing the extra here is currently relied upon
           for verification */
        uint32_t ਅੰਕੜੇ = 0;

        ਚੋਣ (invLabel) {
        ਹਾਲਤ X86IOPortOut8:
            ret = ensure_ਪੋਰਟ_Operation_Allowed(cap, ਪੋਰਟ, 1);
            ਅੰਕੜੇ = raw_ਅੰਕੜੇ & 0xff;
            ਰੋਕੋ;
        ਹਾਲਤ X86IOPortOut16:
            ret = ensure_ਪੋਰਟ_Operation_Allowed(cap, ਪੋਰਟ, 2);
            ਅੰਕੜੇ = raw_ਅੰਕੜੇ & 0xffff;
            ਰੋਕੋ;
        ਹਾਲਤ X86IOPortOut32:
            ret = ensure_ਪੋਰਟ_Operation_Allowed(cap, ਪੋਰਟ, 4);
            ਅੰਕੜੇ = raw_ਅੰਕੜੇ & 0xffffffff;
            ਰੋਕੋ;
        }
        ਜੇ (ret != EXCEPTION_NONE) {
            ਵਾਪਸ ret;
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ invoke_X86_ਪੋਰਟ_ਬਾਹਰਲਾ(invLabel, ਪੋਰਟ, ਅੰਕੜੇ);
    } ਨਹੀਂਤਾਂ {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}

ਖਾਲੀ ਸੈੱਟ_IOPort_ਨਕਾਬ(ਖਾਲੀ *ioport_bitmap, uint16_t ਨੀਵਾਂ, uint16_t ਉੱਚਾ, bool_t ਸੈੱਟ)
{
    //get an aliasing pointer
    ਸ਼ਬਦ_t_may_alias *bitmap = ioport_bitmap;

    ਪੂਰਨ ਨੀਵਾਂ_ਸ਼ਬਦ = ਨੀਵਾਂ >> ਸ਼ਬਦ_Radix;
    ਪੂਰਨ ਉੱਚਾ_ਸ਼ਬਦ = ਉੱਚਾ >> ਸ਼ਬਦ_Radix;
    ਪੂਰਨ ਨੀਵਾਂ_ਸੂਚਕਾਂਕ = ਨੀਵਾਂ & ਨਕਾਬ(ਸ਼ਬਦ_Radix);
    ਪੂਰਨ ਉੱਚਾ_ਸੂਚਕਾਂਕ = ਉੱਚਾ & ਨਕਾਬ(ਸ਼ਬਦ_Radix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    ਜੇ (ਨੀਵਾਂ_ਸ਼ਬਦ == ਉੱਚਾ_ਸ਼ਬਦ) {
        apply_ਨਮੂਨਾ(bitmap + ਨੀਵਾਂ_ਸ਼ਬਦ, make_ਨਮੂਨਾ(ਨੀਵਾਂ_ਸੂਚਕਾਂਕ, ਉੱਚਾ_ਸੂਚਕਾਂਕ + 1), ਸੈੱਟ);
    } ਨਹੀਂਤਾਂ {
        // operate on the potentially partial first word
        apply_ਨਮੂਨਾ(bitmap + ਨੀਵਾਂ_ਸ਼ਬਦ, make_ਨਮੂਨਾ(ਨੀਵਾਂ_ਸੂਚਕਾਂਕ, CONFIG_ਸ਼ਬਦ_ਮਾਪ), ਸੈੱਟ);
        ਨੀਵਾਂ_ਸ਼ਬਦ++;
        // iterate over the whole words
        ਜਦੋਂ (ਨੀਵਾਂ_ਸ਼ਬਦ < ਉੱਚਾ_ਸ਼ਬਦ) {
            apply_ਨਮੂਨਾ(bitmap + ਨੀਵਾਂ_ਸ਼ਬਦ, ~(ਸ਼ਬਦ_t)0, ਸੈੱਟ);
            ਨੀਵਾਂ_ਸ਼ਬਦ++;
        }
        // apply to any remaining bits
        apply_ਨਮੂਨਾ(bitmap + ਨੀਵਾਂ_ਸ਼ਬਦ, make_ਨਮੂਨਾ(0, ਉੱਚਾ_ਸੂਚਕਾਂਕ + 1), ਸੈੱਟ);
    }
}
