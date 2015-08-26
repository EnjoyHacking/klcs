#ifndef _CONVERTION_H
#define _CONVERTION_H



typedef struct _convertion_data_set_t 	convertion_data_set_t;
typedef struct _convertion_data_single_t 	convertion_data_single_t;

struct _convertion_data_set_t {

	LST_StringSet *flows_converted;

	LST_StringSet *tokens;

};


struct _convertion_data_single_t {

	LST_String * flow;
	LST_String * flow_converted;
};

#endif
