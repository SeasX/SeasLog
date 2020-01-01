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
define('SEASLOG_CLOSE_LOGGER_STREAM_MOD_ALL', 1);
define('SEASLOG_CLOSE_LOGGER_STREAM_MOD_ASSIGN', 2);
define('SEASLOG_REQUEST_VARIABLE_DOMAIN_PORT', 1);
define('SEASLOG_REQUEST_VARIABLE_REQUEST_URI', 2);
define('SEASLOG_REQUEST_VARIABLE_REQUEST_METHOD', 3);
define('SEASLOG_REQUEST_VARIABLE_CLIENT_IP', 4);

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
        return true;
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
     *
     * @param string
     *
     * @return bool
     */
    static public function setRequestID($request_id)
    {
        return true;
    }

    /**
     * 获取本次请求标识
     * @return string
     */
    static public function getRequestID()
    {
        return uniqid();
    }

    /**
     * 设置模块目录
     *
     * @param $logger
     *
     * @return bool
     */
    static public function setLogger($logger)
    {
        return true;
    }

    /**
     * 手动清除logger的stream流
     *
     * @param int    $model
     * @param string $logger
     *
     * @return bool
     */
    static public function closeLoggerStream($model = SEASLOG_CLOSE_LOGGER_STREAM_MOD_ALL, $logger)
    {
        return true;
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
     *
     * @param $format
     *
     * @return bool
     */
    static public function setDatetimeFormat($format)
    {
        return true;
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
     * 设置请求变量
     *
     * @param $key
     * @param $value
     *
     * @return bool
     */
    static public function setRequestVariable($key, $value)
    {
        return true;
    }

    /**
     * 获取请求变量
     *
     * @param $key
     *
     * @return string
     */
    static public function getRequestVariable($key)
    {
        return '';
    }

    /**
     * 统计所有类型（或单个类型）行数
     *
     * @param string $level
     * @param string $log_path
     * @param null   $key_word
     *
     * @return array | long
     */
    static public function analyzerCount($level = 'all', $log_path = '*', $key_word = null)
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
     * @param        $order
     *
     * @return array
     */
    static public function analyzerDetail($level = SEASLOG_INFO, $log_path = '*', $key_word = null, $start = 1, $limit = 20, $order = SEASLOG_DETAIL_ORDER_ASC)
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
     * 获取是否开启buffer
     *
     * @return bool
     */
    static public function getBufferEnabled()
    {
        return true;
    }

    /**
     * 获取当前buffer count
     *
     * @return int
     */
    static public function getBufferCount()
    {
        return 0;
    }

    /**
     * 将buffer中的日志立刻刷到硬盘
     *
     * @return bool
     */
    static public function flushBuffer()
    {
        return true;
    }

    /**
     * 记录debug日志
     *
     * @param string|array $message
     * @param array        $context
     * @param string       $module
     *
     * @return bool
     */
    static public function debug($message, array $context = array(), $module = '')
    {
        #$level = SEASLOG_DEBUG
        return true;
    }

    /**
     * 记录info日志
     *
     * @param string|array $message
     * @param array        $context
     * @param string       $module
     *
     * @return bool
     */
    static public function info($message, array $context = array(), $module = '')
    {
        #$level = SEASLOG_INFO
        return true;
    }

    /**
     * 记录notice日志
     *
     * @param string|array $message
     * @param array        $context
     * @param string       $module
     *
     * @return bool
     */
    static public function notice($message, array $context = array(), $module = '')
    {
        #$level = SEASLOG_NOTICE
        return true;
    }

    /**
     * 记录warning日志
     *
     * @param string|array $message
     * @param array        $context
     * @param string       $module
     *
     * @return bool
     */
    static public function warning($message, array $context = array(), $module = '')
    {
        #$level = SEASLOG_WARNING
        return true;
    }

    /**
     * 记录error日志
     *
     * @param string|array $message
     * @param array        $context
     * @param string       $module
     *
     * @return bool
     */
    static public function error($message, array $context = array(), $module = '')
    {
        #$level = SEASLOG_ERROR
        return true;
    }

    /**
     * 记录critical日志
     *
     * @param string|array $message
     * @param array        $context
     * @param string       $module
     *
     * @return bool
     */
    static public function critical($message, array $context = array(), $module = '')
    {
        #$level = SEASLOG_CRITICAL
        return true;
    }

    /**
     * 记录alert日志
     *
     * @param string|array $message
     * @param array        $context
     * @param string       $module
     *
     * @return bool
     */
    static public function alert($message, array $context = array(), $module = '')
    {
        #$level = SEASLOG_ALERT
        return true;
    }

    /**
     * 记录emergency日志
     *
     * @param string|array $message
     * @param array        $context
     * @param string       $module
     *
     * @return bool
     */
    static public function emergency($message, array $context = array(), $module = '')
    {
        #$level = SEASLOG_EMERGENCY
        return true;
    }

    /**
     * 通用日志方法
     *
     * @param              $level
     * @param string|array $message
     * @param array        $context
     * @param string       $module
     *
     * @return bool
     */
    static public function log($level, $message, array $context = array(), $module = '')
    {
        return true;
    }

}

