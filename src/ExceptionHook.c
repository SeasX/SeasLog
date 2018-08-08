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

#include "ExceptionHook.h"
#include "Appender.h"

static void (*old_throw_exception_hook)(zval *exception TSRMLS_DC);

static void process_event_exception(int type, char * error_filename, uint error_lineno, char * msg TSRMLS_DC)
{
    char *event_str;
    int event_str_len;

    event_str_len = spprintf(&event_str, 0, "Exception - type:%d - file:%s - line:%d - msg:%s", type, error_filename, error_lineno, msg);

    seaslog_log_ex(1, SEASLOG_CRITICAL, SEASLOG_CRITICAL_INT, event_str, event_str_len, NULL, 0, seaslog_ce TSRMLS_CC);
    efree(event_str);
}

void seaslog_throw_exception_hook(zval *exception TSRMLS_DC)
{
    zval *message, *file, *line, *code;
#if PHP_VERSION_ID >= 70000
    zval rv;
#endif
    zend_class_entry *default_ce;

    if (!exception)
    {
        return;
    }

#if PHP_VERSION_ID >= 70000
    default_ce = Z_OBJCE_P(exception);
#else
    default_ce = zend_exception_get_default(TSRMLS_C);
#endif

#if PHP_VERSION_ID >= 70000
    message = zend_read_property(default_ce, exception, "message", sizeof("message")-1, 0, &rv);
    file = zend_read_property(default_ce, exception, "file", sizeof("file")-1, 0, &rv);
    line = zend_read_property(default_ce, exception, "line", sizeof("line")-1, 0, &rv);
    code = zend_read_property(default_ce, exception, "code", sizeof("code")-1, 0, &rv);
#else
    message = zend_read_property(default_ce, exception, "message", sizeof("message")-1, 0 TSRMLS_CC);
    file = zend_read_property(default_ce, exception, "file", sizeof("file")-1, 0 TSRMLS_CC);
    line = zend_read_property(default_ce, exception, "line", sizeof("line")-1, 0 TSRMLS_CC);
    code = zend_read_property(default_ce, exception, "code", sizeof("code")-1, 0 TSRMLS_CC);
#endif

    process_event_exception(Z_LVAL_P(code), Z_STRVAL_P(file), Z_LVAL_P(line), Z_STRVAL_P(message) TSRMLS_CC);

    if (old_throw_exception_hook)
    {
        old_throw_exception_hook(exception TSRMLS_CC);
    }
}

void initExceptionHooks(TSRMLS_D)
{
    if (SEASLOG_G(trace_exception))
    {
        if (zend_throw_exception_hook)
        {
            old_throw_exception_hook = zend_throw_exception_hook;
        }

        zend_throw_exception_hook = seaslog_throw_exception_hook;
    }
}

void recoveryExceptionHooks(TSRMLS_D)
{
    if (SEASLOG_G(trace_exception))
    {
        if (old_throw_exception_hook)
        {
            zend_throw_exception_hook = old_throw_exception_hook;
        }
    }
}

void seaslog_throw_exception(int type TSRMLS_DC, const char *format, ...)
{
    va_list args;
    char *message = NULL;

    if (SEASLOG_G(ignore_warning) && !SEASLOG_G(throw_exception))
    {
        return;
    }

    va_start(args, format);
    vspprintf(&message, 0, format, args);

    if (!SEASLOG_G(ignore_warning))
    {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "[SeasLog] %s", message);
    }

    if (SEASLOG_G(throw_exception)
            && (SEASLOG_G(initRComplete) == SEASLOG_INITR_COMPLETE_YES)
            && (SEASLOG_G(error_loop) <= 1))
    {

        if (type == SEASLOG_EXCEPTION_LOGGER_ERROR)
        {
            SEASLOG_G(error_loop)++;
        }

#if PHP_VERSION_ID >= 70000
        zend_throw_exception_ex(NULL, type, "%s", message);
#else
        zend_throw_exception_ex(NULL, type TSRMLS_CC, "%s", message);
#endif

    }

    efree(message);
    va_end(args);
}

