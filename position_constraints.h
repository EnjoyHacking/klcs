#ifndef _POSITION_CONSTRAINTS
#define _POSITION_CONSTRAINTS

#include "lst_string.h"
#include <sys/queue.h>
#include "hash-table.h"


#include "trie.h"

typedef enum
{
    ENCODED, ASCII, HEX, BIN
} dmode_t;

typedef struct _token_t 	token_t;
typedef struct _offset_t	offset_t; 
typedef struct _element_t 	element_t;
typedef struct _flow_t 		flow_t;
typedef struct _flow_set_t	flow_set_t;
typedef struct _offset_variants_t offset_variants_t; 

struct _offset_variants_t {
	int beta_merge;
	int k_offset;       // we only consider the offsets with occurrence greater than or equal to the given threshold 
	/* The hash table @offset_variants store the key-value pair of offset(int) and offset_t.*/
	HashTable *offset_variants; 
};

struct _element_t {
	LST_String *token;
	u_int offset;
	LIST_ENTRY(_element_t) entry;
};

/* flow vector:= {<token_1, offset_1>, <token_2, offset_2>, ...}, where we call the <token_i, offset_i> as a element*/
struct _flow_t {
	LIST_HEAD(element_list, _element_t) element_head;
	LIST_ENTRY(_flow_t) entry;
};


struct _flow_set_t {
	LIST_HEAD(flow_list, _flow_t) flow_head;	
	int size ; 	// the size of flow set
};

struct _token_t {

	LST_String *token;

	u_int position_specific; 

	u_int begin_of_flow;

	HashTable * offset_occurrence;

	LST_String *merge_token;

};

struct _offset_t {
	u_int offset;
	u_int num_variants; 

	LST_StringSet * variants;
};

typedef struct _data_for_flow_new_t {
	flow_set_t * flow_set;
	LST_StringSet * tokens;
} data_for_flow_new_t;


void flow_print(flow_t *flow);
flow_t * flow_new(LST_String *string, LST_StringSet *tokens);
void flow_free(flow_t *flow);

token_t * token_new(LST_String *token, int offset);
void token_add(token_t *t, int offset);
void token_print(token_t *t);
void token_free(token_t *t);

offset_t * offset_new(LST_String *token);
void offset_add(offset_t *o, LST_String *token);
void offset_free(offset_t *o);

flow_set_t * flow_set_new(void);
void flow_set_add(flow_set_t *flow_set, flow_t*flow);
void flow_set_free(flow_set_t *flow_set);

Trie* flow_set_traverse_token(flow_set_t *flow_set);


void print_callback(TrieNode *node, void *extension);
void search_callback(TrieNode *node, void *extension);
void variant_callback(LST_String *string, void *data);
void merge_callback(LST_String *string, void *data);

void merge_by_position_specific_with_offset_variants(Trie *tokens, offset_variants_t *ov);


void offset_variants_traverse(offset_variants_t *o);
void flow_new_cb(LST_String *string, void *data) ;

Trie * position_constraints_main(LST_StringSet * payloads, LST_StringSet * tokens, int k_offset, int beta_merge);



#endif
