#include "List.hpp"
#include <stdint.h>

int main() {
	list_t<int, 10> list;
	InitSingleList(&list);

	for (int i = 1; i <= 10; i++) {
		node_t<int>* n = SLPushItem(&list);
		n->data = i * 10;
	}

	SLRemoveItemAt(&list, 3);
	SLPopItem(&list);

	for (node_t<int>* node = list.head; node; node = node->next) {
		printf("Node val: %d\n", node->data);
	}

	for (node_t<int>* node = list.head; node; node = node->next) {
		printf("Node this: 0x%p\n", node);
		printf("Node next: 0x%p\n", node->next);
	}
}
