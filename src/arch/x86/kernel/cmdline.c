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

static ကိန်းပြည့် is_space(var_na c)
{
    ပြန်ပေး c <= ' ';
}

static ကိန်းပြည့် UNUSED parse_opt(const var_na *cmdline, const var_na *opt, var_na *value, ကိန်းပြည့် bufsize)
{
    ကိန်းပြည့် len = -1;
    const var_na *optptr = NULL;

    နေစဉ် (true) {
        အတွက် (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        အကယ်၍ (*cmdline == 0) {
            bha_nga;
        }

        အတွက် (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        အကယ်၍ (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            အတွက် (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                value[len] = *cmdline;
            }
            အကယ်၍ (bufsize) {
                value[len] = '\0';
            }
        }
        အတွက် (; !is_space(*cmdline); cmdline++);
    }

    ပြန်ပေး len;
}

static ကိန်းပြည့် parse_bool(const var_na *cmdline, const var_na *opt)
{
    const var_na *optptr = NULL;

    နေစဉ် (1) {
        အတွက် (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        အကယ်၍ (*cmdline == 0) {
            ပြန်ပေး 0;
        }

        အတွက် (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        အကယ်၍ (*optptr == '\0' && is_space(*cmdline)) {
            ပြန်ပေး 1;
        } မဟုတ်ပါက {
            အတွက် (; !is_space(*cmdline); cmdline++);
        }
    }
}

static void UNUSED parse_uint16_array(var_na *str, uint16_t *array, ကိန်းပြည့် array_size)
{
    var_na *last;
    ကိန်းပြည့်   i = 0;
    ကိန်းပြည့်   v;

    နေစဉ် (str && i < array_size) {
        အတွက် (last = str; *str && *str != ','; str++);
        အကယ်၍ (*str == 0) {
            str = 0;
        } မဟုတ်ပါက {
            *str = 0;
            str++;
        }
        v = str_to_long(last);
        အကယ်၍ (v == -1) {
            array[i] = 0;
        } မဟုတ်ပါက {
            array[i] = v;
        }
        i++;
    }
}

void cmdline_parse(const var_na *cmdline, cmdline_opt_t *cmdline_opt)
{
#အကယ်၍ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
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
    ကိန်းပြည့် const bda_ports_count       = (*bda_equi >> 9) & 0x7;
#endif

#ifdef CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_port = bda_ports_count && *bda_port ? *bda_port : 0x3f8;

    အကယ်၍ (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_port, 1);
    }

    /* initialise console ports to enable debug output */
    အကယ်၍ (cmdline_opt->console_port) {
        serial_init(cmdline_opt->console_port);
        x86KSconsolePort = cmdline_opt->console_port;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    printf("\nBoot config: parsing cmdline '%s'\n", cmdline);

    အကယ်၍ (cmdline_opt->console_port) {
        printf("Boot config: console_port = 0x%x\n", cmdline_opt->console_port);
    }
#endif

#အကယ်၍ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_port = bda_ports_count && *bda_port ? *bda_port : 0x3f8;
    အကယ်၍ (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_port, 1);
    }

    /* initialise debug ports */
    အကယ်၍ (cmdline_opt->debug_port) {
        serial_init(cmdline_opt->debug_port);
        x86KSdebugPort = cmdline_opt->debug_port;
        printf("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_port);
    }
#pragma GCC diagnostic pop
#endif

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    printf("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
