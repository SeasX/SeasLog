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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "zend_extensions.h"
#include "zend_exceptions.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_smart_str.h"
#include "ext/date/php_date.h"
#include "php_seaslog.h"
#include <stdlib.h>

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

void seaslog_init_logger(TSRMLS_D);
void seaslog_init_buffer(TSRMLS_D);
void seaslog_clear_buffer(TSRMLS_D);
static int seaslog_shutdown_buffer(TSRMLS_D);
void seaslog_clear_logger(TSRMLS_D);
int _ck_log_dir(char *dir TSRMLS_DC);
int _seaslog_log_content(int argc, char *level, char *message, int message_len, HashTable *content, char *module, int module_len, zend_class_entry *ce TSRMLS_DC);
int _seaslog_log(int argc, char *level, char *message, int message_len, char *module, int module_len, zend_class_entry *ce TSRMLS_DC);
int _check_level(char *level TSRMLS_DC);
int _mk_log_dir(char *dir TSRMLS_DC);
int _php_log_ex(char *message, int message_len, char *log_file_path, int log_file_path_len, zend_class_entry *ce TSRMLS_DC);
static char * str_replace(char *ori, char * rep, char * with);

void (*old_error_cb)(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
void seaslog_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args);
void (*old_throw_exception_hook)(zval *exception TSRMLS_DC);
void seaslog_throw_exception_hook(zval *exception TSRMLS_DC);
static void process_event(int event_type, int type, char * error_filename, uint error_lineno, char * msg TSRMLS_DC);

ZEND_DECLARE_MODULE_GLOBALS(seaslog)

static int le_seaslog;
static char *last_logger = "default";
static char *base_path = "";
static zend_bool disting_type = 0;
static zend_bool disting_by_hour = 0;
static zend_bool use_buffer = 0;
static int buffer_size = 0;
static int level = 0;

const zend_function_entry seaslog_functions[] = {
    PHP_FE(seaslog_get_version, NULL)
    PHP_FE(seaslog_get_author,  NULL)
    {NULL, NULL, NULL}
};

const zend_function_entry seaslog_methods[] = {
    PHP_ME(SEASLOG_RES_NAME, __construct,   NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(SEASLOG_RES_NAME, __destruct,    NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)

    PHP_ME(SEASLOG_RES_NAME, setBasePath,   NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SEASLOG_RES_NAME, getBasePath,   NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SEASLOG_RES_NAME, setLogger,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SEASLOG_RES_NAME, getLastLogger, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SEASLOG_RES_NAME, analyzerCount, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SEASLOG_RES_NAME, analyzerDetail,NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SEASLOG_RES_NAME, getBuffer,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SEASLOG_RES_NAME, flushBuffer,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

    PHP_ME(SEASLOG_RES_NAME, log,           NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SEASLOG_RES_NAME, debug,         NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SEASLOG_RES_NAME, info,          NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SEASLOG_RES_NAME, notice,        NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SEASLOG_RES_NAME, warning,       NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SEASLOG_RES_NAME, error,         NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SEASLOG_RES_NAME, critical,      NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SEASLOG_RES_NAME, alert,         NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(SEASLOG_RES_NAME, emergency,     NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

    {NULL, NULL, NULL}
};

zend_module_entry seaslog_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
    STANDARD_MODULE_HEADER,
#endif
    SEASLOG_RES_NAME,
    seaslog_functions,
    PHP_MINIT(seaslog),
    PHP_MSHUTDOWN(seaslog),
    PHP_RINIT(seaslog),
    PHP_RSHUTDOWN(seaslog),
    PHP_MINFO(seaslog),
#if ZEND_MODULE_API_NO >= 20010901
    SEASLOG_VERSION,
#endif
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_SEASLOG
ZEND_GET_MODULE(seaslog)
#endif

PHP_INI_BEGIN()
STD_PHP_INI_ENTRY("seaslog.default_basepath", "/log", PHP_INI_ALL, OnUpdateString, default_basepath, zend_seaslog_globals, seaslog_globals)
STD_PHP_INI_ENTRY("seaslog.default_logger", "default", PHP_INI_ALL, OnUpdateString, default_logger, zend_seaslog_globals, seaslog_globals)
STD_PHP_INI_ENTRY("seaslog.logger", "default", PHP_INI_ALL, OnUpdateString, logger, zend_seaslog_globals, seaslog_globals)
STD_PHP_INI_BOOLEAN("seaslog.disting_type", "0", PHP_INI_ALL, OnUpdateBool, disting_type, zend_seaslog_globals, seaslog_globals)
STD_PHP_INI_BOOLEAN("seaslog.disting_by_hour", "0", PHP_INI_ALL, OnUpdateBool, disting_by_hour, zend_seaslog_globals, seaslog_globals)
STD_PHP_INI_BOOLEAN("seaslog.use_buffer", "0", PHP_INI_ALL, OnUpdateBool, use_buffer, zend_seaslog_globals, seaslog_globals)
STD_PHP_INI_ENTRY("seaslog.buffer_size", "0", PHP_INI_ALL, OnUpdateLongGEZero, buffer_size, zend_seaslog_globals, seaslog_globals)
STD_PHP_INI_ENTRY("seaslog.level", "0", PHP_INI_ALL, OnUpdateLongGEZero, level, zend_seaslog_globals, seaslog_globals)
STD_PHP_INI_ENTRY("seaslog.trace_error", "1", PHP_INI_ALL, OnUpdateLongGEZero, trace_error, zend_seaslog_globals, seaslog_globals)
STD_PHP_INI_ENTRY("seaslog.trace_exception", "0", PHP_INI_ALL, OnUpdateLongGEZero, trace_exception, zend_seaslog_globals, seaslog_globals)
PHP_INI_END()


static void php_seaslog_init_globals(zend_seaslog_globals *seaslog_globals)
{

}


PHP_MINIT_FUNCTION(seaslog)
{
    zend_class_entry seaslog;

    ZEND_INIT_MODULE_GLOBALS(seaslog, php_seaslog_init_globals, NULL);

    REGISTER_INI_ENTRIES();

    REGISTER_STRINGL_CONSTANT("SEASLOG_VERSION",   SEASLOG_VERSION,   sizeof(SEASLOG_VERSION) - 1,  CONST_PERSISTENT | CONST_CS);
    REGISTER_STRINGL_CONSTANT("SEASLOG_AUTHOR",    SEASLOG_AUTHOR,    sizeof(SEASLOG_AUTHOR) - 1,   CONST_PERSISTENT | CONST_CS);

    REGISTER_STRINGL_CONSTANT("SEASLOG_ALL",       SEASLOG_ALL,       sizeof(SEASLOG_ALL) - 1,      CONST_PERSISTENT | CONST_CS);

    REGISTER_STRINGL_CONSTANT("SEASLOG_DEBUG",     SEASLOG_DEBUG,     sizeof(SEASLOG_DEBUG) - 1,    CONST_PERSISTENT | CONST_CS);
    REGISTER_STRINGL_CONSTANT("SEASLOG_INFO",      SEASLOG_INFO,      sizeof(SEASLOG_INFO) - 1,     CONST_PERSISTENT | CONST_CS);
    REGISTER_STRINGL_CONSTANT("SEASLOG_NOTICE",    SEASLOG_NOTICE,    sizeof(SEASLOG_NOTICE) - 1,   CONST_PERSISTENT | CONST_CS);
    REGISTER_STRINGL_CONSTANT("SEASLOG_WARNING",   SEASLOG_WARNING,   sizeof(SEASLOG_WARNING) - 1,  CONST_PERSISTENT | CONST_CS);
    REGISTER_STRINGL_CONSTANT("SEASLOG_ERROR",     SEASLOG_ERROR,     sizeof(SEASLOG_ERROR) - 1,    CONST_PERSISTENT | CONST_CS);
    REGISTER_STRINGL_CONSTANT("SEASLOG_CRITICAL",  SEASLOG_CRITICAL,  sizeof(SEASLOG_CRITICAL) - 1, CONST_PERSISTENT | CONST_CS);
    REGISTER_STRINGL_CONSTANT("SEASLOG_ALERT",     SEASLOG_ALERT,     sizeof(SEASLOG_ALERT) - 1,    CONST_PERSISTENT | CONST_CS);
    REGISTER_STRINGL_CONSTANT("SEASLOG_EMERGENCY", SEASLOG_EMERGENCY, sizeof(SEASLOG_EMERGENCY) - 1,CONST_PERSISTENT | CONST_CS);

    REGISTER_LONG_CONSTANT("SEASLOG_DETAIL_ORDER_ASC", SEASLOG_DETAIL_ORDER_ASC, CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("SEASLOG_DETAIL_ORDER_DESC", SEASLOG_DETAIL_ORDER_DESC, CONST_PERSISTENT | CONST_CS);

    INIT_CLASS_ENTRY(seaslog, SEASLOG_RES_NAME, seaslog_methods);
    seaslog_ce = zend_register_internal_class_ex(&seaslog, NULL, NULL TSRMLS_CC);
    seaslog_ce->ce_flags = ZEND_ACC_IMPLICIT_PUBLIC;

    zend_declare_property_null(seaslog_ce, ZEND_STRL(SEASLOG_BUFFER_NAME), ZEND_ACC_STATIC TSRMLS_CC);
    zend_declare_property_null(seaslog_ce, ZEND_STRL(SEASLOG_BUFFER_SIZE_NAME), ZEND_ACC_STATIC TSRMLS_CC);

    if (SEASLOG_G(trace_error)) {
        old_error_cb = zend_error_cb;
        zend_error_cb = seaslog_error_cb;
    }

    if (SEASLOG_G(trace_exception)) {
        if (zend_throw_exception_hook) {
            old_throw_exception_hook = zend_throw_exception_hook;
        }

        zend_throw_exception_hook = seaslog_throw_exception_hook;
    }

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(seaslog)
{
    UNREGISTER_INI_ENTRIES();

    return SUCCESS;
}

PHP_RINIT_FUNCTION(seaslog)
{
    seaslog_init_logger(TSRMLS_C);
    seaslog_init_buffer(TSRMLS_C);

    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(seaslog)
{
    seaslog_shutdown_buffer(TSRMLS_C);
    seaslog_clear_logger(TSRMLS_C);

    if (SEASLOG_G(trace_error)) {
        if (old_error_cb) {
            zend_error_cb = old_error_cb;
        }
    }

    if (SEASLOG_G(trace_exception)) {
        if (old_throw_exception_hook) {
            zend_throw_exception_hook = old_throw_exception_hook;
        }
    }

    return SUCCESS;
}

PHP_MINFO_FUNCTION(seaslog)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "SeasLog support", "Enabled");
    php_info_print_table_row(2, "SeasLog Version", SEASLOG_VERSION);
    php_info_print_table_row(2, "SeasLog Author", SEASLOG_AUTHOR);
    php_info_print_table_row(2,"SeasLog Supports", "https://github.com/Neeke/SeasLog");
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}

static void process_event(int event_type, int type, char * error_filename, uint error_lineno, char * msg TSRMLS_DC)
{
    char *event_type_str;
    char *event_str;
    int event_str_len;
    if (event_type == SEASLOG_EVENT_ERROR) {
        event_type_str = "Error";
    } else if (event_type == SEASLOG_EVENT_EXCEPTION) {
        event_type_str = "Exception";
    }

    event_str_len = spprintf(&event_str, 0, "%s - type:%d - file:%s - line:%d - msg:%s", event_type_str, type, error_filename, error_lineno, msg);

    _seaslog_log(1, SEASLOG_CRITICAL, event_str, event_str_len, NULL, 0, seaslog_ce TSRMLS_CC);
}

void seaslog_error_cb(int type, const char *error_filename, const uint error_lineno, const char *format, va_list args)
{
    if (type == E_ERROR || type == E_PARSE || type == E_CORE_ERROR || type == E_COMPILE_ERROR || type == E_USER_ERROR || type == E_RECOVERABLE_ERROR) {
       	char *msg;
       	va_list args_copy;
       	TSRMLS_FETCH();

       	va_copy(args_copy, args);
       	vspprintf(&msg, 0, format, args_copy);
       	va_end(args_copy);

       	process_event(SEASLOG_EVENT_ERROR, type, (char *) error_filename, error_lineno, msg TSRMLS_CC);
       	efree(msg);
    }

	old_error_cb(type, error_filename, error_lineno, format, args);
}

void seaslog_throw_exception_hook(zval *exception TSRMLS_DC)
{
	zval *message, *file, *line;
#if PHP_VERSION_ID >= 70000
	zval rv;
#endif
	zend_class_entry *default_ce;

    if (!exception) {
        return;
    }

    default_ce = zend_exception_get_default(TSRMLS_C);

#if PHP_VERSION_ID >= 70000
    message = zend_read_property(default_ce, exception, "message", sizeof("message")-1, 0, &rv);
    file = zend_read_property(default_ce, exception, "file", sizeof("file")-1, 0, &rv);
    line = zend_read_property(default_ce, exception, "line", sizeof("line")-1, 0, &rv);
#else
    message = zend_read_property(default_ce, exception, "message", sizeof("message")-1, 0 TSRMLS_CC);
    file = zend_read_property(default_ce, exception, "file", sizeof("file")-1, 0 TSRMLS_CC);
    line = zend_read_property(default_ce, exception, "line", sizeof("line")-1, 0 TSRMLS_CC);
#endif

    process_event(SEASLOG_EVENT_EXCEPTION, E_EXCEPTION, Z_STRVAL_P(file), Z_LVAL_P(line), Z_STRVAL_P(message) TSRMLS_CC);

    if (old_throw_exception_hook) {
        old_throw_exception_hook(exception TSRMLS_CC);
    }
}

void seaslog_init_logger(TSRMLS_D)
{
    SEASLOG_G(base_path)   = estrdup(SEASLOG_G(default_basepath));
    SEASLOG_G(last_logger) = estrdup(SEASLOG_G(default_logger));
}

void seaslog_clear_logger(TSRMLS_D)
{
    if (SEASLOG_G(base_path)) {
        efree(SEASLOG_G(base_path));
    }

    if (SEASLOG_G(last_logger)) {
        efree(SEASLOG_G(last_logger));
    }
}

void seaslog_init_buffer(TSRMLS_D)
{
    if (SEASLOG_G(use_buffer)) {
        zval *buffer_count;
        buffer_count = zend_read_static_property(seaslog_ce, SL_S(SEASLOG_BUFFER_SIZE_NAME), 0 TSRMLS_CC);

        if (Z_TYPE_P(buffer_count) == IS_NULL) {
            seaslog_clear_buffer(TSRMLS_C);
        }
    }
}

void seaslog_clear_buffer(TSRMLS_D)
{
    zval *buffer;
    zval *buffer_size;
    MAKE_STD_ZVAL(buffer);
    array_init(buffer);

    zend_update_static_property(seaslog_ce, SL_S(SEASLOG_BUFFER_NAME), buffer TSRMLS_CC);
    zval_ptr_dtor(&buffer);


    MAKE_STD_ZVAL(buffer_size);
    ZVAL_LONG(buffer_size, 0);

    zend_update_static_property(seaslog_ce, SL_S(SEASLOG_BUFFER_SIZE_NAME), buffer_size TSRMLS_CC);
    zval_ptr_dtor(&buffer_size);
}

static int real_php_log_ex(char *message, int message_len, char *opt TSRMLS_DC)
{
    php_stream *stream = NULL;
    stream = php_stream_open_wrapper(opt, "a", IGNORE_URL_WIN | ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL);
    if (!stream) {
        return FAILURE;
    }
    php_stream_write(stream, message, message_len);
    php_stream_close(stream);
    php_stream_free(stream, PHP_STREAM_FREE_RELEASE_STREAM);

    return SUCCESS;
}

static int seaslog_buffer_set(char *log_info, int log_info_len, char *path, int path_len, zend_class_entry *ce TSRMLS_DC)
{
    HashTable   *ht;
    zval        *old_log_array;
    zval        *new_array;
    zval       **ppzval;
    int          have_old = FAILURE;
    int          count;

    zval *file_path;

    old_log_array = zend_read_static_property(ce, SL_S(SEASLOG_BUFFER_NAME), 1 TSRMLS_CC);

    if (!old_log_array || Z_TYPE_P(old_log_array) != IS_ARRAY) {
        return 0;
    }

    MAKE_STD_ZVAL(new_array);
    array_init(new_array);

    ht = Z_ARRVAL_P(old_log_array);

    count = zend_hash_num_elements(ht);

    zend_hash_internal_pointer_reset(ht);
    while (zend_hash_get_current_data(ht, (void **)&ppzval) == SUCCESS) {
        char *key;
        ulong idx = 0;
        int file_path_len;

        zend_hash_get_current_key(ht, &key, &idx, 0);

        MAKE_STD_ZVAL(file_path);
        ZVAL_STRING(file_path, key, 1);

        file_path_len = Z_STRLEN_P(file_path) + 1;

        if (strcmp(Z_STRVAL_P(file_path), path) == 0) {
            char *_new_log;
            spprintf(&_new_log, 0, "%s%s", Z_STRVAL_PP(ppzval), log_info);

            add_assoc_string_ex(new_array, Z_STRVAL_P(file_path), file_path_len, _new_log, 1);
            efree(_new_log);
            have_old = SUCCESS;
        } else {
            add_assoc_string_ex(new_array, Z_STRVAL_P(file_path), file_path_len, Z_STRVAL_PP(ppzval), 1);
        }
        zval_ptr_dtor(&file_path);
        zend_hash_move_forward(ht);
    }

    if (have_old == FAILURE) {
        add_assoc_string_ex(new_array, path, path_len + 1, log_info, 1);
    }

    zend_update_static_property(ce, SL_S(SEASLOG_BUFFER_NAME), new_array TSRMLS_CC);
    zval_ptr_dtor(&new_array);

    if (SEASLOG_G(buffer_size) > 0) {
        zval *buffer_count;
        zval *buffer_count_new;
        buffer_count = zend_read_static_property(seaslog_ce, SL_S(SEASLOG_BUFFER_SIZE_NAME), 0 TSRMLS_CC);

        MAKE_STD_ZVAL(buffer_count_new);
        ZVAL_LONG(buffer_count_new, Z_LVAL_P(buffer_count) + 1);

        zend_update_static_property(seaslog_ce, SL_S(SEASLOG_BUFFER_SIZE_NAME), buffer_count_new TSRMLS_CC);
        zval_ptr_dtor(&buffer_count_new);

        if (Z_LVAL_P(buffer_count) + 1 >= SEASLOG_G(buffer_size)) {
            seaslog_shutdown_buffer(TSRMLS_C);
        }
    }

    return SUCCESS;
}

static int seaslog_shutdown_buffer(TSRMLS_D)
{
    if (SEASLOG_G(use_buffer)) {
        HashTable   *ht;
        zval        *old_log_array;
        zval **ppzval;
        int  count;

        old_log_array = zend_read_static_property(seaslog_ce, SL_S(SEASLOG_BUFFER_NAME), 1 TSRMLS_CC);

        if (!old_log_array || Z_TYPE_P(old_log_array) != IS_ARRAY) {
            return 0;
        }

        ht = Z_ARRVAL_P(old_log_array);

        count = zend_hash_num_elements(ht);

        zend_hash_internal_pointer_reset(ht);
        while (zend_hash_get_current_data(ht, (void **)&ppzval) == SUCCESS) {
            char *key;
            ulong idx = 0;

            zend_hash_get_current_key(ht, &key, &idx, 0);

            real_php_log_ex(Z_STRVAL_PP(ppzval), Z_STRLEN_PP(ppzval), key TSRMLS_CC);

            zend_hash_move_forward(ht);
        }

        seaslog_clear_buffer(TSRMLS_C);
    }

    return SUCCESS;
}

static char *mk_real_log_path(char *log_path, char *date, char *level TSRMLS_DC)
{
    char *log_file_path = NULL;
    if (SEASLOG_G(disting_type)) {
        spprintf(&log_file_path, 0, "%s/%s.%s.log", log_path, level, date);
    } else {
        spprintf(&log_file_path, 0, "%s/%s.log", log_path,date);
    }

    return log_file_path;
}

static char *delN(char *a)
{
    int l;
    l = strlen(a);
    a[l - 1] = '\0';

    return a;
}

static char *mic_time()
{
    struct timeval now;
    char *tstr;

    timerclear(&now);
    gettimeofday(&now, NULL);

    spprintf(&tstr, 0, "%ld.%ld", (long)time(NULL), (long)now.tv_usec / 1000);

    return tstr;
}

static char *mk_real_date(TSRMLS_D)
{
    char *_date;

    if (SEASLOG_G(disting_by_hour)) {
        _date = php_format_date("YmdH", 5, (long)time(NULL), (long)time(NULL) TSRMLS_CC);
    } else {
        _date = php_format_date("Ymd",  3, (long)time(NULL), (long)time(NULL) TSRMLS_CC);
    }

    return _date;
}

static long get_type_count(char *log_path, char *level, char *key_word TSRMLS_DC)
{
    FILE * fp;
    char buffer[BUFSIZ];
    char *path, *_log_path, *sh;
    long count;
    int _ck_dir;

    spprintf(&_log_path, 0, "%s/%s", SEASLOG_G(base_path), SEASLOG_G(last_logger));
    _ck_dir = _ck_log_dir(_log_path TSRMLS_CC);
    efree(_log_path);
    if (_ck_dir == FAILURE) {
        return (long)0;
    }
    if (SEASLOG_G(disting_type)) {
        spprintf(&path, 0, "%s/%s/%s.%s*", SEASLOG_G(base_path), SEASLOG_G(last_logger), level, log_path);
    } else {
        spprintf(&path, 0, "%s/%s/%s*",    SEASLOG_G(base_path), SEASLOG_G(last_logger), log_path);
    }

#ifdef WINDOWS
    path = str_replace(path, "/", "\\");
#endif

    if (key_word) {
#ifdef WINDOWS
        spprintf(&sh, 0, "findstr \"%s\" %s | find /c \"%s\" ", level, path, key_word);
#else
        spprintf(&sh, 0, "more %s 2>/dev/null| grep '%s' | grep '%s' -wc", path, level, key_word);
#endif
    } else {
#ifdef WINDOWS
        spprintf(&sh, 0, "findstr \"%s\" %s | find /c /v \"\" ", level, path);
#else
        spprintf(&sh, 0, "more %s 2>/dev/null| grep '%s' -wc", path, level);
#endif
    }

    fp = VCWD_POPEN(sh, "r");
    if (! fp) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to fork [%s]", sh);
        return -1;
    } else {
        char *temp_p = fgets(buffer, sizeof(buffer), fp);
        pclose(fp);
    }

    count = atoi(delN(buffer));
    efree(path);
    efree(sh);

    return count;
}


static char * str_replace(char *ori, char * rep, char * with)
{
    char *result;
    char *ins;
    char *tmp;
    int len_rep;
    int len_with;
    int len_front;
    int count;

    if (!ori)
        return NULL;
    if (!rep)
        rep = "";
    len_rep = strlen(rep);
    if (!with)
        with = "";
    len_with = strlen(with);

    ins = ori;
    for (count = 0; (tmp = strstr(ins, rep)) != NULL ; ++count) {
        ins = tmp + len_rep;
    }

    tmp = result = emalloc(strlen(ori) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    while (count--) {
        ins = strstr(ori, rep);
        len_front = ins - ori;
        tmp = strncpy(tmp, ori, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        ori += len_front + len_rep;
    }
    strcpy(tmp, ori);
    return result;
}

static int get_detail(char *log_path, char *level, char *key_word, long start, long limit, long order, zval *return_value TSRMLS_DC)
{
    FILE * fp;
    char buffer[BUFSIZ + 1];
    char *path;
    char *sh;
    char *command;

    memset(buffer, '\0', sizeof(buffer));

    array_init(return_value);

    if (SEASLOG_G(disting_type)) {
        if (!strcmp(level,"|")) {
            spprintf(&path, 0, "%s/%s/%s.%s*", SEASLOG_G(base_path), SEASLOG_G(last_logger), "*", log_path);
        } else {
            spprintf(&path, 0, "%s/%s/%s.%s*", SEASLOG_G(base_path), SEASLOG_G(last_logger), level, log_path);
        }

    } else {
        spprintf(&path, 0, "%s/%s/%s*",    SEASLOG_G(base_path), SEASLOG_G(last_logger), log_path);
    }

#ifdef WINDOWS
    path = str_replace(path, "/", "\\");
#else
    if (order == SEASLOG_DETAIL_ORDER_DESC) {
        spprintf(&command, 0, "%s `ls -t %s`", "tac", path);
    } else {
        spprintf(&command, 0, "%s %s", "more", path);
    }
#endif

    if (key_word && strlen(key_word) >= 1) {
#ifdef WINDOWS
        spprintf(&sh, 0, "findstr \"%s\" %s | findstr \"%s\" ", level, path, key_word);
#else
        spprintf(&sh, 0, "%s 2>/dev/null| grep '%s' -w | grep '%s' -w | sed -n '%ld,%ld'p", command, level, key_word, start, limit);
#endif
    } else {
#ifdef WINDOWS
        spprintf(&sh, 0, "findstr \"%s\" %s", level, path);
#else
        spprintf(&sh, 0, "%s 2>/dev/null| grep '%s' -w | sed -n '%ld,%ld'p", command, level, start, limit);
#endif
    }

    fp = VCWD_POPEN(sh, "r");

    if (!fp) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to fork [%s]", sh);

        efree(buffer);
        return -1;
    } else {
        while ((fgets(buffer, sizeof(buffer), fp)) != NULL) {
            if (strstr(buffer, SEASLOG_G(base_path)) == NULL) {
                add_next_index_string(return_value, delN(buffer), 1);
            }
        }

        pclose(fp);
    }

    efree(path);
    efree(command);
    efree(sh);

    return 1;
}

static void seaslog_log_by_level_common(INTERNAL_FUNCTION_PARAMETERS, char *level)
{
    int argc = ZEND_NUM_ARGS();
    char *message, *logger = NULL;
    int message_len, logger_len;
    zval **content;
    seaslog_init_buffer(TSRMLS_C);

    if (zend_parse_parameters(argc TSRMLS_CC, "s|Zs", &message, &message_len, &content, &logger, &logger_len) == FAILURE)
        return;

    if (argc > 1 && Z_TYPE_PP(content) != IS_ARRAY) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "The second argument is not an array");
        RETURN_FALSE;
    }

    if (argc > 1) {
        if (_seaslog_log_content(argc, level, message, message_len, HASH_OF(*content), logger, logger_len, seaslog_ce TSRMLS_CC) == FAILURE) {
            RETURN_FALSE;
        }
    } else {
        if (_seaslog_log(argc, level, message, message_len, logger, logger_len, seaslog_ce TSRMLS_CC) == FAILURE) {
            RETURN_FALSE;
        }
    }

    RETURN_TRUE;
}

PHP_FUNCTION(seaslog_get_version)
{
    char *str;
    int len = 0;
    len = spprintf(&str, 0, "%s", SEASLOG_VERSION);

    RETURN_STRINGL(str, len, 0);
}

PHP_FUNCTION(seaslog_get_author)
{
    char *str;
    int len = 0;
    len = spprintf(&str, 0, "%s", SEASLOG_AUTHOR);

    RETURN_STRINGL(str, len, 0);
}

PHP_METHOD(SEASLOG_RES_NAME, __construct)
{
    seaslog_init_logger(TSRMLS_C);
    seaslog_init_buffer(TSRMLS_C);
}

PHP_METHOD(SEASLOG_RES_NAME, __destruct)
{
    seaslog_shutdown_buffer(TSRMLS_C);
}

PHP_METHOD(SEASLOG_RES_NAME, setBasePath)
{
    zval *_base_path;
    int argc = ZEND_NUM_ARGS();

    if (zend_parse_parameters(argc TSRMLS_CC, "z", &_base_path) == FAILURE)
        return;

    if (argc > 0 && Z_TYPE_P(_base_path) == IS_STRING || Z_STRLEN_P(_base_path) > 0) {
        if (!strcmp(SEASLOG_G(base_path),SEASLOG_G(default_basepath)))
        {
            efree(SEASLOG_G(base_path));
        }

        SEASLOG_G(base_path) = estrdup(Z_STRVAL_P(_base_path));
        zval_ptr_dtor(&_base_path);
        RETURN_TRUE;
    }

    RETURN_FALSE;
}

PHP_METHOD(SEASLOG_RES_NAME, getBasePath)
{
    char *module = NULL;
    int argc = ZEND_NUM_ARGS();
    int module_len;
    char *str;
    int len = 0;

    if (zend_parse_parameters(argc TSRMLS_CC, "|s", &module, &module_len) == FAILURE)
        return;

    len = spprintf(&str, 0, "%s", SEASLOG_G(base_path));

    RETURN_STRINGL(str, len, 0);
}

PHP_METHOD(SEASLOG_RES_NAME, setLogger)
{
    zval *_module;
    int argc = ZEND_NUM_ARGS();

    if (zend_parse_parameters(argc TSRMLS_CC, "z", &_module) == FAILURE)
        return;

    if (argc > 0 && Z_TYPE_P(_module) == IS_STRING || Z_STRLEN_P(_module) > 0) {
        if (!strcmp(SEASLOG_G(last_logger),SEASLOG_G(default_logger)))
        {
            efree(SEASLOG_G(last_logger));
        }

        SEASLOG_G(last_logger) = estrdup(Z_STRVAL_P(_module));
        zval_ptr_dtor(&_module);
        RETURN_TRUE;
    }

    RETURN_FALSE;
}

/*the last logger*/
PHP_METHOD(SEASLOG_RES_NAME, getLastLogger)
{
    char *str;
    int len = 0;
    len = spprintf(&str, 0, "%s", SEASLOG_G(last_logger));

    RETURN_STRINGL(str, len, 0);
}


PHP_METHOD(SEASLOG_RES_NAME, analyzerCount)
{
    int argc = ZEND_NUM_ARGS();
    char *log_path, *level = NULL, *key_word = NULL;
    int len = 0;
    int log_path_len, level_len, key_word_len;
    long count;

    if (zend_parse_parameters(argc TSRMLS_CC, "|sss", &level, &level_len, &log_path, &log_path_len, &key_word, &key_word_len) == FAILURE)
        return;

    if (argc == 0 || (argc == 1 && level && !strcmp(level, SEASLOG_ALL))) {
        long count_debug, count_info, count_notice, count_warn, count_erro, count_critical, count_alert, count_emergency;
        array_init(return_value);
        log_path = "";

        count_debug     = get_type_count(log_path, SEASLOG_DEBUG, key_word TSRMLS_CC);
        count_info      = get_type_count(log_path, SEASLOG_INFO, key_word TSRMLS_CC);
        count_notice    = get_type_count(log_path, SEASLOG_NOTICE, key_word TSRMLS_CC);
        count_warn      = get_type_count(log_path, SEASLOG_WARNING, key_word TSRMLS_CC);
        count_erro      = get_type_count(log_path, SEASLOG_ERROR, key_word TSRMLS_CC);
        count_critical  = get_type_count(log_path, SEASLOG_CRITICAL, key_word TSRMLS_CC);
        count_alert     = get_type_count(log_path, SEASLOG_ALERT, key_word TSRMLS_CC);
        count_emergency = get_type_count(log_path, SEASLOG_EMERGENCY, key_word TSRMLS_CC);

        add_assoc_long(return_value, SEASLOG_DEBUG, count_debug);
        add_assoc_long(return_value, SEASLOG_INFO, count_info);
        add_assoc_long(return_value, SEASLOG_NOTICE, count_notice);
        add_assoc_long(return_value, SEASLOG_WARNING, count_warn);
        add_assoc_long(return_value, SEASLOG_ERROR, count_erro);
        add_assoc_long(return_value, SEASLOG_CRITICAL, count_critical);
        add_assoc_long(return_value, SEASLOG_ALERT, count_alert);
        add_assoc_long(return_value, SEASLOG_EMERGENCY, count_emergency);
    } else if (argc == 1) {
        log_path = "";
        count = get_type_count(log_path, level, key_word TSRMLS_CC);

        RETURN_LONG(count);
    } else {
        count = get_type_count(log_path, level, key_word TSRMLS_CC);

        RETURN_LONG(count);
    }

}

PHP_METHOD(SEASLOG_RES_NAME, analyzerDetail)
{
    char *log_path, *level = NULL, *key_word = NULL;
    int log_path_len, level_len, key_word_len;
    long start = 1;
    long limit = 20;
    long order = SEASLOG_DETAIL_ORDER_ASC;
    int argc = ZEND_NUM_ARGS();

    if (zend_parse_parameters(argc TSRMLS_CC, "s|sslll", &level, &level_len, &log_path, &log_path_len, &key_word, &key_word_len, &start, &limit, &order) == FAILURE) {
        RETURN_FALSE;
    }

    if (argc < 2) {
        log_path = "*";
    } else if (argc > 3) {
#ifdef WINDOWS
        zend_error(E_NOTICE, "Param start and limit don't support Windows");
#endif
    } else {
        // do nothing
    }

    if (level && !strcmp(level, SEASLOG_ALL)) {
        level = "|";
    }

    get_detail(log_path, level, key_word, start, limit, order, return_value TSRMLS_CC);
}

PHP_METHOD(SEASLOG_RES_NAME, getBuffer)
{
    if (SEASLOG_G(use_buffer)) {
        zval *buffer;
        buffer = zend_read_static_property(seaslog_ce, ZEND_STRL(SEASLOG_BUFFER_NAME), 0 TSRMLS_CC);

        RETURN_ZVAL(buffer, 1, 0);
    }
}

PHP_METHOD(SEASLOG_RES_NAME, flushBuffer)
{
    seaslog_shutdown_buffer(TSRMLS_C);

    RETURN_TRUE;
}

PHP_METHOD(SEASLOG_RES_NAME, log)
{
    int argc = ZEND_NUM_ARGS();
    char *message, *level = NULL, *logger = NULL;
    int message_len = 0, level_len = 0, logger_len = 0;
    zval **content;

    seaslog_init_buffer(TSRMLS_C);

    if (zend_parse_parameters(argc TSRMLS_CC, "ss|Zs", &level, &level_len, &message, &message_len, &content, &logger, &logger_len) == FAILURE)
        return;

    if (argc > 2 && Z_TYPE_PP(content) != IS_ARRAY) {
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "The third argument is not an array");
        RETURN_FALSE;
    }

    if (argc > 2) {
        if (_seaslog_log_content(argc, level, message, message_len, HASH_OF(*content), logger, logger_len, seaslog_ce TSRMLS_CC) == FAILURE) {
            RETURN_FALSE;
        }
    } else {
        if (_seaslog_log(argc, level, message, message_len, logger, logger_len, seaslog_ce TSRMLS_CC) == FAILURE) {
            RETURN_FALSE;
        }
    }

    RETURN_TRUE;
}

PHP_METHOD(SEASLOG_RES_NAME, debug)
{
    seaslog_log_by_level_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, SEASLOG_DEBUG);
}

PHP_METHOD(SEASLOG_RES_NAME, info)
{
    seaslog_log_by_level_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, SEASLOG_INFO);
}

PHP_METHOD(SEASLOG_RES_NAME, notice)
{
    seaslog_log_by_level_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, SEASLOG_NOTICE);
}

PHP_METHOD(SEASLOG_RES_NAME, warning)
{
    seaslog_log_by_level_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, SEASLOG_WARNING);
}

PHP_METHOD(SEASLOG_RES_NAME, error)
{
    seaslog_log_by_level_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, SEASLOG_ERROR);
}

PHP_METHOD(SEASLOG_RES_NAME, critical)
{
    seaslog_log_by_level_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, SEASLOG_CRITICAL);
}

PHP_METHOD(SEASLOG_RES_NAME, alert)
{
    seaslog_log_by_level_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, SEASLOG_ALERT);
}

PHP_METHOD(SEASLOG_RES_NAME, emergency)
{
    seaslog_log_by_level_common(INTERNAL_FUNCTION_PARAM_PASSTHRU, SEASLOG_EMERGENCY);
}

static char *php_strtr_array(char *str, int slen, HashTable *hash)
{
    zval **entry;
    char  *string_key;
    uint   string_key_len;
    zval **trans;
    zval   ctmp;
    ulong  num_key;
    int    minlen = 128 * 1024;
    int    maxlen = 0, pos, len, found;
    char  *key;
    HashPosition hpos;
    smart_str result = {0};
    HashTable tmp_hash;

    zend_hash_init(&tmp_hash, zend_hash_num_elements(hash), NULL, NULL, 0);
    zend_hash_internal_pointer_reset_ex(hash, &hpos);

    while (zend_hash_get_current_data_ex(hash, (void **)&entry, &hpos) == SUCCESS) {
        switch (zend_hash_get_current_key_ex(hash, &string_key, &string_key_len, &num_key, 0, &hpos)) {
        case HASH_KEY_IS_STRING:
            len = string_key_len - 1;
            if (len < 1) {
                zend_hash_destroy(&tmp_hash);
            } else {
                zend_hash_add(&tmp_hash, string_key, string_key_len, entry, sizeof(zval*), NULL);
                if (len > maxlen) {
                    maxlen = len;
                }
                if (len < minlen) {
                    minlen = len;
                }
            }
            break;

        case HASH_KEY_IS_LONG:
            Z_TYPE(ctmp) = IS_LONG;
            Z_LVAL(ctmp) = num_key;

            convert_to_string(&ctmp);
            len = Z_STRLEN(ctmp);
            zend_hash_add(&tmp_hash, Z_STRVAL(ctmp), len + 1, entry, sizeof(zval*), NULL);
            zval_dtor(&ctmp);

            if (len > maxlen) {
                maxlen = len;
            }
            if (len < minlen) {
                minlen = len;
            }
            break;
        }
        zend_hash_move_forward_ex(hash, &hpos);
    }

    key = emalloc(maxlen + 1);
    pos = 0;

    while (pos < slen) {
        if ((pos + maxlen) > slen) {
            maxlen = slen - pos;
        }

        found = 0;
        memcpy(key, str + pos, maxlen);

        for (len = maxlen; len >= minlen; len--) {
            key[len] = 0;

            if (zend_hash_find(&tmp_hash, key, len + 1, (void**)&trans) == SUCCESS) {
                char *tval;
                int tlen;
                zval tmp;

                if (Z_TYPE_PP(trans) != IS_STRING) {
                    tmp = **trans;
                    zval_copy_ctor(&tmp);
                    convert_to_string(&tmp);
                    tval = Z_STRVAL(tmp);
                    tlen = Z_STRLEN(tmp);
                } else {
                    tval = Z_STRVAL_PP(trans);
                    tlen = Z_STRLEN_PP(trans);
                }

                smart_str_appendl(&result, tval, tlen);
                pos += len;
                found = 1;

                if (Z_TYPE_PP(trans) != IS_STRING) {
                    zval_dtor(&tmp);
                }

                break;
            }
        }

        if (! found) {
            smart_str_appendc(&result, str[pos++]);
        }
    }

    zend_hash_destroy(&tmp_hash);
    smart_str_0(&result);
    efree(key);

    return result.c;
}

int _seaslog_log_content(int argc, char *level, char *message, int message_len, HashTable *content, char *module, int module_len, zend_class_entry *ce TSRMLS_DC)
{
    int ret;
    char *result = php_strtr_array(message, message_len, content);

    ret = _seaslog_log(argc, level, result, strlen(result), module, module_len, ce TSRMLS_CC);
    efree(result);

    return ret;
}

int _seaslog_log(int argc, char *level, char *message, int message_len, char *module, int module_len, zend_class_entry *ce TSRMLS_DC)
{
    char *logger, *_log_path, *_date, *_time, *log_file_path, *log_info,*current_time;
    int log_len, log_file_path_len;

    if (argc > 2 && module_len > 0 && module) {
        logger = module;
//        if (strcmp(SEASLOG_G(last_logger), "default") == 0) {
//            spprintf(&SEASLOG_G(last_logger), 0, "%s", logger);
//        }
    } else {
        logger = SEASLOG_G(last_logger);
    }

    if (_check_level(level TSRMLS_CC) == FAILURE) {
        return FAILURE;
    }

    spprintf(&_log_path, 0, "%s/%s", SEASLOG_G(base_path), logger);
    _mk_log_dir(_log_path TSRMLS_CC);

    _date = mk_real_date(TSRMLS_C);

    _time = php_format_date("Y:m:d H:i:s", 11, (long)time(NULL), (long)time(NULL) TSRMLS_CC);

    log_file_path = mk_real_log_path(_log_path, _date, level TSRMLS_CC);
    efree(_date);
    efree(_log_path);

    log_file_path_len = strlen(log_file_path)+1;

    current_time = mic_time();
    log_len = spprintf(&log_info, 0, "%s | %d | %s | %s | %s \n", level, getpid(), current_time, _time, message);
    efree(_time);
    efree(current_time);

    if (_php_log_ex(log_info, log_len, log_file_path, log_file_path_len, ce TSRMLS_CC) == FAILURE) {
        efree(log_file_path);
        efree(log_info);
        return FAILURE;
    }

    efree(log_file_path);
    efree(log_info);
    return SUCCESS;
}

int _check_level(char *level TSRMLS_DC){
    if (SEASLOG_G(level) < 1) return SUCCESS;
    if (SEASLOG_G(level) > 8) return FAILURE;

    if (strcmp(level, SEASLOG_DEBUG)      == 0 && SEASLOG_G(level) <= 1) return SUCCESS;
    if (strcmp(level, SEASLOG_INFO)       == 0 && SEASLOG_G(level) <= 2) return SUCCESS;
    if (strcmp(level, SEASLOG_NOTICE)     == 0 && SEASLOG_G(level) <= 3) return SUCCESS;
    if (strcmp(level, SEASLOG_WARNING)    == 0 && SEASLOG_G(level) <= 4) return SUCCESS;
    if (strcmp(level, SEASLOG_ERROR)      == 0 && SEASLOG_G(level) <= 5) return SUCCESS;
    if (strcmp(level, SEASLOG_CRITICAL)   == 0 && SEASLOG_G(level) <= 6) return SUCCESS;
    if (strcmp(level, SEASLOG_ALERT)      == 0 && SEASLOG_G(level) <= 7) return SUCCESS;
    if (strcmp(level, SEASLOG_EMERGENCY)  == 0 && SEASLOG_G(level) <= 8) return SUCCESS;

    return FAILURE;
}

int _mk_log_dir(char *dir TSRMLS_DC)
{
    int _ck_dir = _ck_log_dir(dir TSRMLS_CC);

    if (_ck_dir == FAILURE) {
        zval *zcontext = NULL;
        long mode = 0777;
        zend_bool recursive = 1;
        php_stream_context *context;
        umask(1);

        context = php_stream_context_from_zval(zcontext, 0);

        if (php_stream_mkdir(dir, mode, (recursive ? PHP_STREAM_MKDIR_RECURSIVE : 0) | REPORT_ERRORS, context)) {
            return FAILURE;
        }

        return SUCCESS;
    } else {
        return FAILURE;
    }

}

int _ck_log_dir(char *dir TSRMLS_DC)
{
    if (!access(dir,0)) {
        return SUCCESS;
    }

    return FAILURE;
}

int _php_log_ex(char *message, int message_len, char *log_file_path, int log_file_path_len, zend_class_entry *ce TSRMLS_DC)
{
    if (SEASLOG_G(use_buffer)) {
        seaslog_buffer_set(message, message_len, log_file_path, log_file_path_len, ce TSRMLS_CC);
        return SUCCESS;
    } else {
        return real_php_log_ex(message, message_len, log_file_path TSRMLS_CC);
    }
}
