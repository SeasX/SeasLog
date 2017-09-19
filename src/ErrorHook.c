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

static void process_event_error(int type, char * error_filename, uint error_lineno, char * msg TSRMLS_DC)
{
    char *event_str;
    int event_str_len;

    SEASLOG_G(in_error) = 1;
    SEASLOG_G(in_error_filename) = (char *)error_filename;
    SEASLOG_G(in_error_lineno) = (long)error_lineno;

    event_str_len = spprintf(&event_str, 0, "Error - type:%d - file:%s - line:%d - msg:%s", type, error_filename, error_lineno, msg);

    seaslog_log_ex(1, SEASLOG_CRITICAL, SEASLOG_CRITICAL_INT, event_str, event_str_len, NULL, 0, seaslog_ce TSRMLS_CC);
    efree(event_str);

    SEASLOG_G(in_error) = 0;
}

void seaslog_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args)
{
    if (type == E_ERROR || type == E_PARSE || type == E_CORE_ERROR || type == E_COMPILE_ERROR || type == E_USER_ERROR || type == E_RECOVERABLE_ERROR)
    {
        char *msg;
        va_list args_copy;
        TSRMLS_FETCH();

        va_copy(args_copy, args);
        vspprintf(&msg, 0, format, args_copy);
        va_end(args_copy);

        process_event_error(type, (char *) error_filename, error_lineno, msg TSRMLS_CC);
        efree(msg);
    }

    old_error_cb(type, error_filename, error_lineno, format, args);
}

static void initErrorHooks(TSRMLS_D)
{
    if (SEASLOG_G(trace_error))
    {
        old_error_cb = zend_error_cb;
        zend_error_cb = seaslog_error_cb;
    }
}

static void recoveryErrorHooks(TSRMLS_D)
{
    if (SEASLOG_G(trace_error))
    {
        if (old_error_cb)
        {
            zend_error_cb = old_error_cb;
        }
    }
}
