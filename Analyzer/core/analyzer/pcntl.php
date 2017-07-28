<?php
/**
 * 多进程并发处理
 *
 * @author ciogao@gmail.com
 * Date: 14-3-8 下午2:27
 */
namespace core\analyzer;

use constant\config as config;

class pcntl extends analyzerBase
{
    /**
     * 是否等待进程结束
     * @var bool
     */
    static private $waitFlag = FALSE;

    static public function setWait($bool)
    {
        self::$waitFlag = (bool)$bool;
    }

    static private $forkCount = 1;

    static public function setForkCount($num)
    {
        self::$forkCount = (int)$num;
    }

    static private function getBatchArray()
    {
        $_analyz       = config::getAnalyz();
        $_analyz_count = count($_analyz);

        //只有一个线程时，作一个批次
        if (self::$forkCount <= config::PCNTL_FORK_COUNT_MIN) {
            self::$forkCount = config::PCNTL_FORK_COUNT_MIN;
            return array($_analyz);
        }

        //线程数大于配置数，每个配置一个线程
        if ($_analyz_count <= self::$forkCount) {
            self::$forkCount = $_analyz_count;
            return array_chunk($_analyz, 1, TRUE);
        }

        //平均分配置到线程数，最后一个线程分得最多个
        $_mod = $_analyz_count % self::$forkCount;
        if ($_mod == 0) {
            return array_chunk($_analyz, self::$forkCount, TRUE);
        } else {
            $_batchSlice = floor($_analyz_count / self::$forkCount);

            $array_pop = array_slice($_analyz, 0 - $_mod);
            $_analyz   = array_diff_key($_analyz, $array_pop);

            $_batchArray = array_chunk($_analyz, $_batchSlice, TRUE);
            array_push($_batchArray, array_merge(array_pop($_batchArray), $array_pop));
            return $_batchArray;
        }
    }

    static public function run()
    {
        if (!function_exists('pcntl_fork')) throw new \Exception('pcntl ext not exists');

        $batchArray = self::getBatchArray();

        $pids = array();
        for ($i = 0; $i < self::$forkCount; $i++) {
            $pids[$i] = pcntl_fork();

            if ($pids[$i] == -1) {
                throw new \Exception('analyzer couldn`t fork');
            } elseif (!$pids[$i]) {

                if (is_array($batchArray[$i]) && count($batchArray[$i]) > 0) {
                    foreach ($batchArray[$i] as $rName => $config) {
                        self::analyzerRun($rName, $config);
                    }
                }

                \SeasLog::info('Children Process Done.', array(), 'SeasLogAnalyzer');
                exit(0);
            }

            if (self::$waitFlag) {
                pcntl_waitpid($pids[$i], $status, WUNTRACED);
                echo "wait $i -> " . time() . "\n";
            }
        }

        \SeasLog::info('Master Process Done. And Children Process ids => {MyPids}', array('{MyPids}' => json_encode($pids)), 'SeasLogAnalyzer');
    }
}

