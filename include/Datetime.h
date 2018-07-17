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

#ifndef _SEASLOG_DATETIME_H_
#define _SEASLOG_DATETIME_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "php_seaslog.h"

char *make_real_time(TSRMLS_D);
void initRemoteTimeout(TSRMLS_D);
char *seaslog_process_last_sec(int now, int if_first TSRMLS_DC);
void mic_time(smart_str *buf);
char *make_time_RFC3339(TSRMLS_D);
char *make_real_date(TSRMLS_D);
char *make_real_time(TSRMLS_D);
char *seaslog_process_last_min(int now, int if_first TSRMLS_DC);

#ifdef __cplusplus
}
#endif

#endif /* _SEASLOG_DATETIME_H_ */

