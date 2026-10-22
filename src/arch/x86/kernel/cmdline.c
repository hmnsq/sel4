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
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
ಅಕ್ಷರ cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ cmdline_str_max_ಅಂಕೆ_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ cmdline_str_ಅಂಕೆ_sh_ಚೌಕಟ್ಟುಗಳು[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ is_space(ಅಕ್ಷರ c)
{
    ಹಿಂತಿರುಗಿ c <= ' ';
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಬಳಸದ parse_opt(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cmdline, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *opt, ಅಕ್ಷರ *ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ bufsize)
{
    ಪೂರ್ಣಾಂಕ len = -1;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *optptr = ಖಾಲಿ;

    ಆಗ (true) {
        ಗಾಗಿ (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        ಆದರೆ (*cmdline == 0) {
            ನಿಲ್ಲಿಸು;
        }

        ಗಾಗಿ (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        ಆದರೆ (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            ಗಾಗಿ (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                ಮೌಲ್ಯ[len] = *cmdline;
            }
            ಆದರೆ (bufsize) {
                ಮೌಲ್ಯ[len] = '\0';
            }
        }
        ಗಾಗಿ (; !is_space(*cmdline); cmdline++);
    }

    ಹಿಂತಿರುಗಿ len;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ parse_bool(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cmdline, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *opt)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *optptr = ಖಾಲಿ;

    ಆಗ (1) {
        ಗಾಗಿ (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        ಆದರೆ (*cmdline == 0) {
            ಹಿಂತಿರುಗಿ 0;
        }

        ಗಾಗಿ (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        ಆದರೆ (*optptr == '\0' && is_space(*cmdline)) {
            ಹಿಂತಿರುಗಿ 1;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಗಾಗಿ (; !is_space(*cmdline); cmdline++);
        }
    }
}

ಸ್ಥಿರ ಶೂನ್ಯ ಬಳಸದ parse_uint16_array(ಅಕ್ಷರ *str, uint16_t *array, ಪೂರ್ಣಾಂಕ array_ಗಾತ್ರ)
{
    ಅಕ್ಷರ *ಕೊನೆಯ;
    ಪೂರ್ಣಾಂಕ   i = 0;
    ಪೂರ್ಣಾಂಕ   v;

    ಆಗ (str && i < array_ಗಾತ್ರ) {
        ಗಾಗಿ (ಕೊನೆಯ = str; *str && *str != ','; str++);
        ಆದರೆ (*str == 0) {
            str = 0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            *str = 0;
            str++;
        }
        v = str_ಗಮ್ಯ_long(ಕೊನೆಯ);
        ಆದರೆ (v == -1) {
            array[i] = 0;
        } ಇಲ್ಲದಿದ್ದರೆ {
            array[i] = v;
        }
        i++;
    }
}

ಶೂನ್ಯ cmdline_parse(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cmdline, cmdline_opt_t *cmdline_opt)
{
#ಆದರೆ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* use BIOS data area to read serial configuration. The BDA is not
     * fully standardized and parts are absolete. See http://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
     * for an explanation
     */
    /*
     * gcc-12 gives array bounds warning (treated as errors by the
     * build system) for any address below 4096.  Turn that off for
     * accessing the BIOS area.
     */
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Warray-bounds"

    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ *bda_ಪೋರ್ಟ್ = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ *)0x400;
    ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ *bda_equi = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ *)0x410;
    ಪೂರ್ಣಾಂಕ ಸ್ಥಿರಾಂಕ bda_ports_ಎಣಿಕೆ       = (*bda_equi >> 9) & 0x7;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_ಪೋರ್ಟ್ = bda_ports_ಎಣಿಕೆ && *bda_ಪೋರ್ಟ್ ? *bda_ಪೋರ್ಟ್ : 0x3f8;

    ಆದರೆ (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_ಪೋರ್ಟ್, 1);
    }

    /* initialise console ports to enable debug output */
    ಆದರೆ (cmdline_opt->console_ಪೋರ್ಟ್) {
        serial_init(cmdline_opt->console_ಪೋರ್ಟ್);
        x86_KSconsole_ಪೋರ್ಟ್ = cmdline_opt->console_ಪೋರ್ಟ್;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    ಬರೆ("\nBoot config: parsing cmdline '%s'\n", cmdline);

    ಆದರೆ (cmdline_opt->console_ಪೋರ್ಟ್) {
        ಬರೆ("Boot config: console_port = 0x%x\n", cmdline_opt->console_ಪೋರ್ಟ್);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_ಪೋರ್ಟ್ = bda_ports_ಎಣಿಕೆ && *bda_ಪೋರ್ಟ್ ? *bda_ಪೋರ್ಟ್ : 0x3f8;
    ಆದರೆ (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_ಪೋರ್ಟ್, 1);
    }

    /* initialise debug ports */
    ಆದರೆ (cmdline_opt->debug_ಪೋರ್ಟ್) {
        serial_init(cmdline_opt->debug_ಪೋರ್ಟ್);
        x86_KSdebug_ಪೋರ್ಟ್ = cmdline_opt->debug_ಪೋರ್ಟ್;
        ಬರೆ("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_ಪೋರ್ಟ್);
    }
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ
#ಆದರೆ_ಅಂತ್ಯ

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    ಬರೆ("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
