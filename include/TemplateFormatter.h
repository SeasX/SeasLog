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

#ifndef _SEASLOG_TEMPLATEFORMATTER_H_
#define _SEASLOG_TEMPLATEFORMATTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "php_seaslog.h"

void seaslog_init_template(TSRMLS_D);
void seaslog_clear_template(TSRMLS_D);
int seaslog_spprintf(char **pbuf TSRMLS_DC, int generate_type, char *level, size_t max_len, ...);
void seaslog_template_formatter(smart_str *xbuf TSRMLS_DC, int generate_type, const char *fmt, char *level, va_list ap);

#ifdef __cplusplus
}
#endif

#endif /* _SEASLOG_TEMPLATEFORMATTER_H_ */

