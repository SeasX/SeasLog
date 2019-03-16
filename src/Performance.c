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

#include "Performance.h"
#include "Common.h"
#include "Appender.h"
#include "ext/json/php_json.h"

#if PHP_VERSION_ID >= 70000
ZEND_DLEXPORT void (*_clone_zend_execute_ex) (zend_execute_data *execute_data TSRMLS_DC);
ZEND_DLEXPORT void (*_clone_zend_execute_internal) (zend_execute_data *execute_data, zval *return_value);

ZEND_DLEXPORT void seaslog_execute_ex (zend_execute_data *execute_data TSRMLS_DC);
ZEND_DLEXPORT void seaslog_execute_internal(zend_execute_data *execute_data, zval *return_value);

#elif PHP_VERSION_ID >= 50500
ZEND_DLEXPORT void (*_clone_zend_execute_ex) (zend_execute_data *execute_data TSRMLS_DC);
ZEND_DLEXPORT void (*_clone_zend_execute_internal) (zend_execute_data *data, struct _zend_fcall_info *fci, int ret TSRMLS_DC);

ZEND_DLEXPORT void seaslog_execute_ex (zend_execute_data *execute_data TSRMLS_DC);
ZEND_DLEXPORT void seaslog_execute_internal(zend_execute_data *execute_data, struct _zend_fcall_info *fci, int ret TSRMLS_DC);

#else
ZEND_DLEXPORT void (*_clone_zend_execute) (zend_op_array *ops TSRMLS_DC);
ZEND_DLEXPORT void (*_clone_zend_execute_internal) (zend_execute_data *data, int ret TSRMLS_DC);

ZEND_DLEXPORT void seaslog_execute (zend_op_array *ops TSRMLS_DC);
ZEND_DLEXPORT void seaslog_execute_internal(zend_execute_data *execute_data, int ret TSRMLS_DC);
#endif

#define SEASLOG_PERFORMANCE_BUCKET_FOREACH(bucket, _i) do { \
		for (_i = 0; _i < SEASLOG_PERFORMANCE_BUCKET_SLOTS; _i++) { \
            bucket = SEASLOG_G(performance_buckets)[_i]; \
            while (bucket) { \

#define SEASLOG_PERFORMANCE_BUCKET_FOREACH_END \
            }\
		} \
	} while (0)


static inline int seaslog_check_performance_sample(TSRMLS_D)
{
    if (SUCCESS == SEASLOG_G(trace_performance_sample_active))
    {
        return SUCCESS;
    }

    return FAILURE;
}

static inline int seaslog_process_performance_sample(TSRMLS_D)
{
    SEASLOG_G(trace_performance_sample_active) = FAILURE;

    srand(time(NULL));
    if (rand() % 1001 <= SEASLOG_G(trace_performance_sample_rate))
    {
        SEASLOG_G(trace_performance_sample_active) = SUCCESS;
        return SUCCESS;
    }

    return FAILURE;
}

static inline long hash_data(long hash, char *data, size_t size)
{
    size_t i;

    for (i = 0; i < size; ++i)
    {
        hash = hash * 33 + data[i];
    }

    return hash;
}

static inline long performance_microsecond(TSRMLS_D)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    long val;
    val = 1000000 * tv.tv_sec + tv.tv_usec;

    return val;
}

void seaslog_memory_usage(smart_str *buf TSRMLS_DC)
{
    long int usage = zend_memory_usage(0 TSRMLS_CC);
    smart_str_append_long(buf, usage);

    smart_str_0(buf);
}

void seaslog_peak_memory_usage(smart_str *buf TSRMLS_DC)
{
    long int usage = zend_memory_peak_usage(0 TSRMLS_CC);
    smart_str_append_long(buf, usage);

    smart_str_0(buf);
}

void init_zend_hooks(TSRMLS_D)
{
    if (SEASLOG_G(trace_performance))
    {
#if PHP_VERSION_ID < 50500
        _clone_zend_execute = zend_execute;
        zend_execute = seaslog_execute;
#else
        _clone_zend_execute_ex = zend_execute_ex;
        zend_execute_ex = seaslog_execute_ex;
#endif

        _clone_zend_execute_internal = zend_execute_internal;
        zend_execute_internal = seaslog_execute_internal;
    }
}

void recovery_zend_hooks(TSRMLS_D)
{
    if (SEASLOG_G(trace_performance))
    {
#if PHP_VERSION_ID < 50500
        zend_execute = _clone_zend_execute;
#else
        zend_execute_ex = _clone_zend_execute_ex;
#endif

        zend_execute_internal = _clone_zend_execute_internal;
    }
}

void seaslog_rinit_performance(TSRMLS_D)
{
    if (SEASLOG_G(trace_performance))
    {
        SEASLOG_G(stack_level) = 0;
        SEASLOG_G(trace_performance_active) = SUCCESS;
        SEASLOG_G(frame_free_list) = NULL;
        SEASLOG_G(performance_frames) = NULL;

        seaslog_process_performance_sample(TSRMLS_C);
        if (FAILURE == seaslog_check_performance_sample(TSRMLS_C))
        {
            return;
        }

        SEASLOG_G(performance_main) = (seaslog_performance_main *)emalloc(sizeof(seaslog_performance_main));
        SEASLOG_G(performance_main)->wt_start = performance_microsecond(TSRMLS_C);
        SEASLOG_G(performance_main)->mu_start = zend_memory_usage(0 TSRMLS_CC);
    }
}

void seaslog_clear_performance(zend_class_entry *ce TSRMLS_DC)
{
    if (SEASLOG_G(trace_performance))
    {
        if (FAILURE == seaslog_check_performance_sample(TSRMLS_C))
        {
            return;
        }

        SEASLOG_G(stack_level) = 0;
        seaslog_performance_free_the_free_list(TSRMLS_C);

        SEASLOG_G(performance_main)->wall_time = performance_microsecond(TSRMLS_C) - SEASLOG_G(performance_main)->wt_start;
        SEASLOG_G(performance_main)->memory = zend_memory_usage(0 TSRMLS_CC) - SEASLOG_G(performance_main)->mu_start;

        if (SEASLOG_G(performance_main)->wall_time >= SEASLOG_G(trace_performance_min_wall_time) * 1000)
        {
            process_seaslog_performance_log(ce TSRMLS_CC);
        }
        else
        {
            process_seaslog_performance_clear(TSRMLS_C);
        }

        efree(SEASLOG_G(performance_main));
        SEASLOG_G(performance_frames) = NULL;

        SEASLOG_G(trace_performance_active) = FAILURE;
    }
}

int seaslog_check_performance_active(TSRMLS_D)
{
    if (SEASLOG_G(trace_performance) && SUCCESS == SEASLOG_G(trace_performance_active))
    {
        return SUCCESS;
    }

    return FAILURE;
}

#if PHP_VERSION_ID >= 50500
ZEND_DLEXPORT void seaslog_execute_ex (zend_execute_data *execute_data TSRMLS_DC)
#else
ZEND_DLEXPORT void seaslog_execute (zend_op_array *ops TSRMLS_DC)
#endif
{
    int is_tracing = FAILURE;

#if PHP_VERSION_ID < 50500
    zend_execute_data  *execute_data = EG(current_execute_data);
#endif

    zend_execute_data *real_execute_data = execute_data;
    is_tracing = performance_frame_begin(real_execute_data TSRMLS_CC);

#if PHP_VERSION_ID >= 50500
    _clone_zend_execute_ex(execute_data TSRMLS_CC);
#else
    _clone_zend_execute(ops TSRMLS_CC);
#endif

    if (SUCCESS == is_tracing)
    {
        performance_frame_end(TSRMLS_C);
    }
    else if (SEASLOG_CONTINUE == is_tracing)
    {
        SEASLOG_G(stack_level) -= 1;
    }
}


#if PHP_VERSION_ID >= 70000
ZEND_DLEXPORT void seaslog_execute_internal(zend_execute_data *execute_data, zval *ret)
#elif PHP_VERSION_ID >= 50500
ZEND_DLEXPORT void seaslog_execute_internal(zend_execute_data *execute_data, struct _zend_fcall_info *zfi, int ret TSRMLS_DC)
#else
ZEND_DLEXPORT void seaslog_execute_internal(zend_execute_data *execute_data, int ret TSRMLS_DC)
#endif
{

    int is_tracing = FAILURE;

    zend_execute_data *real_execute_data = execute_data;
    is_tracing = performance_frame_begin(real_execute_data TSRMLS_CC);


#if PHP_VERSION_ID >= 70000
    if (_clone_zend_execute_internal)
    {
        _clone_zend_execute_internal(execute_data, ret TSRMLS_CC);
    }
    else
    {
        execute_internal(execute_data, ret TSRMLS_CC);
    }
#elif PHP_VERSION_ID >= 50500
    if (_clone_zend_execute_internal)
    {
        _clone_zend_execute_internal(execute_data, zfi, ret TSRMLS_CC);
    }
    else
    {
        execute_internal(execute_data, zfi, ret TSRMLS_CC);
    }
#else
    if (_clone_zend_execute_internal)
    {
        _clone_zend_execute_internal(execute_data, ret TSRMLS_CC);
    }
    else
    {
        execute_internal(execute_data, ret TSRMLS_CC);
    }
#endif

    if (SUCCESS == is_tracing)
    {
        performance_frame_end(TSRMLS_C);
    }
    else if (SEASLOG_CONTINUE == is_tracing)
    {
        SEASLOG_G(stack_level) -= 1;
    }
}

int performance_frame_begin(zend_execute_data *execute_data TSRMLS_DC)
{
    char *function;
    seaslog_frame *current_frame;
    seaslog_frame *p;
    int recurse_level = 0;
    int stack_level = 0;

    if (FAILURE == seaslog_check_performance_sample(TSRMLS_C))
    {
        return FAILURE;
    }

    if (FAILURE == seaslog_check_performance_active(TSRMLS_C))
    {
        return FAILURE;
    }

    function = seaslog_performance_get_function_name(execute_data TSRMLS_CC);
    if (NULL == function)
    {
        return FAILURE;
    }

    SEASLOG_G(stack_level) += 1;

    if (SEASLOG_G(stack_level) - SEASLOG_G(trace_performance_start_depth) < 0)
    {
        efree(function);
        return SEASLOG_CONTINUE;
    }

    current_frame = seaslog_performance_fast_alloc_frame(TSRMLS_C);
    current_frame->class_name = seaslog_performance_get_class_name(execute_data TSRMLS_CC);
    current_frame->function_name = function;
    current_frame->previous_frame = SEASLOG_G(performance_frames);
    current_frame->wt_start = performance_microsecond(TSRMLS_C);
    current_frame->mu_start = zend_memory_usage(0 TSRMLS_CC);
    current_frame->hash_code = zend_inline_hash_func(function,strlen(function)+1) % SEASLOG_PERFORMANCE_COUNTER_SIZE;

    stack_level = SEASLOG_G(stack_level) - SEASLOG_G(trace_performance_start_depth) + 1;
    if (SEASLOG_G(function_hash_counters)[current_frame->hash_code] > 0)
    {
        for(p = current_frame->previous_frame; p; p = p->previous_frame)
        {
            if (NULL == p->function_name)
            {
                break;
            }

            if (!strcmp(current_frame->function_name,p->function_name) &&
                    ((current_frame->class_name
                      && p->class_name
                      && !strcmp(current_frame->class_name,p->class_name)
                     ) || (
                         NULL == current_frame->class_name
                         && NULL == p->class_name
                     ))
               )
            {
                recurse_level = (p->recurse_level) + 1;
                stack_level = p->stack_level;
                break;
            }
        }
    }
    SEASLOG_G(function_hash_counters)[current_frame->hash_code]++;

    current_frame->recurse_level = recurse_level;
    current_frame->stack_level = stack_level;

    SEASLOG_G(performance_frames) = current_frame;

    return SUCCESS;
}

static inline void seaslog_performance_bucket_process(seaslog_frame* current_frame TSRMLS_DC)
{
    zend_ulong bucket_key = current_frame->hash_code + current_frame->stack_level;
    unsigned int slot = (unsigned int)bucket_key % SEASLOG_PERFORMANCE_BUCKET_SLOTS;
    seaslog_performance_bucket *bucket = SEASLOG_G(performance_buckets)[slot];
    long duration = performance_microsecond(TSRMLS_C) - current_frame->wt_start;

    while (bucket)
    {
        if (bucket->bucket_key == bucket_key &&
                bucket->hash_code == current_frame->hash_code &&
                bucket->stack_level == current_frame->stack_level &&
                !strcmp(bucket->function_name, current_frame->function_name) &&
                ((
                     bucket->class_name
                     && current_frame->class_name
                     && !strcmp(bucket->class_name, current_frame->class_name)
                 ) || (
                     NULL == bucket->class_name
                     && NULL == current_frame->class_name
                 ))
           )
        {

            break;
        }

        bucket = bucket->next;
    }

    if (NULL == bucket)
    {
        bucket = emalloc(sizeof(seaslog_performance_bucket));
        bucket->bucket_key = bucket_key;
        bucket->hash_code = current_frame->hash_code;
        bucket->stack_level = current_frame->stack_level;
        bucket->class_name = current_frame->class_name ? estrdup(current_frame->class_name) : NULL;
        bucket->function_name = estrdup(current_frame->function_name);
        bucket->count = 0;
        bucket->wall_time = 0;
        bucket->memory = 0;
        bucket->next = SEASLOG_G(performance_buckets)[slot];

        SEASLOG_G(performance_buckets)[slot] = bucket;
    }

    bucket->count++;
    bucket->wall_time += duration;
    bucket->memory += (zend_memory_usage(0 TSRMLS_CC) - current_frame->mu_start);
}

void performance_frame_end(TSRMLS_D)
{
    seaslog_frame *current_frame = SEASLOG_G(performance_frames);
    seaslog_frame *previous_frame = current_frame->previous_frame;

    seaslog_performance_bucket_process(current_frame TSRMLS_CC);

    SEASLOG_G(stack_level) -= 1;
    SEASLOG_G(function_hash_counters)[current_frame->hash_code]--;

    SEASLOG_G(performance_frames) = SEASLOG_G(performance_frames)->previous_frame;
    seaslog_performance_fast_free_frame(current_frame TSRMLS_CC);
}

void seaslog_performance_bucket_free(seaslog_performance_bucket *bucket TSRMLS_DC)
{
    if (bucket->class_name)
    {
        efree(bucket->class_name);
    }

    if (bucket->function_name)
    {
        efree(bucket->function_name);
    }

    efree(bucket);
}

seaslog_frame* seaslog_performance_fast_alloc_frame(TSRMLS_D)
{
    seaslog_frame *p;

    p = SEASLOG_G(frame_free_list);

    if (p)
    {
        SEASLOG_G(frame_free_list) = p->previous_frame;
        p->function_name = NULL;
        p->class_name = NULL;
        return p;
    }
    else
    {
        p = (seaslog_frame *)emalloc(sizeof(seaslog_frame));
        p->function_name = NULL;
        p->class_name = NULL;
        return p;
    }
}

void seaslog_performance_fast_free_frame(seaslog_frame *p TSRMLS_DC)
{
    if (p->function_name != NULL)
    {
        efree(p->function_name);
    }
    if (p->class_name != NULL)
    {
        efree(p->class_name);
    }

    p->previous_frame = SEASLOG_G(frame_free_list);
    SEASLOG_G(frame_free_list) = p;
}

void seaslog_performance_free_the_free_list(TSRMLS_D)
{
    seaslog_frame *frame = SEASLOG_G(frame_free_list);
    seaslog_frame *current;

    while (frame)
    {
        current = frame;
        frame = frame->previous_frame;
        efree(current);
    }
}

char* seaslog_performance_get_class_name(zend_execute_data *data TSRMLS_DC)
{
    zend_function *curr_func;

    if (!data)
    {
        return NULL;
    }

#if PHP_VERSION_ID >= 70000
    curr_func = data->func;
#else
    curr_func = data->function_state.function;
#endif

    if (NULL != curr_func->common.scope)
    {
        return STR_NAME_VAL(curr_func->common.scope->name);
    }

    return NULL;
}

char* seaslog_performance_get_function_name(zend_execute_data *data TSRMLS_DC)
{
    zend_function *curr_func;

    if (!data)
    {
        return NULL;
    }

#if PHP_VERSION_ID >= 70000
    curr_func = data->func;
#else
    curr_func = data->function_state.function;
#endif

    if (!curr_func->common.function_name || ZEND_USER_FUNCTION != curr_func->common.type)
    {
        return NULL;
    }

    return STR_NAME_VAL(curr_func->common.function_name);
}

int process_seaslog_performance_log(zend_class_entry *ce TSRMLS_DC)
{
    int i,j,m,n,r,stack_level = 0;
    seaslog_performance_bucket *bucket;
    seaslog_performance_result *result_forward;
    smart_str performance_log = {0};
    int trace_performance_min_function_wall_time = 0;
    seaslog_performance_result*** result_array;
    int have_performance_log_item = FAILURE;

#if PHP_VERSION_ID >= 70000
    zval performance_log_array;
    zval performance_log_item;
    zval performance_log_level_item;
#else
    zval *performance_log_array;
    zval *performance_log_item;
    zval *performance_log_level_item;
#endif

    trace_performance_min_function_wall_time = SEASLOG_G(trace_performance_min_function_wall_time) * 1000;

    result_array =(seaslog_performance_result ***) emalloc(SEASLOG_G(trace_performance_max_depth) * sizeof(seaslog_performance_result *) );

    for (m = 0; m < SEASLOG_G(trace_performance_max_depth); m++)
    {
        result_array[m] = (seaslog_performance_result **)emalloc(SEASLOG_G(trace_performance_max_functions_per_depth) * sizeof(seaslog_performance_result));

        for (n = 0; n < SEASLOG_G(trace_performance_max_functions_per_depth); n++)
        {
            result_array[m][n] = (seaslog_performance_result *)emalloc(sizeof(seaslog_performance_result));
            result_array[m][n]->hash_code = 0;
            result_array[m][n]->wall_time = 0;
        }
    }

    SEASLOG_PERFORMANCE_BUCKET_FOREACH(bucket, i)
    SEASLOG_G(performance_buckets)[i] = bucket->next;

    if (bucket->stack_level <= SEASLOG_G(trace_performance_max_depth) && bucket->wall_time >= trace_performance_min_function_wall_time)
    {
        stack_level = bucket->stack_level - 1;

        for (n = 0; n < SEASLOG_G(trace_performance_max_functions_per_depth); n++)
        {
            if (result_array[stack_level][n]->hash_code == 0 && n == 0)
            {
                result_array[stack_level][n]->hash_code = bucket->hash_code;
                result_array[stack_level][n]->wall_time = bucket->wall_time;
                result_array[stack_level][n]->count = bucket->count;
                result_array[stack_level][n]->memory = bucket->memory;

                if (NULL == bucket->class_name)
                {
                    spprintf(&result_array[stack_level][n]->function,0,"%s",bucket->function_name);
                }
                else
                {
                    spprintf(&result_array[stack_level][n]->function,0,"%s::%s",bucket->class_name,bucket->function_name);
                }
                break;
            }
            else
            {
                if (result_array[stack_level][n]->wall_time >= bucket->wall_time)
                {
                    continue;
                }
                else
                {
                    for (r = SEASLOG_G(trace_performance_max_functions_per_depth) - 1; r > n; r--)
                    {
                        if (result_array[stack_level][r-1]->hash_code == 0 && result_array[stack_level][r-1]->wall_time == 0)
                        {
                            continue;
                        }

                        result_forward = result_array[stack_level][r];
                        result_array[stack_level][r] = result_array[stack_level][r-1];
                        result_array[stack_level][r-1] = result_forward;
                    }

                    if (result_array[stack_level][n]->hash_code > 0)
                    {
                        efree(result_array[stack_level][n]->function);
                    }

                    result_array[stack_level][n]->hash_code = bucket->hash_code;
                    result_array[stack_level][n]->wall_time = bucket->wall_time;
                    result_array[stack_level][n]->count = bucket->count;
                    result_array[stack_level][n]->memory = bucket->memory;
                    if (NULL == bucket->class_name)
                    {
                        spprintf(&result_array[stack_level][n]->function,0,"%s",bucket->function_name);
                    }
                    else
                    {
                        spprintf(&result_array[stack_level][n]->function,0,"%s::%s",bucket->class_name,bucket->function_name);
                    }
                    break;
                }
            }
        }
    }
    seaslog_performance_bucket_free(bucket TSRMLS_CC);
    bucket = SEASLOG_G(performance_buckets)[i];
    SEASLOG_PERFORMANCE_BUCKET_FOREACH_END;

    SEASLOG_ARRAY_INIT(performance_log_array);
    SEASLOG_ARRAY_INIT(performance_log_level_item);
    SEASLOG_ADD_ASSOC_DOUBLE_EX(performance_log_level_item, SEASLOG_STRS("wt"), SEASLOG_G(performance_main)->wall_time / 1000);
    SEASLOG_ADD_ASSOC_LONG_EX(performance_log_level_item, SEASLOG_STRS("mu"), SEASLOG_G(performance_main)->memory);
    SEASLOG_ADD_ASSOC_ZVAL_EX_EX(performance_log_array, SEASLOG_STRS(SEASLOG_PERFORMANCE_ROOT_SYMBOL), performance_log_level_item);

    for (m = 0; m < SEASLOG_G(trace_performance_max_depth); m++)
    {
        have_performance_log_item = FAILURE;
        SEASLOG_ARRAY_INIT(performance_log_level_item);
        for (n = 0; n < SEASLOG_G(trace_performance_max_functions_per_depth); n++)
        {
            if (result_array[m][n]->hash_code > 0)
            {
                have_performance_log_item = SUCCESS;
                SEASLOG_ARRAY_INIT(performance_log_item);
                SEASLOG_ADD_ASSOC_STRING_EX(performance_log_item, SEASLOG_STRS("cm"), result_array[m][n]->function);
                SEASLOG_ADD_ASSOC_LONG_EX(performance_log_item, SEASLOG_STRS("ct"), result_array[m][n]->count);
                SEASLOG_ADD_ASSOC_DOUBLE_EX(performance_log_item, SEASLOG_STRS("wt"), result_array[m][n]->wall_time / 1000);
                SEASLOG_ADD_ASSOC_LONG_EX(performance_log_item, SEASLOG_STRS("mu"), result_array[m][n]->memory);
                SEASLOG_ADD_NEXT_ZVAL(performance_log_level_item, performance_log_item);
                efree(result_array[m][n]->function);
            }
            efree(result_array[m][n]);
        }

        efree(result_array[m]);
        if (SUCCESS == have_performance_log_item)
        {
            SEASLOG_ADD_INDEX_ZVAL(performance_log_array, m + SEASLOG_G(trace_performance_start_depth), performance_log_level_item);
        }
        else
        {
            SEASLOG_ARRAY_DESTROY(performance_log_level_item);
        }
    }
    efree(result_array);

//    php_var_dump(&performance_log_array,1 TSRMLS_CC);

    SEASLOG_JSON_ENCODE(&performance_log, performance_log_array, 0);
    smart_str_0(&performance_log);

    seaslog_log_ex(3, SEASLOG_INFO, SEASLOG_INFO_INT, SEASLOG_SMART_STR_C(performance_log), seaslog_smart_str_get_len(performance_log), SEASLOG_PERFORMANCE_LOGGER, strlen(SEASLOG_PERFORMANCE_LOGGER)+1, ce TSRMLS_CC);
    smart_str_free(&performance_log);

    SEASLOG_ARRAY_DESTROY(performance_log_array);

    return SUCCESS;
}

int process_seaslog_performance_clear(TSRMLS_D)
{
    int i;
    seaslog_performance_bucket *bucket;

    SEASLOG_PERFORMANCE_BUCKET_FOREACH(bucket, i)
    SEASLOG_G(performance_buckets)[i] = bucket->next;
    seaslog_performance_bucket_free(bucket TSRMLS_CC);
    bucket = SEASLOG_G(performance_buckets)[i];
    SEASLOG_PERFORMANCE_BUCKET_FOREACH_END;

    return SUCCESS;
}
