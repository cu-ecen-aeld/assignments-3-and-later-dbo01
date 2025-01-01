#!/bin/sh

if [ $# -lt 2 ]
then
	echo "Invalid numer of arguments, expected 2, have $#"
	exit 1
fi

filesdir=$1
searchstr=$2

if [ -d ${filesdir} ]
then
	rows_cnt=`grep -r "${searchstr}" ${filesdir} | wc -l`
	files_cnt=`find ${filesdir} -type f | wc -l`

	echo "The number of files are $files_cnt and the number of matching lines are $rows_cnt"
else
	echo " ${filesdir} is not recognized as directory, please provide directory to search"
	exit 1
fi
