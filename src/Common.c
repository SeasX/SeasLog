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

static char *str_replace(char *ori, char * rep, char * with)
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
    for (count = 0; (tmp = strstr(ins, rep)) != NULL ; ++count)
    {
        ins = tmp + len_rep;
    }

    tmp = result = emalloc(strlen(ori) + (len_with - len_rep) * count + 1);

    if (!result)
        return NULL;

    while (count--)
    {
        ins = strstr(ori, rep);
        len_front = ins - ori;
        tmp = strncpy(tmp, ori, len_front) + len_front;
        tmp = strcpy(tmp, with) + len_with;
        ori += len_front + len_rep;
    }
    strcpy(tmp, ori);
    return result;
}

static char *str_replace_php7(char *src, const char *oldstr, const char *newstr, size_t len)
{
    char *needle;
    char *tmp;

    if(strcmp(oldstr, newstr)==0)
    {
        return src;
    }

    while((needle = strstr(src, oldstr)) && (needle - src <= len))
    {
        tmp = (char*)emalloc(strlen(src) + (strlen(newstr) - strlen(oldstr)) + 1);

        strncpy(tmp, src, needle-src);

        tmp[needle-src]='\0';
        strcat(tmp, newstr);
        strcat(tmp, needle+strlen(oldstr));

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
                tmp = str_replace_php7(tmp, ZSTR_VAL(str_key), ZSTR_VAL(s), strlen(str));
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