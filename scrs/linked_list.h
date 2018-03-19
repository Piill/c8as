
#ifndef _LINKED_LIST
#define _LINKED_LIST

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <errno.h>
#define NAME_MAX 254

typedef struct _ll {
	char name[NAME_MAX];
	int data;
	struct _ll* next;
} ll;

void destroy_list(ll* list);
_Bool list_contains(ll* list, const char* toFind);
ll* append_to_list(ll* list, const char* name, int addr);
void print_list(ll* list);
ll* create_list(const char* name, int addr);
int get_data(ll* list, const char* name);





#endif //_LINKED_LIST
