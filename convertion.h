#ifndef _CONVERTION_H
#define _CONVERTION_H

#include "lst_string.h"
#include "trie.h"


typedef struct _convertion_data_set_t 		convertion_data_set_t;
typedef struct _convertion_data_single_t 	convertion_data_single_t;

struct _convertion_data_set_t {

	LST_StringSet 	*flows_converted;

	//LST_StringSet 	*tokens;

	Trie * tokens;

};


struct _convertion_data_single_t {

	LST_String 	*flow;
	LST_String 	*flow_converted;
	int 		*replacement;
	int 		first_flag;
	int 		replacement_len;
};

convertion_data_single_t * convertion_data_single_new(LST_String *flow) ;

void convertion_data_single_free(convertion_data_single_t * data);

int * string_replace(const char *src, const char *pattern, int replacement, int replacement_len, int *dst_old, int first_flag) ;

void convert_token_cb(TrieNode *node, void *data) ;

void convert_flow_cb(LST_String *flow, void *data) ;

convertion_data_set_t * convertion_data_set_new(Trie * tokens) ;

void convertion_data_set_free (convertion_data_set_t * data) ;

void flow_converted_print_cb(LST_String *flow_converted, void *data) ;

void flow_converted_int_print_cb(LST_String *flow_converted, void *data) ;

void convertion_main(Trie * tokens, LST_StringSet *flows);

#endif
