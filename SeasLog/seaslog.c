/*
  +----------------------------------------------------------------------+
  | SeasLog                                                              |
  +----------------------------------------------------------------------+
  | This source file is subject to version 2.0 of the Apache license,    |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.apache.org/licenses/LICENSE-2.0.html                      |
  | If you did not receive a copy of the Apache2.0 license and are unable|
  | to obtain it through the world-wide-web, please send a note to       |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Neeke.Gao  <ciogao@gmail.com>                                |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"
#include "ext/standard/php_string.h"
#include "ext/date/php_date.h"
#include "php_seaslog.h"
#include "zend_extensions.h"
#include <sys/resource.h>
#include <stdlib.h>

#ifdef PHP_WIN32
#include "win32/time.h"
#elif defined(NETWARE)
#include <sys/timeval.h>
#include <sys/time.h>
#else
#include <sys/time.h>
#endif

void seaslog_init_logger(TSRMLS_D);
static int seaslog_shutdown_buffer(TSRMLS_D);

ZEND_DECLARE_MODULE_GLOBALS(seaslog)

/* True global resources - no need for thread safety here */
static int le_seaslog;
static char *last_logger = "default";
static char *base_path = "";
static zend_bool disting_type = 0;
static zend_bool disting_by_hour = 0;
static zend_bool use_buffer = 0;

static zval *log_buffer;

typedef struct sl_global_t {
  int  started;
  zval *log_buffer;

} sl_global_t;

static sl_global_t SL_globals;


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
    PHP_FE(seaslog_analyzer_count, NULL)
    PHP_FE(seaslog_analyzer_detail, NULL)
    PHP_FE(seaslog_get_buffer, NULL)
    {NULL, NULL, NULL}	/* Must be the last line in seaslog_functions[] */
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
    STD_PHP_INI_BOOLEAN("seaslog.disting_type", "0", PHP_INI_ALL, OnUpdateBool, disting_type, zend_seaslog_globals, seaslog_globals)
    STD_PHP_INI_BOOLEAN("seaslog.disting_by_hour","0", PHP_INI_ALL,OnUpdateBool, disting_by_hour, zend_seaslog_globals, seaslog_globals)
    STD_PHP_INI_BOOLEAN("seaslog.use_buffer","0",PHP_INI_ALL,OnUpdateBool,use_buffer,zend_seaslog_globals,seaslog_globals)
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
    seaslog_init_logger(TSRMLS_C);

    REGISTER_STRINGL_CONSTANT("SEASLOG_VERSION", SEASLOG_VERSION, 	sizeof(SEASLOG_VERSION) - 1, 	CONST_PERSISTENT | CONST_CS);
    REGISTER_STRINGL_CONSTANT("SEASLOG_AUTHOR", SEASLOG_AUTHOR, 	sizeof(SEASLOG_AUTHOR) - 1, 	CONST_PERSISTENT | CONST_CS);

    REGISTER_LONG_CONSTANT("SEASLOG_TYPE_INFO", SEASLOG_TYPE_INFO, 	CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("SEASLOG_TYPE_WARN", SEASLOG_TYPE_WARN, 	CONST_PERSISTENT | CONST_CS);
    REGISTER_LONG_CONSTANT("SEASLOG_TYPE_ERRO", SEASLOG_TYPE_ERRO, 	CONST_PERSISTENT | CONST_CS);

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
    UNREGISTER_INI_ENTRIES();
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
	seaslog_shutdown_buffer(TSRMLS_C);
    seaslog_init_logger(TSRMLS_C);
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(seaslog)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "SeasLog support", "Enabled");
    php_info_print_table_row(2, "Version", SEASLOG_VERSION);
    php_info_print_table_row(2, "Author", SEASLOG_AUTHOR);
    php_info_print_table_row(2,"Supports","https://github.com/Neeke/SeasLog");
    php_info_print_table_end();

    DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ void seaslog_init_logger(TSRMLS_D)*/
void seaslog_init_logger(TSRMLS_D)
{
    SEASLOG_G(base_path) = SEASLOG_G(default_basepath);
    SEASLOG_G(last_logger) = SEASLOG_G(default_logger);
}
/* }}}*/

/* {{{ void seaslog_init_buffer(TSRMLS_D)*/
void seaslog_init_buffer(TSRMLS_D)
{
	if (SEASLOG_G(use_buffer)) {
        if (!SL_globals.started) {
            if (SL_globals.log_buffer) {
                    zval_dtor(SL_globals.log_buffer);
                    FREE_ZVAL(SL_globals.log_buffer);
            }
            MAKE_STD_ZVAL(SL_globals.log_buffer);
            array_init(SL_globals.log_buffer);
            SL_globals.started = 1;
        }
	}
}
/* }}}*/

/* {{{ static int seaslog_buffer_set(char *log_info,char *path)*/
static int seaslog_buffer_set(char *log_info,char *path TSRMLS_DC) {
    HashTable   *ht;
    void        *data;
    zval        *log_array;
    zval        *old_log_array;
    zval        *new_log;

    if (!SL_globals.log_buffer || !(ht = HASH_OF(SL_globals.log_buffer))) {
        return FAILURE;
    }

    if (zend_hash_find(ht, path, strlen(path) + 1, &data) == SUCCESS) {
        old_log_array = *(zval **) data;

        MAKE_STD_ZVAL(new_log);
        ZVAL_STRING(new_log,log_info,1);

        zend_hash_next_index_insert(Z_ARRVAL_P(old_log_array), (void *)&new_log, sizeof(zval *), NULL);

    } else {
        MAKE_STD_ZVAL(log_array);
        array_init(log_array);

        add_next_index_string(log_array, log_info, 1);
        add_assoc_zval(SL_globals.log_buffer, path, log_array);
    }

    return SUCCESS;
}
/* }}}*/

/* {{{ int _php_log_ex(char *message, int message_len, char *opt) */
static int real_php_log_ex(char *message, int message_len, char *opt)
{
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

/* {{{ static int seaslog_shutdown_buffer(TSRMLS_D)*/
static int seaslog_shutdown_buffer(TSRMLS_D)
{
	if (SEASLOG_G(use_buffer))	{
		if ((sizeof(SL_globals.log_buffer) / sizeof(int)) > 0) {
        HashTable   *ht;
        HashPosition pos;

        if (!SL_globals.log_buffer || !(ht = HASH_OF(SL_globals.log_buffer))) {
            return FAILURE;
        }

        for(zend_hash_internal_pointer_reset_ex(ht, &pos);
            zend_hash_has_more_elements_ex(ht, &pos) == SUCCESS;
            zend_hash_move_forward_ex(ht, &pos))
            {
                char *key;
                uint keylen;
                ulong idx;
                int type;
                int i,num;

                zval **ppzval, tmpcopy;

                type = zend_hash_get_current_key_ex(ht,&key, &keylen,&idx, 0, &pos);
                if (zend_hash_get_current_data_ex(ht,(void**)&ppzval, &pos) == FAILURE) {
                    continue;
                }

                tmpcopy = **ppzval;
                zval_copy_ctor(&tmpcopy);
                INIT_PZVAL(&tmpcopy);

                convert_to_array(&tmpcopy);

                HashTable   *ht_child;
                zval **ppzval_child;
                char *log_info = NULL;
                char *log_info_tmp = NULL;
                char *log_tmp = NULL;

                ht_child = Z_ARRVAL(tmpcopy);
                for(zend_hash_internal_pointer_reset(ht_child);
                        zend_hash_has_more_elements(ht_child) == SUCCESS;
                        zend_hash_move_forward(ht_child))
                {
                    if (zend_hash_get_current_data(ht_child, (void**)&ppzval_child) == FAILURE) {
                        continue;
                    }

                    if (IS_STRING == Z_TYPE_PP(ppzval_child) && Z_STRLEN_PP(ppzval_child)) {
                        log_tmp = Z_STRVAL_PP(ppzval_child);
                        if (log_info) {
                            log_info_tmp = log_info;
                            spprintf(&log_info,0,"%s%s",log_info_tmp,log_tmp);
                        } else {
                            spprintf(&log_info,0,"%s",log_tmp);
                        }

                    }
                }

                real_php_log_ex(log_info, strlen(log_info),key);

                zval_dtor(&tmpcopy);
            }

            if (SL_globals.log_buffer) {
                zval_dtor(SL_globals.log_buffer);
                FREE_ZVAL(SL_globals.log_buffer);
            }

            MAKE_STD_ZVAL(SL_globals.log_buffer);
            array_init(SL_globals.log_buffer);
            SL_globals.started = 1;

            return SUCCESS;
        }
    }

    return SUCCESS;
}
/* }}}*/

/* {{{ char *mk_str_by_type(int stype)*/
static char *mk_str_by_type(int stype)
{
    char *result;
    int length;
    char *str;

    str = SEASLOG_TYPE_INFO_STR;
    if (stype == SEASLOG_TYPE_WARN){
        str = SEASLOG_TYPE_WARN_STR;
    }else if (stype == SEASLOG_TYPE_ERRO){
        str = SEASLOG_TYPE_ERRO_STR;
    }

    spprintf(&result, 0, "%s", str);
    return result;
}
/* }}} */

/* {{{ char *mk_real_log_path(char *log_path,char *date,char *stype)*/
static char *mk_real_log_path(char *log_path,char *date,char *stype TSRMLS_DC){
    char *log_file_path = NULL;
    if (SEASLOG_G(disting_type)){
        spprintf(&log_file_path,0,"%s/%s.%s.log",log_path,stype,date);
    }else{
        spprintf(&log_file_path,0,"%s/%s.log",log_path,date);
    }
    
    return log_file_path;
}
/* }}} */

/* {{{ char *delN(char * a)*/
static char *delN(char * a){
    int l;
    l=strlen(a);
    a[l-1]='\0';
    return a;
}
/* }}} */

/* {{{ char *mic_time()*/
static char *mic_time(){
   struct timeval now;
   char *tstr;
   timerclear(&now);
   gettimeofday(&now,NULL);
   spprintf(&tstr,0,"%ld.%ld",(long)time(NULL),(long)now.tv_usec/1000);
   return tstr;
}
/* }}}*/

/*{{{ char *mk_real_date()*/
static char *mk_real_date(TSRMLS_D){
    char *_date;
    if (SEASLOG_G(disting_by_hour)) {
        _date = php_format_date("YmdH",5,(long)time(NULL),(long)time(NULL));
        } else {
        _date = php_format_date("Ymd",3,(long)time(NULL),(long)time(NULL));
    }

    return _date;
}
/*}}}*/

/* {{{ long get_type_count(char *log_path,int stype)*/
static long get_type_count(char *log_path,int stype TSRMLS_DC)
{
    FILE * fp;
    char buffer[BUFSIZ];
    char *str ,*path,*_log_path,*sh;
    long count;

    spprintf(&_log_path, 0, "%s/%s", SEASLOG_G(base_path),SEASLOG_G(last_logger));
    int _ck_dir = _ck_log_dir(_log_path);
    if (_ck_dir == FAILURE){
        return 0;
    }
    
    if (SEASLOG_G(disting_type)){
        spprintf(&path,0,"%s/%s/%s.%s*",SEASLOG_G(base_path),SEASLOG_G(last_logger),mk_str_by_type(stype),log_path);
    }else{
        spprintf(&path,0,"%s/%s/%s*",SEASLOG_G(base_path),SEASLOG_G(last_logger),log_path);
    }
    
    spprintf(&sh,0,"more %s | grep '%s' -wc",path,mk_str_by_type(stype));
    
    fp = VCWD_POPEN(sh, "r");
    if (!fp){
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to fork [%s]", sh);
        return -1;
    }else{
        char *temp_p = fgets(buffer, sizeof(buffer), fp);
        pclose(fp);
    }

    count = atoi(delN(buffer));
    return count;
}
/* }}} */

/* {{{ int get_detail(char *log_path,int stype,zval *return_value TSRMLS_DC)*/
static int get_detail(char *log_path,int stype,zval *return_value TSRMLS_DC)
{
    FILE * fp;
    char buffer[BUFSIZ+1];
    char *str ,*path ;
    char *sh;
    
    memset(buffer,'\0',sizeof(buffer));

    array_init(return_value);

    if (disting_type){
        spprintf(&path,0,"%s/%s/%s.%s*",SEASLOG_G(base_path),SEASLOG_G(last_logger),mk_str_by_type(stype),log_path);
    }else{
        spprintf(&path,0,"%s/%s/%s*",SEASLOG_G(base_path),SEASLOG_G(last_logger),log_path);
    }

    spprintf(&sh,0,"more %s | grep '%s' -w",path,mk_str_by_type(stype));

    fp = VCWD_POPEN(sh, "r");

    if (!fp){
        php_error_docref(NULL TSRMLS_CC, E_WARNING, "Unable to fork [%s]", sh);
        return -1;
    }else{
       while((fgets(buffer,sizeof(buffer),fp)) != NULL){
            if (strcspn(buffer,SEASLOG_G(base_path)) != 0){
                add_next_index_string(return_value,delN(buffer),1);
            }
       }
       pclose(fp);
    }

    return 1;
}
/* }}} */

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
        spprintf(&SEASLOG_G(base_path),0,"%s",_base_path);
        RETURN_TRUE;
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto string seaslog_get_basepath()
    */
PHP_FUNCTION(seaslog_get_basepath)
{
    char *module = NULL;
    int argc = ZEND_NUM_ARGS();
    int module_len;

    if (zend_parse_parameters(argc TSRMLS_CC, "|s", &module, &module_len) == FAILURE)
        return;

    char *str;
    int len = 0;
    len = spprintf(&str, 0, "%s", SEASLOG_G(base_path));
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
        spprintf(&SEASLOG_G(last_logger),0,"%s",module);
        RETURN_TRUE;
    }

    RETURN_FALSE;
}
/* }}} */

/* {{{ proto bool seaslog(string message[, string module[,int message_type]])
    */
PHP_FUNCTION(seaslog)
{
    seaslog_init_buffer(TSRMLS_C);

    char *message,*module,*log_info,*log_file_path = NULL;
    int argc = ZEND_NUM_ARGS();
    int message_len,module_len,log_len;
    long message_type;
    char *_date ,*_time, *_type = NULL;
    char *_log_path = NULL;
    char *logger;
    char *stype;

    if (zend_parse_parameters(argc TSRMLS_CC, "s|ls",&message, &message_len,&message_type,&module, &module_len) == FAILURE)
        return;

    if (argc > 2){
        logger = module;
        if (strcmp(SEASLOG_G(last_logger),"default") == 0){
            spprintf(&SEASLOG_G(last_logger),0,"%s",logger);
        }
    }else{
        logger = SEASLOG_G(last_logger);
    }

    spprintf(&_log_path, 0, "%s/%s", SEASLOG_G(base_path),logger);
    _mk_log_dir(_log_path);
    
    if (argc < 2){
        message_type = SEASLOG_TYPE_INFO;
    }

    stype = mk_str_by_type(message_type);

    _date = mk_real_date();

    _time = php_format_date("Y:m:d H:i:s",11,(long)time(NULL),(long)time(NULL));

    //~ php_printf("%d\n",argc);
    //~ php_printf("%s\n",logger);
    //~ php_printf("%s\n",last_logger);
    //~ php_printf("%s\n",stype);

    log_file_path = mk_real_log_path(_log_path,_date,stype TSRMLS_CC);
    
    log_len = spprintf(&log_info, 0, "%s | %d | %s | %s | %s \n",stype,getpid(), mic_time(),_time, message);

    if (_php_log_ex(log_info, log_len, log_file_path TSRMLS_CC) == FAILURE) {
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
    len = spprintf(&str, 0, "%s", SEASLOG_G(last_logger));
    RETURN_STRINGL(str, len, 0);
}
/* }}} */

/* {{{ proto long seaslog_analyzer_count(int message_type[,string log_path])
    */
PHP_FUNCTION (seaslog_analyzer_count)
{
    int argc = ZEND_NUM_ARGS();
    char *log_path,*result;
    int len = 0;
    int log_path_len,message_type_len;
    long message_type;
    long count;

    if (zend_parse_parameters(argc TSRMLS_CC, "|ls",&message_type,&log_path, &log_path_len) == FAILURE)
        return;

    if (argc == 0){
        array_init(return_value);
        log_path = "";

        long count_info,count_warn,count_erro;
        count_info = get_type_count(log_path,SEASLOG_TYPE_INFO);
        count_warn = get_type_count(log_path,SEASLOG_TYPE_WARN);
        count_erro = get_type_count(log_path,SEASLOG_TYPE_ERRO);

        add_assoc_long(return_value, SEASLOG_TYPE_INFO_STR, count_info);
        add_assoc_long(return_value, SEASLOG_TYPE_WARN_STR, count_warn);
        add_assoc_long(return_value, SEASLOG_TYPE_ERRO_STR, count_erro);
    }else if (argc == 1){
        log_path = "";
        count = get_type_count(log_path,message_type);

        RETURN_LONG(count);
    }else{
        count = get_type_count(log_path,message_type);

        RETURN_LONG(count);
    }
}
/* }}} */

/* {{{ proto long seaslog_analyzer_detail(int message_type[,string log_path])
    */
PHP_FUNCTION (seaslog_analyzer_detail)
{
    char *log_path;
    int log_path_len;
    long type;
    int argc = ZEND_NUM_ARGS();

    if (zend_parse_parameters(argc TSRMLS_CC, "l|s",&type,&log_path, &log_path_len) == FAILURE) {
        RETURN_FALSE;
    }

    if (argc < 2){
        log_path = "*";
    }else{
        if (type != SEASLOG_TYPE_INFO && type != SEASLOG_TYPE_WARN){
            type = SEASLOG_TYPE_ERRO;
        }
    }

    get_detail(log_path, type, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto array seaslog_get_buffer()
    */
PHP_FUNCTION (seaslog_get_buffer)
{
    if (SEASLOG_G(use_buffer)) {
        RETURN_ZVAL(SL_globals.log_buffer, 1, 0);
    }
}
/* }}}*/


/* {{{ int _mk_log_dir(char *dir) */
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
/* }}} */

/* {{{ int _ck_log_dir(char *dir) */
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
/* }}} */

/* {{{ int _php_log_ex(char *message, int message_len, char *opt) */
PHPAPI int _php_log_ex(char *message, int message_len, char *opt TSRMLS_DC)
{
    if (SEASLOG_G(use_buffer)) {
        seaslog_buffer_set(message,opt TSRMLS_CC);
		return SUCCESS;
	} else {
		return real_php_log_ex(message, message_len, opt);
	}
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
