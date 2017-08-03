# Decode secrets from/for AVM's FRITZ!OS-based devices

This is a new version, which works independently of any other utility (from vendor). It's working even outside of any FRITZ!OS device, as long as you known the used password (for exported settings files) or some properties (see below) of the source device (for other files).

## Purpose(s):
- decode encrypted credentials from any configuration file for system extensions contained in projects like ```Freetz``` or my own ```modfs```
- decode data from an exported settings file, where the export password is known
- decode internal files from a foreign device, e.g. extracted from a TFFS dump
- split export files into the contained settings files
- recompute and check/change the CRC32 checksum at the very end of export files
- encode/decode Base32 (with AVM's character set), Base64 and hexadecimal representations from/to raw binary data (the BusyBox project provides only a Base64 implementation)

Encoding of values is not provided yet - AVM's components accept clear-text values in nearly all places, where an encrypted value may be used.

## Provided files:

<span style="color:red">YOU SHOULD REALLY USE THE C PROGRAM (see below), IF YOUR MAIN INTEREST ISN'T ADDITIONAL RESEARCH.</span>

There are some shell scripts (in the ```scripts``` subfolder), which may be used on any device with a POSIX-compatible shell interpreter and an ```OpenSSL``` (CLI) binary, which supports AES-256 decryption and MD5 digests.

These files are intended to show the principles, how AVM's encryption works and their usage in a 'production environment' should be limited to situations, where no binary ```decoder``` utility is available (and also can't be built with small efforts).

But I expect, the most users will be interested in the C version, which can be found in the ```src``` subfolder. It uses the Nettle implementation (https://www.lysator.liu.se/~nisse/nettle/) for AES-256 and MD5 and creates a statically linked binary. This binary file has only to be copied to the target system and doesn't need any other file(s) - beside the data to be handled.

Compared to the shell scripts, the compiled version is lightning fast and the 'complete' binary with all applets provides some additional features not present as shell script.

It's a 'multi-call binary' and the operation to perform has to be specified as first parameter on the calling command-line or is derived from the name of the symbolic link used to invoke the program.

Which symbolic links are understood and created automatically (during an installation with ```make install```), has to be configured in the ```Makefile``` from ```src``` subfolder ... you have to edit this ```Makefile``` with a text editor yet, an easier configuration with ```Kconfig``` is planned as a future enhancement.

It's possible, that you may find some pre-compiled, statically linked binaries (even for your platform?) in the ```bin``` subfolder. I tend to provide/upload the files for some platforms that I regularly need and use myself. If *you* want to use any of these files, you should check the detached GPG signature(s) *seriously* - my public key can be found in ```PeterPawn.asc``` in this repository.

## Why isn't this a 'cracker'?
This project can only decrypt data, if some properties of the source device are known (see below) or the password used for an export is available. While it may be used as a fast way to determine, if a password is correct (and in the last consequence for a 'brute-force attack' on files with unknown export password), the original firmware from AVM may be used for this purpose already. The ```tr069fwupdate``` utility from AVM has a ```check_configimport``` mode, which will fail immediately (and set an appropriate exit code), if the provided password is a wrong one.

As long as the export password for a file was strong enough, this project will not (additionally) weaken the security of any data from or on a FRITZ!OS-based device. But it may serve as an example, how easy it is to steal secret data from a FRITZ!OS device, if an attacker gets access to the TFFS storage of such a device (where all settings are stored) and is able to extract its 'raw' content. This can already be done with a single '(remote) command execution' vulnerability and is another reason, why really **each** known vulnerability needs to be fixed *as fast as possible*. Even if a vulnerability is only usable from the LAN side of a router ... this is also 'remote' to the router itself and meanwhile even the LAN side of an edge router isn't always a 'friendly area'.

## Which properties of a FRITZ!OS device are important?
To generate a password for decoding of data from a TFFS file, you need to have access to the device in question or you need to know the following data for the source device:
* the serial number, as it was stored in the ```urlader environment``` - it was a sequence of 16 '0's for a long time, but newer models seem to have the serial number from the back of the device stored in this environment
* the MAC address stored as ```maca``` in the ```urlader environment```
* the WLAN key from factory settings - locate it on the back of the device, where it was printed on a sticker or read it from the ```urlader environment``` (value name is ```wlan_key```)
* the TR-069 passphrase, if the device has a CWMP account configured (```tr069_serial``` and ```tr069_passphrase``` are present in the environment)

If data is exported without a password (using ```tr069fwupdate configexport```), the password for the export file will be built from the same values as above, but the WLAN key and TR-069 passphrase will always be omitted, while the MD5 hash will be built (this hash will be used then as encryption password).

For each export file, the specified (or implicit) password is only used to encrypt a random value, which will be stored under ```Password``` in the header of an export file. Every encoded value within the file uses this random value as key.

It looks a little bit strange, but this random value is encoded with a length of 32 bytes, while only the first 16 bytes are used for encryption/decryption and the second 16 bytes are simply a repetition of the first ones.

If a password was provided while exporting the data, its MD5 hash value is used as the key to encrypt the random key and if the password was omitted, the hash is built from the  ```SerialNumber``` (with newline at the end) and the ```maca``` content (and another newline at its end).

If you want to decode an export file, which was created with a password, you need to know exactly this password to decode any data, because the random encryption key from the ```Password``` field in its header can't be decoded without it (it uses a strong AES-256 encryption). If the data was exported without a password, you need the device, where the file was created on or you need to know the ```SerialNumber``` and ```maca``` values of the source device.

## Prerequisites/Installation:
### Shell scripts:
There's a bunch of POSIX-compatible shell scripts for various decoding tasks - take them as 'proof-of-concept' please and don't expect too much, regarding their execution speed.

All shell scripts in the ```scripts``` subfolder need an OpenSSL binary with
* MD5 digest and
* AES-256 decryption
capabilities.

Every call to ```openssl``` itself is encapsulated by the ```crypto``` shell script. If you want to use any other crypto solution, you have only to change the calls there.

To install the shell scripts, you have to copy them to any location ... that's all, as long as the ```openssl``` command-line binary can be found on the system.

You may download a copy of the repository as a ```zip``` or ```gz``` file (for 'release' v0.3) from GitHub and than extract only the ```scripts``` subfolder from it or you may clone the whole repository (this will be a larger transfer with the binary files from the bin subfolder).

If you've cloned it as a whole, you may use ```make TARGET=<directory> install-scripts``` to install shell scripts and symbolic links (from the ```Makefile``` in the ```scripts``` subfolder) to your prefered location (replace ```<directory>``` with the real destination folder). But this needs a ```make``` version on your system, which is usually missing on a FRITZ!OS device.

Beware: *Installation of shell scripts with the provided ```Makefile``` isn't a part of the 'released version' 0.3.*

### C program:
The **C implementation** uses ```Nettle``` (version 3.3) and any C library - it should work with ```glibc``` and ```uClibc``` without problems. If any changes are needed to use ```dietlibc```, your pull requests are very welcome.

If you want to get a dynamically linked binary, you may use OpenSSL's crypto library instead of Nettle.

To create your own binary (supposed you have already a functioning ```gcc``` and ```autoconf``` installation), you need to clone this repository to your system first:

```git clone https://github.com/PeterPawn/decode_passwords.git```

If you want to customize some settings from the ```Makefile``` prior to building the binary, you have now the chance to edit this file. Afterwards you can simply call ```make``` to build a 'native' version for your system.

If you want to create a 32-bit version from a 64-bit host system, specify ```-m32``` for both ```CFLAGS``` and ```LDFLAGS```:

```make CFLAGS="-m32" LDFLAGS="-m32"```

If you want to create a dynamically linked binary, add ```STATIC=n``` to your ```make``` call. This will create a binary, where the ```libnettle``` functions are statically included, but the C library will be loaded dynamically at runtime.

If you want to use OpenSSL's ```libcrypto``` instead of ```libnettle```, you can specify ```OPENSSL=y``` - this will automatically build a dynamically linked binary, because a default ```libcrypto``` library would produce really huge binaries, if used for static linking.

### Integration into a ```Freetz``` build
This project is available from ```Freetz``` trunk with another name ... as ```decrypt-fritzos-cfg```.

This integration uses a slightly different configuration (with an own ```Makefile```), but I'll keep my versions of ```Config.in``` and ```decoder.mk``` (in the root of the project) as a boiler-plate for other toolchains (only the symbol names need usually a change).

To install the new binary and create symbolic links for included applets, call ```make install```. The default install location is ```$HOME/bin``` - to change it, you can specify ```bindir=<directory>``` with the install request (or edit the ```Makefile``` in a step above).

## License changes and limitations:
It's not allowed any longer to create a *lean & mean* version (without comments and/or copyright notices) for other projects from any script or any other source file in this project after the ```v0.2_freeze``` branch.

## Discussions/questions/changes

There is an exhaustive description (http://www.ip-phone-forum.de/showthread.php?t=295101 - sorry, it's only in german language), how AVM's encryption works. It will be moved soon to ```GitHub Pages``` in this repository, because the IPPF forum meanwhile changed its software (it uses Xenforo now) and the new one doesn't support all of the used (and needed) BBCODE tags.

Please contemplate to use the IPPF forum, if you've any questions and use ```GitHub Issues``` for this project only for real issues/errors in the code.
