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

static long get_type_count(char *log_path, char *level, char *key_word TSRMLS_DC)
{
    FILE * fp;
    char buffer[BUFSIZ];
    char *path, *sh;
    long count;

    if (SEASLOG_G(last_logger)->access == FAILURE)
    {
        return (long)0;
    }
    if (SEASLOG_G(disting_type))
    {
        spprintf(&path, 0, "%s/%s.%s*", SEASLOG_G(last_logger)->logger_path, level, log_path);
    }
    else
    {
        spprintf(&path, 0, "%s/%s*",    SEASLOG_G(last_logger)->logger_path, log_path);
    }

#ifdef WINDOWS
    path = str_replace(path, "/", "\\");
#endif

    if (key_word)
    {
#ifdef WINDOWS
        spprintf(&sh, 0, "findstr \"%s\" %s | find /c \"%s\" ", level, path, key_word);
#else
        spprintf(&sh, 0, "cat %s 2>/dev/null| grep '%s' | grep '%s' -c", path, level, key_word);
#endif
    }
    else
    {
#ifdef WINDOWS
        spprintf(&sh, 0, "findstr \"%s\" %s | find /c /v \"\" ", level, path);
#else
        spprintf(&sh, 0, "cat %s 2>/dev/null| grep '%s' -c", path, level);
#endif
    }

    fp = VCWD_POPEN(sh, "r");
    if (! fp)
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

static int get_detail(char *log_path, char *level, char *key_word, long start, long end, long order, zval *return_value TSRMLS_DC)
{
    FILE * fp;
    char buffer[SEASLOG_BUFFER_MAX_SIZE + 1];
    char *path;
    char *sh;
    char *command;

    memset(buffer, '\0', sizeof(buffer));

    array_init(return_value);

    if (SEASLOG_G(disting_type))
    {

        if (!strcmp(level, "|"))
        {
            spprintf(&path, 0, "%s/%s.%s*", SEASLOG_G(last_logger)->logger_path, "*", log_path);
        }
        else
        {
            spprintf(&path, 0, "%s/%s.%s*", SEASLOG_G(last_logger)->logger_path, level, log_path);
        }
    }
    else
    {
        spprintf(&path, 0, "%s/%s*", SEASLOG_G(last_logger)->logger_path, log_path);
    }

#ifdef WINDOWS
    path = str_replace(path, "/", "\\");
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
        spprintf(&sh, 0, "findstr \"%s\" %s | findstr \"%s\" ", level, path, key_word);
#else
        spprintf(&sh, 0, "%s 2>/dev/null| grep '%s' | grep '%s' | sed -n '%ld,%ld'p", command, level, key_word, start, end);
#endif
    }
    else
    {
#ifdef WINDOWS
        spprintf(&sh, 0, "findstr \"%s\" %s", level, path);
#else
        spprintf(&sh, 0, "%s 2>/dev/null| grep '%s' | sed -n '%ld,%ld'p", command, level, start, end);
#endif
    }

    fp = VCWD_POPEN(sh, "r");

    if (!fp)
    {
        seaslog_throw_exception(SEASLOG_EXCEPTION_CONTENT_ERROR TSRMLS_CC, "Unable to fork [%s]", sh);

        efree(buffer);
        return -1;
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

    return 1;
}