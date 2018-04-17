--TEST--
Check for SeasLog log functions with three parameters.
--SKIPIF--
<?php if (!extension_loaded("seaslog")) print "skip"; ?>
--FILE--
<?php
var_dump(SeasLog::debug('this is a debug log {test}.',array('test' => 'replace'),'logger'));
var_dump(SeasLog::info('this is a info log {test}.',array('{test}' => 'replace','')));
var_dump(SeasLog::notice('this is a notice log {test}.',array('test' => 'replace'),NULL));
var_dump(SeasLog::warning('this is a warning log {test}.',array(),'logger'));
var_dump(SeasLog::error('this is a error log {test}.',array('test' => 'replace'),1));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

