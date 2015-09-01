#ifndef _PRODUCT_DISTRIBUTION_MODEL_H
#define _PRODUCT_DISTRIBUTION_MODEL_H

#include "lst_string.h"
#include "hash-table.h"


typedef struct _product_distribution_t product_distribution_t;
typedef struct _byte_distribution_t byte_distribution_t;


struct _product_distribution_t {

	int num_bytes;
	int first_bytes;
	int last_bytes;
	HashTable *offset_distribution;
};


struct _byte_distribution_t {

	int offset;	
	int num_values;
	HashTable *value_frequency; 

};


void byte_distribution_add(byte_distribution_t *bd, LST_String *nbytes, int offset);

byte_distribution_t * byte_distribution_new(LST_String *nbytes, int offset); 

void calculate_product_distribution_callback(LST_String *nbytes, void *data) ;

product_distribution_t * product_distribution_new(LST_StringSet * set, int first_bytes, int last_bytes, int num_bytes);

void product_distribution_free(product_distribution_t * pd) ;

/**
 * @param set	a set of flow with the data type of LST_String 
 */
int product_distribution_main(LST_StringSet * set, int first_bytes, int last_bytes, int num_bytes, int gamma_merge);

#endif
