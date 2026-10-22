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
#تعریف_کړه MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
حرف cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
ثابت حرف cmdline_str_max_عدد_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
ثابت حرف cmdline_str_عدد_sh_چوکاټونه[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
ثابت حرف cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

ساكن صحيح is_space(حرف c)
{
    أرجع c <= ' ';
}

ساكن صحيح ناکارول_شوی parse_opt(ثابت حرف *cmdline, ثابت حرف *opt, حرف *ارزښت, صحيح bufsize)
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
                ارزښت[len] = *cmdline;
            }
            إذا (bufsize) {
                ارزښت[len] = '\0';
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

ساكن فراغ ناکارول_شوی parse_uint16_array(حرف *str, uint16_t *array, صحيح array_اندازه)
{
    حرف *وروستی;
    صحيح   i = 0;
    صحيح   v;

    طالما (str && i < array_اندازه) {
        لكل (وروستی = str; *str && *str != ','; str++);
        إذا (*str == 0) {
            str = 0;
        } وإلا {
            *str = 0;
            str++;
        }
        v = str_منزل_long(وروستی);
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
#لارښوونه GCC diagnostic ټېل
#لارښوونه GCC diagnostic ignored "-Warray-bounds"

    ثابت بینښه لنډ *bda_پورټ = (بینښه لنډ *)0x400;
    ثابت بینښه لنډ *bda_equi = (بینښه لنډ *)0x410;
    صحيح ثابت bda_ports_شمېر       = (*bda_equi >> 9) & 0x7;
#د_که_پای

#که_تعریف_شوی CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_پورټ = bda_ports_شمېر && *bda_پورټ ? *bda_پورټ : 0x3f8;

    إذا (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_پورټ, 1);
    }

    /* initialise console ports to enable debug output */
    إذا (cmdline_opt->console_پورټ) {
        serial_init(cmdline_opt->console_پورټ);
        x86_KSconsole_پورټ = cmdline_opt->console_پورټ;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    چاپ_کړه("\nBoot config: parsing cmdline '%s'\n", cmdline);

    إذا (cmdline_opt->console_پورټ) {
        چاپ_کړه("Boot config: console_port = 0x%x\n", cmdline_opt->console_پورټ);
    }
#د_که_پای

#إذا defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_پورټ = bda_ports_شمېر && *bda_پورټ ? *bda_پورټ : 0x3f8;
    إذا (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_پورټ, 1);
    }

    /* initialise debug ports */
    إذا (cmdline_opt->debug_پورټ) {
        serial_init(cmdline_opt->debug_پورټ);
        x86_KSdebug_پورټ = cmdline_opt->debug_پورټ;
        چاپ_کړه("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_پورټ);
    }
#لارښوونه GCC diagnostic راوباسه
#د_که_پای

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    چاپ_کړه("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
