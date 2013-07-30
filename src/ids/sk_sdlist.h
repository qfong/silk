
#ifndef _SF_SDLIST
#define _SF_SDLIST

/* based off Linked List structure p. 57  _Mastering algorithms in C_
 *
 * Differs from sf_list by using static listitem blocks.
 *
 * Use mempool as the interface to this code instead of trying to use it directly
 *
 */

typedef struct _sdlist_item_t {
    void *data;
    struct _sdlist_item_t *next;
    struct _sdlist_item_t *prev;
} sdlist_item_t;


typedef struct sf_sdlist_t {
    int size;
    sdlist_item_t *head;
    sdlist_item_t *tail;
    void (*destroy)(void *data); /* delete function called for each
                                    member of the linked list */
} sf_sdlist_t;


/* initialize a DList */
int sf_sdlist_t_init(sf_sdlist_t *list, void (*destroy)(void *data));

/* delete an DList */
int sf_sdlist_t_delete(sf_sdlist_t *list);

/* insert item, putting data in container */
int sf_sdlist_t_insert_next(sf_sdlist_t *list, sdlist_item_t *item, void *data,
                          sdlist_item_t *container);

/* remove the item after the item */
int sf_sdlist_t_remove_next(sf_sdlist_t *list, sdlist_item_t *item);

/* remove this item from the list */
int sf_sdlist_t_remove(sf_sdlist_t *list, sdlist_item_t *item);

/* append at the end of the list */
int sf_sdlist_t_append(sf_sdlist_t *list, void *data, sdlist_item_t *container);

void print_sdlist(sf_sdlist_t *list);

void sf_sdlist_free(void* p);

#endif /* _SF_DLIST */
