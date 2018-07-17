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

#include "Buffer.h"
#include "Common.h"
#include "StreamWrapper.h"

void initBufferSwitch(TSRMLS_D)
{
    SEASLOG_G(enable_buffer_real) = FAILURE;

    if (SUCCESS == check_sapi_is_cli(TSRMLS_C) && SEASLOG_G(buffer_disabled_in_cli))
    {
        return;
    }

    if (SEASLOG_G(use_buffer) && SEASLOG_G(buffer_size) > 0)
    {
        SEASLOG_G(enable_buffer_real) = SUCCESS;
        return;
    }

    return;
}

int seaslog_check_buffer_enable(TSRMLS_D)
{
    return SUCCESS == SEASLOG_G(enable_buffer_real);
}

void seaslog_init_buffer(TSRMLS_D)
{
    zval *z_buffer;

    if (seaslog_check_buffer_enable(TSRMLS_C))
    {
        SEASLOG_G(buffer_count) = 0;

#if PHP_VERSION_ID >= 70000

        array_init(&SEASLOG_G(buffer));

#else

        SEASLOG_G(buffer) = NULL;

        MAKE_STD_ZVAL(z_buffer);
        array_init(z_buffer);

        SEASLOG_G(buffer) = z_buffer;
#endif
    }
}

static int real_php_log_buffer(zval *msg_buffer, char *opt, int opt_len TSRMLS_DC)
{
    php_stream *stream = NULL;
    HashTable *ht;

#if PHP_VERSION_ID >= 70000
    zend_ulong num_key;
    zend_string *str_key;
    zval *entry;
#else
    zval **log;
#endif

    stream = process_stream(opt,opt_len TSRMLS_CC);

    if (stream == NULL)
    {
        return FAILURE;
    }

#if PHP_VERSION_ID >= 70000

    ht = HASH_OF(msg_buffer);
    ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, str_key, entry)
    {
        zend_string *s = zval_get_string(entry);
        php_stream_write(stream, ZSTR_VAL(s), ZSTR_LEN(s));
        zend_string_release(s);
    }
    ZEND_HASH_FOREACH_END();
#else

    ht = HASH_OF(msg_buffer);

    zend_hash_internal_pointer_reset(ht);
    while (zend_hash_get_current_data(ht, (void **)&log) == SUCCESS)
    {
        convert_to_string_ex(log);
        php_stream_write(stream,Z_STRVAL_PP(log), Z_STRLEN_PP(log));
        zend_hash_move_forward(ht);
    }
#endif

    return SUCCESS;
}

int seaslog_buffer_set(char *log_info, int log_info_len, char *path, int path_len, zend_class_entry *ce TSRMLS_DC)
{
#if PHP_VERSION_ID >= 70000

    zval new_array;
    zval *_buffer_data_;

    if (IS_ARRAY != Z_TYPE(SEASLOG_G(buffer)))
    {
        return 0;
    }

    if (zend_hash_num_elements(Z_ARRVAL(SEASLOG_G(buffer))) < 1)
    {
        array_init(&new_array);
        SEASLOG_ADD_NEXT_INDEX_STRINGL(&new_array, log_info, log_info_len);
        SEASLOG_ADD_ASSOC_ZVAL_EX(&SEASLOG_G(buffer),path,path_len,&new_array);
    }
    else
    {
        if ((_buffer_data_ = zend_hash_str_find_ptr(Z_ARRVAL(SEASLOG_G(buffer)),path,path_len)) != NULL)
        {
            SEASLOG_ADD_NEXT_INDEX_STRINGL((void *)&_buffer_data_,log_info,log_info_len);
        }
        else
        {
            array_init(&new_array);
            SEASLOG_ADD_NEXT_INDEX_STRINGL(&new_array, log_info, log_info_len);
            SEASLOG_ADD_ASSOC_ZVAL_EX(&SEASLOG_G(buffer),path,path_len,&new_array);
        }
    }

#else
    zval *new_array;
    zval **_buffer_data_;

    if (IS_ARRAY != Z_TYPE_P(SEASLOG_G(buffer)))
    {
        return 0;
    }

    if (zend_hash_num_elements(Z_ARRVAL_P(SEASLOG_G(buffer))) < 1)
    {
        MAKE_STD_ZVAL(new_array);
        array_init(new_array);
        SEASLOG_ADD_NEXT_INDEX_STRINGL(new_array, log_info, log_info_len);
        SEASLOG_ADD_ASSOC_ZVAL_EX(SEASLOG_G(buffer),path,path_len,new_array);
    }
    else
    {

        if (zend_hash_find(HASH_OF(SEASLOG_G(buffer)), path, path_len, (void**)&_buffer_data_) == SUCCESS)
        {
            convert_to_array_ex(_buffer_data_);
            SEASLOG_ADD_NEXT_INDEX_STRINGL(*_buffer_data_, log_info, log_info_len);
        }
        else
        {
            MAKE_STD_ZVAL(new_array);
            array_init(new_array);
            SEASLOG_ADD_NEXT_INDEX_STRINGL(new_array, log_info, log_info_len);
            SEASLOG_ADD_ASSOC_ZVAL_EX(SEASLOG_G(buffer), path, path_len, new_array);
        }
    }

#endif

    if (SEASLOG_G(buffer_size) > 0)
    {
        SEASLOG_G(buffer_count)++;

        if (SEASLOG_G(buffer_count) >= SEASLOG_G(buffer_size))
        {
            seaslog_shutdown_buffer(SEASLOG_BUFFER_RE_INIT_YES TSRMLS_CC);
        }
    }

    return SUCCESS;
}

void seaslog_shutdown_buffer(int re_init TSRMLS_DC)
{
    HashTable   *ht;

#if PHP_VERSION_ID >= 70000
    zend_ulong num_key;
    zend_string *str_key;
    zval *entry;
#else
    zval **ppzval;
#endif

    if (seaslog_check_buffer_enable(TSRMLS_C))
    {
        if (SEASLOG_G(buffer_count) < 1)
        {
            return;
        }

#if PHP_VERSION_ID >= 70000

        ht = Z_ARRVAL(SEASLOG_G(buffer));
        ZEND_HASH_FOREACH_KEY_VAL(ht, num_key, str_key, entry)
        {
            real_php_log_buffer(entry, ZSTR_VAL(str_key), ZSTR_LEN(str_key) TSRMLS_CC);
        }
        ZEND_HASH_FOREACH_END();

#else
        ht = Z_ARRVAL_P(SEASLOG_G(buffer));

        zend_hash_internal_pointer_reset(ht);
        while (zend_hash_get_current_data(ht, (void **)&ppzval) == SUCCESS)
        {
            char *key = NULL;
            ulong idx = 0;

            zend_hash_get_current_key(ht, &key, &idx, 0);
            convert_to_array_ex(ppzval);
            real_php_log_buffer(*ppzval, key, strlen(key) TSRMLS_CC);
            zend_hash_move_forward(ht);
        }
#endif

        if (re_init == SEASLOG_BUFFER_RE_INIT_YES)
        {
            seaslog_clear_buffer(TSRMLS_C);
            seaslog_init_buffer(TSRMLS_C);
        }
    }
}


void seaslog_clear_buffer(TSRMLS_D)
{
    if (seaslog_check_buffer_enable(TSRMLS_C))
    {
        SEASLOG_G(buffer_count) = 0;

#if PHP_VERSION_ID >= 70000

        if (IS_ARRAY == Z_TYPE(SEASLOG_G(buffer)))
        {
            EX_ARRAY_DESTROY(&SEASLOG_G(buffer));
        }

#else

        if (SEASLOG_G(buffer) && IS_ARRAY == Z_TYPE_P(SEASLOG_G(buffer)))
        {
            EX_ARRAY_DESTROY(&(SEASLOG_G(buffer)));
        }

#endif

    }
}

