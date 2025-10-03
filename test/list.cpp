#include "List.hpp"
#include <stdint.h>

int main() {
	list_t list;
	InitSingleList(&list, 10);

	for (int i = 1; i <= 10; i++) {
		SLPushItem(&list, (void*)(uint64_t)(i * 10));
	}

	SLRemoveItemAt(&list, 3);
	SLPopItem(&list);

	for (node_t* node = list.head; node; node = node->next) {
		printf("Node val: %d\n", (uint64_t)node->data);
	}

	for (node_t* node = list.head; node; node = node->next) {
		printf("Node this: 0x%p\n", node);
		printf("Node next: 0x%p\n", node->next);
	}
}
