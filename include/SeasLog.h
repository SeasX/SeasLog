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

#define SEASLOG_RES_NAME                        "SeasLog"
#define SEASLOG_AUTHOR                          "Chitao.Gao  [ neeke@php.net ]"
#define SEASLOG_VERSION                         "2.1.1"
#define SEASLOG_VERSION_ID                      20101
#define SEASLOG_SUPPORTS                        "https://github.com/SeasX/SeasLog"

#define SEASLOG_ALL                             "ALL"
#define SEASLOG_DEBUG                           "DEBUG"
#define SEASLOG_INFO                            "INFO"
#define SEASLOG_NOTICE                          "NOTICE"
#define SEASLOG_WARNING                         "WARNING"
#define SEASLOG_ERROR                           "ERROR"
#define SEASLOG_CRITICAL                        "CRITICAL"
#define SEASLOG_ALERT                           "ALERT"
#define SEASLOG_EMERGENCY                       "EMERGENCY"

#define SEASLOG_ALL_INT                         8
#define SEASLOG_DEBUG_INT                       7
#define SEASLOG_INFO_INT                        6
#define SEASLOG_NOTICE_INT                      5
#define SEASLOG_WARNING_INT                     4
#define SEASLOG_ERROR_INT                       3
#define SEASLOG_CRITICAL_INT                    2
#define SEASLOG_ALERT_INT                       1
#define SEASLOG_EMERGENCY_INT                   0

#define SEASLOG_SYSLOG_FACILITY                 8 //LOG_USER

#define SEASLOG_APPENDER_FILE                   1
#define SEASLOG_APPENDER_TCP                    2
#define SEASLOG_APPENDER_UDP                    3

#define SEASLOG_DETAIL_ORDER_ASC                1
#define SEASLOG_DETAIL_ORDER_DESC               2

#define SEASLOG_EVENT_ERROR                     1
#define SEASLOG_EVENT_EXCEPTION                 2

#define SEASLOG_DIR_MODE                        (mode_t)0777
#define SEASLOG_FILE_MODE                       (mode_t)0666

#define SEASLOG_INIT_FIRST_YES                  1
#define SEASLOG_INIT_FIRST_NO                   2

#define SEASLOG_BUFFER_RE_INIT_YES              1
#define SEASLOG_BUFFER_RE_INIT_NO               2

#define SEASLOG_INITR_COMPLETE_YES              1
#define SEASLOG_INITR_COMPLETE_NO               2

#define SEASLOG_PROCESS_LOGGER_LAST             1
#define SEASLOG_PROCESS_LOGGER_TMP              2

#define SL_S(s)                                 s, sizeof(s) - 1

#define SEASLOG_HASH_VALUE_LOGGER               1
#define SEASLOG_HASH_VALUE_PATH                 2
#define SEASLOG_HASH_VALUE_ACCESS               3

#define SEASLOG_BUFFER_MAX_SIZE                 65535

#define SEASLOG_TRIM_WRAP                       32

#define SEASLOG_EXCEPTION_LOGGER_ERROR          4403
#define SEASLOG_EXCEPTION_CONTENT_ERROR         4406
#define SEASLOG_EXCEPTION_WINDOWS_ERROR         4407

#define SEASLOG_GET_HOST_NULL                   "NoHost"

#define SEASLOG_CLI_KEY                         "cli"
#define SEASLOG_PHPDBG_KEY                      "phpdbg"

#define SEASLOG_REQUEST_VARIABLE_DOMAIN_PORT    1
#define SEASLOG_REQUEST_VARIABLE_REQUEST_URI    2
#define SEASLOG_REQUEST_VARIABLE_REQUEST_METHOD 3
#define SEASLOG_REQUEST_VARIABLE_CLIENT_IP      4

#define SEASLOG_LOGGER_SLASH                    "/"
#define SEASLOG_LOGGER_UNDERLINE                "_"
#define SEASLOG_ASTERISK                        "*"

#define SEASLOG_LOG_LINE_FEED_STR               "\n"
#define SEASLOG_LOG_LINE_FEED_LEN               sizeof(SEASLOG_LOG_LINE_FEED_STR) - 1

#define SEASLOG_GENERATE_CURRENT_TEMPLATE       1
#define SEASLOG_GENERATE_LOG_INFO               2
#define SEASLOG_GENERATE_SYSLOG_INFO            3
#define SEASLOG_GENERATE_LEVEL_TEMPLATE         4
#define SEASLOG_GENERATE_RE_CURRENT_TEMPLATE    5

#define SEASLOG_ANALYZER_DEFAULT_START          1
#define SEASLOG_ANALYZER_DEFAULT_OFFSET         20

#define SEASLOG_STREAM_LIST_DESTROY_YES         1
#define SEASLOG_STREAM_LIST_DESTROY_NO          2

#define SEASLOG_CLOSE_LOGGER_STREAM_MOD_ALL     1
#define SEASLOG_CLOSE_LOGGER_STREAM_MOD_ASSIGN  2
#define SEASLOG_CLOSE_LOGGER_STREAM_CAN_DELETE  3

#define SEASLOG_LOG_FUNCTION_ARGC_USUAL         1
#define SEASLOG_LOG_FUNCTION_ARGC_UNUSUAL       2

#define SEASLOG_GLOBAL_VARS_SERVER  		    TRACK_VARS_SERVER

#define SEASLOG_PERFORMANCE_LOGGER              "performance"
#define SEASLOG_PERFORMANCE_ROOT_SYMBOL         "main()"
#define SEASLOG_PERFORMANCE_COUNTER_SIZE        1024
#define SEASLOG_PERFORMANCE_BUCKET_SLOTS        8192

#define SEASLOG_CONTINUE                        3

#ifndef PHP_STREAM_URL_STAT_NOCACHE
#define PHP_STREAM_URL_STAT_NOCACHE	            4
#endif

typedef struct _logger_entry_t
{
    zend_ulong logger_hash;
    char *folder;
    char *logger;
    int logger_len;
    char *logger_path;
    int logger_path_len;
    int logger_access;
} logger_entry_t;

typedef struct _stream_entry_t
{
    char *opt;
    int opt_len;
    zend_ulong stream_entry_hash;
    php_stream *stream;
    int can_delete;
} stream_entry_t;

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

typedef struct seaslog_performance_main_t
{
    long    wt_start;
    long    mu_start;
    long    wall_time;
    long    memory;
} seaslog_performance_main;

typedef struct seaslog_frame
{
    char                    *function_name;
    char                    *class_name;
    int                     recurse_level;
    long                    stack_level;
    zend_ulong              hash_code;
    long                    wt_start;
    long int                mu_start;
    struct seaslog_frame    *previous_frame;
} seaslog_frame;

typedef struct seaslog_performance_bucket_t
{
    zend_ulong      bucket_key;
    zend_ulong      hash_code;
    char            *function_name;
    char            *class_name;
    long            stack_level;
    long            count;
    long            wall_time;
    long            memory;

    struct seaslog_performance_bucket_t *next;
} seaslog_performance_bucket;

typedef struct seaslog_performance_result_t
{
    zend_ulong      hash_code;
    char            *function;
    long            count;
    long            wall_time;
    long            memory;
} seaslog_performance_result;

#define SEASLOG_LOGO_DATA "data:image/png;base64,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"

#define SEASLOG_LOGO_IMG "<a href=\"http://seasx.io\"><img src=\""SEASLOG_LOGO_DATA"\" height=\"20px\" alt=\"SeasLog logo\"></a>\n"

#endif /* _SEASLOG_H_ */

