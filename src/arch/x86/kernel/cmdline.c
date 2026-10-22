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
#define MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
حرف cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
ثابت حرف cmdline_str_max_num_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
ثابت حرف cmdline_str_num_sh_frames[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
ثابت حرف cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

ساكن صحيح is_space(حرف c)
{
    החזר c <= ' ';
}

ساكن صحيح UNUSED parse_opt(ثابت حرف *cmdline, ثابت حرف *opt, حرف *value, صحيح bufsize)
{
    صحيح len = -1;
    ثابت حرف *optptr = عدم;

    כלעוד (true) {
        עבור (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        אם (*cmdline == 0) {
            اكسر;
        }

        עבור (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        אם (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            עבור (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                value[len] = *cmdline;
            }
            אם (bufsize) {
                value[len] = '\0';
            }
        }
        עבור (; !is_space(*cmdline); cmdline++);
    }

    החזר len;
}

ساكن صحيح parse_bool(ثابت حرف *cmdline, ثابت حرف *opt)
{
    ثابت حرف *optptr = عدم;

    כלעוד (1) {
        עבור (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        אם (*cmdline == 0) {
            החזר 0;
        }

        עבור (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        אם (*optptr == '\0' && is_space(*cmdline)) {
            החזר 1;
        } אחרת {
            עבור (; !is_space(*cmdline); cmdline++);
        }
    }
}

ساكن فراغ UNUSED parse_uint16_array(حرف *str, uint16_t *array, صحيح array_size)
{
    حرف *last;
    صحيح   i = 0;
    صحيح   v;

    כלעוד (str && i < array_size) {
        עבור (last = str; *str && *str != ','; str++);
        אם (*str == 0) {
            str = 0;
        } אחרת {
            *str = 0;
            str++;
        }
        v = str_to_long(last);
        אם (v == -1) {
            array[i] = 0;
        } אחרת {
            array[i] = v;
        }
        i++;
    }
}

فراغ cmdline_parse(ثابت حرف *cmdline, cmdline_opt_t *cmdline_opt)
{
#אם defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* use BIOS data area to read serial configuration. The BDA is not
     * fully standardized and parts are absolete. See http://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
     * for an explanation
     */
    /*
     * gcc-12 gives array bounds warning (treated as errors by the
     * build system) for any address below 4096.  Turn that off for
     * accessing the BIOS area.
     */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"

    ثابت unsigned short *bda_port = (unsigned short *)0x400;
    ثابت unsigned short *bda_equi = (unsigned short *)0x410;
    صحيح ثابت bda_ports_count       = (*bda_equi >> 9) & 0x7;
#endif

#ifdef CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_port = bda_ports_count && *bda_port ? *bda_port : 0x3f8;

    אם (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_port, 1);
    }

    /* initialise console ports to enable debug output */
    אם (cmdline_opt->console_port) {
        serial_init(cmdline_opt->console_port);
        x86KSconsolePort = cmdline_opt->console_port;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    printf("\nBoot config: parsing cmdline '%s'\n", cmdline);

    אם (cmdline_opt->console_port) {
        printf("Boot config: console_port = 0x%x\n", cmdline_opt->console_port);
    }
#endif

#אם defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_port = bda_ports_count && *bda_port ? *bda_port : 0x3f8;
    אם (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_port, 1);
    }

    /* initialise debug ports */
    אם (cmdline_opt->debug_port) {
        serial_init(cmdline_opt->debug_port);
        x86KSdebugPort = cmdline_opt->debug_port;
        printf("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_port);
    }
#pragma GCC diagnostic pop
#endif

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    printf("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
