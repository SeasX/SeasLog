<?php
ini_set('display_errors', 0);
try {
    throw new Exception("e1");
} catch (Exception $ex) {
    echo $ex->getMessage();
}

