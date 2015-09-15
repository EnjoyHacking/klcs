#include "lst_string.h"
#include "convertion.h"
#include <string.h>
#include "trie.h"
#include "position_constraints.h"


FILE * fp = NULL;

convertion_data_single_t * convertion_data_single_new(LST_String *flow) {

	convertion_data_single_t * single = (convertion_data_single_t *) malloc(sizeof(convertion_data_single_t));

	single->flow = flow;

	int * flow_digital = (int *) malloc (sizeof(int) * flow->num_items - 1) ;

	single->flow_converted = lst_string_new(flow_digital, sizeof(int), flow->num_items - 1);

	single->replacement = (int *) malloc (sizeof(int));

	*(single->replacement) = 256;

	single->first_flag = 0;


	return single;
}


void convertion_data_single_free(convertion_data_single_t * data){

	if(!data) {
		return;
	}

	if(data->flow != NULL) {
		lst_string_free(data->flow);
	}

	if(data->flow_converted != NULL) {
		lst_string_free(data->flow_converted);
	}

	if(data->replacement != NULL) { // need to fix
		//free(data->replacement);
	}

	return ;
	

}

int * string_replace(const char *src, const char *pattern, int replacement, int replacement_len, int *dst_old, int first_flag) {

	/*
	printf("src : %s \n", src);
	printf("pattern : %s \n", pattern);
	printf("replacement : %d \n", replacement);
	printf("first_flag : %d \n", first_flag);
	*/

	int * dst = (int *) malloc (sizeof(int) * strlen(src));

	char * src_p = (char *)src;	

	char * pos = NULL;

	int idx = 0;

	int replacement_counter = 0;

	while ((pos = strstr(src_p, pattern)) != NULL)	{

		size_t len = (size_t) (pos - src_p);

		for(int i = 0; i < len; i++) {
			dst[idx++] = (int) (*(src_p + i));

			if(first_flag == 0 && dst_old) {
				dst_old[idx - 1] = (int) (*(src_p + i));
			}
		}
		
		for(int j = 0; j < strlen(pattern); j++) {
			dst[idx++] = replacement; 

			if(dst_old) {
				if (replacement_counter < replacement_len)
					dst_old[idx - 1] = replacement;
				else 
					dst_old[idx - 1] = -1;   // The value -1 mean that the corresponding item need to delete later.

				replacement_counter++;
			}

		}

		src_p = pos + strlen(pattern);
	}

	for(int k = 0; k < strlen(src) - (size_t)(src_p - src); k++) {
		dst[idx++] = (int)*(src_p + k); 

		if(first_flag == 0 && dst_old) {
			dst_old[idx - 1] = (int)*(src_p + k); 
		}
	}

	return dst;

}



void convert_token_cb(TrieNode *node, void *data) {

	if(!node){
		return;
	}
	if(!node->data){
		return;
	}

	convertion_data_single_t * single = (convertion_data_single_t *) data;

	LST_String * flow = single->flow;

	token_t * t = (token_t *)node->data;

	single->replacement_len = t->shortest_len; 
	single->replacement = t->replacement;

	
	int * flow_digital = string_replace((char *)flow->data, (char *)t->token->data, single->replacement, single->replacement_len, \
						(int *)single->flow_converted->data, single->first_flag);


	int * flow_old = (int *) single->flow_converted->data;

	/*
	for(int i = 0; i < flow->num_items; i++) {
		if( flow_old[i] > 255 || flow_old[i] < 0) {
			printf(" %d", flow_old[i]);
		} else {
			printf("%c", (char) flow_old[i]);	
		}
	}
	printf("\n\n");
	*/
	/*
	for(int i = 0; i < flow->num_items; i++) {
		if( flow_digital[i] > 255 || flow_digital[i] < 0) {
			printf(" %d", flow_digital[i]);
		} else {
			printf("%c", (char) flow_digital[i]);	
		}
	}
	printf("\n");
	*/
	
	//(*(single->replacement))++;
	single->first_flag++;
	//single->flow_converted = lst_string_new(flow_old, sizeof(int), flow->num_items);

	return;

}


void convert_flow_cb(LST_String *flow, void *data) {

	convertion_data_set_t * set = (convertion_data_set_t *) data;

	convertion_data_single_t * single = convertion_data_single_new(flow);

	trie_dfs(set->tokens, convert_token_cb, single);	


	/*	
	int * flow_digital = (int *) malloc (sizeof(int) * single->flow_converted->num_items);
	int * origin = (int *)single->flow_converted->data;

	for(int i = 0; i< single->flow_converted->num_items; i++) {
		flow_digital[i] = origin[i];
		printf("%d ", origin[i]);
	}
	*/
	

	LST_String * flow_converted = lst_string_new((int *)single->flow_converted->data, sizeof(int), single->flow_converted->num_items - 1);

	lst_stringset_add(set->flows_converted, flow_converted);

	//convertion_data_single_free(single);

	return ;

}




convertion_data_set_t * convertion_data_set_new(Trie * tokens) {

	convertion_data_set_t * data = (convertion_data_set_t *) malloc (sizeof(convertion_data_set_t));

	data->flows_converted = lst_stringset_new();

	data->tokens = tokens;

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
		trie_free(data->tokens);
	}
}


void flow_converted_int_print_cb(LST_String *flow_converted, void *data) {

	int * flow_digital = (int *) flow_converted->data;
	
	for (int i = 0; i < flow_converted->num_items; i++) {
		fprintf(fp, "%3d ", flow_digital[i]);
		
		if(i == flow_converted->num_items - 1) {
			fprintf(fp,"\n");
		}
	}

	return ;

}


void flow_converted_print_cb(LST_String *flow_converted, void *data) {

	int * flow_digital = (int *) flow_converted->data;
	
	for (int i = 0; i < flow_converted->num_items; i++) {
		if( flow_digital[i] > 255 || flow_digital[i] < 0) {
			printf(" %d", flow_digital[i]);
		} else {
			printf("%c", (char) flow_digital[i]);	
		}
		
		if(i == flow_converted->num_items - 1) {
			printf("\n");
		}
	}

	return;

}

void convertion_main(Trie * tokens, LST_StringSet *flows){

	fp = fopen("./http64.dat", "w");
	if(!fp) {
		perror("Open file failure!");
		return;
	}


	convertion_data_set_t * data = convertion_data_set_new(tokens);	

	lst_stringset_foreach(flows, convert_flow_cb, data);	

	lst_stringset_foreach(data->flows_converted, flow_converted_int_print_cb, NULL);
	//lst_stringset_foreach(data->flows_converted, flow_converted_print_cb, NULL);

	convertion_data_set_free(data);
	
	fclose(fp);
	fp = NULL;

}


