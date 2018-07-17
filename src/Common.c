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

#include "Common.h"

int check_sapi_is_cli(TSRMLS_D)
{
    if (!strncmp(sapi_module.name , SEASLOG_CLI_KEY, sizeof(SEASLOG_CLI_KEY) - 1))
    {
        return SUCCESS;
    }

    return FAILURE;
}

int seaslog_get_level_int(char *level)
{
    if (strcmp(level, SEASLOG_DEBUG) == 0)
    {
        return SEASLOG_DEBUG_INT;
    }
    else if (strcmp(level, SEASLOG_INFO) == 0)
    {
        return SEASLOG_INFO_INT;
    }
    else if (strcmp(level, SEASLOG_NOTICE) == 0)
    {
        return SEASLOG_NOTICE_INT;
    }
    else if (strcmp(level, SEASLOG_WARNING) == 0)
    {
        return SEASLOG_WARNING_INT;
    }
    else if (strcmp(level, SEASLOG_ERROR) == 0)
    {
        return SEASLOG_ERROR_INT;
    }
    else if (strcmp(level, SEASLOG_CRITICAL) == 0)
    {
        return SEASLOG_CRITICAL_INT;
    }
    else if (strcmp(level, SEASLOG_ALERT) == 0)
    {
        return SEASLOG_ALERT_INT;
    }
    else if (strcmp(level, SEASLOG_EMERGENCY) == 0)
    {
        return SEASLOG_EMERGENCY_INT;
    }

    return SEASLOG_DEBUG_INT;
}

int check_log_level(int level TSRMLS_DC)
{
    if (level >= SEASLOG_DEBUG_INT) return SUCCESS;
    if (level < SEASLOG_EMERGENCY_INT) return FAILURE;

    switch (level)
    {
    case SEASLOG_DEBUG_INT:
        if (level >= SEASLOG_DEBUG_INT) return SUCCESS;
        break;
    case SEASLOG_INFO_INT:
        if (level >= SEASLOG_INFO_INT) return SUCCESS;
        break;
    case SEASLOG_NOTICE_INT:
        if (level >= SEASLOG_NOTICE_INT) return SUCCESS;
        break;
    case SEASLOG_WARNING_INT:
        if (level >= SEASLOG_WARNING_INT) return SUCCESS;
        break;
    case SEASLOG_ERROR_INT:
        if (level >= SEASLOG_ERROR_INT) return SUCCESS;
        break;
    case SEASLOG_CRITICAL_INT:
        if (level >= SEASLOG_CRITICAL_INT) return SUCCESS;
        break;
    case SEASLOG_ALERT_INT:
        if (level >= SEASLOG_ALERT_INT) return SUCCESS;
        break;
    case SEASLOG_EMERGENCY_INT:
        if (level >= SEASLOG_EMERGENCY_INT) return SUCCESS;
        break;
    default:
        return FAILURE;
    }

    return FAILURE;
}

static char *str_replace(char *src, const char *from, const char *to)
{
    char *needle;
    char *tmp;
    size_t len;
    size_t len_from;
    size_t len_to;

    if(strcmp(from, to) == 0)
    {
        return src;
    }

    len = strlen(src);
    len_from = strlen(from);
    len_to = strlen(to);

    while((needle = strstr(src, from)) && (needle - src <= len))
    {
        tmp = (char*)emalloc(len + (len_to - len_from) + 1);

        strncpy(tmp, src, needle - src);

        tmp[needle - src]='\0';
        strcat(tmp, to);
        strcat(tmp, needle + len_from);

        efree(src);
        src = tmp;
        len = strlen(src);
    }

    return src;
}

static char *str_appender(char *str, int str_len)
{
    char *string_key_tmp = NULL;
    smart_str tmp_key = {0};

    smart_str_appendc(&tmp_key, '{');
    smart_str_appendl(&tmp_key, str, str_len);
    smart_str_appendc(&tmp_key, '}');
    smart_str_0(&tmp_key);
    string_key_tmp = estrndup(SEASLOG_SMART_STR_C(tmp_key), SEASLOG_SMART_STR_L(tmp_key));

    smart_str_free(&tmp_key);

    return string_key_tmp;
}

#if PHP_VERSION_ID >= 70000
char* php_strtr_array(char *str, int slen, HashTable *hash)
{
    zend_ulong num_key;
    zend_string *str_key;
    char  *string_key, *string_key_tmp = NULL;
    zval *entry;
    char *tmp = estrdup(str);

    ZEND_HASH_FOREACH_KEY_VAL(hash, num_key, str_key, entry)
    {
        if (UNEXPECTED(!str_key))
        {

        }
        else
        {
            zend_string *s = zval_get_string(entry);

            string_key = ZSTR_VAL(str_key);

            if (string_key_tmp)
            {
                efree(string_key_tmp);
            }

            if (string_key[0] != '{')
            {
                string_key_tmp = str_appender(string_key, ZSTR_LEN(str_key));
            }
            else
            {
                string_key_tmp = estrdup(string_key);
            }

            if (strstr(tmp,string_key_tmp))
            {
                tmp = str_replace(tmp, string_key_tmp, ZSTR_VAL(s));
            }

            zend_string_release(s);
        }
    }
    ZEND_HASH_FOREACH_END();

    if (string_key_tmp)
    {
        efree(string_key_tmp);
    }

    return tmp;
}
#else
char* php_strtr_array(char *str, int slen, HashTable *hash)
{
    zval **entry;
    char  *string_key, *string_key_tmp = NULL;
    uint   string_key_len;
    ulong  num_key;
    HashPosition hpos;

    char *tmp = estrdup(str);

    zend_hash_internal_pointer_reset_ex(hash, &hpos);

    while (zend_hash_get_current_data_ex(hash, (void **)&entry, &hpos) == SUCCESS)
    {
        switch (zend_hash_get_current_key_ex(hash, &string_key, &string_key_len, &num_key, 0, &hpos))
        {
        case HASH_KEY_IS_STRING:
            if (string_key_len > 1)
            {
                if (string_key[0] != '{')
                {
                    if (string_key_tmp)
                    {
                        efree(string_key_tmp);
                    }

                    string_key_tmp = str_appender(string_key, string_key_len - 1);
                }

                if (string_key_tmp)
                {
                    string_key = string_key_tmp;
                }

                if (strstr(tmp,string_key))
                {
                    if (IS_STRING != Z_TYPE_PP(entry))
                    {
                        zval strtmp;
                        strtmp = **entry;
                        zval_copy_ctor(&strtmp);
                        convert_to_string(&strtmp);
                        tmp = str_replace(tmp, string_key, Z_STRVAL(strtmp));
                        zval_dtor(&strtmp);
                    }
                    else
                    {
                        tmp = str_replace(tmp, string_key, Z_STRVAL_PP(entry));
                    }
                }
            }
            break;
        }
        zend_hash_move_forward_ex(hash, &hpos);
    }

    if (string_key_tmp)
    {
        efree(string_key_tmp);
    }

    return tmp;
}
#endif

int message_trim_wrap(char *message,int message_len TSRMLS_DC)
{
    int i;
    for (i=0; i<=message_len; i++)
    {
        if(message[i] == '\r' || message[i] == '\n')
        {
            message[i] = SEASLOG_TRIM_WRAP;
        }
    }

    return SUCCESS;
}

char *delN(char *a)
{
    int l;
    l = strlen(a);
    a[l - 1] = '\0';

    return a;
}

char *get_uniqid()
{
    char *uniqid;
    struct timeval tv;

    timerclear(&tv);
    gettimeofday(&tv, NULL);

    spprintf(&uniqid, 0, "%08x%05x", (int)tv.tv_sec, (int)tv.tv_usec % 0x100000);
    return uniqid;
}

