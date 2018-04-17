--TEST--
Check for seaslog_get_version
--SKIPIF--
<?php if (!extension_loaded("seaslog")) print "skip"; ?>
--FILE--
<?php
var_dump(seaslog_get_version() == SEASLOG_VERSION);
?>
--EXPECT--
bool(true)

