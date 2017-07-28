<?php
/**
 * seaslog_analyzer
 *
 * @author ciogao@gmail.com
 * Date: 14-3-8 下午5:10
 */
namespace core;

use constant\config as config;
use core\analyzer as coreRun;

class analyzer
{
    //当前版本
    const SEASLOG_ANALYZER_VERSION = '1.1.0';

    static public function run()
    {
        config::getConfig();

        \SeasLog::setBasePath(config::getAnalyzerPath());

        if ($forkCount = config::getForkCount()) {
            coreRun\pcntl::setForkCount($forkCount);
            coreRun\pcntl::run();
        } else {
            coreRun\single::run();
        }
    }
}