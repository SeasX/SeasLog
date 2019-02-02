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

#ifndef _SEASLOG_COMMON_H_
#define _SEASLOG_COMMON_H_

#include "php_seaslog.h"

int seaslog_smart_str_get_len(smart_str str);

int check_sapi_is_cli(TSRMLS_D);
int check_log_level(int level TSRMLS_DC);
int seaslog_get_level_int(char *level);
char *str_replace(char *src, const char *from, const char *to);
int message_trim_wrap(char *message,int message_len TSRMLS_DC);

char* delN(char *a);
char* get_uniqid();
char* php_strtr_array(char *str, int slen, HashTable *pats);

#endif /* _SEASLOG_COMMON_H_ */

