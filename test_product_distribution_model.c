#include "product_distribution_model.h"
#include "lst_string.h"
#include "hash-table.h"

int main(int arg, char **argv) {

	char * str1 = "HTTP/1.1 index.html";
	char * str2 = "HTTP/1.0 aaaaa.html";
	char * str3 = "HTTP/1.0 bbbbb.html";
	char * str4 = "HTTP/1.1 ccccc.html";
	char * str5 = "HTTP/0.9 ddddd.html";
	char * str6 = "HTTP/1.1 fffff.html";
	char * str7 = "HTTP/0.9 eeeee.html";

	LST_String * nbytes1 = lst_string_new(str1, 1, strlen(str1));
	LST_String * nbytes2 = lst_string_new(str2, 1, strlen(str2));
	LST_String * nbytes3 = lst_string_new(str3, 1, strlen(str3));
	LST_String * nbytes4 = lst_string_new(str4, 1, strlen(str4));
	LST_String * nbytes5 = lst_string_new(str5, 1, strlen(str5));
	LST_String * nbytes6 = lst_string_new(str6, 1, strlen(str6));
	LST_String * nbytes7 = lst_string_new(str7, 1, strlen(str7));

	LST_StringSet * set = lst_stringset_new();
	lst_stringset_add(set, nbytes1);
	lst_stringset_add(set, nbytes2);
	lst_stringset_add(set, nbytes3);
	lst_stringset_add(set, nbytes4);
	lst_stringset_add(set, nbytes5);
	lst_stringset_add(set, nbytes6);
	lst_stringset_add(set, nbytes7);

	int first_bytes = 8;
	int last_bytes = 10;
	int num_bytes = 19;
	int gamma_merge = 2;

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

		int variants = hash_table_num_entries(value1->value_frequency);

		printf("variants : %d \n", variants);

		if (hash_table_num_entries(value1->value_frequency) > gamma_merge) {
			continue;
		}

		printf("offset %d : ", *key1);	
		while(hash_table_iter_has_more(&iterator2)){
			HashTablePair pair2 = hash_table_iter_next(&iterator2);
			char *key2 = (char *) pair2.key;
			int *value2 = (int *) pair2.value;
			printf("<%c, %d>\t", key2[0], *value2);
			/*
			if (0 == *key1) {
				printf("^%s\t", key2);
			} else if (num_bytes - 1 == *key1) {
				printf("%s$\t", key2);
			} else {
				printf("%s\t", key2);
			}
			*/
		}
		printf("\n");
	}

	product_distribution_free(pd);

	return 0;
}

