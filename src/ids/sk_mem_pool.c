

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <strings.h>

#include "sk_sdlist.h"
#ifdef TEST_MEMPOOL_MAIN
#include "sk_ids.h"
#include "sk_mempool_list.h"
#endif

/*
 * This is used to allocate a list of fixed size objects in a static
 * memory pool aside from the concerns of alloc/free
 */

/* $Id: mempool.c,v 1.7 2004/03/23 15:34:45 chris_reid Exp $ */
#include "sk_mem_pool.h"

/* Function: int mempool_init(mempool_t *mempool,
 *                            POOL_COUNT num_objects, size_t obj_size)
 *
 * Purpose: initialize a mempool object and allocate memory for it
 * Args: mempool - pointer to a mempool_t struct
 *       num_objects - number of items in this pool
 *       obj_size    - size of the items
 *
 * Returns: 0 on success, -1 on failure
 */

int mempool_init(mempool_t *mempool, POOL_COUNT num_objects, size_t obj_size)
{
    POOL_COUNT i;

    if(mempool == NULL)
        return -1;

    if(num_objects < 1)
        return -1;

    if(obj_size < 1)
        return -1;

    mempool->obj_size = obj_size;

    /* this is the basis pool that represents all the *data pointers
       in the list */
    mempool->datapool = calloc(num_objects, obj_size);


    if(mempool->datapool == NULL)
    {
        return -1;
    }

    mempool->listpool = calloc(num_objects, sizeof(sdlist_item_t));

    if(mempool->listpool == NULL)
    {
        /* well, that sucked, lets clean up */
        fprintf(stderr, "mempool: listpool is null\n");
        free(mempool->datapool);
        return -1;
    }

    mempool->bucketpool = calloc(num_objects, sizeof(mem_bucket_t));

    if(mempool->bucketpool == NULL)
    {
        fprintf(stderr, "mempool: bucketpool is null\n");
        free(mempool->datapool);
        free(mempool->listpool);
        return -1;
    }

    /* sets up the 2 memory lists */
    if(sf_sdlist_init(&mempool->used_list, NULL))  //sf_sdlist_free)) modify by zhangxingke, 2013.05.07
    {
        fprintf(stderr, "mempool: used_list failed\n");
        free(mempool->datapool);
        free(mempool->listpool);
        free(mempool->bucketpool);
        return -1;
    }

    if(sf_sdlist_init(&mempool->free_list, NULL))
    {
        fprintf(stderr, "mempool: free_list failed\n");
        free(mempool->datapool);
        free(mempool->listpool);
        free(mempool->bucketpool);
        return -1;
    }


    for(i=0; i<num_objects; i++)
    {
        sdlist_item_t *itemp;
        mem_bucket_t *bp;

        bp = &mempool->bucketpool[i];
        itemp = &mempool->listpool[i];

        /* each bucket knows where it resides in the list */
        bp->key = itemp;

#ifdef TEST_MEMPOOL
        printf("listpool: %p itemp: %p diff: %u\n",
               mempool->listpool, itemp,
               (((char *) itemp) -
                ((char *) mempool->listpool)));
#endif /* TEST_MEMPOOL */

        bp->data = ((char *) mempool->datapool) + (i * mempool->obj_size);

#ifdef TEST_MEMPOOL
        printf("datapool: %p bp.data: %p diff: %u\n",
               mempool->datapool,
               mempool->datapool + (i * mempool->obj_size),
               (((char *) bp->data) -
                ((char *) mempool->datapool)));
#endif /* TEST_MEMPOOL */


        if(sf_sdlist_append(&mempool->free_list,
                            &mempool->bucketpool[i],
                            &mempool->listpool[i]))
        {
            fprintf(stderr, "mempool: free_list_append failed\n");
            free(mempool->datapool);
            free(mempool->listpool);
            free(mempool->bucketpool);
            return -1;
        }

        mempool->free++;
    }

    mempool->used = 0;
    mempool->total = num_objects;

    return 0;
}

/* Function: int mempool_destroy(mempool_t *mempool)
 *
 * Purpose: destroy a set of mempool objects
 * Args: mempool - pointer to a mempool_t struct
 *
 * Returns: 0 on success, -1 on failure
 */
int mempool_destroy(mempool_t *mempool)
{
    if(mempool == NULL)
        return -1;

	//2012.05.08 add by xingke.zhang
	free(mempool->datapool);		//free掉这个内存池,但是否真的free掉了，不知道，稍后再看
	free(mempool->bucketpool);		//free掉bucketpool

    free(mempool->listpool);

    /* TBD - callback to free up every stray pointer */
    bzero(mempool, sizeof(mempool_t));

    return 0;
}


/* Function: mem_bucket_t *mempool_alloc(mempool_t *mempool);
 *
 * Purpose: allocate a new object from the mempool
 * Args: mempool - pointer to a mempool_t struct
 *
 * Returns: a pointer to the mempool object on success, NULL on failure
 */
mem_bucket_t *mempool_alloc(mempool_t *mempool)
{
    sdlist_item_t *li = NULL;
    mem_bucket_t *b;

    if(mempool == NULL)
    {
        return NULL;
    }

    if(mempool->free <= 0)
    {
        return NULL;
    }

    /* get one item off the free_list,
       put one item on the usedlist
     */

    li = mempool->free_list.head;

    mempool->free--;
    if((li == NULL) || sf_sdlist_remove(&mempool->free_list, li))
    {
        printf("Failure on sf_sdlist_remove\n");
        return NULL;
    }


    mempool->used++;

    if(sf_sdlist_append(&mempool->used_list, li->data, li))
    {
        printf("Failure on sf_sdlist_append\n");
        return NULL;
    }

    /* TBD -- make configurable */
    b = li->data;
    bzero(b->data, mempool->obj_size);

    return b;
}


void mempool_free(mempool_t *mempool, mem_bucket_t *obj)
{
    if(sf_sdlist_remove(&mempool->used_list, obj->key))
    {
        printf("failure on remove from used_list");
        return;
    }

    mempool->used--;

    /* put the address of the membucket back in the list */
    if(sf_sdlist_append(&mempool->free_list, obj, obj->key))
    {
        printf("failure on add to free_list");
        return;
    }

    mempool->free++;
    return;
}

#ifdef TEST_MEMPOOL_MAIN

conf_t* conf = NULL;	//= {8080, 1024, 20, 10,  "aaa"};

static int mempool_test();

#define SIZE 4
int main(void)
{
	conf = (conf_t*)calloc(1, sizeof(conf_t));
	if(conf == NULL)
		return -1;

	conf->port = 8080;
	conf->max_recv_num  = 1024;
	conf->mempool_num = 10;
	conf->mempool_size = 10;

#if 1	//内存池测试
	mempool_test();
#endif
#if 0	//内存池链表测试
	if(mempool_list_init()<0)
		return -1;

	char str[100][10]  = {0};
	int i;
	for(i=0; i<100; i++)
	{
		sprintf(str+i, "str:%d", i);
	}

	for(i=0; i<100; i++)
	{
		mempool_list_push(str+i, strlen(str+i));
	}

	mempool_list_disp();
#endif
}

int mempool_test()
{
	sdlist_item_t* head;
	int count = 0;
	mempool_t test;
	mem_bucket_t* bucks[4] = {0};
	mem_bucket_t* bucket = NULL;

    char *stuffs[6] = { "aaaa", "bbbb", "cccc", "dddd", "eeee", "ffff" };

    if(mempool_init(&test, 5, 50))
    {
        printf("error in mempool initialization\n");
    }

	printf("=====================================\n");
	//添加数据
	int i;
    for(i = 0; i < 3; i++)
    {
        if((bucks[i] = mempool_alloc(&test)) == NULL)
        {
            printf("error in mempool_alloc: i=%d\n", i);
            continue;
        }

        bucket = bucks[i];

        bucket->data = strncpy(bucket->data, stuffs[i], test.obj_size);
        printf("bucket->data: %s\n", (char *) bucket->data);
    }

#if 1
	//遍利此内存池已使用的内存池
	printf("disp used\n");

	head = test.used_list.head;
	for(count=0; head != NULL; count++, head=head->next)
	{
		bucket = (mem_bucket_t*)(head->data);
		printf("cout:%d\t date:%s\n", count, bucket->data);
	}
#endif

#if 1
	//删除第一个元素
	//“aaaa" "bbbb", "cccc"　删除后
	//		 "bbbb", "cccc"

	printf("del head\n");

	head = test.used_list.head;
	bucket = (mem_bucket_t*)(head->data);
	mempool_free(&test, bucket);
#endif

#if 1
	//遍利此内存池已使用的内存池
	printf("disp used 2\n");

	head = test.used_list.head;
	for(count=0; head != NULL; count++, head=head->next)
	{
		bucket = (mem_bucket_t*)(head->data);
		printf("cout:%d\t date:%s\n", count, bucket->data);
	}
#endif

#if 1
	//删除第一个元素
	//“aaaa" "bbbb", "cccc"　删除后
	//		 "bbbb", "cccc"

	printf("del head\n");

	head = test.used_list.head;
	bucket = (mem_bucket_t*)(head->data);
	mempool_free(&test, bucket);
#endif

#if 1
	//遍利此内存池已使用的内存池
	printf("disp used 3\n");

	head = test.used_list.head;
	for(count=0; head != NULL; count++, head=head->next)
	{
		bucket = (mem_bucket_t*)(head->data);
		printf("cout:%d\t date:%s\n", count, bucket->data);
	}
#endif
}
#endif /* TEST_MEMPOOL */
