/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <util.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <arch/kernel/cmdline.h>
#அந்தர்பாவ <arch/kernel/boot_sys.h>
#அந்தர்பாவ <linker.h>
#அந்தர்பாவ <plat/machine/io.h>

/* 'cmdline_val' is declared globally because of a C-subset restriction.
 * It is only used in cmdline_parse(), which therefore is non-reentrant.
 */
#define MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
எழுத்து cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
மாறிலி எழுத்து cmdline_str_max_இலக்கம்_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
மாறிலி எழுத்து cmdline_str_இலக்கம்_sh_frames[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
மாறிலி எழுத்து cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

நிலையான முழு is_space(எழுத்து c)
{
    திரும்பு c <= ' ';
}

நிலையான முழு UNUSED parse_opt(மாறிலி எழுத்து *cmdline, மாறிலி எழுத்து *opt, எழுத்து *மதிப்பு, முழு bufsize)
{
    முழு len = -1;
    மாறிலி எழுத்து *optptr = வெற்று;

    போது (true) {
        க்கு (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        எனில் (*cmdline == 0) {
            நிறுத்து;
        }

        க்கு (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        எனில் (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            க்கு (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                மதிப்பு[len] = *cmdline;
            }
            எனில் (bufsize) {
                மதிப்பு[len] = '\0';
            }
        }
        க்கு (; !is_space(*cmdline); cmdline++);
    }

    திரும்பு len;
}

நிலையான முழு parse_bool(மாறிலி எழுத்து *cmdline, மாறிலி எழுத்து *opt)
{
    மாறிலி எழுத்து *optptr = வெற்று;

    போது (1) {
        க்கு (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        எனில் (*cmdline == 0) {
            திரும்பு 0;
        }

        க்கு (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        எனில் (*optptr == '\0' && is_space(*cmdline)) {
            திரும்பு 1;
        } இல்லையெனில் {
            க்கு (; !is_space(*cmdline); cmdline++);
        }
    }
}

நிலையான வெறுமை UNUSED parse_uint16_array(எழுத்து *str, uint16_t *array, முழு array_size)
{
    எழுத்து *last;
    முழு   i = 0;
    முழு   v;

    போது (str && i < array_size) {
        க்கு (last = str; *str && *str != ','; str++);
        எனில் (*str == 0) {
            str = 0;
        } இல்லையெனில் {
            *str = 0;
            str++;
        }
        v = str_to_long(last);
        எனில் (v == -1) {
            array[i] = 0;
        } இல்லையெனில் {
            array[i] = v;
        }
        i++;
    }
}

வெறுமை cmdline_parse(மாறிலி எழுத்து *cmdline, cmdline_opt_t *cmdline_opt)
{
#எனில் defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* use BIOS data area to read serial configuration. The BDA is not
     * fully standardized and parts are absolete. See http://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
     * for an explanation
     */
    /*
     * gcc-12 gives array bounds warning (treated as errors by the
     * build system) for any address below 4096.  Turn that off for
     * accessing the BIOS area.
     */
#pragma GCC diagnostic தள்ளு
#pragma GCC diagnostic ignored "-Warray-bounds"

    மாறிலி unsigned short *bda_port = (unsigned short *)0x400;
    மாறிலி unsigned short *bda_equi = (unsigned short *)0x410;
    முழு மாறிலி bda_ports_மொத்தம்       = (*bda_equi >> 9) & 0x7;
#endif

#ifdef CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_port = bda_ports_மொத்தம் && *bda_port ? *bda_port : 0x3f8;

    எனில் (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_port, 1);
    }

    /* initialise console ports to enable debug output */
    எனில் (cmdline_opt->console_port) {
        serial_init(cmdline_opt->console_port);
        x86KSconsolePort = cmdline_opt->console_port;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    printf("\nBoot config: parsing cmdline '%s'\n", cmdline);

    எனில் (cmdline_opt->console_port) {
        printf("Boot config: console_port = 0x%x\n", cmdline_opt->console_port);
    }
#endif

#எனில் defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_port = bda_ports_மொத்தம் && *bda_port ? *bda_port : 0x3f8;
    எனில் (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_port, 1);
    }

    /* initialise debug ports */
    எனில் (cmdline_opt->debug_port) {
        serial_init(cmdline_opt->debug_port);
        x86KSdebugPort = cmdline_opt->debug_port;
        printf("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_port);
    }
#pragma GCC diagnostic எடு
#endif

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    printf("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
