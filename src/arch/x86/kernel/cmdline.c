/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <arch/kernel/cmdline.h>
#అన్తర్భావ <arch/kernel/boot_sys.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine/io.h>

/* 'cmdline_val' is declared globally because of a C-subset restriction.
 * It is only used in cmdline_parse(), which therefore is non-reentrant.
 */
#నిర్వచించు MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
అక్షరం cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
స్థిరరాశి అక్షరం cmdline_str_max_అంకె_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
స్థిరరాశి అక్షరం cmdline_str_అంకె_sh_చట్రాలు[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
స్థిరరాశి అక్షరం cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

స్థిర పూర్ణాంకం is_space(అక్షరం c)
{
    తిరిగి c <= ' ';
}

స్థిర పూర్ణాంకం వాడని parse_opt(స్థిరరాశి అక్షరం *cmdline, స్థిరరాశి అక్షరం *opt, అక్షరం *విలువ, పూర్ణాంకం bufsize)
{
    పూర్ణాంకం len = -1;
    స్థిరరాశి అక్షరం *optptr = ఖాళీ;

    అయితే (true) {
        కొరకు (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        ఒకవేళ (*cmdline == 0) {
            ఆపు;
        }

        కొరకు (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        ఒకవేళ (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            కొరకు (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                విలువ[len] = *cmdline;
            }
            ఒకవేళ (bufsize) {
                విలువ[len] = '\0';
            }
        }
        కొరకు (; !is_space(*cmdline); cmdline++);
    }

    తిరిగి len;
}

స్థిర పూర్ణాంకం parse_bool(స్థిరరాశి అక్షరం *cmdline, స్థిరరాశి అక్షరం *opt)
{
    స్థిరరాశి అక్షరం *optptr = ఖాళీ;

    అయితే (1) {
        కొరకు (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        ఒకవేళ (*cmdline == 0) {
            తిరిగి 0;
        }

        కొరకు (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        ఒకవేళ (*optptr == '\0' && is_space(*cmdline)) {
            తిరిగి 1;
        } లేకపోతే {
            కొరకు (; !is_space(*cmdline); cmdline++);
        }
    }
}

స్థిర శూన్య వాడని parse_uint16_array(అక్షరం *str, uint16_t *array, పూర్ణాంకం array_కొలత)
{
    అక్షరం *చివరి;
    పూర్ణాంకం   i = 0;
    పూర్ణాంకం   v;

    అయితే (str && i < array_కొలత) {
        కొరకు (చివరి = str; *str && *str != ','; str++);
        ఒకవేళ (*str == 0) {
            str = 0;
        } లేకపోతే {
            *str = 0;
            str++;
        }
        v = str_గమ్యం_long(చివరి);
        ఒకవేళ (v == -1) {
            array[i] = 0;
        } లేకపోతే {
            array[i] = v;
        }
        i++;
    }
}

శూన్య cmdline_parse(స్థిరరాశి అక్షరం *cmdline, cmdline_opt_t *cmdline_opt)
{
#ఒకవేళ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* use BIOS data area to read serial configuration. The BDA is not
     * fully standardized and parts are absolete. See http://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
     * for an explanation
     */
    /*
     * gcc-12 gives array bounds warning (treated as errors by the
     * build system) for any address below 4096.  Turn that off for
     * accessing the BIOS area.
     */
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Warray-bounds"

    స్థిరరాశి గుర్తులేని పొట్టి *bda_పోర్టు = (గుర్తులేని పొట్టి *)0x400;
    స్థిరరాశి గుర్తులేని పొట్టి *bda_equi = (గుర్తులేని పొట్టి *)0x410;
    పూర్ణాంకం స్థిరరాశి bda_ports_లెక్క       = (*bda_equi >> 9) & 0x7;
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_పోర్టు = bda_ports_లెక్క && *bda_పోర్టు ? *bda_పోర్టు : 0x3f8;

    ఒకవేళ (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_పోర్టు, 1);
    }

    /* initialise console ports to enable debug output */
    ఒకవేళ (cmdline_opt->console_పోర్టు) {
        serial_init(cmdline_opt->console_పోర్టు);
        x86_KSconsole_పోర్టు = cmdline_opt->console_పోర్టు;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    రాయి("\nBoot config: parsing cmdline '%s'\n", cmdline);

    ఒకవేళ (cmdline_opt->console_పోర్టు) {
        రాయి("Boot config: console_port = 0x%x\n", cmdline_opt->console_పోర్టు);
    }
#అయితే_ముగింపు

#ఒకవేళ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_పోర్టు = bda_ports_లెక్క && *bda_పోర్టు ? *bda_పోర్టు : 0x3f8;
    ఒకవేళ (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_పోర్టు, 1);
    }

    /* initialise debug ports */
    ఒకవేళ (cmdline_opt->debug_పోర్టు) {
        serial_init(cmdline_opt->debug_పోర్టు);
        x86_KSdebug_పోర్టు = cmdline_opt->debug_పోర్టు;
        రాయి("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_పోర్టు);
    }
#సూచన GCC diagnostic తీయి
#అయితే_ముగింపు

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    రాయి("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
