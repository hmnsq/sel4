/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <util.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <arch/kernel/cmdline.h>
#അന്തര്ഭാവ <arch/kernel/boot_sys.h>
#അന്തര്ഭാവ <linker.h>
#അന്തര്ഭാവ <plat/machine/io.h>

/* 'cmdline_val' is declared globally because of a C-subset restriction.
 * It is only used in cmdline_parse(), which therefore is non-reentrant.
 */
#define MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
അക്ഷരം cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
സ്ഥിരം അക്ഷരം cmdline_str_max_അക്കം_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
സ്ഥിരം അക്ഷരം cmdline_str_അക്കം_sh_frames[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
സ്ഥിരം അക്ഷരം cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

സ്ഥിതം പൂർണ്ണസംഖ്യ is_space(അക്ഷരം c)
{
    തിരികെ c <= ' ';
}

സ്ഥിതം പൂർണ്ണസംഖ്യ UNUSED parse_opt(സ്ഥിരം അക്ഷരം *cmdline, സ്ഥിരം അക്ഷരം *opt, അക്ഷരം *മൂല്യം, പൂർണ്ണസംഖ്യ bufsize)
{
    പൂർണ്ണസംഖ്യ len = -1;
    സ്ഥിരം അക്ഷരം *optptr = ശൂന്യ;

    ആകുമ്പോൾ (true) {
        വേണ്ടി (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        എങ്കിൽ (*cmdline == 0) {
            നിർത്തുക;
        }

        വേണ്ടി (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        എങ്കിൽ (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            വേണ്ടി (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                മൂല്യം[len] = *cmdline;
            }
            എങ്കിൽ (bufsize) {
                മൂല്യം[len] = '\0';
            }
        }
        വേണ്ടി (; !is_space(*cmdline); cmdline++);
    }

    തിരികെ len;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parse_bool(സ്ഥിരം അക്ഷരം *cmdline, സ്ഥിരം അക്ഷരം *opt)
{
    സ്ഥിരം അക്ഷരം *optptr = ശൂന്യ;

    ആകുമ്പോൾ (1) {
        വേണ്ടി (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        എങ്കിൽ (*cmdline == 0) {
            തിരികെ 0;
        }

        വേണ്ടി (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        എങ്കിൽ (*optptr == '\0' && is_space(*cmdline)) {
            തിരികെ 1;
        } അല്ലെങ്കിൽ {
            വേണ്ടി (; !is_space(*cmdline); cmdline++);
        }
    }
}

സ്ഥിതം ശൂന്യം UNUSED parse_uint16_array(അക്ഷരം *str, uint16_t *array, പൂർണ്ണസംഖ്യ array_size)
{
    അക്ഷരം *last;
    പൂർണ്ണസംഖ്യ   i = 0;
    പൂർണ്ണസംഖ്യ   v;

    ആകുമ്പോൾ (str && i < array_size) {
        വേണ്ടി (last = str; *str && *str != ','; str++);
        എങ്കിൽ (*str == 0) {
            str = 0;
        } അല്ലെങ്കിൽ {
            *str = 0;
            str++;
        }
        v = str_to_long(last);
        എങ്കിൽ (v == -1) {
            array[i] = 0;
        } അല്ലെങ്കിൽ {
            array[i] = v;
        }
        i++;
    }
}

ശൂന്യം cmdline_parse(സ്ഥിരം അക്ഷരം *cmdline, cmdline_opt_t *cmdline_opt)
{
#എങ്കിൽ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* use BIOS data area to read serial configuration. The BDA is not
     * fully standardized and parts are absolete. See http://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
     * for an explanation
     */
    /*
     * gcc-12 gives array bounds warning (treated as errors by the
     * build system) for any address below 4096.  Turn that off for
     * accessing the BIOS area.
     */
#pragma GCC diagnostic തള്ളുക
#pragma GCC diagnostic ignored "-Warray-bounds"

    സ്ഥിരം unsigned short *bda_port = (unsigned short *)0x400;
    സ്ഥിരം unsigned short *bda_equi = (unsigned short *)0x410;
    പൂർണ്ണസംഖ്യ സ്ഥിരം bda_ports_എണ്ണം       = (*bda_equi >> 9) & 0x7;
#endif

#ifdef CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_port = bda_ports_എണ്ണം && *bda_port ? *bda_port : 0x3f8;

    എങ്കിൽ (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_port, 1);
    }

    /* initialise console ports to enable debug output */
    എങ്കിൽ (cmdline_opt->console_port) {
        serial_init(cmdline_opt->console_port);
        x86KSconsolePort = cmdline_opt->console_port;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    printf("\nBoot config: parsing cmdline '%s'\n", cmdline);

    എങ്കിൽ (cmdline_opt->console_port) {
        printf("Boot config: console_port = 0x%x\n", cmdline_opt->console_port);
    }
#endif

#എങ്കിൽ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_port = bda_ports_എണ്ണം && *bda_port ? *bda_port : 0x3f8;
    എങ്കിൽ (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_port, 1);
    }

    /* initialise debug ports */
    എങ്കിൽ (cmdline_opt->debug_port) {
        serial_init(cmdline_opt->debug_port);
        x86KSdebugPort = cmdline_opt->debug_port;
        printf("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_port);
    }
#pragma GCC diagnostic എടുക്കുക
#endif

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    printf("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
