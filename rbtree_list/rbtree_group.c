#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "rbtree_group.h"
#include "buddy_allocator.h"

struct simul_swap_node swap_head;
unsigned long long int swap_count = 0;
unsigned long long int cnt_node = 0;
unsigned long long int cnt_key = 0;
int GROUP_SIZE = 0; 
int WSS = 0;
unsigned long long int ENABLE_MEM_SIZE = (1024 * 1024 * 1024);	// 8 GB => this means 8 * 1GB / 8;  "/ 8" -> 64bit machine.
unsigned long long int write_count = 0;
unsigned long long int remain_count = 0;


simul_rb_node *parent_of(simul_rb_node *node) {
	return node->rb_parent;
}

void simul_free(simul_rb_node *node) {
	free(node);
}

simul_rb_node *right_of(simul_rb_node *node) {
	return node->rb_right;
}

simul_rb_node *left_of(simul_rb_node *node) {
	return node->rb_left;
}

void init_swap_node() {
	swap_head.node = NULL;
	swap_head.prev = &swap_head;
	swap_head.next = &swap_head;
}

void init_rb_key(simul_rb_key *key_node, long long int key) {
	key_node->rb_key = key;
	init_list_head(&(key_node->list));
//	key_node->area= (struct simul_area *)malloc(sizeof(struct simul_area));
/*
	key_node->area= (struct simul_area *)smalloc(sizeof(struct simul_area));
	(*(key_node->area->addr))++;
*/
}

void init_rb_node(simul_rb_root *T, simul_rb_node *node, simul_rb_key *key_node) {
	set_rb_red(node);
	node->std_key = key_node->rb_key / (GROUP_SIZE);
	init_list_head(&(node->rb_keys));
	list_add(&(key_node->list), &(node->rb_keys));
	node->cnt_key = 1;
//	node->rb_keys[key_node->rb_key % (GROUP+SIZE)].area = key_node->area;
	node->rb_parent = T->nil_node;
	node->rb_right = T->nil_node;
	node->rb_left = T->nil_node;
	node->count = 1;
	node->is_free = 0;
//	node->area = (struct simul_area *)malloc(sizeof(struct simul_area));
/*
	node->area = (struct simul_area *)smalloc(sizeof(struct simul_area));
	(*(node->area->addr))++;
*/
}

void init_rb_root(simul_rb_root *root) {
	root->nil_node = (simul_rb_node *)malloc(sizeof(simul_rb_node));
	root->root_node = (simul_rb_node *)malloc(sizeof(simul_rb_node));

	set_rb_black(root->nil_node);
	root->nil_node->std_key = SIMUL_RB_NIL;
	init_list_head(&(root->nil_node->rb_keys));
	root->nil_node->cnt_key = 0;
	root->nil_node->rb_parent = root->nil_node;
	root->nil_node->rb_right = root->nil_node;
	root->nil_node->rb_left = root->nil_node;
	root->nil_node->count = 1;
	root->nil_node->is_free = 0;
//	root->nil_node->area = (struct simul_area *)malloc(sizeof(struct simul_area));
/*
	root->nil_node->area = (struct simul_area *)smalloc(sizeof(struct simul_area));
	*(root->nil_node->area->addr) = 1;
*/
	set_rb_black(root->root_node);
	root->root_node->std_key = SIMUL_RB_NIL;
	init_list_head(&(root->root_node->rb_keys));
	root->root_node->cnt_key = 0;
	root->root_node->rb_parent = root->nil_node;
	root->root_node->rb_left = root->nil_node;
	root->root_node->rb_right = root->nil_node;
	root->root_node->count = 1;
	root->root_node->is_free = 0;
//	root->root_node->area = (struct simul_area *)malloc(sizeof(struct simul_area));
/*
	root->root_node->area = (struct simul_area *)smalloc(sizeof(struct simul_area));
	*(root->root_node->area->addr) = 1;
*/
}

unsigned int is_rb_black(simul_rb_node *node) {
	return node->rb_color == SIMUL_RB_BLACK ? 1 : 0;
}

unsigned int is_rb_red(simul_rb_node *node) {
	return node->rb_color == SIMUL_RB_RED ? 1 : 0;
}

void set_rb_black(simul_rb_node *node) {
	node->rb_color = SIMUL_RB_BLACK;
}

void set_rb_red(simul_rb_node *node) {
	node->rb_color = SIMUL_RB_RED;
}

simul_rb_root *simul_rb_alloc_root(void) {
	simul_rb_root *ret = (simul_rb_root *)malloc(sizeof(simul_rb_root));
	return ret;
}

simul_rb_node *simul_rb_left_rotate(simul_rb_root *T, simul_rb_node *node) {
	simul_rb_node *ret = NULL;
	simul_rb_node *right_node = right_of(node);

	/* exchange left child of node_right to right child of node */
	node->rb_right = left_of(right_node);
#if DEBUG_WRITE_COUNT
	node->count++;   // for analysis
#endif
//	(*(node->area->addr))++;
//	add_swap_list(node);

	if (left_of(right_node) != T->nil_node) {
		left_of(right_node)->rb_parent = node;
#if DEBUG_WRITE_COUNT
		left_of(right_node)->count++;   // for analysis
#endif
//		(*(right_node->rb_left->area->addr))++;
//		add_swap_list(right_node->rb_left);
	}

	/* change parent connection */
	right_node->rb_parent = parent_of(node);
#if DEBUG_WRITE_COUNT
	right_node->count++;   // for analysis
#endif
//	(*(right_node->area->addr))++;
//	add_swap_list(right_node);

	if (node == parent_of(node)->rb_left) {   /* case: node is left child of parent */
		parent_of(node)->rb_left = right_node;
#if DEBUG_WRITE_COUNT
		parent_of(node)->count++;   // for analysis
#endif
//		(*(node->rb_parent->area->addr))++;
//		add_swap_list(node->rb_parent);
	}
	else if (node == parent_of(node)->rb_right) {   /* case: node is right child of parent */
		parent_of(node)->rb_right = right_node;
#if DEBUG_WRITE_COUNT
		parent_of(node)->count++;   // for analysis
#endif
//		(*(node->rb_parent->area->addr))++;
//		add_swap_list(node->rb_parent);
	}

	right_node->rb_left = node;
#if DEBUG_WRITE_COUNT
	right_node->count++;   // for analysis
#endif
//	(*(right_node->area->addr))++;
//	add_swap_list(right_node);
	node->rb_parent = right_node;
#if DEBUG_WRITE_COUNT
	node->count++;   // for analysis
#endif
//	(*(node->area->addr))++;
//	add_swap_list(node);

	ret = node;

	return ret;
}

simul_rb_node *simul_rb_right_rotate(simul_rb_root *T, simul_rb_node *node) {
	simul_rb_node *ret = NULL;
	simul_rb_node *left_node = left_of(node);

	/* exchange right child of node_left to left child of node */
	node->rb_left = right_of(left_node);
#if DEBUG_WRITE_COUNT
	node->count++;   // for analysis
#endif
//	(*(node->area->addr))++;
//	add_swap_list(node);

	if (right_of(left_node) != T->nil_node) {
		right_of(left_node)->rb_parent = node;
#if DEBUG_WRITE_COUNT
		right_of(left_node)->count++;   // for analysis
#endif
//		(*(left_node->rb_right->area->addr))++;
//		add_swap_list(left_node->rb_right);
	}

	/* change parent connection */
	left_node->rb_parent = parent_of(node);
#if DEBUG_WRITE_COUNT
	left_node->count++;   // for analysis
#endif
//	(*(left_node->area->addr))++;
//	add_swap_list(left_node);

	if (node == parent_of(node)->rb_left) {   /* case: node is left child of parent */
		parent_of(node)->rb_left = left_node;
#if DEBUG_WRITE_COUNT
		parent_of(node)->count++;   // for analysis
#endif
//		(*(node->rb_parent->area->addr))++;
//		add_swap_list(node->rb_parent);
	}
	else if (node == parent_of(node)->rb_right) {   /* case: node is right child of parent */
		parent_of(node)->rb_right = left_node;
#if DEBUG_WRITE_COUNT
		parent_of(node)->count++;   // for analysis
#endif
//		(*(node->rb_parent->area->addr))++;
//		add_swap_list(node->rb_parent);
	}

	left_node->rb_right = node;
#if DEBUG_WRITE_COUNT
	left_node->count++;   // for analysis
#endif
//	(*(left_node->area->addr))++;
//	add_swap_list(left_node);
	node->rb_parent = left_node;
#if DEBUG_WRITE_COUNT
	node->count++;   // for analysis
#endif
//	(*(node->area->addr))++;
//	add_swap_list(node);

	ret = node;

	return ret;
}

/* change parent of chg_node from parent of pre_node */
unsigned int simul_rb_transplant(simul_rb_root *T, simul_rb_node *pre_node, simul_rb_node *chg_node) {
	unsigned int ret = 0;

	if (parent_of(pre_node) == T->nil_node)   /* case: node is root node */
		T->root_node = chg_node;
	else if (pre_node == parent_of(pre_node)->rb_left)   /* case: node is left child of parent */
		parent_of(pre_node)->rb_left = chg_node;
	else if (pre_node == parent_of(pre_node)->rb_right)   /* case: node is right child of parent */
		parent_of(pre_node)->rb_right = chg_node;

	chg_node->rb_parent = parent_of(pre_node);   /* change chg_node's parent */

	return ret;
}

simul_rb_node *simul_rb_minimum(simul_rb_node *node) {
	simul_rb_node *min_node = node;
	while (left_of(min_node)->std_key != SIMUL_RB_NIL)
		min_node = left_of(min_node);

	return min_node;
}

simul_rb_node *simul_rb_successor(simul_rb_root* T, simul_rb_node *node) {
	simul_rb_node *y;
	simul_rb_node *nil = T->nil_node;
	simul_rb_node *root = T->root_node;

	if (nil != (y = node->rb_right)) {
		while (y->rb_left != nil) {
			y = y->rb_left;
		}
		return (y);
	}
	else {
		y = node->rb_parent;
		while (node == y->rb_right) {
			node = y;
			y = y->rb_parent;
		}
		if (y == root) return (nil);
		return (y);
	}
}


void simul_rb_swapping(simul_rb_root *T) {
/*
	simul_rb_node *new_node = NULL;
	struct simul_swap_node *cur_swap_node = swap_head.next, *del_swap_node = NULL;

	while (cur_swap_node != &swap_head) {
		if (cur_swap_node->node->is_free == 0) {
			swap_count++;
			new_node = (simul_rb_node *)malloc(sizeof(simul_rb_node));
			new_node->rb_color = cur_swap_node->node->rb_color;
			new_node->std_key = cur_swap_node->node->std_key;
			new_node->rb_keys.next = cur_swap_node->node->rb_keys.next;
			new_node->rb_keys.next->prev = &(new_node->rb_keys);
			new_node->rb_keys.prev = cur_swap_node->node->rb_keys.prev;
			new_node->rb_keys.prev->next = &(new_node->rb_keys);
			new_node->rb_parent = cur_swap_node->node->rb_parent;
			new_node->rb_left = cur_swap_node->node->rb_left;
			new_node->rb_right = cur_swap_node->node->rb_right;
//			new_node->area = (struct simul_area *)malloc(sizeof(struct simul_area));
*/
/*
			new_node->area = (struct simul_area *)smalloc(sizeof(struct simul_area));
			(*(new_node->area->addr))++;
*/
/*
			new_node->is_free = 0;
			new_node->count = 1;
			
			// parent pointer 
			if (cur_swap_node->node == T->root_node)
				T->root_node = new_node;
			else if (cur_swap_node->node->rb_parent == T->root_node) 	// old_node is root
				T->root_node->rb_left = new_node;
			else if (cur_swap_node->node->rb_parent->rb_left == cur_swap_node->node) 	// old_node is parent's left child
				cur_swap_node->node->rb_parent->rb_left = new_node;
			else if (cur_swap_node->node->rb_parent->rb_right == cur_swap_node->node)	// old_node is parent's right child
				cur_swap_node->node->rb_parent->rb_right = new_node;
			cur_swap_node->node->rb_parent = NULL;	// old_node's parent pointer is clear.

													//children pointer 
			if (cur_swap_node->node->rb_left != T->nil_node)	// old_node's left is exist
				cur_swap_node->node->rb_left->rb_parent = new_node;
			cur_swap_node->node->rb_left = NULL;	// old_node's left pointer is clear.
			if (cur_swap_node->node->rb_right != T->nil_node)	// old_node's right is exist
				cur_swap_node->node->rb_right->rb_parent = new_node;
			cur_swap_node->node->rb_right = NULL;	// old_node's left pointer is clear.
		}
		del_swap_node = cur_swap_node;
		cur_swap_node = cur_swap_node->next;
		del_swap_node->next->prev = del_swap_node->prev;
		del_swap_node->prev->next = del_swap_node->next;
		del_swap_node->node->rb_keys.next = NULL;
		del_swap_node->node->rb_keys.prev = NULL;
		
		//sfree(del_swap_node->node->area);
//		free(del_swap_node->node->area);
		free(del_swap_node->node);
		del_swap_node->next = NULL;
		del_swap_node->prev = NULL;
		del_swap_node->node = NULL;
		free(del_swap_node);
	}
*/
}

void add_swap_list(simul_rb_node *node) {
	struct simul_swap_node *new_swap = NULL, *cur_node = swap_head.next;
	if (node->is_free == 1 || node->count >= SWAP_COUNT) {
		while (cur_node != &swap_head) {
			if (node->std_key == cur_node->node->std_key)
				return;
			cur_node = cur_node->next;
		}
		new_swap = (struct simul_swap_node *)malloc(sizeof(struct simul_swap_node));
		new_swap->node = node;
		new_swap->prev = &swap_head;
		new_swap->next = swap_head.next;
		new_swap->next->prev = new_swap;
		new_swap->prev->next = new_swap;
	}
}

int simul_rb_insert(simul_rb_root *T, long long int inkey) {
	int ret = 0, is_in_group = 0;
	simul_rb_node *comp_node = T->root_node->rb_left;
	simul_rb_node *in_parent_pos = T->root_node;
	simul_rb_node *new_node = (simul_rb_node *)malloc(sizeof(simul_rb_node));
	long long int searching_inkey = inkey / (GROUP_SIZE);
	simul_rb_key *new_key = NULL;

	while (comp_node != T->nil_node) {
		in_parent_pos = comp_node;
		if (searching_inkey < comp_node->std_key)
			comp_node = left_of(comp_node);
		else if (searching_inkey > comp_node->std_key)
			comp_node = right_of(comp_node);
		else if (searching_inkey == comp_node->std_key) {
			is_in_group = 1;
			break;
		}
	}

//	if (is_in_group == 1 && comp_node->rb_keys[inkey % (GROUP_SIZE)].rb_key != 0)
//		return SIMUL_RB_EXIST;
	
	new_key = (simul_rb_key *)malloc(sizeof(simul_rb_key));
	init_rb_key(new_key, inkey);
	cnt_key++;

	if (is_in_group == 1) {
		list_add(&(new_key->list), &(comp_node->rb_keys));	// connect list : second -> first -> second->next
	//	comp_node->rb_keys[new_key->rb_key % GROUP_SIZE].area = new_key->area;
		comp_node->cnt_key++;
		return SIMUL_RB_IN_GROUP;
	}
	
	// init node
	init_rb_node(T, new_node, new_key);
	cnt_node++;

	new_node->rb_parent = in_parent_pos;

	if (in_parent_pos == T->root_node) {   /* the first insert (rbtree is empty) */
		T->root_node->rb_left = new_node;
#if DEBUG_WRITE_COUNT
		T->root_node->count++;   // for analysis
#endif
//		(*(T->root_node->area->addr))++;
	}
	else if (new_node->std_key < in_parent_pos->std_key) {
		in_parent_pos->rb_left = new_node;
#if DEBUG_WRITE_COUNT
		in_parent_pos->count++;   // for analysis
#endif
//		(*(in_parent_pos->area->addr))++;
	}
	else {
		in_parent_pos->rb_right = new_node;
#if DEBUG_WRITE_COUNT
		in_parent_pos->count++;   // for analysis
#endif
//		(*(in_parent_pos->area->addr))++;
	}

	simul_rb_insert_fixup(T, new_node);

	return ret;
}

void simul_rb_insert_fixup(simul_rb_root *T, simul_rb_node *alt_node) {
	simul_rb_node *across_parent = NULL;

	while (is_rb_red(parent_of(alt_node))) {
		if (parent_of(alt_node) == parent_of(alt_node)->rb_parent->rb_left) {

			across_parent = parent_of(alt_node)->rb_parent->rb_right;

			if (is_rb_red(across_parent)) {   /* case 1: color of across is red */
				set_rb_black(parent_of(alt_node));
				set_rb_black(across_parent);
				set_rb_red(parent_of(alt_node->rb_parent));
				alt_node = parent_of(alt_node->rb_parent);
			}
			else {
				if (alt_node == parent_of(alt_node)->rb_right) {   /* case 2: color of across is black && alt_node is right child */
					alt_node = parent_of(alt_node);
					alt_node = simul_rb_left_rotate(T, alt_node);
				}
				/* case 3: color of across is black && alt_node is left child */
				set_rb_black(parent_of(alt_node));
				set_rb_red(parent_of(alt_node->rb_parent));
				simul_rb_right_rotate(T, parent_of(alt_node->rb_parent));
			}
		}
		else if (parent_of(alt_node) == parent_of(alt_node)->rb_parent->rb_right) {

			across_parent = parent_of(alt_node)->rb_parent->rb_left;

			if (is_rb_red(across_parent)) {   /* case 1: color of across is red */
				set_rb_black(parent_of(alt_node));
				set_rb_black(across_parent);
				set_rb_red(parent_of(alt_node->rb_parent));
				alt_node = parent_of(alt_node->rb_parent);
			}
			else {
				if (alt_node == parent_of(alt_node)->rb_left) {   /* case 2: color of across is black && alt_node is right child */
					alt_node = parent_of(alt_node);
					alt_node = simul_rb_right_rotate(T, alt_node);
				}
				/* case 3: color of across is black && alt_node is left child */
				set_rb_black(parent_of(alt_node));
				set_rb_red(parent_of(alt_node->rb_parent));
				simul_rb_left_rotate(T, parent_of(alt_node->rb_parent));
			}
		}
	}

	set_rb_black(T->root_node->rb_left); /* rule 2: color of root node is black */
}

int simul_rb_delete(simul_rb_root *T, long long int delkey) {
	int ret = 0, is_in_group = 0;
	simul_rb_node *del_node = simul_rb_search(T, delkey);
	simul_rb_node *y;
	simul_rb_node *x;
	simul_rb_key *pre_cur_key = NULL, *cur_key = NULL;
	struct simul_list_head *cur_list = NULL;

	if (del_node == T->nil_node)
		return SIMUL_RB_NOT_EXIST;

	pre_cur_key = &(del_node->rb_keys);
	cur_key = container_of(del_node->rb_keys.next, simul_rb_key, list);
	cur_list = del_node->rb_keys.next;

	while (cur_list != &del_node->rb_keys) {
		if (cur_key->rb_key == delkey) {
			is_in_group = 1;
			break;
		}
		pre_cur_key = cur_key;
		cur_key = container_of(cur_key->list.next, simul_rb_key, list);
		cur_list = cur_list->next;
	}

	if (is_in_group == 0)
		return SIMUL_RB_NOT_EXIST;

	list_del(&(cur_key->list));
	free(cur_key);
	del_node->cnt_key--;
	if (del_node->cnt_key > 0) 
		return SIMUL_RB_IN_GROUP;
	
	y = ((del_node->rb_left == T->nil_node) || (del_node->rb_right == T->nil_node)) ? del_node : simul_rb_successor(T, del_node);
	x = (y->rb_left == T->nil_node) ? y->rb_right : y->rb_left;

#if DEBUG_WRITE_COUNT
	x->count++;   // for analysis
#endif
//	(*(x->area->addr))++;
	if (T->root_node == (x->rb_parent = y->rb_parent)) {/* assignment of y->p to x->p is intentional */
		T->root_node->rb_left = x;
#if DEBUG_WRITE_COUNT
		T->root_node->count++;   // for analysis
#endif
//		(*(T->root_node->area->addr))++;
	}
	else {
		if (y == y->rb_parent->rb_left) {
			y->rb_parent->rb_left = x;
#if DEBUG_WRITE_COUNT
			y->rb_parent->count++;   // for analysis
#endif
//			(*(y->rb_parent->area->addr))++;
		}
		else {
			y->rb_parent->rb_right = x;
#if DEBUG_WRITE_COUNT
			y->rb_parent->count++;   // for analysis
#endif
//			(*(y->rb_parent->area->addr))++;
		}
	}

	if (y != del_node) {
		if (is_rb_black(y))
			simul_rb_delete_fixup(T, x);

		y->rb_left = del_node->rb_left;
#if DEBUG_WRITE_COUNT
		y->count++;   // for analysis
#endif
//		(*(y->area->addr))++;
		y->rb_right = del_node->rb_right;
#if DEBUG_WRITE_COUNT
		y->count++;   // for analysis
#endif
//		(*(y->area->addr))++;
		y->rb_parent = del_node->rb_parent;
#if DEBUG_WRITE_COUNT
		y->count++;   // for analysis
#endif
//		(*(y->area->addr))++;
		y->rb_color = del_node->rb_color;
		del_node->rb_left->rb_parent = del_node->rb_right->rb_parent = y;
#if DEBUG_WRITE_COUNT
		del_node->rb_left->count++;      // for analysis
#endif
//		(*(del_node->rb_left->area->addr))++;
#if DEBUG_WRITE_COUNT
		del_node->rb_right->count++;   // for analysis
#endif
//		(*(del_node->rb_right->area->addr))++;
		if (del_node == del_node->rb_parent->rb_left) {
			del_node->rb_parent->rb_left = y;
#if DEBUG_WRITE_COUNT
			del_node->rb_parent->count++;   // for analysis
#endif
//			(*(del_node->rb_parent->area->addr))++;
		}
		else {
			del_node->rb_parent->rb_right = y;
#if DEBUG_WRITE_COUNT
			del_node->rb_parent->count++;   // for analysis
#endif
//			(*(del_node->rb_parent->area->addr))++;
		}

#if DEBUG_WRITE_COUNT
		write_count += del_node->count;
#endif
		free(del_node);

//		del_node->is_free = 1;
//		add_swap_list(del_node);
	}
	else {
		if (is_rb_black(y))
			simul_rb_delete_fixup(T, x);

#if DEBUG_WRITE_COUNT
		write_count += y->count;
#endif
		free(y);
//		y->is_free = 1;
//		add_swap_list(y);
	}
	T->nil_node->rb_parent = T->nil_node;
#if DEBUG_WRITE_COUNT
	T->nil_node->count++;   // for analysis
#endif
	T->nil_node->rb_left = T->nil_node;
#if DEBUG_WRITE_COUNT
	T->nil_node->count++;   // for analysis
#endif
	T->nil_node->rb_right = T->nil_node;
#if DEBUG_WRITE_COUNT
	T->nil_node->count++;   // for analysis
#endif
	return ret;
}

void simul_rb_delete_fixup(simul_rb_root *T, simul_rb_node *chg_node) {
	simul_rb_node *bro_node = NULL;
	while (chg_node != T->root_node && is_rb_black(chg_node)) {

		if (chg_node == parent_of(chg_node)->rb_left) {
			bro_node = parent_of(chg_node)->rb_right;

			if (is_rb_red(bro_node)) { /* case 1: color of bro node is red */
				set_rb_black(bro_node);
				set_rb_red(parent_of(chg_node));
				simul_rb_left_rotate(T, parent_of(chg_node));
				bro_node = parent_of(chg_node)->rb_right;
			}

			if (is_rb_black(left_of(bro_node)) && is_rb_black(right_of(bro_node))) {   /* case 2: color of bro node is black && both of bro's children are black */
				set_rb_red(bro_node);
				chg_node = parent_of(chg_node);

			}
			else {
				if (is_rb_black(right_of(bro_node))) {   /* case 3: bro is black && left child is red && right child is black */
					set_rb_black(left_of(bro_node));
					set_rb_red(bro_node);
					simul_rb_right_rotate(T, bro_node);
					bro_node = parent_of(chg_node)->rb_right;
				}
				bro_node->rb_color = parent_of(chg_node)->rb_color;
				set_rb_black(parent_of(chg_node));
				set_rb_black(right_of(bro_node));
				simul_rb_left_rotate(T, parent_of(chg_node));
				chg_node = T->root_node;
			}
		}
		else {
			bro_node = parent_of(chg_node)->rb_left;

			if (is_rb_red(bro_node)) { /* case 1: color of bro node is red */
				set_rb_black(bro_node);
				set_rb_red(parent_of(chg_node));
				simul_rb_right_rotate(T, parent_of(chg_node));
				bro_node = parent_of(chg_node)->rb_left;
			}

			if (is_rb_black(left_of(bro_node)) && is_rb_black(right_of(bro_node))) {   /* case 2: color of bro node is black && both of bro's children are black */
				set_rb_red(bro_node);
				chg_node = parent_of(chg_node);

			}
			else {
				if (is_rb_black(left_of(bro_node))) {   /* case 3: bro is black && right child is red && left child is black */
					set_rb_black(right_of(bro_node));
					set_rb_red(bro_node);
					simul_rb_left_rotate(T, bro_node);
					bro_node = parent_of(chg_node)->rb_left;
				}
				bro_node->rb_color = parent_of(chg_node)->rb_color;
				set_rb_black(parent_of(chg_node));
				set_rb_black(left_of(bro_node));
				simul_rb_right_rotate(T, parent_of(chg_node));
				chg_node = T->root_node;
			}
		}
	} //while
	set_rb_black(chg_node);
}

simul_rb_node *simul_rb_search(simul_rb_root *T, long long int find_key) {
	simul_rb_node *search_node = T->root_node->rb_left;
	long long int find_std_key = find_key / (GROUP_SIZE);

	while (search_node != T->nil_node) {
		if (find_std_key == search_node->std_key)
			break;

		if (find_std_key < search_node->std_key)
			search_node = left_of(search_node);
		else
			search_node = right_of(search_node);
	}

	return search_node;
}


simul_rb_key *simul_rb_search_key(simul_rb_root *T, long long int find_key) {
	simul_rb_node *search_node = simul_rb_search(T, find_key);
	simul_rb_key *pre_cur_key = NULL, *cur_key = NULL;
	struct simul_list_head *cur_list = NULL;

	if (search_node == T->nil_node)
		return NULL;

	pre_cur_key = &(search_node->rb_keys);
	cur_key = container_of(search_node->rb_keys.next, simul_rb_key, list);
	cur_list = search_node->rb_keys.next;

	while (cur_list != &search_node->rb_keys) {
		if (cur_key->rb_key == find_key) {
			break;
		}
		pre_cur_key = cur_key;
		cur_key = container_of(cur_key->list.next, simul_rb_key, list);
		cur_list = cur_list->next;
	}

	return cur_key;
} 

void simul_rb_pre_order(simul_rb_root *T, simul_rb_node *node) {
//	printf("addr %p \tcount %u\n", node, node->count);
#if DEBUG_WRITE_COUNT
	write_count += node->count;
#endif
	if (node->rb_left != T->nil_node)
		simul_rb_pre_order(T, node->rb_left);
	if (node->rb_right != T->nil_node)
		simul_rb_pre_order(T, node->rb_right);
}

void simul_rb_delete_all(simul_rb_root *T, simul_rb_node *node) {
	simul_rb_key *cur_key = container_of(node->rb_keys.next, simul_rb_key, list);
	simul_rb_key *pre_key = &(node->rb_keys);
	struct simul_list_head *cur_list = node->rb_keys.next;

	if (node == T->nil_node)
		return;

	if (node->rb_left != T->nil_node)
		simul_rb_delete_all(T, node->rb_left);
	if (node->rb_right != T->nil_node)
		simul_rb_delete_all(T, node->rb_right);
	//sfree(node->area);
	//free(node->area);
#if DEBUG_WRITE_COUNT
	remain_count += node->count;
#endif
	while (cur_list != &node->rb_keys) {
		pre_key = cur_key;
		cur_key = container_of(cur_key->list.next, simul_rb_key, list);
		cur_list = cur_list->next;
		free(pre_key);
	}

	free(node);
}


void output_sum(char *fn) {
	char output_fn[60];
	FILE *fp = NULL;
	int i = 0;
	unsigned long long int sum = 0;

	strcpy(output_fn, fn);
	strcat(output_fn, ".orig_group_rb.sum.txt");

	fp = fopen(output_fn, "a+t");

	for (i = 0; i < MEM_POOL_SIZE; i++)
		sum += sfull_mem[i];
	fprintf(fp, "%llu\n", sum);

	fclose(fp);
}

void output_mem(char *fn, int line) {
	char output_fn[60];
	FILE *fp = NULL;
	int i = 0;

	sprintf(output_fn, "%s.orig_group_rb.mem.%d.txt", fn, line);

	fp = fopen(output_fn, "w+t");

	for (i = 0; i < MEM_POOL_SIZE; i++) {
		if (sfull_mem[i] == 0)
			continue;
		fprintf(fp, "%d\n", sfull_mem[i]);
	}
	fclose(fp);
}

void output_var(char *fn) {
	char output_fn[60];
	FILE *fp = NULL;
	int i = 0;
	unsigned long long int sum = 0,cnt = 0;
	double mean, var;

	strcpy(output_fn, fn);
	strcat(output_fn, ".orig_group_rb.var.txt");

	fp = fopen(output_fn, "a+t");

	for (i = 0; i < MEM_POOL_SIZE; i++) {
		if (sfull_mem[i] == 0)
			continue;
		sum += sfull_mem[i];
		cnt++;
	}

	mean = sum / cnt;

	sum = 0;
	cnt = 0;
	for (i = 0; i < MEM_POOL_SIZE; i++) {
		if (sfull_mem[i] == 0)
			continue;
		sum += ((sfull_mem[i] - mean)*(sfull_mem[i] - mean));
		cnt++;
	}
	var = sum / cnt;

	fprintf(fp, "%.0lf %.0lf %.0lf\n", mean, var, sqrt(var));

	fclose(fp);
}

void output_swap(char *fn) {
	char output_fn[60];
	FILE *fp = NULL;
	int i = 0;
	unsigned long long int sum = 0;

	strcpy(output_fn, fn);
	strcat(output_fn, ".orig_group_rb.swap.txt");

	fp = fopen(output_fn, "a+t");

	fprintf(fp, "%llu\n", swap_count);

	fclose(fp);
}

int main(int argc, char *argv[])
{
	simul_rb_root *root = NULL;
	simul_rb_node *node = NULL;
	simul_rb_key *search_node = NULL;
	FILE *fp = NULL;
	long long int in_key = -1, i = 0, *rvalue;
	char in_type[20], fn[40];
	int retval = -1, endpoint = 0, line = 1000;
	time_t t;
	struct timeval start, end, instart, inend, delstart, delend, serstart, serend;

	// for research
	unsigned long long int sizeofmem = 0, insert_write_count = 0, delete_write_count = 0;
	double transmemsize = 0.0;
	int bytelevel = 0;
	char byteunit[5][4] = {"B", "KB", "MB", "GB", "TB"};
	char rfn[100];
	
	if (argc < 3) {
		printf (" [RBTREE SIM] execute error.\n");
		printf (" [RBTREE SIM] usage : %s #ofGROUP #ofWSS [|rand] [|load key range]\n [RBTREE SIM] example : %s 10 100000, %s 10 100000 rand, %s 10 100000 rand load 5 or %s 10 100000 5\n [RBTREE SIM] try again.\n", argv[0], argv[0], argv[0], argv[0], argv[0]);
		return 1;
	}

	GROUP_SIZE = atoi(argv[1]); 
	WSS = atoi(argv[2]);
	if (GROUP_SIZE < 1) {
		printf(" [RBTREE SIM] GROUP size error.\n [RBTREE SIM] # of GROUP must be greater than 0.\n [RBTREE SIM] try again.\n");
		return 1;
	}

	srand((unsigned) time(&t));
	gettimeofday(&start, NULL);


	root = simul_rb_alloc_root();

//	for_debug_init_sfull_memory();
//	swap_count = 0;
//	init_swap_node();
	init_rb_root(root);

	if(argc > 3 &&  strcmp(argv[3], "rand") == 0) {
                printf(" -- run random mode -- \n");

		rvalue = (long long int *)malloc(sizeof(long long int) * WSS);
		if (argc > 4 && strcmp(argv[4], "load") == 0) {
			printf(" -- load random value (range %s) --\n", argv[5]);
			strcpy(rfn, "../rvalues/rvalue_");
			strcat(rfn, argv[5]);
			fp = fopen(rfn, "r");
			for (i = 0; i < WSS; i++)	
				fscanf(fp, "%lld", &rvalue[i]);
			fclose(fp);
		}	

		gettimeofday(&instart, NULL);
                for(i = 0; i<WSS;i++){
			long long int key;
			if (argc < 5)
	                        key = rand() % ENABLE_MEM_SIZE;
			else if (strcmp(argv[4], "load") == 0) 
				key = rvalue[i];
			else
                        	key = rand() % ((WSS)*atoi(argv[4]));
                        retval = simul_rb_insert(root, (long long int)key);
                }
		gettimeofday(&inend, NULL);
#if DEBUG_WRITE_COUNT
		simul_rb_pre_order(root, root->root_node->rb_left);	
		insert_write_count = write_count;
#endif		
		gettimeofday(&serstart, NULL);
		for(i = 0; i<WSS;i++){
			long long int key;
			if (argc < 5)
	                        key = rand() % ENABLE_MEM_SIZE;
			else if (strcmp(argv[4], "load") == 0)
				key = rvalue[i];
			else
                        	key = rand() % ((WSS)*atoi(argv[4]));
                        search_node = simul_rb_search_key(root, (long long int)key);
                }
		gettimeofday(&serend, NULL);

		gettimeofday(&delstart, NULL);
                for(i = 0; i<WSS;i++){
			long long int key;
			if (argc < 5)
	                        key = rand() % ENABLE_MEM_SIZE;
			else if (strcmp(argv[4], "load") == 0)
				key = rvalue[i];
			else
                        	key = rand() % ((WSS)*atoi(argv[4]));
                        retval = simul_rb_delete(root, (long long int)key);
                }
		gettimeofday(&delend, NULL);
#if DEBUG_WRITE_COUNT
		delete_write_count = write_count - insert_write_count;
		simul_rb_delete_all(root, root->root_node->rb_left);
#endif
        } else {
		printf(" -- run sequential mode -- \n");
		gettimeofday(&instart, NULL);
		for(i = 0; i<WSS;i++){
			retval = simul_rb_insert(root, (long long int)i);
	//		simul_rb_swapping(root);
		}
		gettimeofday(&inend, NULL);
#if DEBUG_WRITE_COUNT
		simul_rb_pre_order(root, root->root_node->rb_left);	
		insert_write_count = write_count;
#endif	
		gettimeofday(&serstart, NULL);
		for(i = 0; i<WSS;i++){
                        search_node = simul_rb_search_key(root, (long long int)i);
                }
		gettimeofday(&serend, NULL);
	
		gettimeofday(&delstart, NULL);
		for(i = 0; i<WSS;i++){
			retval = simul_rb_delete(root, (long long int)i);
	//		simul_rb_swapping(root);
		}
		gettimeofday(&delend, NULL);
#if DEBUG_WRITE_COUNT
		delete_write_count = write_count - insert_write_count;
		simul_rb_delete_all(root, root->root_node->rb_left);
#endif
	}
	gettimeofday(&end, NULL);

	sizeofmem = (cnt_node * (sizeof(unsigned int) + sizeof(long long int) + (sizeof(struct simul_rb_node *)*3) + sizeof(unsigned int))) + (cnt_key * (sizeof(struct simul_list_head)));
	while (sizeofmem / 1024 > 0) {
		transmemsize = (double)sizeofmem / 1024;
		sizeofmem /= 1024;
		bytelevel++;
	}

	printf(" number of node : %llu\n", cnt_node);
#if DEBUG_WRITE_COUNT
	printf(" insert's total write : %llu\n", insert_write_count);
	printf(" delete's total write : %llu\n", delete_write_count);

	if(argc > 3 &&  strcmp(argv[3], "rand") == 0) 
		printf(" total write count : %llu\n", delete_write_count + remain_count);
	else
		printf(" total write count : %llu\n", insert_write_count + delete_write_count);

#endif 
	printf(" size of tree memory : %.4lf %s\n", transmemsize, byteunit[bytelevel]);
	
	printf(" Retrieve time: %6.2f s\n", serend.tv_sec - serstart.tv_sec + (serend.tv_usec - serstart.tv_usec) / 1000000.0);
	printf(" Insertion time: %6.2f s\n", inend.tv_sec - instart.tv_sec + (inend.tv_usec - instart.tv_usec) / 1000000.0);
	printf(" Deletion time: %6.2f s\n", delend.tv_sec - delstart.tv_sec + (delend.tv_usec - delstart.tv_usec) / 1000000.0);
	printf(" Elapsed time: %6.2f s\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.0);

/*

//	init_free_area(free_area);
//	init_alloc_area(alloc_area);

	strcpy(fn, "trace2.result");

	while (1) {
		for_debug_init_sfull_memory();
		swap_count = 0;
		init_swap_node();
		init_rb_root(root);
		fp = fopen(fn, "r");
		printf("line %d\n", line);
		for (i = 0; i < line; i++) {
			fscanf(fp, "%s %llu\n", in_type, &in_key);
			if (strcmp(in_type, "insert") == 0)
				retval = simul_rb_insert(root, in_key);
			if (strcmp(in_type, "erase") == 0)
				retval = simul_rb_delete(root, in_key);
			simul_rb_swapping(root);
			if (feof(fp)) {
				endpoint = 1;
				break;
			}

		}
		line += 1000;
		fclose(fp);

		//simul_rb_pre_order(root, root->root_node->rb_left);

		simul_rb_delete_all(root, root->root_node->rb_left);
		printf("end %d\n", line - 1000);
		printf("swap_count : %llu\n", swap_count);
		output_sum(fn);
		output_mem(fn, i);
		output_var(fn);
		output_swap(fn);
		if (endpoint == 1)
			break;
		for_debug_sfull_memory();
		sfree(root->nil_node->area);
		sfree(root->root_node->area);
	}
	printf("\n%d %d\n", root->root_node->area->start, root->nil_node->area->start);


	gettimeofday(&end, NULL);
	printf("Elapsed time: %6.2f s\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.0);
*/
	return 0;
}
