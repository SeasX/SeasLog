--TEST--
Check for seaslog_get_author
--SKIPIF--
<?php if (!extension_loaded("seaslog")) print "skip"; ?>
--FILE--
<?php
var_dump(seaslog_get_author() == 'Chitao.Gao  [ neeke@php.net ]');
?>
--EXPECT--
bool(true)

