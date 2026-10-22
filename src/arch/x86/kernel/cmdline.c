/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <util.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <arch/kernel/cmdline.h>
#અન્તર્ભાવ <arch/kernel/boot_sys.h>
#અન્તર્ભાવ <linker.h>
#અન્તર્ભાવ <plat/machine/io.h>

/* 'cmdline_val' is declared globally because of a C-subset restriction.
 * It is only used in cmdline_parse(), which therefore is non-reentrant.
 */
#define MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
અક્ષર cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
અચલ અક્ષર cmdline_str_max_અંક_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
અચલ અક્ષર cmdline_str_અંક_sh_frames[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
અચલ અક્ષર cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

સ્થિર પૂર્ણાંક is_space(અક્ષર c)
{
    પાછા c <= ' ';
}

સ્થિર પૂર્ણાંક UNUSED parse_opt(અચલ અક્ષર *cmdline, અચલ અક્ષર *opt, અક્ષર *કિંમત, પૂર્ણાંક bufsize)
{
    પૂર્ણાંક len = -1;
    અચલ અક્ષર *optptr = ખાલી;

    જ્યારે (true) {
        માટે (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        જો (*cmdline == 0) {
            તોડો;
        }

        માટે (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        જો (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            માટે (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                કિંમત[len] = *cmdline;
            }
            જો (bufsize) {
                કિંમત[len] = '\0';
            }
        }
        માટે (; !is_space(*cmdline); cmdline++);
    }

    પાછા len;
}

સ્થિર પૂર્ણાંક parse_bool(અચલ અક્ષર *cmdline, અચલ અક્ષર *opt)
{
    અચલ અક્ષર *optptr = ખાલી;

    જ્યારે (1) {
        માટે (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        જો (*cmdline == 0) {
            પાછા 0;
        }

        માટે (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        જો (*optptr == '\0' && is_space(*cmdline)) {
            પાછા 1;
        } નહીંતર {
            માટે (; !is_space(*cmdline); cmdline++);
        }
    }
}

સ્થિર શૂન્ય UNUSED parse_uint16_array(અક્ષર *str, uint16_t *array, પૂર્ણાંક array_size)
{
    અક્ષર *last;
    પૂર્ણાંક   i = 0;
    પૂર્ણાંક   v;

    જ્યારે (str && i < array_size) {
        માટે (last = str; *str && *str != ','; str++);
        જો (*str == 0) {
            str = 0;
        } નહીંતર {
            *str = 0;
            str++;
        }
        v = str_to_long(last);
        જો (v == -1) {
            array[i] = 0;
        } નહીંતર {
            array[i] = v;
        }
        i++;
    }
}

શૂન્ય cmdline_parse(અચલ અક્ષર *cmdline, cmdline_opt_t *cmdline_opt)
{
#જો defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* use BIOS data area to read serial configuration. The BDA is not
     * fully standardized and parts are absolete. See http://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
     * for an explanation
     */
    /*
     * gcc-12 gives array bounds warning (treated as errors by the
     * build system) for any address below 4096.  Turn that off for
     * accessing the BIOS area.
     */
#pragma GCC diagnostic ધકેલો
#pragma GCC diagnostic ignored "-Warray-bounds"

    અચલ unsigned short *bda_port = (unsigned short *)0x400;
    અચલ unsigned short *bda_equi = (unsigned short *)0x410;
    પૂર્ણાંક અચલ bda_ports_ગણતરી       = (*bda_equi >> 9) & 0x7;
#endif

#ifdef CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_port = bda_ports_ગણતરી && *bda_port ? *bda_port : 0x3f8;

    જો (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_port, 1);
    }

    /* initialise console ports to enable debug output */
    જો (cmdline_opt->console_port) {
        serial_init(cmdline_opt->console_port);
        x86KSconsolePort = cmdline_opt->console_port;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    printf("\nBoot config: parsing cmdline '%s'\n", cmdline);

    જો (cmdline_opt->console_port) {
        printf("Boot config: console_port = 0x%x\n", cmdline_opt->console_port);
    }
#endif

#જો defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_port = bda_ports_ગણતરી && *bda_port ? *bda_port : 0x3f8;
    જો (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_port, 1);
    }

    /* initialise debug ports */
    જો (cmdline_opt->debug_port) {
        serial_init(cmdline_opt->debug_port);
        x86KSdebugPort = cmdline_opt->debug_port;
        printf("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_port);
    }
#pragma GCC diagnostic કાઢો
#endif

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    printf("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
