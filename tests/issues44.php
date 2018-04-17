<?php
/**
 * @author ciogao@gmail.com
 * Date: 15-6-30 下午12:39
 */

var_dump(SeasLog::getBasePath());

var_dump(SeasLog::getLastLogger());

SeasLog::debug('debug test');

SeasLog::error('错误{aaa}',array('{aaa}'=>'bbb'));
SeasLog::error('错误{aaa}',array('{aaa}'=>'ccc'),'cc');
SeasLog::log('error','错误{aaa}',array('{aaa}'=>'ddd'));
SeasLog::log('asdf','错误{aaa}',array());

//Seaslog::debug("test");

var_dump(Seaslog::analyzerDetail("all","*",null,1,1000));
var_dump(Seaslog::analyzerDetail(SEASLOG_ERROR,"*",null,1,1000));
var_dump(Seaslog::analyzerDetail(SEASLOG_DEBUG));

var_dump(SeasLog::analyzerCount());
var_dump(SeasLog::analyzerCount(SEASLOG_ALL));
var_dump(SeasLog::analyzerCount(SEASLOG_DEBUG));


var_dump(SEASLOG_ALL);

