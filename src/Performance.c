/*
  +----------------------------------------------------------------------+
  | SeasLog                                                              |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Chitao Gao  <neeke@php.net>                                  |
  +----------------------------------------------------------------------+
*/


static void seaslog_memory_usage(smart_str *buf TSRMLS_DC)
{
    long int usage = zend_memory_usage(0 TSRMLS_CC);
    smart_str_append_long(buf, usage);

    smart_str_0(buf);
}

static void seaslog_peak_memory_usage(smart_str *buf TSRMLS_DC)
{
    long int usage = zend_memory_peak_usage(0 TSRMLS_CC);
    smart_str_append_long(buf, usage);

    smart_str_0(buf);
}

