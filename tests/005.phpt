--TEST--
Check for setLogger
--SKIPIF--
<?php if (!extension_loaded("seaslog")) print "skip"; ?>
--FILE--
<?php
SeasLog::setBasePath('base_path');
var_dump(SeasLog::setLogger('logger'));
var_dump(SeasLog::getLastLogger() == 'logger');
?>
--EXPECT--
bool(true)
bool(true)

