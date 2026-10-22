/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <kernel/thread.h>
#ضمن <api/failures.h>
#ضمن <api/syscall.h>
#ضمن <machine/io.h>
#ضمن <arch/object/ioport.h>
#ضمن <arch/api/invocation.h>
#ضمن <plat/machine/io.h>

ساكن مضمن فراغ apply_نمونه(کلمه_t_may_alias *w, کلمه_t نمونه, bool_t سېټ)
{
    إذا (سېټ) {
        *w |= نمونه;
    } وإلا {
        *w &= ~نمونه;
    }
}

ساكن مضمن کلمه_t make_نمونه(صحيح پیل, صحيح پای_ټکی)
{
    // number of bits we want to have set
    صحيح عدد_بټونه = پای_ټکی - پیل;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    أرجع (~(کلمه_t)0) >> (CONFIG_کلمه_اندازه - عدد_بټونه) << پیل;
}

ساكن exception_t ensure_پورټ_Operation_Allowed(cap_t cap, uint32_t پیل_پورټ, uint32_t اندازه)
{
    uint32_t لومړی_allowed = cap_io_پورټ_cap_get_cap_IOPort_لومړی_پورټ(cap);
    uint32_t وروستی_allowed = cap_io_پورټ_cap_get_cap_IOPort_وروستی_پورټ(cap);
    uint32_t پای_ټکی_پورټ = پیل_پورټ + اندازه - 1;
    أكد(لومړی_allowed <= وروستی_allowed);
    أكد(پیل_پورټ <= پای_ټکی_پورټ);

    إذا ((پیل_پورټ < لومړی_allowed) || (پای_ټکی_پورټ > وروستی_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (صحيح)پیل_پورټ, (صحيح)پای_ټکی_پورټ,
                  (صحيح)لومړی_allowed, (صحيح)وروستی_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

فراغ freeIOPortRange(uint16_t لومړی_پورټ, uint16_t وروستی_پورټ)
{
    سېټ_IOPort_ماسک(x86KSAllocatedIOPorts, لومړی_پورټ, وروستی_پورټ, false);
}

ساكن bool_t isIOPortRangeFree(uint16_t لومړی_پورټ, uint16_t وروستی_پورټ)
{
    صحيح ټیټ_کلمه = لومړی_پورټ >> کلمه_Radix;
    صحيح لوړ_کلمه = وروستی_پورټ >> کلمه_Radix;
    صحيح ټیټ_شاخص = لومړی_پورټ & ماسک(کلمه_Radix);
    صحيح لوړ_شاخص = وروستی_پورټ & ماسک(کلمه_Radix);

    // check if we are operating on a partial word
    إذا (ټیټ_کلمه == لوړ_کلمه) {
        إذا ((x86KSAllocatedIOPorts[ټیټ_کلمه] & make_نمونه(ټیټ_شاخص, لوړ_شاخص + 1)) != 0) {
            أرجع false;
        }
        أرجع true;
    }
    // check the starting word
    إذا ((x86KSAllocatedIOPorts[ټیټ_کلمه] & make_نمونه(ټیټ_شاخص, CONFIG_کلمه_اندازه)) != 0) {
        أرجع false;
    }
    ټیټ_کلمه++;
    // check the rest of the whole words
    طالما (ټیټ_کلمه < لوړ_کلمه) {
        إذا (x86KSAllocatedIOPorts[ټیټ_کلمه] != 0) {
            أرجع false;
        }
        ټیټ_کلمه++;
    }
    // check any trailing bits
    إذا ((x86KSAllocatedIOPorts[ټیټ_کلمه] & make_نمونه(0, لوړ_شاخص + 1)) != 0) {
        أرجع false;
    }
    أرجع true;
}

ساكن exception_t invoke_X86_پورټ_Control(uint16_t لومړی_پورټ, uint16_t وروستی_پورټ, cte_t *ioport_خونه, cte_t *control_خونه)
{
    سېټ_IOPort_ماسک(x86KSAllocatedIOPorts, لومړی_پورټ, وروستی_پورټ, true);
    cte_ورننباسه(cap_io_پورټ_cap_new(لومړی_پورټ, وروستی_پورټ
#که_تعریف_شوی CONFIG_VTX
                                  , VPID_INVALID
#د_که_پای
                                 ),
              control_خونه, ioport_خونه);

    أرجع EXCEPTION_NONE;
}

exception_t decode_X86_پورټ_Control_Invocation(
    کلمه_t invLabel,
    کلمه_t اوږدتیا,
    cptr_t cptr,
    cte_t *خونه,
    cap_t cap,
    کلمه_t *بفر
)
{
    uint16_t لومړی_پورټ;
    uint16_t وروستی_پورټ;
    کلمه_t شاخص, ژوروالی;
    cap_t cnodeCap;
    cte_t *dest_خونه;
    lookup_خونه_ret_t lu_ret;
    exception_t حالت;

    إذا (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (اوږدتیا < 4 || current_extra_caps.excaprefs[0] == عدم) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    لومړی_پورټ = getSyscallArg(0, بفر) & 0xffff;
    وروستی_پورټ = getSyscallArg(1, بفر) & 0xffff;
    شاخص = getSyscallArg(2, بفر);
    ژوروالی = getSyscallArg(3, بفر);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    إذا (وروستی_پورټ < لومړی_پورټ) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!isIOPortRangeFree(لومړی_پورټ, وروستی_پورټ)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = se_L4_Revoke_لومړی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_موخه_خونه(cnodeCap, شاخص, ژوروالی);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(بفر, 0));
        أرجع lu_ret.حالت;
    }
    dest_خونه = lu_ret.خونه;

    حالت = ensure_تشه_خونه(dest_خونه);
    إذا (حالت != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(بفر, 0));
        أرجع حالت;
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_X86_پورټ_Control(لومړی_پورټ, وروستی_پورټ, dest_خونه, خونه);
}

ساكن exception_t invoke_X86_پورټ_دننه(کلمه_t invLabel, uint16_t پورټ, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = غوټه_STATE(ksCurThread);

    بدل (invLabel) {
    حالة X86IOPortIn8:
        res = in8(پورټ);
        اكسر;
    حالة X86IOPortIn16:
        res = in16(پورټ);
        اكسر;
    حالة X86IOPortIn32:
        res = in32(پورټ);
        اكسر;
    }

    إذا (call) {
        کلمه_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سېټ_ثبتوونکی(thread, badge_ثبتوونکی, 0);
        بینښه صحيح اوږدتیا = سېټ_MR(thread, ipc_بفر, 0, res);
        سېټ_ثبتوونکی(thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, اوږدتیا)));
    }
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t invoke_X86_پورټ_بهر(کلمه_t invLabel, uint16_t پورټ, uint32_t معلومات)
{
    بدل (invLabel) {
    حالة X86IOPortOut8:
        out8(پورټ, معلومات);
        اكسر;
    حالة X86IOPortOut16:
        out16(پورټ, معلومات);
        اكسر;
    حالة X86IOPortOut32:
        out32(پورټ, معلومات);
        اكسر;
    }

    أرجع EXCEPTION_NONE;
}

exception_t decode_X86_پورټ_Invocation(
    کلمه_t invLabel,
    کلمه_t اوږدتیا,
    cptr_t cptr,
    cte_t *خونه,
    cap_t cap,
    bool_t call,
    کلمه_t *بفر
)
{
    exception_t ret = EXCEPTION_NONE;

    إذا (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        إذا (اوږدتیا < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t پورټ = getSyscallArg(0, بفر) & 0xffff;
        بدل (invLabel) {
        حالة X86IOPortIn8:
            ret = ensure_پورټ_Operation_Allowed(cap, پورټ, 1);
            اكسر;
        حالة X86IOPortIn16:
            ret = ensure_پورټ_Operation_Allowed(cap, پورټ, 2);
            اكسر;
        حالة X86IOPortIn32:
            ret = ensure_پورټ_Operation_Allowed(cap, پورټ, 4);
            اكسر;
        }
        إذا (ret != EXCEPTION_NONE) {
            أرجع ret;
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع invoke_X86_پورټ_دننه(invLabel, پورټ, call);
    } وإلا إذا (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        إذا (اوږدتیا < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to write to. */
        uint16_t پورټ = getSyscallArg(0, بفر) & 0xffff;
        se_L4_کلمه raw_معلومات = getSyscallArg(1, بفر);
        /* We construct the value for data from raw_data based on the actual size of the port
           operation. This ensures that there is no 'random' user data left over in the value
           passed to invokeX86PortOut. Whilst invokeX86PortOut will ignore any extra data and
           cast down to the correct word size removing the extra here is currently relied upon
           for verification */
        uint32_t معلومات = 0;

        بدل (invLabel) {
        حالة X86IOPortOut8:
            ret = ensure_پورټ_Operation_Allowed(cap, پورټ, 1);
            معلومات = raw_معلومات & 0xff;
            اكسر;
        حالة X86IOPortOut16:
            ret = ensure_پورټ_Operation_Allowed(cap, پورټ, 2);
            معلومات = raw_معلومات & 0xffff;
            اكسر;
        حالة X86IOPortOut32:
            ret = ensure_پورټ_Operation_Allowed(cap, پورټ, 4);
            معلومات = raw_معلومات & 0xffffffff;
            اكسر;
        }
        إذا (ret != EXCEPTION_NONE) {
            أرجع ret;
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع invoke_X86_پورټ_بهر(invLabel, پورټ, معلومات);
    } وإلا {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

فراغ سېټ_IOPort_ماسک(فراغ *ioport_bitmap, uint16_t ټیټ, uint16_t لوړ, bool_t سېټ)
{
    //get an aliasing pointer
    کلمه_t_may_alias *bitmap = ioport_bitmap;

    صحيح ټیټ_کلمه = ټیټ >> کلمه_Radix;
    صحيح لوړ_کلمه = لوړ >> کلمه_Radix;
    صحيح ټیټ_شاخص = ټیټ & ماسک(کلمه_Radix);
    صحيح لوړ_شاخص = لوړ & ماسک(کلمه_Radix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    إذا (ټیټ_کلمه == لوړ_کلمه) {
        apply_نمونه(bitmap + ټیټ_کلمه, make_نمونه(ټیټ_شاخص, لوړ_شاخص + 1), سېټ);
    } وإلا {
        // operate on the potentially partial first word
        apply_نمونه(bitmap + ټیټ_کلمه, make_نمونه(ټیټ_شاخص, CONFIG_کلمه_اندازه), سېټ);
        ټیټ_کلمه++;
        // iterate over the whole words
        طالما (ټیټ_کلمه < لوړ_کلمه) {
            apply_نمونه(bitmap + ټیټ_کلمه, ~(کلمه_t)0, سېټ);
            ټیټ_کلمه++;
        }
        // apply to any remaining bits
        apply_نمونه(bitmap + ټیټ_کلمه, make_نمونه(0, لوړ_شاخص + 1), سېټ);
    }
}
