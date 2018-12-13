--TEST--
Check for SeasLog::log() function with three parameters.
--SKIPIF--
<?php if (!extension_loaded("seaslog")) print "skip"; ?>
--FILE--
<?php
SeasLog::setBasePath('base_path');
var_dump(SeasLog::log(SEASLOG_DEBUG,'this is a debug log {test}.',array('test' => 'replace')));
var_dump(SeasLog::log(SEASLOG_INFO,'this is a info log {test}.',array('test' => 'replace')));
var_dump(SeasLog::log(SEASLOG_NOTICE,'this is a notice log {test}.',array('test' => 'replace')));
var_dump(SeasLog::log(SEASLOG_WARNING,'this is a warning log {test}.',array('test' => 'replace')));
var_dump(SeasLog::log(SEASLOG_ERROR,'this is a error log {test}.',array('test' => 'replace')));
var_dump(SeasLog::log(SEASLOG_CRITICAL,'this is a critical log {test}.',array('test' => 'replace')));
var_dump(SeasLog::log(SEASLOG_ALERT,'this is a alert log {test}.',array('test' => 'replace')));
var_dump(SeasLog::log(SEASLOG_EMERGENCY,'this is a emergency log {test}.',array('test' => 'replace')));
var_dump(SeasLog::log('my_level','this is a my_level log {test}.',array('test' => 'replace')));

$aLogs = array(
    'this is a log_1 from array',
    'this is a log_2 from array',
    'this is a log_3 from array {test}',
    112233445566,
    NULL,
);

var_dump(SeasLog::log(SEASLOG_DEBUG,$aLogs,array('test' => 'replace')));

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
bool(true)
bool(true)

