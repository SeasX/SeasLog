--TEST--
Check for SeasLog::log() function with two parameters.
--SKIPIF--
<?php if (!extension_loaded("seaslog")) print "skip"; ?>
--FILE--
<?php
SeasLog::setBasePath('base_path');
var_dump(SeasLog::log(SEASLOG_INFO,'this is a emergency log test.'));
?>
--EXPECT--
bool(true)

