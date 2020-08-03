--TEST--
Check for SeasLog::analyzerCount() function.
--SKIPIF--
<?php
if (!extension_loaded('seaslog')
    || !method_exists('SeasLog','analyzerCount'))
{
    print 'skip';
}
?>
--INI--
seaslog.default_template = "%T | %L | %P | %Q | %t | %M"
--FILE--
<?php
SeasLog::setBasePath('base_path');
var_dump(SeasLog::debug('this is a debug log {test}.',array('test' => 'replace')));
var_dump(SeasLog::info('this is a info log {test}.',array('test' => 'replace')));
var_dump(SeasLog::notice('this is a notice log {test}.',array('test' => 'replace')));
var_dump(SeasLog::warning('this is a warning log {test}.',array('test' => 'replace')));
$iInfoCountWithLogger = SeasLog::analyzerCount(SEASLOG_INFO, date('Ymd', time()));
var_dump((int)$iInfoCountWithLogger > 0);

$iInfoCountWithType = SeasLog::analyzerCount(SEASLOG_INFO);
var_dump((int)$iInfoCountWithType > 0);

$iLogCount = SeasLog::analyzerCount();
var_dump(is_array($iLogCount) && count($iLogCount) == 8);
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

