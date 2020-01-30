#!/bin/bash
TEST_DIR="`pwd`/tests/"

TEST_PHP_ARGS="-m --show-diff" make test

for file in `find $TEST_DIR -name "*.diff" 2>/dev/null`
do
	grep "\-\-XFAIL--" ${file/%diff/phpt} >/dev/null 2>&1
	if [ $? -gt 0 ]
	then
		FAILS[${#FAILS[@]}]="$file"
	fi
done

if [ ${#FAILS[@]} -gt 0 ]
then
	for fail in "${FAILS[@]}"
	do
		sh -xc "cat $fail"
	done
	exit 1
else
	exit 0
fi
