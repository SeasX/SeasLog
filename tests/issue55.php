<?php
/**
 * @author ciogao@gmail.com
 * Date: 15-10-17 下午4:12
 */
var_dump(SEASLOG_DETAIL_ORDER_ASC);

var_dump(SEASLOG_DETAIL_ORDER_DESC);


var_dump(SeasLog::getBasePath());

var_dump(SeasLog::getLastLogger());

SeasLog::debug('debug test');

SeasLog::error('错误{aaa}', array('{aaa}' => 'bbb'));
SeasLog::error('错误{aaa}', array('{aaa}' => 'ccc'), 'cc');
SeasLog::log('error', '错误{aaa}', array('{aaa}' => 'ddd'));
SeasLog::log('asdf', '错误{aaa}', array());

//Seaslog::debug("test");

var_dump(SeasLog::analyzerCount());
var_dump(Seaslog::analyzerDetail("all", "*", NULL, 1, 10, SEASLOG_DETAIL_ORDER_DESC));

var_dump(SeasLog::analyzerCount(SEASLOG_ALL));
var_dump(Seaslog::analyzerDetail(SEASLOG_ERROR, "*", NULL, 1, 1000, SEASLOG_DETAIL_ORDER_DESC));

var_dump(SeasLog::analyzerCount(SEASLOG_DEBUG));
var_dump(Seaslog::analyzerDetail(SEASLOG_DEBUG));

