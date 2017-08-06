### 'ready to use' files
This folder contains some pre-built binaries, all of them are linked statically and it should be possible to execute them without any further precautions, after you've copied the file for your platform to the target system and made it executable (with ```chmod```). If you want to use a file on a regular base, you should create symbolic links for the wanted applets.

Please don't forget to check the detached GnuPG signature, provided together with each binary file. You can find my public key (```KeyID 0x30311D96```) on ```keys.gnupg.net``` or as file ```PeterPawn.asc``` in the root folder of this project.

The files provided and their target platforms are:

* decoder.armv7
  - built with ```libnettle``` and ```glibc``` on a Raspberry Pi (3B) running Raspian (Jessie)
  - it's a little bit larger with glibc, but I had no usable ```uClibc``` for Raspian and there's no other need to build one
* decoder.mips32r2
  - built for ```big endian``` machines with ```libnettle``` and ```uClibc``` using the ```Freetz``` toolchain
  - really small for a statically linked program, thanks to the used C library
  - should be usable on all FRITZ!Box models with VR9 and GRX350 chipset, *should* work on Vx180 (7390) too (but that's untested)
* decoder.x86
  - built with ```libnettle``` and ```glibc```, targeting Intel 80386 compatible systems
  - somewhat larger due to the usage of ```glibc```
* decoder.x86_64
  - built with ```libnettle``` and ```glibc```, targeting x86 compatible systems
  - tested also with (Ubuntu) ```bash``` on a Windows 10 (x64) installation
  - somewhat larger due to the usage of ```glibc```
