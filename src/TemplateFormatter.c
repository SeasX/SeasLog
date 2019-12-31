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

#include "TemplateFormatter.h"
#include "Common.h"
#include "Performance.h"
#include "Datetime.h"
#include "Request.h"

#define NUL             '\0'
#define INT_NULL        ((int *)0)

#define S_NULL          "(null)"
#define S_NULL_LEN      6

#if PHP_VERSION_ID >= 70000
#define EXIST_CHAR(xbuf)  xbuf->s
#else
#define EXIST_CHAR(xbuf) xbuf->c
#endif

#define INS_CHAR_NR(xbuf, ch) do {	\
	smart_str_appendc(xbuf, ch);	\
} while (0)

#define INS_STRING(xbuf, s, slen) do { 	\
	smart_str_appendl(xbuf, s, slen);	\
} while (0)

static smart_str* get_class_and_action(smart_str *result  TSRMLS_DC)
{
    char *func;
    char *cls;
    int recall_depth = SEASLOG_G(recall_depth);
    zend_execute_data *execute_data = EG(current_execute_data);

    if (!execute_data || !(execute_data = execute_data->prev_execute_data))
    {
        return NULL;
    }

#if PHP_VERSION_ID >= 70000

    while(recall_depth > 0)
    {
        if (execute_data->prev_execute_data != NULL && execute_data->prev_execute_data->func &&
                ZEND_USER_CODE(execute_data->prev_execute_data->func->common.type)
           )
        {
            execute_data = execute_data->prev_execute_data;
        }
        else
        {
            break;
        }
        recall_depth--;
    }
#else
    while(recall_depth > 0)
    {
        if (execute_data->prev_execute_data && execute_data->prev_execute_data->opline)
        {
            execute_data = execute_data->prev_execute_data;
        }
        else
        {
            break;
        }
        recall_depth--;
    }
#endif

    func = seaslog_performance_get_function_name(execute_data TSRMLS_CC);
    if (!func)
    {
        return NULL;
    }

    cls = seaslog_performance_get_class_name(execute_data TSRMLS_CC);
    if (cls)
    {
        INS_STRING(result, cls, strlen(cls));
        INS_STRING(result, "::", 2);
    }

    INS_STRING(result, func, strlen(func));

    efree(cls);
    efree(func);

    return result;
}

void seaslog_init_template(TSRMLS_D)
{
    seaslog_spprintf(&SEASLOG_G(current_template) TSRMLS_CC, SEASLOG_GENERATE_CURRENT_TEMPLATE, NULL, 0);
}

void seaslog_re_init_template(TSRMLS_D)
{
    efree(SEASLOG_G(current_template));
    seaslog_spprintf(&SEASLOG_G(current_template) TSRMLS_CC, SEASLOG_GENERATE_RE_CURRENT_TEMPLATE, NULL, 0);
}

void seaslog_clear_template(TSRMLS_D)
{
    if (SEASLOG_G(current_template))
    {
        efree(SEASLOG_G(current_template));
    }
    if (SEASLOG_G(level_template))
    {
        efree(SEASLOG_G(level_template));
    }
}

int seaslog_spprintf(char **pbuf TSRMLS_DC, int generate_type, char *level, size_t max_len, ...)
{
    int len;
    va_list ap;

    smart_str xbuf = {0};

    va_start(ap, max_len);

    switch (generate_type)
    {
    case SEASLOG_GENERATE_CURRENT_TEMPLATE:
    case SEASLOG_GENERATE_RE_CURRENT_TEMPLATE:
        seaslog_template_formatter(&xbuf TSRMLS_CC, generate_type, SEASLOG_G(default_template), level, ap);
        break;
    case SEASLOG_GENERATE_LEVEL_TEMPLATE:
        if (strlen(SEASLOG_G(level_template)) == 0 || (level && !strcmp(level, SEASLOG_ALL)))
        {
            INS_STRING(&xbuf, SEASLOG_ALL, strlen(SEASLOG_ALL));
        }
        else
        {
            seaslog_template_formatter(&xbuf TSRMLS_CC, generate_type, SEASLOG_G(level_template), level, ap);
        }
        break;
    case SEASLOG_GENERATE_SYSLOG_INFO:
    case SEASLOG_GENERATE_LOG_INFO:
        seaslog_template_formatter(&xbuf TSRMLS_CC, generate_type, SEASLOG_G(current_template), level, ap);
        break;
    default:
        break;
    }
    va_end(ap);

    if (max_len && seaslog_smart_str_get_len(xbuf) > max_len)
    {
        SEASLOG_SMART_STR_L(xbuf) = max_len;
    }
    smart_str_0(&xbuf);

    *pbuf = estrdup(SEASLOG_SMART_STR_C(xbuf));
    len = seaslog_smart_str_get_len(xbuf);

    smart_str_free(&xbuf);

    return len;
}

void seaslog_template_formatter(smart_str *xbuf TSRMLS_DC, int generate_type, const char *fmt, char *level, va_list ap)
{
    char *s = NULL;
    int s_len;

    char char_buf[2];
    smart_str tmp_buf = {0};

    char level_template[SEASLOG_BUFFER_MAX_SIZE];
    int level_format_start = 0;
    int level_format_stop = 0;
    int level_format_over = 0;
    int level_format_index = 0;

    if (SEASLOG_GENERATE_CURRENT_TEMPLATE == generate_type)
    {
        level_template[0] = '\0';
    }

    while (*fmt)
    {
        if (*fmt != '%')
        {
            INS_CHAR_NR(xbuf, *fmt);
            if (SEASLOG_GENERATE_CURRENT_TEMPLATE == generate_type)
            {
                if (level_format_stop == 0)
                {
                    level_template[level_format_index] = *fmt;
                    level_format_index++;
                }
            }
        }
        else
        {
            fmt++;

            switch(generate_type)
            {
            case SEASLOG_GENERATE_CURRENT_TEMPLATE:
            case SEASLOG_GENERATE_RE_CURRENT_TEMPLATE:
            {
                if (SEASLOG_GENERATE_CURRENT_TEMPLATE == generate_type)
                {
                    if (!level_format_stop)
                    {
                        if (level_format_start)
                        {
                            level_format_stop = 1;
                            level_template[level_format_index] = '\0';
                            level_format_over = 1;
                        }
                        else
                        {
                            if (*fmt == 'L')
                            {
                                level_template[level_format_index++] = '%';
                                level_template[level_format_index] = *fmt;
                                level_format_index++;
                                level_format_start = 1;
                            }
                            else
                            {
                                level_format_index = 0;
                            }
                        }
                    }
                }

                switch (*fmt)
                {
                case 'H': //HostName
                    s = SEASLOG_G(host_name);
                    s_len  = SEASLOG_G(host_name_len);
                    break;
                case 'P': //Precess Id
                    s = SEASLOG_G(process_id);
                    s_len  = SEASLOG_G(process_id_len);
                    break;
                case 'D': //Domain:Port
                    s = SEASLOG_G(request_variable)->domain_port;
                    s_len = SEASLOG_G(request_variable)->domain_port_len;
                    break;
                case 'R': //Request Uri
                    s = SEASLOG_G(request_variable)->request_uri;
                    s_len = SEASLOG_G(request_variable)->request_uri_len;
                    break;
                case 'm': //Request Method
                    s = SEASLOG_G(request_variable)->request_method;
                    s_len = SEASLOG_G(request_variable)->request_method_len;
                    break;
                case 'I': //Client IP
                    s = SEASLOG_G(request_variable)->client_ip;
                    s_len = SEASLOG_G(request_variable)->client_ip_len;
                    break;
                case NUL:
                    continue;
                default:
                    char_buf[0] = '%';
                    char_buf[1] = *fmt;
                    s = char_buf;
                    s_len = 2;
                    break;
                }
            }
            break;
            case SEASLOG_GENERATE_LOG_INFO:
            case SEASLOG_GENERATE_LEVEL_TEMPLATE:
            case SEASLOG_GENERATE_SYSLOG_INFO:
            {
                switch (*fmt)
                {
                case 'T': //Time  2017:08:16 19:15:02
                    s = make_real_time(TSRMLS_C);
                    s_len  = strlen(s);
                    break;
                case 't': //time  1502882102.862
                {
                    if (SEASLOG_SMART_STR_C(tmp_buf))
                    {
                        smart_str_free(&tmp_buf);
                    }
                    mic_time(&tmp_buf);
                    s = SEASLOG_SMART_STR_C(tmp_buf);
                    s_len  = seaslog_smart_str_get_len(tmp_buf);
                }
                break;
                case 'Q': //Request uniqid
                    s = SEASLOG_G(request_id);
                    s_len  = SEASLOG_G(request_id_len);
                    break;
                case 'L': //Level
                    s = level;
                    s_len = strlen(level);
                    break;
                case 'M': //Message
                    s = va_arg(ap, char *);
                    if (s != NULL)
                    {
                        s_len = strlen(s);
                    }
                    else
                    {
                        s = S_NULL;
                        s_len = S_NULL_LEN;
                    }
                    break;
                case 'F': //Filename:Line  test.php:28
                    if (SEASLOG_SMART_STR_C(tmp_buf))
                    {
                        smart_str_free(&tmp_buf);
                    }
                    get_code_filename_line(&tmp_buf TSRMLS_CC);

                    s = SEASLOG_SMART_STR_C(tmp_buf);
                    s_len = seaslog_smart_str_get_len(tmp_buf);
                    break;
                case 'U': //zend_memory_usage
                    if (SEASLOG_SMART_STR_C(tmp_buf))
                    {
                        smart_str_free(&tmp_buf);
                    }
                    seaslog_memory_usage(&tmp_buf TSRMLS_CC);
                    s = SEASLOG_SMART_STR_C(tmp_buf);
                    s_len = seaslog_smart_str_get_len(tmp_buf);
                    break;
                case 'u': //peak_memory_usage
                    if (SEASLOG_SMART_STR_C(tmp_buf))
                    {
                        smart_str_free(&tmp_buf);
                    }
                    seaslog_peak_memory_usage(&tmp_buf TSRMLS_CC);
                    s = SEASLOG_SMART_STR_C(tmp_buf);
                    s_len  = seaslog_smart_str_get_len(tmp_buf);
                    break;
                case 'C':
                    if (SEASLOG_SMART_STR_C(tmp_buf))
                    {
                        smart_str_free(&tmp_buf);
                    }

                    if (get_class_and_action(&tmp_buf TSRMLS_CC))
                    {
                        s = SEASLOG_SMART_STR_C(tmp_buf);
                        s_len  = seaslog_smart_str_get_len(tmp_buf);
                    }
                    else
                    {
                        s = S_NULL;
                        s_len = S_NULL_LEN;
                    }
                    break;
                case 'B': //BasePath
                    s = SEASLOG_G(base_path);
                    if(s == NULL)
                    {
                        s = SEASLOG_G(default_basepath);
                    }
                    s_len = strlen(s);
                    break;
                case NUL:
                    continue;
                default:
                    char_buf[0] = '%';
                    char_buf[1] = *fmt;
                    s = char_buf;
                    s_len = 2;
                    break;
                }
            }
            break;
            }

            INS_STRING(xbuf, s, s_len);
        }
skip_output:
        fmt++;
    }

    switch(generate_type)
    {
    case SEASLOG_GENERATE_LOG_INFO:
    case SEASLOG_GENERATE_SYSLOG_INFO:
        INS_STRING(xbuf, SEASLOG_LOG_LINE_FEED_STR, SEASLOG_LOG_LINE_FEED_LEN);
        break;
    case SEASLOG_GENERATE_CURRENT_TEMPLATE:
    {
        if (!level_format_start)
        {
            level_format_index = 0;
        }
        if (!level_format_over)
        {
            level_template[level_format_index] = '\0';
        }
        SEASLOG_G(level_template) = estrdup(level_template);
        break;
    }
    }

    if (!EXIST_CHAR(xbuf))
    {
        INS_STRING(xbuf, "", 0);
    }

    if (SEASLOG_SMART_STR_C(tmp_buf))
    {
        smart_str_free(&tmp_buf);
    }
}

