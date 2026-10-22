/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <util.h>
#समावेश <machine/io.h>
#समावेश <arch/kernel/cmdline.h>
#समावेश <arch/kernel/boot_sys.h>
#समावेश <linker.h>
#समावेश <plat/machine/io.h>

/* 'cmdline_val' is declared globally because of a C-subset restriction.
 * It is only used in cmdline_parse(), which therefore is non-reentrant.
 */
#घोषणा MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
अक्षर cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
स्थिर अक्षर cmdline_str_max_अंक_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
स्थिर अक्षर cmdline_str_अंक_sh_frames[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
स्थिर अक्षर cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

अटल पूर्णांक is_space(अक्षर c)
{
    वापस c <= ' ';
}

अटल पूर्णांक UNUSED parse_opt(स्थिर अक्षर *cmdline, स्थिर अक्षर *opt, अक्षर *मूल्य, पूर्णांक bufsize)
{
    पूर्णांक len = -1;
    स्थिर अक्षर *optptr = शून्य;

    जबतक (true) {
        क्रम (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        अगर (*cmdline == 0) {
            अवरोध;
        }

        क्रम (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        अगर (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            क्रम (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                मूल्य[len] = *cmdline;
            }
            अगर (bufsize) {
                मूल्य[len] = '\0';
            }
        }
        क्रम (; !is_space(*cmdline); cmdline++);
    }

    वापस len;
}

अटल पूर्णांक parse_bool(स्थिर अक्षर *cmdline, स्थिर अक्षर *opt)
{
    स्थिर अक्षर *optptr = शून्य;

    जबतक (1) {
        क्रम (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        अगर (*cmdline == 0) {
            वापस 0;
        }

        क्रम (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        अगर (*optptr == '\0' && is_space(*cmdline)) {
            वापस 1;
        } अन्यथा {
            क्रम (; !is_space(*cmdline); cmdline++);
        }
    }
}

अटल व्योम UNUSED parse_uint16_array(अक्षर *str, uint16_t *array, पूर्णांक array_आकार)
{
    अक्षर *last;
    पूर्णांक   i = 0;
    पूर्णांक   v;

    जबतक (str && i < array_आकार) {
        क्रम (last = str; *str && *str != ','; str++);
        अगर (*str == 0) {
            str = 0;
        } अन्यथा {
            *str = 0;
            str++;
        }
        v = str_to_long(last);
        अगर (v == -1) {
            array[i] = 0;
        } अन्यथा {
            array[i] = v;
        }
        i++;
    }
}

व्योम cmdline_parse(स्थिर अक्षर *cmdline, cmdline_opt_t *cmdline_opt)
{
#अगर defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* use BIOS data area to read serial configuration. The BDA is not
     * fully standardized and parts are absolete. See http://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
     * for an explanation
     */
    /*
     * gcc-12 gives array bounds warning (treated as errors by the
     * build system) for any address below 4096.  Turn that off for
     * accessing the BIOS area.
     */
#आशय GCC diagnostic धकेलो
#आशय GCC diagnostic ignored "-Warray-bounds"

    स्थिर अचिन्हित लघु *bda_port = (अचिन्हित लघु *)0x400;
    स्थिर अचिन्हित लघु *bda_equi = (अचिन्हित लघु *)0x410;
    पूर्णांक स्थिर bda_ports_गिनती       = (*bda_equi >> 9) & 0x7;
#पूर्ण_अगर

#अगर_घोषित CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_port = bda_ports_गिनती && *bda_port ? *bda_port : 0x3f8;

    अगर (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_port, 1);
    }

    /* initialise console ports to enable debug output */
    अगर (cmdline_opt->console_port) {
        serial_init(cmdline_opt->console_port);
        x86KSconsolePort = cmdline_opt->console_port;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    म_लिखो("\nBoot config: parsing cmdline '%s'\n", cmdline);

    अगर (cmdline_opt->console_port) {
        म_लिखो("Boot config: console_port = 0x%x\n", cmdline_opt->console_port);
    }
#पूर्ण_अगर

#अगर defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_port = bda_ports_गिनती && *bda_port ? *bda_port : 0x3f8;
    अगर (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_port, 1);
    }

    /* initialise debug ports */
    अगर (cmdline_opt->debug_port) {
        serial_init(cmdline_opt->debug_port);
        x86KSdebugPort = cmdline_opt->debug_port;
        म_लिखो("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_port);
    }
#आशय GCC diagnostic निकालो
#पूर्ण_अगर

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    म_लिखो("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
