#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "rbtree.h"
#include "buddy_allocator.h"

//#define WSS	1000000

unsigned long long int cnt_node = 0;
int WSS = 0;
unsigned long long int ENABLE_MEM_SIZE = (1024 * 1024 * 1024);  // 8 GB => 8 * 1024 * 1024 * 1024 / 8; "/ 8" means 64bit machine.

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

void init_rb_node(simul_rb_root *T, simul_rb_node *node, long long int key) {
	set_rb_red(node);
	node->rb_key = key;
	node->rb_parent = T->nil_node;
	node->rb_right = T->nil_node;
	node->rb_left = T->nil_node;
	node->count = 1;
	node->area = (struct simul_area *)malloc(sizeof(struct simul_area));
/*
	node->area = (struct simul_area *)smalloc(sizeof(struct simul_area));
	(*(node->area->addr))++;
*/
}

void init_rb_root(simul_rb_root *root) {
	root->nil_node = (simul_rb_node *)malloc(sizeof(simul_rb_node));
	root->root_node = (simul_rb_node *)malloc(sizeof(simul_rb_node));

	set_rb_black(root->nil_node);
	root->nil_node->rb_key = SIMUL_RB_NIL;
	root->nil_node->rb_parent = root->nil_node;
	root->nil_node->rb_right = root->nil_node;
	root->nil_node->rb_left = root->nil_node;
	root->nil_node->count = 1;
	root->nil_node->area = (struct simul_area *)malloc(sizeof(struct simul_area));
/*
	root->nil_node->area = (struct simul_area *)smalloc(sizeof(struct simul_area));
	*(root->nil_node->area->addr) = 1;
*/
	set_rb_black(root->root_node);
	root->root_node->rb_key = SIMUL_RB_NIL;
	root->root_node->rb_parent = root->nil_node;
	root->root_node->rb_left = root->nil_node;
	root->root_node->rb_right = root->nil_node;
	root->root_node->count = 1;
	root->root_node->area = (struct simul_area *)malloc(sizeof(struct simul_area));
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

unsigned int simul_rb_left_rotate(simul_rb_root *T, simul_rb_node *node) {
	unsigned int ret = 0;
	simul_rb_node *right_node = right_of(node);

	/* exchange left child of node_right to right child of node */
	node->rb_right = left_of(right_node);
	node->count++;	// for analysis
//	(*(node->area->addr))++;

	if (left_of(right_node) != T->nil_node) {
		left_of(right_node)->rb_parent = node;
		left_of(right_node)->count++;	// for analysis
//		(*(right_node->rb_left->area->addr))++;
	}

	/* change parent connection */
	right_node->rb_parent = parent_of(node);
	right_node->count++;	// for analysis
//	(*(right_node->area->addr))++;

	if (node == parent_of(node)->rb_left) {	/* case: node is left child of parent */
		parent_of(node)->rb_left = right_node;
		parent_of(node)->count++;	// for analysis
//		(*(node->rb_parent->area->addr))++;
	}
	else if (node == parent_of(node)->rb_right) {	/* case: node is right child of parent */
		parent_of(node)->rb_right = right_node;
		parent_of(node)->count++;	// for analysis
//		(*(node->rb_parent->area->addr))++;
	}

	right_node->rb_left = node;
	right_node->count++;	// for analysis
//	(*(right_node->area->addr))++;
	node->rb_parent = right_node;
	node->count++;	// for analysis
//	(*(node->area->addr))++;

	return ret;
}

unsigned int simul_rb_right_rotate(simul_rb_root *T, simul_rb_node *node) {
	unsigned int ret = 0;
	simul_rb_node *left_node = left_of(node);

	/* exchange right child of node_left to left child of node */
	node->rb_left = right_of(left_node);
	node->count++;	// for analysis
//	(*(node->area->addr))++;

	if (right_of(left_node) != T->nil_node) {
		right_of(left_node)->rb_parent = node;
		right_of(left_node)->count++;	// for analysis
//		(*(left_node->rb_right->area->addr))++;
	}

	/* change parent connection */
	left_node->rb_parent = parent_of(node);
	left_node->count++;	// for analysis
//	(*(left_node->area->addr))++;

	if (node == parent_of(node)->rb_left) {	/* case: node is left child of parent */
		parent_of(node)->rb_left = left_node;
		parent_of(node)->count++;	// for analysis
//		(*(node->rb_parent->area->addr))++;
	}
	else if (node == parent_of(node)->rb_right) {	/* case: node is right child of parent */
		parent_of(node)->rb_right = left_node;
		parent_of(node)->count++;	// for analysis
//		(*(node->rb_parent->area->addr))++;
	}

	left_node->rb_right = node;
	left_node->count++;	// for analysis
//	(*(left_node->area->addr))++;
	node->rb_parent = left_node;
	node->count++;	// for analysis
//	(*(node->area->addr))++;

	return ret;
}

/* change parent of chg_node from parent of pre_node */
unsigned int simul_rb_transplant(simul_rb_root *T, simul_rb_node *pre_node, simul_rb_node *chg_node) {
	unsigned int ret = 0;

	if (parent_of(pre_node) == T->nil_node)	/* case: node is root node */
		T->root_node = chg_node;
	else if (pre_node == parent_of(pre_node)->rb_left)	/* case: node is left child of parent */
		parent_of(pre_node)->rb_left = chg_node;
	else if (pre_node == parent_of(pre_node)->rb_right)	/* case: node is right child of parent */
		parent_of(pre_node)->rb_right = chg_node;

	chg_node->rb_parent = parent_of(pre_node);	/* change chg_node's parent */

	return ret;
}

simul_rb_node *simul_rb_minimum(simul_rb_node *node) {
	simul_rb_node *min_node = node;
	while (left_of(min_node)->rb_key != SIMUL_RB_NIL)
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

int simul_rb_insert(simul_rb_root *T, long long int inkey) {
	int ret = 0;
	simul_rb_node *comp_node = T->root_node->rb_left;
	simul_rb_node *in_parent_pos = T->root_node;
	simul_rb_node *new_node = (simul_rb_node *)malloc(sizeof(simul_rb_node));

	while (comp_node != T->nil_node) {
		in_parent_pos = comp_node;

		if (inkey < comp_node->rb_key)
			comp_node = left_of(comp_node);
		else if (inkey > comp_node->rb_key)
			comp_node = right_of(comp_node);
		else if (inkey == comp_node->rb_key)
			return SIMUL_RB_EXIST;
	}

	init_rb_node(T, new_node, inkey);
	new_node->rb_parent = in_parent_pos;
	cnt_node++;

	if (in_parent_pos == T->root_node) {	/* the first insert (rbtree is empty) */
		T->root_node->rb_left = new_node;
		T->root_node->count++;	// for analysis
//		(*(T->root_node->area->addr))++;
	}
	else if (new_node->rb_key < in_parent_pos->rb_key) {
		in_parent_pos->rb_left = new_node;
		in_parent_pos->count++;	// for analysis
//		(*(in_parent_pos->area->addr))++;
	}
	else {
		in_parent_pos->rb_right = new_node;
		in_parent_pos->count++;	// for analysis
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

			if (is_rb_red(across_parent)) {	/* case 1: color of across is red */
				set_rb_black(parent_of(alt_node));
				set_rb_black(across_parent);
				set_rb_red(parent_of(alt_node->rb_parent));
				alt_node = parent_of(alt_node->rb_parent);
			}
			else {
				if (alt_node == parent_of(alt_node)->rb_right) {	/* case 2: color of across is black && alt_node is right child */
					alt_node = parent_of(alt_node);
					simul_rb_left_rotate(T, alt_node);
				}
				/* case 3: color of across is black && alt_node is left child */
				set_rb_black(parent_of(alt_node));
				set_rb_red(parent_of(alt_node->rb_parent));
				simul_rb_right_rotate(T, parent_of(alt_node->rb_parent));
			}
		}
		else if (parent_of(alt_node) == parent_of(alt_node)->rb_parent->rb_right) {

			across_parent = parent_of(alt_node)->rb_parent->rb_left;

			if (is_rb_red(across_parent)) {	/* case 1: color of across is red */
				set_rb_black(parent_of(alt_node));
				set_rb_black(across_parent);
				set_rb_red(parent_of(alt_node->rb_parent));
				alt_node = parent_of(alt_node->rb_parent);
			}
			else {
				if (alt_node == parent_of(alt_node)->rb_left) {	/* case 2: color of across is black && alt_node is right child */
					alt_node = parent_of(alt_node);
					simul_rb_right_rotate(T, alt_node);
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
	int ret = 0;
	simul_rb_node *del_node = simul_rb_search(T, delkey);
	simul_rb_node *y;
	simul_rb_node *x;

	if (del_node == T->nil_node)
		return SIMUL_RB_NOT_EXIST;

	y = ((del_node->rb_left == T->nil_node) || (del_node->rb_right == T->nil_node)) ? del_node : simul_rb_successor(T, del_node);
	x = (y->rb_left == T->nil_node) ? y->rb_right : y->rb_left;

	x->count++;	// for analysis
//	(*(x->area->addr))++;
	if (T->root_node == (x->rb_parent = y->rb_parent)) {/* assignment of y->p to x->p is intentional */
		T->root_node->rb_left = x;
		T->root_node->count++;	// for analysis
//		(*(T->root_node->area->addr))++;
	}
	else {
		if (y == y->rb_parent->rb_left) {
			y->rb_parent->rb_left = x;
			y->rb_parent->count++;	// for analysis
//			(*(y->rb_parent->area->addr))++;
		}
		else {
			y->rb_parent->rb_right = x;
			y->rb_parent->count++;	// for analysis
//			(*(y->rb_parent->area->addr))++;
		}
	}

	if (y != del_node) {
		if (is_rb_black(y))
			simul_rb_delete_fixup(T, x);

		y->rb_left = del_node->rb_left;
		y->count++;	// for analysis
//		(*(y->area->addr))++;
		y->rb_right = del_node->rb_right;
		y->count++;	// for analysis
//		(*(y->area->addr))++;
		y->rb_parent = del_node->rb_parent;
		y->count++;	// for analysis
//		(*(y->area->addr))++;
		y->rb_color = del_node->rb_color;
		del_node->rb_left->rb_parent = del_node->rb_right->rb_parent = y;
		del_node->rb_left->count++;		// for analysis
//		(*(del_node->rb_left->area->addr))++;
		del_node->rb_right->count++;	// for analysis
//		(*(del_node->rb_right->area->addr))++;
		if (del_node == del_node->rb_parent->rb_left) {
			del_node->rb_parent->rb_left = y;
			del_node->rb_parent->count++;	// for analysis
//			(*(del_node->rb_parent->area->addr))++;
		}
		else {
			del_node->rb_parent->rb_right = y;
			del_node->rb_parent->count++;	// for analysis
//			(*(del_node->rb_parent->area->addr))++;
		}
		//printf("addr %p\t count %u\n", del_node, del_node->count);
		//sfree(del_node->area);
		free(del_node->area);
		free(del_node);
	}
	else {
		if (is_rb_black(y))
			simul_rb_delete_fixup(T, x);
		//printf("addr %p\t count %u\n", y, y->count);
		//sfree(y->area);
		free(y->area);
		free(y);
	}

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

			if (is_rb_black(left_of(bro_node)) && is_rb_black(right_of(bro_node))) {	/* case 2: color of bro node is black && both of bro's children are black */
				set_rb_red(bro_node);
				chg_node = parent_of(chg_node);

			}
			else {
				if (is_rb_black(right_of(bro_node))) {	/* case 3: bro is black && left child is red && right child is black */
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

			if (is_rb_black(left_of(bro_node)) && is_rb_black(right_of(bro_node))) {	/* case 2: color of bro node is black && both of bro's children are black */
				set_rb_red(bro_node);
				chg_node = parent_of(chg_node);

			}
			else {
				if (is_rb_black(left_of(bro_node))) {	/* case 3: bro is black && right child is red && left child is black */
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
	T->nil_node->rb_parent = T->nil_node;
	T->nil_node->count++;	// for analysis
	T->nil_node->rb_left = T->nil_node;
	T->nil_node->count++;	// for analysis
	T->nil_node->rb_right = T->nil_node;
	T->nil_node->count++;	// for analysis

	set_rb_black(chg_node);
}

simul_rb_node *simul_rb_search(simul_rb_root *T, long long int find_key) {
	simul_rb_node *search_node = T->root_node->rb_left;

	while (search_node != T->nil_node) {
		if (find_key == search_node->rb_key)
			break;

		if (find_key < search_node->rb_key)
			search_node = left_of(search_node);
		else
			search_node = right_of(search_node);
	}

	return search_node;
}
void simul_rb_pre_order(simul_rb_root *T, simul_rb_node *node) {
	printf("addr %p \tcount %u\n", node, node->count);
	if (node->rb_left != T->nil_node)
		simul_rb_pre_order(T, node->rb_left);
	if (node->rb_right != T->nil_node)
		simul_rb_pre_order(T, node->rb_right);
}

void simul_rb_delete_all(simul_rb_root *T, simul_rb_node *node) {
	if (node->rb_left != T->nil_node)
		simul_rb_delete_all(T, node->rb_left);
	if (node->rb_right != T->nil_node)
		simul_rb_delete_all(T, node->rb_right);
	//sfree(node->area);
	free(node->area);
	free(node);
}

void output_sum(char *fn) {
	char output_fn[60];
	FILE *fp = NULL;
	int i = 0;
	unsigned long long int sum = 0;

	strcpy(output_fn, fn);
	strcat(output_fn, ".orig_rb.sum.txt");

	fp = fopen(output_fn, "a+t");

	for (i = 0; i < MEM_POOL_SIZE; i++)
		sum += sfull_mem[i];
	fprintf(fp, "%llu\n", sum);

	fclose(fp);
}

void output_mem(char *fn, int line){
	char output_fn[60];
	FILE *fp = NULL;
	int i = 0;

	sprintf(output_fn, "%s.orig_rb.mem.%d.txt", fn, line);

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
	unsigned long long int sum = 0, cnt = 0;
	double mean, var;

	strcpy(output_fn, fn);
	strcat(output_fn, ".orig_rb.var.txt");

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

int main(int argc, char *argv[])
{
	simul_rb_root *root = NULL;
	simul_rb_node *node = NULL;
	FILE *fp = NULL;
	long long int in_key = -1, i = 0;
	char in_type[20], fn[40];
	int retval = -1, endpoint = 0, line = 1000;
        time_t t;
        struct timeval start, end, instart, inend, delstart, delend;

        // for research
        unsigned long long int sizeofmem = 0;
        double transmemsize = 0.0;
        int bytelevel = 0;
        char byteunit[5][4] = {"B", "KB", "MB", "GB", "TB"};


        if (argc < 2) {
                printf (" [RBTREE SIM] execute error.\n");
                printf (" [RBTREE SIM] usage : %s #ofWSS [|rand]\n [RBTREE SIMUL] try again.\n", argv[0]);
                return 1;
        }

        WSS = atoi(argv[1]);

	// micro bench	

        srand((unsigned) time(&t));
        gettimeofday(&start, NULL);

        init_free_area(free_area);
        init_alloc_area(alloc_area);

        root = simul_rb_alloc_root();

        for_debug_init_sfull_memory();
        init_rb_root(root);

	if(argc > 2 && 	strcmp(argv[2], "rand") == 0) {
		printf(" -- run random mode -- \n");
		gettimeofday(&instart, NULL);
        	for(i = 0; i<WSS;i++){
//			long long int key = rand() % WSS;	// key range is small
			long long int key = rand() % ENABLE_MEM_SIZE;
                	retval = simul_rb_insert(root, (long long int)key);
       		}	
		gettimeofday(&inend, NULL);
		gettimeofday(&delstart, NULL);
       		for(i = 0; i<WSS;i++){
//			long long int key = rand() % WSS;	// key range is small
			long long int key = rand() % ENABLE_MEM_SIZE;
       	        	retval = simul_rb_delete(root, (long long int)key);
       		}
		gettimeofday(&delend, NULL);
		
	} else {
		printf(" -- run sequential mode -- \n");
		gettimeofday(&instart, NULL);
        	for(i = 0; i<WSS;i++){
                	retval = simul_rb_insert(root, (long long int)i);
       		}
		gettimeofday(&inend, NULL);
		gettimeofday(&delstart, NULL);
       		for(i = 0; i<WSS;i++){
       	        	retval = simul_rb_delete(root, (long long int)i);
       		}
		gettimeofday(&delend, NULL);
	}

	sizeofmem = cnt_node * (sizeof(unsigned int) + sizeof(long long int) + (sizeof(struct simul_rb_node *)*3));
        while (sizeofmem / 1024 > 0) {
                transmemsize = sizeofmem / 1024;
                sizeofmem /= 1024;
                bytelevel++;
        }
        gettimeofday(&end, NULL);

        printf(" number of node : %llu\n", cnt_node);
        printf(" size of tree memory : %.4lf %s\n", transmemsize, byteunit[bytelevel]);

	printf(" Insertion time: %6.2f s\n", inend.tv_sec - instart.tv_sec + (inend.tv_usec - instart.tv_usec) / 1000000.0);
	printf(" Deletion time: %6.2f s\n", delend.tv_sec - delstart.tv_sec + (delend.tv_usec - delstart.tv_usec) / 1000000.0);
        printf(" Elapsed time: %6.2f s\n", end.tv_sec - start.tv_sec + (end.tv_usec - start.tv_usec) / 1000000.0);



	// file read
/*
	if (argc < 2) {
		printf(" execute error.\n");
		printf(" usage : %s key_file\n", argv[0]);
		return 1;
	}

	strcpy(fn, argv[1]);
*/
//	init_free_area(free_area);
//	init_alloc_area(alloc_area);

//	root = simul_rb_alloc_root();
/*
	strcpy(fn, "trace2.result");
	while (1) {
		for_debug_init_sfull_memory();
		init_rb_root(root);
		fp = fopen(fn, "r");
		printf("line %d\n", line);
		for (i = 0; i < line; i++) {
			fscanf(fp, "%s %llu\n", in_type, &in_key);
			if (strcmp(in_type, "insert") == 0)
				retval = simul_rb_insert(root, in_key);
			if (strcmp(in_type, "erase") == 0)
				retval = simul_rb_delete(root, in_key);
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
		output_sum(fn);
		output_mem(fn, i);
		output_var(fn);
		if (endpoint == 1)
			break;
		for_debug_sfull_memory();
		sfree(root->nil_node->area);
		sfree(root->root_node->area);
		
	}
	printf("\n%d %d\n", root->root_node->area->start, root->nil_node->area->start);
*/
	return 0;
}

