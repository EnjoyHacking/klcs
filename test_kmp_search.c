#include "pattern_search.h"


int main() {
	char *target = "ABABDABACDABABCABAB";
	char *pattern = "ABc";

	printf("target_len : %d \n", strlen(target));
	printf("pattern_len : %d \n", strlen(pattern));

	int *indices = kmp_search(pattern, target);

	if(!indices)
		return 0;
	if(-1 == indices[0]) {
		return 0;
	}

	int index = 0;
	while(indices[index] != -1 && index < strlen(target)) {
		printf("%d \t", indices[index++]);
	}	

	printf("\n");

	free(indices);

	return 0;
}
