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

#include "Request.h"
#include "Common.h"

void seaslog_init_host_name(TSRMLS_D)
{
    char buf[255];

    if (gethostname(buf, sizeof(buf) - 1))
    {
        SEASLOG_G(host_name) = estrdup(SEASLOG_GET_HOST_NULL);
        SEASLOG_G(host_name_len) = strlen(SEASLOG_GET_HOST_NULL);
    }
    else
    {
        SEASLOG_G(host_name_len) = spprintf(&SEASLOG_G(host_name), 0, "%s", buf);
    }
}

void seaslog_clear_host_name(TSRMLS_D)
{
    if (SEASLOG_G(host_name))
    {
        efree(SEASLOG_G(host_name));
    }
}

void seaslog_init_pid(TSRMLS_D)
{
    SEASLOG_G(process_id_len) = spprintf(&SEASLOG_G(process_id),0, "%d", getpid());
}

void seaslog_clear_pid(TSRMLS_D)
{
    if (SEASLOG_G(process_id))
    {
        efree(SEASLOG_G(process_id));
    }
}

void seaslog_init_request_id(TSRMLS_D)
{
    SEASLOG_G(request_id) = get_uniqid();
    SEASLOG_G(request_id_len) = strlen(SEASLOG_G(request_id));
}

void seaslog_clear_request_id(TSRMLS_D)
{
    if (SEASLOG_G(request_id))
    {
        efree(SEASLOG_G(request_id));
    }
}

void seaslog_init_auto_globals(TSRMLS_D)
{
    SEASLOG_AUTO_GLOBAL("_SERVER");
}


zval *seaslog_request_query(uint type, void *name, size_t len TSRMLS_DC)
{
#if PHP_VERSION_ID >= 70000
    zval *carrier = NULL, *ret;

    switch (type)
    {
    case SEASLOG_GLOBAL_VARS_SERVER:
        carrier = zend_hash_str_find(&EG(symbol_table), ZEND_STRL("_SERVER"));
        break;
    default:
        break;
    }

    if (!carrier)
    {
        return NULL;
    }

    if ((ret = zend_hash_str_find(Z_ARRVAL_P(carrier), (char *)name, len)) == NULL)
    {
        return NULL;
    }

    Z_TRY_ADDREF_P(ret);

    return ret;
#else
    zval **carrier = NULL, **ret;

    switch (type)
    {
    case SEASLOG_GLOBAL_VARS_SERVER:
        carrier = &PG(http_globals)[type];
        break;
    default:
        break;
    }

    if (!carrier)
    {
        return NULL;
    }

    if (zend_hash_find(Z_ARRVAL_PP(carrier), name, len + 1, (void **)&ret) == FAILURE)
    {
        return NULL;
    }

    Z_ADDREF_P(*ret);

    return *ret;

#endif
}

int seaslog_init_request_variable(TSRMLS_D)
{
    zval *client_ip;
    zval *domain;
    zval *request_uri;
    zval *request_method;

    SEASLOG_G(request_variable) = ecalloc(sizeof(request_variable_t), 1);

    if (!strncmp(sapi_module.name, SEASLOG_CLI_KEY, sizeof(SEASLOG_CLI_KEY) - 1) || !strncmp(sapi_module.name, SEASLOG_PHPDBG_KEY, sizeof(SEASLOG_PHPDBG_KEY) - 1))
    {
        request_uri = seaslog_request_query(SEASLOG_GLOBAL_VARS_SERVER, ZEND_STRL("SCRIPT_NAME") TSRMLS_CC);
        if (request_uri != NULL && IS_STRING == Z_TYPE_P(request_uri))
        {
            SEASLOG_G(request_variable)->request_uri_len = spprintf(&SEASLOG_G(request_variable)->request_uri, 0, "%s", Z_STRVAL_P(request_uri));
            SEASLOG_ZVAL_PTR_DTOR(request_uri);
        }

        request_method = seaslog_request_query(SEASLOG_GLOBAL_VARS_SERVER, ZEND_STRL("SHELL") TSRMLS_CC);
        if (request_method != NULL && IS_STRING == Z_TYPE_P(request_method))
        {
            SEASLOG_G(request_variable)->request_method_len = spprintf(&SEASLOG_G(request_variable)->request_method, 0, "%s", Z_STRVAL_P(request_method));
            SEASLOG_ZVAL_PTR_DTOR(request_method);
        }

        SEASLOG_G(request_variable)->domain_port_len = spprintf(&SEASLOG_G(request_variable)->domain_port, 0, "cli");
        SEASLOG_G(request_variable)->client_ip_len = spprintf(&SEASLOG_G(request_variable)->client_ip, 0, "local");
    }
    else
    {
        domain = seaslog_request_query(SEASLOG_GLOBAL_VARS_SERVER, ZEND_STRL("HTTP_HOST") TSRMLS_CC);
        if (domain != NULL && IS_STRING == Z_TYPE_P(domain))
        {
            SEASLOG_G(request_variable)->domain_port_len = spprintf(&SEASLOG_G(request_variable)->domain_port, 0, "%s", Z_STRVAL_P(domain));
            SEASLOG_ZVAL_PTR_DTOR(domain);
        }

        request_uri = seaslog_request_query(SEASLOG_GLOBAL_VARS_SERVER, ZEND_STRL("REQUEST_URI") TSRMLS_CC);
        if (request_uri != NULL && IS_STRING == Z_TYPE_P(request_uri))
        {
            SEASLOG_G(request_variable)->request_uri_len = spprintf(&SEASLOG_G(request_variable)->request_uri, 0, "%s", Z_STRVAL_P(request_uri));
            SEASLOG_ZVAL_PTR_DTOR(request_uri);
        }

        request_method = seaslog_request_query(SEASLOG_GLOBAL_VARS_SERVER, ZEND_STRL("REQUEST_METHOD") TSRMLS_CC);
        if (request_method != NULL && IS_STRING == Z_TYPE_P(request_method))
        {
            SEASLOG_G(request_variable)->request_method_len = spprintf(&SEASLOG_G(request_variable)->request_method, 0, "%s", Z_STRVAL_P(request_method));
            SEASLOG_ZVAL_PTR_DTOR(request_method);
        }

        client_ip = seaslog_request_query(SEASLOG_GLOBAL_VARS_SERVER, ZEND_STRL("HTTP_X_REAL_IP") TSRMLS_CC);
        if (client_ip != NULL && IS_STRING == Z_TYPE_P(client_ip))
        {
            SEASLOG_G(request_variable)->client_ip_len = spprintf(&SEASLOG_G(request_variable)->client_ip, 0, "%s", Z_STRVAL_P(client_ip));
            SEASLOG_ZVAL_PTR_DTOR(client_ip);
            return SUCCESS;
        }

        client_ip = seaslog_request_query(SEASLOG_GLOBAL_VARS_SERVER, ZEND_STRL("HTTP_X_FORWARDED_FOR") TSRMLS_CC);
        if (client_ip != NULL && IS_STRING == Z_TYPE_P(client_ip))
        {
            SEASLOG_G(request_variable)->client_ip_len = spprintf(&SEASLOG_G(request_variable)->client_ip, 0, "%s", Z_STRVAL_P(client_ip));
            SEASLOG_ZVAL_PTR_DTOR(client_ip);
            return SUCCESS;
        }

        client_ip = seaslog_request_query(SEASLOG_GLOBAL_VARS_SERVER, ZEND_STRL("REMOTE_ADDR") TSRMLS_CC);
        if (client_ip != NULL && IS_STRING == Z_TYPE_P(client_ip))
        {
            SEASLOG_G(request_variable)->client_ip_len = spprintf(&SEASLOG_G(request_variable)->client_ip, 0, "%s", Z_STRVAL_P(client_ip));
            SEASLOG_ZVAL_PTR_DTOR(client_ip);
            return SUCCESS;
        }
    }

    return SUCCESS;
}

void seaslog_clear_request_variable(TSRMLS_D)
{
    if(SEASLOG_G(request_variable)->request_uri)
    {
        efree(SEASLOG_G(request_variable)->request_uri);
    }

    if(SEASLOG_G(request_variable)->request_method)
    {
        efree(SEASLOG_G(request_variable)->request_method);
    }

    if(SEASLOG_G(request_variable)->domain_port)
    {
        efree(SEASLOG_G(request_variable)->domain_port);
    }

    if(SEASLOG_G(request_variable)->client_ip)
    {
        efree(SEASLOG_G(request_variable)->client_ip);
    }

    efree(SEASLOG_G(request_variable));
}

void get_code_filename_line(smart_str *result TSRMLS_DC)
{
    const char *ret;
    long code_line = 0;
    size_t filename_len;
    int recall_depth = SEASLOG_G(recall_depth);

#if PHP_VERSION_ID >= 70000
    zend_string *filename = NULL;
    if (SEASLOG_G(in_error) == 1)
    {
        ret = SEASLOG_G(in_error_filename);
        code_line = SEASLOG_G(in_error_lineno);
    }
    else
    {
        zend_execute_data *ptr = EG(current_execute_data);

        while(recall_depth >= 0)
        {
            if (ptr->prev_execute_data != NULL && ptr->prev_execute_data->func &&
                    ZEND_USER_CODE(ptr->prev_execute_data->func->common.type)
               )
            {
                ptr = ptr->prev_execute_data;
            }
            else
            {
                break;
            }
            recall_depth--;
        }
        if (ptr->func && ZEND_USER_CODE(ptr->func->type))
        {
            ret = ZSTR_VAL(ptr->func->op_array.filename);
            code_line = ptr->opline->lineno;
        }
    }

    filename = php_basename(ret, strlen(ret), NULL, 0);

    smart_str_appendl(result,ZSTR_VAL(filename),ZSTR_LEN(filename));
    smart_str_appendc(result,':');
    smart_str_append_long(result,(long)code_line);

    smart_str_0(result);
    zend_string_release(filename);
#else
    char *filename = NULL;

    if (SEASLOG_G(in_error) == 1)
    {
        ret = SEASLOG_G(in_error_filename);
        code_line = SEASLOG_G(in_error_lineno);
    }
    else
    {
        zend_execute_data *ptr = EG(current_execute_data);
        while(recall_depth > 0)
        {
            if (ptr->prev_execute_data && ptr->prev_execute_data->opline)
            {
                ptr = ptr->prev_execute_data;
            }
            else
            {
                break;
            }
            recall_depth--;
        }

        if (ptr->op_array)
        {
            ret = ptr->op_array->filename;
            code_line = ptr->opline->lineno;
        }
        else if (ptr->prev_execute_data && ptr->prev_execute_data->opline)
        {
            ret = ptr->op_array->filename;
            code_line = ptr->prev_execute_data->opline->lineno;
        }
    }

    php_basename(ret, strlen(ret), NULL, 0, &filename, &filename_len TSRMLS_CC);

    smart_str_appendl(result,filename,filename_len);
    smart_str_appendc(result,':');
    smart_str_append_long(result,(long)code_line);

    smart_str_0(result);
    efree(filename);

#endif
}

