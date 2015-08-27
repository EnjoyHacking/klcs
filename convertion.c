#include "lst_string.h"
#include "convertion.h"

#include <string.h>

convertion_data_single_t * convertion_data_single_new() {

	convertion_data_single_t * single = (convertion_data_single_t *) malloc(sizeof(convertion_data_single_t));

	return single;
}


void convertion_data_single_free(convertion_data_single_t * data){


}


void * string_replace(const char *src, const void *pattern, const void *replacement, size_t item_size) {

	void * dst = (void *) malloc (sizeof(char) * strlen(src) * item_size);

	char * src_p = (char *)src;	

	char * pos = NULL;

	while ((pos = strstr(src_p, pattern)) != NULL)	{

		size_t len = (size_t) (pos - src_p);
		strncat(dst, src_p, len);	
		strncat(dst, replacement, strlen(replacement));
		src_p = pos + strlen(pattern);
	}

	strncat(dst, src_p, strlen(src) - (size_t)(src_p - src));

	return dst;

}


void convert_token_cb(LST_String *token, void *data) {

	convertion_data_single_t * single = (convertion_data_single_t *) data;

	LST_String * flow = single->flow;
	LST_String * flow_converted = single->flow_converted;

	

	return;

}


void convert_flow_cb(LST_String *flow, void *data) {

	convertion_data_set_t * set = (convertion_data_set_t *) data;

	convertion_data_single_t * single = convertion_data_single_new();

	lst_stringset_foreach(set->tokens, convert_token_cb, single);

	LST_String * flow_converted = lst_string_new(single->flow_converted->data, 1, single->flow_converted->num_items);

	lst_stringset_add(set->flows_converted, flow_converted);

	convertion_data_single_free(single);

	return ;

}




convertion_data_set_t * convertion_data_set_new() {

	convertion_data_set_t * data = (convertion_data_set_t *) malloc (sizeof(convertion_data_set_t));

	data->flows_converted = lst_stringset_new();

	data->tokens = lst_stringset_new();

	return data;
}


void convertion_data_set_free (convertion_data_set_t * data) {

	if(!data) {
		return;
	}

	if(data->flows_converted) {
	
		lst_stringset_free(data->flows_converted);
	}

	if(data->tokens) {
		lst_stringset_free(data->tokens);
	}
}



int main(int argc, char **argv) {


	char * payload1 = "HTTP/1.1 index.html";
	char * payload2 = "HTTP/1.0 index.html";

	LST_String *flow1 = lst_string_new(payload1, 1, strlen(payload1));
	LST_String *flow2 = lst_string_new(payload2, 1, strlen(payload2));

	LST_StringSet *flows = lst_stringset_new();
	lst_stringset_add(flows, flow1);
	lst_stringset_add(flows, flow2);


	char * str1 = "HTTP1.1";
	char * str2 = "HTTP1.0";

	LST_String *token1 = lst_string_new(str1, 1, strlen(str1));
	LST_String *token2 = lst_string_new(str2, 1, strlen(str2));

	LST_StringSet *tokens = lst_stringset_new();
	lst_stringset_add(tokens, token1);
	lst_stringset_add(tokens, token2);
	

	/*
	convertion_data_set_t * data = convertion_data_set_new();	

	lst_stringset_foreach(flows, convert_flow_cb, data);	

	convertion_data_set_free(data);
	*/
	
	
	char * merge = string_replace("This is just for test, for testing.", "test", "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");

	printf("%s\n", merge);
	return 0;
}