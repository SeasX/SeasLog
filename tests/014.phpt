--TEST--
Check for SeasLog::getBuffer() function.
--SKIPIF--
<?php
if (!extension_loaded('seaslog')
    || !ini_get('seaslog.use_buffer')
    || intval(ini_get('seaslog.buffer_size')) < 5
    || !SeasLog::getBufferEnabled())
{
    print 'skip';
}
?>
--FILE--
<?php
var_dump(SeasLog::debug('this is a debug log {test}.',array('test' => 'replace')));
var_dump(SeasLog::info('this is a info log {test}.',array('test' => 'replace')));
var_dump(SeasLog::notice('this is a notice log {test}.',array('test' => 'replace')));
var_dump(SeasLog::warning('this is a warning log {test}.',array('test' => 'replace')));
$aBuffer = SeasLog::getBuffer();
var_dump(is_array($aBuffer));
var_dump(count($aBuffer) >= 1);
foreach ($aBuffer as $buffer) {
    var_dump(count($buffer) == 4);
}
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

