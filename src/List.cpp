#include "List.hpp"
#include <string.h>

void InitSingleList(list_t* list, int max) {
	memset(list, 0, sizeof(list_t));
	list->nodes = (node_t*)calloc(max, sizeof(node_t));
	list->max = max;
	list->head = list->tail = list->nodes;
}

void SLPushItem(list_t* list, void* data) {
	if (list->count >= list->max) return;
	node_t new_node = { 0 };
	new_node.active = true;
	new_node.next = 0;
	new_node.data = data;
	*list->tail = new_node;
	list->tail->next = list->tail + 1;
	list->tail = list->tail + 1;
	list->count++;
}

void SLPopItem(list_t* list) {
	if (list->count <= 0) return;
	list->tail->active = false;
	list->tail = list->tail - 1;
	list->count--;
}

void SLReset(list_t* list) {
	list->tail = list->nodes;
	list->count = 0;
	memset(list->nodes, 0, sizeof(node_t) * list->max);
}