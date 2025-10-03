#ifndef	LIST_INCLUDED
#define LIST_INCLUDED

#include "stdlib.h"
#include "stdio.h"

// Single list
struct node_t {
	bool active;
	void* data;
	struct node_t* next;
};

typedef struct {
	struct node_t* head;
	struct node_t* nodes;
	int count;
	int last_search;
	int max;
} list_t;

void InitSingleList(list_t* list, int max);

void DestroySingleList(list_t* list);

node_t* SLPushItem(list_t* list, void* data);

void SLPopItem(list_t* list);

void SLRemoveItemAt(list_t* list, int pos);

void SLReset(list_t* list);

// Double Linked List
struct dual_node_t {
	bool active;
	void* data;
	struct dual_node_t* prev;
	struct dual_node_t* next;
};

typedef struct {
	struct dual_node_t* head;
	struct dual_node_t* tail;
	struct dual_node_t* nodes;
	int count;
	int max;
} dual_list_t;

#endif