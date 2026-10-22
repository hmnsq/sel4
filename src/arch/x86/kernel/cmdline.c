/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <util.h>
#شامل <machine/io.h>
#شامل <arch/kernel/cmdline.h>
#شامل <arch/kernel/boot_sys.h>
#شامل <linker.h>
#شامل <plat/machine/io.h>

/* 'cmdline_val' is declared globally because of a C-subset restriction.
 * It is only used in cmdline_parse(), which therefore is non-reentrant.
 */
#تعریف_کرو MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
حرف cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
ثابت حرف cmdline_str_max_عدد_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
ثابت حرف cmdline_str_عدد_sh_چوکھٹے[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
ثابت حرف cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

ساکن صحیح is_space(حرف c)
{
    واپس c <= ' ';
}

ساکن صحیح غیر_مستعمل parse_opt(ثابت حرف *cmdline, ثابت حرف *opt, حرف *قدر, صحیح bufsize)
{
    صحیح len = -1;
    ثابت حرف *optptr = تہی;

    جب (true) {
        برائے (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        اگر (*cmdline == 0) {
            توڑو;
        }

        برائے (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        اگر (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            برائے (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                قدر[len] = *cmdline;
            }
            اگر (bufsize) {
                قدر[len] = '\0';
            }
        }
        برائے (; !is_space(*cmdline); cmdline++);
    }

    واپس len;
}

ساکن صحیح parse_bool(ثابت حرف *cmdline, ثابت حرف *opt)
{
    ثابت حرف *optptr = تہی;

    جب (1) {
        برائے (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        اگر (*cmdline == 0) {
            واپس 0;
        }

        برائے (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        اگر (*optptr == '\0' && is_space(*cmdline)) {
            واپس 1;
        } ورنہ {
            برائے (; !is_space(*cmdline); cmdline++);
        }
    }
}

ساکن خالی غیر_مستعمل parse_uint16_array(حرف *str, uint16_t *array, صحیح array_سائز)
{
    حرف *آخری;
    صحیح   i = 0;
    صحیح   v;

    جب (str && i < array_سائز) {
        برائے (آخری = str; *str && *str != ','; str++);
        اگر (*str == 0) {
            str = 0;
        } ورنہ {
            *str = 0;
            str++;
        }
        v = str_مقصد_long(آخری);
        اگر (v == -1) {
            array[i] = 0;
        } ورنہ {
            array[i] = v;
        }
        i++;
    }
}

خالی cmdline_parse(ثابت حرف *cmdline, cmdline_opt_t *cmdline_opt)
{
#اگر defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* use BIOS data area to read serial configuration. The BDA is not
     * fully standardized and parts are absolete. See http://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
     * for an explanation
     */
    /*
     * gcc-12 gives array bounds warning (treated as errors by the
     * build system) for any address below 4096.  Turn that off for
     * accessing the BIOS area.
     */
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Warray-bounds"

    ثابت غیرعلامتی مختصر *bda_پورٹ = (غیرعلامتی مختصر *)0x400;
    ثابت غیرعلامتی مختصر *bda_equi = (غیرعلامتی مختصر *)0x410;
    صحیح ثابت bda_ports_گنتی       = (*bda_equi >> 9) & 0x7;
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_پورٹ = bda_ports_گنتی && *bda_پورٹ ? *bda_پورٹ : 0x3f8;

    اگر (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_پورٹ, 1);
    }

    /* initialise console ports to enable debug output */
    اگر (cmdline_opt->console_پورٹ) {
        serial_init(cmdline_opt->console_پورٹ);
        x86_KSconsole_پورٹ = cmdline_opt->console_پورٹ;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    لکھو("\nBoot config: parsing cmdline '%s'\n", cmdline);

    اگر (cmdline_opt->console_پورٹ) {
        لکھو("Boot config: console_port = 0x%x\n", cmdline_opt->console_پورٹ);
    }
#اگر_ختم

#اگر defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_پورٹ = bda_ports_گنتی && *bda_پورٹ ? *bda_پورٹ : 0x3f8;
    اگر (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_پورٹ, 1);
    }

    /* initialise debug ports */
    اگر (cmdline_opt->debug_پورٹ) {
        serial_init(cmdline_opt->debug_پورٹ);
        x86_KSdebug_پورٹ = cmdline_opt->debug_پورٹ;
        لکھو("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_پورٹ);
    }
#ہدایت GCC diagnostic نکالو
#اگر_ختم

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    لکھو("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
