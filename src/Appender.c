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

static php_stream *seaslog_stream_open_wrapper(char *opt TSRMLS_DC)
{
    php_stream *stream = NULL;
    char *res = NULL;
    int first_create_file = 0;
    mode_t file_mode;

#if PHP_VERSION_ID >= 70000
    zend_long reslen;
#else
    long reslen;
#endif

    switch SEASLOG_G(appender)
    {
    case SEASLOG_APPENDER_TCP:
        reslen = spprintf(&res, 0, "tcp://%s:%d", SEASLOG_G(remote_host), SEASLOG_G(remote_port));
        stream = php_stream_xport_create(res, reslen, REPORT_ERRORS, STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT, 0, 0, NULL, NULL, NULL);
        efree(res);
        break;

    case SEASLOG_APPENDER_UDP:
        reslen = spprintf(&res, 0, "udp://%s:%d", SEASLOG_G(remote_host), SEASLOG_G(remote_port));
        stream = php_stream_xport_create(res, reslen, REPORT_ERRORS, STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT, 0, 0, NULL, NULL, NULL);

        efree(res);
        break;
    case SEASLOG_APPENDER_FILE:
    default:
        if (access(opt,F_OK) != 0)
        {
            first_create_file = 1;
        }

        stream = php_stream_open_wrapper(opt, "a", IGNORE_URL_WIN | REPORT_ERRORS, NULL);

        if (first_create_file == 1)
        {
            file_mode = (mode_t) SEASLOG_FILE_MODE;
            VCWD_CHMOD(opt, file_mode);
        }
    }

    return stream;
}

static int seaslog_real_log_ex(char *message, int message_len, char *opt TSRMLS_DC)
{
    php_stream *stream = NULL;

    stream = seaslog_stream_open_wrapper(opt TSRMLS_CC);

    if (!stream)
    {
        return FAILURE;
    }

    php_stream_write(stream, message, message_len);
    php_stream_close(stream);
    php_stream_free(stream, PHP_STREAM_FREE_RELEASE_STREAM);

    return SUCCESS;
}

static int seaslog_log_content(int argc, char *level, char *message, int message_len, HashTable *content, char *module, int module_len, zend_class_entry *ce TSRMLS_DC)
{
    int ret;
    char *result = php_strtr_array(message, message_len, content);

    ret = seaslog_log_ex(argc, level, result, strlen(result), module, module_len, ce TSRMLS_CC);

    // result is on heap, we need free it.
    efree(result);

    return ret;
}

static int seaslog_log_ex(int argc, char *level, char *message, int message_len, char *module, int module_len, zend_class_entry *ce TSRMLS_DC)
{
    logger_entry_t *logger;

    if (check_log_level(level TSRMLS_CC) == FAILURE)
    {
        return FAILURE;
    }

    if (argc > 2 && module_len > 0 && module)
    {
        logger = process_logger(module, module_len, SEASLOG_PROCESS_LOGGER_TMP TSRMLS_CC);
    }
    else
    {
        logger = SEASLOG_G(last_logger);
    }

    if (SEASLOG_G(trim_wrap))
    {
        message_trim_wrap(message,message_len TSRMLS_CC);
    }

    switch SEASLOG_G(appender)
    {
    case SEASLOG_APPENDER_TCP:
    case SEASLOG_APPENDER_UDP:
        return appender_handle_tcp_udp(message, message_len, level, logger, ce TSRMLS_CC);
        break;
    case SEASLOG_APPENDER_FILE:
    default:
        return appender_handle_file(message, message_len, level, logger, ce TSRMLS_CC);
    }

    return SUCCESS;
}

static int appender_handle_file(char *message, int message_len, char *level, logger_entry_t *logger, zend_class_entry *ce TSRMLS_DC)
{

    char *log_file_path, *log_info, *current_time, *real_date, *real_time;
    int log_len, log_file_path_len;

    current_time = mic_time();
    real_time = make_real_time(TSRMLS_C);

    real_date = make_real_date(TSRMLS_C);
    if (SEASLOG_G(disting_type))
    {
        log_file_path_len = spprintf(&log_file_path, 0, "%s/%s.%s.log", logger->logger_path, level, real_date);
    }
    else
    {
        log_file_path_len = spprintf(&log_file_path, 0, "%s/%s.log", logger->logger_path,real_date);
    }

    log_len = spprintf(&log_info, 0, "%s | %d | %s | %s | %s | %s \n", level, getpid(), SEASLOG_G(request_id), current_time, real_time, message);

    if (seaslog_real_buffer_log_ex(log_info, log_len, log_file_path, log_file_path_len + 1, ce TSRMLS_CC) == FAILURE)
    {
        efree(log_file_path);
        efree(log_info);
        return FAILURE;
    }

    efree(real_date);
    efree(log_file_path);
    efree(log_info);
    efree(real_time);
    efree(current_time);

    return SUCCESS;
}

static int appender_handle_tcp_udp(char *message, int message_len, char *level, logger_entry_t *logger, zend_class_entry *ce TSRMLS_DC)
{
    char *log_info, *current_time, *real_time;
    int log_len;

    current_time = mic_time();
    real_time = make_real_time(TSRMLS_C);

    log_len = spprintf(&log_info, 0, "%s | %s | %s | %d | %s | %s | %s | %s \n", SEASLOG_G(host_name), logger->logger, level, getpid(), SEASLOG_G(request_id), current_time, real_time, message);

    if (seaslog_real_buffer_log_ex(log_info, log_len, logger->logger, logger->logger_len, ce TSRMLS_CC) == FAILURE)
    {
        efree(log_info);
        return FAILURE;
    }

    efree(log_info);
    efree(real_time);
    efree(current_time);

    return SUCCESS;
}

static int check_log_level(char *level TSRMLS_DC)
{
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

static int make_log_dir(char *dir TSRMLS_DC)
{

    if (SEASLOG_G(appender) == SEASLOG_APPENDER_FILE)
    {
        int _ck_dir = check_log_dir(dir TSRMLS_CC);

        if (_ck_dir == FAILURE)
        {
            zval *zcontext = NULL;
            php_stream_context *context;
            mode_t imode;
            int ret;

            context = php_stream_context_from_zval(zcontext, 0);

            if (!php_stream_mkdir(dir, SEASLOG_DIR_MODE, PHP_STREAM_MKDIR_RECURSIVE | REPORT_ERRORS, context))
            {
                return FAILURE;
            }

            imode = (mode_t) SEASLOG_DIR_MODE;
            ret = VCWD_CHMOD(dir, imode);
            if (ret == FAILURE)
            {
                return FAILURE;
            }

            return SUCCESS;
        }
        else
        {
            return SUCCESS;
        }
    }
    else
    {
        return SUCCESS;
    }

}

static int check_log_dir(char *dir TSRMLS_DC)
{
    if (!access(dir, 0))
    {
        return SUCCESS;
    }

    return FAILURE;
}

static int seaslog_real_buffer_log_ex(char *message, int message_len, char *log_file_path, int log_file_path_len, zend_class_entry *ce TSRMLS_DC)
{
    if (SEASLOG_G(use_buffer))
    {
        seaslog_buffer_set(message, message_len, log_file_path, log_file_path_len, ce TSRMLS_CC);
        return SUCCESS;
    }
    else
    {
        return seaslog_real_log_ex(message, message_len, log_file_path TSRMLS_CC);
    }
}