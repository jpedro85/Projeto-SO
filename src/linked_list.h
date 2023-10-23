#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#define ForEach_LinkedList(head,item) for( item = head->first ; item != NULL ; item = item->next )
#include <stdbool.h>

typedef struct listItem {

	struct listItem* next;
	void* value ;

}listItem;

typedef struct linkedList{
	
	listItem* first;
	int length;

}linkedList;

bool isEmpty_LinkedList(linkedList* head);

//Initiate a new linked list;
linkedList createLinkedList();
//linkedList createLinkedList(void* value);

//return the item in head linked list that as the desired index
listItem* getListItemByIndex_LinkedList(linkedList* head, int index);

//return the value in item in head linked list that as the desired index
void* getValueByIndex_LInkedList(linkedList* head, int index);

//add new value to the linked list head
void addValue_LinkedList(linkedList* head, void* value);
void addInt_LinkedList(linkedList* head, int value);
void addDouble_LinkedList(linkedList* head, double value);
void addChar_LinkedList(linkedList* head, char ch);

//removes the item with the desired index in the head linked list
void removeItemByIndex_LinkedList(linkedList* head, int index);

//print
void print_LinkedList(linkedList* head);

#endif