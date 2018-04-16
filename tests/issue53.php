<?php
/**
 * @Author: leandre
 * @Date:   2015-10-15 11:08:43
 * @Last Modified by:   leandre
 * @Last Modified time: 2015-10-15 11:15:24
 */

SeasLog::setBasePath('./logs/');

SeasLog::setLogger('test');

SeasLog::info('this is a info log');

SeasLog::notice('this is a notice log');

SeasLog::flushBuffer();

sleep(60);

SeasLog::error('a error log');
SeasLog::critical('some thing was critical');

