#! /bin/bash
# SPDX-License-Identifier: GPL-2.0-or-later
#
# variables
#
NETTLE_VERSION="$1"
keyID=28C67298
keyServer=hkp://keys.gnupg.net:11371
keyFingerprint="343C 2FF0 FBEE 5EC2 EDBE  F399 F359 9FF8 28C6 7298"
server="https://ftp.gnu.org/gnu/nettle/"
project="nettle-${NETTLE_VERSION}"
source="$project.tar.gz"
sig="$source.sig"
#
# check libnettle version
#
if [ -z "$NETTLE_VERSION" ]; then
	printf "\n\033[31m\033[1mMissing NETTLE_VERSION parameter from caller.\a\033[0m\n\n" 1>&2
	exit 1
fi
#
# constants
#
hdir=.gnupg
export LANG=en-US.UTF-8
#
# check GnuPG presence
#
if ! gpg --version | grep -q GnuPG; then
	printf "\n\033[31m\033[1mMissing GnuPG, it's a prerequisite for this script.\033[0m\a\n\n" 1>&2
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
if ! gpg --homedir $hdir --fingerprint $keyID | grep -q "$keyFingerprint"; then
	printf "\n\033[31m\033[1mGPG key fingerprint does not match, maybe the key wasn't found or it's the wrong key.\033[0m\a\n\n" 1>&2
	rmdir -r .gnupg
	exit 1
fi
umask $omask
#
# get source tarball and check the signature
#
rm $source $sig 2>/dev/null
rm -r ${project} 2>/dev/null
wget -q $server$source
wget -q $server$sig
if ! ( [ -f $source ] && [ -f $sig ] ); then
	printf "\n\033[31m\033[1mError downloading source or signature file.\nSource tried from: %s%s\nSignature tried from: %s%s\033[0m\a\n\n" $server $source $server $sig 1>&2
	exit 1
fi
#
# check signature
#
if ! gpg --homedir $hdir --verify $sig $source >/dev/null 2>&1; then
	printf "\n\033[31m\033[1mError verifying signature - either a download problem or the signature is really wrong.\033[0m\a\n\n" 1>&2
	exit 1
fi
#
# unpack tarball
#
tar -x -f $source
#
# downloaded and unpacked
#
