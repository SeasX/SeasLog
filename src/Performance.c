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

#include "Performance.h"

void seaslog_memory_usage(smart_str *buf TSRMLS_DC)
{
    long int usage = zend_memory_usage(0 TSRMLS_CC);
    smart_str_append_long(buf, usage);

    smart_str_0(buf);
}

void seaslog_peak_memory_usage(smart_str *buf TSRMLS_DC)
{
    long int usage = zend_memory_peak_usage(0 TSRMLS_CC);
    smart_str_append_long(buf, usage);

    smart_str_0(buf);
}

void initZendHooks(TSRMLS_D)
{
    if (SEASLOG_G(trace_stack))
    {
#if PHP_VERSION_ID < 50500
        _clone_zend_execute = zend_execute;
        zend_execute = seaslog_execute;
#else
        _clone_zend_execute_ex = zend_execute_ex;
        zend_execute_ex = seaslog_execute_ex;
#endif

        _clone_zend_execute_internal = zend_execute_internal;
        zend_execute_internal = seaslog_execute_internal;
    }
}

void recoveryZendHooks(TSRMLS_D)
{
    if (SEASLOG_G(trace_stack))
    {
#if PHP_VERSION_ID < 50500
        zend_execute = _clone_zend_execute;
#else
        zend_execute_ex = _clone_zend_execute_ex;
#endif

        zend_execute_internal = _clone_zend_execute_internal;
    }
}

#if PHP_VERSION_ID >= 50500
ZEND_DLEXPORT void seaslog_execute_ex (zend_execute_data *execute_data TSRMLS_DC)
#else
ZEND_DLEXPORT void seaslog_execute (zend_op_array *ops TSRMLS_DC)
#endif
{
    int is_tracing = FAILURE;

#if PHP_VERSION_ID < 50500
    zend_execute_data  *execute_data = EG(current_execute_data);
#endif

    zend_execute_data *real_execute_data = execute_data;
    is_tracing = performance_frame_begin(real_execute_data TSRMLS_CC);

#if PHP_VERSION_ID >= 50500
    _clone_zend_execute_ex(execute_data TSRMLS_CC);
#else
    _clone_zend_execute(ops TSRMLS_CC);
#endif

    if (SUCCESS == is_tracing)
    {
        performance_frame_end(TSRMLS_C);
    }
}


#if PHP_VERSION_ID >= 70000
ZEND_DLEXPORT void seaslog_execute_internal(zend_execute_data *execute_data, zval *ret)
#elif PHP_VERSION_ID >= 50500
ZEND_DLEXPORT void seaslog_execute_internal(zend_execute_data *execute_data, struct _zend_fcall_info *zfi, int ret TSRMLS_DC)
#else
ZEND_DLEXPORT void seaslog_execute_internal(zend_execute_data *execute_data, int ret TSRMLS_DC)
#endif
{

    int is_tracing = FAILURE;

    zend_execute_data *real_execute_data = execute_data;
    is_tracing = performance_frame_begin(real_execute_data TSRMLS_CC);


#if PHP_VERSION_ID >= 70000
    if (_clone_zend_execute_internal)
    {
        _clone_zend_execute_internal(execute_data, ret TSRMLS_CC);
    }
    else
    {
        execute_internal(execute_data, ret TSRMLS_CC);
    }
#elif PHP_VERSION_ID >= 50500
    if (_clone_zend_execute_internal)
    {
        _clone_zend_execute_internal(execute_data, zfi, ret TSRMLS_CC);
    }
    else
    {
        execute_internal(execute_data, zfi, ret TSRMLS_CC);
    }
#else
    if (_clone_zend_execute_internal)
    {
        _clone_zend_execute_internal(execute_data, ret TSRMLS_CC);
    }
    else
    {
        execute_internal(execute_data, ret TSRMLS_CC);
    }
#endif

    if (SUCCESS == is_tracing)
    {
        performance_frame_end(TSRMLS_C);
    }
}

int performance_frame_begin(zend_execute_data *execute_data TSRMLS_DC)
{
    return SUCCESS;
}

void performance_frame_end(TSRMLS_D)
{

}

