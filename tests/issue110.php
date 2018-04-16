<?php
/**
 * @author ciogao@gmail.com
 * Date: 16/12/6 下午8:08
 */
$logger_1 = new SeasLog();


$logger_2 = new SeasLog();

$logger_1->aaa = 1111;
$logger_2->aaa = 2222;

var_dump($logger_1->aaa);

var_dump($logger_2);

