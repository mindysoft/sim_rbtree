#ifndef _SIMUL_RBTREE_H
#define _SIMUL_RBTREE_H
#include "buddy_allocator.h"
#define SIMUL_RB_RED		0
#define SIMUL_RB_BLACK	1
#define SIMUL_INIT_KEY	-1
#define SIMUL_RB_NIL		-2
#define SIMUL_RB_NOT_EXIST	-3
#define SIMUL_RB_EXIST	-4

typedef struct simul_rb_node {
	unsigned int rb_color;
	long long int rb_key;
	struct simul_rb_node *rb_parent;
	struct simul_rb_node *rb_right;
	struct simul_rb_node *rb_left;
	unsigned int count;
	struct simul_area *area;
} simul_rb_node;

typedef struct simul_rb_root {
	simul_rb_node *root_node;
	simul_rb_node *nil_node;
} simul_rb_root;

simul_rb_node *parent_of(simul_rb_node *node);
simul_rb_node *right_of(simul_rb_node *node);
simul_rb_node *left_of(simul_rb_node *node);

void init_rb_node(simul_rb_root *T, simul_rb_node *node, long long int key);
void init_rb_root(simul_rb_root *root);

unsigned int is_rb_black(simul_rb_node *node);
unsigned int is_rb_red(simul_rb_node *node);

void set_rb_black(simul_rb_node *node);
void set_rb_red(simul_rb_node *node);

simul_rb_root *simul_rb_alloc_root(void);
unsigned int simul_rb_left_rotate(simul_rb_root *T, simul_rb_node *node);
unsigned int simul_rb_right_rotate(simul_rb_root *T, simul_rb_node *node);
unsigned int simul_rb_transplant(simul_rb_root *T, simul_rb_node *pre_node, simul_rb_node *chg_node);
simul_rb_node *simul_rb_minimum(simul_rb_node *node);
simul_rb_node *simul_rb_successor(simul_rb_root* T, simul_rb_node *node);

int simul_rb_insert(simul_rb_root *T, long long int inkey);
void simul_rb_insert_fixup(simul_rb_root *T, simul_rb_node *alt_node);
int simul_rb_delete(simul_rb_root *T, long long int delkey);
void simul_rb_delete_fixup(simul_rb_root *T, simul_rb_node *chg_node);

simul_rb_node *simul_rb_search(simul_rb_root *T, long long int find_key);
void simul_rb_pre_order(simul_rb_root *T, simul_rb_node *node);
void simul_rb_delete_all(simul_rb_root *T, simul_rb_node *node);

void output_sum(char *fn);
void output_mem(char *fn, int line);
void output_var(char *fn);
#endif