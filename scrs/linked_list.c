#include "linked_list.h"

ll* create_list(const char* name, int addr) {
	ll* new = malloc(sizeof(ll));
	strncpy(new->name, name, NAME_MAX);
	new->data = addr;
	new->next = NULL;
	return new;
}

void destroy_list(ll* list) {
	if(list->next != NULL) {
		destroy_list(list->next);
	}
	free(list);
}

_Bool list_contains(ll* list, const char* toFind) {
	for(ll* elem = list; elem != NULL; elem = elem->next) {
		if(strncmp(toFind, elem->name, NAME_MAX) == 0) {
			return true;
		}
	}

	return false;
}

ll* append_to_list(ll* _list, const char* name, int addr) {
	if(_list == NULL) {
		_list = create_list(name, addr);
	} else {
		ll* list = _list;
		while(list->next != NULL) {
			list = list->next;
		}
		/*
		for(list = _list; list->next != NULL; list = list->next) {
			printf("a\n");
		}*/


		list->next = create_list(name, addr);
	}
	return _list;
}

void print_list(ll* list) {
	for(list; list != NULL; list = list->next) {
		printf("%s: %d\n", list->name, list->data);
	}
}


int get_data(ll* list, const char* name) {
	for(ll* elem = list; elem != NULL; elem = elem->next) {
		if(strncmp(name, elem->name, NAME_MAX) == 0) {
			return elem->data;
		}
	}
	return -1;
}
