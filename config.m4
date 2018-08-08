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

    seaslog_source_file="seaslog.c \
        src/Performance.c \
        src/Analyzer.c \
        src/StreamWrapper.c \
        src/TemplateFormatter.c \
        src/ExceptionHook.c \
        src/Appender.c \
        src/Datetime.c
        src/Logger.c \
        src/Buffer.c \
        src/ErrorHook.c \
        src/Request.c \
        src/Common.c"
    PHP_NEW_EXTENSION(seaslog, $seaslog_source_file, $ext_shared,,,)

    PHP_INSTALL_HEADERS([ext/SeasLog], [*.h config.h include/*.h])

    dnl PHP_ADD_INCLUDE([$ext_srcdir])
    dnl PHP_ADD_INCLUDE([$ext_srcdir/include])

fi

