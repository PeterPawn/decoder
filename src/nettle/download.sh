#! /bin/bash
#
# variables
#
keyID=28C67298
keyServer=hkp://keys.gnupg.net:11371
keyFingerprint="343C 2FF0 FBEE 5EC2 EDBE  F399 F359 9FF8 28C6 7298"
server="https://ftp.gnu.org/gnu/nettle/"
project="nettle-3.3"
source="$project.tar.gz"
sig="$source.sig"
#
# constants
#
hdir=.gnupg
export LANG=en-US.UTF-8
#
# check GnuPG presence
#
if ! gpg --version | grep -q GnuPG; then
	printf "Missing GnuPG, it's a prerequisite for this script.\n" 1>&2
	exit 1
fi
#
# prepare a new base directory and load author's key
#
rm -r $hdir 2>/dev/null
omask=$(umask)
umask 077
mkdir $hdir
gpg --homedir $hdir --keyserver $keyServer --recv-keys $keyID >/dev/null 2>&1
if ! [ "$(gpg --homedir $hdir --fingerprint $keyID | sed -n -e 's/.* fingerprint = \(.*\)/\1/p')" = "$keyFingerprint" ]; then
	printf "GPG key fingerprint does not match, maybe the key wasn't found or it's the wrong key.\n" 1>&2
	rmdir -r .gnupg
	exit 1
fi
umask $omask
#
# get source tarball and check the signature
#
rm $source $sig 2>/dev/null
rm -r ${project} 2>/dev/null
wget $server$source
wget $server$sig
#
# check signature
#
if ! gpg --homedir $hdir --verify $sig $source >/dev/null 2>&1; then
	printf "Error verifying signature - either a download problem or the signature is really wrong.\n" 1>&2
	exit 1
fi
#
# unpack tarball
#
tar -x -f $source
#
# downloaded and unpacked
#
