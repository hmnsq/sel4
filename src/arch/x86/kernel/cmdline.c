/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <util.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <arch/kernel/cmdline.h>
#অন্তর্ভাব <arch/kernel/boot_sys.h>
#অন্তর্ভাব <linker.h>
#অন্তর্ভাব <plat/machine/io.h>

/* 'cmdline_val' is declared globally because of a C-subset restriction.
 * It is only used in cmdline_parse(), which therefore is non-reentrant.
 */
#define MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
অক্ষর cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
ধ্রুবক অক্ষর cmdline_str_max_অঙ্ক_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
ধ্রুবক অক্ষর cmdline_str_অঙ্ক_sh_frames[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
ধ্রুবক অক্ষর cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

স্থির পূর্ণসংখ্যা is_space(অক্ষর c)
{
    ফেরত c <= ' ';
}

স্থির পূর্ণসংখ্যা UNUSED parse_opt(ধ্রুবক অক্ষর *cmdline, ধ্রুবক অক্ষর *opt, অক্ষর *মান, পূর্ণসংখ্যা bufsize)
{
    পূর্ণসংখ্যা len = -1;
    ধ্রুবক অক্ষর *optptr = ফাঁকা;

    যখন (true) {
        জন্য (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        যদি (*cmdline == 0) {
            বিরতি;
        }

        জন্য (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        যদি (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            জন্য (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                মান[len] = *cmdline;
            }
            যদি (bufsize) {
                মান[len] = '\0';
            }
        }
        জন্য (; !is_space(*cmdline); cmdline++);
    }

    ফেরত len;
}

স্থির পূর্ণসংখ্যা parse_bool(ধ্রুবক অক্ষর *cmdline, ধ্রুবক অক্ষর *opt)
{
    ধ্রুবক অক্ষর *optptr = ফাঁকা;

    যখন (1) {
        জন্য (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        যদি (*cmdline == 0) {
            ফেরত 0;
        }

        জন্য (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        যদি (*optptr == '\0' && is_space(*cmdline)) {
            ফেরত 1;
        } অন্যথা {
            জন্য (; !is_space(*cmdline); cmdline++);
        }
    }
}

স্থির শূন্য UNUSED parse_uint16_array(অক্ষর *str, uint16_t *array, পূর্ণসংখ্যা array_আকার)
{
    অক্ষর *last;
    পূর্ণসংখ্যা   i = 0;
    পূর্ণসংখ্যা   v;

    যখন (str && i < array_আকার) {
        জন্য (last = str; *str && *str != ','; str++);
        যদি (*str == 0) {
            str = 0;
        } অন্যথা {
            *str = 0;
            str++;
        }
        v = str_to_long(last);
        যদি (v == -1) {
            array[i] = 0;
        } অন্যথা {
            array[i] = v;
        }
        i++;
    }
}

শূন্য cmdline_parse(ধ্রুবক অক্ষর *cmdline, cmdline_opt_t *cmdline_opt)
{
#যদি defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* use BIOS data area to read serial configuration. The BDA is not
     * fully standardized and parts are absolete. See http://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
     * for an explanation
     */
    /*
     * gcc-12 gives array bounds warning (treated as errors by the
     * build system) for any address below 4096.  Turn that off for
     * accessing the BIOS area.
     */
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Warray-bounds"

    ধ্রুবক unsigned short *bda_port = (unsigned short *)0x400;
    ধ্রুবক unsigned short *bda_equi = (unsigned short *)0x410;
    পূর্ণসংখ্যা ধ্রুবক bda_ports_গুনতি       = (*bda_equi >> 9) & 0x7;
#endif

#ifdef CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_port = bda_ports_গুনতি && *bda_port ? *bda_port : 0x3f8;

    যদি (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_port, 1);
    }

    /* initialise console ports to enable debug output */
    যদি (cmdline_opt->console_port) {
        serial_init(cmdline_opt->console_port);
        x86KSconsolePort = cmdline_opt->console_port;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    printf("\nBoot config: parsing cmdline '%s'\n", cmdline);

    যদি (cmdline_opt->console_port) {
        printf("Boot config: console_port = 0x%x\n", cmdline_opt->console_port);
    }
#endif

#যদি defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_port = bda_ports_গুনতি && *bda_port ? *bda_port : 0x3f8;
    যদি (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_port, 1);
    }

    /* initialise debug ports */
    যদি (cmdline_opt->debug_port) {
        serial_init(cmdline_opt->debug_port);
        x86KSdebugPort = cmdline_opt->debug_port;
        printf("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_port);
    }
#pragma GCC diagnostic বের_করো
#endif

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    printf("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
