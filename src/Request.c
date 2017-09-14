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
| Author: Neeke.Gao  <neeke@php.net>                                   |
+----------------------------------------------------------------------+
*/

static void seaslog_init_host_name(TSRMLS_D)
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

static void seaslog_clear_host_name(TSRMLS_D)
{
    if (SEASLOG_G(host_name))
    {
        efree(SEASLOG_G(host_name));
    }
}

static void seaslog_init_pid(TSRMLS_D)
{
    SEASLOG_G(process_id_len) = spprintf(&SEASLOG_G(process_id),0, "%d", getpid());
}

static void seaslog_clear_pid(TSRMLS_D)
{
    if (SEASLOG_G(process_id))
    {
        efree(SEASLOG_G(process_id));
    }
}

static void seaslog_init_request_id(TSRMLS_D)
{
    SEASLOG_G(request_id) = get_uniqid();
    SEASLOG_G(request_id_len) = strlen(SEASLOG_G(request_id));
}

static void seaslog_clear_request_id(TSRMLS_D)
{
    if (SEASLOG_G(request_id))
    {
        efree(SEASLOG_G(request_id));
    }
}

static void seaslog_init_auto_globals(TSRMLS_D)
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
        zval *empty;
        MAKE_STD_ZVAL(empty);
        ZVAL_NULL(empty);
        return empty;
    }

    if (zend_hash_find(Z_ARRVAL_PP(carrier), name, len + 1, (void **)&ret) == FAILURE)
    {
        zval *empty;
        MAKE_STD_ZVAL(empty);
        ZVAL_NULL(empty);
        return empty;
    }

    Z_ADDREF_P(*ret);

    return *ret;

#endif
}

static int seaslog_init_request_variable(TSRMLS_D)
{
    zval *client_ip;
    zval *domain;

    SEASLOG_G(request_variable) = ecalloc(sizeof(request_variable_t), 1);

    if (!strncmp(sapi_module.name, "cli", sizeof("cli") - 1))
    {
        SEASLOG_G(request_variable)->request_uri = seaslog_request_query(SEASLOG_GLOBAL_VARS_SERVER, ZEND_STRL("SCRIPT_NAME") TSRMLS_CC);

        SEASLOG_G(request_variable)->request_method = seaslog_request_query(SEASLOG_GLOBAL_VARS_SERVER, ZEND_STRL("SUDO_COMMAND") TSRMLS_CC);

        SEASLOG_G(request_variable)->domain_port_len = spprintf(&SEASLOG_G(request_variable)->domain_port, 0, "cli");
        SEASLOG_G(request_variable)->client_ip_len = spprintf(&SEASLOG_G(request_variable)->client_ip, 0, "local");
    }
    else
    {
        domain = seaslog_request_query(SEASLOG_GLOBAL_VARS_SERVER, ZEND_STRL("HTTP_HOST") TSRMLS_CC);
        SEASLOG_G(request_variable)->domain_port = Z_STRVAL_P(domain);
        SEASLOG_G(request_variable)->domain_port_len = Z_STRLEN_P(domain);
        SEASLOG_ZVAL_PTR_DTOR(domain);

        SEASLOG_G(request_variable)->request_uri = seaslog_request_query(SEASLOG_GLOBAL_VARS_SERVER, ZEND_STRL("REQUEST_URI") TSRMLS_CC);

        SEASLOG_G(request_variable)->request_method = seaslog_request_query(SEASLOG_GLOBAL_VARS_SERVER, ZEND_STRL("REQUEST_METHOD") TSRMLS_CC);

        client_ip = seaslog_request_query(SEASLOG_GLOBAL_VARS_SERVER, ZEND_STRL("HTTP_X_REAL_IP") TSRMLS_CC);
        if (client_ip != NULL && Z_TYPE_P(client_ip) == IS_STRING)
        {
            SEASLOG_G(request_variable)->client_ip = Z_STRVAL_P(client_ip);
            SEASLOG_G(request_variable)->client_ip_len = Z_STRLEN_P(client_ip);
            SEASLOG_ZVAL_PTR_DTOR(client_ip);
            return SUCCESS;
        }

        client_ip = seaslog_request_query(SEASLOG_GLOBAL_VARS_SERVER, ZEND_STRL("HTTP_X_FORWARDED_FOR") TSRMLS_CC);
        if (client_ip != NULL && Z_TYPE_P(client_ip) == IS_STRING)
        {
            SEASLOG_G(request_variable)->client_ip = Z_STRVAL_P(client_ip);
            SEASLOG_G(request_variable)->client_ip_len = Z_STRLEN_P(client_ip);
            SEASLOG_ZVAL_PTR_DTOR(client_ip);
            return SUCCESS;
        }

        client_ip = seaslog_request_query(SEASLOG_GLOBAL_VARS_SERVER, ZEND_STRL("REMOTE_ADDR") TSRMLS_CC);
        if (client_ip != NULL && Z_TYPE_P(client_ip) == IS_STRING)
        {
            SEASLOG_G(request_variable)->client_ip = Z_STRVAL_P(client_ip);
            SEASLOG_G(request_variable)->client_ip_len = Z_STRLEN_P(client_ip);
            SEASLOG_ZVAL_PTR_DTOR(client_ip);
            return SUCCESS;
        }
    }

    return SUCCESS;
}

static void seaslog_clear_request_variable(TSRMLS_D)
{
    SEASLOG_ZVAL_PTR_DTOR(SEASLOG_G(request_variable)->request_uri);
    SEASLOG_ZVAL_PTR_DTOR(SEASLOG_G(request_variable)->request_method);
    efree(SEASLOG_G(request_variable)->domain_port);
    efree(SEASLOG_G(request_variable)->client_ip);
    efree(SEASLOG_G(request_variable));
}

static void get_code_filename_line(smart_str *result TSRMLS_DC)
{
    const char *ret;
    long code_line = 0;
    size_t filename_len;

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

        if ((!ptr->func || !ZEND_USER_CODE(ptr->func->common.type)) &&
                ptr->prev_execute_data->func &&
                ZEND_USER_CODE(ptr->prev_execute_data->func->common.type)
           )
        {
            ptr = ptr->prev_execute_data;
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