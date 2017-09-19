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

static int seaslog_get_level_int(char *level)
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

#if PHP_VERSION_ID >= 70000
static char *php_strtr_array(char *str, int slen, HashTable *pats)
{
    zend_ulong num_key;
    zend_string *str_key;
    zval *entry;
    char *tmp = estrdup(str);

    ZEND_HASH_FOREACH_KEY_VAL(pats, num_key, str_key, entry)
    {
        if (UNEXPECTED(!str_key))
        {

        }
        else
        {
            zend_string *s = zval_get_string(entry);

            if (strstr(str,ZSTR_VAL(str_key)))
            {
                tmp = str_replace(tmp, ZSTR_VAL(str_key), ZSTR_VAL(s));
            }

            zend_string_release(s);
        }
    }
    ZEND_HASH_FOREACH_END();

    return tmp;
}

#else
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
    char  *result_str;
    HashTable tmp_hash;

    zend_hash_init(&tmp_hash, zend_hash_num_elements(hash), NULL, NULL, 0);
    zend_hash_internal_pointer_reset_ex(hash, &hpos);

    while (zend_hash_get_current_data_ex(hash, (void **)&entry, &hpos) == SUCCESS)
    {
        switch (zend_hash_get_current_key_ex(hash, &string_key, &string_key_len, &num_key, 0, &hpos))
        {
        case HASH_KEY_IS_STRING:
            len = string_key_len - 1;
            if (len < 1)
            {
                zend_hash_destroy(&tmp_hash);
            }
            else
            {
                zend_hash_add(&tmp_hash, string_key, string_key_len, entry, sizeof(zval*), NULL);
                if (len > maxlen)
                {
                    maxlen = len;
                }
                if (len < minlen)
                {
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

            if (len > maxlen)
            {
                maxlen = len;
            }
            if (len < minlen)
            {
                minlen = len;
            }
            break;
        }
        zend_hash_move_forward_ex(hash, &hpos);
    }

    key = emalloc(maxlen + 1);
    pos = 0;

    while (pos < slen)
    {
        if ((pos + maxlen) > slen)
        {
            maxlen = slen - pos;
        }

        found = 0;
        memcpy(key, str + pos, maxlen);

        for (len = maxlen; len >= minlen; len--)
        {
            key[len] = 0;

            if (zend_hash_find(&tmp_hash, key, len + 1, (void**)&trans) == SUCCESS)
            {
                char *tval;
                int tlen;
                zval tmp;

                if (Z_TYPE_PP(trans) != IS_STRING)
                {
                    tmp = **trans;
                    zval_copy_ctor(&tmp);
                    convert_to_string(&tmp);
                    tval = Z_STRVAL(tmp);
                    tlen = Z_STRLEN(tmp);
                }
                else
                {
                    tval = Z_STRVAL_PP(trans);
                    tlen = Z_STRLEN_PP(trans);
                }

                smart_str_appendl(&result, tval, tlen);
                pos += len;
                found = 1;

                if (Z_TYPE_PP(trans) != IS_STRING)
                {
                    zval_dtor(&tmp);
                }

                break;
            }
        }

        if (! found)
        {
            smart_str_appendc(&result, str[pos++]);
        }
    }

    zend_hash_destroy(&tmp_hash);
    result_str = estrndup(result.c, result.len);
    efree(key);
    smart_str_free(&result);

    return result_str;
}
#endif

static int message_trim_wrap(char *message,int message_len TSRMLS_DC)
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

static char *delN(char *a)
{
    int l;
    l = strlen(a);
    a[l - 1] = '\0';

    return a;
}

static char *get_uniqid()
{
    char *uniqid;
    struct timeval tv;

    timerclear(&tv);
    gettimeofday(&tv, NULL);

    spprintf(&uniqid, 0, "%08x%05x", (int)tv.tv_sec, (int)tv.tv_usec % 0x100000);
    return uniqid;
}

static void initRStart(TSRMLS_D)
{
    SEASLOG_G(initRComplete) = SEASLOG_INITR_COMPLETE_NO;
    SEASLOG_G(error_loop) = 0;
}

static void initREnd(TSRMLS_D)
{
    SEASLOG_G(initRComplete) = SEASLOG_INITR_COMPLETE_YES;
}