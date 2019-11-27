#include <linux/slab.h>

#include "list.h"
#include "log.h"

ListItem_t * ListItem_create(void) {
    ListItem_t *new_item;

    new_item = kmalloc(sizeof(ListItem_t), GFP_KERNEL);
    if (new_item != NULL) {
        new_item->prev = new_item->next = new_item->data = NULL;
    }

    return new_item;
}

void ListItem_destroy(ListItem_t *this) {
    kfree(this);
    return;
}

int ListItem_setData(ListItem_t *this, void *data) {
    if (this == NULL) {
        yarr_log("this is NULL");
        return 1;
    }

    this->data = data;
    return 0;
}

void * ListItem_getData(ListItem_t *this) {
    if (this == NULL) {
        yarr_log("this is NULL");
        return NULL;
    }

    return this->data;
}

void ListItem_print(ListItem_t *this, void (*print_data)(void *data)) {
    if (this == NULL) {
        yarr_log("this is NULL");
        return;
    }

    yarr_log("ListItem_t [prev = %px, next = %px, data = %px]",
            this->prev, this->next, this->data);

    if (this->data == NULL || print_data == NULL) {
        return;
    }

    // TODO: ListItem_t or List_t (depending on how flexible we want to be)
    // should be given with a function that prints the elements kept by the
    // list items. I guess this operation should be kept by the list, instead
    // on each element of the list. Assume the list stores one type, not many.
    print_data(this->data);
}

//---------------------------------------------------------

List_t * List_create(void (*print_data)(void *)) {
    List_t *list;
    
    list = kmalloc(sizeof(List_t), GFP_KERNEL);
    if (list != NULL) {
        list->head = list->tail = NULL;
        list->length = 0;
        list->print_data = print_data;
    }

    return list;
}

void List_destroy(List_t *this) {
    kfree(this);
    return;
}

unsigned int List_length(List_t *this) {
    if (this != NULL) {
        return this->length;
    }

    return 0;
}

int List_insertItem(List_t *this, ListItem_t *item) {
    if (this == NULL) {
        yarr_log("this is NULL");
        return 1;
    }

    if (item == NULL) {
        yarr_log("item is NULL");
        return 1;
    }

    item->next = NULL;
    item->prev = this->tail;

    if (this->tail != NULL) {
        this->tail->next = item;
    }

    this->tail = item;

    if (this->length == 0) {
        this->head = item;
    }

    this->length++;
    return 0;
}

int List_insertData(List_t *this, void *data) {
    ListItem_t *item;
    int err;

    if (this == NULL) {
        yarr_log("this is NULL");
        return 1;
    }

    if (data == NULL) {
        yarr_log("data is NULL");
        return 1;
    }

    item = ListItem_create();
    if (item == NULL) {
        yarr_log("Couldn't create new ListItem");
        return 1;
    }

    err = ListItem_setData(item, data);
    if (err) {
        yarr_log("Couldn't set data in the item");
        ListItem_destroy(item);
        return 1;
    }

    err = List_insertItem(this, item);
    if (err) {
        yarr_log("Couldn't insert item into list");
        kfree(item);
        return 1;
    }

    return 0;
}

int List_removeItem(List_t *this, ListItem_t *item) {
    ListItem_t *iter;

    if (this == NULL) {
        yarr_log("this is NULL");
        return 1;
    }

    if (item == NULL) {
        yarr_log("item is NULL");
        return 1;
    }

    //TODO: O(n), can be improved to O(1).
    iter = this->head;
    while (iter != NULL) {
        if (iter == item) {
            if (item->prev != NULL) {
                item->prev->next = item->next;
            } else {
                this->head = item->next;
            }

            if (item->next != NULL) {
                item->next->prev = item->prev;
            } else {
                this->tail = item->prev;
            }

            item->prev = item->next = NULL;
            this->length--;
            return 0;
        }
    }

    return 1;
}

ListItem_t * List_removeByIndex(List_t *this, unsigned int index) {
    ListItem_t *iter;
    int i;

    if (this == NULL) {
        yarr_log("this is NULL");
        return NULL;
    }

    if (index >= this->length) {
        yarr_log("index out of bounds");
        return NULL;
    }

    //TODO: We could optimize a bit checking if we are closer starting from
    // the tail.
    for (i = 0, iter = this->head; i < index; i++, iter = iter->next);

    if (iter->prev != NULL) {
        iter->prev->next = iter->next;
    } else {
        this->head = iter->next;
    }

    if (iter->next != NULL) {
        iter->next->prev = iter->prev;
    } else {
        this->tail = iter->prev;
    }

    iter->prev = iter->next = NULL;
    this->length--;
    return iter;
}

ListItem_t * List_getItem(List_t *this, unsigned int index) {
    ListItem_t *iter;
    int i;

    if (this == NULL) {
        yarr_log("this is NULL");
        return NULL;
    }

    if (index >= this->length) {
        yarr_log("index %d out of bounds", index);
        return NULL;
    }

    //TODO: We could optimize a bit checking if we are closer starting from
    // the tail.
    for (i = 0, iter = this->head; i < index; iter = iter->next, i++);
    return iter;
}

void * List_getData(List_t *this, unsigned int index) {
    ListItem_t *iter;
    unsigned int i;

    if (this == NULL) {
        yarr_log("this is NULL");
        return NULL;
    }

    if (index >= this->length) {
        yarr_log("index %d out of bounds", index);
        return NULL;
    }

    //TODO: We could optimize a bit checking if we are closer starting from
    // the tail.
    for (i = 0, iter = this->head; i < index; iter = iter->next, i++);
    return iter->data;
}

void List_print(List_t *this) {
    ListItem_t *iter;

    if (this == NULL) {
        yarr_log("this is NULL");
        return;
    }

    yarr_log("this = %px", this);
    iter = this->head;
    while (iter != NULL) {
        ListItem_print(iter, this->print_data);
        iter = iter->next;
    }
}

