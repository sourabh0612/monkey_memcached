/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Monkey HTTP Daemon
 *  ------------------
 *  Copyright (C) 2001-2012, Sourabh Chandak<sourabh3934@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>

#include "memcache.h"


memcached_st * memcached_connect(const char *server, int len, 
                                  duda_request_t *dr_web)
{
//    struct mk_list *list_memcached_fd;
//    duda_memcached_t *dr;
    memcached_st *memc = memcached(server, len);

    memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_NO_BLOCK, 1);
    memcached_behavior_set(memc, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL, 1);

    if (memc == NULL) {
        printf("MEMCACHED: Can't connect: %s\n", memcached_last_error_message(memc));
        exit(EXIT_FAILURE);
    }

    /* Will be used in case of event handling */
/*    dr = monkey->mem_alloc(sizeof(duda_memcached_t));
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
*/
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

void libmemcached_get(memcached_st * memc, const char * key, size_t * value_len, memcached_return_t * rc,
                        void * dr_web, void (*cb_read) (const memcached_st *, char *, void *))
{
    /*FIXME: Blocking Call used */
    
    char *value = memcached_get(memc, key, (size_t) strlen(key), value_len, NULL, rc);
    if(!value)
    {
        printf("%s\n", memcached_last_error_message(memc));
        memcached_free(memc);
        exit(1);
    }
    
    cb_read(memc, value, dr_web);
}

/* Will be used in case of event handling */
/*
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
*/
