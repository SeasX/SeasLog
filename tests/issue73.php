<?php
/**
 * @author neeke@php.net
 * Date: 16/1/11 上午10:15
 */
function testAction(){
    $path = "/usr/local/nginx/html/logs";
    \SeasLog::setBasePath($path);
    \SeasLog::setBasePath($path);
    \SeasLog::notice("a");
    return;
}

testAction();

