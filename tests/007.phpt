--TEST--
Check for setRequestID
--SKIPIF--
<?php if (!extension_loaded("seaslog")) print "skip"; ?>
--FILE--
<?php
$id = rand(1,1000);
var_dump(SeasLog::setRequestID($id));
var_dump(SeasLog::getRequestID() == $id);
?>
--EXPECT--
bool(true)
bool(true)

