<?php
/**
 * @author neeke@php.net
 */

ini_set('seaslog.disting_type', 1);

$countResult = SeasLog::analyzerCount();

ini_set('seaslog.disting_type', 1);

SeasLog::setBasePath(realpath('.'));

SeasLog::setLogger('log');

SeasLog::info('log info test');

print_r($countResult);