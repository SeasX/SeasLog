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

/* $Id$ */

#ifndef _PHP_SEASLOG_H_
#define _PHP_SEASLOG_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "include/SeasLog.h"

#ifdef PHP_WIN32
#include "win32/time.h"
#include <windows.h>
#elif defined(NETWARE)
#include <sys/timeval.h>
#include <sys/time.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

extern zend_module_entry seaslog_module_entry;
#define phpext_seaslog_ptr &seaslog_module_entry

#ifdef PHP_WIN32
# define PHP_SEASLOG_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
# define PHP_SEASLOG_API __attribute__ ((visibility("default")))
#else
# define PHP_SEASLOG_API
#endif

ZEND_BEGIN_MODULE_GLOBALS(seaslog)
    char *default_basepath;
    char *default_logger;
    char *default_datetime_format;
    char *current_datetime_format;
    int  current_datetime_format_len;
    char *base_path;

    char *host_name;
    int  host_name_len;

	char *request_id;
	int  request_id_len;

	char *process_id;
    int  process_id_len;

    char *default_template;
    char *current_template;
    char *slash_or_underline;

    logger_entry_t *tmp_logger;
    logger_entry_t *last_logger;
    last_sec_entry_t *last_sec;
    last_min_entry_t *last_min;

    zend_bool disting_folder;
    zend_bool disting_type;
    zend_bool disting_by_hour;
    zend_bool use_buffer;
    zend_bool buffer_disabled_in_cli;
    zend_bool enable_buffer_real;
    zend_bool trim_wrap;
    zend_bool throw_exception;
    zend_bool ignore_warning;

    zend_bool trace_notice;
    zend_bool trace_warning;
    zend_bool trace_error;
    zend_bool trace_exception;

    int buffer_size;
    int level;
    int buffer_count;
    int initRComplete;
    int error_loop;
    int recall_depth;

    int appender;
    int appender_retry;
    char *remote_host;
    int remote_port;
    int remote_timeout;
    struct timeval remote_timeout_real;

    request_variable_t *request_variable;

    int in_error;
    char *in_error_filename;
    long in_error_lineno;

#if PHP_VERSION_ID >= 70000
    zval buffer;
    zval logger_list;
    zval stream_list;
#else
    zval *buffer;
    zval *logger_list;
    zval *stream_list;
#endif

ZEND_END_MODULE_GLOBALS(seaslog)

extern ZEND_DECLARE_MODULE_GLOBALS(seaslog);

#ifdef ZTS
#include "TSRM.h"
#define SEASLOG_G(v) TSRMG(seaslog_globals_id, zend_seaslog_globals *, v)
#else
#define SEASLOG_G(v) (seaslog_globals.v)
#endif

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
PHP_METHOD(SEASLOG_RES_NAME, setRequestID);
PHP_METHOD(SEASLOG_RES_NAME, getRequestID);
PHP_METHOD(SEASLOG_RES_NAME, setLogger);
PHP_METHOD(SEASLOG_RES_NAME, getLastLogger);
PHP_METHOD(SEASLOG_RES_NAME, setDatetimeFormat);
PHP_METHOD(SEASLOG_RES_NAME, getDatetimeFormat);
PHP_METHOD(SEASLOG_RES_NAME, analyzerCount);
PHP_METHOD(SEASLOG_RES_NAME, analyzerDetail);
PHP_METHOD(SEASLOG_RES_NAME, getBuffer);
PHP_METHOD(SEASLOG_RES_NAME, getBufferEnabled);
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

#endif /* _PHP_SEASLOG_H_ */

