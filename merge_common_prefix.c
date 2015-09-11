#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "trie.h"
#include "merge_common_prefix.h"

#define NUM_TEST_VALUES 100
#define MAX_SUBSTRING_SIZE 20

int test_array[NUM_TEST_VALUES];
char test_strings[NUM_TEST_VALUES][10];

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

	if(!node->use_count){
		return;
	}

	if(node->use_count == 1) {
		return;
	}


	if (node->use_count < mcp->alpha_merge ) {

		char * new_data = (char *)malloc(sizeof(char) * MAX_SUBSTRING_SIZE);	

		strcpy(new_data, (char *)node->data);

		strcat(new_data, "\(");

		for(int i = 0; i < 256; ++i) {
			if (node->next[i] != NULL) {
				char *ch = (char *) malloc (sizeof(char) * 2);

				sprintf(ch, "%c", i);

				strcat(new_data, ch);

				TrieNode *child = node->next[i];

				node->next[i] = child->next[i];
				
			} 
		}
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



