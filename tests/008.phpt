--TEST--
Check for SeasLog log functions with one parameter.
--SKIPIF--
<?php if (!extension_loaded("seaslog")) print "skip"; ?>
--FILE--
<?php
var_dump(SeasLog::debug('this is a debug log test.'));
var_dump(SeasLog::info('this is a info log test.'));
var_dump(SeasLog::notice('this is a notice log test.'));
var_dump(SeasLog::warning('this is a warning log test.'));
var_dump(SeasLog::error('this is a error log test.'));
var_dump(SeasLog::critical('this is a critical log test.'));
var_dump(SeasLog::alert('this is a alert log test.'));
var_dump(SeasLog::emergency('this is a emergency log test.'));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

