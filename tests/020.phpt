--TEST--
Check for SeasLog::setLogger() function.
--SKIPIF--
<?php
if (!extension_loaded('seaslog')
    || !method_exists('SeasLog','setLogger'))
{
    print 'skip';
}
?>
--FILE--
<?php
SeasLog::setBasePath('base_path');
var_dump(SeasLog::getLastLogger());
SeasLog::setLogger('aaa');
var_dump(SeasLog::getLastLogger());
SeasLog::setLogger('aaaBBB');
var_dump(SeasLog::getLastLogger());
SeasLog::setLogger('aaa');
var_dump(SeasLog::getLastLogger());
SeasLog::setLogger('aaaBBB');
var_dump(SeasLog::getLastLogger());
SeasLog::setLogger('a');
var_dump(SeasLog::getLastLogger());
SeasLog::setLogger('b');
var_dump(SeasLog::getLastLogger());
?>
--EXPECT--
string(7) "default"
string(3) "aaa"
string(6) "aaaBBB"
string(3) "aaa"
string(6) "aaaBBB"
string(1) "a"
string(1) "b"
