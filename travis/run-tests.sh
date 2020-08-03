#!/bin/bash
TEST_DIR="`pwd`/tests/"

TEST_PHP_ARGS="-m --show-diff -q" make test

EXIT_STATUS=0

for file in `find $TEST_DIR -type f ! -name '*.phpt' -and ! -name '*.php' -and ! -name '*.png'`
do
    echo '====================================================================='
    sh -xc "cat $file"
    EXIT_STATUS=1
done

exit $EXIT_STATUS
