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

#ifndef _SEASLOG_H_
#define _SEASLOG_H_

#include "php.h"
#include "php_ini.h"
#include "main/php_main.h"
#include "php_streams.h"
#include "zend_extensions.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "ext/standard/php_string.h"
#include "ext/date/php_date.h"
#include "php7_wrapper.h"

#if PHP_VERSION_ID >= 70000
#include "zend_smart_str.h"
#else
#include "ext/standard/php_smart_str.h"
#endif

#include <stdlib.h>

#define SEASLOG_RES_NAME                    "SeasLog"
#define SEASLOG_AUTHOR                      "Chitao.Gao  [ neeke@php.net ]"
#define SEASLOG_VERSION                     "1.8.5"
#define SEASLOG_VERSION_ID                  10805
#define SEASLOG_SUPPORTS                    "https://github.com/SeasX/SeasLog"

#define SEASLOG_ALL                         "ALL"
#define SEASLOG_DEBUG                       "DEBUG"
#define SEASLOG_INFO                        "INFO"
#define SEASLOG_NOTICE                      "NOTICE"
#define SEASLOG_WARNING                     "WARNING"
#define SEASLOG_ERROR                       "ERROR"
#define SEASLOG_CRITICAL                    "CRITICAL"
#define SEASLOG_ALERT                       "ALERT"
#define SEASLOG_EMERGENCY                   "EMERGENCY"

#define SEASLOG_ALL_INT                     8
#define SEASLOG_DEBUG_INT                   7
#define SEASLOG_INFO_INT                    6
#define SEASLOG_NOTICE_INT                  5
#define SEASLOG_WARNING_INT                 4
#define SEASLOG_ERROR_INT                   3
#define SEASLOG_CRITICAL_INT                2
#define SEASLOG_ALERT_INT                   1
#define SEASLOG_EMERGENCY_INT               0

#define SEASLOG_SYSLOG_FACILITY             8 //LOG_USER

#define SEASLOG_APPENDER_FILE               1
#define SEASLOG_APPENDER_TCP                2
#define SEASLOG_APPENDER_UDP                3

#define SEASLOG_DETAIL_ORDER_ASC            1
#define SEASLOG_DETAIL_ORDER_DESC           2

#define SEASLOG_EVENT_ERROR                 1
#define SEASLOG_EVENT_EXCEPTION             2

#define SEASLOG_DIR_MODE                    (mode_t)0777
#define SEASLOG_FILE_MODE                   (mode_t)0666

#define SEASLOG_INIT_FIRST_YES              1
#define SEASLOG_INIT_FIRST_NO               2

#define SEASLOG_BUFFER_RE_INIT_YES          1
#define SEASLOG_BUFFER_RE_INIT_NO           2

#define SEASLOG_INITR_COMPLETE_YES          1
#define SEASLOG_INITR_COMPLETE_NO           2

#define SEASLOG_PROCESS_LOGGER_LAST         1
#define SEASLOG_PROCESS_LOGGER_TMP          2

#define SL_S(s)                             s, sizeof(s) - 1

#define SEASLOG_HASH_VALUE_LOGGER           1
#define SEASLOG_HASH_VALUE_PATH             2
#define SEASLOG_HASH_VALUE_ACCESS           3

#define SEASLOG_BUFFER_MAX_SIZE             65535

#define SEASLOG_TRIM_WRAP                   26

#define SEASLOG_EXCEPTION_LOGGER_ERROR      4403
#define SEASLOG_EXCEPTION_CONTENT_ERROR     4406
#define SEASLOG_EXCEPTION_WINDOWS_ERROR     4407

#define SEASLOG_GET_HOST_NULL               "NoHost"

#define SEASLOG_CLI_KEY                     "cli"
#define SEASLOG_PHPDBG_KEY                  "phpdbg"

#define SEASLOG_LOGGER_SLASH                "/"
#define SEASLOG_LOGGER_UNDERLINE            "_"
#define SEASLOG_ASTERISK                    "*"

#define SEASLOG_LOG_LINE_FEED_STR           "\n"
#define SEASLOG_LOG_LINE_FEED_LEN           sizeof(SEASLOG_LOG_LINE_FEED_STR) - 1

#define SEASLOG_GENERATE_CURRENT_TEMPLATE   1
#define SEASLOG_GENERATE_LOG_INFO           2
#define SEASLOG_GENERATE_SYSLOG_INFO        3

#define SEASLOG_ANALYZER_DEFAULT_START      1
#define SEASLOG_ANALYZER_DEFAULT_OFFSET     20

#define SEASLOG_GLOBAL_VARS_SERVER  		TRACK_VARS_SERVER

#ifndef PHP_STREAM_URL_STAT_NOCACHE
#define PHP_STREAM_URL_STAT_NOCACHE	        4
#endif

#if PHP_VERSION_ID >= 70000

#define EX_ARRAY_DESTROY(arr) \
	do { \
		zval_ptr_dtor(arr); \
		ZVAL_NULL(arr); \
	} while(0)

#else

#define EX_ARRAY_DESTROY(arr) \
	do { \
		zval_ptr_dtor(arr);\
	} while(0)

#endif

typedef struct _logger_entry_t
{
    ulong logger_hash;
    char *folder;
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

typedef struct _request_variable_t
{
    char *domain_port;
    int domain_port_len;

    char *client_ip;
    int client_ip_len;

    char *request_uri;
    int request_uri_len;

    char *request_method;
    int request_method_len;
} request_variable_t;

#endif /* _SEASLOG_H_ */

