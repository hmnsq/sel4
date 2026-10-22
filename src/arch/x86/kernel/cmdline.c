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
char cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
const char cmdline_str_max_num_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
const char cmdline_str_num_sh_frames[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
const char cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

static цяло is_space(char c)
{
    върни c <= ' ';
}

static цяло UNUSED parse_opt(const char *cmdline, const char *opt, char *value, цяло bufsize)
{
    цяло len = -1;
    const char *optptr = NULL;

    докато (true) {
        за (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        ако (*cmdline == 0) {
            break;
        }

        за (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        ако (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            за (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                value[len] = *cmdline;
            }
            ако (bufsize) {
                value[len] = '\0';
            }
        }
        за (; !is_space(*cmdline); cmdline++);
    }

    върни len;
}

static цяло parse_bool(const char *cmdline, const char *opt)
{
    const char *optptr = NULL;

    докато (1) {
        за (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        ако (*cmdline == 0) {
            върни 0;
        }

        за (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        ако (*optptr == '\0' && is_space(*cmdline)) {
            върни 1;
        } иначе {
            за (; !is_space(*cmdline); cmdline++);
        }
    }
}

static void UNUSED parse_uint16_array(char *str, uint16_t *array, цяло array_size)
{
    char *last;
    цяло   i = 0;
    цяло   v;

    докато (str && i < array_size) {
        за (last = str; *str && *str != ','; str++);
        ако (*str == 0) {
            str = 0;
        } иначе {
            *str = 0;
            str++;
        }
        v = str_to_long(last);
        ако (v == -1) {
            array[i] = 0;
        } иначе {
            array[i] = v;
        }
        i++;
    }
}

void cmdline_parse(const char *cmdline, cmdline_opt_t *cmdline_opt)
{
#ако defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
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
    цяло const bda_ports_count       = (*bda_equi >> 9) & 0x7;
#endif

#ifdef CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_port = bda_ports_count && *bda_port ? *bda_port : 0x3f8;

    ако (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_port, 1);
    }

    /* initialise console ports to enable debug output */
    ако (cmdline_opt->console_port) {
        serial_init(cmdline_opt->console_port);
        x86KSconsolePort = cmdline_opt->console_port;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    printf("\nBoot config: parsing cmdline '%s'\n", cmdline);

    ако (cmdline_opt->console_port) {
        printf("Boot config: console_port = 0x%x\n", cmdline_opt->console_port);
    }
#endif

#ако defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_port = bda_ports_count && *bda_port ? *bda_port : 0x3f8;
    ако (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_port, 1);
    }

    /* initialise debug ports */
    ако (cmdline_opt->debug_port) {
        serial_init(cmdline_opt->debug_port);
        x86KSdebugPort = cmdline_opt->debug_port;
        printf("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_port);
    }
#pragma GCC diagnostic pop
#endif

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    printf("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
