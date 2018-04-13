--TEST--
Check for SeasLog::log() function with four parameters.
--SKIPIF--
<?php if (!extension_loaded("seaslog")) print "skip"; ?>
--FILE--
<?php
var_dump(SeasLog::log(SEASLOG_DEBUG,'this is a debug log {test}.',array('test' => 'replace'),'logger'));
var_dump(SeasLog::log(SEASLOG_INFO,'this is a info log {test}.',array('test' => 'replace'),NULL));
var_dump(SeasLog::log(SEASLOG_NOTICE,'this is a notice log {test}.',array('test' => 'replace'),''));
var_dump(SeasLog::log(SEASLOG_WARNING,'this is a warning log {test}.',array('test' => 'replace'),1));
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)

