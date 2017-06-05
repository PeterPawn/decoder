**crypto**, version 0.3, from decode_passwords                                                          
                                                                                                    
This script is a part of the project from https://github.com/PeterPawn/decode_passwords.            
                                                                                                    
Copyright (C) 2014-2017 P.Haemmerlein (peterpawn@yourfritz.de)                                      
                                                                                                    
This project is free software, licensed under a slightly modified GPL version; most people          
can redistribute it and/or modify it under the terms of the GNU General Public License as           
published by the Free Software Foundation; either version 2 of the License, or (at your             
option) any later version.                                                                          
                                                                                                    
This project is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;           
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.           
See the GNU General Public License under http://www.gnu.org/licenses/gpl-2.0.html for more          
details.                                                                                            

**Purpose:**

This script is only a wrapper to call the OpenSSL binary and it provides some customized
YourFritz script library functions (in a single place) for the other scripts.

**Usage:**

    __crypto__ [ _options_ ] _function_ [ _parameters_ ]

Supported *options* are:

-d, --debug   - display debug info on STDERR; must prefix all other options, if used
-h, --help    - show this information (must be the first option)
-V, --version - show version and exit (must be the first option)

The following *functions* are implemented:

**aes_decrypt**

    The hexadecimal strings from STDIN are decrypted with the specified *key* (it's the
    first argument) and *iv* (the second argument, both specified as hexadecimal strings)
    and the result is written to STDOUT as a hexadecimal string.

**digest**

    The MD5 digest value for all data from STDIN is written to STDOUT as a hexadecimal
    string. If an option '-x' is specified for *parameters*, data from STDIN is assumed
    to be a hexadecimal string and it gets converted to binary first.

**b32dec**

    The Base32 encoded string from STDIN is converted and written as a hexadecimal
    string to STDOUT.

**hexdec**

    The hexadecimal string from STDIN is converted to its binary content and the result
    is written to STDOUT (hexadecimal to binary).

**hex2dec**

    The hexadecimal string from STDIN is converted to its decimal value (as a string)
    on STDOUT (hexadecimal to decimal).

**mktemp**

    Creates a temporary file or directory with a unique name - some platforms don't
    have another *mktemp* utility.
