#ifdef _POSITION_CONSTRAINTS
#define _POSITION_CONSTRAINTS

#include "lst_string.h"
#include "queue.h"


/* flow vector:= {<token_1, offset_1>, <token_2, offset_2, ...>}, where we call the <token_i, offset_i> as a element*/

struct _element_t {
	LST_String *token;
	u_int offset;
	LIST_ENTRY(_element_t) entry;
};

struct _flow_t {
	LIST_HEAD(element_list, element_t) element_head;
};

struct _token_t {

	LST_String *token;

	LIST_HEAD(offset_list, offset_t) offset_head; // store all occur offset positions 

	u_int positon_specific; 

	u_int begin_of_flow;

	LIST_ENTRY(_token_t) entry;

};

struct _offset_t {
	u_int offset;
	u_int num_variants; 

	LIST_HEAD(token_list, token_t) token_head;
};

typedef struct _token_t 	token_t;
typedef struct _offset_t	offset_t; 
typedef struct _element_t 	element_t;
typedef struct _flow_t 		flow_t;

#endif
