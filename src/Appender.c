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

#include "Appender.h"
#include "Common.h"
#include "Logger.h"
#include "Datetime.h"
#include "Buffer.h"
#include "TemplateFormatter.h"
#include "StreamWrapper.h"
#include "ExceptionHook.h"

static int seaslog_real_log_ex(char *message, int message_len, char *opt, int opt_len TSRMLS_DC)
{
    size_t written;
    int retry = SEASLOG_G(appender_retry);
    php_stream *stream = NULL;

    stream = process_stream(opt,opt_len TSRMLS_CC);

    if (!stream)
    {
        return FAILURE;
    }

    written = php_stream_write(stream, message, message_len);
    if (written != message_len)
    {
        if (retry > 0)
        {
            while(retry > 0)
            {
                written = php_stream_write(stream, message, message_len);
                if (written == message_len)
                {
                    return SUCCESS;
                }
                else
                {
                    retry--;
                }
            }
        }

        switch SEASLOG_G(appender)
        {
        case SEASLOG_APPENDER_TCP:
            seaslog_throw_exception(SEASLOG_EXCEPTION_LOGGER_ERROR TSRMLS_CC, "SeasLog Can Not Send Data To TCP Server - tcp://%s:%d - %s", SEASLOG_G(remote_host), SEASLOG_G(remote_port), message);
            break;
        case SEASLOG_APPENDER_UDP:
            seaslog_throw_exception(SEASLOG_EXCEPTION_LOGGER_ERROR TSRMLS_CC, "SeasLog Can Not Send Data To UDP Server - udp://%s:%d - %s", SEASLOG_G(remote_host), SEASLOG_G(remote_port), message);
            break;
        case SEASLOG_APPENDER_FILE:
        default:
            seaslog_throw_exception(SEASLOG_EXCEPTION_LOGGER_ERROR TSRMLS_CC, "SeasLog Can Not Send Data To File - %s - %s", opt, message);
        }

        return FAILURE;
    }

    return SUCCESS;
}

int seaslog_log_content(int argc, char *level, int level_int, char *message, int message_len, HashTable *content, char *module, int module_len, zend_class_entry *ce TSRMLS_DC)
{
    int ret;
    char *result = php_strtr_array(message, message_len, content);

    ret = seaslog_log_ex(argc, level, level_int, result, strlen(result), module, module_len, ce TSRMLS_CC);

    efree(result);

    return ret;
}

static int seaslog_real_buffer_log_ex(char *message, int message_len, char *log_file_path, int log_file_path_len, zend_class_entry *ce TSRMLS_DC)
{
    if (seaslog_check_buffer_enable(TSRMLS_C))
    {
        seaslog_buffer_set(message, message_len, log_file_path, log_file_path_len, ce TSRMLS_CC);
        return SUCCESS;
    }
    else
    {
        return seaslog_real_log_ex(message, message_len, log_file_path, log_file_path_len TSRMLS_CC);
    }
}

static int appender_handle_tcp_udp(char *message, int message_len, char *level, int level_int, logger_entry_t *logger, zend_class_entry *ce TSRMLS_DC)
{
    char *log_info, *log_content, *time_RFC3339;
    int log_len, log_content_len, PRI;

    time_RFC3339 = make_time_RFC3339(TSRMLS_C);

    PRI = SEASLOG_SYSLOG_FACILITY + level_int;

    log_content_len = seaslog_spprintf(&log_content TSRMLS_CC, SEASLOG_GENERATE_SYSLOG_INFO, level, 0, message);

    log_len = spprintf(&log_info, 0, "<%d>1 %s %s %s %s %s %s", PRI, time_RFC3339, SEASLOG_G(host_name), SEASLOG_G(request_variable)->domain_port, SEASLOG_G(process_id), logger->logger, log_content);

    efree(time_RFC3339);
    efree(log_content);

    if (seaslog_real_buffer_log_ex(log_info, log_len, logger->logger, logger->logger_len, ce TSRMLS_CC) == FAILURE)
    {
        efree(log_info);
        return FAILURE;
    }

    efree(log_info);
    return SUCCESS;
}

static int appender_handle_file(char *message, int message_len, char *level, int level_int, logger_entry_t *logger, zend_class_entry *ce TSRMLS_DC)
{

    char *log_file_path, *log_info, *real_date;
    int log_len, log_file_path_len;

    real_date = make_real_date(TSRMLS_C);
    if (SEASLOG_G(disting_type))
    {
        log_file_path_len = spprintf(&log_file_path, 0, "%s%s%s.%s.log", logger->logger_path, SEASLOG_G(slash_or_underline), real_date, level);
    }
    else
    {
        log_file_path_len = spprintf(&log_file_path, 0, "%s%s%s.log", logger->logger_path, SEASLOG_G(slash_or_underline), real_date);
    }

    log_len = seaslog_spprintf(&log_info TSRMLS_CC, SEASLOG_GENERATE_LOG_INFO, level, 0, message);

    if (seaslog_real_buffer_log_ex(log_info, log_len, log_file_path, log_file_path_len + 1, ce TSRMLS_CC) == FAILURE)
    {
        efree(log_file_path);
        efree(log_info);
        return FAILURE;
    }

    efree(log_file_path);
    efree(log_info);

    return SUCCESS;
}

int seaslog_log_ex(int argc, char *level, int level_int, char *message, int message_len, char *module, int module_len, zend_class_entry *ce TSRMLS_DC)
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
        return appender_handle_tcp_udp(message, message_len, level, level_int, logger, ce TSRMLS_CC);
        break;
    case SEASLOG_APPENDER_FILE:
    default:
        return appender_handle_file(message, message_len, level, level_int, logger, ce TSRMLS_CC);
    }

    return SUCCESS;
}

int make_log_dir(char *dir TSRMLS_DC)
{
    int ret, dir_len, offset;
    char *p, *e;
    char buf[MAXPATHLEN];

#if PHP_VERSION_ID >= 70000
    zend_stat_t sb;
#else
    struct stat sb;
#endif


    if (SEASLOG_G(appender) == SEASLOG_APPENDER_FILE)
    {
        if (strncasecmp(dir, "file://", sizeof("file://") - 1) == 0)
        {
            dir += sizeof("file://") - 1;
        }

        if (VCWD_ACCESS(dir, F_OK) == 0)
        {
            return SUCCESS;
        }

        dir_len = (int)strlen(dir);
        offset = 0;

        if (!SEASLOG_EXPAND_FILE_PATH(dir, buf))
        {
            seaslog_throw_exception(SEASLOG_EXCEPTION_LOGGER_ERROR TSRMLS_CC, "%s %s", dir, "Invalid path");
            return FAILURE;
        }

        e = buf +  strlen(buf);

        if ((p = memchr(buf, DEFAULT_SLASH, dir_len)))
        {
            offset = p - buf + 1;
        }

        if (p && dir_len == 1)
        {
            /* buf == "DEFAULT_SLASH" */
        }
        else
        {
            /* find a top level directory we need to create */
            while ( (p = strrchr(buf + offset, DEFAULT_SLASH)) || (offset != 1 && (p = strrchr(buf, DEFAULT_SLASH))) )
            {
                int n = 0;

                *p = '\0';
                while (p > buf && *(p-1) == DEFAULT_SLASH)
                {
                    ++n;
                    --p;
                    *p = '\0';
                }

                if (VCWD_STAT(buf, &sb) == 0)
                {
                    while (1)
                    {
                        *p = DEFAULT_SLASH;
                        if (!n) break;
                        --n;
                        ++p;
                    }
                    break;
                }
            }
        }

        if (p == buf)
        {
            ret = php_mkdir_ex(dir, SEASLOG_DIR_MODE, PHP_STREAM_MKDIR_RECURSIVE TSRMLS_CC);
            if (ret < 0)
            {
                seaslog_throw_exception(SEASLOG_EXCEPTION_LOGGER_ERROR TSRMLS_CC, "%s %s", dir, strerror(errno));
            }
        }
        else
        {
            if (!(ret = php_mkdir_ex(buf, SEASLOG_DIR_MODE, PHP_STREAM_MKDIR_RECURSIVE TSRMLS_CC)))
            {
                if (!p)
                {
                    p = buf;
                }

                while (++p != e)
                {
                    if (*p == '\0')
                    {
                        *p = DEFAULT_SLASH;
                        if ((*(p+1) != '\0') &&
                                (ret = VCWD_MKDIR(buf, SEASLOG_DIR_MODE)) < 0)
                        {
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

        if (ret < 0)
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

