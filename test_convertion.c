#include "convertion.h"
#include "lst_string.h"
#include "trie.h"
#include "position_constraints.h"

int main(int argc, char **argv) {


	char * payload1 = "HTTP/1.1 index.html GET";
	char * payload2 = "HTTP/1.0 index.html";

	LST_String *flow1 = lst_string_new(payload1, 1, strlen(payload1));
	LST_String *flow2 = lst_string_new(payload2, 1, strlen(payload2));

	LST_StringSet *flows = lst_stringset_new();
	lst_stringset_add(flows, flow1);
	lst_stringset_add(flows, flow2);


	char * str1 = "HTTP/1.1";
	char * str2 = "HTTP/1.0";
	char * str3 = "GET";

	LST_String *token1 = lst_string_new(str1, 1, strlen(str1));
	LST_String *token2 = lst_string_new(str2, 1, strlen(str2));
	LST_String *token3 = lst_string_new(str3, 1, strlen(str3));

	LST_StringSet *tokens = lst_stringset_new();
	lst_stringset_add(tokens, token1);
	lst_stringset_add(tokens, token2);
	lst_stringset_add(tokens, token3);

	int beta_merge = 5;

	int k_offset = 2; 

	Trie * trie = position_constraints_main(flows, tokens, k_offset, beta_merge);

	if(!trie || trie_num_entries(trie) == 0) {
		return;
	}


	convertion_data_set_t * data = convertion_data_set_new(trie);	

	lst_stringset_foreach(flows, convert_flow_cb, data);	

	lst_stringset_foreach(data->flows_converted, flow_converted_print_cb, NULL);

	convertion_data_set_free(data);
	

	/* just for test sting_replace function*/

	/*

	char * src = "This is just for test, for testing.";
	char * pattern = "test";

	int replacement = 300;
	
	int * flow_digital = string_replace(src, pattern, replacement);

	LST_String * flow_converted = lst_string_new(flow_digital, sizeof(int), strlen(src));
	
	int * merge = (int *) flow_converted->data;

	for(int i = 0; i < strlen(src); i++) {
		if( merge[i] > 255 || merge[i] < 0) {
			printf(" %d", merge[i]);
		} else {
			printf("%c", (char) merge[i]);	
		}
	}

	LST_StringSet * flows_converted = lst_stringset_new();

	lst_stringset_add(flows_converted, flow_converted);

	lst_stringset_foreach(flows_converted, flow_converted_print_cb, NULL);
	
	printf("\n");
	 */
	return 0;
}
