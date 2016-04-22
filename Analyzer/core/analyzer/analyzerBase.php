<?php
/**
 * @author ciogao@gmail.com
 * Date: 14-3-31 下午5:39
 */
namespace core\analyzer;

use lib\alarmer;
use constant\mailTpl;

class analyzerBase
{

    static public function analyzerRun($configKey = NULL, $config = array())
    {
        if (empty($configKey) || !is_array($config) || count($config) < 1) {
            throw new \Exception('$configKey can`t be empty || $config must be an array.');
        }

        \SeasLog::setLogger($config['module']);

        $logLevel = intval($config['level']);
        if (empty($logLevel)) $logLevel = SEASLOG_ERROR;

        $_analyzer_count = \SeasLog::analyzerCount($logLevel);

        $_count_bar = array_key_exists('bar', $config) ? intval($config['bar']) : 1;

        if ($_analyzer_count > 0 && $_analyzer_count >= $_count_bar) {

            $to  = array_key_exists('mail_to', $config) ? explode(',', $config['mail_to']) : array();
            $cc  = array_key_exists('mail_cc', $config) ? explode(',', $config['mail_cc']) : array();
            $bcc = array_key_exists('mail_bcc', $config) ? explode(',', $config['mail_bcc']) : array();

            $subject = ' report - ' . $configKey . ' - ' . date('Y-m-d H:i:s', time());
            $_title  = $config['module'] . ' 检测到 ' . $_analyzer_count . "个问题\n\n";

            $_detail = \SeasLog::analyzerDetail($logLevel);
            $_detail = '以下是此次检测详情<br /><br />' . implode("<br />", $_detail);
            $content = mailTpl::processMail($_title, $_detail, 'processed time ' . date('Y-m-d H:i:s', time()));
            alarmer::instanse()->SendEmail($subject, $content, $to, $cc, $bcc);
        }
    }
}