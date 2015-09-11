#ifndef _MERGE_COMMON_PREFIX_H
#define _MERGE_COMMON_PREFIX_H

#include "trie.h"

typedef struct _merge_common_prefix_t	merge_common_prefix_t;

struct _merge_common_prefix_t {
	int alpha_merge;
};

Trie *generate_trie(void);

void lookup_trie(Trie * trie);

void int_callback(TrieNode *node, void *extension) ;

void str_callback(TrieNode *node, void *extension) ;

void trie_node_merge_callback(TrieNode *node, void *extension);
#endif
