--TEST--
Check for setBasePath
--SKIPIF--
<?php if (!extension_loaded("seaslog")) print "skip"; ?>
--FILE--
<?php
var_dump(SeasLog::setBasePath('base_path'));
var_dump(SeasLog::getBasePath() == 'base_path');
?>
--EXPECT--
bool(true)
bool(true)

