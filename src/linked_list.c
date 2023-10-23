#include <stdio.h>
#include <stdlib.h>

#include "auxiliar_console.h"

struct list_item {

	struct list_item* ant;
	struct list_item* next;
	void* value ;
};

struct list_item* create_LinkedList(void* value){

	struct list_item* newlist = (struct list_item *) malloc(sizeof(struct list_item));
	newlist->value = value;
	newlist->ant = NULL;
	newlist->next = NULL;

	return newlist;
}

int length_LinkedList(struct list_item* head ){

	int length = 0;
	struct list_item* list_item_aux = head;

	while(list_item_aux != NULL) {

		list_item_aux = list_item_aux->next;
		length++;
	}
	return length;
}

struct list_item* getItemByIndex_LinkedList(struct list_item* head ,int index){

	int atual_index = 0;
	struct list_item* list_item_aux = head;

	while(list_item_aux != NULL){

		if(atual_index == index){
			return list_item_aux;
		}

		list_item_aux = list_item_aux->next;
		atual_index++;
	}

	printf("Index out of range.\n");
	return NULL;
}

void* getValueByIndex_LinkedList(struct list_item* head ,int index){
	return  getItemByIndex_LinkedList(head,index)->value;
}

void addItem_LinkedList(struct list_item* head, void* value){

	if(head != NULL){

		struct list_item* list_item_aux = head;

		while( list_item_aux->next != NULL){

			list_item_aux = list_item_aux->next;
		}

		list_item_aux->next = (struct list_item*)malloc(sizeof(struct list_item));
		list_item_aux->next->value = value;
		list_item_aux->next->ant = list_item_aux;

	}else
		printf("Can not add item to NULL list head.\n");
}

void addInt_LinkedList(struct list_item* head, int value){

	int * valueToAdd = (int*)malloc(sizeof(int));
	*valueToAdd = value;
	addItem_LinkedList(head,valueToAdd);
}

void addDouble_LinkedList(struct list_item* head, double value){

	double * valueToAdd = (double*)malloc(sizeof(double));
	*valueToAdd = value;
	addItem_LinkedList(head,valueToAdd);
}

void addChar_LinkedList(struct list_item* head, char ch){

	char * valueToAdd = (char*)malloc(sizeof(char));
	*valueToAdd = ch;
	addItem_LinkedList(head,valueToAdd);
}

void removeItemByIndex_LinkedList(struct list_item** head, int index){

	if(*head != NULL){

		struct list_item* list_item_remove;

		if(index == 0){

			list_item_remove = *head;
			*head = (*head)->next;
			free(list_item_remove);

		}else{

			list_item_remove = getItemByIndex_LinkedList(*head,index);

			if(list_item_remove != NULL){

				if(list_item_remove->next == NULL)
					list_item_remove->ant->next == NULL;
				else 
					list_item_remove->ant->next = list_item_remove->next;
					
				free(list_item_remove);
			}
		}

	}else
		printf("Can not remove item from NULL list head.\n");
}

void print_LinkedList(struct list_item* head){

	struct list_item * list_item_aux = head;

	printf("\n[\n");

	while (list_item_aux != NULL)
	{
		printf(" ");
		printOptn(list_item_aux->value);
		list_item_aux = list_item_aux->next;
	}

	printf("]\n\n");
	
}