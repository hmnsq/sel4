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

static ຈຳນວນເຕັມ is_space(var_na c)
{
    ສົ່ງຄືນ c <= ' ';
}

static ຈຳນວນເຕັມ UNUSED parse_opt(const var_na *cmdline, const var_na *opt, var_na *value, ຈຳນວນເຕັມ bufsize)
{
    ຈຳນວນເຕັມ len = -1;
    const var_na *optptr = NULL;

    ໃນຂະນະທີ່ (true) {
        ສຳລັບ (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        ຖ້າ (*cmdline == 0) {
            bha_nga;
        }

        ສຳລັບ (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        ຖ້າ (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            ສຳລັບ (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                value[len] = *cmdline;
            }
            ຖ້າ (bufsize) {
                value[len] = '\0';
            }
        }
        ສຳລັບ (; !is_space(*cmdline); cmdline++);
    }

    ສົ່ງຄືນ len;
}

static ຈຳນວນເຕັມ parse_bool(const var_na *cmdline, const var_na *opt)
{
    const var_na *optptr = NULL;

    ໃນຂະນະທີ່ (1) {
        ສຳລັບ (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        ຖ້າ (*cmdline == 0) {
            ສົ່ງຄືນ 0;
        }

        ສຳລັບ (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        ຖ້າ (*optptr == '\0' && is_space(*cmdline)) {
            ສົ່ງຄືນ 1;
        } ບໍ່ດັ່ງນັ້ນ {
            ສຳລັບ (; !is_space(*cmdline); cmdline++);
        }
    }
}

static void UNUSED parse_uint16_array(var_na *str, uint16_t *array, ຈຳນວນເຕັມ array_size)
{
    var_na *last;
    ຈຳນວນເຕັມ   i = 0;
    ຈຳນວນເຕັມ   v;

    ໃນຂະນະທີ່ (str && i < array_size) {
        ສຳລັບ (last = str; *str && *str != ','; str++);
        ຖ້າ (*str == 0) {
            str = 0;
        } ບໍ່ດັ່ງນັ້ນ {
            *str = 0;
            str++;
        }
        v = str_to_long(last);
        ຖ້າ (v == -1) {
            array[i] = 0;
        } ບໍ່ດັ່ງນັ້ນ {
            array[i] = v;
        }
        i++;
    }
}

void cmdline_parse(const var_na *cmdline, cmdline_opt_t *cmdline_opt)
{
#ຖ້າ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
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
    ຈຳນວນເຕັມ const bda_ports_count       = (*bda_equi >> 9) & 0x7;
#endif

#ifdef CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_port = bda_ports_count && *bda_port ? *bda_port : 0x3f8;

    ຖ້າ (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_port, 1);
    }

    /* initialise console ports to enable debug output */
    ຖ້າ (cmdline_opt->console_port) {
        serial_init(cmdline_opt->console_port);
        x86KSconsolePort = cmdline_opt->console_port;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    printf("\nBoot config: parsing cmdline '%s'\n", cmdline);

    ຖ້າ (cmdline_opt->console_port) {
        printf("Boot config: console_port = 0x%x\n", cmdline_opt->console_port);
    }
#endif

#ຖ້າ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_port = bda_ports_count && *bda_port ? *bda_port : 0x3f8;
    ຖ້າ (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_port, 1);
    }

    /* initialise debug ports */
    ຖ້າ (cmdline_opt->debug_port) {
        serial_init(cmdline_opt->debug_port);
        x86KSdebugPort = cmdline_opt->debug_port;
        printf("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_port);
    }
#pragma GCC diagnostic pop
#endif

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    printf("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
