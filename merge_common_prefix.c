#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "trie.h"
#include "merge_common_prefix.h"

#define NUM_TEST_VALUES 100
#define MAX_SUBSTRING_SIZE 20

int test_array[NUM_TEST_VALUES];
char test_strings[NUM_TEST_VALUES][10];

LST_StringSet * set = NULL;

Trie *generate_trie(void)
{
	Trie *trie;
	int i;
	unsigned int entries;

	/* Create a trie and fill it with a large number of values */

	trie = trie_new();
	entries = 0;

	for(i = 0; i < NUM_TEST_VALUES; ++i) {
		test_array[i] = i;
		sprintf(test_strings[i], "%i", i);

		trie_insert(trie, test_strings[i], &test_array[i]);
		++entries;

		assert(trie_num_entries(trie) == entries);

	}

	return trie;



}

void lookup_trie(Trie * trie){

	int i;
	int *val;
	char buf[10];
	
	if (!trie) {
		printf("Error: trie is null!\n");
		return;
	}


	for(i = 0; i < NUM_TEST_VALUES; ++i) {
		sprintf(buf, "%i", i);
		val = (int *) trie_lookup(trie, buf);
		assert(*val == i);
		printf("%i \t", *val);
	}

	printf("\n");
	return ;

}

void trie_node_merge_callback(TrieNode *node, void *extension){

	if(!node){
		return;
	}
	if(!node->data){
		return;
	}

	merge_common_prefix_t * mcp = (merge_common_prefix_t *)extension;	

	if(mcp->flag == 1) {
		char *new_merge_substring = (char *) malloc (sizeof(char) * MAX_SUBSTRING_SIZE);
		memset(new_merge_substring, '\0', MAX_SUBSTRING_SIZE);
		strcpy(new_merge_substring, mcp->merge_substring);
		char * tmp_p = node->data;
		tmp_p += mcp->common_prefix_len + 1;
		strcat(new_merge_substring,  tmp_p);
		node->data = new_merge_substring;
		return;
	}

	int child_nums = 0;

	for(int i = 0; i < 256; i++) {

		if(node->next[i] != NULL){
			child_nums++;
		}
	}

	//printf("child_nums : %d -- %s \n", child_nums, (char *)node->data);
	//printf("node->use_count : %d -- %s \n", node->use_count, (char *)node->data);

	if (child_nums == 0 || child_nums == 1) {
		return;	
	}


	int tmp_counter = 1;
	if (child_nums < mcp->alpha_merge ) {

		char * new_data = (char *)malloc(sizeof(char) * MAX_SUBSTRING_SIZE);	

		strcpy(new_data, (char *)node->data);

		strcat(new_data, "(");

		for(int i = 0; i < 256; ++i) {
			if (node->next[i] != NULL) {
				char *ch = (char *) malloc (sizeof(char) * 2);

				sprintf(ch, "%c", i);

				strcat(new_data, ch);
				if(tmp_counter < child_nums) {
					strcat(new_data, "|");
				} else {
					strcat(new_data, ")");
				}
				tmp_counter++;
			} 
		}
		mcp->common_prefix_len = strlen(node->data);
		mcp->merge_substring = new_data;
		mcp->flag = 1;

		node->data = new_data;
	}
	return;
}


void int_callback(TrieNode *node, void *extension) {

	if(!node){
		return;
	}
	if(!node->data){
		return;
	}
	
	int * value = (int *) node->data;	
	
	printf("%i \t", *value);

	return;

}

void str_callback(TrieNode *node, void *extension) {

	if(!node){
		return;
	}
	if(!node->data){
		return;
	}
	
	char * value = (char *) node->data;	
	
	printf("%s\n", value);


	return ;
}

void add_callback(TrieNode *node, void *extension) {

	if(!node){
		return;
	}
	if(!node->data){
		return;
	}
	char * value = (char *) node->data;	

	char * value_cp = (char *) malloc(sizeof(char) * (strlen(value) + 1));

	strcpy(value_cp, value);
	
	lst_stringset_add(set, lst_string_new(value_cp, 1, strlen(value)));

	return ;
}

tokens_for_one_trie_t * tokens_for_one_trie_new(LST_String *query_token){

	if(!query_token) {
		return NULL;
	}

	tokens_for_one_trie_t *tfot = (tokens_for_one_trie_t *) malloc (sizeof(tokens_for_one_trie_t ));
	tfot->query_token = query_token;
	tfot->set = lst_stringset_new();

	return tfot;
}
void tokens_for_one_trie_free(tokens_for_one_trie_t *tfot ){

	if(!tfot) {
		return;
	}

	if(tfot->set) {
		//lst_stringset_free(tfot->set);
		//tfot->set = NULL;
	}

	if(tfot->query_token) {
		//lst_string_free(tfot->query_token);
		//tfot->query_token = NULL;
	}
	return;
}


void obtain_tokens_with_common_prefix_cb1(LST_String *query, void *data) {

	if(!query || !data) {
		return;
	}

	tokens_for_one_trie_t * tfot  = tokens_for_one_trie_new(query);
	LST_StringSet * tokens = (LST_StringSet *) data;
	lst_stringset_foreach(tokens, obtain_tokens_with_common_prefix_cb2, tfot);

	//printf("%s : %d\n", (char *)query->data, tfot->set->size);

	if(tfot->set) {
		Trie * trie = trie_new();
		lst_stringset_foreach(tfot->set, construct_trie_cb, trie);
		merge_substring(trie);
	}


	tokens_for_one_trie_free(tfot);

	return;

}

void obtain_tokens_with_common_prefix_cb2(LST_String *string, void *data) {

	tokens_for_one_trie_t * tfot = (tokens_for_one_trie_t *)data;

	LST_String * query_token = tfot->query_token;
	int * indices = kmp_search((char *)query_token->data, (char *)string->data);

	if(!indices) {
		return;
	}

	if (indices[0] == 0) {
		lst_stringset_add(tfot->set, lst_string_new((char *) string->data, 1, strlen((char *) string->data)));
	}

	return;
}

void  construct_trie_cb(LST_String *string, void *data){

	Trie *trie = (Trie *) data;

	trie_insert(trie, (char *)string->data, (char *)string->data);

	return;


}

void merge_substring(Trie *trie){

	if(!trie)
		return;

	merge_common_prefix_t  *mcp = (merge_common_prefix_t *) malloc (sizeof(merge_common_prefix_t));
	mcp->alpha_merge = 5;
	mcp->flag = 0;
	mcp->merge_substring = NULL;


	//trie_dfs(trie, str_callback, NULL);
//	printf("1.-------------------------\n");
	trie_dfs(trie, trie_node_merge_callback, mcp);
	//printf("2.-------------------------\n");
	trie_dfs(trie, str_callback, NULL);
	//printf("3.-------------------------\n");
	trie_dfs(trie, add_callback, NULL);

	return;
}

void merge_common_prefix_main(LST_StringSet * substrings) {
	set = lst_stringset_new();
	lst_stringset_foreach(substrings, obtain_tokens_with_common_prefix_cb1, substrings);
}


