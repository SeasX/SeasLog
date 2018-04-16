--TEST--
Check for setLogger
--SKIPIF--
<?php if (!extension_loaded("seaslog")) print "skip"; ?>
--FILE--
<?php
var_dump(SeasLog::setLogger('logger'));
var_dump(SeasLog::getLastLogger() == 'logger');
?>
--EXPECT--
bool(true)
bool(true)

