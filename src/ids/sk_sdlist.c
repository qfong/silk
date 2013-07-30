
/*
 * Copyright(C) xingke.zhang
 * Copyright(C) cs2c,Inc
 */


#include "sk_sdlist.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Function: int sf_sdlist_init(sfSDlist *list, void (*destroy)(void *data))
 *
 * Purpose: initialize an dlist
 * Args: list - pointer to a dlist structure
 *       destroy - free function ( use NULL for none )
 * Returns:
 *     1 on failure , 0 on success
 */

int sf_sdlist_init(sf_sdlist_t *list, void (*destroy)(void *data))
{
    list->destroy = destroy;
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;

    return 0;
}


/* Function: int sf_sdlist_delete(sf_sdlist_t *list)
 *
 * Purpose: delete every item of a list
 * Args: list -> pointer to a dlist structure
 *
 * Returns: 1 on failure , 0 on success
 */
int sf_sdlist_delete(sf_sdlist_t *list)
{
    while(list->head != NULL)
    {
        sf_sdlist_remove_next(list, NULL);
    }

    return 0;
}

/*
 * Function: int sf_sdlist_insert_next(sf_sdlist_t *list, sdlist_item_t *item,
 *                                    void *data, sdlist_item_t *container)
 *
 * Purpose: insert data in container in the list after the item
 * Args: list - dlist structure
 *       item - current position in list structure
 *       data - current data to insert
 *       container - place to put the data
 *
 * Returns: 0 on sucess,  1 on failure
 */
int sf_sdlist_insert_next(sf_sdlist_t *list, sdlist_item_t *item, void *data,
                          sdlist_item_t *container)
{
    sdlist_item_t *new = container;

    if(!new) return -1;

    new->data = data;

    if(item == NULL)
    {
        /* We are inserting at the head of the list HEAD */

        if(list->size == 0)
        {
            list->tail = new;
        }

        new->next = list->head;
        list->head = new;
    }
    else
    {
        if(item->next == NULL)
        {
            /* TAIL */
            list->tail = new;
        }

        new->next = item->next;
        item->next = new;
    }

    new->prev = item;
    list->size++;
    return 0;
}

int sf_sdlist_append(sf_sdlist_t *list, void *data, sdlist_item_t *container) {
    return sf_sdlist_insert_next(list, list->tail, data, container);
}

int sf_sdlist_remove_next(sf_sdlist_t *list, sdlist_item_t *item) {
    sdlist_item_t *li = NULL;
    void *data;

    if(list->size == 0)
    {
        return -1;
    }

    /* remove the head */
    if(item == NULL)
    {
        li = list->head;
        data = li->data;
        list->head = li->next;
    }
    else
    {
        data = item->data;
        if(item->next == NULL)
        {
            return -1;
        }

        li = item->next;
        item->next = li->next;
        item->prev = li->prev;
    }

    if(li->next != NULL)
    {
        li->next->prev = item;
    }


    if(list->destroy != NULL)
        list->destroy(data);

    list->size--;

    if(list->size == 0) {
        list->tail = NULL;
    }

    return 0;
}


/*
 * Function: int sf_sdlist_remove(sf_sdlist_t *list, sdlist_item_t *item)
 *
 * Purpose: remove the item pointed to by item
 * Args: list - list pointer
 *       item - item to unlink from the list
 *
 * Returns: 0 on success , 1 on exception
 *
 */
int sf_sdlist_remove(sf_sdlist_t *list, sdlist_item_t *item)
{
    sdlist_item_t *next_item;
    sdlist_item_t *prev_item;

    if(item == NULL)
    {
        return -1;
    }

    next_item = item->next;
    prev_item = item->prev;

    if(next_item != NULL)
    {
        next_item->prev = prev_item;
    } else {
        list->tail = prev_item;
    }

    if(prev_item != NULL)
    {
        prev_item->next = next_item;
    } else {
        /* HEAD */
        list->head = next_item;
    }




    if(list->destroy != NULL)
        list->destroy(item->data);


    list->size--;

    if(list->size == 0)
    {
        list->head = NULL;
        list->tail = NULL;
    }

    return 0;
}


void print_sdlist(sf_sdlist_t *a)
{
    sdlist_item_t *li;
    printf("***");
    printf(" size: %d\n", a->size);
    for(li = a->head; li != NULL; li = li->next) {
        printf(" `- %p\n", li);
    }
}

void sf_sdlist_free(void* p)
{
	free(p);
	return;
}

#ifdef TEST_SDLIST
void bad(void *d) {
    free(d);
    return;
}

int main(void) {
    sf_sdlist_t a;

    sdlist_item_t *li;
    sdlist_item_t listpool[1000];

    sf_sdlist_init(&a, &bad);
    if(sf_sdlist_append(&a, (char *) strdup("second"), &listpool[0]))
    {
        printf("error appending!\n");
    }

    sf_sdlist_append(&a, (char *) strdup("third"), &listpool[1]);
    sf_sdlist_append(&a, (char *) strdup("forth"), &listpool[2]);

    sf_sdlist_insert_next(&a, NULL, (char *) strdup("zero"), &listpool[3]);
    sf_sdlist_insert_next(&a, &listpool[3], (char *) strdup("first"), &listpool[4]);


    printf("list size %d\n", a.size);

    for(li = a.head; li != NULL; li = li->next)
    {
        printf("%s\n", (char *) li->data);
    }


    printf("*** removing ***\n");
    sf_sdlist_remove(&a, &listpool[1]);
    printf("list size %d\n", a.size);
    for(li = a.head; li != NULL; li = li->next)
    {
        printf("%s\n", (char *) li->data);
    }

    sf_sdlist_delete(&a);

    printf("list size %d\n", a.size);
    return 0;
}
#endif /* TEST_SDLIST */
