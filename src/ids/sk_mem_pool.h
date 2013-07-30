
/*
 * Copyright(C)	xingke.zhang
 * Copyright(C) cs2c, Inc
 */

#ifndef _MEMPOOL_H
#define _MEMPOOL_H

#include "sk_sdlist.h"

typedef unsigned int POOL_COUNT;

typedef struct _mem_bucket_t
{
    sdlist_item_t *key;
    int used;
    void *data;
	long len;
} mem_bucket_t;

typedef struct _mempool_t
{
    void **datapool; /* memory buffer for mem_bucket_t->data */

    mem_bucket_t *bucketpool; /* memory buffer */

    sdlist_item_t *listpool; /* list of things to use for memory bufs */

    POOL_COUNT free; /*  free block count */
    POOL_COUNT used;  /* used block count */

    POOL_COUNT total;

    sf_sdlist_t free_list;
    sf_sdlist_t used_list;

    size_t obj_size;
} mempool_t;

int mempool_init(mempool_t *mempool, POOL_COUNT num_objects, size_t obj_size);
int mempool_destroy(mempool_t *mempool);
mem_bucket_t *mempool_alloc(mempool_t *mempool);
void mempool_free(mempool_t *mempool, mem_bucket_t *obj);


#endif /* _MEMPOOL_H */

