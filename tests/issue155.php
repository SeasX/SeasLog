<?php

/**
 * @author aiwhj
 * Date: 17/10/31 下午13.27
 */
class fileAndLine
{
    public function test($log)
    {
        $this->log2($log);
    }

    public function log($log)
    {
        SeasLog::debug($log);
    }

    public function log2($log)
    {
        $this->log($log);
    }
}

$fileAndLine = new fileAndLine();
ini_set('seaslog.recall_depth', 0);
$fileAndLine->test('should line ' . __LINE__);
ini_set('seaslog.recall_depth', 1);
$fileAndLine->test('should line ' . __LINE__);
ini_set('seaslog.recall_depth', 2);
$fileAndLine->test('should line ' . __LINE__);
ini_set('seaslog.recall_depth', 3);
$fileAndLine->test('should line ' . __LINE__);
ini_set('seaslog.recall_depth', 4);
$fileAndLine->test('should line ' . __LINE__);

