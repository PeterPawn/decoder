**crypto**, version 0.3, from decode_passwords<br /><br />
This script is a part of the project from https://github.com/PeterPawn/decode_passwords.<br /><br />
Copyright (C) 2014-2017 P.Haemmerlein (peterpawn@yourfritz.de)<br /><br />
This project is free software, licensed under a slightly modified GPL version; most people<br />
can redistribute it and/or modify it under the terms of the GNU General Public License as<br />
published by the Free Software Foundation; either version 2 of the License, or (at your<br />
option) any later version.<br /><br />
This project is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;<br />
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.<br />
See the GNU General Public License under http://www.gnu.org/licenses/gpl-2.0.html for more<br />
details.<br /><br />
**Purpose:**<br /><br />
This script is only a wrapper to call the OpenSSL binary and it provides some customized<br />
YourFritz script library functions (in a single place) for the other scripts.<br /><br />
**Usage:**<br /><br />
    __crypto__ [ <em>options</em> ] _function_ [ _parameters_ ]<br /><br />
Supported *options* are:<br />
<table>
<tr><td>-d, --debug</td><td>display debug info on STDERR; must prefix all other options, if used</td></tr>
<tr><td>-h, --help</td><td>show this information (must be the first option)</td></tr>
<tr><td>-V, --version</td><td>show version and exit (must be the first option)</td></tr>
</table><br />
The following *functions* are implemented:<br /><br />
**aes_decrypt**<br />
    The hexadecimal strings from STDIN are decrypted with the specified *key* (it's the<br />
    first argument) and *iv* (the second argument, both specified as hexadecimal strings)<br />
    and the result is written to STDOUT as a hexadecimal string.<br /><br />
**digest**<br />
    The MD5 digest value for all data from STDIN is written to STDOUT as a hexadecimal<br />
    string. If an option '-x' is specified for *parameters*, data from STDIN is assumed<br />
    to be a hexadecimal string and it gets converted to binary first.<br /><br />
**b32dec**<br />
    The Base32 encoded string from STDIN is converted and written as a hexadecimal<br />
    string to STDOUT.<br /><br />
**hexdec**<br />
    The hexadecimal string from STDIN is converted to its binary content and the result<br />
    is written to STDOUT (hexadecimal to binary).<br /><br />
**hex2dec**<br />
    The hexadecimal string from STDIN is converted to its decimal value (as a string)<br />
    on STDOUT (hexadecimal to decimal).<br /><br />
**mktemp**<br />
    Creates a temporary file or directory with a unique name - some platforms don't<br />
    have another *mktemp* utility.<br />
