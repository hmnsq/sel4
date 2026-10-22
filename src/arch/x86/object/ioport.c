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

ساكن مضمن فراغ apply_الگو(کلمه_t_may_alias *w, کلمه_t الگو, bool_t مجموعه)
{
    إذا (مجموعه) {
        *w |= الگو;
    } وإلا {
        *w &= ~الگو;
    }
}

ساكن مضمن کلمه_t make_الگو(صحيح شروع, صحيح انتها)
{
    // number of bits we want to have set
    صحيح رقم_بیتها = انتها - شروع;
    // shift down to cut off the bits we don't want, then shift up to put the
    // bits into position
    أرجع (~(کلمه_t)0) >> (CONFIG_کلمه_اندازه - رقم_بیتها) << شروع;
}

ساكن exception_t ensure_پورت_Operation_Allowed(cap_t cap, uint32_t شروع_پورت, uint32_t اندازه)
{
    uint32_t اولی_allowed = cap_io_پورت_cap_get_cap_IOPort_اولی_پورت(cap);
    uint32_t آخرین_allowed = cap_io_پورت_cap_get_cap_IOPort_آخرین_پورت(cap);
    uint32_t انتها_پورت = شروع_پورت + اندازه - 1;
    أكد(اولی_allowed <= آخرین_allowed);
    أكد(شروع_پورت <= انتها_پورت);

    إذا ((شروع_پورت < اولی_allowed) || (انتها_پورت > آخرین_allowed)) {
        userError("IOPort: Ports %d--%d fall outside permitted range %d--%d.",
                  (صحيح)شروع_پورت, (صحيح)انتها_پورت,
                  (صحيح)اولی_allowed, (صحيح)آخرین_allowed);
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

فراغ freeIOPortRange(uint16_t اولی_پورت, uint16_t آخرین_پورت)
{
    مجموعه_IOPort_ماسک(x86KSAllocatedIOPorts, اولی_پورت, آخرین_پورت, false);
}

ساكن bool_t isIOPortRangeFree(uint16_t اولی_پورت, uint16_t آخرین_پورت)
{
    صحيح کم_کلمه = اولی_پورت >> کلمه_Radix;
    صحيح بالا_کلمه = آخرین_پورت >> کلمه_Radix;
    صحيح کم_شاخص = اولی_پورت & ماسک(کلمه_Radix);
    صحيح بالا_شاخص = آخرین_پورت & ماسک(کلمه_Radix);

    // check if we are operating on a partial word
    إذا (کم_کلمه == بالا_کلمه) {
        إذا ((x86KSAllocatedIOPorts[کم_کلمه] & make_الگو(کم_شاخص, بالا_شاخص + 1)) != 0) {
            أرجع false;
        }
        أرجع true;
    }
    // check the starting word
    إذا ((x86KSAllocatedIOPorts[کم_کلمه] & make_الگو(کم_شاخص, CONFIG_کلمه_اندازه)) != 0) {
        أرجع false;
    }
    کم_کلمه++;
    // check the rest of the whole words
    طالما (کم_کلمه < بالا_کلمه) {
        إذا (x86KSAllocatedIOPorts[کم_کلمه] != 0) {
            أرجع false;
        }
        کم_کلمه++;
    }
    // check any trailing bits
    إذا ((x86KSAllocatedIOPorts[کم_کلمه] & make_الگو(0, بالا_شاخص + 1)) != 0) {
        أرجع false;
    }
    أرجع true;
}

ساكن exception_t invoke_X86_پورت_Control(uint16_t اولی_پورت, uint16_t آخرین_پورت, cte_t *ioport_جایگاه, cte_t *control_جایگاه)
{
    مجموعه_IOPort_ماسک(x86KSAllocatedIOPorts, اولی_پورت, آخرین_پورت, true);
    cte_درج(cap_io_پورت_cap_new(اولی_پورت, آخرین_پورت
#اگر_تعریف_شده CONFIG_VTX
                                  , VPID_INVALID
#پایان_اگر
                                 ),
              control_جایگاه, ioport_جایگاه);

    أرجع EXCEPTION_NONE;
}

exception_t decode_X86_پورت_Control_Invocation(
    کلمه_t invLabel,
    کلمه_t درازی,
    cptr_t cptr,
    cte_t *جایگاه,
    cap_t cap,
    کلمه_t *میانگیر
)
{
    uint16_t اولی_پورت;
    uint16_t آخرین_پورت;
    کلمه_t شاخص, عمق;
    cap_t cnodeCap;
    cte_t *dest_جایگاه;
    lookup_جایگاه_ret_t lu_ret;
    exception_t وضعیت;

    إذا (invLabel != X86IOPortControlIssue) {
        userError("IOPortControl: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (درازی < 4 || current_extra_caps.excaprefs[0] == عدم) {
        userError("IOPortControl: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    اولی_پورت = getSyscallArg(0, میانگیر) & 0xffff;
    آخرین_پورت = getSyscallArg(1, میانگیر) & 0xffff;
    شاخص = getSyscallArg(2, میانگیر);
    عمق = getSyscallArg(3, میانگیر);

    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    إذا (آخرین_پورت < اولی_پورت) {
        userError("IOPortControl: Last port must be > first port.");
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!isIOPortRangeFree(اولی_پورت, آخرین_پورت)) {
        userError("IOPortControl: Some ports in range already in use.");
        current_syscall_error.type = se_L4_Revoke_اولی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_هدف_جایگاه(cnodeCap, شاخص, عمق);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap invalid: cap %lu.", getExtraCPtr(میانگیر, 0));
        أرجع lu_ret.وضعیت;
    }
    dest_جایگاه = lu_ret.جایگاه;

    وضعیت = ensure_خالی_جایگاه(dest_جایگاه);
    إذا (وضعیت != EXCEPTION_NONE) {
        userError("Target slot for new IO Port cap not empty: cap %lu.", getExtraCPtr(میانگیر, 0));
        أرجع وضعیت;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invoke_X86_پورت_Control(اولی_پورت, آخرین_پورت, dest_جایگاه, جایگاه);
}

ساكن exception_t invoke_X86_پورت_درون(کلمه_t invLabel, uint16_t پورت, bool_t call)
{
    uint32_t res;
    tcb_t *thread;
    thread = گره_STATE(ksCurThread);

    بدل (invLabel) {
    حالة X86IOPortIn8:
        res = in8(پورت);
        اكسر;
    حالة X86IOPortIn16:
        res = in16(پورت);
        اكسر;
    حالة X86IOPortIn32:
        res = in32(پورت);
        اكسر;
    }

    إذا (call) {
        کلمه_t *ipc_میانگیر = lookupIPCBuffer(true, thread);
        مجموعه_ثبات(thread, badge_ثبات, 0);
        بیعلامت صحيح درازی = مجموعه_MR(thread, ipc_میانگیر, 0, res);
        مجموعه_ثبات(thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                        se_L4_پیام_Info_new(0, 0, 0, درازی)));
    }
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

ساكن exception_t invoke_X86_پورت_بیرون(کلمه_t invLabel, uint16_t پورت, uint32_t داده)
{
    بدل (invLabel) {
    حالة X86IOPortOut8:
        out8(پورت, داده);
        اكسر;
    حالة X86IOPortOut16:
        out16(پورت, داده);
        اكسر;
    حالة X86IOPortOut32:
        out32(پورت, داده);
        اكسر;
    }

    أرجع EXCEPTION_NONE;
}

exception_t decode_X86_پورت_Invocation(
    کلمه_t invLabel,
    کلمه_t درازی,
    cptr_t cptr,
    cte_t *جایگاه,
    cap_t cap,
    bool_t call,
    کلمه_t *میانگیر
)
{
    exception_t ret = EXCEPTION_NONE;

    إذا (invLabel == X86IOPortIn8 || invLabel == X86IOPortIn16 || invLabel == X86IOPortIn32) {
        إذا (درازی < 1) {
            userError("IOPort: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to read from. */
        uint16_t پورت = getSyscallArg(0, میانگیر) & 0xffff;
        بدل (invLabel) {
        حالة X86IOPortIn8:
            ret = ensure_پورت_Operation_Allowed(cap, پورت, 1);
            اكسر;
        حالة X86IOPortIn16:
            ret = ensure_پورت_Operation_Allowed(cap, پورت, 2);
            اكسر;
        حالة X86IOPortIn32:
            ret = ensure_پورت_Operation_Allowed(cap, پورت, 4);
            اكسر;
        }
        إذا (ret != EXCEPTION_NONE) {
            أرجع ret;
        }
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع invoke_X86_پورت_درون(invLabel, پورت, call);
    } وإلا إذا (invLabel == X86IOPortOut8 || invLabel == X86IOPortOut16 || invLabel == X86IOPortOut32) {
        /* Ensure the incoming message is long enough for the write. */
        إذا (درازی < 2) {
            userError("IOPort Out: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        /* Get the port the user is trying to write to. */
        uint16_t پورت = getSyscallArg(0, میانگیر) & 0xffff;
        se_L4_کلمه raw_داده = getSyscallArg(1, میانگیر);
        /* We construct the value for data from raw_data based on the actual size of the port
           operation. This ensures that there is no 'random' user data left over in the value
           passed to invokeX86PortOut. Whilst invokeX86PortOut will ignore any extra data and
           cast down to the correct word size removing the extra here is currently relied upon
           for verification */
        uint32_t داده = 0;

        بدل (invLabel) {
        حالة X86IOPortOut8:
            ret = ensure_پورت_Operation_Allowed(cap, پورت, 1);
            داده = raw_داده & 0xff;
            اكسر;
        حالة X86IOPortOut16:
            ret = ensure_پورت_Operation_Allowed(cap, پورت, 2);
            داده = raw_داده & 0xffff;
            اكسر;
        حالة X86IOPortOut32:
            ret = ensure_پورت_Operation_Allowed(cap, پورت, 4);
            داده = raw_داده & 0xffffffff;
            اكسر;
        }
        إذا (ret != EXCEPTION_NONE) {
            أرجع ret;
        }
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع invoke_X86_پورت_بیرون(invLabel, پورت, داده);
    } وإلا {
        userError("IOPort: Unknown operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

فراغ مجموعه_IOPort_ماسک(فراغ *ioport_bitmap, uint16_t کم, uint16_t بالا, bool_t مجموعه)
{
    //get an aliasing pointer
    کلمه_t_may_alias *bitmap = ioport_bitmap;

    صحيح کم_کلمه = کم >> کلمه_Radix;
    صحيح بالا_کلمه = بالا >> کلمه_Radix;
    صحيح کم_شاخص = کم & ماسک(کلمه_Radix);
    صحيح بالا_شاخص = بالا & ماسک(کلمه_Radix);

    // see if we are just manipulating bits inside a single word. handling this
    // specially makes reasoning easier
    إذا (کم_کلمه == بالا_کلمه) {
        apply_الگو(bitmap + کم_کلمه, make_الگو(کم_شاخص, بالا_شاخص + 1), مجموعه);
    } وإلا {
        // operate on the potentially partial first word
        apply_الگو(bitmap + کم_کلمه, make_الگو(کم_شاخص, CONFIG_کلمه_اندازه), مجموعه);
        کم_کلمه++;
        // iterate over the whole words
        طالما (کم_کلمه < بالا_کلمه) {
            apply_الگو(bitmap + کم_کلمه, ~(کلمه_t)0, مجموعه);
            کم_کلمه++;
        }
        // apply to any remaining bits
        apply_الگو(bitmap + کم_کلمه, make_الگو(0, بالا_شاخص + 1), مجموعه);
    }
}
