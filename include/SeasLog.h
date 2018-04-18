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

//Common Toolkit
static int check_sapi_is_cli(TSRMLS_D);
static int seaslog_get_level_int(char *level);
static int check_log_level(int level TSRMLS_DC);
static char *str_appender(char *str, int str_len);
static char *str_replace(char *src, const char *from, const char *to);
static char *delN(char *a);
static char *get_uniqid();
static int message_trim_wrap(char *message,int message_len TSRMLS_DC);
static void initRStart(TSRMLS_D);
static void initREnd(TSRMLS_D);

#if PHP_VERSION_ID >= 70000
static char *php_strtr_array(char *str, int slen, HashTable *pats);
#else
static char *php_strtr_array(char *str, int slen, HashTable *hash);
#endif


//Logger
static void seaslog_init_slash_or_underline(TSRMLS_D);
static void seaslog_init_last_time(TSRMLS_D);
static void seaslog_clear_last_time(TSRMLS_D);
static void seaslog_init_logger(TSRMLS_D);
static void seaslog_init_default_last_logger(TSRMLS_D);
static void seaslog_clear_logger(TSRMLS_D);
static void seaslog_init_logger_list(TSRMLS_D);
static void seaslog_clear_logger_list(TSRMLS_D);
static logger_entry_t *process_logger(char *logger, int logger_len, int last_or_tmp TSRMLS_DC);

//Datetime
static void initRemoteTimeout(TSRMLS_D);
static char *seaslog_process_last_sec(int now, int if_first TSRMLS_DC);
static char *seaslog_process_last_min(int now, int if_first TSRMLS_DC);
static char *seaslog_format_date(char *format, int format_len, time_t ts TSRMLS_DC);
static char *make_real_date(TSRMLS_D);
static char *make_real_time(TSRMLS_D);
static void mic_time(smart_str *buf);
static char *make_time_RFC3339(TSRMLS_D);
static struct timeval seaslog_get_remote_timeout(TSRMLS_D);


//Buffer
static void initBufferSwitch(TSRMLS_D);
static int seaslog_check_buffer_enable(TSRMLS_D);
static void seaslog_init_buffer(TSRMLS_D);
static int real_php_log_buffer(zval *msg_buffer, char *opt, int opt_len TSRMLS_DC);
static int seaslog_buffer_set(char *log_info, int log_info_len, char *path, int path_len, zend_class_entry *ce TSRMLS_DC);
static void seaslog_shutdown_buffer(int re_init TSRMLS_DC);
static void seaslog_clear_buffer(TSRMLS_D);

//ErrorHook
void (*old_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
void seaslog_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
static void process_event_error(const char *event_type, int type, char * error_filename, uint error_lineno, char * msg TSRMLS_DC);
static void initErrorHooks(TSRMLS_D);
static void recoveryErrorHooks(TSRMLS_D);

//ExceptionHook
void (*old_throw_exception_hook)(zval *exception TSRMLS_DC);
void seaslog_throw_exception_hook(zval *exception TSRMLS_DC);
static void process_event_exception(int type, char * error_filename, uint error_lineno, char * msg TSRMLS_DC);
static void initExceptionHooks(TSRMLS_D);
static void recoveryExceptionHooks(TSRMLS_D);
static void seaslog_throw_exception(int type TSRMLS_DC, const char *format, ...);

//Request
static void seaslog_init_host_name(TSRMLS_D);
static void seaslog_clear_host_name(TSRMLS_D);
static void seaslog_init_pid(TSRMLS_D);
static void seaslog_clear_pid(TSRMLS_D);
static void seaslog_init_request_id(TSRMLS_D);
static void seaslog_clear_request_id(TSRMLS_D);
static void seaslog_init_auto_globals(TSRMLS_D);
static void get_code_filename_line(smart_str *result TSRMLS_DC);
static int seaslog_init_request_variable(TSRMLS_D);
static void seaslog_clear_request_variable(TSRMLS_D);
zval *seaslog_request_query(uint type, void *name, size_t len TSRMLS_DC);

//Appender
static int seaslog_real_log_ex(char *message, int message_len, char *opt, int opt_len TSRMLS_DC);
static int seaslog_log_content(int argc, char *level, int level_int, char *message, int message_len, HashTable *content, char *module, int module_len, zend_class_entry *ce TSRMLS_DC);
static int seaslog_log_ex(int argc, char *level, int level_int, char *message, int message_len, char *module, int module_len, zend_class_entry *ce TSRMLS_DC);
static int appender_handle_file(char *message, int message_len, char *level, int level_int, logger_entry_t *logger, zend_class_entry *ce TSRMLS_DC);
static int appender_handle_tcp_udp(char *message, int message_len, char *level, int level_int, logger_entry_t *logger, zend_class_entry *ce TSRMLS_DC);
static int make_log_dir(char *dir TSRMLS_DC);
static int seaslog_real_buffer_log_ex(char *message, int message_len, char *log_file_path, int log_file_path_len, zend_class_entry *ce TSRMLS_DC);

//TemplateFormatter
static void seaslog_init_template(TSRMLS_D);
static void seaslog_clear_template(TSRMLS_D);
static int seaslog_spprintf(char **pbuf TSRMLS_DC, int generate_type, char *level, size_t max_len, ...);
static void seaslog_template_formatter(smart_str *xbuf TSRMLS_DC, int generate_type, const char *fmt, char *level, va_list ap);

//StreamWrapper
static inline php_stream *seaslog_stream_open_wrapper(char *opt TSRMLS_DC);
static void seaslog_init_stream_list(TSRMLS_D);
static void seaslog_clear_stream_list(TSRMLS_D);
static inline php_stream *process_stream(char *opt, int opt_len TSRMLS_DC);


//Analyzer
static long get_type_count(char *log_path, char *level, char *key_word TSRMLS_DC);
static int get_detail(char *log_path, char *level, char *key_word, long start, long end, long order, zval *return_value TSRMLS_DC);

//Performance
static void seaslog_memory_usage(smart_str *buf TSRMLS_DC);
static void seaslog_peak_memory_usage(smart_str *buf TSRMLS_DC);

#endif /* _SEASLOG_H_ */

