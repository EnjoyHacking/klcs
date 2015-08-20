#include "position_constraints.h"
#include "pattern_search.h"
#include "lst_string.h"

/*
flow_t *flow_new(LST_String *string, LST_StringSet *tokens){
	flow_t *flow;
	LIST_INIT(&(flow->element_head));
	LST_String *token;

	if (!tokens || !callback)
		return;
	int *indices;
	for (token = tokens->members.lh_first; token; token = tokens->set.le_next) {
		indices = kmp_search((char *)string->data, token);

		for(int i = 0; i < lst_string_get_length(string); i++) {
			element_t *element = (element_t *) malloc(sizeof(element_t));
			element->token = (char *)string->data;
			element->offset = indices[i];
			LIST_INSERT_HEAD(&(flow->element_head), element, entry);
		}
	}
	return flow;
	
}
*/



int main(int argc, char * argv[]) {

	//LST_String * flow1 = lst_string_new("HTTP1.1 index.html", 18);
	//LST_String * flow2 = lst_string_new("HTTP1.0 index.html", 18);

	flow_t flow;
	element_t element;

	return 0;
}
