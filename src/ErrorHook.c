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

#include "ErrorHook.h"
#include "Appender.h"

#if PHP_VERSION_ID < 80000
void (*old_error_cb)(int type, const char *error_filename, const SEASLOG_UINT error_lineno, const char *format, va_list args);
#elif PHP_VERSION_ID<80400
void (*old_error_cb)(int type, const zend_string *error_filename, const SEASLOG_UINT error_lineno, zend_string *message);
#else
void (*old_error_cb)(int type, zend_string *error_filename, const uint32_t error_lineno, zend_string *message);
#endif

static void process_event_error(const char *event_type, int type, char * error_filename, SEASLOG_UINT error_lineno, char * msg TSRMLS_DC)
{
    char *event_str;
    int event_str_len;

    SEASLOG_G(in_error) = 1;
    SEASLOG_G(in_error_filename) = (char *)error_filename;
    SEASLOG_G(in_error_lineno) = (long)error_lineno;

    event_str_len = spprintf(&event_str, 0, "%s - type:%d - file:%s - line:%d - msg:%s", event_type, type, error_filename, error_lineno, msg);

    seaslog_log_ex(1, SEASLOG_CRITICAL, SEASLOG_CRITICAL_INT, event_str, event_str_len, NULL, 0, seaslog_ce TSRMLS_CC);
    efree(event_str);

    SEASLOG_G(in_error) = 0;
}

#if PHP_VERSION_ID < 80000
void seaslog_error_cb(int type, const char *error_filename, const SEASLOG_UINT error_lineno, const char *format, va_list args)
#elif PHP_VERSION_ID <80400
void seaslog_error_cb(int type, const char *error_filename, const SEASLOG_UINT error_lineno,zend_string *message)
#else
void seaslog_error_cb(int type,zend_string *error_filename,const uint32_t error_lineno,zend_string *message)
#endif
{
    TSRMLS_FETCH();
    if (SEASLOG_G(initRComplete) != SEASLOG_INITR_COMPLETE_YES)
    {
#if PHP_VERSION_ID < 80000
        return old_error_cb(type, error_filename, error_lineno, format, args);
#else
        return old_error_cb(type, error_filename, error_lineno, message);
#endif
    }

    if (SEASLOG_G(trace_error)
            || SEASLOG_G(last_min)
            || SEASLOG_G(last_logger)
            || SEASLOG_G(trace_warning)
            || SEASLOG_G(trace_notice))
    {
#if PHP_VERSION_ID < 80000
        char *msg;
        va_list args_copy;

        va_copy(args_copy, args);
        vspprintf(&msg, 0, format, args_copy);
        va_end(args_copy);
#else
        char *msg = ZSTR_VAL(message);
#endif
        if (type == E_ERROR || type == E_PARSE || type == E_CORE_ERROR || type == E_COMPILE_ERROR || type == E_USER_ERROR || type == E_RECOVERABLE_ERROR)
        {
            if (SEASLOG_G(trace_error))
            {
                process_event_error("Error", type, (char *) error_filename, error_lineno, msg TSRMLS_CC);
            }
        }
        else if (type == E_WARNING || type == E_CORE_WARNING || type == E_COMPILE_WARNING || type == E_USER_WARNING)
        {
            if (SEASLOG_G(trace_warning))
            {
                process_event_error("Warning", type, (char *) error_filename, error_lineno, msg TSRMLS_CC);
            }
        }
        else if (type == E_NOTICE || type == E_USER_NOTICE || type == E_STRICT || type == E_DEPRECATED || type == E_USER_DEPRECATED)
        {
            if (SEASLOG_G(trace_notice))
            {
                process_event_error("Notice", type, (char *) error_filename, error_lineno, msg TSRMLS_CC);
            }
        }
#if PHP_VERSION_ID < 80000
        efree(msg);
#endif
    }
#if PHP_VERSION_ID < 80000
    return old_error_cb(type, error_filename, error_lineno, format, args);
#else
    return old_error_cb(type, error_filename, error_lineno, message);
#endif
}

void init_error_hooks(TSRMLS_D)
{
    old_error_cb = zend_error_cb;
    zend_error_cb = seaslog_error_cb;
}

void recovery_error_hooks(TSRMLS_D)
{
    if (old_error_cb)
    {
        zend_error_cb = old_error_cb;
    }
}

