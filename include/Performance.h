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

#ifndef _SEASLOG_PERFORMANCE_H_
#define _SEASLOG_PERFORMANCE_H_

#include "php_seaslog.h"

void seaslog_memory_usage(smart_str *buf TSRMLS_DC);
void seaslog_peak_memory_usage(smart_str *buf TSRMLS_DC);

void initZendHooks(TSRMLS_D);
void recoveryZendHooks(TSRMLS_D);

void seaslog_init_performance(TSRMLS_D);
void seaslog_clear_performance(TSRMLS_D);

#if PHP_VERSION_ID >= 70000
void (*_clone_zend_execute_ex) (zend_execute_data *execute_data TSRMLS_DC);
static void (*_clone_zend_execute_internal) (zend_execute_data *execute_data, zval *return_value);

ZEND_DLEXPORT void seaslog_execute_ex (zend_execute_data *execute_data TSRMLS_DC);
ZEND_DLEXPORT void seaslog_execute_internal(zend_execute_data *execute_data, zval *return_value);

#elif PHP_VERSION_ID >= 50500
static void (*_clone_zend_execute_ex) (zend_execute_data *execute_data TSRMLS_DC);
static void (*_clone_zend_execute_internal) (zend_execute_data *data, struct _zend_fcall_info *fci, int ret TSRMLS_DC);

ZEND_DLEXPORT void seaslog_execute_ex (zend_execute_data *execute_data TSRMLS_DC);
ZEND_DLEXPORT void seaslog_execute_internal(zend_execute_data *execute_data, struct _zend_fcall_info *fci, int ret TSRMLS_DC);

#else
ZEND_DLEXPORT void (*_clone_zend_execute) (zend_op_array *ops TSRMLS_DC);
ZEND_DLEXPORT void (*_clone_zend_execute_internal) (zend_execute_data *data, int ret TSRMLS_DC);

ZEND_DLEXPORT void seaslog_execute (zend_op_array *ops TSRMLS_DC);
ZEND_DLEXPORT void seaslog_execute_internal(zend_execute_data *execute_data, int ret TSRMLS_DC);
#endif

int performance_frame_begin(zend_execute_data *execute_data TSRMLS_DC);
void performance_frame_end(TSRMLS_D);

seaslog_frame_t* seaslog_performance_fast_alloc_frame(TSRMLS_D);
void seaslog_performance_fast_free_frame(seaslog_frame_t *p TSRMLS_DC);
char* seaslog_performance_get_class_name(zend_execute_data *data TSRMLS_DC);
char* seaslog_performance_get_function_name(zend_execute_data *data TSRMLS_DC);

#endif /* _SEASLOG_PERFORMANCE_H_ */

