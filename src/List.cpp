#include "List.hpp"
#include <string.h>

void InitSingleList(list_t* list, int max) {
	memset(list, 0, sizeof(list_t));
	list->nodes = (node_t*)calloc(max, sizeof(node_t));
	list->max = max;
	list->head = list->tail = list->nodes;
	list->search_next = 0;
}

void DestroySingleList(list_t* list) {
	free(list->nodes);
	memset(list, 0, sizeof(list_t));
}

void SLPushItem(list_t* list, void* data) {
	if (list->count >= list->max) return;

	int max = list->max;
	int i = list->search_next;
	for (; i < max; i++) {
		if (list->nodes[i].active) continue;

		list->search_next = i + 1;
		list->nodes[i].active = true;
		list->nodes[i].data = data;
		list->nodes[i].next = 0;
		list->tail = list->nodes + i;
		goto end_push;
	}

	if (0 != list->search_next) {
		max = list->search_next;
		for (int i = 0; i < max; i++) {
			if (list->nodes[i].active) continue;

			list->search_next = i + 1;
			list->nodes[i].active = true;
			list->nodes[i].data = data;
			list->nodes[i].next = 0;
			list->tail = list->nodes + i;
			break;
		}
	}

end_push:
	list->count++;
}

void SLPopItem(list_t* list) {
	if (list->count <= 0) return;
	list->tail->active = false;
	list->tail = list->tail - 1;
	list->count--;
}

void SLRemoveItemAt(list_t* list, int pos) {

}

void SLReset(list_t* list) {
	list->tail = list->nodes;
	list->count = 0;
	memset(list->nodes, 0, sizeof(node_t) * list->max);
}