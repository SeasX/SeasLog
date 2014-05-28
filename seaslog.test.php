<?php
/**
 * @author ciogao@gmail.com
 * Date: 14-1-27 下午4:41
 */

echo '<pre>';
var_dump(SEASLOG_TYPE_INFO,SEASLOG_TYPE_INFO_STR);
var_dump(SEASLOG_TYPE_WARN,SEASLOG_TYPE_WARN_STR);
var_dump(SEASLOG_TYPE_ERRO,SEASLOG_TYPE_ERRO_STR);


var_dump(seaslog_get_basepath());
var_dump(seaslog_get_lastlogger());
var_dump(seaslog_get_basepath());
var_dump(seaslog('test info'));
var_dump(seaslog('test warning', SEASLOG_TYPE_WARN));
var_dump(seaslog('test error', SEASLOG_TYPE_ERRO));
var_dump(seaslog('test error2', SEASLOG_TYPE_ERRO));
var_dump(seaslog('test error3', SEASLOG_TYPE_ERRO));
var_dump(seaslog('test error4', SEASLOG_TYPE_ERRO));


var_dump(seaslog_set_basepath('/log/base_test'));
var_dump(seaslog_set_logger('testModule/app1'));
var_dump(seaslog('test info 2'));
var_dump(seaslog('test warning 2', SEASLOG_TYPE_WARN));
var_dump(seaslog('test error 2', SEASLOG_TYPE_ERRO));
var_dump(seaslog_get_basepath());
var_dump(seaslog_get_lastlogger());
var_dump(seaslog_get_basepath());

var_dump(seaslog('test error 3', SEASLOG_TYPE_ERRO, 'test/bb'));
var_dump(seaslog_get_lastlogger());

var_dump(seaslog_analyzer_count()); // == seaslog_analyzer_count(ALL_TYPE);
var_dump(seaslog_analyzer_count(SEASLOG_TYPE_ERRO)); // == seaslog_analyzer_count(SEASLOG_TYPE_ERRO,'*')
var_dump(seaslog_analyzer_count(SEASLOG_TYPE_ERRO,date('Ymd',time())));

var_dump(seaslog_analyzer_detail(SEASLOG_TYPE_ERRO)); // == seaslog_analyzer_detail(SEASLOG_TYPE_ERRO,'*')
var_dump(seaslog_analyzer_detail(SEASLOG_TYPE_ERRO,date('Ymd',time())));
var_dump(seaslog_analyzer_detail(SEASLOG_TYPE_ERRO,date('Ym',time())));

var_dump(seaslog_get_buffer());
echo "\n";
