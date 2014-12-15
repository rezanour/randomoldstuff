#ifndef _LIST_H_
#define _LIST_H_

// REVIEW: need to determine in which cases item deleter should be used.
// What if you want to remove an item without deleting it?
//
// one idea is to have ListDelete(), which is different than ListRemove()

// item deleter callbacks can be optionally provided
// to the listcreate and will be called whenever a list
// is deleting an item
typedef void (*item_deleter)(void* item);

typedef void* arraylist_t;

// create a new list, optionally providing a deleter callback
arraylist_t ListCreate(item_deleter deleter);

// destroys the contents of the list and deallocates it.
// if a deleter callback was provided, it will be invoked for each item
void ListDestroy(arraylist_t list);

void ListAdd(arraylist_t list, void* item);
void ListRemove(arraylist_t list, void* item);
void ListRemoveAt(arraylist_t list, uint_t index);

uint_t ListSize(arraylist_t list);
void ListClear(arraylist_t list);

// get or set items in the list at a particular index.
// note that set will NOT call the item deleter on the previous contents
void* ListGet(arraylist_t list, uint_t index);
void ListSet(arraylist_t list, uint_t index, void* item);

#endif // _LIST_H_