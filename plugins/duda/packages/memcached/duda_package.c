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

#include "duda_api.h"
#include "duda_package.h"
#include "memcache.h"


struct duda_api_memcached *get_memcached_api()
{
    struct duda_api_memcached *memcached_obj;

    /* Alloc object */
    memcached_obj = malloc(sizeof(struct duda_api_memcached));

    /* Map API calls */
    memcached_obj->connect           = memcached_connect;
    memcached_obj->disconnect        = memcached_disconnect;  
    memcached_obj->get               = libmemcached_get;
    memcached_obj->set               = libmemcached_set;
    //memcached_obj->getDudarequest    = memcached_request_map;
    
    return memcached_obj;
}

duda_package_t *duda_package_main(struct duda_api_objects *api)
{
    duda_package_t *dpkg;

    /* Initialize package internals */
    duda_package_init();

    /* Init redis*/
    memcached_init();

    /* Package object */    
    dpkg = monkey->mem_alloc(sizeof(duda_package_t));
    dpkg->name    = "memcached";
    dpkg->version = "0.1";
    dpkg->api     = get_memcached_api();

    return dpkg;
}
