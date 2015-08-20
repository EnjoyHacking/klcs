#include "pattern_search.h"

void build_lps_array(char *pattern, int M, int *lps) {

	int len = 0; // length of the previous longest prefix suffix
	int i = 1;
	lps[0] = 0; // lps[0] is always 0


	while (i < M) {
		if (pattern[i] == pattern[len]) {
			len++;
			lps[i] = len;
			i++;
		} else {
			if (len != 0) {
				len = lps[len - 1];
			} else {
				lps[i] = 0;
				i++;
			}
		}
	}


}

int*  kmp_search(char *pattern, char *target) {

	if(!pattern || !target) {
		return ;
	}

	int M = strlen(pattern);
	int N = strlen(target);

	int *indices = (int *) malloc (sizeof(int) * N);
	int index;
	for(index = 0; index < N; index++){
		indices[index] = -1;
	}
	index = 0;

	/* create lps array that will hold the longest prefix suffix values for pattern*/

	int *lps = (int *) malloc(sizeof(int) * M);

	/* preprocess the pattern to calculate lps array */
	build_lps_array(pattern, M, lps);

	int j = 0; // index for pattern
	int i = 0; // index for target

	while(i < N) {
		if (pattern[j] == target[i]) {
			i++;
			j++;
		}

		if (j == M) {
			printf("Info: Found pattern <%s> at index '%d' of <%s> \n", pattern, i-j+1, target);	
			indices[index++] = i - j + 1;
			j = lps[j - 1];
		} else if (i < N && pattern[j] != target[i]) { // mismatch after j matches

			if (j != 0) {
				j = lps[i - 1] ;
			} else {
				i = i + 1;
			}
		
		}
	}

	free(lps); // to avoid memory leak

	return indices;
}


/*

int main() {
	char *target = "ABABDABACDABABCABAB";
	char *pattern = "AB";
	int *indices = kmp_search(pattern, target);

	int index = 0;
	while(indices[index] != -1 && index < strlen(target)) {
		printf("%d \t", indices[index++]);
	}	

	printf("\n");

	free(indices);

	return 0;
}
*/
