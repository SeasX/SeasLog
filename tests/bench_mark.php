<?php
/**
 * @author ciogao@gmail.com
 * Date: 18/5/15 下午5:55
 */

//seaslog.default_template = '%T | %L | %P | %Q | %t | %M'

$iTotal       = 10000;
$sLogMessage  = 'message';
$sFilePutPath = './file_put_path.log';

function testSeasLog($iTotal, $sLogMessage)
{
    for ($i = 0; $i < $iTotal; $i++) {
        SeasLog::info($sLogMessage);
    }
}

function testSyslog($iTotal, $sLogMessage)
{
    for ($i = 0; $i < $iTotal; $i++) {
        $iTime   = time();
        $log_tmp = date('Y-m-d H:i:s', $iTime) . ' | info | ' . posix_getpid() . ' | ' . rand(1, 100000) . ' | ' . $iTime . ' | ' . $sLogMessage;
        syslog(LOG_SYSLOG, $log_tmp);
    }
}

function testFilePutContents($iTotal, $sLogMessage, $sFilePutPath)
{
    if (file_exists($sFilePutPath)) {
        unlink($sFilePutPath);
    }

    for ($i = 0; $i < $iTotal; $i++) {
        $iTime   = time();
        $log_tmp = date('Y-m-d H:i:s', $iTime) . ' | info | ' . posix_getpid() . ' | ' . rand(1, 100000) . ' | ' . $iTime . ' | ' . $sLogMessage . PHP_EOL;
        file_put_contents($sFilePutPath, $log_tmp, FILE_APPEND);
    }

    if (file_exists($sFilePutPath)) {
        unlink($sFilePutPath);
    }
}


function testFopenOnceClose($iTotal, $sLogMessage, $sFilePutPath)
{
	if (file_exists($sFilePutPath))
	{
		unlink($sFilePutPath);
	}

	$file = fopen($sFilePutPath, "a");

	for ($i = 0; $i < $iTotal; $i++)
	{
		$iTime = time();
		$log_tmp = date('Y-m-d H:i:s', $iTime) . ' | info | ' . posix_getpid() . ' | ' . rand(1, 100000) . ' | ' . $iTime . ' | ' . $sLogMessage . PHP_EOL;
		fwrite($file, $log_tmp);
	}

	fclose($file);

	if (file_exists($sFilePutPath))
	{
		unlink($sFilePutPath);
	}
}

function testFopenEachClose($iTotal, $sLogMessage, $sFilePutPath)
{
	if (file_exists($sFilePutPath))
	{
		unlink($sFilePutPath);
	}

	for ($i = 0; $i < $iTotal; $i++)
	{
		$file = fopen($sFilePutPath, "a");
		$iTime = time();
		$log_tmp = date('Y-m-d H:i:s', $iTime) . ' | info | ' . posix_getpid() . ' | ' . rand(1, 100000) . ' | ' . $iTime . ' | ' . $sLogMessage . PHP_EOL;
		fwrite($file, $log_tmp);
		fclose($file);
	}

	if (file_exists($sFilePutPath))
	{
		unlink($sFilePutPath);
	}
}

///////////////////////////
function getmicrotime()
{
    $t = gettimeofday();

    return ($t['sec'] + $t['usec'] / 1000000);
}

function start_test()
{
    ob_start();

    return getmicrotime();
}

function end_test($start, $name)
{
    global $total;
    $end = getmicrotime();
    ob_end_clean();
    $total += $end - $start;
    $num = number_format($end - $start, 3);
    $pad = str_repeat(" ", 32 - strlen($name) - strlen($num));

    echo $name . $pad . $num . "\n";
    ob_start();

    return getmicrotime();
}

function total()
{
    global $total;
    $pad = str_repeat("-", 32);
    echo $pad . "\n";
    $num = number_format($total, 3);
    $pad = str_repeat(" ", 32 - strlen("Total") - strlen($num));
    echo "Total" . $pad . $num . "\n";
}


$t0 = $t = start_test();
testSeasLog($iTotal, $sLogMessage);
$t = end_test($t, "testSeasLog");
testSyslog($iTotal, $sLogMessage);
$t = end_test($t, "testSyslog");
testFilePutContents($iTotal, $sLogMessage, $sFilePutPath);
$t = end_test($t, "testFilePutContents");
testFopenOnceClose($iTotal, $sLogMessage, $sFilePutPath);
$t = end_test($t, "testFopenOnceClose");
testFopenEachClose($iTotal, $sLogMessage, $sFilePutPath);
$t = end_test($t, "testFopenEachClose");
total($t0, "Total");


