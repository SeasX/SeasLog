<?php
/**
 * @author aiwhj
 * Date: 17/10/31 下午13.27
 */
class fileAndLine {
	public function test($log) {
		$this->log($log);
	}
	public function log($log) {
		SeasLog::debug($log);
	}
}
$fileAndLine = new fileAndLine();
ini_set('seaslog.recall', 0);
$fileAndLine->test('should line 11');
ini_set('seaslog.recall', 1);
$fileAndLine->test('should line 8');
ini_set('seaslog.recall', 2);
$fileAndLine->test('should line 20');
ini_set('seaslog.recall', 3);
$fileAndLine->test('should line 22');
ini_set('seaslog.recall', 4);
$fileAndLine->test('should line 24');
