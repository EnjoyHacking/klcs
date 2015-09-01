#include "position_constraints.h"
#include "pattern_search.h"
#include "lst_string.h"
#include "trie.h"
#include "hash-table.h"
#include "hash-int.h"
#include "compare-int.h"




#define MERGE_TOKEN_MAX_LEN 50

void flow_print(flow_t *flow) {

	if(!flow) {
		return;
	}

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

void flow_free(flow_t *flow){ if(!flow) return; while(flow->element_head.lh_first != NULL) {
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

	if(count == 1) {
		t->position_specific = 1;
	}
	return;

}

offset_t *offset_new(LST_String *token){

	if (!token) {
		return NULL;	
	}

	offset_t * o = (offset_t *) malloc(sizeof(offset_t));

	o->variants = lst_stringset_new();
	lst_stringset_add(o->variants, token);
	o->num_variants = o->variants->size;

	return o;
}

void offset_add(offset_t *o, LST_String *token) {

	if (!o || !token) {
		return;
	}

	if(o->variants != NULL) {
		lst_stringset_add(o->variants, token);
		o->num_variants = o->variants->size;
	}

	return;
}

void offset_free(offset_t * o) {

	if(!o){
		return;
	}

	if(o->variants != NULL) {
		lst_stringset_free(o->variants);
	}

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
Trie*  flow_set_traverse_token(flow_set_t *flow_set) {

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

 void print_callback(TrieNode *node, void * extension) {

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

void search_callback(TrieNode *node, void * extension) {

	if(!node || ! extension){
		return;
	}
	if(!node->data){
		return;
	}
	

	token_t  *token = (token_t *) node->data;	
	
	offset_variants_t * ov = (offset_variants_t *)extension;

	HashTable *offset_variants = ov->offset_variants; // for return

	HashTableIterator iterator;
	hash_table_iterate(token->offset_occurrence, &iterator);

	while(hash_table_iter_has_more(&iterator)) {
		HashTablePair pair = hash_table_iter_next(&iterator);
		int *key = (int *)pair.key;  // a offset
		int *value = (int *)pair.value; // the corresponding ocurrence of the token in the offset
		if(*value >= ov->k_offset) {
			HashTableValue result_value;
			if ((result_value = hash_table_lookup(offset_variants, key)) != HASH_TABLE_NULL) {
				offset_add((offset_t *)result_value, token->token);
			} else {
				offset_t *t = offset_new(token->token);
				hash_table_insert(offset_variants, key, t);	
			}
		}
	}	

	return ;

}


void variant_callback(LST_String *string, void *data)
{
  printf("%s", lst_string_print(string));
  printf("%s", (char *)data);
}


/* This callback function for lst_stringset_foreach is to remove the string */
/*
void remove_callback(LST_String *string, void *data) {
	if(!string || !data) {
		return;
	}

	LST_StringSet * set = (LST_StringSet *)data;
	lst_stringset_remove(string);
	return;
}
*/

/* This callback function for lst_stringset_foreach is to merge some strings and return data */
void merge_callback(LST_String *string, void *data) {
	if(!string || !data) {
		return;
	}
	char * tmp = (char *) data;
	strcat(tmp, lst_string_print(string));	
	strcat(tmp, "|");	
	return ;
}

void merge_by_position_speific_with_offset_variants(Trie *tokens, offset_variants_t *ov) {
	if(!tokens || !ov) {
		return;
	}

	HashTableIterator iterator;
	hash_table_iterate(ov->offset_variants, &iterator);
	while(hash_table_iter_has_more(&iterator)) {
		HashTablePair pair = hash_table_iter_next(&iterator);	
		int * key = (int *)pair.key;
		offset_t * value = (offset_t *)pair.value;
		if( value->num_variants < ov->beta_merge) {
			char * str = (char *)malloc(sizeof(char) * MERGE_TOKEN_MAX_LEN); // need to modify

			if(*key == 0) {
				strcat(str, "^");
			}

			lst_stringset_foreach(value->variants, merge_callback, str);

			str[strlen(str) - 1] = '\0';

			LST_String * merge_token = lst_string_new(str, 1, strlen(str));

			token_t *t = token_new(merge_token, *key);

			trie_insert(tokens, (char *)merge_token->data, t);			
		}
	}

}



void offset_variants_traverse(offset_variants_t *o) {
	if(!o) {
		return;
	}

	HashTableIterator iterator;
	hash_table_iterate(o->offset_variants, &iterator);
	while(hash_table_iter_has_more(&iterator)) {
		HashTablePair pair = hash_table_iter_next(&iterator);	
		int * key = (int *)pair.key;
		offset_t * value = (offset_t *)pair.value;
		printf("%d : ", *key);
		lst_stringset_foreach(value->variants, variant_callback, "\n");
	}
	return;

}




void flow_new_cb(LST_String *string, void *data) {

	if(!string || !data) {
		return;
	}

	data_for_flow_new_t * dffn = (data_for_flow_new_t *)data;

	flow_t * flow = flow_new(string, dffn->tokens);

	printf("-------------\n");

	printf("payload: %s \n", lst_string_print(string));
	flow_print(flow);
	printf("-------------\n");

	flow_set_add(dffn->flow_set, flow);

	return;

}


Trie * position_constraints_main(LST_StringSet * payloads, LST_StringSet * tokens){

	
	if(!payloads || !tokens) {
		return;
	}

	data_for_flow_new_t * dffn = (data_for_flow_new_t *) malloc (sizeof(data_for_flow_new_t));

	dffn->flow_set = flow_set_new();
	dffn->tokens = tokens;

	lst_stringset_foreach(payloads, string_cb, "\n");

	lst_stringset_foreach(payloads, flow_new_cb, dffn);

	flow_set_t * flow_set = dffn->flow_set;

	/* The first round search, return a trie of tokens*/
	Trie *trie = flow_set_traverse_token(flow_set);
	trie_dfs(trie, print_callback, (void *)NULL);

	/* The second round search, return a hash table of offsets*/
	offset_variants_t * res = (offset_variants_t *)malloc(sizeof(offset_variants_t));	
	res->k_offset = 2;
	res->beta_merge = 5;
	res->offset_variants = hash_table_new(int_hash, int_equal);
	trie_dfs(trie, search_callback, res);	

	/* print offset -> variants*/
	offset_variants_traverse(res);

	merge_by_position_speific_with_offset_variants(trie, res) ;

	trie_dfs(trie, print_callback, (void *)NULL);

	/* free allocated memory */
	hash_table_free(res->offset_variants);
	free(res);

	return trie;
}




