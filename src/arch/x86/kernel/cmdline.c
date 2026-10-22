/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/cmdline.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/boot_sys.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/io.h>

/* 'cmdline_val' is declared globally because of a C-subset restriction.
 * It is only used in cmdline_parse(), which therefore is non-reentrant.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_CMDLINE_VAL_LEN 1000
BOOT_BSS
ਅੱਖਰ cmdline_val[MAX_CMDLINE_VAL_LEN];

/* workaround because string literals are not supported by C parser */
ਅਚਲ ਅੱਖਰ cmdline_str_max_ਅੰਕ_nodes[]  = {'m', 'a', 'x', '_', 'n', 'u', 'm', '_', 'n', 'o', 'd', 'e', 's', 0};
ਅਚਲ ਅੱਖਰ cmdline_str_ਅੰਕ_sh_ਚੌਖਟੇ[]  = {'n', 'u', 'm', '_', 's', 'h', '_', 'f', 'r', 'a', 'm', 'e', 's', 0};
ਅਚਲ ਅੱਖਰ cmdline_str_disable_iommu[]  = {'d', 'i', 's', 'a', 'b', 'l', 'e', '_', 'i', 'o', 'm', 'm', 'u', 0};

ਸਥਿਰ ਪੂਰਨ is_space(ਅੱਖਰ c)
{
    ਵਾਪਸ c <= ' ';
}

ਸਥਿਰ ਪੂਰਨ ਅਣਵਰਤਿਆ parse_opt(ਅਚਲ ਅੱਖਰ *cmdline, ਅਚਲ ਅੱਖਰ *opt, ਅੱਖਰ *ਮੁੱਲ, ਪੂਰਨ bufsize)
{
    ਪੂਰਨ len = -1;
    ਅਚਲ ਅੱਖਰ *optptr = ਨਿਰਾ;

    ਜਦੋਂ (true) {
        ਲਈ (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        ਜੇ (*cmdline == 0) {
            ਰੋਕੋ;
        }

        ਲਈ (optptr = opt; *optptr && *cmdline && (*cmdline != '=') && !is_space(*cmdline)
             && (*optptr == *cmdline); optptr++, cmdline++);

        ਜੇ (*optptr == '\0' && *cmdline == '=') {
            cmdline++;

            ਲਈ (len = 0; !is_space(*cmdline) && (len < bufsize - 1); cmdline++, len++) {
                ਮੁੱਲ[len] = *cmdline;
            }
            ਜੇ (bufsize) {
                ਮੁੱਲ[len] = '\0';
            }
        }
        ਲਈ (; !is_space(*cmdline); cmdline++);
    }

    ਵਾਪਸ len;
}

ਸਥਿਰ ਪੂਰਨ parse_bool(ਅਚਲ ਅੱਖਰ *cmdline, ਅਚਲ ਅੱਖਰ *opt)
{
    ਅਚਲ ਅੱਖਰ *optptr = ਨਿਰਾ;

    ਜਦੋਂ (1) {
        ਲਈ (; is_space(*cmdline) && (*cmdline != 0); cmdline++);
        ਜੇ (*cmdline == 0) {
            ਵਾਪਸ 0;
        }

        ਲਈ (optptr = opt; *optptr && *cmdline && !is_space(*cmdline) && (*optptr == *cmdline); optptr++, cmdline++);

        ਜੇ (*optptr == '\0' && is_space(*cmdline)) {
            ਵਾਪਸ 1;
        } ਨਹੀਂਤਾਂ {
            ਲਈ (; !is_space(*cmdline); cmdline++);
        }
    }
}

ਸਥਿਰ ਖਾਲੀ ਅਣਵਰਤਿਆ parse_uint16_array(ਅੱਖਰ *str, uint16_t *array, ਪੂਰਨ array_ਮਾਪ)
{
    ਅੱਖਰ *ਆਖਰੀ;
    ਪੂਰਨ   i = 0;
    ਪੂਰਨ   v;

    ਜਦੋਂ (str && i < array_ਮਾਪ) {
        ਲਈ (ਆਖਰੀ = str; *str && *str != ','; str++);
        ਜੇ (*str == 0) {
            str = 0;
        } ਨਹੀਂਤਾਂ {
            *str = 0;
            str++;
        }
        v = str_ਮੰਜ਼ਿਲ_long(ਆਖਰੀ);
        ਜੇ (v == -1) {
            array[i] = 0;
        } ਨਹੀਂਤਾਂ {
            array[i] = v;
        }
        i++;
    }
}

ਖਾਲੀ cmdline_parse(ਅਚਲ ਅੱਖਰ *cmdline, cmdline_opt_t *cmdline_opt)
{
#ਜੇ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* use BIOS data area to read serial configuration. The BDA is not
     * fully standardized and parts are absolete. See http://wiki.osdev.org/Memory_Map_(x86)#BIOS_Data_Area_.28BDA.29
     * for an explanation
     */
    /*
     * gcc-12 gives array bounds warning (treated as errors by the
     * build system) for any address below 4096.  Turn that off for
     * accessing the BIOS area.
     */
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Warray-bounds"

    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ *bda_ਪੋਰਟ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ *)0x400;
    ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ *bda_equi = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ *)0x410;
    ਪੂਰਨ ਅਚਲ bda_ports_ਗਿਣਤੀ       = (*bda_equi >> 9) & 0x7;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
    /* initialise to default or use BDA if available */
    cmdline_opt->console_ਪੋਰਟ = bda_ports_ਗਿਣਤੀ && *bda_ਪੋਰਟ ? *bda_ਪੋਰਟ : 0x3f8;

    ਜੇ (parse_opt(cmdline, "console_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->console_ਪੋਰਟ, 1);
    }

    /* initialise console ports to enable debug output */
    ਜੇ (cmdline_opt->console_ਪੋਰਟ) {
        serial_init(cmdline_opt->console_ਪੋਰਟ);
        x86_KSconsole_ਪੋਰਟ = cmdline_opt->console_ਪੋਰਟ;
    }

    /* only start printing here after having parsed/set/initialised the console_port */
    ਲਿਖੋ("\nBoot config: parsing cmdline '%s'\n", cmdline);

    ਜੇ (cmdline_opt->console_ਪੋਰਟ) {
        ਲਿਖੋ("Boot config: console_port = 0x%x\n", cmdline_opt->console_ਪੋਰਟ);
    }
#ਜੇ_ਅੰਤ

#ਜੇ defined(CONFIG_PRINTING) || defined(CONFIG_DEBUG_BUILD)
    /* initialise to default or use BDA if available */
    cmdline_opt->debug_ਪੋਰਟ = bda_ports_ਗਿਣਤੀ && *bda_ਪੋਰਟ ? *bda_ਪੋਰਟ : 0x3f8;
    ਜੇ (parse_opt(cmdline, "debug_port", cmdline_val, MAX_CMDLINE_VAL_LEN) != -1) {
        parse_uint16_array(cmdline_val, &cmdline_opt->debug_ਪੋਰਟ, 1);
    }

    /* initialise debug ports */
    ਜੇ (cmdline_opt->debug_ਪੋਰਟ) {
        serial_init(cmdline_opt->debug_ਪੋਰਟ);
        x86_KSdebug_ਪੋਰਟ = cmdline_opt->debug_ਪੋਰਟ;
        ਲਿਖੋ("Boot config: debug_port = 0x%x\n", cmdline_opt->debug_ਪੋਰਟ);
    }
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ
#ਜੇ_ਅੰਤ

    cmdline_opt->disable_iommu = parse_bool(cmdline, cmdline_str_disable_iommu);
    ਲਿਖੋ("Boot config: disable_iommu = %s\n", cmdline_opt->disable_iommu ? "true" : "false");
}
