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
#include "Performance.h"
#include "Datetime.h"
#include "Request.h"

#define NUL             '\0'
#define INT_NULL        ((int *)0)

#define S_NULL          "(null)"
#define S_NULL_LEN      6

#define INS_CHAR_NR(xbuf, ch) do {	\
	smart_str_appendc(xbuf, ch);	\
} while (0)

#define INS_STRING(xbuf, s, slen) do { 	\
	smart_str_appendl(xbuf, s, slen);	\
} while (0)


void seaslog_init_template(TSRMLS_D)
{
    seaslog_spprintf(&SEASLOG_G(current_template) TSRMLS_CC, SEASLOG_GENERATE_CURRENT_TEMPLATE, NULL, 0);
}

void seaslog_clear_template(TSRMLS_D)
{
    if (SEASLOG_G(current_template))
    {
        efree(SEASLOG_G(current_template));
    }
}

int seaslog_spprintf(char **pbuf TSRMLS_DC, int generate_type, char *level, size_t max_len, ...)
{
    int len;
    va_list ap;

    smart_str xbuf = {0};

    va_start(ap, max_len);
    if (generate_type == SEASLOG_GENERATE_CURRENT_TEMPLATE)
    {
        seaslog_template_formatter(&xbuf TSRMLS_CC, generate_type, SEASLOG_G(default_template), level, ap);
    }
    else
    {
        seaslog_template_formatter(&xbuf TSRMLS_CC, generate_type, SEASLOG_G(current_template), level, ap);
    }
    va_end(ap);

    if (max_len && SEASLOG_SMART_STR_L(xbuf) > max_len)
    {
        SEASLOG_SMART_STR_L(xbuf) = max_len;
    }
    smart_str_0(&xbuf);

    *pbuf = estrdup(SEASLOG_SMART_STR_C(xbuf));
    len = SEASLOG_SMART_STR_L(xbuf);

    smart_str_free(&xbuf);

    return len;
}

void seaslog_template_formatter(smart_str *xbuf TSRMLS_DC, int generate_type, const char *fmt, char *level, va_list ap)
{
    char *s = NULL;
    int s_len;

    char char_buf[2];
    smart_str tmp_buf = {0};

    while (*fmt)
    {
        if (*fmt != '%')
        {
            INS_CHAR_NR(xbuf, *fmt);
        }
        else
        {
            fmt++;

            switch(generate_type)
            {
            case SEASLOG_GENERATE_CURRENT_TEMPLATE:
            {
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
                    s_len  = SEASLOG_SMART_STR_L(tmp_buf);
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
                    s_len  = SEASLOG_SMART_STR_L(tmp_buf);
                    break;
                case 'U': //zend_memory_usage
                    if (SEASLOG_SMART_STR_C(tmp_buf))
                    {
                        smart_str_free(&tmp_buf);
                    }
                    seaslog_memory_usage(&tmp_buf TSRMLS_CC);
                    s = SEASLOG_SMART_STR_C(tmp_buf);
                    s_len  = SEASLOG_SMART_STR_L(tmp_buf);
                    break;
                case 'u': //peak_memory_usage
                    if (SEASLOG_SMART_STR_C(tmp_buf))
                    {
                        smart_str_free(&tmp_buf);
                    }
                    seaslog_peak_memory_usage(&tmp_buf TSRMLS_CC);
                    s = SEASLOG_SMART_STR_C(tmp_buf);
                    s_len  = SEASLOG_SMART_STR_L(tmp_buf);
                    break;
                case 'C': //TODO Class::Action
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
    }

    if (SEASLOG_SMART_STR_C(tmp_buf))
    {
        smart_str_free(&tmp_buf);
    }
}

