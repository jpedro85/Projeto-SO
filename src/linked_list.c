#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "consoleAddons.h"
#include "linked_list.h"

bool isEmpty_LinkedList(LinkedList* head){
	return head->first == NULL;
}

LinkedList* createLinkedList(){
	
	LinkedList* newList = ( LinkedList* ) malloc(sizeof(LinkedList));
	newList->first = NULL;
	newList->length = 0;
	return newList;
}

void initialize_LinkedList(LinkedList* head){

	head->length = 0;
	head->first = NULL;
}

void addValue_LinkedList(LinkedList* head, void* value){

	if( isEmpty_LinkedList(head) ){

		head->first = (ListItem*)malloc(sizeof(ListItem));
		head->first->next = NULL;
		head->first->value = value;
	
	} else {

		ListItem* list_item_aux = head->first;

		while( list_item_aux->next != NULL){

			list_item_aux = list_item_aux->next;
		}

		list_item_aux->next = (ListItem*)malloc(sizeof(ListItem));
		list_item_aux->next->value = value;
	}

	head->length++;

}

void addInt_LinkedList(LinkedList* head, int value){

	int * valueToAdd = (int*)malloc(sizeof(int));
	*valueToAdd = value;
	addValue_LinkedList(head,valueToAdd);
}

void addDouble_LinkedList(LinkedList* head, double value){

	double * valueToAdd = (double*)malloc(sizeof(double));
	*valueToAdd = value;
	addValue_LinkedList(head,valueToAdd);
}

void addChar_LinkedList(LinkedList* head, char ch){

	char * valueToAdd = (char*)malloc(sizeof(char));
	*valueToAdd = ch;
	addValue_LinkedList(head,valueToAdd);
}


void* getValueByIndex_LInkedList(LinkedList* head, int index){

	return getListItemByIndex_LinkedList(head,index)->value;
}

ListItem* getListItemByIndex_LinkedList(LinkedList* head, int index){

	if(index >= head->length || index < 0){
		printError("Index out of range.\n");

	} else {

		int currentIndex = 0;
		ListItem* listItem_aux = head->first;

		while ( listItem_aux->next != NULL){
			
			if(currentIndex == index)
				return listItem_aux;

			listItem_aux = listItem_aux->next;
			currentIndex++;
		}
		
	}

	return NULL;
}

void removeItemByIndex_LinkedList(LinkedList* head, int index){

	if( isEmpty_LinkedList(head) )
		printError("Can not remove item from Empty List.\n");
	
	else if(index >= head->length || index < 0)
		printError("Index out of range.\n");

	else {

		ListItem* listItem_remove;

		if(index == 0){

			listItem_remove = head->first;
			head->first = head->first->next;
			head->length--;
			free(listItem_remove);

		} else {

			int currenIndex = 0;
			ListItem* listItem_current = head->first;

			while( listItem_current->next != NULL){

				if(currenIndex == index-1){
					listItem_remove = listItem_current->next;
					listItem_current->next = listItem_remove->next;
					free(listItem_remove);
					head->length--;
					break;
				}

				listItem_current = listItem_current->next;
				currenIndex++;

			}

		}

	}

}

void printInts_LinkedList(LinkedList* mayTesteList){

    // ListItem* a;

    // printf("\n[\n");
    // ForEach_LinkedList(mayTesteList,a){
    //     printInfo( *((int*)a->value) );
    // }
    // printf("]\n");
}