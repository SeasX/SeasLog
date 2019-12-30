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

#ifndef _SEASLOG_EXCEPTIONHOOK_H_
#define _SEASLOG_EXCEPTIONHOOK_H_

#include "php_seaslog.h"

void seaslog_throw_exception(int type , const char *format, ...);
void init_exception_hooks(void);
void recovery_exception_hooks(void);
void seaslog_throw_exception_hook(zval *exception );

#endif /* _SEASLOG_EXCEPTIONHOOK_H_ */

