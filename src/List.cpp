#include "List.hpp"
#include <string.h>

void InitSingleList(list_t* list, int max) {
	memset(list, 0, sizeof(list_t));
	list->nodes = (node_t*)calloc(max, sizeof(node_t));
	list->max = max;
	list->head = list->nodes;
	list->last_search = 0;
	list->count = 0;
}

void DestroySingleList(list_t* list) {
	free(list->nodes);
	memset(list, 0, sizeof(list_t));
}

node_t* SLPushItem(list_t* list, void* data) {
	if (list->count >= list->max) return nullptr;

	int max = list->max, max2 = max;
	node_t* node = nullptr;
	int i = list->last_search;
	int start = i;
search:
	// Look for space
	for (; i < max2; i++) {
		if (!list->nodes[i].active) {
			// Free node
			node = list->nodes + i;

			node->active = true;
			node->data = data;
			node->next = nullptr;
			list->last_search = (i + 1 >= max) ? 0 : i + 1;

			goto look_for_node;
		}
	}

	i = 0;
	max2 = start;
	goto search;

look_for_node:
	// Look for the last node
	for (node_t* idx = list->head; ; ) {
		if (list->count == 0) {
			break;
		}
		else if (idx->next == 0) {
			idx->next = node;
			break;
		}
		idx = idx->next;
	}

	// Finally node is found and that stuff
	list->count++;
	return node;
}

void SLPopItem(list_t* list) {
	if (list->count <= 0) return;
	node_t* prev = 0;
	for (node_t* node = list->head; ; ) {
		if (node->next == 0) {
			if (prev == 0) {
				node->active = false;
				break;
			}
			else {
				node->active = false;
				prev->next = 0;
				break;
			}
		}
		prev = node;
		node = node->next;
	}
	list->count--;
}

void SLRemoveItemAt(list_t* list, int pos) {
	if (pos >= list->count || pos < 0) return;
	node_t* prev = 0;
	int i = 0;
	for (node_t* node = list->head; ; ) {
		if (i == pos) {
			if (prev == 0) {
				node->active = false;
				list->head = node->next;
				break;
			}
			else {
				node->active = false;
				prev->next = node->next;
				break;
			}
		}
		prev = node;
		node = node->next;
		i++;
	}
	list->count--;
}

void SLReset(list_t* list) {
	memset(list->nodes, 0, sizeof(node_t) * list->max);
	list->count = 0;
	list->head = list->nodes;
	list->last_search = 0;
}