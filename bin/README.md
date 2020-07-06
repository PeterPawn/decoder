### 'ready to use' files
This folder contains some pre-built binaries, all of them are linked statically and it should be possible to execute them without any further precautions, after you've copied the file for your platform to the target system and made it executable (with ```chmod```). If you want to use a file on a regular base, you should create symbolic links for the wanted applets.

Please don't forget to check the detached GnuPG signature, provided together with each binary file. You can find my public key (```KeyID 0x30311D96```) on ```keys.gnupg.net``` or as file ```PeterPawn.asc``` in the root folder of this project.

The files provided and their target platforms are:

* decoder.armv7l
  - built with ```libnettle``` (3.6) and ```uClibc-ng``` (1.0.14) using the ```Freetz``` toolchain (from my YourFreetz fork) for IPQ40x8/9 devices
* decoder.mips
  - built for ```big endian``` machines with ```libnettle``` (3.6) and ```uClibc-ng``` (1.0.14) using the ```Freetz``` toolchain (from my YourFreetz fork)
  - should be usable on all FRITZ!Box models with VR9 and GRX5 chipset, works on Vx180 (7390), too
* decoder.i686
  - built with ```libnettle``` (3.6) and ```uClibc-ng``` (1.0.14), targeting Intel 80386 compatible systems (Puma's ATOM and other x86 platforms)
* decoder.x86_64
  - built with ```libnettle``` (3.6) and ```glibc```, targeting x64 compatible systems
  - should be usable from WSL (Windows Subsystem for Linux) on Windows 10
  - somewhat larger due to the usage of ```glibc```
