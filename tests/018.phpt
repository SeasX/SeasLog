--TEST--
Check for new SeasLog().
--SKIPIF--
<?php
if (!extension_loaded('seaslog'))
{
    print 'skip';
}
?>
--FILE--
<?php
SeasLog::setBasePath('base_path');
$oSeasLogOne = new SeasLog();
$oSeasLogTwo = new SeasLog();

$oSeasLogOne->arrtA = 'aaa';
$oSeasLogTwo->arrtA = 'bbb';

var_dump($oSeasLogOne->debug('this is a debug log {test}.',array('test' => 'replace')));
var_dump($oSeasLogOne->info('this is a info log {test}.',array('test' => 'replace')));
var_dump($oSeasLogTwo->notice('this is a notice log {test}.',array('test' => 'replace')));
var_dump($oSeasLogTwo->warning('this is a warning log {test}.',array('test' => 'replace')));

var_dump($oSeasLogOne->arrtA == 'aaa');
var_dump($oSeasLogTwo->arrtA == 'bbb');

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)

