#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
	int *check = NULL;
	int r, cnt = 0;	

	if (argc < 2) {
		printf(" [GEN ERROR] execute error\n");
		printf(" [GEN ERROR] usage. %s WSS key range\n", argv[0]);
		printf(" [GEN ERROR] example. %s 1000000 5\n", argv[0]);
		return 0;
	}

	srand(time(NULL));
	check = (int *)malloc(sizeof(int) * atoi(argv[1]) * atoi(argv[2]));
	memset (check, 0, atoi(argv[1]) * atoi(argv[2]));
	
	while ( cnt != atoi(argv[1])) {
		r = rand() % (atoi(argv[1]) * atoi(argv[2]));
		if (check[r] == 0) {
			check[r] = 1;
			printf("%d\n", r);
			cnt++;	
		}
	}
	
	return 0;
}
