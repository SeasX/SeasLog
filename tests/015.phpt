--TEST--
Check for SeasLog::closeLoggerStream() function.
--SKIPIF--
<?php
if (!extension_loaded('seaslog')
    || !method_exists('SeasLog','closeLoggerStream'))
{
    print 'skip';
}
?>
--FILE--
<?php
SeasLog::setBasePath('base_path');
var_dump(SeasLog::setLogger('logger'));
var_dump(SeasLog::debug('this is a debug log {test}.',array('test' => 'replace')));
var_dump(SeasLog::info('this is a info log {test}.',array('test' => 'replace')));
var_dump(SeasLog::notice('this is a notice log {test}.',array('test' => 'replace')));
var_dump(SeasLog::warning('this is a warning log {test}.',array('test' => 'replace')));
var_dump(SeasLog::closeLoggerStream(SEASLOG_CLOSE_LOGGER_STREAM_MOD_ASSIGN, 'logger'));
var_dump(SeasLog::closeLoggerStream());
var_dump(SeasLog::closeLoggerStream(SEASLOG_CLOSE_LOGGER_STREAM_MOD_ALL));
var_dump(SeasLog::warning('this is a warning log {test}.',array('test' => 'replace')));
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

