#ifndef __YARR_LIST
#define __YARR_LIST

typedef struct list_item {
    struct list_item *prev, *next;
    void *data;
} ListItem_t;

/**
 * Creates a new list item.
 *
 * @return A pointer to the newly created item or NULL.
 */
ListItem_t * ListItem_create(void);

/**
 * Destroy a list item.
 *
 * @this: The item to be destroyed.
 * @free_data: A pointer to a function that frees the resources taken by the
 * data we keep in this element.
 */
void ListItem_destroy(ListItem_t *this, void (*free_data)(void *data));

/**
 * Set the data contained in this item.
 *
 * @this: The list item that will contain the data.
 * @data: A pointer to whatever data we want to hold on this item.
 * @return: 0 on success, non-zero otherwise.
 */
int ListItem_setData(ListItem_t *this, void *data);

/**
 * Get the data contained in this item.
 *
 * @this: The ListItem_t from where to get the data.
 * @return: A pointer to the data contained or NULL.
 */
void * ListItem_getData(ListItem_t *this);

/**
 * Checks if the data contained in this item is the same as the data passed.
 *
 * @this: The ListItem_t that contains the data to compare against.
 * @data: A pointer to the data to compare.
 * @cmp: A pointer to a function that compares two datas. This function shall
 * return zero if e1 == e2, less than zero if e1 < e2 or greater than zero if
 * e1 > e2.
 * @return: Non-zero if data is equal to the data kept in this item, zero
 * elsewhere.
 */
int ListItem_dataIsEqual(
        ListItem_t *this,
        void *data,
        int (*cmp)(void *e1, void *e2));

/**
 * Prints the list item. Mostly for debugging purposes.
 *
 * @this: The item to print.
 * @print_data: Pointer to a function able to print the data stored by the
 * item.
 */
void ListItem_print(ListItem_t *this, void (*print_data)(void *));

//----------------------------------------------------------

typedef struct list {
    ListItem_t *head, *tail;
    unsigned int length;
    void (*print_data)(void *data);
    int (*cmp_data)(void *e1, void *e2);
    void (*free_data)(void *data);
} List_t;

/*
 * About List_t use:
 *
 * - The list is not sorted.
 * - Indexes start at 0.
 * - Expect bugs, use at your own risk xD.
 */

/**
 * Creates an empty initialized list.
 *
 * @print_data: Pointer to a function that prints the elements stored.
 * @cmp_data: Pointer to a function that compares two datas. Shall return zero
 * if both elements are equal, less than zero if e1 < e2 or greater than zero
 * if e1 > e2.
 * @free_data: Pointer to a function that frees the data kept in the elements
 * of the list.
 * @return A pointer to the newly created list or NULL;
 */
List_t * List_create(
        void (*print_data)(void *data),
        int (*cmp_data)(void *e1, void *e2),
        void (*free_data)(void *data));

/**
 * Destroys a list. The list should've been created with List_create().
 *
 * @this: The list to be destroyed.
 */
void List_destroy(List_t *this);

/**
 * Get the size of the list.
 *
 * @this: The list to which get the size.
 */
unsigned int List_length(List_t *this);

/**
 * Insert an item into the list. Use with caution.
 *
 * @this: The list to where insert the element.
 * @item: The item to insert in the list.
 * @return: 0 if the item was successfully inserted, 1 otherwise.
 */
int List_insertItem(List_t *this, ListItem_t *item);

/**
 * Insert some data into the list. The ListItem will be handled underneath.
 *
 * @this: The list to where insert the element.
 * @data: A pointer to the data to insert.
 * @return: 0 if the item was successfully inserted, 1 otherwise.
 */
int List_insertData(List_t *this, void *data);

/**
 * Remove an element from the list. Use with caution.
 *
 * @this: The list from where to remove the element.
 * @item: The item to remove from the list.
 * @return: 0 if the item was successfully removed, 1 otherwise.
 */
int List_removeItem(List_t *this, ListItem_t *item);

/**
 * Remove an element from the list based on index. Use with caution.
 *
 * @this: The list from where to remove the element.
 * @index: The index of the item to remove.
 * @return: A pointer to the element removed from the list or NULL.
 */
ListItem_t * List_removeByIndex(List_t *this, unsigned int index);

/**
 * Get an item from the list.
 *
 * @this: The list from where to get the item.
 * @index: The index of the item we want to get.
 * @return: A pointer to the item or NULL.
 */
ListItem_t * List_getItem(List_t *this, unsigned int index);

/**
 * Get the data associated to an item.
 *
 * @this: The list from where to get the data.
 * @index: The index of the item that keeps the data.
 * @return: A pointer to the data or NULL.
 */
void * List_getData(List_t *this, unsigned int index);

/**
 * Checks if a some data is contained in the list.
 *
 * @this: The list where the data might be.
 * @data: The value of data we are looking for.
 * @return: Zero if value is not present, non-zero elsewhere.
 */
int List_itemIsContained(List_t *this, void *data);

/**
 * Returns the ListItem_t that stores the data passed as argument. This method
 * doesn't compare the pointer data with the pointer of data stored in the
 * element, instead it compares the two blocks of data with the cmp_data
 * function passed when the list was created.
 *
 * @this: The pointer to the list.
 * @data: A pointer to a block of data we want to check if it exists already in
 * the list. Note that repetition is allowed in the list, if the same block of
 * data is stored in the list, this method returns the first ocurrence.
 * @return: A pointer to the item storing the data or NULL.
 */
ListItem_t * List_getItemByData(List_t *this, void *data);

/**
 * Prints a representation of the list. This is mostly for debugging purposes.
 *
 * @this: The list to print.
 */
void List_print(List_t *this);

#endif

