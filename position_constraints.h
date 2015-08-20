#ifdef _POSITION_CONSTRAINTS
#define _POSITION_CONSTRAINTS

#include "lst_string.h"
#include "queue.h"

typedef _token_t 	token_t;
typedef _offset_t	offset_t; 

struct _token_t {

	LST_String *token;

	LIST_HEAD(offset_list, offset_t) offset_head; // store all occur offset positions 

	u_int positon_specific; 

	u_int begin_of_flow;

	LIST_ENTRY(_token_t);

};

struct _offset_t {
	u_int offset;
	u_int num_variants; 

	LIST_HEAD(token_list, token_t) token_head;
};


#endif
