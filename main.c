#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "lst_structs.h"
#include "lst_stree.h"
#include "lst_string.h"
#include "lst_algorithms.h"

#include "position_constraints.h"

#define ASSUMPTION_NUM_DISTINCT_STRINGS 256

#include "utils.h"
extern int fileCounter;
static void
test_usage(char *progname)
{
  printf("USAGE: %s minlen maxlen <strings ...>\n"
	 "This test program computes the longest common substring(s)\n"
	 "no longer than maxlen characters (or as long as possible if\n"
	 "maxlen is 0) of the strings provided on the command line.\n", progname);
  exit(0);
}


void
node_cb(LST_Node *node, void *data)
{
  if (node->up_edge)
    printf("Node: %u, string index: %i\n", node->id,
	   lst_stree_get_string_index((LST_STree *) data, node->up_edge->range.string));
}



void checkForSubString(LST_STree *tree, LST_String  *string)
{
    int res = lst_alg_substring_check(tree, string);
    if(res == 1)
	    printf("Pattern <%s> is a Substring\n", lst_string_print(string));
    else
	    printf("Pattern <%s> is NOT a Substring\n", lst_string_print(string));
}


#if 1

int main(int argc, char **argv) {

	LST_STree     *tree;
	LST_StringSet *payloads, *tokens;
	u_int min_len, max_len;
	int k;
	char* dirName;

	if (argc <  5)
		test_usage(argv[0]);

	min_len = atoi(argv[1]);
	max_len = atoi(argv[2]);
	k = atoi(argv[3]);
	dirName = argv[4];

	
	/* Create a string set to conveniently hold all our strings from a given directory */
	payloads = lst_stringset_new();
	dirScan(dirName, payloads);
	fprintf(stdout, "string set size : %d \n", payloads->size);
	fprintf(stdout, "fileCounter : %d \n", fileCounter);

	lst_stringset_foreach(payloads, string_cb, "\n");

	/* Create a suffix tree for all strings in the set */
	tree = lst_stree_new(payloads);
	fprintf(stdout, "tree.string_index : %d \n", tree->string_index);

	/* Find k longest common substring(s) */
	u_int *num_distinct_strings = (u_int *) malloc(sizeof(u_int) * ASSUMPTION_NUM_DISTINCT_STRINGS);

	memset(num_distinct_strings, 0, ASSUMPTION_NUM_DISTINCT_STRINGS);

	tokens = lst_alg_k_longest_common_substring(tree, min_len, max_len, k, num_distinct_strings);

	//tokens = lst_alg_first_k_longest_common_substring(tree, min_len, max_len, k, num_distinct_strings);

	//tokens = lst_alg_longest_common_substring(tree, min_len, max_len, num_distinct_strings);


	if(tokens == NULL){
		printf("Info: result is null.\n"); 
		return 0;
	}

	/* Print them out, if any. */
	if (tokens){
		printf("result size : %d \n", tokens->size);
		lst_stringset_foreach(tokens, string_cb, "\t");
		printf("\n");
	}


	/* Free suffix tree: */
	lst_stree_free(tree);


	lst_stringset_foreach(payloads, string_cb, "\n");
	Trie * trie = position_constraints_main(payloads, tokens);

	return 0;
}

#endif



