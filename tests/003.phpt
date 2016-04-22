--TEST--
Check for seaslog getLogFormat
--SKIPIF--
<?php if (!extension_loaded("seaslog")) print "skip"; ?>
--FILE--
<?php 
SeasLog::setLogFormat(true,true,true);
$message = SeasLog::getLogFormat();
var_dump($message);
?>
--EXPECT--
string(67) "[level] 2016:04:22 09:46:23 | 1461289583.237 | 4632 | getLogFormat

