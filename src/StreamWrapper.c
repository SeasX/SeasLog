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

#include "StreamWrapper.h"
#include "ErrorHook.h"
#include "ExceptionHook.h"

php_stream *seaslog_stream_open_wrapper(char *opt TSRMLS_DC)
{
    php_stream *stream = NULL;
    char *res = NULL;
    int first_create_file = 0;

#if PHP_VERSION_ID >= 70000
    zend_long reslen;
#else
    long reslen;
#endif

    switch SEASLOG_G(appender)
    {
    case SEASLOG_APPENDER_TCP:
        reslen = spprintf(&res, 0, "tcp://%s:%d", SEASLOG_G(remote_host), SEASLOG_G(remote_port));
        stream = php_stream_xport_create(res, reslen, REPORT_ERRORS, STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT, 0, &SEASLOG_G(remote_timeout_real), NULL, NULL, NULL);

        if (stream == NULL)
        {
            seaslog_throw_exception(SEASLOG_EXCEPTION_LOGGER_ERROR TSRMLS_CC, "SeasLog Can Not Create TCP Connect - %s", res);
            efree(res);
            return NULL;
        }

        efree(res);
        break;

    case SEASLOG_APPENDER_UDP:
        reslen = spprintf(&res, 0, "udp://%s:%d", SEASLOG_G(remote_host), SEASLOG_G(remote_port));
        stream = php_stream_xport_create(res, reslen, REPORT_ERRORS, STREAM_XPORT_CLIENT | STREAM_XPORT_CONNECT, 0, &SEASLOG_G(remote_timeout_real), NULL, NULL, NULL);

        if (stream == NULL)
        {
            seaslog_throw_exception(SEASLOG_EXCEPTION_LOGGER_ERROR TSRMLS_CC, "SeasLog Can Not Create UDP Connect - %s", res);
            efree(res);
            return NULL;
        }

        efree(res);
        break;
    case SEASLOG_APPENDER_FILE:
    default:
        if (access(opt,F_OK) != 0)
        {
            first_create_file = 1;
        }

        stream = php_stream_open_wrapper(opt, "a", IGNORE_URL_WIN, NULL);

        if (stream == NULL)
        {
            seaslog_throw_exception(SEASLOG_EXCEPTION_LOGGER_ERROR TSRMLS_CC, "SeasLog Invalid Log File - %s", opt);
            return NULL;
        }
        else
        {
            if (first_create_file == 1)
            {
                VCWD_CHMOD(opt, SEASLOG_FILE_MODE);
            }
        }
    }

    return stream;
}

void seaslog_init_stream_list(TSRMLS_D)
{
    zval *z_stream_list;

#if PHP_VERSION_ID >= 70000
    array_init(&SEASLOG_G(stream_list));
#else

    MAKE_STD_ZVAL(z_stream_list);
    array_init(z_stream_list);

    SEASLOG_G(stream_list) = z_stream_list;
#endif

}

int seaslog_clear_stream(int destroy, int model, char *opt TSRMLS_DC)
{
    php_stream *stream = NULL;
    HashTable *ht;
    stream_entry_t *stream_entry;
    int result = FAILURE;

    zend_ulong num_key;

#if PHP_VERSION_ID >= 70000
    if (IS_ARRAY == Z_TYPE(SEASLOG_G(stream_list)))
    {
        ht = Z_ARRVAL(SEASLOG_G(stream_list));
        ZEND_HASH_FOREACH_NUM_KEY_PTR(ht, num_key, stream_entry)
        {
            if (SEASLOG_CLOSE_LOGGER_STREAM_MOD_ALL == model
                    || (SEASLOG_CLOSE_LOGGER_STREAM_MOD_ASSIGN == model && strstr(stream_entry->opt, opt))
               )
            {
                stream = stream_entry->stream;
                if (stream)
                {
                    php_stream_close(stream);
                    zend_hash_index_del(ht, num_key);
                }
                efree(stream_entry->opt);
                efree(stream_entry);
                result = SUCCESS;
            }
        }
        ZEND_HASH_FOREACH_END();

        if (SEASLOG_STREAM_LIST_DESTROY_YES == destroy)
        {
            SEASLOG_ARRAY_DESTROY(SEASLOG_G(stream_list));
        }
    }
#else
    if (SEASLOG_G(stream_list) && IS_ARRAY == Z_TYPE_P(SEASLOG_G(stream_list)))
    {
        ht = HASH_OF(SEASLOG_G(stream_list));

        zend_hash_internal_pointer_reset(ht);
        while (zend_hash_get_current_data(ht, (void **)&stream_entry) == SUCCESS)
        {
            if (SEASLOG_CLOSE_LOGGER_STREAM_MOD_ALL == model
                    || (SEASLOG_CLOSE_LOGGER_STREAM_MOD_ASSIGN == model && strstr(stream_entry->opt, opt))
               )
            {
                zend_hash_get_current_key_ex(ht, NULL, NULL, &num_key, 1, NULL);
                stream = stream_entry->stream;
                if (stream && stream_entry->can_delete == SEASLOG_CLOSE_LOGGER_STREAM_CAN_DELETE)
                {
                    php_stream_close(stream);
                    efree(stream_entry->opt);
                    zend_hash_index_del(ht, num_key);
                }
                else
                {
                    zend_hash_move_forward(ht);
                }

                result = SUCCESS;
            }
            else
            {
                zend_hash_move_forward(ht);
            }
        }

        if (SEASLOG_STREAM_LIST_DESTROY_YES == destroy)
        {
            SEASLOG_ARRAY_DESTROY(SEASLOG_G(stream_list));
        }
    }
#endif

    return result;
}

php_stream *process_stream(char *opt, int opt_len TSRMLS_DC)
{
    zend_ulong stream_entry_hash;
    php_stream *stream = NULL;
    HashTable *ht_list;
    php_stream_statbuf dest_s;
    stream_entry_t *stream_entry;

    switch SEASLOG_G(appender)
    {
    case SEASLOG_APPENDER_TCP:
        stream_entry_hash = zend_inline_hash_func("tcp", 4);
        break;
    case SEASLOG_APPENDER_UDP:
        stream_entry_hash = zend_inline_hash_func("udp", 4);
        break;
    case SEASLOG_APPENDER_FILE:
    default:
        stream_entry_hash = zend_inline_hash_func(opt, opt_len);
    }

#if PHP_VERSION_ID >= 70000
    ht_list = Z_ARRVAL(SEASLOG_G(stream_list));

    if ((stream_entry = zend_hash_index_find_ptr(ht_list, stream_entry_hash)) != NULL)
    {
        stream = stream_entry->stream;
        if (stream && stream_entry->can_delete == SEASLOG_CLOSE_LOGGER_STREAM_CAN_DELETE)
        {
            switch SEASLOG_G(appender)
            {
            case SEASLOG_APPENDER_TCP:
            case SEASLOG_APPENDER_UDP:
                if (php_stream_eof(stream))
                {
                    goto create_stream;
                }
                break;
            case SEASLOG_APPENDER_FILE:
            default:
                if (php_stream_stat_path_ex(opt, PHP_STREAM_URL_STAT_QUIET | PHP_STREAM_URL_STAT_NOCACHE, &dest_s, NULL) < 0)
                {
                    goto create_stream;
                }
            }

            return stream;
        }

        return NULL;
#else
    ht_list = HASH_OF(SEASLOG_G(stream_list));

    if (zend_hash_index_find(ht_list, stream_entry_hash, (void **)&stream_entry) == SUCCESS)
    {
        stream = stream_entry->stream;
        if (stream && stream_entry->can_delete == SEASLOG_CLOSE_LOGGER_STREAM_CAN_DELETE)
        {
            switch SEASLOG_G(appender)
            {
            case SEASLOG_APPENDER_TCP:
            case SEASLOG_APPENDER_UDP:
                if (php_stream_eof(stream))
                {
                    goto create_stream;
                }
                break;
            case SEASLOG_APPENDER_FILE:
            default:
                if (php_stream_stat_path_ex(opt, PHP_STREAM_URL_STAT_QUIET | PHP_STREAM_URL_STAT_NOCACHE, &dest_s, NULL) < 0)
                {
                    goto create_stream;
                }
            }

            return stream;
        }

        return NULL;
#endif
    }
    else
    {
create_stream:
        stream = seaslog_stream_open_wrapper(opt TSRMLS_CC);
        if (stream == NULL)
        {
            return NULL;
        }
        else
        {

#if PHP_VERSION_ID >= 70000
            stream_entry = ecalloc(1,sizeof(stream_entry_t));
#else
            stream_entry = pecalloc(1,sizeof(stream_entry_t),1);
#endif

            stream_entry->opt_len = spprintf(&stream_entry->opt, 0, "%s",opt);
            stream_entry->stream_entry_hash = stream_entry_hash;
            stream_entry->stream = stream;
            stream_entry->can_delete = SEASLOG_CLOSE_LOGGER_STREAM_CAN_DELETE;

            SEASLOG_ZEND_HASH_INDEX_UPDATE(ht_list, stream_entry_hash, stream_entry, sizeof(stream_entry_t), NULL);

            return stream;
        }
    }

    return stream;
}

