--TEST--
Check the SeasLog default file datetime separator.
--INI--
seaslog.default_file_datetime_separator="-"
--SKIPIF--
<?php
if (!extension_loaded('seaslog'))
{
    print 'skip';
}
?>
--FILE--
<?php

SeasLog::error(time());

var_dump(true);
?>
--EXPECT--
bool(true)
