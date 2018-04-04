#include "linked_list.h"
#include <stdio.h>

void init_list_head(struct simul_list_head *list)
{
	list->next = list;
	list->prev = list;
}

void list_add(struct simul_list_head *new, struct simul_list_head *head) {
	__list_add(new, head, head->next);
}

void __list_add(struct simul_list_head *new, struct simul_list_head *prev, struct simul_list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}
void list_add_tail(struct simul_list_head *new, struct simul_list_head *head) {
	__list_add(new, head->prev, head);
}
void __list_del(struct simul_list_head * prev, struct simul_list_head * next) {
	next->prev = prev;
	prev->next = next;
}

void __list_del_entry(struct simul_list_head *entry) {
	__list_del(entry->prev, entry->next);
}

void list_del(struct simul_list_head *entry) {
	__list_del(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}