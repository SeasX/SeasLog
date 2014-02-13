<?php
/**
 * @author ciogao@gmail.com
 * Date: 14-1-27 下午4:47
 */
/**
 * @param $basePath
 * @return bool
 */
function seaslog_set_basepath($basePath)
{
    return TRUE;
}

/**
 * @return string
 */
function seaslog_get_basepath()
{
    return 'base_path';
}

/**
 * @param $msg
 * @param string $module
 * @param int $type
 * @return bool
 */
function seaslog($msg, $type = 1, $module = 'defaultPath')
{
    return TRUE;
}

/**
 * @param $module
 * @return bool
 */
function seaslog_set_logger($module)
{
    return TRUE;
}

/**
 * @return string
 */
function seaslog_get_lastlogger()
{
    return 'lastLogger';
}

/**
 * @param string $type
 * @param string $log_path
 * @return array | long
 */
function seaslog_analyzer_count($type = 'alltype',$log_path = '*')
{
    return array();
}

/**
 * @param $type
 * @param string $log_path
 * @return array
 */
function seaslog_analyzer_detail($type = SEASLOG_TYPE_ERRO,$log_path = '*')
{
    return array();
}