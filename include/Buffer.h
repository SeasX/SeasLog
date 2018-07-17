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

#ifndef _SEASLOG_BUFFER_H_
#define _SEASLOG_BUFFER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "php_seaslog.h"

void initBufferSwitch(TSRMLS_D);
void seaslog_init_buffer(TSRMLS_D);
void seaslog_shutdown_buffer(int re_init TSRMLS_DC);
void seaslog_clear_buffer(TSRMLS_D);
int seaslog_check_buffer_enable(TSRMLS_D);
int seaslog_buffer_set(char *log_info, int log_info_len, char *path, int path_len, zend_class_entry *ce TSRMLS_DC);

#ifdef __cplusplus
}
#endif

#endif /* _SEASLOG_BUFFER_H_ */

