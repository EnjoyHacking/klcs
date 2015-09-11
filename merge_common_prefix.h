#ifndef _MERGE_COMMON_PREFIX_H
#define _MERGE_COMMON_PREFIX_H

#include "trie.h"
#include "lst_string.h"

typedef struct _merge_common_prefix_t	merge_common_prefix_t;
typedef struct _tokens_for_one_trie_t   tokens_for_one_trie_t;

struct _merge_common_prefix_t {
	int alpha_merge;
	int flag;  // If flag = 1, denote this trie has done one merge.
	char * merge_substring;
	int common_prefix_len;
};

struct _tokens_for_one_trie_t {

	LST_StringSet * query_token;
	LST_StringSet * set;

};

Trie *generate_trie(void);

void lookup_trie(Trie * trie);

void int_callback(TrieNode *node, void *extension) ;

void str_callback(TrieNode *node, void *extension) ;

void add_callback(TrieNode *node, void *extension) ;

void trie_node_merge_callback(TrieNode *node, void *extension);

void obtain_tokens_with_common_prefix_cb2(LST_String *string, void *data) ;

void obtain_tokens_with_common_prefix_cb1(LST_String *query, void *data) ;

void tokens_for_one_trie_free(tokens_for_one_trie_t *tfot );

tokens_for_one_trie_t * tokens_for_one_trie_new(LST_String *query_token);

void  construct_trie_cb(LST_String *string, void *data);

void merge_common_prefix_main(LST_StringSet * substrings) ;

#endif
