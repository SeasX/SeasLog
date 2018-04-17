<?php
/**
 * @author neeke@php.net
 * Date: 16/5/14 下午10:56
 */

function format($mMsg, $sMethod = '', $sLine = '')
{
    if (is_array($mMsg)) {
        $mMsg = var_export($mMsg, true);
    }

    $sMsg  = sprintf("%s | %s | %s\n", "TestModule", $sMethod, $sLine);
    $sMsg .= $mMsg;
    $sMsg .= "\n\n";

    return $sMsg;
}

function test_log()
{
    $iCount = 10000;
    while ($iCount-- > 0) {
        SeasLog::error(format(array('msg' => "HelloWorld", 'id' => $iCount), "__main__", '123'), array(), 'test1');
    }

    $iCount = 10000;
    while ($iCount-- > 0) {
        SeasLog::info(format(array('msg' => "HelloWorld", 'id' => $iCount), "__main__", '123'), array(), 'test2');
    }

    $iCount = 10000;
    while ($iCount-- > 0) {
        SeasLog::debug(format(array('msg' => "HelloWorld", 'id' => $iCount), "__main__", '123'));
    }
}

while (True) {
    sleep(1);
    test_log();
}

