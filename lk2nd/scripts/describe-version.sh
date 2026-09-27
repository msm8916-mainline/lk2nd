#!/bin/sh
# SPDX-License-Identifier: BSD-3-Clause
# Copyright (c) 2023 Nikita Travkin <nikita@trvn.ru>

# This script generates a pretty formatted version description using git VCS
# if it's available in the tree.
#
# The format is as follows:
#     <last tag>[-next-<commit>-<date>][-dirty]

# Return -dirty or empty string
git_dirty() {
	if [ -n "$(git status --porcelain)" ]
	then
		echo "-dirty"
	fi
}

# Echo version of the project based on git
get_version_string() {
	if ! git rev-parse --is-inside-work-tree > /dev/null 2>&1
	then
		echo "unknown-$(date +%Y%m%d)"
		return
	fi

	head_rev=$(git rev-parse --short HEAD)
	head_date=$(git log -1 --format=%cd --date=format:"%Y%m%d")
	last_tag=$(git tag --sort=-taggerdate --merged | head -n1)
	if [ -z "$last_tag" ]
	then
		echo "next-$head_rev-$head_date$(git_dirty)"
		return
	fi

	last_tag_ref=$(git rev-list -n1 "$last_tag")
	if [ "$last_tag_ref" = "$(git rev-list -n1 HEAD)" ]
	then
		echo "$last_tag$(git_dirty)"
		return
	fi

	echo "$last_tag-next-$head_rev-$head_date$(git_dirty)"
}

get_version_string
