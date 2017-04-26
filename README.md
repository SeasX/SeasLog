SeasLog
======
[![Build Status](https://travis-ci.org/Neeke/SeasLog.svg?branch=master)](https://travis-ci.org/Neeke/SeasLog)

An effective,fast,stable log extension for PHP

@author Chitao.Gao [neeke@php.net]

[中文文档](https://github.com/Neeke/SeasLog/blob/master/README_zh.md)

> ---
- **[Synopsis](#synopsis)**
    - **[Why use seaslog](#why-use-seaslog)**
    - **[What is provided at present](#what-is-provided-at-present)**
    - **[What is the target](#what-is-the-target)**
- **[Install](#install)**
    - **[Make Install SeasLog](#make-Install-SeasLog)**
    - **[seaslog.ini](#seaslog.ini)**
- **[Use age](#use-age)**
    - **[Constants and functions](#constants-and-functions)**
        - [Constant list](#constant-list)
        - [Function list](#function-list)
    - **[PHP Re result](#php-re-result)**
    - **[The useage of seaslog logger](#the-useage-of-seaslog-logger)**
        - [Get and set base path](#get-and-set-base-path)
        - [Get and set logger](#get-and-set-logger)
        - [Fast write log](#fast-write-log)
    - **[The useage of SeasLog Analyzer](#the-useage-of-seaslog-analyzer)**
        - [Fast count some type of log count value](#fast-count-some-type-of-log-count-value)
        - [Acquisit some type of log list](#acquisit-some-type-of-log-list)
    - **[Using seaslog to conduct healthy warning](#using-seaslog-to-conduct-healthy-warning)**
        - [Warning configuration](#warning-configuration)
        - [Crontab configuration](#crontab-configuration)
    - **[Companies are now known to use](#companies-are-now-known-to-use)**
        - [List of companies](#list-of-companies)
        - [Accepted donations](#accepted-donations)


> ---

## Synopsis
### Why use seaslog
The log journal，which is usually the operate record of the system，software and the application record. Through the analysis of the log，it can facilitate users to understand the operation of the system，software and the application situation. If your application log is rich enough，it can also analyze the previous use’s operation behavior，type ，regional distribution or other more information. The application log also points the multiple levels at the same time, you can easily get the application analysis of health status, timely find problems and quick positioning, and solve the problem, remedy the loss.

The error_log, syslog function which built in PHP is powerful and excellent performance, but due to various defects (error_log have no error level, no fixed format, syslog regardless of module, and mix with system log ), reducing a lot of flexibility , and can't meet the application requirements.

The good news is that there are a number of third-party log class library established to make up for the defects, such as log4php, plog, Analog (of course, there are many applications in the project development of the log class). Of which [log4php](http://logging.apache.org/log4php/) is the most famous and excellent design, perfect document format , powerful function，which is recommanded.

But log4php did very badly in terms of performance, below chart is SeasLog with log4php ab concurrent performance test (test environment: Ubuntu12.04 standalone, I3 CPU, memory16g，hard disk SATA 7200) :
![SeasLogVSlog4php](https://raw.githubusercontent.com/Neeke/SeasLog/master/tests/SeasLogVSlog4php.png)


So is there a log of libraries meet the following requirements：
* Modules, classification
* Simple configuration(preferably without configuration)
* Clear log format and easy understanding
* Simple application and well performance

`SeasLog` just meet these demands.

### What is provided at present
* In the PHP project, record the log specification and repidly.
* Configure the default log directory and module
* Specified log directory and capture current configuration
* Preliminary analysis of the early warning framework
* Efficient log buffer and convenient buffer debug
* Follow the [PSR-3](https://github.com/php-fig/fig-standards/blob/master/accepted/PSR-3-logger-interface.md) log interface specification
* Automatically record error information
* Automatically record abnormal information
* Support Connect the TCP port
* Support Connect the UDP port

### What is the target
* Convenient, standarded log records
* Efficient mass log analysis
* Configurable, multi-channel log warning

## Install

### Make Install SeasLog
```sh
$ /path/to/phpize
$ ./configure --with-php-config=/path/to/php-config
$ make && make install
```

### PECL Install SeasLog
```sh
$ pecl install seaslog
```

### Use SeasLog with Windows
Go to PECL/SeasLog and find .dll to install. [PECL/SeasLog Windows Dll](http://pecl.php.net/package/SeasLog/1.6.0/windows) 

### seaslog.ini
```conf
; configuration for php SeasLog module
extension = seaslog.so
seaslog.default_basepath = /log/seaslog-test            ;Default Log Base Path
seaslog.default_logger = default                        ;Default Logger Path
seaslog.disting_type = 1                                ;Switch use the logger with type 1-Y 0-N(Default)
seaslog.disting_by_hour = 1                             ;Switch use the logger with hour 1-Y 0-N(Default)
seaslog.use_buffer = 1                                  ;Switch use the log buffer with memory 1-Y 0-N(Default)
seaslog.buffer_size = 100                               ;The buffer size
seaslog.level = 0                                       ;Record logger level   Default 0 (All of them)
seaslog.trace_error = 1                                 ;Automatic Record final error with default logger. 1-Y(Default) 0-N
seaslog.trace_exception = 0                             ;Automatic Record exception with default logger. 1-Y 0-N(Default)
seaslog.default_datetime_format = "Y:m:d H:i:s"         ;The DateTime Style.  Default “Y:m:d H:i:s"
seaslog.appender = 1                                    ;Switch the Record Log Data Store.     1File 2TCP 3UDP (Switch default 1)
seaslog.remote_host = 127.0.0.1                         ;If you use  Record TCP or UDP, configure this remote ip.(Default 127.0.0.1)
seaslog.remote_port = 514                               ;If you use Record TCP or UDP, configure this remote port.(Default 514)
seaslog.trim_wrap = 0                                   ;Trim the \n and \r in log message. (Default 0)
```
> `seaslog.disting_type = 1` Switch use Logger DisTing by type, it’s meaning SeasLog will create the file deistic info\warn\error and the other type.

> `seaslog.disting_by_hour = 1` Switch use Logger DisTing by hour. It’s  meaning SeasLog will create the file each one hour.

> `seaslog.use_buffer = 1` Switch the configure use_buffer on. The use_buffer switch default off. If switch use_buffer on, SeasLog prerecord the log with memory, and them would be rewritten down into the Data Store by request shutdown or php process exit (PHP RSHUTDOWN or PHP MSHUTDOWN).

> `seaslog.buffer_size = 100` Configure the buffer_size with 100.  The buffer_size default 0, it’s meaning don’t use buffer. If buffer_size > 0,   SeasLog will rewritten down into the Data Store when the prerecorded log in memory count >= this buffer_size,and then refresh the memory poll.

> `seaslog.level = 3` Default logger level. The Default value was 0, it’s meaning all of the level. SeasLog will record the log which level after DEBUG when level setting in 1, and so on. Yes, SeasLog won’t record anything when level setting in 8.

## Use age

### Constants and functions
#### Constant list
`SeasLog exposes log level by eight levels.`
* SEASLOG_DEBUG                       "debug"
* SEASLOG_INFO                        "info"
* SEASLOG_NOTICE                      "notice"
* SEASLOG_WARNING                     "warning"
* SEASLOG_ERROR                       "error"
* SEASLOG_CRITICAL                    "critical"
* SEASLOG_ALERT                       "alert"
* SEASLOG_EMERGENCY                   "emergency"
```php
var_dump(SEASLOG_DEBUG,SEASLOG_INFO,SEASLOG_NOTICE);
/*
string('debug') debug level
string('info')  info level
string('notice') notice level
*/
```
#### Function list
`SeasLog` provides such a set of functions that you can easily get and set the root directory, log module directory, fast write and statistics. 
Believe that from the following pseudo code comments, you can quickly get function information, specific use will follow.

```php
<?php
/**
 * @author neeke@php.net
 * Date: 14-1-27 16:47
 */

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
     * Set The basePath
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
     * Get The basePath
     *
     * @return string
     */
    static public function getBasePath()
    {
        return 'the base_path';
    }

    /**
     * Set The logger
     * @param $module
     *
     * @return bool
     */
    static public function setLogger($module)
    {
        return TRUE;
    }

    /**
     * Get The lastest logger
     * @return string
     */
    static public function getLastLogger()
    {
        return 'the lastLogger';
    }

    /**
     * Set The DatetimeFormat
     * @param $format
     *
     * @return bool
     */
    static public function setDatetimeFormat($format)
    {
        return TRUE;
    }

    /**
     * Get The DatetimeFormat
     * @return string
     */
    static public function getDatetimeFormat()
    {
        return 'the datetimeFormat';
    }

    /**
     * Count All Types（Or Type）Log Lines
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
     * Get The Logs As Array
     *
     * @param        $level
     * @param string $log_path
     * @param null   $key_word
     * @param int    $start
     * @param int    $limit
     * @param        $order SEASLOG_DETAIL_ORDER_ASC(Default)  SEASLOG_DETAIL_ORDER_DESC
     *
     * @return array
     */
    static public function analyzerDetail($level = SEASLOG_INFO, $log_path = '*', $key_word = NULL, $start = 1, $limit = 20, $order = SEASLOG_DETAIL_ORDER_ASC)
    {
        return array();
    }

    /**
     * Get The Buffer In Memory As Array
     *
     * @return array
     */
    static public function getBuffer()
    {
        return array();
    }

    /**
     * Flush The Buffer Dump To Writer Appender
     *
     * @return bool
     */
    static public function flushBuffer()
    {
        return TRUE;
    }

    /**
     * Record Debug Log
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
     * Record Info Log
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
     * Record Notice Log
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
     * Record Warning Log
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
     * Record Error Log
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
     * Record Critical Log
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
     * Record Alert Log
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
     * Record Emergency Log
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
     * The Common Record Log Function
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

### PHP Re result

```php
/usr/local/php/php-7.0.6-zts-debug/bin/php --re seaslog

Extension [ <persistent> extension #32 SeasLog version 1.6.9 ] {

  - Dependencies {
  }

  - INI {
    Entry [ seaslog.default_basepath <ALL> ]
      Current = '/var/log/www'
    }
    Entry [ seaslog.default_logger <ALL> ]
      Current = 'defauult'
    }
    Entry [ seaslog.default_datetime_format <ALL> ]
      Current = 'Y:m:d H:i:s'
    }
    Entry [ seaslog.disting_type <ALL> ]
      Current = '0'
    }
    Entry [ seaslog.disting_by_hour <ALL> ]
      Current = '0'
    }
    Entry [ seaslog.use_buffer <ALL> ]
      Current = '1'
    }
    Entry [ seaslog.trace_error <ALL> ]
      Current = '1'
    }
    Entry [ seaslog.trace_exception <ALL> ]
      Current = '1'
    }
    Entry [ seaslog.buffer_size <ALL> ]
      Current = '10'
    }
    Entry [ seaslog.level <ALL> ]
      Current = '0'
    }
    Entry [ seaslog.appender <ALL> ]
      Current = '1'
    }
    Entry [ seaslog.remote_host <ALL> ]
      Current = '127.0.0.1'
    }
    Entry [ seaslog.remote_port <ALL> ]
      Current = '514'
    }
  }

  - Constants [16] {
    Constant [ string SEASLOG_VERSION ] { 1.6.9 }
    Constant [ string SEASLOG_AUTHOR ] { Chitao.Gao  [ neeke@php.net ] }
    Constant [ string SEASLOG_ALL ] { all }
    Constant [ string SEASLOG_DEBUG ] { debug }
    Constant [ string SEASLOG_INFO ] { info }
    Constant [ string SEASLOG_NOTICE ] { notice }
    Constant [ string SEASLOG_WARNING ] { warning }
    Constant [ string SEASLOG_ERROR ] { error }
    Constant [ string SEASLOG_CRITICAL ] { critical }
    Constant [ string SEASLOG_ALERT ] { alert }
    Constant [ string SEASLOG_EMERGENCY ] { emergency }
    Constant [ integer SEASLOG_DETAIL_ORDER_ASC ] { 1 }
    Constant [ integer SEASLOG_DETAIL_ORDER_DESC ] { 2 }
    Constant [ integer SEASLOG_APPENDER_FILE ] { 1 }
    Constant [ integer SEASLOG_APPENDER_TCP ] { 2 }
    Constant [ integer SEASLOG_APPENDER_UDP ] { 3 }
  }

  - Functions {
    Function [ <internal:SeasLog> function seaslog_get_version ] {
    }
    Function [ <internal:SeasLog> function seaslog_get_author ] {
    }
  }

  - Classes [1] {
    Class [ <internal:SeasLog> class SeasLog ] {

      - Constants [0] {
      }

      - Static properties [0] {
      }

      - Static methods [19] {
        Method [ <internal:SeasLog> static public method setBasePath ] {

          - Parameters [1] {
            Parameter #0 [ <required> $base_path ]
          }
        }

        Method [ <internal:SeasLog> static public method getBasePath ] {
        }

        Method [ <internal:SeasLog> static public method setLogger ] {

          - Parameters [1] {
            Parameter #0 [ <required> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method getLastLogger ] {
        }

        Method [ <internal:SeasLog> static public method setDatetimeFormat ] {

          - Parameters [1] {
            Parameter #0 [ <required> $format ]
          }
        }

        Method [ <internal:SeasLog> static public method getDatetimeFormat ] {
        }

        Method [ <internal:SeasLog> static public method analyzerCount ] {

          - Parameters [3] {
            Parameter #0 [ <required> $level ]
            Parameter #1 [ <optional> $log_path ]
            Parameter #2 [ <optional> $key_word ]
          }
        }

        Method [ <internal:SeasLog> static public method analyzerDetail ] {

          - Parameters [6] {
            Parameter #0 [ <required> $level ]
            Parameter #1 [ <optional> $log_path ]
            Parameter #2 [ <optional> $key_word ]
            Parameter #3 [ <optional> $start ]
            Parameter #4 [ <optional> $limit ]
            Parameter #5 [ <optional> $order ]
          }
        }

        Method [ <internal:SeasLog> static public method getBuffer ] {
        }

        Method [ <internal:SeasLog> static public method flushBuffer ] {
        }

        Method [ <internal:SeasLog> static public method log ] {

          - Parameters [4] {
            Parameter #0 [ <required> $level ]
            Parameter #1 [ <optional> $message ]
            Parameter #2 [ <optional> $content ]
            Parameter #3 [ <optional> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method debug ] {

          - Parameters [3] {
            Parameter #0 [ <required> $message ]
            Parameter #1 [ <optional> $content ]
            Parameter #2 [ <optional> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method info ] {

          - Parameters [3] {
            Parameter #0 [ <required> $message ]
            Parameter #1 [ <optional> $content ]
            Parameter #2 [ <optional> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method notice ] {

          - Parameters [3] {
            Parameter #0 [ <required> $message ]
            Parameter #1 [ <optional> $content ]
            Parameter #2 [ <optional> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method warning ] {

          - Parameters [3] {
            Parameter #0 [ <required> $message ]
            Parameter #1 [ <optional> $content ]
            Parameter #2 [ <optional> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method error ] {

          - Parameters [3] {
            Parameter #0 [ <required> $message ]
            Parameter #1 [ <optional> $content ]
            Parameter #2 [ <optional> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method critical ] {

          - Parameters [3] {
            Parameter #0 [ <required> $message ]
            Parameter #1 [ <optional> $content ]
            Parameter #2 [ <optional> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method alert ] {

          - Parameters [3] {
            Parameter #0 [ <required> $message ]
            Parameter #1 [ <optional> $content ]
            Parameter #2 [ <optional> $logger ]
          }
        }

        Method [ <internal:SeasLog> static public method emergency ] {

          - Parameters [3] {
            Parameter #0 [ <required> $message ]
            Parameter #1 [ <optional> $content ]
            Parameter #2 [ <optional> $logger ]
          }
        }
      }

      - Properties [0] {
      }

      - Methods [2] {
        Method [ <internal:SeasLog, ctor> public method __construct ] {
        }

        Method [ <internal:SeasLog, dtor> public method __destruct ] {
        }
      }
    }
  }
}

```


### The useage of seaslog logger
#### Get and set base path
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
> Use the Function `SeasLog::getBasePath()` will get the value of `seaslog.default_basepath` what configured in php.ini(seaslog.ini).

> Use the Function `SeasLog::setBasePath()` will change the value of function `SeasLog::getBasePath()`.

#### Get and set logger
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

> Use the Function `SeasLog::getLastLogger()` will get the value of `seaslog.default_logger` what configured in php.ini(seaslog.ini).

> Use the Function `SeasLog::setLogger()` will change the value of function `SeasLog::getLastLogger()`.

#### Fast write log
We have been set the basePath and logger, and then the logger path has be all ready,
> log记录目录 = basePath / logger / {fileName}.log
Log filename, begin with `YearMonthDay`.It will be `{fileName}` = `20140218`,because today is 02-18-2014.

Would you remember that `seaslog.disting_type` in `php.ini`?

Switch Default `seaslog.disting_type = 0`, If we used `SeasLog` today, the log file will been finally :
* LogFile = basePath / logger / 20140218.log

If `seaslog.disting_type = 1`, the log file would be looks like
* infoLogFile = basePath / logger / INFO.20140218.log

* warnLogFile = basePath / logger / WARN.20140218.log

* erroLogFile = basePath / logger / ERRO.20140218.log

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
These functions can receive `logger` cased by the third param all the time.
Tips:  `last_logger == ‘default’` will be used same to:
SeasLog::setLogger('test/new/path');
SeasLog::error('test error 3');
The `logger` cased by the third param would be used right this right now, like a temp logger, when the function SeasLog::setLogger() called in pre content.
*/
```
> log record style is： `{type} | {pid} | {timeStamp} |{dateTime} | {logInfo}`
```sh
error | 23625 | 1406422432.786 | 2014:07:27 08:53:52 | this is a error test by log
debug | 23625 | 1406422432.786 | 2014:07:27 08:53:52 | this is a neeke debug
info | 23625 | 1406422432.787 | 2014:07:27 08:53:52 | this is a info log
notice | 23625 | 1406422432.787 | 2014:07:27 08:53:52 | this is a notice log
warning | 23625 | 1406422432.787 | 2014:07:27 08:53:52 | your github.com was down,please rboot it ASAP!
error | 23625 | 1406422432.787 | 2014:07:27 08:53:52 | a error log
critical | 23625 | 1406422432.787 | 2014:07:27 08:53:52 | some thing was critical
emergency | 23625 | 1406422432.787 | 2014:07:27 08:53:52 | Just now, the house next door was completely burnt out! it is a joke
```

#### SeasLog will send log to  tcp://remote_host:remote_port or udp://remote_host:remote_port server, when `seaslog.appender` configured to `2 (TCP)` or `3 (UDP)`.
> and now log record style is: `{hostName} | {loggerName} | {type} | {pid} | {timeStamp} |{dateTime} | {logInfo}`

```sh
vagrant-ubuntu-trusty | test/logger | error | 21423 | 1466787583.321 | 2016:06:25 00:59:43 | this is a error test by ::log 
vagrant-ubuntu-trusty | test/logger | debug | 21423 | 1466787583.322 | 2016:06:25 00:59:43 | this is a neeke debug 
vagrant-ubuntu-trusty | test/logger | info | 21423 | 1466787583.323 | 2016:06:25 00:59:43 | this is a info log 
vagrant-ubuntu-trusty | test/logger | notice | 21423 | 1466787583.324 | 2016:06:25 00:59:43 | this is a notice log 
vagrant-ubuntu-trusty | test/logger | warning | 21423 | 1466787583.325 | 2016:06:25 00:59:43 | your github.com was down,please rboot it ASAP! 
vagrant-ubuntu-trusty | test/logger | error | 21423 | 1466787583.326 | 2016:06:25 00:59:43 | a error log 
vagrant-ubuntu-trusty | test/logger | critical | 21423 | 1466787583.327 | 2016:06:25 00:59:43 | some thing was critical 
vagrant-ubuntu-trusty | test/logger | alert | 21423 | 1466787583.328 | 2016:06:25 00:59:43 | yes this is a alertMSG 
vagrant-ubuntu-trusty | test/logger | emergency | 21423 | 1466787583.329 | 2016:06:25 00:59:43 | Just now, the house next door was completely burnt out! it`s a joke
```

### The useage of SeasLog Analyzer
#### Fast count some type of log count value
`SeasLog` get count value of `grep -wc` use system pipe and return to PHP (array or int).
```php
$countResult_1 = SeasLog::analyzerCount();
$countResult_2 = SeasLog::analyzerCount(SEASLOG_WARNING);
$countResult_3 = SeasLog::analyzerCount(SEASLOG_ERROR,date('Ymd',time()));

var_dump($countResult_1,$countResult_2,$countResult_3);
/*
array(8) {
  ["debug"]=>
  int(3)
  ["info"]=>
  int(3)
  ["notice"]=>
  int(3)
  ["warning"]=>
  int(3)
  ["error"]=>
  int(6)
  ["critical"]=>
  int(3)
  ["alert"]=>
  int(3)
  ["emergency"]=>
  int(3)
}


int(7)

int(1)

*/
```
#### Acquisit some type of log list
`SeasLog` get count value of `grep -w` use system pipe and return array to PHP.
```php
$detailErrorArray_inAll   = SeasLog::analyzerDetail(SEASLOG_ERROR);
$detailErrorArray_today   = SeasLog::analyzerDetail(SEASLOG_ERROR,date('Ymd',time()));

var_dump($detailErrorArray_inAll,$detailErrorArray_today);

/*
SeasLog::analyzerDetail(SEASLOG_ERROR) == SeasLog::analyzerDetail(SEASLOG_ERROR,'*');
Get all log list of the level SEASLOG_ERROR under this logger:
array(6) {
 [0] =>
  string(66) "error | 8568 | 1393172042.717 | 2014:02:24 00:14:02 | test error 3 "
  [1] =>
  string(66) "error | 8594 | 1393172044.104 | 2014:02:24 00:14:04 | test error 3 "
  [2] =>
  string(66) "error | 8620 | 1393172044.862 | 2014:02:24 00:14:04 | test error 3 "
  [3] =>
  string(66) "error | 8646 | 1393172045.989 | 2014:02:24 00:14:05 | test error 3 "
  [4] =>
  string(66) "error | 8672 | 1393172047.882 | 2014:02:24 00:14:07 | test error 3 "
  [5] =>
  string(66) "error | 8698 | 1393172048.736 | 2014:02:24 00:14:08 | test error 3 "
}

SeasLog::analyzerDetail(SEASLOG_ERROR,date('Ymd',time()));
Get all log list of the level SEASLOG_ERROR under this logger and today.
array(2) {
  [0] =>
  string(66) "error | 8568 | 1393172042.717 | 2014:02:24 00:14:02 | test error 3 "
  [1] =>
  string(66) "error | 8594 | 1393172044.104 | 2014:02:24 00:14:04 | test error 3 "
}

You can also get this month log list:
$detailErrorArray_mouth = SeasLog::analyzerDetail(SEASLOG_ERROR,date('Ym',time()));

*/
```

### Using seaslog to conduct healthy warning
#### Warning configuration
```conf
[base]
wait_analyz_log_path = /log/base_test

[fork]
;Switch open fork thread 1-Y 0-N
fork_open = 1

;thread count
fork_count = 3

[warning]
email[smtp_host] = smtp.163.com
email[smtp_port] = 25
email[subject_pre] = Alert Mail Title -
email[smtp_user] = seaslogdemo@163.com
email[smtp_pwd] = seaslog#demo
email[mail_from] = seaslogdemo@163.com
email[mail_to] = neeke@php.net
email[mail_cc] = ciogao@gmail.com
email[mail_bcc] =

[analyz]
; enum
; SEASLOG_DEBUG      "debug"
; SEASLOG_INFO       "info"
; SEASLOG_NOTICE     "notice"
; SEASLOG_WARNING    "warning"
; SEASLOG_ERROR      "error"
; SEASLOG_CRITICAL   "critical"
; SEASLOG_ALERT      "alert"
; SEASLOG_EMERGENCY  "emergency"

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
#### Crontab configuration
```conf
;Run this shell every day at three clock in the morning.
0 3 * * * /path/to/php /path/to/SeasLog/Analyzer/SeasLogAnalyzer.php
```