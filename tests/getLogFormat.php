<?php
SeasLog::setLogFormat(true ,true,true);
$message = SeasLog::getLogFormat();
SeasLog::error('this is a format error');
SeasLog::warning('this is a format warning');
SeasLog::notice('this is a format notice');
var_dump($message);

