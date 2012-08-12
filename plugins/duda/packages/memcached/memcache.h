#ifndef DUDA_PACKAGE_REDIS_H
#define DUDA_PACKAGE_REDIS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "libmemcached/memcached.h"
//#include "libmemcached/memcached_pool.h"

#include "duda_api.h"
#include "webservice.h"

pthread_key_t memcached_key;

typedef struct duda_memcached {
    
    memcached_st *memc;
    duda_request_t *dr;
    struct mk_list _head_memcached_fd;
    
} duda_memcached_t;

struct duda_api_memcached {

    /* memcached functions */
    memcached_st *(*connect) (const char *, int, 
                                   duda_request_t *);
    void (*disconnect) (memcached_st *);
    memcached_return_t (*get) (memcached_st *, const char *, 
                               memcached_return_t (*)(const memcached_st *ptr, 
                                                      memcached_result_st *result,
                                                      void *context), 
                               void *);
    memcached_return_t (*set)(memcached_st *, const char *, const char *);
    duda_request_t * (*getDudarequest) (const memcached_st *);
};

typedef struct duda_api_memcached memcached_object_t;

memcached_object_t *memcached_obj;

memcached_st * memcached_connect(const char *server, int len, duda_request_t *dr);
void memcached_disconnect(memcached_st *memc);

memcached_return_t libmemcached_set(memcached_st * memc, const char * key, const char * value);
memcached_return_t libmemcached_get(memcached_st * memc, const char * key, memcached_execute_fn * callback, void *);

int memcached_init();

duda_request_t * memcached_request_map(const memcached_st *memc);


#endif
