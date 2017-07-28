<?php
/**
 * @author ciogao@gmail.com
 * Date: 14-3-8 下午4:30
 */
function SeasLog_AutoLoader($class)
{

    if (!class_exists($class)) {
        $filename = BASE_PATH . '/' . str_replace('\\', '/', $class) . '.php';
        include_once "{$filename}";
    }
}

spl_autoload_register('SeasLog_AutoLoader');