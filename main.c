#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <math.h>

#include "lst_structs.h"
#include "lst_stree.h"
#include "lst_string.h"
#include "lst_algorithms.h"

#include "position_constraints.h"
#include "product_distribution_model.h"
#include "convertion.h"
#include "merge_common_prefix.h"

#define ASSUMPTION_NUM_DISTINCT_STRINGS 256

#include "utils.h"
extern int fileCounter;

extern LST_StringSet * set;

/* We totally test K samples. */
int K;
/* We truncate every flow payload with 256 bytes. */
extern int N;  
/* We set the minimum substring length l_min = 2 for k-common substring extraction. */
int l_min;
/* We set the minimum coverage k_min = 10% * K for k-common substring extraction. */
//int k_min = (int) ceil(0.1 * K);
int k_min;
/* The follow three merge parameters all takes 5 for token merging. */
int alpha_merge;
int beta_merge;
int gamma_merge;
/*The k_offset with K means that we only consider position-specific tokens that appear in all the flows.*/
int k_offset; 

extern int global_max_len;

static void
test_usage(char *progname)
{
  printf("USAGE: %s minlen maxlen <strings ...>\n"
	 "This test program computes the longest common substring(s)\n"
	 "no longer than maxlen characters (or as long as possible if\n"
	 "maxlen is 0) of the strings provided on the command line.\n", progname);
  exit(0);
}



#if 1

int main(int argc, char **argv) {

	LST_STree     *tree;
	LST_StringSet *payloads, *tokens = NULL;
	u_int min_len, max_len;
	int k;
	char* dirName;

	if (argc <  4)
		test_usage(argv[0]);

	min_len = atoi(argv[1]);
	max_len = atoi(argv[2]);
	dirName = argv[3];

	/*The experimental paramter setting as follows.*/
	
	/* We truncate every flow payload with 256 bytes. */
	N = 16;  
	/* We set the minimum substring length l_min = 2 for k-common substring extraction. */
	l_min = min_len;
	/* The follow three merge parameters all takes 5 for token merging. */
	alpha_merge = 5;
	beta_merge = 5;
	gamma_merge = 5;
	/*The k_offset with K means that we only consider position-specific tokens that appear in all the flows.*/
	k_offset = K; 
	/* Create a string set to conveniently hold all our strings from a given directory */

	payloads = lst_stringset_new();

	dirScan(dirName, payloads);

	fprintf(stdout, "string set size : %d \n", payloads->size);

	fprintf(stdout, "fileCounter : %d \n", fileCounter);

	/* We totally test K samples. */
	K = payloads->size;
	/* We set the minimum coverage k_min = 10% * K for k-common substring extraction. */
	k_min = (int) ceil(0.15 * K);
	//k_min = K;


	if(!payloads) {
		printf("payloads is null\n");
		return 0;
	}
	lst_stringset_foreach(payloads, string_cb, "\n");


	/* Create a suffix tree for all strings in the set */
	tree = lst_stree_new(payloads);

	fprintf(stdout, "tree.string_index : %d \n", tree->string_index);

	/* Find k longest common substring(s) */
	u_int *num_distinct_strings = (u_int *) malloc(sizeof(u_int) * ASSUMPTION_NUM_DISTINCT_STRINGS);

	memset(num_distinct_strings, 0, ASSUMPTION_NUM_DISTINCT_STRINGS);

	//tokens = lst_alg_k_longest_common_substring(tree, l_min, 0, k_min, num_distinct_strings);

	tokens = lst_alg_first_k_longest_common_substring(tree, min_len, 0 , k_min, num_distinct_strings);

	//tokens = lst_alg_longest_common_substring(tree, min_len, max_len, num_distinct_strings);

	if(tokens == NULL){
		printf("Info: result is null.\n"); 
		return 0;
	}

	/* Print them out, if any. */
	if (tokens){
		printf("result size : %d \n", tokens->size);
		lst_stringset_foreach(tokens, str_encoded_cb, "\t");
		printf("\n");
	}
	/* Merge the substrings with common prefix before removing non-distinct substrings*/
	
	/*
	merge_common_prefix_main(tokens);

	if(set) {
		printf("set->size : %d\n", set->size);
		lst_stringset_foreach(set, string_cb, "\t");
		printf("\n");
	}
	*/


	/* 2. peform the second sub-module - introducing position constraints */
	Trie * trie = position_constraints_main(payloads, tokens, k_offset, beta_merge);

	printf("--------- 2. Introducting position constraints ----------\n");
	printf("\t\tToken \t\t Position-specific \t Replacement \t Associate Tokens \t Shortest Length \t Offset(occurrence) \n");
	trie_dfs(trie, print_callback, (void *)NULL);
	printf("---------------------------------------------------------\n");



	/* 3. perform the third sub-module - extracting single byte tokens using product distribution model*/

	int first_bytes = 4;
	int last_bytes = 4;
	int num_bytes = N;

	printf("--------- 3. Extracting single byte tokens ----------\n");
	product_distribution_main(trie, payloads, first_bytes, last_bytes, num_bytes, gamma_merge);

	trie_dfs(trie, print_callback, (void *)NULL);
	printf("-----------------------------------------------------\n");



	/* 4. convertion */
	printf("--------- 4. Convert raw payloads to the integer sequence ----------\n");
	convertion_main(trie, payloads);

	/* Free suffix tree: */
	lst_stree_free(tree);
	return 0;
}

#endif



