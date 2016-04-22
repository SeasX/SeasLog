--TEST--
Check for seaslog setLogFormat
--SKIPIF--
<?php if (!extension_loaded("seaslog")) print "skip"; ?>
--FILE--
<?php
  $status = SeasLog::setLogFormat(true, true, true);
  var_dump($status);
?>
--EXPECT--
true
