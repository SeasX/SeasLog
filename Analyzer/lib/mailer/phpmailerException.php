<?php
/**
 * @author ciogao@gmail.com
 * Date: 14-3-11 上午11:26
 */
namespace lib\mailer;

use Exception;

class phpmailerException extends Exception
{
    public function errorMessage()
    {
        $errorMsg = '<strong>' . $this->getMessage() . "</strong><br />\n";
        return $errorMsg;
    }
}