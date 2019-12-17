--TEST--
pr 284
--INI--
seaslog.default_template = "%M|%F"
--SKIPIF--
<?php
if (!extension_loaded('seaslog'))
{
    print 'skip';
}
?>
--FILE--
<?php

class Log
{
    public static function __callStatic($name, $arguments)
    {
        forward_static_call_array(['SeasLog', $name], $arguments);
    }

}
Log::error("aaa");
var_dump(true);
?>
--EXPECT--
bool(true)
