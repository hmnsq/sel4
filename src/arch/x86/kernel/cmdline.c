/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <util.h>
#ضمن <machine/io.h>
#ضمن <arch/kernel/cmdline.h>
#ضمن <arch/kernel/boot_sys.h>
#ضمن <linker.h>
#ضمن <plat/machine/io.h>

/* 'cmdline_val' is declared globally because of a C-subset restriction.
 * It is only used in cmdline_parse(), which therefore is non-reentrant.
 */
#وصف_ڪريو MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
حرف cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
ثابت حرف cmdline_str_max_انگ_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
ثابت حرف cmdline_str_انگ_sh_چوکٺا[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
ثابت حرف cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

ساكن صحيح is_space(حرف c)
{
    أرجع c <= ' ';
}

ساكن صحيح اڻ_استعمال_ٿيل parse_opt(ثابت حرف *cmdline, ثابت حرف *opt, حرف *قدر, صحيح bufsize)
{
    صحيح len = -1;
    ثابت حرف *optptr = عدم;

    طالما (true) {
        لكل (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        إذا (*cmdline == 0) {
            اكسر;
        }

        لكل (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        إذا (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            لكل (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                قدر[len] = *cmdline;
            }
            إذا (bufsize) {
                قدر[len] = '\0';
            }
        }
        لكل (; !is_space(*cmdline); cmdline++);
    }

    أرجع len;
}

ساكن صحيح parse_bool(ثابت حرف *cmdline, ثابت حرف *opt)
{
    ثابت حرف *optptr = عدم;

    طالما (1) {
        لكل (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        إذا (*cmdline == 0) {
            أرجع 0;
        }

        لكل (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        إذا (*optptr == '\0' && is_space(*cmdline)) {
            أرجع 1;
        } وإلا {
            لكل (; !is_space(*cmdline); cmdline++);
        }
    }
}

ساكن فراغ اڻ_استعمال_ٿيل parse_uint16_array(حرف *str, uint16_t *array, صحيح array_سائيز)
{
    حرف *آخري;
    صحيح   i = 0;
    صحيح   v;

    طالما (str && i < array_سائيز) {
        لكل (آخري = str; *str && *str != ','; str++);
        إذا (*str == 0) {
            str = 0;
        } وإلا {
            *str = 0;
            str++;
        }
        v = str_منزل_long(آخري);
        إذا (v == -1) {
            array[i] = 0;
        } وإلا {
            array[i] = v;
        }
        i++;
    }
}

فراغ cmdline_parse(ثابت حرف *cmdline, cmdline_opt_t *cmdline_opt)
{
#إذا defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* use BIOS data area to read serial configuration. The BDA is not
     * fully standardized and parts are absolete. See http://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
     * for an explanation
     */
    /*
     * gcc-12 gives array bounds warning (treated as errors by the
     * build system) for any address below 4096.  Turn that off for
     * accessing the BIOS area.
     */
#هدايت GCC diagnostic ڌڪو
#هدايت GCC diagnostic ignored "-Warray-bounds"

    ثابت بنانشان ننڍو *bda_پورٽ = (بنانشان ننڍو *)0x400;
    ثابت بنانشان ننڍو *bda_equi = (بنانشان ننڍو *)0x410;
    صحيح ثابت bda_ports_ڳڻپ       = (*bda_equi >> 9) & 0x7;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_پورٽ = bda_ports_ڳڻپ && *bda_پورٽ ? *bda_پورٽ : 0x3f8;

    إذا (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_پورٽ, 1);
    }

    /* initialise console ports to enable debug output */
    إذا (cmdline_opt->console_پورٽ) {
        serial_init(cmdline_opt->console_پورٽ);
        x86_KSconsole_پورٽ = cmdline_opt->console_پورٽ;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    لکو("\nBoot config: parsing cmdline '%s'\n", cmdline);

    إذا (cmdline_opt->console_پورٽ) {
        لکو("Boot config: console_port = 0x%x\n", cmdline_opt->console_پورٽ);
    }
#جيڪڏهن_پورو

#إذا defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_پورٽ = bda_ports_ڳڻپ && *bda_پورٽ ? *bda_پورٽ : 0x3f8;
    إذا (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_پورٽ, 1);
    }

    /* initialise debug ports */
    إذا (cmdline_opt->debug_پورٽ) {
        serial_init(cmdline_opt->debug_پورٽ);
        x86_KSdebug_پورٽ = cmdline_opt->debug_پورٽ;
        لکو("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_پورٽ);
    }
#هدايت GCC diagnostic ڪڍو
#جيڪڏهن_پورو

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    لکو("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
