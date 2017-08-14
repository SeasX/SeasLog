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

static void seaslog_init_host_name(TSRMLS_D)
{
    if (SEASLOG_G(appender) == SEASLOG_APPENDER_TCP || SEASLOG_G(appender) == SEASLOG_APPENDER_UDP)
    {
        char buf[255];

        if (gethostname(buf, sizeof(buf) - 1))
        {
            SEASLOG_G(host_name) = "";
        }
        else
        {
            spprintf(&SEASLOG_G(host_name), 0, "%s", buf);
        }
    }
}

static void seaslog_clear_host_name(TSRMLS_D)
{
    if (SEASLOG_G(host_name))
    {
        efree(SEASLOG_G(host_name));
    }
}

static void seaslog_init_request_id(TSRMLS_D)
{
    SEASLOG_G(request_id) = get_uniqid();
}

static void seaslog_clear_request_id(TSRMLS_D)
{
    if (SEASLOG_G(request_id))
    {
        efree(SEASLOG_G(request_id));
    }
}