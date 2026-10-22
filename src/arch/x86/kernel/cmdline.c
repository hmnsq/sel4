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
#define MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
अक्षर cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
स्थिरांक अक्षर cmdline_str_max_अंक_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
स्थिरांक अक्षर cmdline_str_अंक_sh_frames[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
स्थिरांक अक्षर cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

स्थिर पूर्णांक is_space(अक्षर c)
{
    परत c <= ' ';
}

स्थिर पूर्णांक UNUSED parse_opt(स्थिरांक अक्षर *cmdline, स्थिरांक अक्षर *opt, अक्षर *मूल्य, पूर्णांक bufsize)
{
    पूर्णांक len = -1;
    स्थिरांक अक्षर *optptr = रिक्त;

    पर्यंत (true) {
        साठी (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        जर (*cmdline == 0) {
            थांबा;
        }

        साठी (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        जर (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            साठी (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                मूल्य[len] = *cmdline;
            }
            जर (bufsize) {
                मूल्य[len] = '\0';
            }
        }
        साठी (; !is_space(*cmdline); cmdline++);
    }

    परत len;
}

स्थिर पूर्णांक parse_bool(स्थिरांक अक्षर *cmdline, स्थिरांक अक्षर *opt)
{
    स्थिरांक अक्षर *optptr = रिक्त;

    पर्यंत (1) {
        साठी (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        जर (*cmdline == 0) {
            परत 0;
        }

        साठी (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        जर (*optptr == '\0' && is_space(*cmdline)) {
            परत 1;
        } अन्यथा {
            साठी (; !is_space(*cmdline); cmdline++);
        }
    }
}

स्थिर शून्य UNUSED parse_uint16_array(अक्षर *str, uint16_t *array, पूर्णांक array_size)
{
    अक्षर *last;
    पूर्णांक   i = 0;
    पूर्णांक   v;

    पर्यंत (str && i < array_size) {
        साठी (last = str; *str && *str != ','; str++);
        जर (*str == 0) {
            str = 0;
        } अन्यथा {
            *str = 0;
            str++;
        }
        v = str_to_long(last);
        जर (v == -1) {
            array[i] = 0;
        } अन्यथा {
            array[i] = v;
        }
        i++;
    }
}

शून्य cmdline_parse(स्थिरांक अक्षर *cmdline, cmdline_opt_t *cmdline_opt)
{
#जर defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* use BIOS data area to read serial configuration. The BDA is not
     * fully standardized and parts are absolete. See http://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
     * for an explanation
     */
    /*
     * gcc-12 gives array bounds warning (treated as errors by the
     * build system) for any address below 4096.  Turn that off for
     * accessing the BIOS area.
     */
#pragma GCC diagnostic ढकला
#pragma GCC diagnostic ignored "-Warray-bounds"

    स्थिरांक अचिन्हित लघु *bda_port = (अचिन्हित लघु *)0x400;
    स्थिरांक अचिन्हित लघु *bda_equi = (अचिन्हित लघु *)0x410;
    पूर्णांक स्थिरांक bda_ports_मोजणी       = (*bda_equi >> 9) & 0x7;
#endif

#ifdef CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_port = bda_ports_मोजणी && *bda_port ? *bda_port : 0x3f8;

    जर (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_port, 1);
    }

    /* initialise console ports to enable debug output */
    जर (cmdline_opt->console_port) {
        serial_init(cmdline_opt->console_port);
        x86KSconsolePort = cmdline_opt->console_port;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    म_लिखो("\nBoot config: parsing cmdline '%s'\n", cmdline);

    जर (cmdline_opt->console_port) {
        म_लिखो("Boot config: console_port = 0x%x\n", cmdline_opt->console_port);
    }
#endif

#जर defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_port = bda_ports_मोजणी && *bda_port ? *bda_port : 0x3f8;
    जर (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_port, 1);
    }

    /* initialise debug ports */
    जर (cmdline_opt->debug_port) {
        serial_init(cmdline_opt->debug_port);
        x86KSdebugPort = cmdline_opt->debug_port;
        म_लिखो("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_port);
    }
#pragma GCC diagnostic काढा
#endif

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    म_लिखो("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
