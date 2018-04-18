SeasLog
======
[![Build Status](https://travis-ci.org/SeasX/SeasLog.svg?branch=master)](https://travis-ci.org/SeasX/SeasLog)

An effective,fast,stable log extension for PHP

@author Chitao.Gao [neeke@php.net]

@交流群 312910117

[English Document](https://github.com/SeasX/SeasLog/blob/master/README.md)

[日志规范](https://github.com/SeasX/SeasLog/blob/master/Specification/README_zh.md)

[日志收集](https://github.com/SeasX/SeasLog/blob/master/Specification/SeasLog_Rsyslog_ELK_zh.md)

> ---
- **[简介](#简介)**
    - **[为什么使用SeasLog](#为什么使用seaslog)**
    - **[目前提供了什么](#目前提供了什么)**
    - **[目标是怎样的](#目标是怎样的)**
- **[安装](#安装)**
    - **[编译安装 SeasLog](#编译安装-seaslog)**
    - **[seaslog.ini的配置](#seaslogini的配置)**
    - **[自定义日志模板](#自定义日志模板)**
        - [日志模板说明](#日志模板说明)
        - [预设变量表](#预设变量表)
- **[使用](#使用)**
    - **[常量与函数](#常量与函数)**
        - [常量列表](#常量列表)
        - [函数列表](#函数列表)
    - **[PHP Re 结果](#php-re-结果)**
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
    - **[目前已知使用SeasLog的企业](#目前已知使用SeasLog的企业)**
        - [企业名单](#企业名单)
        - [接受捐赠](#接受捐赠)


> ---

## 简介
### 为什么使用SeasLog
log日志，通常是系统或软件、应用的运行记录。通过log的分析，可以方便用户了解系统或软件、应用的运行情况；如果你的应用log足够丰富，也可以分析以往用户的操作行为、类型喜好、地域分布或其他更多信息；如果一个应用的log同时也分了多个级别，那么可以很轻易地分析得到该应用的健康状况，及时发现问题并快速定位、解决问题，补救损失。

php内置error_log、syslog函数功能强大且性能极好，但由于各种缺陷(error_log无错误级别、无固定格式，syslog不分模块、与系统日志混合)，灵活度降低了很多，不能满足应用需求。

好消息是，有不少第三方的log类库弥补了上述缺陷，如log4php、plog、Analog等(当然也有很多应用在项目中自己开发的log类)。其中以[log4php](http://logging.apache.org/log4php/)最为著名，设计精良、格式完美、文档完善、功能强大。推荐。

不过log4php在性能方面表现非常差,下图是SeasLog与log4php的ab并发性能测试( 测试环境:Ubuntu12.04单机,CPU I3,内存 16G,硬盘 SATA 7200):
![SeasLogVSlog4php](https://raw.githubusercontent.com/SeasX/SeasLog/master/tests/SeasLogVSlog4php.png)


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
* 遵循 [PSR-3](https://github.com/php-fig/fig-standards/blob/master/accepted/PSR-3-logger-interface.md) 日志接口规范
* 自动记录错误信息
* 自动记录异常信息
* 连接TCP端口，以RFC5424格式发送日志
* 连接UDP端口，以RFC5424格式发送日志
* 支持RequestId区分请求
* 支持日志模板自定义

### 目标是怎样的
* 便捷、规范的log记录
* 高效的海量log分析
* 可配置、多途径的log预警

## 安装

### 编译安装 SeasLog
```sh
$ /path/to/phpize
$ ./configure --with-php-config=/path/to/php-config
$ make && make install
```

### PECL安装SeasLog
```sh
$ pecl install seaslog
```

### Windows环境中使用SeasLog
到PECL/SeasLog主页找到对应的dll进行安装 [PECL/SeasLog Windows Dll](http://pecl.php.net/package/SeasLog/1.8.4/windows)

### seaslog.ini的配置
```conf
[SeasLog]
;configuration for php SeasLog module
extension = seaslog.so

;默认log根目录
seaslog.default_basepath = "/var/log/www"

;默认logger目录
seaslog.default_logger = "default"

;日期格式配置 默认"Y-m-d H:i:s"
seaslog.default_datetime_format = "Y-m-d H:i:s"

;日志格式模板 默认"%T | %L | %P | %Q | %t | %M"
seaslog.default_template = "%T | %L | %P | %Q | %t | %M"

;是否以目录区分Logger 1是(默认) 0否
seaslog.disting_folder = 1

;是否以type分文件 1是 0否(默认)
seaslog.disting_type = 0

;是否每小时划分一个文件 1是 0否(默认)
seaslog.disting_by_hour = 0

;是否启用buffer 1是 0否(默认)
seaslog.use_buffer = 0

;buffer中缓冲数量 默认0(不使用buffer_size)
seaslog.buffer_size = 100

;cli运行时关闭buffer
;1是 0否(默认)
seaslog.buffer_disabled_in_cli = 0

;记录日志级别，数字越大，根据级别记的日志越多。
;0-EMERGENCY 1-ALERT 2-CRITICAL 3-ERROR 4-WARNING 5-NOTICE 6-INFO 7-DEBUG 8-ALL
;默认8(所有日志)
;
;   注意, 该配置项自1.7.0版本开始有变动。
;   在1.7.0版本之前, 该值数字越小，根据级别记的日志越多: 
;   0-all 1-debug 2-info 3-notice 4-warning 5-error 6-critical 7-alert 8-emergency
;   1.7.0 之前的版本，该值默认为0(所有日志);
seaslog.level = 8

;日志函数调用回溯层级
;影响预定义变量 %F 中的行数
;默认0
seaslog.recall_depth = 0

;自动记录notice 默认0(关闭)
seaslog.trace_notice = 0

;自动记录warning 默认0(开启)
seaslog.trace_warning = 0

;自动记录错误 默认1(开启)
seaslog.trace_error = 1

;自动记录异常信息 默认0(关闭)
seaslog.trace_exception = 0

;日志存储介质 1File 2TCP 3UDP (默认为1)
seaslog.appender = 1

;写入重试次数
;默认0(不重试)
seaslog.appender_retry = 0

;接收ip 默认127.0.0.1 (当使用TCP或UDP时必填)
seaslog.remote_host = "127.0.0.1"

;接收端口 默认514 (当使用TCP或UDP时必填)
seaslog.remote_port = 514

;接收端口的超时时间 默认1秒
seaslog.remote_timeout = 1

;过滤日志中的回车和换行符 (默认为0)
seaslog.trim_wrap = 0

;是否开启抛出SeasLog自身异常  1开启(默认) 0否
seaslog.throw_exception = 1

;是否开启忽略SeasLog自身warning  1开启(默认) 0否
seaslog.ignore_warning = 1
```
> `seaslog.disting_folder = 1` 开启以目录分文件，即logger以目录区分。当关闭时，logger以下划线拼接时间, 如default_20180211.log。

> `seaslog.disting_type = 1` 开启以type分文件，即log文件区分info\warn\erro

> `seaslog.disting_by_hour = 1` 开启每小时划分一个文件

> `seaslog.use_buffer = 1` 开启buffer。默认关闭。当开启此项时，日志预存于内存，当请求结束时(或异常退出时)一次写入文件。

> `seaslog.buffer_size = 100` 设置缓冲数量为100. 默认为0,即无缓冲数量限制.当buffer_size大于0时,缓冲量达到该值则写一次文件.

> `seaslog.buffer_disabled_in_cli = 1` 开启CLI运行时禁用缓存。默认关闭。当开启此项时，CLI运行时将忽略seaslog.use_buffer设定，日志写入文件。

> `seaslog.level = 8` 记录的日志级别.默认为8,即所有日志均记录。

> `seaslog.level = 0` 记录EMERGENCY。

> `seaslog.level = 1` 记录EMERGENCY、ALERT。

> `seaslog.level = 2` 记录EMERGENCY、ALERT、CRITICAL。

> `seaslog.level = 3` 记录EMERGENCY、ALERT、CRITICAL、ERROR。

> `seaslog.level = 4` 记录EMERGENCY、ALERT、CRITICAL、ERROR、WARNING。

> `seaslog.level = 5` 记录EMERGENCY、ALERT、CRITICAL、ERROR、WARNING、NOTICE。

> `seaslog.level = 6` 记录EMERGENCY、ALERT、CRITICAL、ERROR、WARNING、NOTICE、INFO。

> `seaslog.level = 7` 记录EMERGENCY、ALERT、CRITICAL、ERROR、WARNING、NOTICE、INFO、DEBUG。

> `seaslog.throw_exception = 1` 开启抛出SeasLog抛出自身的异常。当出现录权限或接收服务器端口不通等情况时，抛出异常；关闭时不抛出异常。

> `seaslog.ignore_warning = 1` 开启忽略SeasLog自身的警告。当出现目录权限或接收服务器端口不通等情况时，将进行忽略；关闭时，将抛出警告。

### 自定义日志模板
很多朋友在使用过程中提到自定义日志模板的需求，于是`SeasLog`自1.7.2版本开始，拥有了这个能力，允许用户自定义日志的模板，
同时在模板中可以使用`SeasLog`预置的诸多预设变量，参照[预设变量表](#预设变量表)。

#### 日志模板说明
* 模板默认为：`seaslog.default_template = "%T | %L | %P | %Q | %t | %M"`
* 意味着，默认的格式为`{dateTime} | {level} | {pid} | {uniqid} | {timeStamp} | {logInfo}`
* 如果自定义的格式为：`seaslog.default_template = "[%T]:%L %P %Q %t %M" `
* 那么，日志格式将被自定义为：`[{dateTime}]:{level} {pid} {uniqid} {timeStamp} {logInfo}`

#### 预设变量表
`SeasLog`提供了下列预设变量，可以直接使用在日志模板中，将在日志最终生成时替换成对应值。
* `%L` - Level 日志级别。
* `%M` - Message 日志信息。
* `%T` - DateTime 如`2017-08-16 19:15:02`，受`seaslog.default_datetime_format`影响。
* `%t` - Timestamp 如`1502882102.862`，精确到毫秒数。
* `%Q` - RequestId 区分单次请求，如没有调用`SeasLog::setRequestId($string)`方法，则在初始化请求时，采用内置的`static char *get_uniqid()`方法生成的惟一值。
* `%H` - HostName 主机名。
* `%P` - ProcessId 进程ID。
* `%D` - Domain:Port 域名:口号，如`www.cloudwise.com:8080`; Cli模式下为`cli`。
* `%R` - Request URI 请求URI，如`/app/user/signin`; Cli模式下为入口文件，如`CliIndex.php`。
* `%m` - Request Method 请求类型，如`GET`; Cli模式下为执行命令，如`/bin/bash`。
* `%I` - Client IP 来源客户端IP; Cli模式下为`local`。取值优先级为：HTTP_X_REAL_IP > HTTP_X_FORWARDED_FOR > REMOTE_ADDR
* `%F` - FileName:LineNo 文件名:行号，如`UserService.php:118`。
* `%U` - MemoryUsage 当前内容使用量，单位byte。调用`zend_memory_usage`。
* `%u` - PeakMemoryUsage 当前内容使用峰值量，单位byte。调用`zend_memory_peak_usage`。
* `%C` - `TODO` Class::Action 类名::方法名，如`UserService::getUserInfo`。

## 使用

### 常量与函数
#### 常量列表
`SeasLog 共将日志分成8个级别`
##### SEASLOG_DEBUG       
* "DEBUG"       - debug信息、细粒度信息事件
##### SEASLOG_INFO
* "INFO"        - 重要事件、强调应用程序的运行过程
##### SEASLOG_NOTICE
* "NOTICE"      - 一般重要性事件、执行过程中较INFO级别更为重要的信息
##### SEASLOG_WARNING
* "WARNING"     - 出现了非错误性的异常信息、潜在异常信息、需要关注并且需要修复
##### SEASLOG_ERROR
* "ERROR"       - 运行时出现的错误、不必要立即进行修复、不影响整个逻辑的运行、需要记录并做检测
##### SEASLOG_CRITICAL
* "CRITICAL"    - 紧急情况、需要立刻进行修复、程序组件不可用
##### SEASLOG_ALERT
* "ALERT"       - 必级立即采取行动的紧急事件、需要立即通知相关人员紧急修复
##### SEASLOG_EMERGENCY
* "EMERGENCY"   - 系统不可用
```php
var_dump(SEASLOG_DEBUG,SEASLOG_INFO,SEASLOG_NOTICE);
/*
string('DEBUG') debug级别
string('INFO')  info级别
string('NOTICE') notice级别
*/
```
#### 函数列表
`SeasLog` 提供了这样一组函数，可以方便地获取与设置根目录、模块目录、快速写入与统计log。
相信从下述伪代码的注释中，您可以快速获取函数信息，具体使用将紧接其后：
```php
<?php
/**
 * @author neeke@php.net
 * Date: 14-1-27 下午4:47
 */
define('SEASLOG_ALL', 'ALL');
define('SEASLOG_DEBUG', 'DEBUG');
define('SEASLOG_INFO', 'INFO');
define('SEASLOG_NOTICE', 'NOTICE');
define('SEASLOG_WARNING', 'WARNING');
define('SEASLOG_ERROR', 'ERROR');
define('SEASLOG_CRITICAL', 'CRITICAL');
define('SEASLOG_ALERT', 'ALERT');
define('SEASLOG_EMERGENCY', 'EMERGENCY');
define('SEASLOG_DETAIL_ORDER_ASC', 1);
define('SEASLOG_DETAIL_ORDER_DESC', 2);

class SeasLog
{
    public function __construct()
    {
        #SeasLog init
    }

    public function __destruct()
    {
        #SeasLog distroy
    }

    /**
     * 设置basePath
     *
     * @param $basePath
     *
     * @return bool
     */
    static public function setBasePath($basePath)
    {
        return TRUE;
    }

    /**
     * 获取basePath
     *
     * @return string
     */
    static public function getBasePath()
    {
        return 'the base_path';
    }
    
    /**
     * 设置本次请求标识
     * @param string
     * @return bool
     */
    static public function setRequestID($request_id){
        return TRUE;
    }
    /**
     * 获取本次请求标识
     * @return string
     */
    static public function getRequestID(){
        return uniqid();
    }

    /**
     * 设置模块目录
     * @param $module
     *
     * @return bool
     */
    static public function setLogger($module)
    {
        return TRUE;
    }

    /**
     * 获取最后一次设置的模块目录
     * @return string
     */
    static public function getLastLogger()
    {
        return 'the lastLogger';
    }

    /**
     * 设置DatetimeFormat配置
     * @param $format
     *
     * @return bool
     */
    static public function setDatetimeFormat($format)
    {
        return TRUE;
    }

    /**
     * 返回当前DatetimeFormat配置格式
     * @return string
     */
    static public function getDatetimeFormat()
    {
        return 'the datetimeFormat';
    }

    /**
     * 统计所有类型（或单个类型）行数
     * @param string $level
     * @param string $log_path
     * @param null   $key_word
     *
     * @return array | long
     */
    static public function analyzerCount($level = 'all', $log_path = '*', $key_word = NULL)
    {
        return array();
    }

    /**
     * 以数组形式，快速取出某类型log的各行详情
     *
     * @param        $level
     * @param string $log_path
     * @param null   $key_word
     * @param int    $start
     * @param int    $limit
     * @param        $order 默认为正序 SEASLOG_DETAIL_ORDER_ASC，可选倒序 SEASLOG_DETAIL_ORDER_DESC
     *
     * @return array
     */
    static public function analyzerDetail($level = SEASLOG_INFO, $log_path = '*', $key_word = NULL, $start = 1, $limit = 20, $order = SEASLOG_DETAIL_ORDER_ASC)
    {
        return array();
    }

    /**
     * 获得当前日志buffer中的内容
     *
     * @return array
     */
    static public function getBuffer()
    {
        return array();
    }

    /**
     * 将buffer中的日志立刻刷到硬盘
     *
     * @return bool
     */
    static public function flushBuffer()
    {
        return TRUE;
    }

    /**
     * 记录debug日志
     *
     * @param        $message
     * @param array  $content
     * @param string $module
     */
    static public function debug($message, array $content = array(), $module = '')
    {
        #$level = SEASLOG_DEBUG
    }

    /**
     * 记录info日志
     *
     * @param        $message
     * @param array  $content
     * @param string $module
     */
    static public function info($message, array $content = array(), $module = '')
    {
        #$level = SEASLOG_INFO
    }

    /**
     * 记录notice日志
     *
     * @param        $message
     * @param array  $content
     * @param string $module
     */
    static public function notice($message, array $content = array(), $module = '')
    {
        #$level = SEASLOG_NOTICE
    }

    /**
     * 记录warning日志
     *
     * @param        $message
     * @param array  $content
     * @param string $module
     */
    static public function warning($message, array $content = array(), $module = '')
    {
        #$level = SEASLOG_WARNING
    }

    /**
     * 记录error日志
     *
     * @param        $message
     * @param array  $content
     * @param string $module
     */
    static public function error($message, array $content = array(), $module = '')
    {
        #$level = SEASLOG_ERROR
    }

    /**
     * 记录critical日志
     *
     * @param        $message
     * @param array  $content
     * @param string $module
     */
    static public function critical($message, array $content = array(), $module = '')
    {
        #$level = SEASLOG_CRITICAL
    }

    /**
     * 记录alert日志
     *
     * @param        $message
     * @param array  $content
     * @param string $module
     */
    static public function alert($message, array $content = array(), $module = '')
    {
        #$level = SEASLOG_ALERT
    }

    /**
     * 记录emergency日志
     *
     * @param        $message
     * @param array  $content
     * @param string $module
     */
    static public function emergency($message, array $content = array(), $module = '')
    {
        #$level = SEASLOG_EMERGENCY
    }

    /**
     * 通用日志方法
     * @param        $level
     * @param        $message
     * @param array  $content
     * @param string $module
     */
    static public function log($level, $message, array $content = array(), $module = '')
    {

    }
}



```

### PHP Re 结果
[SeasLog_PHP_Re_Result](https://github.com/SeasX/SeasLog/blob/master/Specification/SeasLog_PHP_Re_Result.md)



### SeasLog Logger的使用
#### 获取与设置basePath
```php
$basePath_1 = SeasLog::getBasePath();

SeasLog::setBasePath('/log/base_test');
$basePath_2 = SeasLog::getBasePath();

var_dump($basePath_1,$basePath_2);

/*
string(19) "/log/seaslog-ciogao"
string(14) "/log/base_test"
*/
```
> 直接使用 `SeasLog::getBasePath()`，将获取php.ini(seaslog.ini)中设置的 `seaslog.default_basepath` 的值。

> 使用 `SeasLog::setBasePath()` 函数，将改变 `SeasLog::getBasePath()` 的取值。

#### 设置logger与获取lastLogger
```php
$lastLogger_1 = SeasLog::getLastLogger();

SeasLog::setLogger('testModule/app1');
$lastLogger_2 = SeasLog::getLastLogger();

var_dump($lastLogger_1,$lastLogger_2);
/*
string(7) "default"
string(15) "testModule/app1"
*/
```
> 与basePath相类似的，

> 直接使用 `SeasLog::getLastLogger()`，将获取php.ini(seaslog.ini)中设置的 `seaslog.default_logger` 的值。

> 使用 `SeasLog::setLogger()` 函数，将改变 `SeasLog::getLastLogger()` 的取值。

#### 快速写入log
上面已经设置过了basePath与logger，于是log记录的目录已经产生了，
> log记录目录 = basePath / logger / {fileName}.log
log文件名，以 `年月日` 分文件，如今天是2014年02月18日期，那么 `{fileName}` = `20140218`;

还记得 `php.ini` 中设置的 `seaslog.disting_type` 吗？

默认的 `seaslog.disting_type = 0`，如果今天我使用了 `SeasLog` ，那么将产生最终的log文件：
* LogFile = basePath / logger / 20140218.log

如果 `seaslog.disting_type = 1`，则最终的log文件将是这样的三个文件
* infoLogFile = basePath / logger / 20140218.INFO.log

* warnLogFile = basePath / logger / 20140218.WARNING.log

* erroLogFile = basePath / logger / 20140218.ERROR.log

```php

SeasLog::log(SEASLOG_ERROR,'this is a error test by ::log');

SeasLog::debug('this is a {userName} debug',array('{userName}' => 'neeke'));

SeasLog::info('this is a info log');

SeasLog::notice('this is a notice log');

SeasLog::warning('your {website} was down,please {action} it ASAP!',array('{website}' => 'github.com','{action}' => 'rboot'));

SeasLog::error('a error log');

SeasLog::critical('some thing was critical');

SeasLog::alert('yes this is a {messageName}',array('{messageName}' => 'alertMSG'));

SeasLog::emergency('Just now, the house next door was completely burnt out! {note}',array('{note}' => 'it`s a joke'));

/*
这些函数同时也接受第3个参数为logger的设置项
注意，当last_logger == 'default'时等同于:
SeasLog::setLogger('test/new/path');
SeasLog::error('test error 3');
如果已经在前文使用过SeasLog::setLogger()函数，第3个参数的log只在此处临时使用，不影响下文。
*/
```
> log格式受`seaslog.default_template`影响。
> seaslog.default_template默认模板为 seaslog.default_template = "%T | %L | %P | %Q | %t | %M"
> 那么在默认情况下，日志格式为： `{dateTime} | {level} | {pid} | {uniqid} | {timeStamp} | {logInfo}`
> 关于自定义模板，及SeasLog中的预置值，可参阅 [自定义日志模板](#自定义日志模板)
```sh
2014-07-27 08:53:52 | ERROR | 23625 | 599159975a9ff | 1406422432.786 | this is a error test by log
2014-07-27 08:53:52 | DEBUG | 23625 | 599159975a9ff | 1406422432.786 | this is a neeke debug
2014-07-27 08:53:52 | INFO | 23625 | 599159975a9ff | 1406422432.787 | this is a info log
2014-07-27 08:53:52 | NOTICE | 23625 | 599159975a9ff | 1406422432.787 | this is a notice log
2014-07-27 08:53:52 | WARNING | 23625 | 599159975a9ff | 1406422432.787 | your github.com was down,please rboot it ASAP!
2014-07-27 08:53:52 | ERROR | 23625 | 599159975a9ff | 1406422432.787 | a error log
2014-07-27 08:53:52 | CRITICAL | 23625 | 599159975a9ff | 1406422432.787 | some thing was critical
2014-07-27 08:53:52 | EMERGENCY | 23625 | 599159975a9ff | 1406422432.787 | Just now, the house next door was completely burnt out! it is a joke
```

#### 当`seaslog.appender`配置为 `2（TCP）` 或 `3（UDP）` 时，日志将推送至remote_host:remote_port的TCP或UDP端口
> SeasLog发送至远端时，遵循规范[RFC5424](http://www.faqs.org/rfcs/rfc5424.html)
> log格式统一为：`<PRI>1 {timeStampWithRFC3339} {HostName} {loggerName}[{pid}]: {logInfo}`
> 上述`{logInfo}` 受配置  `seaslog.default_template`影响。

```sh
发送出去的格式如：
<15>1 2017-08-27T01:24:59+08:00 vagrant-ubuntu-trusty test/logger[27171]: 2016-06-25 00:59:43 | DEBUG | 21423 | 599157af4e937 | 1466787583.322 | this is a neeke debug
<14>1 2017-08-27T01:24:59+08:00 vagrant-ubuntu-trusty test/logger[27171]: 2016-06-25 00:59:43 | INFO | 21423 | 599157af4e937 | 1466787583.323 | this is a info log
<13>1 2017-08-27T01:24:59+08:00 vagrant-ubuntu-trusty test/logger[27171]: 2016-06-25 00:59:43 | NOTICE | 21423 | 599157af4e937 | 1466787583.324 | this is a notice log
```

### SeasLog Analyzer的使用
#### 快速统计某类型log的count值
`SeasLog`在扩展中使用管道调用shell命令 `grep -wc`快速地取得count值，并返回值(array || int)给PHP。
```php
$countResult_1 = SeasLog::analyzerCount();
$countResult_2 = SeasLog::analyzerCount(SEASLOG_WARNING);
$countResult_3 = SeasLog::analyzerCount(SEASLOG_ERROR,date('Ymd',time()));

var_dump($countResult_1,$countResult_2,$countResult_3);
/*
array(8) {
  ["DEBUG"]=>
  int(3)
  ["INFO"]=>
  int(3)
  ["NOTICE"]=>
  int(3)
  ["WARNING"]=>
  int(3)
  ["ERROR"]=>
  int(6)
  ["CRITICAL"]=>
  int(3)
  ["ALERT"]=>
  int(3)
  ["EMERGENCY"]=>
  int(3)
}


int(7)

int(1)

*/
```
#### 获取某类型log列表
`SeasLog`在扩展中使用管道调用shell命令 `grep -w`快速地取得列表，并返回array给PHP。
```php
$detailErrorArray_inAll   = SeasLog::analyzerDetail(SEASLOG_ERROR);
$detailErrorArray_today   = SeasLog::analyzerDetail(SEASLOG_ERROR,date('Ymd',time()));

var_dump($detailErrorArray_inAll,$detailErrorArray_today);

/*
SeasLog::analyzerDetail(SEASLOG_ERROR) == SeasLog::analyzerDetail(SEASLOG_ERROR,'*');
取当前模块下所有level为 SEASLOG_ERROR 的信息列表:
array(6) {
 [0] =>
  string(66) "2014-02-24 00:14:02 | ERROR | 8568 | 599157af4e937 | 1393172042.717 | test error 3 "
  [1] =>
  string(66) "2014-02-24 00:14:04 | ERROR | 8594 | 5991576584446 | 1393172044.104 | test error 3 "
  [2] =>
  string(66) "2014-02-24 00:14:04 | ERROR | 8620 | 1502697015147 | 1393172044.862 | test error 3 "
  [3] =>
  string(66) "2014-02-24 00:14:05 | ERROR | 8646 | 599159975a9ff | 1393172045.989 | test error 3 "
  [4] =>
  string(66) "2014-02-24 00:14:07 | ERROR | 8672 | 599159986ec28 | 1393172047.882 | test error 3 "
  [5] =>
  string(66) "2014-02-24 00:14:08 | ERROR | 8698 | 5991599981cec | 1393172048.736 | test error 3 "
}

SeasLog::analyzerDetail(SEASLOG_ERROR,date('Ymd',time()));
只取得当前模块下，当前一天内,level为SEASLOG_ERROR 的信息列表:
array(2) {
  [0] =>
  string(66) "2014-02-24 00:14:02 | ERROR | 8568 | 599157af4e937 | 1393172042.717 | test error 3 "
  [1] =>
  string(66) "2014-02-24 00:14:04 | ERROR | 8594 | 5991576584446 | 1393172044.104 | test error 3 "
}

同理，取当月
$detailErrorArray_mouth = SeasLog::analyzerDetail(SEASLOG_ERROR,date('Ym',time()));

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
; SEASLOG_DEBUG      "DEBUG"
; SEASLOG_INFO       "INFO"
; SEASLOG_NOTICE     "NOTICE"
; SEASLOG_WARNING    "WARNING"
; SEASLOG_ERROR      "ERROR"
; SEASLOG_CRITICAL   "CRITICAL"
; SEASLOG_ALERT      "ALERT"
; SEASLOG_EMERGENCY  "EMERGENCY"

test1[module] = test/bb
test1[level] = SEASLOG_ERROR
test1[bar] = 1
test1[mail_to] = gaochitao@weiboyi.com

test2[module] = 222
test2[level] = SEASLOG_WARNING

test3[module] = 333
test3[level] = SEASLOG_CRITICAL

test4[module] = 444
test4[level] = SEASLOG_EMERGENCY

test5[module] = 555
test5[level] = SEASLOG_DEBUG

```
#### crontab配置
```conf
;每天凌晨3点执行
0 3 * * * /path/to/php /path/to/SeasLog/Analyzer/SeasLogAnalyzer.php
```

## 目前已知使用SeasLog的企业
### 企业名单
 - 云智慧 www.cloudwise.com
 - 高德(部分项目)
 - 腾讯(部分项目)
 - Formax  www.jrq.com
 - 重庆易宠科技(中国最大的独立宠物平台) www.epet.com
 - 微财富 www.weicaifu.com
 - 美丽说(部分项目) www.meilishuo.com
 - 百度Crab平台 crab.baidu.com
 - 爱宠医生(中国最大的互联网宠物医疗平台) www.5ichong.com
 - 爱奇艺秀场(美女直播平台) x.pps.tv
 - 更多..

