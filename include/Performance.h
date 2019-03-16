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

void init_zend_hooks(TSRMLS_D);
void recovery_zend_hooks(TSRMLS_D);

void seaslog_rinit_performance(TSRMLS_D);
void seaslog_clear_performance(zend_class_entry *ce TSRMLS_DC);
int seaslog_check_performance_active(TSRMLS_D);

int performance_frame_begin(zend_execute_data *execute_data TSRMLS_DC);
void performance_frame_end(TSRMLS_D);

seaslog_frame* seaslog_performance_fast_alloc_frame(TSRMLS_D);
void seaslog_performance_fast_free_frame(seaslog_frame *p TSRMLS_DC);
void seaslog_performance_free_the_free_list(TSRMLS_D);

void seaslog_performance_bucket_free(seaslog_performance_bucket *bucket TSRMLS_DC);

char* seaslog_performance_get_class_name(zend_execute_data *data TSRMLS_DC);
char* seaslog_performance_get_function_name(zend_execute_data *data TSRMLS_DC);

int process_seaslog_performance_log(zend_class_entry *ce TSRMLS_DC);
int process_seaslog_performance_clear(TSRMLS_D);
#endif /* _SEASLOG_PERFORMANCE_H_ */

