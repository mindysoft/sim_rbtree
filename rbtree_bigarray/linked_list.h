#pragma once

#define container_of(ptr, type, member) \
   (type *)((char *)(ptr) - (char *) &((type *)0)->member)

struct simul_list_head {
	struct simul_list_head *prev;
	struct simul_list_head *next;
};

void init_list_head(struct simul_list_head *list);
void list_add(struct simul_list_head *new, struct simul_list_head *head);
void __list_add(struct simul_list_head *new, struct simul_list_head *prev, struct simul_list_head *next);
void list_add_tail(struct simul_list_head *new, struct simul_list_head *head);
void __list_del(struct simul_list_head * prev, struct simul_list_head * next);
void __list_del_entry(struct simul_list_head *entry);
void list_del(struct simul_list_head *entry);