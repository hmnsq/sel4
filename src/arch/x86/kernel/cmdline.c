/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <util.h>
#ضمن <machine/io.h>
#ضمن <arch/kernel/cmdline.h>
#ضمن <arch/kernel/boot_sys.h>
#ضمن <linker.h>
#ضمن <plat/machine/io.h>

/* 'cmdline_val' is declared globally because of a C-subset restriction.
 * It is only used in cmdline_parse(), which therefore is non-reentrant.
 */
#تعریف_کن MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
حرف cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
ثابت حرف cmdline_str_max_رقم_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
ثابت حرف cmdline_str_رقم_sh_قابها[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
ثابت حرف cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

ساكن صحيح is_space(حرف c)
{
    أرجع c <= ' ';
}

ساكن صحيح استفادهنشده parse_opt(ثابت حرف *cmdline, ثابت حرف *opt, حرف *مقدار, صحيح bufsize)
{
    صحيح len = -1;
    ثابت حرف *optptr = عدم;

    طالما (true) {
        لكل (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        إذا (*cmdline == 0) {
            اكسر;
        }

        لكل (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        إذا (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            لكل (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                مقدار[len] = *cmdline;
            }
            إذا (bufsize) {
                مقدار[len] = '\0';
            }
        }
        لكل (; !is_space(*cmdline); cmdline++);
    }

    أرجع len;
}

ساكن صحيح parse_bool(ثابت حرف *cmdline, ثابت حرف *opt)
{
    ثابت حرف *optptr = عدم;

    طالما (1) {
        لكل (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        إذا (*cmdline == 0) {
            أرجع 0;
        }

        لكل (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        إذا (*optptr == '\0' && is_space(*cmdline)) {
            أرجع 1;
        } وإلا {
            لكل (; !is_space(*cmdline); cmdline++);
        }
    }
}

ساكن فراغ استفادهنشده parse_uint16_array(حرف *str, uint16_t *array, صحيح array_اندازه)
{
    حرف *آخرین;
    صحيح   i = 0;
    صحيح   v;

    طالما (str && i < array_اندازه) {
        لكل (آخرین = str; *str && *str != ','; str++);
        إذا (*str == 0) {
            str = 0;
        } وإلا {
            *str = 0;
            str++;
        }
        v = str_مقصد_long(آخرین);
        إذا (v == -1) {
            array[i] = 0;
        } وإلا {
            array[i] = v;
        }
        i++;
    }
}

فراغ cmdline_parse(ثابت حرف *cmdline, cmdline_opt_t *cmdline_opt)
{
#إذا defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* use BIOS data area to read serial configuration. The BDA is not
     * fully standardized and parts are absolete. See http://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
     * for an explanation
     */
    /*
     * gcc-12 gives array bounds warning (treated as errors by the
     * build system) for any address below 4096.  Turn that off for
     * accessing the BIOS area.
     */
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic ignored "-Warray-bounds"

    ثابت بیعلامت کوتاه *bda_پورت = (بیعلامت کوتاه *)0x400;
    ثابت بیعلامت کوتاه *bda_equi = (بیعلامت کوتاه *)0x410;
    صحيح ثابت bda_ports_شمار       = (*bda_equi >> 9) & 0x7;
#پایان_اگر

#اگر_تعریف_شده CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_پورت = bda_ports_شمار && *bda_پورت ? *bda_پورت : 0x3f8;

    إذا (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_پورت, 1);
    }

    /* initialise console ports to enable debug output */
    إذا (cmdline_opt->console_پورت) {
        serial_init(cmdline_opt->console_پورت);
        x86_KSconsole_پورت = cmdline_opt->console_پورت;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    چاپ_کن("\nBoot config: parsing cmdline '%s'\n", cmdline);

    إذا (cmdline_opt->console_پورت) {
        چاپ_کن("Boot config: console_port = 0x%x\n", cmdline_opt->console_پورت);
    }
#پایان_اگر

#إذا defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_پورت = bda_ports_شمار && *bda_پورت ? *bda_پورت : 0x3f8;
    إذا (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_پورت, 1);
    }

    /* initialise debug ports */
    إذا (cmdline_opt->debug_پورت) {
        serial_init(cmdline_opt->debug_پورت);
        x86_KSdebug_پورت = cmdline_opt->debug_پورت;
        چاپ_کن("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_پورت);
    }
#رهنمود GCC diagnostic برداشت
#پایان_اگر

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    چاپ_کن("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
