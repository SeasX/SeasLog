SeasLog
======
[![Build Status](https://travis-ci.org/SeasX/SeasLog.svg?branch=master)](https://travis-ci.org/SeasX/SeasLog)

An effective,fast,stable log extension for PHP

@author Chitao.Gao [neeke@php.net]

[中文文档](https://github.com/SeasX/SeasLog/blob/master/README_zh.md)

> ---
- **[Synopsis](#synopsis)**
    - **[Why use seaslog](#why-use-seaslog)**
    - **[What is provided at present](#what-is-provided-at-present)**
    - **[What is the target](#what-is-the-target)**
- **[Install](#install)**
    - **[Make Install SeasLog](#make-Install-SeasLog)**
    - **[seaslog.ini](#seaslog.ini)**
    - **[Custom log template](#custom-log-template)**
        - [Log template overview](#log-template-overview)
        - [Default variable table](#default-variable-table)
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
![SeasLogVSlog4php](https://raw.githubusercontent.com/SeasX/SeasLog/master/tests/SeasLogVSlog4php.png)


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
* Support Connect the TCP port, send with RFC5424
* Support Connect the UDP port, send with RFC5424
* Support RequestId differentiated requests
* Support for log template customizations

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
[SeasLog]
;configuration for php SeasLog module
extension = seaslog.so

;Default Log Base Path
seaslog.default_basepath = "/var/log/www"

;Default Logger Path
seaslog.default_logger = "default"

;The DateTime Style.  Default "Y-m-d H:i:s"
seaslog.default_datetime_format = "Y-m-d H:i:s"

;Default Log template. 
;Default "%T | %L | %P | %Q | %t | %M"
seaslog.default_template = "%T | %L | %P | %Q | %t | %M"

;Switch use the logger with folder. 
;1-Y(Default) 0-N
seaslog.disting_folder = 1

;Switch use the logger with type.
;1-Y 0-N(Default)
seaslog.disting_type = 0

;Switch use the logger with hour.
;1-Y 0-N(Default)
seaslog.disting_by_hour = 0

;Switch use the log buffer with memory.
;1-Y 0-N(Default)
seaslog.use_buffer = 0

;The buffer size
seaslog.buffer_size = 100

;Record logger level. 
;0-EMERGENCY 1-ALERT 2-CRITICAL 3-ERROR 4-WARNING 5-NOTICE 6-INFO 7-DEBUG 8-ALL
;Default 8 (All of them).
;
;   Tips: The configuration item has changed since the 1.7.0 version.
;   Before the 1.7.0 version, the smaller the value, the more logs are taken according to the level: 
;   0-all 1-debug 2-info 3-notice 4-warning 5-error 6-critical 7-alert 8-emergency
;   Before the 1.7.0 version, Default 0 (All of them).
seaslog.level = 8

;Log function recall depth
;Will affected variable `LineNo` in `%F`
;Default 0
seaslog.recall_depth = 0

;Automatic Record final error with default logger. 
;1-Y(Default) 0-N
seaslog.trace_error = 1

;Automatic Record exception with default logger. 
;1-Y 0-N(Default)
seaslog.trace_exception = 0

;Switch the Record Log Data Store.     
;1File 2TCP 3UDP (Switch default 1)
seaslog.appender = 1

;Record Log Retry Count 
;Default 0 (Do Not Retry)
seaslog.appender_retry = 0

;If you use  Record TCP or UDP, configure this remote ip.
;Default "127.0.0.1"
seaslog.remote_host = "127.0.0.1"

;If you use Record TCP or UDP, configure this remote port.
;Default 514
seaslog.remote_port = 514

;Trim the \n and \r in log message.
;1-On 0-Off(Default)
seaslog.trim_wrap = 0

;Switch throw SeasLog exception.
;1-On(Default) 0-Off
seaslog.throw_exception = 1

;Switch ignore SeasLog warning.
;1-On(Default) 0-Off
seaslog.ignore_warning = 1
```
> `seaslog.disting_folder = 1` Switch use Logger DisTing by folder, it’s meaning SeasLog will create the file deistic by folder, and when this configure close SeasLog will create file use underline connect Logger and Time like default_20180211.log.

> `seaslog.disting_type = 1` Switch use Logger DisTing by type, it’s meaning SeasLog will create the file deistic info\warn\error and the other type.

> `seaslog.disting_by_hour = 1` Switch use Logger DisTing by hour. It’s  meaning SeasLog will create the file each one hour.

> `seaslog.use_buffer = 1` Switch the configure use_buffer on. The use_buffer switch default off. If switch use_buffer on, SeasLog prerecord the log with memory, and them would be rewritten down into the Data Store by request shutdown or php process exit (PHP RSHUTDOWN or PHP MSHUTDOWN).

> `seaslog.buffer_size = 100` Configure the buffer_size with 100.  The buffer_size default 0, it’s meaning don’t use buffer. If buffer_size > 0,   SeasLog will rewritten down into the Data Store when the prerecorded log in memory count >= this buffer_size,and then refresh the memory poll.

> `seaslog.level = 8` Default logger level.The Default value was 8, it’s meaning SeasLog will record all of the level.

> `seaslog.level = 0` SeasLog will record which level EMERGENCY.

> `seaslog.level = 1` SeasLog will record which level EMERGENCY,ALERT.

> `seaslog.level = 2` SeasLog will record which level EMERGENCY,ALERT,CRITICAL.

> `seaslog.level = 3` SeasLog will record which level EMERGENCY,ALERT,CRITICAL,ERROR.

> `seaslog.level = 4` SeasLog will record which level EMERGENCY,ALERT,CRITICAL,ERROR,WARNING.

> `seaslog.level = 5` SeasLog will record which level EMERGENCY,ALERT,CRITICAL,ERROR,WARNING,NOTICE.

> `seaslog.level = 6` SeasLog will record which level EMERGENCY,ALERT,CRITICAL,ERROR,WARNING,NOTICE,INFO.

> `seaslog.level = 7` SeasLog will record which level EMERGENCY,ALERT,CRITICAL,ERROR,WARNING,NOTICE,INFO,DEBUG.

> `seaslog.throw_exception = 1` Open an exception that throws the SeasLog to throw itself. When the directory authority or the receive server port is blocked, throw an exception; do not throw an exception when closed.

> `seaslog.ignore_warning = 1` Open a warning to ignore SeasLog itself. When directory permissions or receive server ports are blocked, they are ignored; when closed, a warning is thrown.

### Custom log template
Many friends mentioned the need for custom log templates during their use, 
so `SeasLog` began with the 1.7.2 version and has the ability to allow users to customize the template for the log.

At the same time, many preset variables that are preset by `SeasLog` can be used in the template,please reference[Default variable table](#default-variable-table)。

#### Log template overview
* Default log template is:`seaslog.default_template = "%T | %L | %P | %Q | %t | %M"`,
* that's mean,default log style is:`{dateTime} | {level} | {pid} | {uniqid} | {timeStamp} | {logInfo}`

* If you custom log template, such as:`seaslog.default_template = "[%T]:%L %P %Q %t %M" `
* that's will mean,log style was custom as:`[{dateTime}]:{level} {pid} {uniqid} {timeStamp} {logInfo}`
> Tips：The `%L` must before than the`%M`,that is：the log level must before than the log message。

#### Default variable table
`SeasLog`The following default variables are provided, which can be used directly in the log template and replaced as a corresponding value when the log is eventually generated.
* `%L` - Level.
* `%M` - Message.
* `%T` - DateTime. Such as`2017-08-16 19:15:02`,affected by `seaslog.default_datetime_format`.
* `%t` - Timestamp. Such as`1502882102.862`,accurate to milliseconds.
* `%Q` - RequestId. To distinguish a single request, such as not invoking the `SeasLog::setRequestId($string)` function，the unique value generated by the built-in `static char *get_uniqid ()` function is used when the request is initialized.。
* `%H` - HostName.
* `%P` - ProcessId.
* `%D` - Domain:Port. Such as`www.cloudwise.com:80`; When Cli, Such as `cli`.
* `%R` - Request URI. Such as`/app/user/signin`; When Cli it's the index script, Such as `CliIndex.php`.
* `%m` - Request Method. Such as`Get`; When Cli it's the command script, Such as `/bin/bash`.
* `%I` - Client IP; When Cli it's `local`. Priority value: HTTP_X_REAL_IP > HTTP_X_FORWARDED_FOR > REMOTE_ADDR
* `%F` - FileName:LineNo. Such as `UserService.php:118`.
* `%U` - MemoryUsage. byte. Call `zend_memory_usage`.
* `%u` - PeakMemoryUsage. byte. Call `zend_memory_peak_usage`。
* `%C` - `TODO` Class::Action. Such as `UserService::getUserInfo`

## Use age

### Constants and functions
#### Constant list
`SeasLog exposes log level by eight levels.`
##### SEASLOG_DEBUG       
* "DEBUG"       - Detailed debug information.Fine-grained information events.
##### SEASLOG_INFO
* "INFO"        - Interesting events.Emphasizes the running process of the application.
##### SEASLOG_NOTICE
* "NOTICE"      - Normal but significant events.Information that is more important than the INFO level during execution.
##### SEASLOG_WARNING
* "WARNING"     - Exceptional occurrences that are not errors.Potentially aberrant information that needs attention and needs to be repaired.
##### SEASLOG_ERROR
* "ERROR"       - Runtime errors that do not require immediate action but should typically.
##### SEASLOG_CRITICAL
* "CRITICAL"    - Critical conditions.Need to be repaired immediately, and the program component is unavailable.
##### SEASLOG_ALERT
* "ALERT"       - Action must be taken immediately.Immediate attention should be given to relevant personnel for emergency repairs.
##### SEASLOG_EMERGENCY
* "EMERGENCY"   - System is unusable.
```php
var_dump(SEASLOG_DEBUG,SEASLOG_INFO,SEASLOG_NOTICE);
/*
string('DEBUG') debug level
string('INFO')  info level
string('NOTICE') notice level
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
     * Set The requestID
     * @param string
     * @return bool
     */
    static public function setRequestID($request_id){
        return TRUE;
    }
    /**
     * Get The requestID
     * @return string
     */
    static public function getRequestID(){
        return uniqid();
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
      Current = 'Y-m-d H:i:s'
    }
    Entry [ seaslog.default_template <ALL> ]
      Current = '%L | %P | %Q | %t | %T | %M'
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
    Entry [ seaslog.trim_wrap <ALL> ]
      Current = '0'
    }
    Entry [ seaslog.throw_exception <ALL> ]
      Current = '1'
    }
    Entry [ seaslog.ignore_warning <ALL> ]
      Current = '1'
    }
  }

  - Constants [16] {
    Constant [ string SEASLOG_VERSION ] { 1.7.5 }
    Constant [ string SEASLOG_AUTHOR ] { Chitao.Gao  [ neeke@php.net ] }
    Constant [ string SEASLOG_ALL ] { ALL }
    Constant [ string SEASLOG_DEBUG ] { DEBUG }
    Constant [ string SEASLOG_INFO ] { INFO }
    Constant [ string SEASLOG_NOTICE ] { NOTICE }
    Constant [ string SEASLOG_WARNING ] { WARNING }
    Constant [ string SEASLOG_ERROR ] { ERROR }
    Constant [ string SEASLOG_CRITICAL ] { CRITICAL }
    Constant [ string SEASLOG_ALERT ] { ALERT }
    Constant [ string SEASLOG_EMERGENCY ] { EMERGENCY }
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

        Method [ <internal:SeasLog> static public method setRequestID ] {

          - Parameters [1] {
            Parameter #0 [ <required> $request_id ]
          }
        }

        Method [ <internal:SeasLog> static public method getRequestID ] {
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
> logSeasLog will record which level 目录 = basePath / logger / {fileName}.log
Log filename, begin with `YearMonthDay`.It will be `{fileName}` = `20140218`,because today is 02-18-2014.

Would you remember that `seaslog.disting_type` in `php.ini`?

Switch Default `seaslog.disting_type = 0`, If we used `SeasLog` today, the log file will been finally :
* LogFile = basePath / logger / 20140218.log

If `seaslog.disting_type = 1`, the log file would be looks like
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
These functions can receive `logger` cased by the third param all the time.
Tips:  `last_logger == ‘default’` will be used same to:
SeasLog::setLogger('test/new/path');
SeasLog::error('test error 3');
The `logger` cased by the third param would be used right this right now, like a temp logger, when the function SeasLog::setLogger() called in pre content.
*/
```

> Log style affected by`seaslog.default_template`.
> As the default, `seaslog.default_template = "%T | %L | %P | %Q | %t | %M"`
> that's mean,as the default,log record style is： `{dateTime} | {level} | {pid} | {uniqid} | {timeStamp} | {logInfo}`.
> About custom log template,and how to use default variable table,see [Custom log template](#custom-log-template)
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

#### SeasLog will send log to  tcp://remote_host:remote_port or udp://remote_host:remote_port server, when `seaslog.appender` configured to `2 (TCP)` or `3 (UDP)`.
> When `SeasLog` send log to TCP/UDP，style follow [RFC5424](http://www.faqs.org/rfcs/rfc5424.html)
> log style is formatted by SeasLog with send header:`<PRI>1 {timeStampWithRFC3339} {HostName} {loggerName}[{pid}]: {logInfo}`
> The `{logInfo}` affected by  `seaslog.default_template`。

```sh
The log style finally formatted such as:
<15>1 2017-08-27T01:24:59+08:00 vagrant-ubuntu-trusty test/logger[27171]: 2016-06-25 00:59:43 | DEBUG | 21423 | 599157af4e937 | 1466787583.322 | this is a neeke debug
<14>1 2017-08-27T01:24:59+08:00 vagrant-ubuntu-trusty test/logger[27171]: 2016-06-25 00:59:43 | INFO | 21423 | 599157af4e937 | 1466787583.323 | this is a info log
<13>1 2017-08-27T01:24:59+08:00 vagrant-ubuntu-trusty test/logger[27171]: 2016-06-25 00:59:43 | NOTICE | 21423 | 599157af4e937 | 1466787583.324 | this is a notice log
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
  string(83) "2014-02-24 00:14:02 | ERROR | 8568 | 599157af4e937 | 1393172042.717 | test error 3 "
  [1] =>
  string(83) "2014-02-24 00:14:04 | ERROR | 8594 | 5991576584446 | 1393172044.104 | test error 3 "
  [2] =>
  string(83) "2014-02-24 00:14:04 | ERROR | 8620 | 1502697015147 | 1393172044.862 | test error 3 "
  [3] =>
  string(83) "2014-02-24 00:14:05 | ERROR | 8646 | 599159975a9ff | 1393172045.989 | test error 3 "
  [4] =>
  string(83) "2014-02-24 00:14:07 | ERROR | 8672 | 599159986ec28 | 1393172047.882 | test error 3 "
  [5] =>
  string(83) "2014-02-24 00:14:08 | ERROR | 8698 | 5991599981cec | 1393172048.736 | test error 3 "
}

SeasLog::analyzerDetail(SEASLOG_ERROR,date('Ymd',time()));
Get all log list of the level SEASLOG_ERROR under this logger and today.
array(2) {
  [0] =>
  string(83) "2014-02-24 00:14:02 | ERROR | 8568 | 599157af4e937 | 1393172042.717 | test error 3 "
  [1] =>
  string(83) "2014-02-24 00:14:04 | ERROR | 8594 | 5991576584446 | 1393172044.104 | test error 3 "
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
#### Crontab configuration
```conf
;Run this shell every day at three clock in the morning.
0 3 * * * /path/to/php /path/to/SeasLog/Analyzer/SeasLogAnalyzer.php
```