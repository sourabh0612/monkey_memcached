/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include "webservice.h"
#include "packages/memcached/memcache.h"


DUDA_REGISTER("Service Example", "service");

duda_global_t my_data_mem;
duda_global_t my_data_empty;

/*
 *
 * URI Map example
 * +--------------------------------------------------------------+
 * |  Interface         Method     Param Name  Param Max Length   |
 * +--------------------------------------------------------------+
 * |  libmemcached     test                          0            |
 * +--------------------------------------------------------------+
 * |                   write_key    key_name         6            |
 * |                                key_value        6            |
 * +--------------------------------------------------------------+
 * |                   read_key     key_name         6            |
 * +--------------------------------------------------------------+
 *
 */

void cb_end(duda_request_t *dr)
{
    msg->info("my end callback");
}

void libmemcached_read(const memcached_st *memc, char *value, void *context)
{
    printf("Value:%s\n", value);
    memcached_st *temp_memc;
    temp_memc = memc;
    duda_request_t *dr;
    dr = (duda_request_t *)context;
    memcached_obj->disconnect(temp_memc);
    response->cont(dr);
    response->body_print(dr, "Memcached test successful\n", 22);
    response->end(dr, cb_end);   
}

void cb_test(duda_request_t *dr)
{
    const char *config_string = "--SERVER=127.0.0.1:12345";
    const char *key = "foo";
    const char *value = "bar";
    size_t val_len = 3;

    response->http_status(dr, 200);
    response->http_header(dr, "Content-Type: text/plain", 24);
    response->wait(dr);    
    
    memcached_st *memc = memcached_obj->connect(config_string, strlen(config_string), dr);
    memcached_return_t rc = memcached_obj->set(memc,key,value);

    if(rc != MEMCACHED_SUCCESS)
    {
        printf("%s\n", memcached_last_error_message(memc));
        exit(1);
    }
    memcached_obj->get(memc, key, &val_len, &rc, dr, libmemcached_read);
    
}

void *cb_global_mem()
{
    void *mem = monkey->mem_alloc(16);
    return mem;
}

int duda_main(struct duda_api_objects *api)
{
    duda_interface_t *if_system;
    duda_method_t    *method;
    duda_param_t *params;

    duda_service_init();

    session->init();

    duda_load_package(memcached_obj, "memcached");

    /* An empty global variable */
    duda_global_init(my_data_empty, NULL);

    /* A global variable with the value returned by the callback */
    duda_global_init(my_data_mem, cb_global_mem);

    /* archive interface */
    if_system = map->interface_new("libmemcached");

    method = map->method_new("test", "cb_test", 0);
    map->interface_add_method(method, if_system);
/*

  //  /* URI: /memcachedWS/memcache/write_key/key_name/key_value 
    method = map->method_new("write_key", "cb_write_key", 2);
    params = map->param_new("key_name", 6);
    map->method_add_param(params, method);
    params = map->param_new("key_value", 6);
    map->method_add_param(params, method);
    map->interface_add_method(method, if_system);

//    /* URI: /memcachedWS/memcache/read_key/key_name 
    method = map->method_new("read_key", "cb_read_key", 1);
    params = map->param_new("key_name", 6);
    map->method_add_param(params, method);
    map->interface_add_method(method, if_system);
*/
    /* Add interface to map */
    duda_service_add_interface(if_system);

    duda_service_ready();
}
