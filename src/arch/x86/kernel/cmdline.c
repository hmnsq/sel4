/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <util.h>
#include <machine/io.h>
#include <arch/kernel/cmdline.h>
#include <arch/kernel/boot_sys.h>
#include <linker.h>
#include <plat/machine/io.h>

/* 'cmdline_val' is declared globally because of a C-subset restriction.
 * It is only used in cmdline_parse(), which therefore is non-reentrant.
 */
#define MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
var_na cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
const var_na cmdline_str_max_num_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
const var_na cmdline_str_num_sh_frames[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
const var_na cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

static puur_naa_nka is_space(var_na c)
{
    ပွၵ်ႈ c <= ' ';
}

static puur_naa_nka UNUSED parse_opt(const var_na *cmdline, const var_na *opt, var_na *value, puur_naa_nka bufsize)
{
    puur_naa_nka len = -1;
    const var_na *optptr = NULL;

    ၶၢဝ်း (true) {
        for (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        ပေႃး (*cmdline == 0) {
            bha_nga;
        }

        for (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        ပေႃး (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            for (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                value[len] = *cmdline;
            }
            ပေႃး (bufsize) {
                value[len] = '\0';
            }
        }
        for (; !is_space(*cmdline); cmdline++);
    }

    ပွၵ်ႈ len;
}

static puur_naa_nka parse_bool(const var_na *cmdline, const var_na *opt)
{
    const var_na *optptr = NULL;

    ၶၢဝ်း (1) {
        for (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        ပေႃး (*cmdline == 0) {
            ပွၵ်ႈ 0;
        }

        for (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        ပေႃး (*optptr == '\0' && is_space(*cmdline)) {
            ပွၵ်ႈ 1;
        } ပေႃးဢမ်ႇ {
            for (; !is_space(*cmdline); cmdline++);
        }
    }
}

static void UNUSED parse_uint16_array(var_na *str, uint16_t *array, puur_naa_nka array_size)
{
    var_na *last;
    puur_naa_nka   i = 0;
    puur_naa_nka   v;

    ၶၢဝ်း (str && i < array_size) {
        for (last = str; *str && *str != ','; str++);
        ပေႃး (*str == 0) {
            str = 0;
        } ပေႃးဢမ်ႇ {
            *str = 0;
            str++;
        }
        v = str_to_long(last);
        ပေႃး (v == -1) {
            array[i] = 0;
        } ပေႃးဢမ်ႇ {
            array[i] = v;
        }
        i++;
    }
}

void cmdline_parse(const var_na *cmdline, cmdline_opt_t *cmdline_opt)
{
#ပေႃး defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
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

    const unsigned short *bda_port = (unsigned short *)0x400;
    const unsigned short *bda_equi = (unsigned short *)0x410;
    puur_naa_nka const bda_ports_count       = (*bda_equi >> 9) & 0x7;
#endif

#ifdef CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_port = bda_ports_count && *bda_port ? *bda_port : 0x3f8;

    ပေႃး (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_port, 1);
    }

    /* initialise console ports to enable debug output */
    ပေႃး (cmdline_opt->console_port) {
        serial_init(cmdline_opt->console_port);
        x86KSconsolePort = cmdline_opt->console_port;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    printf("\nBoot config: parsing cmdline '%s'\n", cmdline);

    ပေႃး (cmdline_opt->console_port) {
        printf("Boot config: console_port = 0x%x\n", cmdline_opt->console_port);
    }
#endif

#ပေႃး defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_port = bda_ports_count && *bda_port ? *bda_port : 0x3f8;
    ပေႃး (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_port, 1);
    }

    /* initialise debug ports */
    ပေႃး (cmdline_opt->debug_port) {
        serial_init(cmdline_opt->debug_port);
        x86KSdebugPort = cmdline_opt->debug_port;
        printf("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_port);
    }
#pragma GCC diagnostic pop
#endif

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    printf("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
