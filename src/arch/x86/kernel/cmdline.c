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
#define MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
نویسه cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
ثابت نویسه cmdline_str_max_رقم_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
ثابت نویسه cmdline_str_رقم_sh_frames[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
ثابت نویسه cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

ایستا صحیح is_space(نویسه c)
{
    بازگشت c <= ' ';
}

ایستا صحیح UNUSED parse_opt(ثابت نویسه *cmdline, ثابت نویسه *opt, نویسه *مقدار, صحیح bufsize)
{
    صحیح len = -1;
    ثابت نویسه *optptr = پوچ;

    وقتی (true) {
        برای (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        اگر (*cmdline == 0) {
            بشکن;
        }

        برای (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        اگر (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            برای (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                مقدار[len] = *cmdline;
            }
            اگر (bufsize) {
                مقدار[len] = '\0';
            }
        }
        برای (; !is_space(*cmdline); cmdline++);
    }

    بازگشت len;
}

ایستا صحیح parse_bool(ثابت نویسه *cmdline, ثابت نویسه *opt)
{
    ثابت نویسه *optptr = پوچ;

    وقتی (1) {
        برای (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        اگر (*cmdline == 0) {
            بازگشت 0;
        }

        برای (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        اگر (*optptr == '\0' && is_space(*cmdline)) {
            بازگشت 1;
        } وگرنه {
            برای (; !is_space(*cmdline); cmdline++);
        }
    }
}

ایستا تهی UNUSED parse_uint16_array(نویسه *str, uint16_t *array, صحیح array_size)
{
    نویسه *last;
    صحیح   i = 0;
    صحیح   v;

    وقتی (str && i < array_size) {
        برای (last = str; *str && *str != ','; str++);
        اگر (*str == 0) {
            str = 0;
        } وگرنه {
            *str = 0;
            str++;
        }
        v = str_to_long(last);
        اگر (v == -1) {
            array[i] = 0;
        } وگرنه {
            array[i] = v;
        }
        i++;
    }
}

تهی cmdline_parse(ثابت نویسه *cmdline, cmdline_opt_t *cmdline_opt)
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
#pragma GCC diagnostic افزودن
#pragma GCC diagnostic ignored "-Warray-bounds"

    ثابت unsigned short *bda_port = (unsigned short *)0x400;
    ثابت unsigned short *bda_equi = (unsigned short *)0x410;
    صحیح ثابت bda_ports_شمار       = (*bda_equi >> 9) & 0x7;
#endif

#ifdef CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_port = bda_ports_شمار && *bda_port ? *bda_port : 0x3f8;

    اگر (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_port, 1);
    }

    /* initialise console ports to enable debug output */
    اگر (cmdline_opt->console_port) {
        serial_init(cmdline_opt->console_port);
        x86KSconsolePort = cmdline_opt->console_port;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    printf("\nBoot config: parsing cmdline '%s'\n", cmdline);

    اگر (cmdline_opt->console_port) {
        printf("Boot config: console_port = 0x%x\n", cmdline_opt->console_port);
    }
#endif

#اگر defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_port = bda_ports_شمار && *bda_port ? *bda_port : 0x3f8;
    اگر (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_port, 1);
    }

    /* initialise debug ports */
    اگر (cmdline_opt->debug_port) {
        serial_init(cmdline_opt->debug_port);
        x86KSdebugPort = cmdline_opt->debug_port;
        printf("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_port);
    }
#pragma GCC diagnostic برداشت
#endif

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    printf("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
