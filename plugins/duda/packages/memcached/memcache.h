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

#ifndef DUDA_PACKAGE_REDIS_H
#define DUDA_PACKAGE_REDIS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "libmemcached/memcached.h"

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
    void (*get) (memcached_st *, const char *, size_t *,
                   memcached_return_t *, void *, 
                   void (*) (const memcached_st *, char *, void *));

    memcached_return_t (*set)(memcached_st *, const char *, const char *);
    //duda_request_t * (*getDudarequest) (const memcached_st *);
};

typedef struct duda_api_memcached memcached_object_t;

memcached_object_t *memcached_obj;

memcached_st * memcached_connect(const char *server, int len, duda_request_t *dr);
void memcached_disconnect(memcached_st *memc);

memcached_return_t libmemcached_set(memcached_st * memc, const char * key, const char * value);
void libmemcached_get(memcached_st * memc, const char * key, size_t * value_len, memcached_return_t * rc,
                        void * dr_web, void (*cb_read) (const memcached_st *, char *, void *));

int memcached_init();

//duda_request_t * memcached_request_map(const memcached_st *memc);


#endif
