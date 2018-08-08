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

#include "Logger.h"
#include "Datetime.h"
#include "Appender.h"

void seaslog_init_slash_or_underline(TSRMLS_D)
{
    if (SEASLOG_G(disting_folder))
    {
        SEASLOG_G(slash_or_underline) = SEASLOG_LOGGER_SLASH;
    }
    else
    {
        SEASLOG_G(slash_or_underline) = SEASLOG_LOGGER_UNDERLINE;
    }
}

void seaslog_init_last_time(TSRMLS_D)
{
    int now;

    SEASLOG_G(current_datetime_format)      = estrdup(SEASLOG_G(default_datetime_format));
    SEASLOG_G(current_datetime_format_len)  = strlen(SEASLOG_G(current_datetime_format));

    now = (int)time(NULL);
    seaslog_process_last_sec(now, SEASLOG_INIT_FIRST_YES TSRMLS_CC);
    seaslog_process_last_min(now, SEASLOG_INIT_FIRST_YES TSRMLS_CC);
}

void seaslog_clear_last_time(TSRMLS_D)
{
    if (SEASLOG_G(last_sec))
    {
        efree(SEASLOG_G(last_sec)->real_time);
        efree(SEASLOG_G(last_sec));
    }

    if (SEASLOG_G(last_min))
    {
        efree(SEASLOG_G(last_min)->real_time);
        efree(SEASLOG_G(last_min));
    }

    if (SEASLOG_G(current_datetime_format))
    {
        efree(SEASLOG_G(current_datetime_format));
    }
}

void seaslog_init_logger(TSRMLS_D)
{
    SEASLOG_G(base_path) = estrdup(SEASLOG_G(default_basepath));

    SEASLOG_G(last_logger) = ecalloc(1,sizeof(logger_entry_t));
    SEASLOG_G(tmp_logger) = ecalloc(1,sizeof(logger_entry_t));

    seaslog_init_default_last_logger(TSRMLS_C);
}


void seaslog_init_default_last_logger(TSRMLS_D)
{
    if (SEASLOG_G(last_logger)->logger)
    {
        // do nothing
    }
    else
    {
        SEASLOG_G(last_logger)->logger_len = spprintf(&SEASLOG_G(last_logger)->logger, 0, "%s",SEASLOG_G(default_logger));
    }

    if (SEASLOG_G(last_logger)->logger_path)
    {
        efree(SEASLOG_G(last_logger)->logger_path);
    }

    SEASLOG_G(last_logger)->logger_path_len = spprintf(&SEASLOG_G(last_logger)->logger_path, 0, "%s/%s", SEASLOG_G(base_path), SEASLOG_G(last_logger)->logger);

    if (SEASLOG_G(disting_folder))
    {
        if (make_log_dir(SEASLOG_G(last_logger)->logger_path TSRMLS_CC) == SUCCESS)
        {
            SEASLOG_G(last_logger)->access = SUCCESS;
        }
        else
        {
            SEASLOG_G(last_logger)->access = FAILURE;
        }
    }
    else
    {
        if (make_log_dir(SEASLOG_G(base_path) TSRMLS_CC) == SUCCESS)
        {
            SEASLOG_G(last_logger)->access = SUCCESS;
        }
        else
        {
            SEASLOG_G(last_logger)->access = FAILURE;
        }
    }
}


void seaslog_clear_logger(TSRMLS_D)
{
    if (SEASLOG_G(base_path))
    {
        efree(SEASLOG_G(base_path));
    }

    if (SEASLOG_G(last_logger))
    {
        if (SEASLOG_G(last_logger)->logger)
        {
            efree(SEASLOG_G(last_logger)->logger);
        }

        if (SEASLOG_G(last_logger)->logger_path)
        {
            efree(SEASLOG_G(last_logger)->logger_path);
        }

        efree(SEASLOG_G(last_logger));
    }

    if (SEASLOG_G(tmp_logger))
    {
        if (SEASLOG_G(tmp_logger)->logger)
        {
            efree(SEASLOG_G(tmp_logger)->logger);
        }

        if (SEASLOG_G(tmp_logger)->logger_path)
        {
            efree(SEASLOG_G(tmp_logger)->logger_path);
        }

        efree(SEASLOG_G(tmp_logger));
    }
}

void seaslog_init_logger_list(TSRMLS_D)
{
    zval *z_logger_list;

#if PHP_VERSION_ID >= 70000
    array_init(&SEASLOG_G(logger_list));
#else
    SEASLOG_G(logger_list) = NULL;

    MAKE_STD_ZVAL(z_logger_list);
    array_init(z_logger_list);

    SEASLOG_G(logger_list) = z_logger_list;
#endif
}

void seaslog_clear_logger_list(TSRMLS_D)
{
#if PHP_VERSION_ID >= 70000
    if (IS_ARRAY == Z_TYPE(SEASLOG_G(logger_list)))
    {
        EX_ARRAY_DESTROY(&SEASLOG_G(logger_list));
    }
#else
    if (SEASLOG_G(logger_list) && IS_ARRAY == Z_TYPE_P(SEASLOG_G(logger_list)))
    {
        EX_ARRAY_DESTROY(&(SEASLOG_G(logger_list)));
    }
#endif
}

logger_entry_t *process_logger(char *logger, int logger_len, int last_or_tmp TSRMLS_DC)
{
    ulong logger_entry_hash = zend_inline_hash_func(logger, logger_len);
    logger_entry_t *logger_entry;
    HashTable *ht_list;
    HashTable *ht_logger;
    char *folder;
    int folder_len;
    char folder_tmp[1024];

#if PHP_VERSION_ID >= 70000
    zval logger_array;
    zval *logger_find;
    zval *logger_tmp_find,*logger_path_tmp_find,*logger_access_find;
#else
    zval *logger_array;
    zval **logger_find;
    zval **logger_tmp_find,**logger_path_tmp_find,**logger_access_find;
#endif

    if (last_or_tmp == SEASLOG_PROCESS_LOGGER_LAST)
    {
        logger_entry = SEASLOG_G(last_logger);
    }
    else
    {
        logger_entry = SEASLOG_G(tmp_logger);
    }

    if (logger_entry_hash == logger_entry->logger_hash)
    {
        return logger_entry;
    }

    if (logger_entry->logger)
    {
        efree(logger_entry->logger);
    }

    if (logger_entry->logger_path)
    {
        efree(logger_entry->logger_path);
    }

    logger_entry->logger_hash = logger_entry_hash;

#if PHP_VERSION_ID >= 70000
    ht_list = Z_ARRVAL(SEASLOG_G(logger_list));

    if ((logger_find = zend_hash_index_find(ht_list, logger_entry_hash)) != NULL)
    {
        ht_logger = Z_ARRVAL_P(logger_find);
        logger_tmp_find = zend_hash_index_find(ht_logger, SEASLOG_HASH_VALUE_LOGGER);
        logger_path_tmp_find = zend_hash_index_find(ht_logger, SEASLOG_HASH_VALUE_PATH);
        logger_access_find = zend_hash_index_find(ht_logger, SEASLOG_HASH_VALUE_ACCESS);

        logger_entry->logger_len = spprintf(&logger_entry->logger, 0, "%s",Z_STRVAL_P(logger_tmp_find));
        logger_entry->logger_path_len = spprintf(&logger_entry->logger_path, 0, "%s",Z_STRVAL_P(logger_path_tmp_find));
        logger_entry->access = Z_LVAL_P(logger_access_find);
#else
    ht_list = HASH_OF(SEASLOG_G(logger_list));

    if (zend_hash_index_find(ht_list, logger_entry_hash, (void **)&logger_find) == SUCCESS)
    {
        ht_logger = HASH_OF(*logger_find);

        zend_hash_index_find(ht_logger, SEASLOG_HASH_VALUE_LOGGER, (void **)&logger_tmp_find);
        zend_hash_index_find(ht_logger, SEASLOG_HASH_VALUE_PATH, (void **)&logger_path_tmp_find);
        zend_hash_index_find(ht_logger, SEASLOG_HASH_VALUE_ACCESS, (void **)&logger_access_find);

        logger_entry->logger_len = spprintf(&logger_entry->logger, 0, "%s",Z_STRVAL_PP(logger_tmp_find));
        logger_entry->logger_path_len = spprintf(&logger_entry->logger_path, 0, "%s",Z_STRVAL_PP(logger_path_tmp_find));
        logger_entry->access = Z_LVAL_PP(logger_access_find);

#endif
    }
    else
    {
        logger_entry->logger_len = spprintf(&logger_entry->logger, 0, "%s", logger);
        logger_entry->logger_path_len = spprintf(&logger_entry->logger_path, 0, "%s/%s", SEASLOG_G(base_path), logger_entry->logger);
        logger_entry->access = SUCCESS;

        if (SEASLOG_G(disting_folder))
        {
            if (make_log_dir(logger_entry->logger_path TSRMLS_CC) == SUCCESS)
            {
                logger_entry->access = SUCCESS;
            }
            else
            {
                logger_entry->access = FAILURE;
            }
        }
        else
        {
            folder = strrchr(logger_entry->logger_path,'/');
            if (folder != NULL)
            {
                folder_len = logger_entry->logger_path_len - strlen(folder);
                strncpy(folder_tmp, logger_entry->logger_path, folder_len);
                folder_tmp[folder_len] = '\0';
                logger_entry->folder = folder_tmp;

                if (make_log_dir(logger_entry->folder TSRMLS_CC) == SUCCESS)
                {
                    logger_entry->access = SUCCESS;
                }
                else
                {
                    logger_entry->access = FAILURE;
                }
            }
        }

#if PHP_VERSION_ID >= 70000
        array_init(&logger_array);
#else
        MAKE_STD_ZVAL(logger_array);
        array_init(logger_array);
#endif

        SEASLOG_ADD_INDEX_STRINGL(logger_array,SEASLOG_HASH_VALUE_LOGGER,logger_entry->logger,logger_entry->logger_len);
        SEASLOG_ADD_INDEX_STRINGL(logger_array,SEASLOG_HASH_VALUE_PATH,logger_entry->logger_path,logger_entry->logger_path_len);
        SEASLOG_ADD_INDEX_LONG(logger_array,SEASLOG_HASH_VALUE_ACCESS,logger_entry->access);

        SEASLOG_ADD_INDEX_ZVAL(SEASLOG_G(logger_list),logger_entry_hash,logger_array);
    }

    return logger_entry;
}

