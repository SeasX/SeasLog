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

static void seaslog_process_last_sec(int now TSRMLS_DC)
{
    last_sec_entry_t *last_sec = ecalloc(sizeof(last_sec_entry_t), 1);
    last_sec->sec = now;
    last_sec->real_time = seaslog_format_date(SEASLOG_G(current_datetime_format), SEASLOG_G(current_datetime_format_len), now TSRMLS_CC);
    SEASLOG_G(last_sec) = last_sec;
}

static void seaslog_process_last_min(int now TSRMLS_DC)
{
    SEASLOG_G(last_min) = ecalloc(sizeof(last_min_entry_t), 1);
    SEASLOG_G(last_min)->sec = now;

    if (SEASLOG_G(disting_by_hour))
    {
        SEASLOG_G(last_min)->real_time = seaslog_format_date("YmdH", 4, now TSRMLS_CC);
    }
    else
    {
        SEASLOG_G(last_min)->real_time = seaslog_format_date("Ymd",  3, now TSRMLS_CC);
    }
}

static char *seaslog_format_date(char *format, int format_len, time_t ts TSRMLS_DC)
{
#if PHP_VERSION_ID >= 70000
    zend_string *_date;
    char *_date_tmp;
    _date = php_format_date(format, format_len, ts, 1 TSRMLS_CC);
    _date_tmp = estrdup(ZSTR_VAL(_date));
    zend_string_release(_date);
    return _date_tmp;
#else
    return php_format_date(format, format_len, ts, 1 TSRMLS_CC);
#endif
}

static char *make_real_date(TSRMLS_D)
{
    char *_date = NULL;

    int now = (long)time(NULL);
    if (now > SEASLOG_G(last_min)->sec + 60)
    {
        efree(SEASLOG_G(last_min)->real_time);
        efree(SEASLOG_G(last_min));

        seaslog_process_last_min(now TSRMLS_CC);
    }

    _date = estrdup(SEASLOG_G(last_min)->real_time);

    return _date;
}

static char *make_real_time(TSRMLS_D)
{
    char *real_time = NULL;

    int now = (long)time(NULL);
    if (now > SEASLOG_G(last_sec)->sec)
    {
        efree(SEASLOG_G(last_sec)->real_time);
        efree(SEASLOG_G(last_sec));

        seaslog_process_last_sec(now TSRMLS_CC);
    }

    real_time = estrdup(SEASLOG_G(last_sec)->real_time);

    return real_time;
}

static char *mic_time()
{
    struct timeval now;
    char *tstr;

    timerclear(&now);
    gettimeofday(&now, NULL);

    spprintf(&tstr, 0, "%ld.%ld", (long)time(NULL), (long)now.tv_usec / 1000);

    return tstr;
}
