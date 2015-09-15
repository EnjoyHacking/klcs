#include "product_distribution_model.h"
#include "lst_string.h"
#include "hash-int.h"
#include "compare-int.h"
#include "hash-string.h"
#include "compare-string.h"
#include "hash-table.h"

#include "position_constraints.h"




/** This callback function for lst_stringset_foreach() to calculate product distribution */

void calculate_product_distribution_callback(LST_String *nbytes, void *data) {

	if (!nbytes || !data) {
		return;
	}

	product_distribution_t * pd = (product_distribution_t *) data;

	int index = 0;
	while (index < pd->first_bytes || (index >= pd->num_bytes - pd->last_bytes && index < pd->num_bytes)) {
		
		HashTableValue bd;	
		int *offset = (int *)malloc(sizeof(int));
		*offset = index;
		if((bd = hash_table_lookup(pd->offset_distribution, offset)) != HASH_TABLE_NULL){
			byte_distribution_add((byte_distribution_t *)bd, nbytes, *offset);	
		} else {
			byte_distribution_t * bd_new = byte_distribution_new(nbytes, *offset);
			hash_table_insert(pd->offset_distribution, offset, bd_new);
		}

		if(index == pd->first_bytes - 1) {
			index = pd->num_bytes - pd->last_bytes;
		} else {
			index++;
		}
	}

	return;

}
/**
 * @param set
 * @param first_bytes
 * @param last_bytes
 * @param num_bytes
 */
product_distribution_t * product_distribution_new(LST_StringSet * set, int first_bytes, int last_bytes, int num_bytes){

	if(!set) {
		return NULL;	
	}

	product_distribution_t * pd = (product_distribution_t *) malloc (sizeof(product_distribution_t));
	pd->first_bytes = first_bytes;
	pd->last_bytes = last_bytes;
	pd->num_bytes = num_bytes;
	pd->offset_distribution = hash_table_new(int_hash, int_equal);

	lst_stringset_foreach(set, calculate_product_distribution_callback, pd);

	return pd;	
}

void product_distribution_free(product_distribution_t * pd) {
	if(!pd) {
		return;
	}
	if(pd->offset_distribution != NULL) {
		hash_table_free(pd->offset_distribution);
	}
	free(pd);
	return;
}

void byte_distribution_add(byte_distribution_t *bd, LST_String *nbytes, int offset){
	
	if(!bd || !nbytes) {
		return;
	}

	if(offset < 0 || offset >= lst_string_get_length(nbytes)) {
		return;
	}

	char *value = (char *) lst_string_get_item(nbytes, offset);

	HashTableValue frequency;
	char * key = (char *) malloc(sizeof(char) * 2);
	memset(key, '\0', 2);
	*key = value[0];

	if((frequency = hash_table_lookup(bd->value_frequency, key)) != HASH_TABLE_NULL){
	       (*(int *)frequency)++;				
	} else {
		int *freq = (int *) malloc (sizeof(int)); 
		*freq = 1;
		hash_table_insert(bd->value_frequency, key, freq);
	}
	return;
}


byte_distribution_t * byte_distribution_new(LST_String *nbytes, int offset) {

	char * item = (char *)lst_string_get_item(nbytes, offset);

	byte_distribution_t * bd = (byte_distribution_t *) malloc (sizeof(byte_distribution_t));

	bd->value_frequency = hash_table_new(string_hash, string_equal);	
	
	char * value = (char *) malloc (sizeof(char) * 2);
	memset(value, '\0', 2);
	*value = item[0];
	int * frequency = (int *) malloc (sizeof(int));
	*frequency = 1;

	hash_table_insert(bd->value_frequency, value, frequency);		
	
	return bd;
}

int product_distribution_main(Trie * trie, LST_StringSet * set, int first_bytes, int last_bytes, int num_bytes, int gamma_merge){


	product_distribution_t * pd = product_distribution_new(set, first_bytes, last_bytes, num_bytes);

	/* print pd */
	HashTableIterator iterator1;
	hash_table_iterate(pd->offset_distribution, &iterator1);

	while (hash_table_iter_has_more(&iterator1)){
		HashTablePair pair1 = hash_table_iter_next(&iterator1);

		int *key1 = (int *) pair1.key;
		byte_distribution_t *value1 = (byte_distribution_t *) pair1.value;

		HashTableIterator iterator2;
		hash_table_iterate(value1->value_frequency, &iterator2);

		if (hash_table_num_entries(value1->value_frequency) > gamma_merge) {
			continue;
		}
		printf("offset %d, size %d: \n", *key1, hash_table_num_entries(value1->value_frequency));	
		while(hash_table_iter_has_more(&iterator2)){
			HashTablePair pair2 = hash_table_iter_next(&iterator2);
			char *key2 = (char *) pair2.key;
			int *value2 = (int *) pair2.value;
			//printf("<%c, %d>\t", key2[0], *value2);
			char * single_token = (char *) malloc (sizeof(char) * 3);
			memset(single_token, '\0', 3);

			
			char hex[4];
			memset(hex, '\0', 4);
			if (!(isprint(key2[0]) && key2[0] != '%')) {
				sprintf(hex, "%%%.2x", key2[0]);
			} else {
				strncpy(hex, key2, 3);
			}

			if (0 == *key1) {
				printf("^%s\t", hex);
				sprintf(single_token, "^%s", key2);
			} else if (num_bytes - 1 == *key1) {
				printf("%s$\t", hex);
				sprintf(single_token, "%s$", key2);
			} else {
				printf("%s\t", hex);
				sprintf(single_token, "%s", key2);
			}
			//printf("%s -- strlen: %d \n", single_token, strlen(single_token));


			token_t * t = token_new(lst_string_new(single_token, 1, strlen(single_token)), *key1);
			if(t){
				trie_insert(trie, (char *)t->token->data, t); 
			}
		}
		printf("\n");
	}

	product_distribution_free(pd);

}


