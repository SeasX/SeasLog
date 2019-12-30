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

void seaslog_memory_usage(smart_str *buf );
void seaslog_peak_memory_usage(smart_str *buf );

void init_zend_hooks(void);
void recovery_zend_hooks(void);

void seaslog_rinit_performance(void);
void seaslog_clear_performance(zend_class_entry *ce );
int seaslog_check_performance_active(void);

int performance_frame_begin(zend_execute_data *execute_data );
void performance_frame_end(void);

seaslog_frame* seaslog_performance_fast_alloc_frame(void);
void seaslog_performance_fast_free_frame(seaslog_frame *p );
void seaslog_performance_free_the_free_list(void);

void seaslog_performance_bucket_free(seaslog_performance_bucket *bucket );

char* seaslog_performance_get_class_name(zend_execute_data *data );
char* seaslog_performance_get_function_name(zend_execute_data *data );

int process_seaslog_performance_log(zend_class_entry *ce );
int process_seaslog_performance_clear(void);
#endif /* _SEASLOG_PERFORMANCE_H_ */

