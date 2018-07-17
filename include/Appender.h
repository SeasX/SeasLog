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

#ifndef _SEASLOG_APPENDER_H_
#define _SEASLOG_APPENDER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "php_seaslog.h"

int seaslog_log_content(int argc, char *level, int level_int, char *message, int message_len, HashTable *content, char *module, int module_len, zend_class_entry *ce TSRMLS_DC);
int seaslog_log_ex(int argc, char *level, int level_int, char *message, int message_len, char *module, int module_len, zend_class_entry *ce TSRMLS_DC);
int make_log_dir(char *dir TSRMLS_DC);


#ifdef __cplusplus
}
#endif

#endif /* _SEASLOG_APPENDER_H_ */

