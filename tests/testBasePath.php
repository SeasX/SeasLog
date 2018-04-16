<?php
/**
 * @author ciogao@gmail.com
 * Date: 16/12/6 下午8:23
 */
var_dump(SeasLog::setBasePath('/tmp/ddd'));
var_dump(SeasLog::getBasePath());
var_dump(SeasLog::setLogger('wwwwww'));
var_dump(SeasLog::info('111111'));

var_dump(SeasLog::setBasePath('/tmp/aaa'));
var_dump(SeasLog::setLogger("eeeeeeeee"));
var_dump(SeasLog::getBasePath());
var_dump(SeasLog::info('222222'));

var_dump(SeasLog::setBasePath('/tmp/bbbb'));
var_dump(SeasLog::setLogger("eeeeeeeee"));
var_dump(SeasLog::info('33333'));
//
//var_dump(SeasLog::setBasePath('/tmp/bbb'));
//var_dump(SeasLog::setLogger('ccc'));
//var_dump(SeasLog::getBasePath());
//var_dump(SeasLog::getLastLogger());
//var_dump(SeasLog::info('333333'));


var_dump(SeasLog::getBuffer());

