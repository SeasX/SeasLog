
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "ext/date/php_date.h"
#include "php_seaslog.h"

ZEND_DECLARE_MODULE_GLOBALS(seaslog)

/* True global resources - no need for thread safety here */
static int le_seaslog;
static char *last_logger = "default";
static char *base_path = "";

/* {{{ seaslog_functions[]
 *
 * Every user visible function must have an entry in seaslog_functions[].
 */
const zend_function_entry seaslog_functions[] = {
	PHP_FE(seaslog_set_basepath,	NULL)
	PHP_FE(seaslog_get_basepath,	NULL)
	PHP_FE(seaslog,	NULL)
	PHP_FE(seaslog_set_logger,	NULL)
	PHP_FE(seaslog_get_lastlogger,	NULL)
	PHP_FE(,	NULL)
	PHP_FE_END	/* Must be the last line in seaslog_functions[] */
};
/* }}} */

/* {{{ seaslog_module_entry
 */
zend_module_entry seaslog_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"SeasLog",
	seaslog_functions,
	PHP_MINIT(seaslog),
	PHP_MSHUTDOWN(seaslog),
	PHP_RINIT(seaslog),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(seaslog),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(seaslog),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_SEASLOG
ZEND_GET_MODULE(seaslog)
#endif

/* {{{ PHP_INI
 */

PHP_INI_BEGIN()
	STD_PHP_INI_ENTRY("seaslog.default_basepath", "/log", PHP_INI_ALL, OnUpdateString, default_basepath, zend_seaslog_globals, seaslog_globals)
	STD_PHP_INI_ENTRY("seaslog.default_logger", "default", PHP_INI_ALL, OnUpdateString, default_logger, zend_seaslog_globals, seaslog_globals)
	STD_PHP_INI_ENTRY("seaslog.logger", "default", PHP_INI_ALL, OnUpdateString, logger, zend_seaslog_globals, seaslog_globals)
PHP_INI_END()

/* }}} */

/* {{{ php_seaslog_init_globals
 */

static void php_seaslog_init_globals(zend_seaslog_globals *seaslog_globals)
{

}

/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(seaslog)
{

	REGISTER_INI_ENTRIES();
	base_path = SEASLOG_G(default_basepath);

	REGISTER_STRINGL_CONSTANT("SEASLOG_VERSION", SEASLOG_VERSION, 	sizeof(SEASLOG_VERSION) - 1, 	CONST_PERSISTENT | CONST_CS);
	REGISTER_STRINGL_CONSTANT("SEASLOG_AUTHOR", SEASLOG_AUTHOR, 	sizeof(SEASLOG_AUTHOR) - 1, 	CONST_PERSISTENT | CONST_CS);

	REGISTER_STRINGL_CONSTANT("SEASLOG_TYPE_INFO", SEASLOG_TYPE_INFO, 	sizeof(SEASLOG_TYPE_INFO) - 1, 	CONST_PERSISTENT | CONST_CS);
	REGISTER_STRINGL_CONSTANT("SEASLOG_TYPE_WARN", SEASLOG_TYPE_WARN, 	sizeof(SEASLOG_TYPE_WARN) - 1, 	CONST_PERSISTENT | CONST_CS);
	REGISTER_STRINGL_CONSTANT("SEASLOG_TYPE_ERRO", SEASLOG_TYPE_ERRO, 	sizeof(SEASLOG_TYPE_ERRO) - 1, 	CONST_PERSISTENT | CONST_CS);

	REGISTER_STRINGL_CONSTANT("SEASLOG_TYPE_INFO_STR", SEASLOG_TYPE_INFO_STR, 	sizeof(SEASLOG_TYPE_INFO_STR) - 1, 	CONST_PERSISTENT | CONST_CS);
	REGISTER_STRINGL_CONSTANT("SEASLOG_TYPE_WARN_STR", SEASLOG_TYPE_WARN_STR, 	sizeof(SEASLOG_TYPE_WARN_STR) - 1, 	CONST_PERSISTENT | CONST_CS);
	REGISTER_STRINGL_CONSTANT("SEASLOG_TYPE_ERRO_STR", SEASLOG_TYPE_ERRO_STR, 	sizeof(SEASLOG_TYPE_ERRO_STR) - 1, 	CONST_PERSISTENT | CONST_CS);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(seaslog)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(seaslog)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(seaslog)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(seaslog)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "seaslog support", "enabled");
	php_info_print_table_row(2, "Version", SEASLOG_VERSION);
	php_info_print_table_row(2, "Author", SEASLOG_AUTHOR);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

static char *mk_str_by_type(char *stype)
{
    char *result;
    int length;
    zval *str;
    MAKE_STD_ZVAL(str);

    str = SEASLOG_TYPE_INFO_STR;
    if (strcmp(stype,SEASLOG_TYPE_WARN) == 0){
        str = SEASLOG_TYPE_WARN_STR;
    }else if (strcmp(stype,SEASLOG_TYPE_ERRO) == 0){
        str = SEASLOG_TYPE_ERRO_STR;
    }

    spprintf(&result, 0, "%s", str);
    return result;
}

/* {{{ proto bool seaslog_set_basepath(string base_path)
    */
PHP_FUNCTION(seaslog_set_basepath)
{
	char *_base_path = NULL;
	int argc = ZEND_NUM_ARGS();
	int _base_path_len;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &_base_path, &_base_path_len) == FAILURE)
		return;

    if (argc > 0){
        spprintf(&base_path,0,"%s",_base_path);
        RETURN_TRUE;
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto string seaslog_get_basepath()
    */
PHP_FUNCTION(seaslog_get_basepath)
{
	char *str;
	int len = 0;
	len = spprintf(&str, 0, "%s", base_path);
	RETURN_STRINGL(str, len, 0);
}
/* }}} */

/* {{{ proto bool seaslog_set_logger(string module)
    */
PHP_FUNCTION(seaslog_set_logger)
{
	char *module = NULL;
	int argc = ZEND_NUM_ARGS();
	int module_len;

	if (zend_parse_parameters(argc TSRMLS_CC, "s", &module, &module_len) == FAILURE)
		return;

    if (argc > 0){
        spprintf(&last_logger,0,"%s",module);
        RETURN_TRUE;
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool seaslog(string message[, string module[,int message_type]])
    */
PHP_FUNCTION(seaslog)
{
	char *message,*module,*log_info,*log_file_path = NULL;
	int argc = ZEND_NUM_ARGS();
	int message_len,module_len,log_len,message_type_len;
	char *message_type;
	char *_date ,*_time, *_type = NULL;
	char *_log_path = NULL;
	char *logger;
	char *stype;

	if (zend_parse_parameters(argc TSRMLS_CC, "s|ss",&message, &message_len,&message_type,&message_type_len,&module, &module_len) == FAILURE)
		return;

	TSRMLS_FETCH();
	
	_date = php_format_date("Ymd",3,(long)time(NULL),(long)time(NULL));
	_time = php_format_date("Y:m:d H:i:s",11,(long)time(NULL),(long)time(NULL));

	if (argc > 2){
		logger = module;
	}else{
		if (strcmp(last_logger,"default") == 0){
			logger = base_path;
		}else{
			logger = last_logger;
		}
	}

	spprintf(&last_logger,0,"%s",logger);

	spprintf(&_log_path, 0, "%.78s/%.78s", base_path,logger);
	_mk_log_dir(_log_path);

    if (argc < 2){
        message_type = SEASLOG_TYPE_INFO;
    }

    stype = mk_str_by_type(message_type);

//	php_printf("%d\n",argc);
//	php_printf("%s\n",logger);
//	php_printf("%s\n",last_logger);
//  php_printf("%s\n",stype);

	spprintf(&log_file_path,0,"%.78s/%.78s.log",_log_path,_date);
	log_len = spprintf(&log_info, 0, "%.78s | %.78s | %.78s \n",stype, _time, message);

	if (_php_log_ex(log_info, log_len, log_file_path) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string seaslog_get_lastlogger()
    */
PHP_FUNCTION(seaslog_get_lastlogger)
{
	char *str;
	int len = 0;
	len = spprintf(&str, 0, "%s", last_logger);
	RETURN_STRINGL(str, len, 0);
}
/* }}} */

/* {{{ proto  ()
    */
PHP_FUNCTION()
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	php_error(E_WARNING, ": not yet implemented");
}
/* }}} */

PHPAPI int _mk_log_dir(char *dir)
{
	int _ck_dir = _ck_log_dir(dir);
	
	if (_ck_dir == FAILURE){
	    zval *zcontext = NULL;
		long mode = 0777;
		zend_bool recursive = 1;
		php_stream_context *context;

		context = php_stream_context_from_zval(zcontext, 0);

		if (php_stream_mkdir(dir, mode, (recursive ? PHP_STREAM_MKDIR_RECURSIVE : 0) | REPORT_ERRORS, context)){
			return FAILURE;
		}

		return SUCCESS;
	}else {
	    return FAILURE;
	}

}

PHPAPI int _ck_log_dir(char *dir)
{
	zval *function_name;
	zval *retval;
	zval *str;
	zval **param[1]; 
	MAKE_STD_ZVAL(function_name);
	ZVAL_STRING(function_name , "is_dir", 1);

 	MAKE_STD_ZVAL(str);
    ZVAL_STRING(str, dir, 1); 
    param[0] = &str;

	if(call_user_function_ex(EG(function_table), NULL, function_name, &retval, 1, param,0, EG(active_symbol_table)) != SUCCESS)
	{
		zend_error(E_ERROR, "Function call failed");
	}

	if (retval != NULL && zval_is_true(retval)){
		return SUCCESS;
	}
	
	return FAILURE;
}

PHPAPI int _php_log_ex(char *message, int message_len, char *opt) /* {{{ */
{/* {{{ */
	php_stream *stream = NULL;

	stream = php_stream_open_wrapper(opt, "a", IGNORE_URL_WIN | ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL);
	if (!stream) {
		return FAILURE;
	}
	php_stream_write(stream, message, message_len);
	php_stream_close(stream);

	return SUCCESS;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
