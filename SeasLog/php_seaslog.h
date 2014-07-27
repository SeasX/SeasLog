
#ifndef PHP_SEASLOG_H
#define PHP_SEASLOG_H

extern zend_module_entry seaslog_module_entry;
#define phpext_seaslog_ptr &seaslog_module_entry

#ifdef PHP_WIN32
#	define PHP_SEASLOG_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_SEASLOG_API __attribute__ ((visibility("default")))
#else
#	define PHP_SEASLOG_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#define SEASLOG_RES_NAME                    "SeasLog"
#define SEASLOG_VERSION                     "1.0.0"
#define SEASLOG_AUTHOR                      "Chitao.Gao [neeke@php.net]"

#define SEASLOG_DEBUG                       "debug"
#define SEASLOG_INFO                        "info"
#define SEASLOG_NOTICE                      "notice"
#define SEASLOG_WARNING                     "warning"
#define SEASLOG_ERROR                       "error"
#define SEASLOG_CRITICAL                    "critical"
#define SEASLOG_ALERT                       "alert"
#define SEASLOG_EMERGENCY                   "emergency"

PHP_MINIT_FUNCTION(seaslog);
PHP_MSHUTDOWN_FUNCTION(seaslog);
PHP_RINIT_FUNCTION(seaslog);
PHP_RSHUTDOWN_FUNCTION(seaslog);
PHP_MINFO_FUNCTION(seaslog);

PHP_FUNCTION(seaslog_get_version);
PHP_FUNCTION(seaslog_get_author);

zend_class_entry *seaslog_ce;
PHP_METHOD(SEASLOG_RES_NAME,__construct);
PHP_METHOD(SEASLOG_RES_NAME,__destruct);
PHP_METHOD(SEASLOG_RES_NAME,setBasePath);
PHP_METHOD(SEASLOG_RES_NAME,getBasePath);
PHP_METHOD(SEASLOG_RES_NAME,setLogger);
PHP_METHOD(SEASLOG_RES_NAME,getLastLogger);
PHP_METHOD(SEASLOG_RES_NAME,analyzerCount);
PHP_METHOD(SEASLOG_RES_NAME,analyzerDetail);
PHP_METHOD(SEASLOG_RES_NAME,getBuffer);
PHP_METHOD(SEASLOG_RES_NAME,log);
PHP_METHOD(SEASLOG_RES_NAME,debug);
PHP_METHOD(SEASLOG_RES_NAME,info);
PHP_METHOD(SEASLOG_RES_NAME,notice);
PHP_METHOD(SEASLOG_RES_NAME,warning);
PHP_METHOD(SEASLOG_RES_NAME,error);
PHP_METHOD(SEASLOG_RES_NAME,critical);
PHP_METHOD(SEASLOG_RES_NAME,alert);
PHP_METHOD(SEASLOG_RES_NAME,emergency);

ZEND_BEGIN_MODULE_GLOBALS(seaslog)
	char *default_basepath;
	char *default_logger;
	char *logger;
	char *last_logger;
	char *base_path;
    zend_bool disting_type;
    zend_bool disting_by_hour;
    zend_bool use_buffer;
ZEND_END_MODULE_GLOBALS(seaslog)

/* In every utility function you add that needs to use variables 
   in php_seaslog_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as SEASLOG_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define SEASLOG_G(v) TSRMG(seaslog_globals_id, zend_seaslog_globals *, v)
#else
#define SEASLOG_G(v) (seaslog_globals.v)
#endif

#endif	/* PHP_SEASLOG_H */

