#include <stdio.h>
#include <stdlib.h>

#include "memcache.h"


memcached_st * memcached_connect(const char *server, int len, 
                                  duda_request_t *dr_web)
{
    struct mk_list *list_memcached_fd;
    duda_memcached_t *dr;
    memcached_st *memc = memcached(server, len);

    memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, 1);

    if (memc == NULL) {
        printf("MEMCACHED: Can't connect: %s\n", memcached_last_error_message(memc));
        exit(EXIT_FAILURE);
    }
    dr = monkey->mem_alloc(sizeof(duda_memcached_t));
    dr->memc = memc;
    dr->dr = dr_web;
    list_memcached_fd = pthread_getspecific(memcached_key);
    if(list_memcached_fd == NULL)
    {
        list_memcached_fd = malloc(sizeof(struct mk_list));
        mk_list_init(list_memcached_fd);
        pthread_setspecific(memcached_key, (void *) list_memcached_fd);    
    }

    mk_list_add(&dr->_head_memcached_fd, list_memcached_fd);
    return memc;
}

void memcached_disconnect(memcached_st *memc)
{
    memcached_free(memc);
}

int memcached_init()
{
    pthread_key_create(&memcached_key, NULL);
    
    return 1;
}

memcached_return_t libmemcached_set(memcached_st * memc, const char * key, const char * value)
{
    return memcached_set(memc, key, strlen(key), value, strlen(value), (time_t)0, MEMCACHED_BEHAVIOR_NO_BLOCK);
}

memcached_return_t libmemcached_get(memcached_st * memc, const char * key, memcached_execute_fn * callback, void * dr_web)
{
    return memcached_mget_execute(memc, key, strlen(key), sizeof(key), callback, dr_web, (uint32_t)1);
}

duda_request_t * memcached_request_map(const memcached_st *memc)
{
    struct mk_list *list_memcached_fd,*head,*tmp;
    duda_request_t *dr;
    duda_memcached_t *dr_entry;
    list_memcached_fd = pthread_getspecific(memcached_key);

    mk_list_foreach_safe(head, tmp, list_memcached_fd) {
        dr_entry = mk_list_entry(head, duda_memcached_t, _head_memcached_fd);
        if(dr_entry->memc == memc) {
            dr = dr_entry->dr;
            mk_list_del(&dr_entry->_head_memcached_fd);
            free(dr_entry);
            return dr;
        }
    }
    return NULL;
}
