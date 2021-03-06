/*

Copyright (C) 2003-2006 Christian Kreibich <christian@whoop.org>.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
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


#if 2

int main(int argc, char **argv) {

	LST_STree     *tree;
	LST_StringSet *set, *result;
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
	set = lst_stringset_new();
	dirScan(dirName, set);
	fprintf(stdout, "string set size : %d \n", set->size);
	fprintf(stdout, "fileCounter : %d \n", fileCounter);

	/* Create a suffix tree for all strings in the set */
	tree = lst_stree_new(set);
	fprintf(stdout, "tree.string_index : %d \n", tree->string_index);

	/* Find k longest common substring(s) */
	u_int *num_distinct_strings = (u_int *) malloc(sizeof(u_int) * ASSUMPTION_NUM_DISTINCT_STRINGS);

	memset(num_distinct_strings, 0, ASSUMPTION_NUM_DISTINCT_STRINGS);

	result = lst_alg_k_longest_common_substring(tree, min_len, max_len, k, num_distinct_strings);

	//result = lst_alg_first_k_longest_common_substring(tree, min_len, max_len, k, num_distinct_strings);

	//result = lst_alg_longest_common_substring(tree, min_len, max_len, num_distinct_strings);



	if(result == NULL){
		printf("result is null.\n"); 
		return 0;
	}

	/* Print them out, if any. */
	if (result)
	{
		printf("result size : %d \n", result->size);
		lst_stringset_foreach(result, string_cb, "\t");
		printf("\n");
	}

	/*
	for(int i = 0; i < result->size; i++)
		printf("%u\t", num_distinct_strings[i]);	
	printf("\n");
	*/

	/* Free suffix tree: */
	lst_stree_free(tree);

	return 0;
}

#endif




#if 0 

int
main(int argc, char **argv)
{
  LST_STree     *tree;
  LST_StringSet *set, *result;
  u_int min_len, max_len;
  int k;
  char* dirName;
  
  if (argc <  5)
    test_usage(argv[0]);

  min_len = atoi(argv[1]);
  max_len = atoi(argv[2]);
  k = atoi(argv[3]);
  dirName = argv[4];

  /* Create a string set to conveniently hold all our strings: */
  set = lst_stringset_new();

  /* Add all strings passed on the command line to the set.
   * Note that we pass the string length excluding the null
   * terminator, libstree handles that generically.
   */
  for (int i = 4; i < argc; i++)
    lst_stringset_add(set, lst_string_new(argv[i], 1, strlen(argv[i])));
  
  //dirScan(dirName, set);
  //fprintf(stdout, "string set size : %d \n", set->size);
  //fprintf(stdout, "fileCounter : %d \n", fileCounter);
  /* Create a suffix tree for all strings in the set: */
  tree = lst_stree_new(set);
  /* Find longest common substring(s) */
  printf("1. tree finished! \n");
  //result = lst_alg_k_longest_common_substring(tree, min_len, max_len, k);
  printf("2. result finished! \n");
  //result = lst_alg_longest_common_substring(tree, min_len, max_len);
  //printf("3. lcs finished! \n");
  
  u_int *num_distinct_strings = (u_int *) malloc(sizeof(u_int) * ASSUMPTION_NUM_DISTINCT_STRINGS);
  memset(num_distinct_strings, 0, ASSUMPTION_NUM_DISTINCT_STRINGS);
  //result = lst_alg_first_k_longest_common_substring(tree, min_len, max_len, k, num_distinct_strings);

  result = lst_alg_k_longest_common_substring(tree, min_len, max_len, k, num_distinct_strings);

  //result = lst_alg_longest_common_substring(tree, min_len, max_len, num_distinct_strings);

  printf("3. klcs finished! \n");

  if(result == NULL){
 	printf("result is null.\n"); 
  }
  
  fprintf(stdout, "tree.string_index : %d \n", tree->string_index);
  /* Print them out, if any. */
  if (result)
  {
	  printf("result size : %d \n", result->size);
	  lst_stringset_foreach(result, string_cb, "\t");
	  printf("\n");
  }

  //for(int i = result->size - 1; i >= 0; i--)
//	  printf("%u\t", num_distinct_strings[i]);	
 // printf("\n");

   /* Testing suffix tree for substring check */

   LST_String *substr1 = lst_string_new("hello", 1, 5);
   LST_String *substr2 = lst_string_new("ello", 1, 4);
   LST_String *substr3 = lst_string_new("ell", 1, 3);
   LST_String *substr4 = lst_string_new("china", 1, 5);
   LST_String *substr5 = lst_string_new("welcom", 1, 6);

   checkForSubString(tree, substr1);
   checkForSubString(tree, substr2);
   checkForSubString(tree, substr3);
   checkForSubString(tree, substr4);
   checkForSubString(tree, substr5);

      

  /* Free suffix tree: */
  lst_stree_free(tree);

  return 0;
}
#endif
