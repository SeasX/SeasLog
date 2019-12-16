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
    // 代理seaslog的静态方法，如 Seaslog::debug
    public static function __callStatic($name, $arguments)
    {
        forward_static_call_array(['SeasLog', $name], $arguments);
    }

}
Log::error("aaa");
var_dump(10)
?>
--EXPECT--
int(10)
