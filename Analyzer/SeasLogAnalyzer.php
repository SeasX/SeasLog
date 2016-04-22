<?php
/**
 * SeasLog分析预警
 *
 * @author ciogao@gmail.com
 * Date: 14-2-8 下午3:00
 */
define('BASE_PATH', realpath(dirname(__FILE__)));
include_once "config/autoload.php";

use core\analyzer;

analyzer::run();