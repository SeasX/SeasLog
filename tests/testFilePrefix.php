<?php
/**
 * @author rock@php.net
 */

class Log
{
    public static function __callStatic($name, $arguments)
    {
        forward_static_call_array(array('SeasLog', $name), $arguments);
    }

}


var_dump(SeasLog::setFilePrefix("test123-rock"));
var_dump(SeasLog::getFilePrefix());
var_dump(SeasLog::info('33333'));