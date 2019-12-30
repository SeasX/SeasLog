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

#ifndef _SEASLOG_REQUEST_H_
#define _SEASLOG_REQUEST_H_

#include "php_seaslog.h"

void seaslog_init_pid(void);
void seaslog_init_host_name(void);
void seaslog_init_request_id(void);
void seaslog_init_auto_globals(void);
int seaslog_init_request_variable(void);
void seaslog_clear_request_id(void);
void seaslog_clear_pid(void);
void seaslog_clear_host_name(void);
void seaslog_clear_request_variable(void);
void get_code_filename_line(smart_str *result );

#endif /* _SEASLOG_REQUEST_H_ */

