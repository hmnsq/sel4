/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <util.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/cmdline.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/boot_sys.h>
#ଅନ୍ତର୍ଭାଵ <linker.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/io.h>

/* 'cmdline_val' is declared globally because of a C-subset restriction.
 * It is only used in cmdline_parse(), which therefore is non-reentrant.
 */
#define MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
ଅକ୍ଷର cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
ଅଚଳ ଅକ୍ଷର cmdline_str_max_ଅଙ୍କ_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
ଅଚଳ ଅକ୍ଷର cmdline_str_ଅଙ୍କ_sh_frames[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
ଅଚଳ ଅକ୍ଷର cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_space(ଅକ୍ଷର c)
{
    ଫେରନ୍ତୁ c <= ' ';
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା UNUSED parse_opt(ଅଚଳ ଅକ୍ଷର *cmdline, ଅଚଳ ଅକ୍ଷର *opt, ଅକ୍ଷର *ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା bufsize)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା len = -1;
    ଅଚଳ ଅକ୍ଷର *optptr = ଖାଲି;

    ଯେତେବେଳେ (true) {
        ପାଇଁ (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        ଯଦି (*cmdline == 0) {
            ଭାଙ୍ଗ;
        }

        ପାଇଁ (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        ଯଦି (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            ପାଇଁ (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                ମୂଲ୍ୟ[len] = *cmdline;
            }
            ଯଦି (bufsize) {
                ମୂଲ୍ୟ[len] = '\0';
            }
        }
        ପାଇଁ (; !is_space(*cmdline); cmdline++);
    }

    ଫେରନ୍ତୁ len;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_bool(ଅଚଳ ଅକ୍ଷର *cmdline, ଅଚଳ ଅକ୍ଷର *opt)
{
    ଅଚଳ ଅକ୍ଷର *optptr = ଖାଲି;

    ଯେତେବେଳେ (1) {
        ପାଇଁ (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        ଯଦି (*cmdline == 0) {
            ଫେରନ୍ତୁ 0;
        }

        ପାଇଁ (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        ଯଦି (*optptr == '\0' && is_space(*cmdline)) {
            ଫେରନ୍ତୁ 1;
        } ଅନ୍ୟଥା {
            ପାଇଁ (; !is_space(*cmdline); cmdline++);
        }
    }
}

ସ୍ଥିର ଶୂନ୍ୟ UNUSED parse_uint16_array(ଅକ୍ଷର *str, uint16_t *array, ପୂର୍ଣ୍ଣସଂଖ୍ୟା array_size)
{
    ଅକ୍ଷର *last;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା   i = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା   v;

    ଯେତେବେଳେ (str && i < array_size) {
        ପାଇଁ (last = str; *str && *str != ','; str++);
        ଯଦି (*str == 0) {
            str = 0;
        } ଅନ୍ୟଥା {
            *str = 0;
            str++;
        }
        v = str_to_long(last);
        ଯଦି (v == -1) {
            array[i] = 0;
        } ଅନ୍ୟଥା {
            array[i] = v;
        }
        i++;
    }
}

ଶୂନ୍ୟ cmdline_parse(ଅଚଳ ଅକ୍ଷର *cmdline, cmdline_opt_t *cmdline_opt)
{
#ଯଦି defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* use BIOS data area to read serial configuration. The BDA is not
     * fully standardized and parts are absolete. See http://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
     * for an explanation
     */
    /*
     * gcc-12 gives array bounds warning (treated as errors by the
     * build system) for any address below 4096.  Turn that off for
     * accessing the BIOS area.
     */
#pragma GCC diagnostic ଠେଲ
#pragma GCC diagnostic ignored "-Warray-bounds"

    ଅଚଳ unsigned short *bda_port = (unsigned short *)0x400;
    ଅଚଳ unsigned short *bda_equi = (unsigned short *)0x410;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଚଳ bda_ports_ଗଣନା       = (*bda_equi >> 9) & 0x7;
#endif

#ifdef CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_port = bda_ports_ଗଣନା && *bda_port ? *bda_port : 0x3f8;

    ଯଦି (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_port, 1);
    }

    /* initialise console ports to enable debug output */
    ଯଦି (cmdline_opt->console_port) {
        serial_init(cmdline_opt->console_port);
        x86KSconsolePort = cmdline_opt->console_port;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    printf("\nBoot config: parsing cmdline '%s'\n", cmdline);

    ଯଦି (cmdline_opt->console_port) {
        printf("Boot config: console_port = 0x%x\n", cmdline_opt->console_port);
    }
#endif

#ଯଦି defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_port = bda_ports_ଗଣନା && *bda_port ? *bda_port : 0x3f8;
    ଯଦି (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_port, 1);
    }

    /* initialise debug ports */
    ଯଦି (cmdline_opt->debug_port) {
        serial_init(cmdline_opt->debug_port);
        x86KSdebugPort = cmdline_opt->debug_port;
        printf("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_port);
    }
#pragma GCC diagnostic କାଢ଼
#endif

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    printf("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
