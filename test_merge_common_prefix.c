#include <stdio.h>
#include <stdlib.h>

#include "merge_common_prefix.h"
#include "trie.h"


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

	trie_dfs(trie, str_callback, NULL);
	printf("-------------------------\n");
	trie_dfs(trie, trie_node_merge_callback, mcp);
	trie_dfs(trie, str_callback, NULL);

	return 0;
}
