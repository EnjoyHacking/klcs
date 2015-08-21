#include "position_constraints.h"
#include "pattern_search.h"
#include "lst_string.h"
#include "trie.h"
#include "hash-table.h"
#include "hash-int.h"
#include "compare-int.h"


void flow_print(flow_t *flow) {

	element_t *element;
	for(element = flow->element_head.lh_first; element; element = element->entry.le_next) {
		printf("<%s, %d>\n", lst_string_print(element->token), element->offset);	
	}

	return ;

}

flow_t *flow_new(LST_String *string, LST_StringSet *tokens){
	flow_t *flow = (flow_t *)malloc(sizeof(flow_t));
	LIST_INIT(&(flow->element_head));
	LST_String *token;

	if (!tokens || !string)
		return NULL;
	int *indices;
	for (token = tokens->members.lh_first; token; token = token->set.le_next) {
		indices = kmp_search((char *)token->data, (char *)string->data);

		for(int i = 0; i < lst_string_get_length(string)&& indices[i] != -1; i++) {
			element_t *element = (element_t *) malloc(sizeof(element_t));
			element->token = token;
			element->offset = indices[i];
			LIST_INSERT_HEAD(&(flow->element_head), element, entry);
		}
	}
	return flow;
	
}

void flow_free(flow_t *flow){

	if(!flow)
		return;
	while(flow->element_head.lh_first != NULL) {
		element_t *element = flow->element_head.lh_first;
		LIST_REMOVE(flow->element_head.lh_first, entry);	
		free(element);
	}
	return;
}


token_t * token_new(LST_String *token, int offset){

	if(!token) {
		return NULL;
	}

	token_t *t = (token_t *) malloc(sizeof(token_t));
	t->token = token;
	
	t->position_specific = 0;
	t->begin_of_flow = 0;

	t->offset_occurrence = hash_table_new(int_hash, int_equal);

	int * key = (int *) malloc (sizeof(int));
	*key = offset;
	int * value = (int *) malloc (sizeof(int));
	*value = 1;
	hash_table_insert(t->offset_occurrence, key, value);

	return t;

}
/**
 * For the existing token in the token set, we will add the new offset of the token or the occurrence of offset.
 */
void token_add(token_t *t, int offset) {
	
	if(!t) {
		return;	
	}

	if(!t->offset_occurrence) {
		return;
	}

	int *occurrence;
	int *find_key = (int *) malloc(sizeof(int));
	*find_key = offset;

	int * key = (int *) malloc(sizeof(int));
	int * value = (int *) malloc(sizeof(int));
	*key = offset;
	if ((occurrence = (int *) hash_table_lookup(t->offset_occurrence, key)) != HASH_TABLE_NULL) {
		*value = *((int *)occurrence) + 1;
	} else {
		*value = 1;
	}
	hash_table_insert(t->offset_occurrence, key, value);

	return ;

}

void token_print(token_t *t){

	if(!t){
		return ;
	}

	HashTable *hash_table;
	HashTableIterator iterator;
	int count;

	hash_table = t->offset_occurrence;
	count = 0;

	/* Iterate over all values in the table */
	hash_table_iterate(hash_table, &iterator);
	
	printf("<%s> : ", lst_string_print(t->token));	

	while (hash_table_iter_has_more(&iterator)) {
		HashTablePair pair = hash_table_iter_next(&iterator);
		printf("%d(%d) \t", *((int *)pair.key), *((int *)pair.value));
		++count;
	}
	printf("\n");
	return;

}

void token_free(token_t *t){

	if(!t) {
		return;
	}

	if(t->token){
		lst_string_free(t->token);
	}

	if(t->offset_occurrence){
		hash_table_free(t->offset_occurrence);
	}

	free(t);
	t = NULL;
}

flow_set_t* flow_set_new(void){

	flow_set_t *flow_set;

	flow_set = calloc(1, sizeof(flow_set_t));

	if (!flow_set)
		return NULL;

	LIST_INIT(&flow_set->flow_head);

	return flow_set;
}

void flow_set_add(flow_set_t *flow_set, flow_t *flow){
	if (!flow_set || !flow)
		return;

	LIST_INSERT_HEAD(&flow_set->flow_head, flow, entry);
	return;

}

void flow_set_free(flow_set_t *flow_set){
	flow_t *flow;

	if (!flow_set)
		return;

	while (flow_set->flow_head.lh_first)
	{
		flow = flow_set->flow_head.lh_first;
		LIST_REMOVE(flow_set->flow_head.lh_first, entry);
		flow_free(flow);
	}

	free(flow_set);
	flow_set = NULL;

}

/**
 * Caculate the different offset and the corresponding occurrence for each given token in all flows
 * @param flow_set	the all flows, each flow is a instance of @flow_t
 * @return a trie of storing the variables of token_t data type
 */
Trie*  flow_set_traverse(flow_set_t *flow_set) {

	if(!flow_set) {
		return NULL;
	}

	flow_t *flow;
	Trie *trie = trie_new();

	for (flow = flow_set->flow_head.lh_first; flow; flow = flow->entry.le_next) {
		element_t *element;
		for (element = flow->element_head.lh_first; element; element = element->entry.le_next) {

			token_t *t;

			if ((t = trie_lookup(trie, (char *)element->token->data)) != TRIE_NULL) {
				token_add(t, element->offset);
			} else {
				t = token_new(element->token, element->offset);	
			}

			trie_insert(trie, (char *)element->token->data, t);		
		}
	}
	
	return trie;

}

static void print_callback(TrieNode *node) {

	if(!node){
		return;
	}
	if(!node->data){
		return;
	}
	
	token_t  *token = (token_t *) node->data;	
	
	token_print(token);

	return ;
}




int main(int argc, char * argv[]) {

	char *str1 = "HTTP1.1 index.html 200 a HTTP";
	char *str2 = "HTTP1.0 index.html HTTP b 200";

	LST_String * payload1 = lst_string_new(str1, 1, strlen(str1));
	LST_String * payload2 = lst_string_new(str2, 1, strlen(str2));

	LST_StringSet *tokens = lst_stringset_new();

	char *token1 = "HTTP";
	char *token2 = "200";
	lst_stringset_add(tokens, lst_string_new(token1, 1, strlen(token1)));
	lst_stringset_add(tokens, lst_string_new(token2, 1, strlen(token2)));

	flow_t *flow1 = flow_new(payload1, tokens);
	flow_print(flow1);
	printf("----------------------------\n");

	flow_t *flow2 = flow_new(payload2, tokens);
	flow_print(flow2);
	printf("----------------------------\n");
	
	flow_set_t * flow_set = flow_set_new();
	flow_set_add(flow_set, flow1);
	flow_set_add(flow_set, flow2);

	Trie *trie = flow_set_traverse(flow_set);
	trie_dfs(trie, print_callback);

	trie_free(trie);
	flow_free(flow1);
	flow_free(flow2);
	lst_stringset_free(tokens);


	return 0;
}
