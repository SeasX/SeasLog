<?php
/**
 * @author neeke@php.net
 * Date: 16/5/12 下午4:21
 */

$array = ['key' => 'test array'];
SeasLog::debug('SeasLog Test: {var}', ['{var}' => $array['key']]);

