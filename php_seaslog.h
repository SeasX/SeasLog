/*
+----------------------------------------------------------------------+
| SeasLog                                                              |
+----------------------------------------------------------------------+
| This source file is subject to version 2.0 of the Apache license,    |
| that is bundled with this package in the file LICENSE, and is        |
| available through the world-wide-web at the following url:           |
| http://www.apache.org/licenses/LICENSE-2.0.html                      |
| If you did not receive a copy of the Apache2.0 license and are unable|
| to obtain it through the world-wide-web, please send a note to       |
| license@php.net so we can mail you a copy immediately.               |
+----------------------------------------------------------------------+
| Author: Neeke.Gao  <neeke@php.net>                                   |
+----------------------------------------------------------------------+
*/

#ifndef PHP_SEASLOG_H
#define PHP_SEASLOG_H

extern zend_module_entry seaslog_module_entry;
#define phpext_seaslog_ptr &seaslog_module_entry

#ifdef PHP_WIN32
# define PHP_SEASLOG_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define PHP_SEASLOG_API __attribute__ ((visibility("default")))
#else
# define PHP_SEASLOG_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef ZTS
#define SEASLOG_G(v) TSRMG(seaslog_globals_id, zend_seaslog_globals *, v)
#else
#define SEASLOG_G(v) (seaslog_globals.v)
#endif

#define SEASLOG_RES_NAME                    "SeasLog"
#define SEASLOG_VERSION                     "1.6.2"
#define SEASLOG_AUTHOR                      "Chitao.Gao  [ neeke@php.net ]"

#define SEASLOG_ALL                         "all"
#define SEASLOG_DEBUG                       "debug"
#define SEASLOG_INFO                        "info"
#define SEASLOG_NOTICE                      "notice"
#define SEASLOG_WARNING                     "warning"
#define SEASLOG_ERROR                       "error"
#define SEASLOG_CRITICAL                    "critical"
#define SEASLOG_ALERT                       "alert"
#define SEASLOG_EMERGENCY                   "emergency"

#define SEASLOG_APPENDER_FILE               1
#define SEASLOG_APPENDER_TCP                2
#define SEASLOG_APPENDER_UDP                3

#define SEASLOG_DETAIL_ORDER_ASC            1
#define SEASLOG_DETAIL_ORDER_DESC           2

#define SEASLOG_EVENT_ERROR                 1
#define SEASLOG_EVENT_EXCEPTION             2

#define SEASLOG_DIR_MODE                    0755
#define SEASLOG_FILE_MODE                   0666

#define SL_S(s)                             s, sizeof(s) - 1

#ifndef E_EXCEPTION
# define E_EXCEPTION (1<<15L)
#endif

PHP_MINIT_FUNCTION(seaslog);
PHP_MSHUTDOWN_FUNCTION(seaslog);
PHP_RINIT_FUNCTION(seaslog);
PHP_RSHUTDOWN_FUNCTION(seaslog);
PHP_MINFO_FUNCTION(seaslog);

PHP_FUNCTION(seaslog_get_version);
PHP_FUNCTION(seaslog_get_author);

zend_class_entry *seaslog_ce;
PHP_METHOD(SEASLOG_RES_NAME, __construct);
PHP_METHOD(SEASLOG_RES_NAME, __destruct);
PHP_METHOD(SEASLOG_RES_NAME, setBasePath);
PHP_METHOD(SEASLOG_RES_NAME, getBasePath);
PHP_METHOD(SEASLOG_RES_NAME, setLogger);
PHP_METHOD(SEASLOG_RES_NAME, getLastLogger);
PHP_METHOD(SEASLOG_RES_NAME, setDatetimeFormat);
PHP_METHOD(SEASLOG_RES_NAME, getDatetimeFormat);
PHP_METHOD(SEASLOG_RES_NAME, analyzerCount);
PHP_METHOD(SEASLOG_RES_NAME, analyzerDetail);
PHP_METHOD(SEASLOG_RES_NAME, getBuffer);
PHP_METHOD(SEASLOG_RES_NAME, flushBuffer);
PHP_METHOD(SEASLOG_RES_NAME, log);
PHP_METHOD(SEASLOG_RES_NAME, debug);
PHP_METHOD(SEASLOG_RES_NAME, info);
PHP_METHOD(SEASLOG_RES_NAME, notice);
PHP_METHOD(SEASLOG_RES_NAME, warning);
PHP_METHOD(SEASLOG_RES_NAME, error);
PHP_METHOD(SEASLOG_RES_NAME, critical);
PHP_METHOD(SEASLOG_RES_NAME, alert);
PHP_METHOD(SEASLOG_RES_NAME, emergency);

#if PHP_VERSION_ID >= 70000
#define EX_ARRAY_DESTROY(arr) zval_ptr_dtor((arr));ZVAL_UNDEF((arr));
#else
#define EX_ARRAY_DESTROY(arr) zval_dtor(arr);FREE_ZVAL(arr);
#endif

typedef struct _logger_entry_t
{
    char *logger;
    int logger_len;
    char *logger_path;
    int logger_path_len;
    int access;
} logger_entry_t;

typedef struct _last_sec_entry_t
{
    int  sec;
    char *real_time;
} last_sec_entry_t;

typedef struct _last_min_entry_t
{
    int  sec;
    char *real_time;
} last_min_entry_t;

ZEND_BEGIN_MODULE_GLOBALS(seaslog)
    char *default_basepath;
    char *default_logger;
    char *default_datetime_format;
    char *current_datetime_format;
    int  current_datetime_format_len;
    char *base_path;
    char *host_name;

    logger_entry_t *logger;
    logger_entry_t *last_logger;
    last_sec_entry_t *last_sec;
    last_min_entry_t *last_min;

    zend_bool disting_type;
    zend_bool disting_by_hour;
    zend_bool use_buffer;
    zend_bool trace_error;
    zend_bool trace_exception;

    int buffer_size;
    int level;
    int buffer_count;

    int appender;
    char *remote_host;
    int remote_port;

#if PHP_VERSION_ID >= 70000
    zval buffer;
    zval logger_list;
#else
    zval *buffer;
    zval *logger_list;
#endif

ZEND_END_MODULE_GLOBALS(seaslog)

#endif /* PHP_SEASLOG_H */

