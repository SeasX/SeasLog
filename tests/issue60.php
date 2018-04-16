<?php
/**
 * @author neeke@php.net
 * Date: 15-11-9 下午7:16
 */

for($i=0; $i<100; $i++){
    SeasLog::error(time());
    sleep(1);

    var_dump(SeasLog::getBuffer());
}

