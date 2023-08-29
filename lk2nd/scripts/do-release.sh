#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2023 Nikita Travkin <nikita@trvn.ru>

# This script compiles listed projects from the lk2nd tree
# and renames it's artifacts of interest by the project name.
# It supports generating and signing a hashsum file to
# help verify the artifacts.

# List of the projects to build by default
PROJECTS="
	lk2nd-msm8226
	lk2nd-msm8909
	lk2nd-msm8916
	lk2nd-msm8952
	lk2nd-msm8953
	lk2nd-msm8960
	lk2nd-msm8974
	lk2nd-msm8994
	lk2nd-msm8996
"

# List of artifacts to extract from the build dir
ARTIFACTS="
	lk2nd.img
"

# cd to the lk2nd root tree
SCRIPT_DIR=$(CDPATH="" cd -- "$(dirname -- "$0")" && pwd)
cd "$SCRIPT_DIR"/../.. || exit


if [ ! -d lk2nd ] || [ ! -f makefile ]
then
	echo "Failed to find the lk2nd project root"
	exit 1
fi

usage() {
	echo "Usage: $0 [-hs] [project ...]"
	echo "Create tagged release builds for select lk2nd projects."
	echo "If [project ...] list is not given, a default set of projects will be built."
	echo
	echo "  -S    Sign the release"
	echo "  -h    This help"
	echo
}

sign_release=0

while getopts ":dhS" opt
do
	case $opt in
		S)
			sign_release=1
			;;
		h)
			usage
			exit 0 ;;
		*)
			usage
			echo "Unkown option: -$OPTARG"
			echo
			exit 1 ;;
	esac
done
shift $((OPTIND-1))

if [ $# -ne 0 ]
then
	PROJECTS="$*"
fi

if [ "$(echo build-*)" != "build-*" ] && ! [ -d "build-*" ]
then
	echo "All existing build dirs will be removed."
	printf "Continue? [y]"
	read -r

	rm -rf build-*
fi

version_string="$("$SCRIPT_DIR/describe-version.sh")"

echo "Building release artifacts for $version_string"
echo

for proj in $PROJECTS
do
	echo "Building $proj"
	echo "----------------------"
	make -j"$(nproc)" TOOLCHAIN_PREFIX=arm-none-eabi- "$proj"
	echo
done

release_dir="release/$version_string"

if [ -d "$release_dir" ]
then
	echo "$release_dir already exists. It will be removed."
	printf "Continue? [y]"
	read -r

	rm -rf "$release_dir"
fi

mkdir -p "$release_dir"

for proj in $PROJECTS
do
	echo "Packing $proj"
	for artifact in $ARTIFACTS
	do
		if [ -e "build-$proj/$artifact" ]
		then
			new_name="$proj$(echo "$artifact" | sed "s/^lk2nd//")"
			echo "  $new_name"
			cp "build-$proj/$artifact" "$release_dir/$new_name"
		fi
	done
done

if [ $sign_release -eq 1 ]
then
	echo "Signing the release artifacts."
	cd "$release_dir" || exit
	sha256sum -- * > sha256sums.txt
	gpg --detach-sign sha256sums.txt
fi
