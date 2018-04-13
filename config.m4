dnl config.m4 for extension seaslog

dnl +----------------------------------------------------------------------+
dnl | SeasLog                                                              |
dnl +----------------------------------------------------------------------+
dnl | This source file is subject to version 2.0 of the Apache license,    |
dnl | that is bundled with this package in the file LICENSE, and is        |
dnl | available through the world-wide-web at the following url:           |
dnl | http://www.apache.org/licenses/LICENSE-2.0.html                      |
dnl | If you did not receive a copy of the Apache2.0 license and are unable|
dnl | to obtain it through the world-wide-web, please send a note to       |
dnl | license@php.net so we can mail you a copy immediately.               |
dnl +----------------------------------------------------------------------+
dnl | Author: Neeke.Gao  <neeke@php.net>                                   |
dnl +----------------------------------------------------------------------+

PHP_ARG_WITH(seaslog, for seaslog support,
[  --with-seaslog             Include seaslog support])

dnl Otherwise use enable:

if test "$PHP_SEASLOG" != "no"; then
  dnl # --with-seaslog -> add include path
  dnl PHP_ADD_INCLUDE($SEASLOG_DIR/include)

    PHP_NEW_EXTENSION(seaslog, seaslog.c, $ext_shared)

    dnl PHP_ADD_INCLUDE([$ext_srcdir])
    dnl PHP_ADD_INCLUDE([$ext_srcdir/include])

fi

