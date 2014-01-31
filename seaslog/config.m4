dnl $Id$
dnl config.m4 for extension seaslog

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(seaslog, for seaslog support,
dnl Make sure that the comment is aligned:
[  --with-seaslog             Include seaslog support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(seaslog, whether to enable seaslog support,
dnl Make sure that the comment is aligned:
dnl [  --enable-seaslog           Enable seaslog support])

if test "$PHP_SEASLOG" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-seaslog -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/seaslog.h"  # you most likely want to change this
  dnl if test -r $PHP_SEASLOG/$SEARCH_FOR; then # path given as parameter
  dnl   SEASLOG_DIR=$PHP_SEASLOG
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for seaslog files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       SEASLOG_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$SEASLOG_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the seaslog distribution])
  dnl fi

  dnl # --with-seaslog -> add include path
  dnl PHP_ADD_INCLUDE($SEASLOG_DIR/include)

  dnl # --with-seaslog -> check for lib and symbol presence
  dnl LIBNAME=seaslog # you may want to change this
  dnl LIBSYMBOL=seaslog # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $SEASLOG_DIR/lib, SEASLOG_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_SEASLOGLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong seaslog lib version or lib not found])
  dnl ],[
  dnl   -L$SEASLOG_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(SEASLOG_SHARED_LIBADD)

  PHP_NEW_EXTENSION(seaslog, seaslog.c, $ext_shared)
fi
