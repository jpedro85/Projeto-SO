#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "consoleAddons.h"
#include "linked_list.h"

bool isEmpty_LinkedList(linkedList* head){
	return head->first == NULL;
}

linkedList createLinkedList(){
	
	linkedList* newList = ( linkedList* ) malloc(sizeof(linkedList));
	newList->first = NULL;
	newList->length = 0;
	return *newList;
}

void initializeLinkedList(linkedList head){

	head.length = 0;
	head.first = NULL;
}

void addValue_LinkedList(linkedList* head, void* value){

	if( isEmpty_LinkedList(head) ){

		head->first = (listItem*)malloc(sizeof(listItem));
		head->first->next = NULL;
		head->first->value = value;
	
	} else {

		listItem* list_item_aux = head->first;

		while( list_item_aux->next != NULL){

			list_item_aux = list_item_aux->next;
		}

		list_item_aux->next = (listItem*)malloc(sizeof(listItem));
		list_item_aux->next->value = value;
	}

	head->length++;

}

void addInt_LinkedList(linkedList* head, int value){

	int * valueToAdd = (int*)malloc(sizeof(int));
	*valueToAdd = value;
	addValue_LinkedList(head,valueToAdd);
}

void addDouble_LinkedList(linkedList* head, double value){

	double * valueToAdd = (double*)malloc(sizeof(double));
	*valueToAdd = value;
	addValue_LinkedList(head,valueToAdd);
}

void addChar_LinkedList(linkedList* head, char ch){

	char * valueToAdd = (char*)malloc(sizeof(char));
	*valueToAdd = ch;
	addValue_LinkedList(head,valueToAdd);
}


void* getValueByIndex_LInkedList(linkedList* head, int index){

	return getListItemByIndex_LinkedList(head,index)->value;
}

listItem* getListItemByIndex_LinkedList(linkedList* head, int index){

	if(index >= head->length || index < 0){
		printError("Index out of range.\n");

	} else {

		int currentIndex = 0;
		listItem* listItem_aux = head->first;

		while ( listItem_aux->next != NULL){
			
			if(currentIndex == index)
				return listItem_aux;

			listItem_aux = listItem_aux->next;
			currentIndex++;
		}
		
	}

	return NULL;
}

void removeItemByIndex_LinkedList(linkedList* head, int index){

	if( isEmpty_LinkedList(head) )
		printError("Can not remove item from Empty List.\n");
	
	else if(index >= head->length || index < 0)
		printError("Index out of range.\n");

	else {

		if(index == 0){

			listItem* listItem_remove = head->first;
			head->first = head->first->next;
			head->length--;
			free(listItem_remove);

		} else {

			int currenIndex = 0;
			listItem* listItem_remove;
			listItem* listItem_current = head->first;

			while( listItem_current->next != NULL){

				if(currenIndex == index-1){
					listItem_remove = listItem_current->next;
					listItem_current->next = listItem_remove->next;
					free(listItem_remove);
					break;
				}

				listItem_current = listItem_current->next;
				currenIndex++;

			}

		}

	}

}

void printInts_LinkedList(linkedList* mayTesteList){

    // listItem* a;

    // printf("\n[\n");
    // ForEach_LinkedList(mayTesteList,a){
    //     printInfo( *((int*)a->value) );
    // }
    // printf("]\n");
}