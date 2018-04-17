--TEST--
Check for setDatetimeFormat
--SKIPIF--
<?php if (!extension_loaded("seaslog")) print "skip"; ?>
--FILE--
<?php
var_dump(SeasLog::setDatetimeFormat('Y-m-d H:i:s'));
var_dump(SeasLog::getDatetimeFormat() == 'Y-m-d H:i:s');
?>
--EXPECT--
bool(true)
bool(true)

