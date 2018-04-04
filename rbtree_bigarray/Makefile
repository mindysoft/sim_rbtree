exefile : linked_list.o buddy_allocator.o rbtree_group.o
	gcc -o exefile linked_list.o buddy_allocator.o rbtree_group.o -lm

linked_list.o : linked_list.h linked_list.c
	gcc -c -o linked_list.o linked_list.c -lm

buddy_allocator.o : linked_list.h buddy_allocator.h buddy_allocator.c
	gcc -c -o buddy_allocator.o buddy_allocator.c -lm

rbtree_group.o : linked_list.h rbtree_group.h buddy_allocator.h rbtree_group.c
	gcc -c -o rbtree_group.o rbtree_group.c -lm

clean :
	rm *.o exefile

