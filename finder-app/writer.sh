#!/bin/sh

if [ $# -lt 2 ]
then
	echo "Invalid numer of arguments, expected 2, have $#"
	exit 1
fi

writefile=$1
writestr=$2

dir=$(dirname $writefile)

mkdir -p $dir || exit 1
touch $writefile || exit 1
echo $writestr > $writefile || exit 1
