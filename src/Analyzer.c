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

#include "Analyzer.h"
#include "Common.h"
#include "ExceptionHook.h"

long get_type_count(char *log_path, char *level, char *key_word TSRMLS_DC)
{
    FILE * fp;
    char buffer[BUFSIZ];
    char *path, *sh;
    long count;
    int is_level_all = 0;

    if (SEASLOG_G(last_logger)->access == FAILURE)
    {
        return (long)0;
    }

    if (SEASLOG_G(disting_type))
    {
        if (!strcmp(level, SEASLOG_ALL))
        {
            is_level_all = 1;
        }

        if (is_level_all == 1)
        {
            spprintf(&path, 0, "%s%s%s*.*", SEASLOG_G(last_logger)->logger_path, SEASLOG_G(slash_or_underline), log_path);
        }
        else
        {
            spprintf(&path, 0, "%s%s%s*.%s*", SEASLOG_G(last_logger)->logger_path, SEASLOG_G(slash_or_underline), log_path, level);
        }
    }
    else
    {
        spprintf(&path, 0, "%s%s%s*",    SEASLOG_G(last_logger)->logger_path, SEASLOG_G(slash_or_underline), log_path);
    }

#ifdef WINDOWS
    path = str_replace(path, "/", "\\");
#endif

    if (key_word)
    {
#ifdef WINDOWS
        spprintf(&sh, 0, "findstr \"%s\" %s | find /c \"%s\" ", level, path, key_word);
#else
        if (is_level_all == 1)
        {
            spprintf(&sh, 0, "cat %s 2>/dev/null| grep '%s' -aic", path, key_word);
        }
        else
        {
            spprintf(&sh, 0, "cat %s 2>/dev/null| grep -ai '%s' | grep '%s' -aic", path, level, key_word);
        }
#endif
    }
    else
    {
#ifdef WINDOWS
        spprintf(&sh, 0, "findstr \"%s\" %s | find /c /v \"\" ", level, path);
#else
        if (is_level_all == 1)
        {
            spprintf(&sh, 0, "cat %s 2>/dev/null| wc -l", path);
        }
        else
        {
            spprintf(&sh, 0, "cat %s 2>/dev/null| grep '%s' -aic", path, level);
        }
#endif
    }

    fp = VCWD_POPEN(sh, "r");

    if (!fp)
    {
        seaslog_throw_exception(SEASLOG_EXCEPTION_CONTENT_ERROR TSRMLS_CC, "Unable to fork [%s]", sh);
        return -1;
    }
    else
    {
        char *temp_p = fgets(buffer, sizeof(buffer), fp);
        pclose(fp);
    }

    count = atoi(delN(buffer));
    efree(path);
    efree(sh);

    return count;
}

int get_detail(char *log_path, char *level, char *key_word, long start, long end, long order, zval *return_value TSRMLS_DC)
{
    FILE * fp;
    char buffer[SEASLOG_BUFFER_MAX_SIZE];
    char *path;
    char *sh;
    char *command;
    int is_level_all = 0;

    memset(buffer, '\0', sizeof(buffer));

    array_init(return_value);

    if (start < 0)
    {
        start = SEASLOG_ANALYZER_DEFAULT_START;
    }

    if (end < 0)
    {
        end = SEASLOG_ANALYZER_DEFAULT_OFFSET;
    }

    if (!strcmp(level, SEASLOG_ALL))
    {
        is_level_all = 1;
    }

    if (SEASLOG_G(disting_type))
    {
        if (is_level_all == 1)
        {
            spprintf(&path, 0, "%s%s%s*.*", SEASLOG_G(last_logger)->logger_path, SEASLOG_G(slash_or_underline), log_path);
        }
        else
        {
            spprintf(&path, 0, "%s%s%s*.%s*", SEASLOG_G(last_logger)->logger_path, SEASLOG_G(slash_or_underline), log_path, level);
        }
    }
    else
    {
        spprintf(&path, 0, "%s%s%s*", SEASLOG_G(last_logger)->logger_path, SEASLOG_G(slash_or_underline), log_path);
    }

#ifdef WINDOWS
    path = str_replace(path, "/", "\\");

    spprintf(&command, 0, "%s", "findstr");
#else
    if (order == SEASLOG_DETAIL_ORDER_DESC)
    {
        spprintf(&command, 0, "%s `ls -t %s 2>/dev/null;if [ $? -ne 0 ] ;then echo 'NOLOGGER';fi`", "tac", path);
    }
    else
    {
        spprintf(&command, 0, "%s %s", "cat", path);
    }
#endif

    if (key_word && strlen(key_word) >= 1)
    {
#ifdef WINDOWS
        spprintf(&sh, 0, "%s \"%s\" %s | %s \"%s\" ", command, level, path, command, key_word);
#else
        if (is_level_all == 1)
        {
            spprintf(&sh, 0, "%s 2>/dev/null| grep -ai '%s' | sed -n '%ld,%ld'p", command, key_word, start, end);
        }
        else
        {
            spprintf(&sh, 0, "%s 2>/dev/null| grep -ai '%s' | grep -ai '%s' | sed -n '%ld,%ld'p", command, level, key_word, start, end);
        }
#endif
    }
    else
    {
#ifdef WINDOWS
        spprintf(&sh, 0, "%s \"%s\" %s", command, level, path);
#else
        if (is_level_all == 1)
        {
            spprintf(&sh, 0, "%s 2>/dev/null| sed -n '%ld,%ld'p", command, start, end);
        }
        else
        {
            spprintf(&sh, 0, "%s 2>/dev/null| grep -ai '%s' | sed -n '%ld,%ld'p", command, level, start, end);
        }
#endif
    }

    fp = VCWD_POPEN(sh, "r");

    if (!fp)
    {
        seaslog_throw_exception(SEASLOG_EXCEPTION_CONTENT_ERROR TSRMLS_CC, "Unable to fork [%s]", sh);

        return FAILURE;
    }
    else
    {
        while ((fgets(buffer, sizeof(buffer), fp)) != NULL)
        {
            if (strstr(buffer, SEASLOG_G(base_path)) == NULL)
            {
                SEASLOG_ADD_NEXT_INDEX_STRING(return_value, delN(buffer));
            }
        }

        pclose(fp);
    }

    efree(path);
    efree(command);
    efree(sh);

    return SUCCESS;
}

