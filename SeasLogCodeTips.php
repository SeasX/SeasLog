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
