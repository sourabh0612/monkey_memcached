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
 * |  redis            version                       0            |
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
/*
void connectCallback(const redisAsyncContext *c, int status) {
    if (status != REDIS_OK) {
        printf("Error: %s\n", c->errstr);
        exit(1);
        return;
    }
    printf("Connected...\n");
}

void disconnectCallback(const redisAsyncContext *c, int status) {
    if (status != REDIS_OK) {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Disconnected...\n");
    duda_request_t *dr = redis->getDudarequest(c);
    response->cont(dr);
    response->body_print(dr, "Redis test successful\n", 22);
    response->end(dr, cb_end);
}

void versionCallback(redisAsyncContext *c, void *r, void *privdata) {
    redisReply *reply = r;
    if (reply == NULL) return;
    
    const char *field = "redis_version:";
    char *p, *eptr;
    int major, minor;

    p = strstr(reply->str,field);
    major = strtol(p+strlen(field),&eptr,10);
    p = eptr+1;
    minor = strtol(p,&eptr,10);
    
    printf("Version:%d.%d\n",major,minor);
    redis->disconnect(c);
}

void getCallback(redisAsyncContext *c, void *r, void *privdata) {
    redisReply *reply = r;
    if (reply == NULL) return;
    
    printf("%s: %s\n", (char*)privdata, reply->str);

    redis->disconnect(c);
}

void setCallback(redisAsyncContext *c, void *r, void *privdata) {
    redisReply *reply = r;
    if (reply == NULL) return;
    
    printf("%s\n", reply->str);

    redis->disconnect(c);

}
*/

memcached_return_t cb_read(const memcached_st *ptr, memcached_result_st *result, void *context)
{
    printf("value:%s\n",memcached_result_key_value(result));
    memcached_st *memc;
    memc = ptr;
    duda_request_t *dr;
    dr = (duda_request_t *)context;
    memcached_obj->disconnect(memc);
    response->cont(dr);
    response->body_print(dr, "Memcached test successful\n", 22);
    response->end(dr, cb_end);   
}

void cb_version(duda_request_t *dr)
{
    const char *config_string = "--SERVER=127.0.0.1:12345";
    const char *key = "sourabh";
    const char *value = "neo";

    response->http_status(dr, 200);
    response->http_header(dr, "Content-Type: text/plain", 24);
    response->wait(dr);    
    
    memcached_st *memc = memcached_obj->connect(config_string, strlen(config_string), dr);
    
    memcached_return_t rc = memcached_obj->set(memc,key,value);

    if(rc != MEMCACHED_SUCCESS)
    {
        printf("falied\n");
        exit(1);
    }

//    rc = memcached_obj->get(memc, key, cb_read, dr);
    memcached_obj->disconnect(memc);
    response->cont(dr);
    response->body_print(dr, "Memcached test successful\n", 22);
    response->end(dr, cb_end);   

}
/*
void cb_read_key(duda_request_t *dr)
{
    char *key;
    const char *config_string = "--SERVER=127.0.0.1:12345";
    response->http_status(dr, 200);
    response->http_header(dr, "Content-Type: text/plain", 24);
    response->wait(dr);

    memcached_st * memc = memcached->connect(config_string, strlen(config_string), dr);

    memcached->disconnect(memc);
}

void cb_write_key(duda_request_t *dr)
{
    char *key,*value;
    response->http_status(dr, 200);
    response->http_header(dr, "Content-Type: text/plain", 24);
    
    redisAsyncContext *rc = redis->connect("127.0.0.1", 6379, dr);
    response->wait(dr);
    
    redis->attach(rc,dr);
    redis->setConnectCallback(rc,connectCallback);
    redis->setDisconnectCallback(rc, disconnectCallback);
    key = param->get(dr, 0);
    value = param->get(dr,1);
    redis->command(rc, setCallback, NULL, "SET %b %b", key, strlen(key), value, strlen(value));

}
*/
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

    method = map->method_new("version", "cb_version", 0);
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
