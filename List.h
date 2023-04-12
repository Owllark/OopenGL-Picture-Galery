#pragma once
#include <stdlib.h>

typedef struct pictureInf {
	char picName[255];
	char path[255];
	char description[1024];
};

typedef struct Picture {
	struct pictureInf* inf;
	struct Picture* next;
};

class List
{
	
	struct Picture* getLast(struct Picture* head) {
		if (head == NULL) {
			return NULL;
		}
		while (head->next) {
			head = head->next;
		}
		return head;
	}
	void pushBack(struct Picture* head, struct pictureInf* inf) {
		struct Picture* last = getLast(head);
		struct Picture* tmp = (struct Picture*)malloc(sizeof(int));
		tmp->inf = inf;
		tmp->next = NULL;
		last->next = tmp;
	}
	void popBack(struct Picture* head) {
		if (head->next == NULL) {
			return;
		}
		struct Picture* cur = head->next;
		struct Picture* prev = head;
		while (cur->next) {
			cur = cur->next;
			prev = prev->next;
		}
		prev->next = NULL;
		return;
		free(cur);
	}
};

 