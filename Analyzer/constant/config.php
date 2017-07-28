<?php
/**
 * 常量配置
 * @author ciogao@gmail.com
 * Date: 14-3-8 下午4:51
 */
namespace constant;

class config
{
    //最小进程数
    const PCNTL_FORK_COUNT_MIN = 1;

    static private $config = array();

    /**
     * 取得配置项
     * @throws \Exception
     * @return array
     */
    static public function getConfig()
    {
        if (count(self::$config) > 1) return self::$config;

        $ini    = BASE_PATH . '/config/config.ini';
        $config = @parse_ini_file($ini, TRUE);

        if (!is_array($config) || count($config) < 1) {
            throw new \Exception($ini . ' is null');
        }

        self::$config = $config;

        return self::$config;
    }

    /**
     * 是否开启多线程
     * @return bool|int
     */
    static public function getForkCount()
    {
        if (!array_key_exists('fork', config::$config) || !array_key_exists('fork_open', config::$config['fork'])) {
            return FALSE;
        }

        if (!config::$config['fork']['fork_open'] || intval(config::$config['fork']['fork_count']) < 1) {
            return FALSE;
        }

        return intval(config::$config['fork']['fork_count']);
    }

    /**
     * 取得email配置
     * @return array
     */
    static public function getAnalyzEmail()
    {
        if (!array_key_exists('warning', config::$config) || !array_key_exists('email', config::$config['warning'])) return array();

        $emailConfig             = config::$config['warning']['email'];
        $emailConfig['mail_to']  = explode(',', $emailConfig['mail_to']);
        $emailConfig['mail_cc']  = explode(',', $emailConfig['mail_cc']);
        $emailConfig['mail_bcc'] = explode(',', $emailConfig['mail_bcc']);

        return $emailConfig;
    }

    /**
     * 取得待分析配置
     * @return mixed
     */
    static public function getAnalyz()
    {
        if (!array_key_exists('analyz', config::$config) || !is_array(config::$config['analyz'])) return array();
        return (array)config::$config['analyz'];
    }

    /**
     * 取得待分析日志目录
     * @return mixed
     */
    static public function getAnalyzerPath()
    {
        return (string)config::$config['base']['wait_analyz_log_path'];
    }
}