--TEST--
Check for SeasLog::getBufferCount() function.
--SKIPIF--
<?php
if (!extension_loaded('seaslog')
    || !ini_get('seaslog.use_buffer')
    || intval(ini_get('seaslog.buffer_size')) < 20
    || !SeasLog::getBufferEnabled()
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
