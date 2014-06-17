# SeasLog
A effective,fast,stable log extension for PHP
@author ciogao@gmail.com

> ---
- **[简介](#简介)**
    - **[为什么使用SeasLog](#为什么使用seaslog)**
    - **[目前提供了什么](#目前提供了什么)**
    - **[目标是怎样的](#目标是怎样的)**
- **[安装](#安装)**
    - **[编译安装 SeasLog](#编译安装-seaslog)**
    - **[seaslog.ini的配置](#seaslogini的配置)**
- **[使用](#使用)**
    - **[常量与函数](#常量与函数)**
        - [常量列表](#常量列表)
        - [函数列表](#函数列表)
	- **[SeasLog Logger的使用](#seaslog-logger的使用)**
        - [获取与设置basePath](#获取与设置basepath)
        - [设置logger与获取lastLogger](#设置logger与获取lastlogger)
        - [快速写入log](#快速写入log)
	- **[SeasLog Analyzer的使用](#seaslog-analyzer的使用)**
		- [快速统计某类型log的count值](#快速统计某类型log的count值)
		- [获取某类型log列表](#获取某类型log列表)
	- **[使用SeasLog进行健康预警](#使用seaslog进行健康预警)**
        - [预警的配置](#预警的配置)
		- [crontab配置](#crontab配置)
        
> ---

## 简介
### 为什么使用SeasLog
log日志，通常是系统或软件、应用的运行记录。通过log的分析，可以方便用户了解系统或软件、应用的运行情况；如果你的应用log足够丰富，也可以分析以往用户的操作行为、类型喜好、地域分布或其他更多信息；如果一个应用的log同时也分了多个级别，那么可以很轻易地分析得到该应用的健康状况，及时发现问题并快速定位、解决问题，补救损失。

php内置error_log、syslog函数功能强大且性能极好，但由于各种缺陷(error_log无错误级别、无固定格式，syslog不分模块、与系统日志混合)，灵活度降低了很多，不能满足应用需求。

好消息是，有不少第三方的log类库弥补了上述缺陷，如log4php、plog、Analog等(当然也有很多应用在项目中自己开发的log类)。其中以[log4php](http://logging.apache.org/log4php/)最为著名，设计精良、格式完美、文档完善、功能强大。推荐。(log4php的性能有待测试)

那么有没有一种log类库满足以下需求呢：
* 分模块、分级别
* 配置简单(最好是勿须配置)
* 日志格式清晰易读
* 应用简单、性能很棒

`SeasLog` 正是应此需求而生。

### 目前提供了什么
* 在PHP项目中便捷、规范地记录log
* 可配置的默认log目录与模块
* 指定log目录与获取当前配置
* 初步的分析预警框架
* 高效的日志缓冲、便捷的缓冲debug


### 目标是怎样的
* 便捷、规范的log记录
* 高效的海量log分析
* 可配置、多途径的log预警

## 安装

### 编译安装 seaslog
```sh
$ /path/to/phpize
$ ./configure --with-php-config=/path/to/php-config
$ make && make install
```

### seaslog.ini的配置
```conf
; configuration for php SeasLog module
extension = seaslog.so
seaslog.default_basepath = /log/seaslog-test    ;默认log根目录
seaslog.default_logger = default                ;默认logger目录
seaslog.disting_type = 1                        ;是否以type分文件 1是 0否(默认)
seaslog.disting_by_hour = 1                     ;是否每小时划分一个文件 1是 0否(默认)
seaslog.use_buffer = 1                          ;是否启用buffer 1是 0否(默认)
```
> `seaslog.disting_type = 1` 开启以type分文件，即log文件区分info\warn\erro

> `seaslog.disting_by_hour = 1` 开启每小时划分一个文件

> `seaslog.use_buffer = 1` 开启buffer。默认关闭。当开启此项时，日志预存于内存，当请求结束时(或异常退出时)一次写入文件。

## 使用

### 常量与函数
#### 常量列表
* SEASLOG_TYPE_INFO = 1
* SEASLOG_TYPE_WARN = 2
* SEASLOG_TYPE_ERRO = 3
```php
var_dump(SEASLOG_TYPE_INFO,SEASLOG_TYPE_WARN,SEASLOG_TYPE_ERRO);
/*
int(1) info类型
int(2) warn类型
int(3) erro类型
*/
```
#### 函数列表
`SeasLog` 提供了这样一组函数，可以方便地获取与设置根目录、模块目录、快速写入与统计log。
相信从下述伪代码的注释中，您可以快速获取函数信息，具体使用将紧接其后：
```php
<?php
/**
 * @author ciogao@gmail.com
 * Date: 14-1-27 下午4:47
 */
/**
 * 设置basePath
 * @param $basePath
 * @return bool
 */
function seaslog_set_basepath($basePath)
{
    return TRUE;
}

/**
 * 获取basePath
 * @return string
 */
function seaslog_get_basepath()
{
    return 'the base_path';
}

/**
 * 设置模块目录
 * @param $module
 * @return bool
 */
function seaslog_set_logger($module)
{
    return TRUE;
}

/**
 * 获取最后一次设置的模块目录
 * @return string
 */
function seaslog_get_lastlogger()
{
    return 'the lastLogger';
}

/**
 * 快速写入log
 * @param $msg
 * @param string $module
 * @param int $type
 * @return bool
 */
function seaslog($msg, $type = SEASLOG_TYPE_INFO, $module = 'defaultPath')
{
    return TRUE;
}

/**
 * 统计所有类型（或单个类型）行数
 * @param string $type
 * @param string $log_path
 * @return array | long
 */
function seaslog_analyzer_count($type = 'allType',$log_path = '*')
{
    return array();
}

/**
 * 以数组形式，快速取出某类型log的各行详情
 * @param $type
 * @param string $log_path
 * @return array
 */
function seaslog_analyzer_detail($type = SEASLOG_TYPE_ERRO,$log_path = '*')
{
    return array();
}

/**
 * 获得当前日志buffer中的内容
 * @return array
 */
function seaslog_get_buffer()
{
    return array();
}

```

### SeasLog Logger的使用
#### 获取与设置basePath
```php
$basePath_1 = seaslog_get_basepath();

seaslog_set_basepath('/log/base_test');
$basePath_2 = seaslog_get_basepath();

var_dump($basePath_1,$basePath_2);

/*
string(19) "/log/seaslog-ciogao"
string(14) "/log/base_test"
*/
```
> 直接使用 `seaslog_get_basepath()`，将获取php.ini(seaslog.ini)中设置的 `seaslog.default_basepath` 的值。

> 使用 `seaslog_set_basepath()` 函数，将改变 `seaslog_get_basepath()` 的取值。

#### 设置logger与获取lastLogger
```php
$lastLogger_1 = seaslog_get_lastlogger();

seaslog_set_logger('testModule/app1');
$lastLogger_2 = seaslog_get_lastlogger();

var_dump($lastLogger_1,$lastLogger_2);
/*
string(7) "default"
string(15) "testModule/app1"
*/
```
> 与basePath相类似的，

> 直接使用 `seaslog_get_lastlogger()`，将获取php.ini(seaslog.ini)中设置的 `seaslog.default_logger` 的值。

> 使用 `seaslog_set_logger()` 函数，将改变 `seaslog_get_lastlogger()` 的取值。

#### 快速写入log
上面已经设置过了basePath与logger，于是log记录的目录已经产生了，
> log记录目录 = basePath / logger / {fileName}.log
log文件名，以 `年月日` 分文件，如今天是2014年02月18日期，那么 `{fileName}` = `20140218`;

还记得 `php.ini` 中设置的 `seaslog.disting_type` 吗？

默认的 `seaslog.disting_type = 0`，如果今天我使用了 `SeasLog` ，那么将产生最终的log文件：
* LogFile = basePath / logger / 20140218.log

如果 `seaslog.disting_type = 1`，则最终的log文件将是这样的三个文件
* infoLogFile = basePath / logger / INFO.20140218.log

* warnLogFile = basePath / logger / WARN.20140218.log

* erroLogFile = basePath / logger / ERRO.20140218.log

```php
seaslog('this is a info');
seaslog('this is a error 1', SEASLOG_TYPE_ERRO);
seaslog('this is a error 2', SEASLOG_TYPE_ERRO);
seaslog('this is a warning', SEASLOG_TYPE_WARN);

seaslog('test error 3', SEASLOG_TYPE_ERRO, 'test/new/path');
/*
seaslog()函数同时也接受第3个参数为logger的设置项
注意，当last_logger == 'default'时等同于:
seaslog_set_logger('test/new/path');
seaslog('test error 3', SEASLOG_TYPE_ERRO);
如果已经在前文使用过seaslog_set_logger函数，第3个参数的log只在此处临时使用，不影响下文。
*/
```
> log格式统一为： `{type} | {pid} | {timeStamp} |{dateTime} | {logInfo}`
```sh
ERRO | 7670 | 1393171368.875 | 2014:02:24 00:02:48 | test error 3
INFO | 7670 | 1393171372.344 | 2014:02:24 00:02:52 | this is a info 
ERRO | 7670 | 1393171515.336 | 2014:02:24 00:05:15 | this is a error 1
ERRO | 7670 | 1393171609.881 | 2014:02:24 00:06:49 | this is a error 2
```

### SeasLog Analyzer的使用
#### 快速统计某类型log的count值
`SeasLog`在扩展中使用管道调用shell命令 `grep -wc`快速地取得count值，并返回值(array || int)给PHP。
```php
$countResult_1 = seaslog_analyzer_count();
$countResult_2 = seaslog_analyzer_count(SEASLOG_TYPE_WARN);
$countResult_3 = seaslog_analyzer_count(SEASLOG_TYPE_ERRO,date('Ymd',time()));

var_dump($countResult_1,$countResult_2,$countResult_3);
/*
array(3) {
  'INFO' =>
  int(0)
  'WARN' =>
  int(0)
  'ERRO' =>
  int(7)
}

int(7)

int(1)

*/
```
#### 获取某类型log列表
`SeasLog`在扩展中使用管道调用shell命令 `grep -w`快速地取得列表，并返回array给PHP。
```php
$detailErrorArray_inAll   = seaslog_analyzer_detail(SEASLOG_TYPE_ERRO);
$detailErrorArray_today   = seaslog_analyzer_detail(SEASLOG_TYPE_ERRO,date('Ymd',time()));

var_dump($detailErrorArray_inAll,$detailErrorArray_today);

/*
seaslog_analyzer_detail(SEASLOG_TYPE_ERRO) == seaslog_analyzer_detail(SEASLOG_TYPE_ERRO,'*');
取当前模块下所有type为 SEASLOG_TYPE_ERRO 的信息列表:
array(6) {
 [0] =>
  string(66) "ERRO | 8568 | 1393172042.717 | 2014:02:24 00:14:02 | test error 3 "
  [1] =>
  string(66) "ERRO | 8594 | 1393172044.104 | 2014:02:24 00:14:04 | test error 3 "
  [2] =>
  string(66) "ERRO | 8620 | 1393172044.862 | 2014:02:24 00:14:04 | test error 3 "
  [3] =>
  string(66) "ERRO | 8646 | 1393172045.989 | 2014:02:24 00:14:05 | test error 3 "
  [4] =>
  string(66) "ERRO | 8672 | 1393172047.882 | 2014:02:24 00:14:07 | test error 3 "
  [5] =>
  string(66) "ERRO | 8698 | 1393172048.736 | 2014:02:24 00:14:08 | test error 3 "
}

seaslog_analyzer_detail(SEASLOG_TYPE_ERRO,date('Ymd',time()));
只取得当前模块下，当前一天内,type为 SEASLOG_TYPE_ERRO 的信息列表:
array(2) {
  [0] =>
  string(66) "ERRO | 8568 | 1393172042.717 | 2014:02:24 00:14:02 | test error 3 "
  [1] =>
  string(66) "ERRO | 8594 | 1393172044.104 | 2014:02:24 00:14:04 | test error 3 "
}

同理，取当月 
$detailErrorArray_mouth = seaslog_analyzer_detail(SEASLOG_TYPE_ERRO,date('Ym',time()));

*/
```

### 使用SeasLog进行健康预警
#### 预警的配置
```conf
[base]
wait_analyz_log_path = /log/base_test

[fork]
;是否开启多线程 1开启 0关闭
fork_open = 1

;线程个数
fork_count = 3

[warning]
email[smtp_host] = smtp.163.com
email[smtp_port] = 25
email[subject_pre] = 预警邮件 -
email[smtp_user] = seaslogdemo@163.com
email[smtp_pwd] = seaslog#demo
email[mail_from] = seaslogdemo@163.com
email[mail_to] = gaochitao@weiboyi.com
email[mail_cc] = ciogao@gmail.com
email[mail_bcc] =

[analyz]
; enum
;info => SEASLOG_TYPE_INFO
;warn => SEASLOG_TYPE_WARN
;erro => SEASLOG_TYPE_ERRO

test1[module] = test/bb
test1[type] = SEASLOG_TYPE_ERRO
test1[bar] = 1
test1[mail_to] = gaochitao@weiboyi.com

test2[module] = 222
test2[type] = SEASLOG_TYPE_INFO

test3[module] = 333
test3[type] = SEASLOG_TYPE_WARN

test4[module] = 444
test4[type] = SEASLOG_TYPE_WARN

test5[module] = 555
test5[type] = SEASLOG_TYPE_WARN

```
#### crontab配置
```conf
;每天凌晨3点执行
0 3 * * * /path/to/php /path/to/SeasLog/Analyzer/SeasLogAnalyzer.php
```
