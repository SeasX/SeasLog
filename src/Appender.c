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

static int seaslog_real_log_ex(char *message, int message_len, char *opt, int opt_len TSRMLS_DC)
{
    php_stream *stream = NULL;

    stream = process_stream(opt,opt_len TSRMLS_CC);

    if (!stream)
    {
        return FAILURE;
    }

    php_stream_write(stream, message, message_len);

    return SUCCESS;
}

static int seaslog_log_content(int argc, char *level, int level_int, char *message, int message_len, HashTable *content, char *module, int module_len, zend_class_entry *ce TSRMLS_DC)
{
    int ret;
    char *result = php_strtr_array(message, message_len, content);

    ret = seaslog_log_ex(argc, level, level_int, result, strlen(result), module, module_len, ce TSRMLS_CC);

    efree(result);

    return ret;
}

static int seaslog_log_ex(int argc, char *level, int level_int, char *message, int message_len, char *module, int module_len, zend_class_entry *ce TSRMLS_DC)
{
    logger_entry_t *logger;

    if (check_log_level(level_int TSRMLS_CC) == FAILURE)
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

    log_len = spprintf(&log_info, 0, "%s | %d | %s | %s | %s | %s \n", level, SEASLOG_G(process_id), SEASLOG_G(request_id), current_time, real_time, message);

    if (seaslog_real_buffer_log_ex(log_info, log_len, log_file_path, log_file_path_len + 1, ce TSRMLS_CC) == FAILURE)
    {
        efree(log_file_path);
        efree(log_info);
        return FAILURE;
    }

    efree(log_file_path);
    efree(log_info);
    efree(current_time);

    return SUCCESS;
}

static int appender_handle_tcp_udp(char *message, int message_len, char *level, logger_entry_t *logger, zend_class_entry *ce TSRMLS_DC)
{
    char *log_info, *current_time, *real_time;
    int log_len;

    current_time = mic_time();
    real_time = make_real_time(TSRMLS_C);

    log_len = spprintf(&log_info, 0, "%s | %s | %s | %d | %s | %s | %s | %s \n", SEASLOG_G(host_name), logger->logger, level, SEASLOG_G(process_id), SEASLOG_G(request_id), current_time, real_time, message);

    if (seaslog_real_buffer_log_ex(log_info, log_len, logger->logger, logger->logger_len, ce TSRMLS_CC) == FAILURE)
    {
        efree(log_info);
        return FAILURE;
    }

    efree(log_info);
    efree(current_time);

    return SUCCESS;
}

static int check_log_level(int level TSRMLS_DC)
{
    if (SEASLOG_G(level) < 1) return SUCCESS;
    if (SEASLOG_G(level) > 8) return FAILURE;

    switch (level)
    {
        case SEASLOG_DEBUG_INT:
            if (SEASLOG_G(level) <= SEASLOG_DEBUG_INT) return SUCCESS;
            break;
        case SEASLOG_INFO_INT:
            if (SEASLOG_G(level) <= SEASLOG_INFO_INT) return SUCCESS;
            break;
        case SEASLOG_NOTICE_INT:
            if (SEASLOG_G(level) <= SEASLOG_NOTICE_INT) return SUCCESS;
            break;
        case SEASLOG_WARNING_INT:
            if (SEASLOG_G(level) <= SEASLOG_WARNING_INT) return SUCCESS;
            break;
        case SEASLOG_ERROR_INT:
            if (SEASLOG_G(level) <= SEASLOG_ERROR_INT) return SUCCESS;
            break;
        case SEASLOG_CRITICAL_INT:
            if (SEASLOG_G(level) <= SEASLOG_CRITICAL_INT) return SUCCESS;
            break;
        case SEASLOG_ALERT_INT:
            if (SEASLOG_G(level) <= SEASLOG_ALERT_INT) return SUCCESS;
            break;
        case SEASLOG_EMERGENCY_INT:
            if (SEASLOG_G(level) <= SEASLOG_EMERGENCY_INT) return SUCCESS;
            break;
        default:
            return FAILURE;
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
        return seaslog_real_log_ex(message, message_len, log_file_path, log_file_path_len TSRMLS_CC);
    }
}

static int make_log_dir(char *dir TSRMLS_DC)
{
	int ret;

    if (SEASLOG_G(appender) == SEASLOG_APPENDER_FILE)
    {
        if (strncasecmp(dir, "file://", sizeof("file://") - 1) == 0) {
            dir += sizeof("file://") - 1;
        }

        if (VCWD_ACCESS(dir, F_OK) == 0) {
            return SUCCESS;
        }

        /* we look for directory separator from the end of string, thus hopefuly reducing our work load */
        char *p;
        char *e;
        SEASLOG_INIT_STAT(sb);

        int dir_len = (int)strlen(dir);
        int offset = 0;
        char buf[MAXPATHLEN];

        if (!expand_filepath_with_mode(dir, buf, NULL, 0, CWD_EXPAND TSRMLS_CC)) {
            seaslog_throw_exception(SEASLOG_EXCEPTION_LOGGER_ERROR TSRMLS_CC, "%s %s", dir, "Invalid path");
            return FAILURE;
        }

        e = buf +  strlen(buf);

        if ((p = memchr(buf, DEFAULT_SLASH, dir_len))) {
            offset = p - buf + 1;
        }

        if (p && dir_len == 1) {
            /* buf == "DEFAULT_SLASH" */
        }
        else
        {
            /* find a top level directory we need to create */
            while ( (p = strrchr(buf + offset, DEFAULT_SLASH)) || (offset != 1 && (p = strrchr(buf, DEFAULT_SLASH))) ) {
                int n = 0;

                *p = '\0';
                while (p > buf && *(p-1) == DEFAULT_SLASH) {
                    ++n;
                    --p;
                    *p = '\0';
                }
                if (VCWD_STAT(buf, &sb) == 0) {
                    while (1) {
                        *p = DEFAULT_SLASH;
                        if (!n) break;
                        --n;
                        ++p;
                    }
                    break;
                }
            }
        }

        if (p == buf) {
            ret = php_mkdir_ex(dir, SEASLOG_DIR_MODE, PHP_STREAM_MKDIR_RECURSIVE TSRMLS_CC);
            if (ret < 0)
            {
                seaslog_throw_exception(SEASLOG_EXCEPTION_LOGGER_ERROR TSRMLS_CC, "%s %s", dir, strerror(errno));
            }
        }
        else
        {
             if (!(ret = php_mkdir_ex(buf, SEASLOG_DIR_MODE, PHP_STREAM_MKDIR_RECURSIVE TSRMLS_CC))) {
                if (!p) {
                    p = buf;
                }

                while (++p != e) {
                    if (*p == '\0') {
                        *p = DEFAULT_SLASH;
                        if ((*(p+1) != '\0') &&
                            (ret = VCWD_MKDIR(buf, SEASLOG_DIR_MODE)) < 0) {
                            seaslog_throw_exception(SEASLOG_EXCEPTION_LOGGER_ERROR TSRMLS_CC, "%s %s", buf, strerror(errno));
                            break;
                        }
                    }
                }
            }
            else
            {
                 seaslog_throw_exception(SEASLOG_EXCEPTION_LOGGER_ERROR TSRMLS_CC, "%s %s", buf, strerror(errno));
            }
        }

        if (ret < 0) {
            return FAILURE;
        }

        return SUCCESS;
    }
    else
    {
        return SUCCESS;
    }
}