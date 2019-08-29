--TEST--
Check for SeasLog::getBufferCount() function.
--SKIPIF--
<?php
if (!extension_loaded('seaslog')
    || !method_exists('SeasLog','getBufferCount'))
{
    print 'skip';
}
?>
--FILE--
<?php
Seaslog::setLogger("logger");
for ($i = 0; $i < 10; $i++) {
    Seaslog::info("test logger $i");
}

var_dump(Seaslog::getBufferCount());

Seaslog::setLogger("change");
for ($i = 0; $i < 10; $i++) {
    Seaslog::info("test change $i");
}

var_dump(Seaslog::getBufferCount());

Seaslog::flushBuffer(0);
var_dump(Seaslog::getBufferCount());
?>
--EXPECT--
int(10)
int(20)
int(0)
