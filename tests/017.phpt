--TEST--
Check for SeasLog::analyzerDetail() function.
--SKIPIF--
<?php
if (!extension_loaded('seaslog')
    || !method_exists('SeasLog','analyzerDetail')
    || strtoupper(substr(PHP_OS,0,3)) === 'WIN')
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
var_dump(SeasLog::notice('this is a notice log 1 {test}.',array('test' => 'replace')));
var_dump(SeasLog::notice('this is a notice log 2 {test}.',array('test' => 'replace')));
var_dump(SeasLog::notice('this is a notice log 3 {test}.',array('test' => 'replace')));
var_dump(SeasLog::notice('this is a notice log 4 {test}.',array('test' => 'replace')));
var_dump(SeasLog::warning('this is a warning log {test}.',array('test' => 'replace')));

$aLogsAll = SeasLog::analyzerDetail(SEASLOG_ALL);
var_dump(is_array($aLogsAll) && count($aLogsAll) > 1);

$aLogsWithType = SeasLog::analyzerDetail(SEASLOG_INFO);
var_dump(is_array($aLogsWithType) && count($aLogsWithType) > 1);

$aLogsWithLogger = SeasLog::analyzerDetail(SEASLOG_DEBUG, date('Ymd', time()));
var_dump(is_array($aLogsWithLogger) && count($aLogsWithLogger) > 1);

$aLogsWithKeyWord = SeasLog::analyzerDetail(SEASLOG_NOTICE, date('Ymd', time()),'replace');
var_dump(is_array($aLogsWithKeyWord) && count($aLogsWithKeyWord) > 1);

$aLogsWithLimit = SeasLog::analyzerDetail(SEASLOG_NOTICE, date('Ymd', time()),'replace',1,3);
var_dump(is_array($aLogsWithLimit) && count($aLogsWithLimit) == 3);

$aLogsWithOrder = SeasLog::analyzerDetail(SEASLOG_NOTICE, date('Ymd', time()),'replace',1,3,SEASLOG_DETAIL_ORDER_DESC);
var_dump(is_array($aLogsWithOrder) && count($aLogsWithOrder) == 3);

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
bool(true)
bool(true)
bool(true)

