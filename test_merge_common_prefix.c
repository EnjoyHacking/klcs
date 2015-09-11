#include <stdio.h>
#include <stdlib.h>

#include "merge_common_prefix.h"
#include "trie.h"
#include "lst_string.h"

extern LST_StringSet * set;



int main(void){

	char * str1 = "HTTP/1.";
	char * str2 = "HTTP/1.0";
	char * str3 = "HTTP/1.1";
	char * str4 = "HTTP/1.1 20";
	char * str5 = "HTTP/1.1 30";
	char * str6 = "HTTP/1.1 40";
	char * str7 = "HTTP/1.0 20";
	char * str8 = "HTTP/1.0 30";
	
	LST_String * substring1 = lst_string_new(str1, 1, strlen(str1));
	LST_String * substring2 = lst_string_new(str2, 1, strlen(str2));
	LST_String * substring3 = lst_string_new(str3, 1, strlen(str3));
	LST_String * substring4 = lst_string_new(str4, 1, strlen(str4));
	LST_String * substring5 = lst_string_new(str5, 1, strlen(str5));
	LST_String * substring6 = lst_string_new(str6, 1, strlen(str6));
	LST_String * substring7 = lst_string_new(str7, 1, strlen(str7));
	LST_String * substring8 = lst_string_new(str8, 1, strlen(str8));

	LST_StringSet *substrings = lst_stringset_new();

	lst_stringset_add(substrings, substring1);
	lst_stringset_add(substrings, substring2);
	lst_stringset_add(substrings, substring3);
	lst_stringset_add(substrings, substring4);
	lst_stringset_add(substrings, substring5);
	lst_stringset_add(substrings, substring6);
	lst_stringset_add(substrings, substring7);
	lst_stringset_add(substrings, substring8);

	merge_common_prefix_main(substrings);

	if(set) {
		printf("set->size : %d\n", set->size);
		lst_stringset_foreach(set, string_cb, "\n");
	}


	return 0;
}





#if 0
int main(int argc, char** argv) {

	/*
	Trie *trie = generate_trie();

	lookup_trie(trie);
	*/

	Trie * trie = trie_new();
	/*
	trie_insert(trie, "aa", "aa");
	trie_insert(trie, "ab", "ab");
	trie_insert(trie, "ac", "ac");

	trie_insert(trie, "ba", "ba");
	trie_insert(trie, "bb", "bb");
	trie_insert(trie, "bc", "bc");
	*/

	trie_insert(trie, "HTTP/1.", "HTTP/1.");
	trie_insert(trie, "HTTP/1.1 ", "HTTP/1.1 ");
	trie_insert(trie, "HTTP/1.0 ", "HTTP/1.0 ");
	trie_insert(trie, "HTTP/1.1 20", "HTTP/1.1 20");
	trie_insert(trie, "HTTP/1.0 20", "HTTP/1.0 20");
	trie_insert(trie, "HTTP/1.0 30", "HTTP/1.0 30");
	trie_insert(trie, "HTTP/1.1 30", "HTTP/1.1 30");
	trie_insert(trie, "HTTP/1.1 40", "HTTP/1.1 40");
	

	merge_common_prefix_t  *mcp = (merge_common_prefix_t *) malloc (sizeof(merge_common_prefix_t));
	mcp->alpha_merge = 5;
	mcp->flag = 0;
	mcp->merge_substring = NULL;

	trie_dfs(trie, str_callback, NULL);
	printf("-------------------------\n");
	trie_dfs(trie, trie_node_merge_callback, mcp);
	trie_dfs(trie, str_callback, NULL);

	return 0;
}
#endif
