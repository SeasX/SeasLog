--TEST--
pr 284
--INI--
seaslog.default_template = "%M|%F"
seaslog.default_basepath="base_path"
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
class Log
{
    public static function __callStatic($name, $arguments)
    {
        forward_static_call_array(array('SeasLog', $name), $arguments);
    }

}
Log::error("aaa");
var_dump(true);
?>
--EXPECT--
bool(true)
