#ifndef LINKED_LIST_H
#define LINKED_LIST_H

struct list_item {

	struct list_item* ant;
	struct list_item* next;
	void* value ;
};

//Creates a new linked list;
struct list_item* create_LinkedList(void* value);

//return the length of the linked list;
int length_LinkedList(struct list_item* head );

//return the item in head linked list that as the desired index
struct list_item* getItemByIndex_LinkedList(struct list_item* head ,int index);

//return the value in item in head linked list that as the desired index
void* getValueByIndex_LinkedList(struct list_item* head ,int index);

//add new value to the linked list head
void addItem_LinkedList(struct list_item* head, void* value);
void addInt_LinkedList(struct list_item* head, int value);
void addDouble_LinkedList(struct list_item* head, double value);
void addChar_LinkedList(struct list_item* head, char ch);

//removes the item with the desired index in the head linked list
void removeItemByIndex_LinkedList(struct list_item** head, int index);

//print
void print_LinkedList(struct list_item* head);

#endif
