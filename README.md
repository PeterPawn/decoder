# Decode secrets from/for AVM's FRITZ!OS-based devices

This is a new version, which works independently of any binary decoding utility from vendor like 'allcfgconv', 'webdavcfginfo' and even from the 'partial import exploit', used by the tools from M.Engelke.

### Purpose(s):
- decode encrypted credentials from any configuration file for system extensions contained in projects like Freetz
- decode data from an exported settings file, where the export password is known
- decode internal files from a foreign device, e.g. extracted from a TFFS dump

### Prerequisites:
All scripts in this project need an OpenSSL binary with
* MD5 digest and
* AES-256 decryption
capabilities.

For recurring tasks (like data conversions) the shell script library from the YourFritz repository (www.yourfritz.de) will be used. Therefore you have to provide a valid installation of this library or you may prepare include files for each script. If a file ```*script_name*.yf_scriptlib``` exists, it will be included with the 'dot' command (.) directly. Please keep in mind, that you may not publish/distribute such a prepared file - only the whole script library (with intact comments) may be distributed to others.

To generate a password for decoding of data from a TFFS file, you need to have access to the device in question or you need to know the following data for the source device:
* the serial number, as it was stored in the 'urlader environment' - it was a sequence of 16 '0'es for a long time, but newer models seem to have the serial number from the back of the device stored in this environment
* the MAC address stored as 'maca' in the 'urlader environment' and last, but not least
* the WLAN key from factory settings - locate it on the back of the device, where it was printed on a sticker
* the TR-069 passphrase, if the device has a CWMP account configured (tr069_serial and tr069_passphrase are present in the environment)

If data is exported without a password (using 'tr069fwupdate configexport' - possibly this will not function any longer, since the 2FA was introduced for new versions), the password for the export file will be built from the same values as above, but the WLAN key and TR-069 passphrase will always be omitted while building the MD5 hash.

For each export file, the specified (or implicit) password is only used to encrypt a random value, which will be stored under "Password" in the header of an export file. Every encoded value within the export file uses this random value as key. It looks a little bit strange, but this random value is encoded with a length of 32 byte, while only the first 16 byte are used for encryption/decryption and the 2nd 16 byte are simply a repetition of the 1st 16 byte. If a password was provided while exporting the data, its MD5 hash value is used as the key to encrypt the random key and if the password was omitted, the hash is built from the 'SerialNumber' (with newline at the end) and the 'maca' content (a newline at its end is needed too).

If you want to decode an export file, which was created with a password, you need to know exactly this password to decode any data, because the random encryption key from the "Password" field in its header can't be decoded without it (it uses a strong AES-256 encryption). If the data was exported without a password, you need the device, where the file was created on or you need to know the 'SerialNumber' and 'maca' values of the source device.

The C implementation needs an OpenSSL version to be built and any C library - it should work with 'glibc' and 'uClibc' without problems. If any changes are needed to use 'dietlibc', your pull requests are very welcome.

### License changes and limitations:
It's not allowed any longer to create a 'lean & mean' version (without comments and/or copyright notices) for other projects from any script or any other source file in this project after the 'v0.2_freeze' branch.

Even if this project is licensed under the GPLv2, I'd prefer to include (justified) suggestions into the base project and therefore there's a license exception to the GPLv2 text: You may fork this project and modify it to meet your own desire.

But the Freetz project (from www.freetz.org) may only use the unmodified version from this repository. If any changes are really needed, I'll incorporate them into this project, but I don't want this project to be "occupied" again by a special Freetz developer.

There will be an exhaustive description (in a future IPPF thread), how AVM's encryption works. If the Freetz project really needs it's own version and should I deny to make the required changes to my own version, an experienced programmer should be able to create his own version from scratch.

I would like to work *together* on an usable version, but this means to work in *common* and it's not the "fine english manner" to use the project of a stranger and make own changes, without any attempts to discuss their sense (or senselessness) first.

### Provided files:
The whole project consists of POSIX-compatible shell scripts for various decoding tasks. Encoding of values is not provided here - AVM's components accept clear-text values in nearly all places, where an encrypted value may be used. Because the shell-based decryption isn't very fast (that's a little bit of understatement ... it's really, really, really slow) and the original firmware from vendor doesn't contain the needed OpenSSL binary, it's a possible, alternative approach to use an own C program for decryption. This binary can use the existing OpenSSL libraries from stock firmware.

The 'source' subfolder contains an implementation of a 'multi-call' utility in C, which provides all the functions, that are available as shell scripts too.

### Discussions/questions/changes

There is a (german) thread regarding this project at

http://www.ip-phone-forum.de/showthread.php?t=276183

Please contemplate to use this forum, if you've any questions.
