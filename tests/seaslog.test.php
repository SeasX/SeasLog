<?php
/**
 * @author ciogao@gmail.com
 * Date: 14-1-27 下午4:41
 */

//error_reporting(1);

SeasLog::getBasePath();

SeasLog::setBasePath('/log/test/base/path');

SeasLog::setLogger('test/logger');

SeasLog::log(SEASLOG_ERROR, 'this is a error test by ::log');

SeasLog::debug('this is a {userName} debug', array('{userName}' => 'neeke'));

SeasLog::info('this is a info log');

SeasLog::info('this is a info log',array(),'logger_info_test');

SeasLog::notice('this is a notice log');

SeasLog::warning('your {website} was down,please {action} it ASAP!', array('{website}' => 'github.com', '{action}' => 'rboot'));

SeasLog::error('a error log');

SeasLog::critical('some thing was critical');

SeasLog::alert('yes this is a {messageName}', array('{messageName}' => 'alertMSG'));

SeasLog::emergency('Just now, the house next door was completely burnt out! {note}', array('{note}' => 'it`s a joke'));

var_dump(SeasLog::analyzerCount(SEASLOG_INFO, date('Ymd', time())));
var_dump(SeasLog::analyzerCount(SEASLOG_INFO));
var_dump(SeasLog::analyzerCount());
var_dump(SeasLog::analyzerDetail(SEASLOG_INFO, date('Ymd', time())));
var_dump(SeasLog::analyzerDetail(SEASLOG_DEBUG, date('Ymd', time()), 'neeke', 1, 10));
var_dump(SeasLog::analyzerDetail(SEASLOG_DEBUG, date('Ymd', time()), 'neeke', 1, 10, SEASLOG_DETAIL_ORDER_DESC));
var_dump(SeasLog::getLastLogger());

$buffer = SeasLog::getBuffer();
var_dump($buffer);

