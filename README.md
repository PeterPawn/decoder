# Decode secrets from/for AVM's FRITZ!OS-based devices

This is a new version, which works independently of any binary decoding utility from vendor like 'allcfgconv', 'webdavcfginfo' and even from the 'partial import exploit', used by the tools from M.Engelke.

### Purpose(s):
- decode encrypted credentials from any configuration file for system extensions contained in projects like Freetz
- decode data from an exported settings file, where the export password is known
- decode internal files from a foreign device, e.g. extracted from a TFFS dump

### Prerequisites:
All shell scripts in this project need an OpenSSL binary with
* MD5 digest and
* AES-256 decryption
capabilities.
Every call to ```openssl``` is encapsulated by the ```crypto``` shell script. If you want to use any other crypto solution, you have to change the calls there.

YOU SHOULD REALLY USE THE C PROGRAM (see below), AS LONG AS YOU AREN'T INTERESTED IN INVESTIGATIONS OF THE USED ALGORITHMS.

To generate a password for decoding of data from a TFFS file, you need to have access to the device in question or you need to know the following data for the source device:
* the serial number, as it was stored in the 'urlader environment' - it was a sequence of 16 '0'es for a long time, but newer models seem to have the serial number from the back of the device stored in this environment
* the MAC address stored as 'maca' in the 'urlader environment'
* the WLAN key from factory settings - locate it on the back of the device, where it was printed on a sticker
* the TR-069 passphrase, if the device has a CWMP account configured (tr069_serial and tr069_passphrase are present in the environment)

If data is exported without a password (using 'tr069fwupdate configexport'), the password for the export file will be built from the same values as above, but the WLAN key and TR-069 passphrase will always be omitted while building this MD5 hash.

For each export file, the specified (or implicit) password is only used to encrypt a random value, which will be stored under "Password" in the header of an export file. Every encoded value within the export file uses this random value as key. It looks a little bit strange, but this random value is encoded with a length of 32 byte, while only the first 16 byte are used for encryption/decryption and the 2nd 16 byte are simply a repetition of the 1st 16 byte. If a password was provided while exporting the data, its MD5 hash value is used as the key to encrypt the random key and if the password was omitted, the hash is built from the 'SerialNumber' (with newline at the end) and the 'maca' content (a newline at its end is needed too).

If you want to decode an export file, which was created with a password, you need to know exactly this password to decode any data, because the random encryption key from the "Password" field in its header can't be decoded without it (it uses a strong AES-256 encryption). If the data was exported without a password, you need the device, where the file was created on or you need to know the 'SerialNumber' and 'maca' values of the source device.

The C implementation uses Nettle (version 3.3) now to be built and any C library - it should work with 'glibc' and 'uClibc' without problems. If any changes are needed to use 'dietlibc', your pull requests are very welcome. If you want to get a dynamically linked binary, you may use OpenSSL's crypto library instead of Nettle.

### License changes and limitations:
It's not allowed any longer to create a 'lean & mean' version (without comments and/or copyright notices) for other projects from any script or any other source file in this project after the 'v0.2_freeze' branch.

There is an exhaustive description (http://www.ip-phone-forum.de/showthread.php?t=295101 - sorry, it's only in german language), how AVM's encryption works. It will be moved soon to GitHub Pages in this repository, because the IPPF forum meanwhile changed its software (it's Xenforo now) and the new one doesn't support all of the used (and needed) BBCODE tags.

### Integration into a Freetz build
This project is available now from Freetz trunk with another name ... ```decrypt-fritzos-cfg```. This integration uses a slightly different configuration (with an own ```Makefile```), but I'll keep my versions of ```Config.in``` and ```decoder.mk``` (in the root of the project) as a boiler-plate for other toolchains (only the symbol names need usually a change).

### Provided files:
The whole project consists of POSIX-compatible shell scripts for various decoding tasks. Encoding of values is not provided here - AVM's components accept clear-text values in nearly all places, where an encrypted value may be used. Because the shell-based decryption isn't very fast (that's a little bit of understatement ... it's really, really, really slow) and the original firmware from vendor doesn't contain the needed OpenSSL binary, it's a possible, alternative approach to use an own C program for decryption. This binary can use the existing OpenSSL libraries from stock firmware.

The ```src``` subfolder contains an implementation of a multi-call utility in C, which provides all the functions (and some additionals too), that are available as shell scripts too. But I would advise you to use a compiled version - it's lightning fast, compared to the shell versions.

It's possible, that you may find some pre-compiled, statically linked binaries (even for your platform?) in the ```bin``` directory. I tend to provide/upload some files, which I myself need and use regularly. If you want to use any of these files, you should check the detached GPG signature(s) seriously.

### Discussions/questions/changes

There is a thread (with a description in german language, but questions in english are accepted too) regarding this project at

http://www.ip-phone-forum.de/showthread.php?t=295101

Please contemplate to use this forum, if you've any questions.
