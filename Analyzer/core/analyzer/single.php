<?php
/**
 * 单进程处理
 *
 * @author ciogao@gmail.com
 * Date: 14-3-10 上午9:48
 */
namespace core\analyzer;

use constant\config as config;

class single extends analyzerBase
{
    static public function run()
    {
        $analyz = config::getAnalyz();

        if (!is_array($analyz) || count($analyz) < 1) return '';

        foreach ($analyz as $rName => $config) {
            self::analyzerRun($rName, $config);
        }

        \SeasLog::info('Single Process Done.', array(), 'SeasLogAnalyzer');
    }
}