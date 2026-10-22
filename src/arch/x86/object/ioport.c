/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <kernel/thread.h>
#شامل <api/failures.h>
#شامل <api/syscall.h>
#شامل <machine/io.h>
#شامل <arch/object/ioport.h>
#شامل <arch/api/invocation.h>
#شامل <plat/machine/io.h>

ساکن اندرونی خالی apply_نمونہ(لفظ_t_may_alias *w, لفظ_t نمونہ, bool_t سیٹ)
{
    اگر (سیٹ) {
        *w |= نمونہ;
    } ورنہ {
        *w &= ~نمونہ;
    }
}

ساکن اندرونی لفظ_t make_نمونہ(صحیح آغاز, صحیح انتہا)
{
    // number of bits we want to have set
    صحیح عدد_بٹ = انتہا - آغاز;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    واپس (~(لفظ_t)0) >> (CONFIG_لفظ_سائز - عدد_بٹ) << آغاز;
}

ساکن exception_t ensure_پورٹ_Operation_Allowed(cap_t cap, uint32_t آغاز_پورٹ, uint32_t سائز)
{
    uint32_t پہلا_allowed = cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(cap);
    uint32_t آخری_allowed = cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(cap);
    uint32_t انتہا_پورٹ = آغاز_پورٹ + سائز - 1;
    أكد(پہلا_allowed <= آخری_allowed);
    أكد(آغاز_پورٹ <= انتہا_پورٹ);

    اگر ((آغاز_پورٹ < پہلا_allowed) || (انتہا_پورٹ > آخری_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (صحیح)آغاز_پورٹ, (صحیح)انتہا_پورٹ,
                  (صحیح)پہلا_allowed, (صحیح)آخری_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    واپس EXCEPTION_NONE;
}

خالی freeIOPortRange(uint16_t پہلا_پورٹ, uint16_t آخری_پورٹ)
{
    سیٹ_IOPort_نقاب(x86KSAllocatedIOPorts, پہلا_پورٹ, آخری_پورٹ, false);
}

ساکن bool_t isIOPortRangeFree(uint16_t پہلا_پورٹ, uint16_t آخری_پورٹ)
{
    صحیح کم_لفظ = پہلا_پورٹ >> لفظ_Radix;
    صحیح بلند_لفظ = آخری_پورٹ >> لفظ_Radix;
    صحیح کم_اشاریہ = پہلا_پورٹ & نقاب(لفظ_Radix);
    صحیح بلند_اشاریہ = آخری_پورٹ & نقاب(لفظ_Radix);

    // check if we are operating on a partial word
    اگر (کم_لفظ == بلند_لفظ) {
        اگر ((x86KSAllocatedIOPorts[کم_لفظ] & make_نمونہ(کم_اشاریہ, بلند_اشاریہ + 1)) != 0) {
            واپس false;
        }
        واپس true;
    }
    // check the starting word
    اگر ((x86KSAllocatedIOPorts[کم_لفظ] & make_نمونہ(کم_اشاریہ, CONFIG_لفظ_سائز)) != 0) {
        واپس false;
    }
    کم_لفظ++;
    // check the rest of the whole words
    جب (کم_لفظ < بلند_لفظ) {
        اگر (x86KSAllocatedIOPorts[کم_لفظ] != 0) {
            واپس false;
        }
        کم_لفظ++;
    }
    // check any trailing bits
    اگر ((x86KSAllocatedIOPorts[کم_لفظ] & make_نمونہ(0, بلند_اشاریہ + 1)) != 0) {
        واپس false;
    }
    واپس true;
}

ساکن exception_t invoke_X86_پورٹ_Control(uint16_t پہلا_پورٹ, uint16_t آخری_پورٹ, cte_t *ioport_خانہ, cte_t *control_خانہ)
{
    سیٹ_IOPort_نقاب(x86KSAllocatedIOPorts, پہلا_پورٹ, آخری_پورٹ, true);
    cte_داخل_کرو(cap_io_پورٹ_cap_new(پہلا_پورٹ, آخری_پورٹ
#اگر_تعریف_شدہ CONFIG_VTX
                                  , VPID_INVALID
#اگر_ختم
                                 ),
              control_خانہ, ioport_خانہ);

    واپس EXCEPTION_NONE;
}

exception_t decode_X86_پورٹ_Control_Invocation(
    لفظ_t invLabel,
    لفظ_t لمبائی,
    cptr_t cptr,
    cte_t *خانہ,
    cap_t cap,
    لفظ_t *بفر
)
{
    uint16_t پہلا_پورٹ;
    uint16_t آخری_پورٹ;
    لفظ_t اشاریہ, گہرائی;
    cap_t cnodeCap;
    cte_t *dest_خانہ;
    lookup_خانہ_ret_t lu_ret;
    exception_t حالت_2;

    اگر (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (لمبائی < 4 || current_extra_caps.excaprefs[0] == تہی) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    پہلا_پورٹ = getSyscallArg(0, بفر) & 0xffff;
    آخری_پورٹ = getSyscallArg(1, بفر) & 0xffff;
    اشاریہ = getSyscallArg(2, بفر);
    گہرائی = getSyscallArg(3, بفر);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    اگر (آخری_پورٹ < پہلا_پورٹ) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (!isIOPortRangeFree(پہلا_پورٹ, آخری_پورٹ)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = se_L4_Revoke_پہلا;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_ہدف_خانہ(cnodeCap, اشاریہ, گہرائی);
    اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(بفر, 0));
        واپس lu_ret.حالت_2;
    }
    dest_خانہ = lu_ret.خانہ;

    حالت_2 = ensure_خالی_خانے_خانہ(dest_خانہ);
    اگر (حالت_2 != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(بفر, 0));
        واپس حالت_2;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invoke_X86_پورٹ_Control(پہلا_پورٹ, آخری_پورٹ, dest_خانہ, خانہ);
}

ساکن exception_t invoke_X86_پورٹ_اندر(لفظ_t invLabel, uint16_t پورٹ, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = گرہ_STATE(ksCurThread);

    انتخاب (invLabel) {
    حالت X86IOPortIn8:
        res = in8(پورٹ);
        توڑو;
    حالت X86IOPortIn16:
        res = in16(پورٹ);
        توڑو;
    حالت X86IOPortIn32:
        res = in32(پورٹ);
        توڑو;
    }

    اگر (call) {
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
        غیرعلامتی صحیح لمبائی = سیٹ_MR(thread, ipc_بفر, 0, res);
        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, لمبائی)));
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    واپس EXCEPTION_NONE;
}

ساکن exception_t invoke_X86_پورٹ_برآمد(لفظ_t invLabel, uint16_t پورٹ, uint32_t ڈیٹا)
{
    انتخاب (invLabel) {
    حالت X86IOPortOut8:
        out8(پورٹ, ڈیٹا);
        توڑو;
    حالت X86IOPortOut16:
        out16(پورٹ, ڈیٹا);
        توڑو;
    حالت X86IOPortOut32:
        out32(پورٹ, ڈیٹا);
        توڑو;
    }

    واپس EXCEPTION_NONE;
}

exception_t decode_X86_پورٹ_Invocation(
    لفظ_t invLabel,
    لفظ_t لمبائی,
    cptr_t cptr,
    cte_t *خانہ,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    exception_t ret = EXCEPTION_NONE;

    اگر (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        اگر (لمبائی < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t پورٹ = getSyscallArg(0, بفر) & 0xffff;
        انتخاب (invLabel) {
        حالت X86IOPortIn8:
            ret = ensure_پورٹ_Operation_Allowed(cap, پورٹ, 1);
            توڑو;
        حالت X86IOPortIn16:
            ret = ensure_پورٹ_Operation_Allowed(cap, پورٹ, 2);
            توڑو;
        حالت X86IOPortIn32:
            ret = ensure_پورٹ_Operation_Allowed(cap, پورٹ, 4);
            توڑو;
        }
        اگر (ret != EXCEPTION_NONE) {
            واپس ret;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس invoke_X86_پورٹ_اندر(invLabel, پورٹ, call);
    } ورنہ اگر (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        اگر (لمبائی < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to write to. */
        uint16_t پورٹ = getSyscallArg(0, بفر) & 0xffff;
        se_L4_لفظ raw_ڈیٹا = getSyscallArg(1, بفر);
        /* We construct the value for data from raw_data based on the actual size of the port
           operation. This ensures that there is no 'random' user data left over in the value
           passed to invokeX86PortOut. Whilst invokeX86PortOut will ignore any extra data and
           cast down to the correct word size removing the extra here is currently relied upon
           for verification */
        uint32_t ڈیٹا = 0;

        انتخاب (invLabel) {
        حالت X86IOPortOut8:
            ret = ensure_پورٹ_Operation_Allowed(cap, پورٹ, 1);
            ڈیٹا = raw_ڈیٹا & 0xff;
            توڑو;
        حالت X86IOPortOut16:
            ret = ensure_پورٹ_Operation_Allowed(cap, پورٹ, 2);
            ڈیٹا = raw_ڈیٹا & 0xffff;
            توڑو;
        حالت X86IOPortOut32:
            ret = ensure_پورٹ_Operation_Allowed(cap, پورٹ, 4);
            ڈیٹا = raw_ڈیٹا & 0xffffffff;
            توڑو;
        }
        اگر (ret != EXCEPTION_NONE) {
            واپس ret;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس invoke_X86_پورٹ_برآمد(invLabel, پورٹ, ڈیٹا);
    } ورنہ {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
}

خالی سیٹ_IOPort_نقاب(خالی *ioport_bitmap, uint16_t کم, uint16_t بلند, bool_t سیٹ)
{
    //get an aliasing pointer
    لفظ_t_may_alias *bitmap = ioport_bitmap;

    صحیح کم_لفظ = کم >> لفظ_Radix;
    صحیح بلند_لفظ = بلند >> لفظ_Radix;
    صحیح کم_اشاریہ = کم & نقاب(لفظ_Radix);
    صحیح بلند_اشاریہ = بلند & نقاب(لفظ_Radix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    اگر (کم_لفظ == بلند_لفظ) {
        apply_نمونہ(bitmap + کم_لفظ, make_نمونہ(کم_اشاریہ, بلند_اشاریہ + 1), سیٹ);
    } ورنہ {
        // operate on the potentially partial first word
        apply_نمونہ(bitmap + کم_لفظ, make_نمونہ(کم_اشاریہ, CONFIG_لفظ_سائز), سیٹ);
        کم_لفظ++;
        // iterate over the whole words
        جب (کم_لفظ < بلند_لفظ) {
            apply_نمونہ(bitmap + کم_لفظ, ~(لفظ_t)0, سیٹ);
            کم_لفظ++;
        }
        // apply to any remaining bits
        apply_نمونہ(bitmap + کم_لفظ, make_نمونہ(0, بلند_اشاریہ + 1), سیٹ);
    }
}
