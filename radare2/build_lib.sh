#!/bin/bash

red() {
	echo -e "\e[1;31m$@\e[0m"
}

usage() {
	echo "This script builds radare2 for fuzzing with afl"
	echo "usage:  R2_ROOT=~/src/radare2 $0"
}

if [ -z "$R2_ROOT" ]; then
	usage 1>&2
	exit 1
fi

red "FIXME: currently, r2 doesn't support out-of-place compilation"; exit 1

$R2_ROOT/configure
make
