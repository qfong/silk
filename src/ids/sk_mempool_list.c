
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _PTHREAD
#include <pthread.h>
#endif

#include "sk_ids.h"


#ifdef _PTHREAD
extern pthread_mutex_t* mutex;
#endif

mempool_list_t* mp_list_head = NULL;
mempool_list_t* mp_list_cur = NULL;

static int mempool_list_add();
static mem_bucket_t* mempool_list_alloc();


/*
 * 缓存池初始化
 * return 0:succ -1:fail
 */
sk_ptr_t
mempool_list_init(){
	mempool_list_t* new = (mempool_list_t*)calloc(1,
			sizeof(mempool_list_t));
	if(new == NULL)
		FATAL_ERR("init mempool list error");

	new->next = NULL;
	FATAL_ERR_CHECK(mempool_init(&(new->mempool), sk_ctx->mempool_num,
			sk_ctx->mempool_size+1), "init mempool list error");

	mp_list_head = new;

	mp_list_cur = new;

	return SK_OK;
}


/*
 *  向缓存池中添加数据
 *  return 0:succ -1:fail
 */
sk_ptr_t
mempool_list_push(void* data, int len)
{
	if(data==NULL || len<=0)
		return -1;

	int err;
#ifdef _PTHREAD
	if((err = pthread_mutex_lock(mutex))!=0)
	{
		save_err_log("mempool_list_push get mutex error");
		return -1;
	}
	else
#endif


	{
		char* sid;
		char* body;

		int body_len = len;

		if((body_len + NIDS_SID_LEN) < sk_ctx->mempool_size)	//一个内存池中的元素可以存放
		{
			mem_bucket_t* bucket = mempool_list_alloc();
			if(bucket == NULL)
			{
#ifdef _PTHREAD
				pthread_mutex_unlock(mutex);
#endif


				return SK_ERR;
			}

			memcpy(bucket->data, data, len+NIDS_SID_LEN);

			bucket->len = NIDS_SID_LEN + len;
			//printf("...............%s\n",bucket->data);
#ifdef _PTHREAD
			pthread_mutex_unlock(mutex);
#endif
		}
		else		//需要使用多个内存池中的元素存放
		{
			int per_content = sk_ctx->mempool_size - NIDS_SID_LEN -1;

			int num = body_len/per_content;
			if(body_len%per_content != 0)
				num++;

			int i;
			for(i=0; i<num; i++)
			{
				mem_bucket_t* bucket = mempool_list_alloc();

				if(bucket == NULL)
				{
#ifdef _PTHREAD
					pthread_mutex_unlock(mutex);
#endif


					return SK_ERR;
				}


				memcpy(bucket->data, data + i*per_content, per_content);
				bucket->len = sk_ctx->mempool_size - 1;
			}
		}


	}
	return SK_OK;
}

/*
 *	从缓存池中读取数据
 *	return 0:succ -1:fail
 */
sk_ptr_t
mempool_list_pull(void* data, int* len)
{
	if(mp_list_head == NULL)
		return -1;

	int err;
#ifdef _PTHREAD
	if((err = pthread_mutex_lock(mutex))!=0)
	{
		save_err_log("mempool_list_pull get mutex error");
		return -1;
	}
	else
#endif

	{
		/*若mp_list_head指向元素的内存池存储量为0,则返回-2，表明当前缓存池链表中无数据*/
		//printf("header ........ %d \n",mp_list_head);
		if(mp_list_head->mempool.used == 0)
		{

#ifdef _PTHREAD
			pthread_mutex_unlock(mutex);
#endif
			return -2;
		}

		/*若mp_list_head指向元素的内存池存储量不为0,则读取此内存池内容*/
		/*找到mp_list_head指向元素的内存池中已使用的内存链表开头位置*/
		sdlist_item_t* used_item_head = mp_list_head->mempool.used_list.head;
		mem_bucket_t* bucket = (mem_bucket_t*)(used_item_head->data);

		memcpy(data, bucket->data, bucket->len);
		*len = bucket->len;

		/*把取出的bucket从mp_list_head指向元素的内存池中删除*/
		mempool_free(&(mp_list_head->mempool), bucket);
		if(mp_list_head->mempool.used == 0){	//若mp_list_head指向元素的内存池存储量为0

			if(mp_list_head->next != NULL){
				mempool_destroy(&(mp_list_head->mempool));	//销毁此mp_list_head指向元素上面的内存池
				//释放内存池
				save_err_log("mempool_destroy");
				mempool_list_t* tmp = mp_list_head;
				mp_list_head =mp_list_head->next;

				FREE(tmp);
			}
		}
	}

#ifdef _PTHREAD
	pthread_mutex_unlock(mutex);
#endif
	return 0;
}

/*
 * 显示缓存池中的内容
 */
sk_ptr_t
mempool_list_disp()
{
	mempool_list_t* tmp = mp_list_head;
	unsigned long total = 0;
	for(; tmp!=NULL; tmp = tmp->next){
		sdlist_item_t* tmp_item = tmp->mempool.used_list.head;
		mem_bucket_t* bucket;
		for( ; tmp_item != NULL; tmp_item = tmp_item->next){
			bucket = (mem_bucket_t*)(tmp_item->data);
			printf("%s\n", (char*)(bucket->data));
			total++;
		}
	}
	printf("The total is %ld\n", total);

	return 0;
}

/*
 * 缓存池中增加一个缓存池节点
 * return 0:succ -1:fail
 */
static int mempool_list_add()
{
	if(mp_list_cur == NULL)
		FATAL_ERR("mempool current pointer is NULL");

	mempool_list_t* new = (mempool_list_t*)calloc(1, sizeof(mempool_list_t));
	if(new == NULL)
		FATAL_ERR("init mempool list error");

	new->next = NULL;
	FATAL_ERR_CHECK(mempool_init(&(new->mempool), sk_ctx->mempool_num,
			sk_ctx->mempool_size), "mempool_init error");

	mp_list_cur->next = new;
	mp_list_cur = new;

	return SK_OK;
}

/*
 * 从缓存池中申请空间
 * return !NULL:succ NULL:fail
 */
static mem_bucket_t* mempool_list_alloc()
{
	if(mp_list_cur == NULL)
		FATAL_ERR("mempool current pointer is NULL");

	if(mp_list_cur->mempool.used >= mp_list_cur->mempool.total)	//当前内存池已消耗完
		FATAL_ERR_CHECK(mempool_list_add(), "mempool list add error");

	mem_bucket_t* bucket = mempool_alloc(&mp_list_cur->mempool);
	if(bucket == NULL){
		return NULL;
	}

	return bucket;
}
