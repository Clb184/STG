#ifndef	LIST_INCLUDED
#define LIST_INCLUDED

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Single list
template<typename T>
struct node_t {
	bool active;
	T data;
	struct node_t<T>* next;
};

template<typename T, int max_space>
struct list_t {
	struct node_t<T>* head;
	struct node_t<T> nodes[max_space];
	int count;
	int last_search;
};

template<typename T, int max_space>
void InitSingleList(list_t<T, max_space>* list) {
	memset(list, 0, sizeof(list_t<T, max_space>));
	list->head = list->nodes;
	list->last_search = 0;
	list->count = 0;
}

template<typename T, int max_space>
void DestroySingleList(list_t<T, max_space>* list) {
	memset(list, 0, sizeof(list_t<T, max_space>));
}

template<typename T, int max_space>
node_t<T>* SLPushItem(list_t<T, max_space>* list, T data) {
	if (list->count >= max_space) return nullptr;

	int max = max_space, max2 = max_space;
	node_t<T>* node = nullptr;
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
	for (node_t<T>* idx = list->head; ; ) {
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

template<typename T, int max_space>
node_t<T>* SLPushItem(list_t<T, max_space>* list) {
	if (list->count >= max_space) return nullptr;

	int max = max_space, max2 = max_space;
	node_t<T>* node = nullptr;
	int i = list->last_search;
	int start = i;
search:
	// Look for space
	for (; i < max2; i++) {
		if (!list->nodes[i].active) {
			// Free node
			node = list->nodes + i;

			node->active = true;
			// node->data = { }; Maybe I can avoid doing stuff here for few extra cycles lol
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
	for (node_t<T>* idx = list->head; ; ) {
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

template<typename T, int max_space>
node_t<T>* SLPushItemEx(list_t<T, max_space>* list, T data, int cnt) {
	if (list->count >= max_space) return nullptr;
	if (list->count + cnt >= max_space) cnt = max_space - list->count;

	int max = max_space, max2 = max_space;
	node_t<T>* node = nullptr;
	int i = list->last_search;
	int start = i; 
	node_t<T>* idx = list->head;
	int cnt2 = 0;
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
			cnt2++;
			goto look_for_node;
		}
	}

	i = 0;
	max2 = start;
	goto search;

look_for_node:
	// Look for the last node
	for (; ; ) {
		if (list->count == 0) {
			if (cnt2 < cnt) {
				list->count++;
				goto search;
			}
			break;
		}
		else if (idx->next == 0) {
			idx->next = node;
			if (cnt2 < cnt) {
				list->count++;
				goto search;
			}
			break;
		}
		idx = idx->next;
	}

	// Finally node is found and that stuff
	list->count++;
	return node;
}

template<typename T, int max_space>
node_t<T>* SLPushItemEx(list_t<T, max_space>* list, int cnt) {
	if (list->count >= max_space) return nullptr;
	if (list->count + cnt >= max_space) cnt = max_space - list->count;

	int max = max_space, max2 = max_space;
	node_t<T>* node = nullptr;
	int i = list->last_search;
	int start = i;
	node_t<T>* idx = list->head;
	int cnt2 = 0;
search:
	// Look for space
	for (; i < max2; i++) {
		if (!list->nodes[i].active) {
			// Free node
			node = list->nodes + i;

			node->active = true;
			// node->data = { }; Maybe I can avoid doing stuff here for few extra cycles lol
			node->next = nullptr;
			list->last_search = (i + 1 >= max) ? 0 : i + 1;
			cnt2++;
			goto look_for_node;
		}
	}

	i = 0;
	max2 = start;
	goto search;

look_for_node:
	// Look for the last node
	for (; ; ) {
		if (list->count == 0) {
			if (cnt2 < cnt) {
				list->count++;
				goto search;
			}
			break;
		}
		else if (idx->next == 0) {
			idx->next = node;
			if (cnt2 < cnt) {
				list->count++;
				goto search;
			}
			break;
		}
		idx = idx->next;
	}

	// Finally node is found and that stuff
	list->count++;
	return node;
}

template<typename T, int max_space>
void SLPopItem(list_t<T, max_space>* list) {
	if (list->count <= 0) return;
	node_t<T>* prev = 0;
	for (node_t<T>* node = list->head; ; ) {
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

template<typename T, int max_space>
void SLRemoveItemAt(list_t<T, max_space>* list, int pos) {
	if (pos >= list->count || pos < 0) return;
	node_t<T>* prev = 0;
	int i = 0;
	for (node_t<T>* node = list->head; ; ) {
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

template<typename T, int max_space>
void SLReset(list_t<T, max_space>* list) {
	memset(list->nodes, 0, sizeof(node_t<T>) * max_space);
	list->count = 0;
	list->head = list->nodes;
	list->last_search = 0;
}

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