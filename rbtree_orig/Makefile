exefile : linked_list.o buddy_allocator.o rbtree.o
	gcc -o exefile linked_list.o buddy_allocator.o rbtree.o -lm

linked_list.o : linked_list.h linked_list.c
	gcc -c -o linked_list.o linked_list.c -lm

buddy_allocator.o : linked_list.h buddy_allocator.h buddy_allocator.c
	gcc -c -o buddy_allocator.o buddy_allocator.c -lm

rbtree.o : linked_list.h rbtree.h buddy_allocator.h rbtree.c
	gcc -c -o rbtree.o rbtree.c -lm

clean :
	rm *.o exefile

