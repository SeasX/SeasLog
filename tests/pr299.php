<?php

/**
 * @author ciogao@gmail.com
 * Date: 2021/1/19 2:37 PM
 */
class TestClassA
{
    public function TestFunctionB()
    {
        return true;
    }

    public function TestFunctionC()
    {
        return 111 / $notFound;
    }
}


$sTestClass = new TestClassA();


$sTestClass->TestFunctionB();
$sTestClass->TestFunctionC();

