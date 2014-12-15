#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

struct list_node_s;

typedef struct list_node_s
{
    struct list_node_s* Prev;
    struct list_node_s* Next;
} list_node_t;

list_node_t* LinkedListCreateNode(uint_t numBytes);
void LinkedListDeleteNode(list_node_t* node);

void* LinkedListObjectFromNode(list_node_t* node);
list_node_t* LinkedListNodeFromObject(void* obj);

void LinkedListAddNode(list_node_t* existing, list_node_t* node);
void LinkedListRemoveNode(list_node_t* node);


#endif // _LINKEDLIST_H_