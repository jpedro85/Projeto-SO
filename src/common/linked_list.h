#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#define ForEach_LinkedList(head,item) for( item = head->first ; item != NULL ; item = item->next )
#include <stdbool.h>

typedef void*(clearValueFunc)(void* itemValue);
typedef struct ListItem{

	struct ListItem* next;
	void* value ;

}ListItem;

typedef struct {
	
	ListItem* first;
	int length;

}LinkedList;

//Initiate a linked list;
void initialize_LinkedList(LinkedList* head);

//create dynamic head for linked list
LinkedList* createLinkedList();
//linkedList createLinkedList(void* value);

//return the item in head linked list that as the desired index
ListItem* getListItemByIndex_LinkedList(LinkedList* head, int index);

//return the value in item in head linked list that as the desired index
void* getValueByIndex_LInkedList(LinkedList* head, int index);

//add new value to the linked list head
void addValue_LinkedList(LinkedList* head, void* value);
void addInt_LinkedList(LinkedList* head, int value);
void addDouble_LinkedList(LinkedList* head, double value);
void addChar_LinkedList(LinkedList* head, char ch);

//removes the item with the desired index in the head linked list
void removeItemByIndex_LinkedList(LinkedList* head, int index);

bool isEmpty_LinkedList(LinkedList* head);

//clear
void clear_linkedList(LinkedList* head);
void clear_linkedListItemsValueWithFunc(LinkedList* head, clearValueFunc freeStruct );

//print functions
void print_LinkedList(LinkedList* head);

#endif