#include "List.hpp"
#include <string.h>

void InitSingleList(list_t* list, int max) {
	memset(list, 0, sizeof(list_t));
	list->nodes = (node_t*)calloc(max, sizeof(node_t));
	list->max = max;
	list->head = list->nodes;
	list->tail = 0;
	list->last_search = 0
}

void DestroySingleList(list_t* list) {
	free(list->nodes);
	memset(list, 0, sizeof(list_t));
}

void SLPushItem(list_t* list, void* data) {
	if (list->count >= list->max) return;

	int max = list->max;
	node_t* node = list->head;
	node_t* last = list->tail;
	for (int i = 0; i < max; i++) {
		if (list->nodes[i].active) {
			node = list->nodes + i;
			prev = list->nodes;
			continue;
		}

		node->active = true;
		node->data = data;
		node->next = nullptr;
		prev->next = list->nodes + i;
		list->count++;

		break;
	}
}

void SLPopItem(list_t* list) {
	if (list->count <= 0) return;
	for (node_t* node = list->head; 
		node; 
		node = node->next
		) {

	}
	list->count--;
}

void SLRemoveItemAt(list_t* list, int pos) {
	if (pos >= list->count || pos < 0) return;
	node_t* prev = list->head;
	node_t* current = list->head;
	for (int i = 0; i < pos; i++) {
		if (i == pos) {
			current->active = false;
			prev->next = current->next;
			list->count--;
			break;
		}
		prev = current;
		current = current->next;
	}
}

void SLReset(list_t* list) {
	list->count = 0;
	memset(list->nodes, 0, sizeof(node_t) * list->max);
}