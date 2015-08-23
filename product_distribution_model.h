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


#endif
