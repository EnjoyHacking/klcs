#ifndef _POSITION_CONSTRAINTS
#define _POSITION_CONSTRAINTS

#include "lst_string.h"
#include <sys/queue.h>
#include "hash-table.h"


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

};

struct _offset_t {
	u_int offset;
	u_int num_variants; 

	LST_StringSet * variants;
};


#endif
