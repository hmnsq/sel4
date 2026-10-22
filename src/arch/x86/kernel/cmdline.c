/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/cmdline.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/boot_sys.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/io.h>

/* 'cmdline_val' is declared globally because of a C-subset restriction.
 * It is only used in cmdline_parse(), which therefore is non-reentrant.
 */
#define MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
ವರ್ಣ cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
ಧ್ರುವ ವರ್ಣ cmdline_str_max_ಅಂಕೆ_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
ಧ್ರುವ ವರ್ಣ cmdline_str_ಅಂಕೆ_sh_frames[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
ಧ್ರುವ ವರ್ಣ cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ is_space(ವರ್ಣ c)
{
    ಪ್ರತ್ಯಾಗಮ c <= ' ';
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ UNUSED parse_opt(ಧ್ರುವ ವರ್ಣ *cmdline, ಧ್ರುವ ವರ್ಣ *opt, ವರ್ಣ *ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ bufsize)
{
    ಪೂರ್ಣಾಙ್ಕ len = -1;
    ಧ್ರುವ ವರ್ಣ *optptr = ರಿಕ್ತ;

    ಯಾವತ್ (true) {
        ಪ್ರತಿ (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        ಯದಿ (*cmdline == 0) {
            ಭಙ್ಗ;
        }

        ಪ್ರತಿ (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        ಯದಿ (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            ಪ್ರತಿ (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                ಮೌಲ್ಯ[len] = *cmdline;
            }
            ಯದಿ (bufsize) {
                ಮೌಲ್ಯ[len] = '\0';
            }
        }
        ಪ್ರತಿ (; !is_space(*cmdline); cmdline++);
    }

    ಪ್ರತ್ಯಾಗಮ len;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parse_bool(ಧ್ರುವ ವರ್ಣ *cmdline, ಧ್ರುವ ವರ್ಣ *opt)
{
    ಧ್ರುವ ವರ್ಣ *optptr = ರಿಕ್ತ;

    ಯಾವತ್ (1) {
        ಪ್ರತಿ (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        ಯದಿ (*cmdline == 0) {
            ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಪ್ರತಿ (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        ಯದಿ (*optptr == '\0' && is_space(*cmdline)) {
            ಪ್ರತ್ಯಾಗಮ 1;
        } ಅನ್ಯಥಾ {
            ಪ್ರತಿ (; !is_space(*cmdline); cmdline++);
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ UNUSED parse_uint16_array(ವರ್ಣ *str, uint16_t *array, ಪೂರ್ಣಾಙ್ಕ array_size)
{
    ವರ್ಣ *last;
    ಪೂರ್ಣಾಙ್ಕ   i = 0;
    ಪೂರ್ಣಾಙ್ಕ   v;

    ಯಾವತ್ (str && i < array_size) {
        ಪ್ರತಿ (last = str; *str && *str != ','; str++);
        ಯದಿ (*str == 0) {
            str = 0;
        } ಅನ್ಯಥಾ {
            *str = 0;
            str++;
        }
        v = str_to_long(last);
        ಯದಿ (v == -1) {
            array[i] = 0;
        } ಅನ್ಯಥಾ {
            array[i] = v;
        }
        i++;
    }
}

ಶೂನ್ಯ cmdline_parse(ಧ್ರುವ ವರ್ಣ *cmdline, cmdline_opt_t *cmdline_opt)
{
#ಯದಿ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* use BIOS data area to read serial configuration. The BDA is not
     * fully standardized and parts are absolete. See http://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
     * for an explanation
     */
    /*
     * gcc-12 gives array bounds warning (treated as errors by the
     * build system) for any address below 4096.  Turn that off for
     * accessing the BIOS area.
     */
#pragma GCC diagnostic ತಳ್ಳು
#pragma GCC diagnostic ignored "-Warray-bounds"

    ಧ್ರುವ unsigned short *bda_port = (unsigned short *)0x400;
    ಧ್ರುವ unsigned short *bda_equi = (unsigned short *)0x410;
    ಪೂರ್ಣಾಙ್ಕ ಧ್ರುವ bda_ports_ಎಣಿಕೆ       = (*bda_equi >> 9) & 0x7;
#endif

#ifdef CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_port = bda_ports_ಎಣಿಕೆ && *bda_port ? *bda_port : 0x3f8;

    ಯದಿ (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_port, 1);
    }

    /* initialise console ports to enable debug output */
    ಯದಿ (cmdline_opt->console_port) {
        serial_init(cmdline_opt->console_port);
        x86KSconsolePort = cmdline_opt->console_port;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    printf("\nBoot config: parsing cmdline '%s'\n", cmdline);

    ಯದಿ (cmdline_opt->console_port) {
        printf("Boot config: console_port = 0x%x\n", cmdline_opt->console_port);
    }
#endif

#ಯದಿ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_port = bda_ports_ಎಣಿಕೆ && *bda_port ? *bda_port : 0x3f8;
    ಯದಿ (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_port, 1);
    }

    /* initialise debug ports */
    ಯದಿ (cmdline_opt->debug_port) {
        serial_init(cmdline_opt->debug_port);
        x86KSdebugPort = cmdline_opt->debug_port;
        printf("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_port);
    }
#pragma GCC diagnostic ತೆಗೆ
#endif

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    printf("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
