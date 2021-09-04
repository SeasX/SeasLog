--TEST--
pr 284
--INI--
seaslog.default_template = "%M|%F"
seaslog.default_file_prefix="app-"
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
        forward_static_call_array(array('SeasLog', $name), $arguments);
    }

}
Log::setFilePrefix("seaslog-");
Log::getFilePrefix();
Log::error("aaa");


var_dump(true);
?>
--EXPECT--
bool(true)
