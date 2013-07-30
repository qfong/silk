
#ifndef _MEMPOOL_LIST_H__
#define _MEMPOOL_LIST_H__

#include "sk_mem_pool.h"
#include "sk_sdlist.h"

/*缓存池链表*/
typedef struct mempool_list_t
{
	mempool_t mempool;
	struct mempool_list_t* next;
}mempool_list_t;

//目前，对外只有三个接口，最终会把不使用的接口隐藏
//mempool_list_init		(使用之前调用)
//mempool_list_push		(向内存池中插入数据)
//mempool_list_pull		(从内存池中取出数据)
extern mempool_list_t* mp_list_head ;
extern mempool_list_t* mp_list_cur ;

extern sk_ptr_t mempool_list_init();
extern sk_ptr_t mempool_list_push(void* data, int len);
extern sk_ptr_t mempool_list_pull(void* data, int* len);

extern sk_ptr_t mempool_list_disp();

#endif /*#ifdef __MEMPOOL_LIST_H__*/
