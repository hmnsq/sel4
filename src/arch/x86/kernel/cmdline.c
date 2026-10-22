/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <arch/kernel/cmdline.h>
#अन्तर्भाव <arch/kernel/boot_sys.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine/io.h>

/* 'cmdline_val' is declared globally because of a C-subset restriction.
 * It is only used in cmdline_parse(), which therefore is non-reentrant.
 */
#परिभाषय MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
वर्ण cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
ध्रुव वर्ण cmdline_str_max_अङ्कः_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
ध्रुव वर्ण cmdline_str_अङ्कः_sh_चौकटाः[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
ध्रुव वर्ण cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

स्थिर पूर्णाङ्क is_space(वर्ण c)
{
    प्रत्यागम c <= ' ';
}

स्थिर पूर्णाङ्क अप्रयुक्तम् parse_opt(ध्रुव वर्ण *cmdline, ध्रुव वर्ण *opt, वर्ण *मानम्, पूर्णाङ्क bufsize)
{
    पूर्णाङ्क len = -1;
    ध्रुव वर्ण *optptr = रिक्त;

    यावत् (true) {
        प्रति (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        यदि (*cmdline == 0) {
            भङ्ग;
        }

        प्रति (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        यदि (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            प्रति (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                मानम्[len] = *cmdline;
            }
            यदि (bufsize) {
                मानम्[len] = '\0';
            }
        }
        प्रति (; !is_space(*cmdline); cmdline++);
    }

    प्रत्यागम len;
}

स्थिर पूर्णाङ्क parse_bool(ध्रुव वर्ण *cmdline, ध्रुव वर्ण *opt)
{
    ध्रुव वर्ण *optptr = रिक्त;

    यावत् (1) {
        प्रति (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        यदि (*cmdline == 0) {
            प्रत्यागम 0;
        }

        प्रति (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        यदि (*optptr == '\0' && is_space(*cmdline)) {
            प्रत्यागम 1;
        } अन्यथा {
            प्रति (; !is_space(*cmdline); cmdline++);
        }
    }
}

स्थिर शून्य अप्रयुक्तम् parse_uint16_array(वर्ण *str, uint16_t *array, पूर्णाङ्क array_आकार)
{
    वर्ण *चरमः;
    पूर्णाङ्क   i = 0;
    पूर्णाङ्क   v;

    यावत् (str && i < array_आकार) {
        प्रति (चरमः = str; *str && *str != ','; str++);
        यदि (*str == 0) {
            str = 0;
        } अन्यथा {
            *str = 0;
            str++;
        }
        v = str_गन्तव्यम्_long(चरमः);
        यदि (v == -1) {
            array[i] = 0;
        } अन्यथा {
            array[i] = v;
        }
        i++;
    }
}

शून्य cmdline_parse(ध्रुव वर्ण *cmdline, cmdline_opt_t *cmdline_opt)
{
#यदि defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* use BIOS data area to read serial configuration. The BDA is not
     * fully standardized and parts are absolete. See http://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
     * for an explanation
     */
    /*
     * gcc-12 gives array bounds warning (treated as errors by the
     * build system) for any address below 4096.  Turn that off for
     * accessing the BIOS area.
     */
#निर्देशः GCC diagnostic प्रक्षिप
#निर्देशः GCC diagnostic ignored "-Warray-bounds"

    ध्रुव अचिह्नित ह्रस्व *bda_पत्तनम् = (अचिह्नित ह्रस्व *)0x400;
    ध्रुव अचिह्नित ह्रस्व *bda_equi = (अचिह्नित ह्रस्व *)0x410;
    पूर्णाङ्क ध्रुव bda_ports_गणनम्       = (*bda_equi >> 9) & 0x7;
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_पत्तनम् = bda_ports_गणनम् && *bda_पत्तनम् ? *bda_पत्तनम् : 0x3f8;

    यदि (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_पत्तनम्, 1);
    }

    /* initialise console ports to enable debug output */
    यदि (cmdline_opt->console_पत्तनम्) {
        serial_init(cmdline_opt->console_पत्तनम्);
        x86_KSconsole_पत्तनम् = cmdline_opt->console_पत्तनम्;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    प्रारूप_लिख("\nBoot config: parsing cmdline '%s'\n", cmdline);

    यदि (cmdline_opt->console_पत्तनम्) {
        प्रारूप_लिख("Boot config: console_port = 0x%x\n", cmdline_opt->console_पत्तनम्);
    }
#यदि_अन्तः

#यदि defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_पत्तनम् = bda_ports_गणनम् && *bda_पत्तनम् ? *bda_पत्तनम् : 0x3f8;
    यदि (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_पत्तनम्, 1);
    }

    /* initialise debug ports */
    यदि (cmdline_opt->debug_पत्तनम्) {
        serial_init(cmdline_opt->debug_पत्तनम्);
        x86_KSdebug_पत्तनम् = cmdline_opt->debug_पत्तनम्;
        प्रारूप_लिख("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_पत्तनम्);
    }
#निर्देशः GCC diagnostic निष्कासय
#यदि_अन्तः

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    प्रारूप_लिख("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
