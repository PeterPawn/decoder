#! /bin/sh
# SPDX-License-Identifier: GPL-2.0-or-later
#
# create an export file with an encrypted binary file
#
# MUST BE EXECUTED ON A FRITZ!OS DEVICE
#
# COMMENT: It looks like AVM isn't using the encryption anymore for "dvb.cfg",
#          starting with FRITZ!OS version 07.19. At least the file is exported
#          on my 7490 device (with 113.07.19-73513) without encryption, as a 
#          normal B64FILE entry.
# UPDATE:  Even the labor version for 6490 (141.07.19-75251) does not use an
#          encryption anymore for 'dvb.cfg' file. The whole 'CRYPTEDB64FILE'
#          support is based on assumptions from the earlier 'CRYPTEDBINFILE'
#          implementation.
#
# remove the character device for an existing dvb.cfg file
#
rm /var/flash/dvb.cfg 2>/dev/null
#
# create our own file
#
cat >/var/flash/dvb.cfg <<'EOT'
/*
 * /var/flash/dvb.cfg
 * Sun May 14 20:57:35 2017
 */

meta { encoding = "utf-8"; }

version_dvb {
        revision = "$Revision: 1.0 $";
        creatversion = "1.00.00";
}


dvbcfg {
        nit_pid = 16;
        network_id = 61444;
        network_name = "Kabel Deutschland";
        bouquets = 32;
        tv_services = 101;
        radio_services = 67;
        other_services = 0;
        channels {
                sid = 53019;
                tsid = 10008;
                frequency = 442000000;
                lcn_nordig = 0;
                lcn_eacem = 0;
                flags = 32;
                url = 239.2.1.1;
                provider = "";
                name = "53019";
                type = 0;
                ca_mode = 0;
                pid_pmt = 220;
                pid_pcr = 0;
                pid_cnt = 0;
       }
}


// EOF
EOT
#
# declare TV functions active
#
export CONFIG_LINEARTV=y
#
# export the data with or without password
#
# ATTENTION:
#
# It looks like AVM disabled (accident or not, that's the question) the ability
# to export data without a password, since 2FA was added to the firmware.
# Exporting a file without password is not functioning on any of my 06.83 devices.
#
# We strip off all lines around the encrypted content.
#
tr069fwupdate configexport $1 | sed -n -e "/\*\*\*\* CRYPTEDB\(IN\|64\)FILE/,/\*\*\*\* END OF FILE/p" | sed -e "1d;\$d"
#
# data should be visible on STDOUT now
#
