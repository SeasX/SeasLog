<?php

/**
 * @author neeke@php.net
 * Date: 16/4/14 下午9:57
 */
class testBasePathAndLogger
{
    public function __construct()
    {

    }

    public function TestRun()
    {
        $this->testBasePath();
        $this->testLogger();
    }

    private function testBasePath()
    {
        SeasLog::setBasePath('aaa');
        SeasLog::setBasePath('bbb');
    }

    private function testLogger()
    {
        SeasLog::setLogger('aaa');
        SeasLog::setLogger('bbb');
    }
}

$testObj = new testBasePathAndLogger();
$testObj->TestRun();

