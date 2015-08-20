/*

Copyright (C) 2003-2006 Christian Kreibich <christian@whoop.org>.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <math.h>
#include <string.h>
#include "lst_string.h"
#include "lst_stree.h"
#include "lst_structs.h"
#include "lst_debug.h"
#include "lst_algorithms.h"


static void bitstrings_copy(u_char *dst, u_char *src, int size);
static long convert_bitstrings_to_int(u_char* bitstrings, int bitstrings_size);
// add by syf
void
str_cb(LST_String *string, void *data)
{
  printf("%s ", lst_string_print(string));
  printf("%s", data);
}


typedef struct lst_node_it
{
  TAILQ_ENTRY(lst_node_it) items;
  LST_Node *node;
} LST_NodeIt;


static LST_NodeIt *
alg_node_it_new(LST_Node *node)
{
  LST_NodeIt *it;

  it = calloc(1, sizeof(LST_NodeIt));
  it->node = node;

  return it;
}

static void
alg_node_it_free(LST_NodeIt *it)
{
  if (!it)
    return;
  
  free(it);
}

static void 
alg_node_it_set_visited(LST_NodeIt *it)
{
	//it->node->lcs_visited = 1;
	LST_Node * tmp_node = it->node;
	while(!lst_node_is_root(tmp_node)){
		tmp_node->lcs_visited = 1;	
		tmp_node = tmp_node->up_edge->src_node;
	}

	return ;
}

void         
lst_alg_bfs(LST_STree *tree, LST_NodeVisitCB callback, void *data)
{
  LST_Node *node;
  LST_Edge *edge;
  TAILQ_HEAD(qhead, lst_node) queue;

  if (!tree || !callback)
    return;

  TAILQ_INIT(&queue);
  TAILQ_INSERT_HEAD(&queue, tree->root_node, iteration);
  
  while (queue.tqh_first)
    {
      node = queue.tqh_first;
      TAILQ_REMOVE(&queue, queue.tqh_first, iteration);
      
      if (callback(node, data))
	{
	  for (edge = node->kids.lh_first; edge; edge = edge->siblings.le_next)
	    TAILQ_INSERT_TAIL(&queue, edge->dst_node, iteration);
	}
    }

}


void         
lst_alg_dfs(LST_STree *tree, LST_NodeVisitCB callback, void *data)
{
  LST_Node *node;
  LST_Edge *edge;
  TAILQ_HEAD(shead, lst_node) stack;

  if (!tree || !callback)
    return;

  TAILQ_INIT(&stack);
  TAILQ_INSERT_HEAD(&stack, tree->root_node, iteration);

  while (stack.tqh_first)
    {

	  node = stack.tqh_first;
      TAILQ_REMOVE(&stack, stack.tqh_first, iteration);
      
      if (callback(node, data))
	{
	  for (edge = node->kids.lh_first; edge; edge = edge->siblings.le_next)
	    TAILQ_INSERT_HEAD(&stack, edge->dst_node, iteration);
	}      
    }
}


static int
alg_clear_busflag(LST_Node *node, void *data)
{
  node->bus_visited = 0;

  return 1;
  //data = NULL;
}


void
lst_alg_bus(LST_STree *tree, LST_NodeVisitCB callback, void *data)
{
  TAILQ_HEAD(nodes_s, lst_node_it) nodes;
  LST_Node *node;
  LST_NodeIt *it;

  TAILQ_INIT(&nodes);
  lst_alg_bfs(tree, alg_clear_busflag, NULL);

  if (!tree->leafs.lh_first)
    return;

  for (node = tree->leafs.lh_first; node; node = node->leafs.le_next)
    {
      callback(node, data);

      if (node == tree->root_node)
	continue;

      node->up_edge->src_node->bus_visited++;

      if (node->up_edge->src_node->bus_visited == 1)
	{
	  it = alg_node_it_new(node->up_edge->src_node);
	  TAILQ_INSERT_TAIL(&nodes, it, items);
	}
    }

  while (nodes.tqh_first)
    {
      it = nodes.tqh_first;
      node = it->node;
      TAILQ_REMOVE(&nodes, nodes.tqh_first, items);

      if (node->bus_visited < node->num_kids)
	{
	  TAILQ_INSERT_TAIL(&nodes, it, items);
	  continue;
	}

      callback(node, data);
      alg_node_it_free(it);

      if (node == tree->root_node)
        continue;

      node->up_edge->src_node->bus_visited++;

      if (node->up_edge->src_node->bus_visited == 1)
	{
	  it = alg_node_it_new(node->up_edge->src_node);
	  TAILQ_INSERT_TAIL(&nodes, it, items);
	}
    }
}


void         
lst_alg_leafs(LST_STree *tree, LST_NodeVisitCB callback, void *data)
{
  LST_Node *node;

  if (!tree || !callback)
    return;

  for (node = tree->leafs.lh_first; node; node = node->leafs.le_next)
    {
      if (callback(node, data) == 0)
	break;
    }
}


typedef struct lst_lcs_data
{
  LST_STree    *tree;

  /* 1 for longest common substring, 0 for longest repeated substring */
  int          lcs;

  /* Used in LCS case only */
  //u_int         all_visitors;
  u_char        *all_bitstrings;
  int           all_bitstrings_size;
  /* Used in k-LCS case only */
  int            k; // denote longest k common substring

  TAILQ_HEAD(nodes, lst_node_it) nodes;
  int           deepest;
  int           num_deepest;
  int           max_depth;

} LST_LCS_Data;

static int
alg_clear_visitors(LST_Node *node, void *data)
{
  //node->visitors = 0;
  LST_LCS_Data *tmp = (LST_LCS_Data *)data;
  node->bitstrings = (u_char *) malloc( sizeof(u_char) * (ceil((double)tmp->tree->num_strings / 8)) );
  memset(node->bitstrings, (u_char)0, sizeof(u_char) * ceil((double)tmp->tree->num_strings / 8));
  node->bitstrings_size = ceil((double)tmp->tree->num_strings / 8);
  return 1;
//  data = NULL;
}

static int
alg_set_visitors(LST_Node *node, LST_LCS_Data *data)
{
  if (lst_node_is_root(node))
    {
      //D(("Node %u: visitors %i\n", node->id, node->visitors));
	  printf("This is the root node. \n");
      return 1;
    }
  
  int string_index = lst_stree_get_string_index(data->tree, node->up_edge->range.string);
  //printf("%d : %s \n", string_index, lst_string_print(lst_node_get_string(node, 0)) );
  printf("%d\n", string_index);


  int block =  node->bitstrings_size - (int) ((double)string_index / 8) - 1;
  int offset = string_index % 8;

  printf("%d ---- %d \n", block, offset);

  if (lst_node_is_leaf(node))
    {

	  //printf("This is the leaf node. \n");
      u_char index = ((u_char)(1)) << offset;
      
      node->bitstrings[block] = index;
      node->up_edge->src_node->bitstrings[block] |= index;
    }
  else
    {
	  //printf("This is the internel node. \n");
      node->up_edge->src_node->bitstrings[block] |= node->bitstrings[block];
    }

  //printf ("node int : %ld\n", convert_bitstrings_to_int(node->up_edge->src_node->bitstrings, node->bitstrings_size)) ;
 // printf ("data int : %ld\n", convert_bitstrings_to_int(data->all_bitstrings, data->all_bitstrings_size));
  if (convert_bitstrings_to_int(node->up_edge->src_node->bitstrings, node->bitstrings_size) > convert_bitstrings_to_int(data->all_bitstrings, data->all_bitstrings_size))
    //data->all_bitstrings[block] = node->up_edge->src_node->bitstrings[block];
    bitstrings_copy(data->all_bitstrings, node->up_edge->src_node->bitstrings, node->bitstrings_size);

  //D(("Node %u: visitors %i\n", node->id, node->visitors));
  return 1;
}

u_char *
lst_alg_set_visitors(LST_STree *tree)
{
  LST_LCS_Data data;

  if (!tree)
    return 0;

  if (!tree->needs_visitor_update)
	  return tree->bitstrings;
	  //return tree->visitors;
  
  memset(&data, 0, sizeof(LST_LCS_Data));
  data.tree = tree;
  data.all_bitstrings = (u_char *)malloc(sizeof(u_char) * ceil((double)(data.tree)->num_strings / 8));
  memset(data.all_bitstrings, (u_char)0, sizeof(u_char) * ceil((double)(data.tree)->num_strings / 8));
  data.all_bitstrings_size = ceil((double)(data.tree)->num_strings / 8);
  
  /* First, establish the visitor bitstrings in the tree. */
  lst_alg_bus(tree, alg_clear_visitors, &data);
  lst_alg_bus(tree, (LST_NodeVisitCB) alg_set_visitors, &data);
  
  tree->needs_visitor_update = 0;
  bitstrings_copy(tree->bitstrings, data.all_bitstrings, data.all_bitstrings_size);
  //tree->visitors = data.all_visitors;
  //printf("In set visitors function . data.bitstrings : \n");
  //print_bitstrings(data.all_bitstrings, data.all_bitstrings_size);
  //printf ("Int : %ld\n", convert_bitstrings_to_int(data.all_bitstrings, data.all_bitstrings_size));


  return data.all_bitstrings;
}



/*
* add by syf
*/
static void bitstrings_copy(u_char *dst, u_char *src, int size){

	memcpy(dst, src, size);	

}
/*
* add by syf
*/
static long convert_bitstrings_to_int(u_char* bitstrings, int bitstrings_size){

	long res = 0;

	for(int block = 0; block < bitstrings_size; block++){
/*
		for(int offset = 1; offset <= 8; offset++){
			int bit_value = offset & bitstrings[block];
			printf("bit-strings[%d] : %d \n", block, bitstrings[block]);
			printf("bit-value : %d \n", bit_value);
			res += bit_value * (long)pow(2, offset - 1 + 8 * (bitstrings_size - block - 1));
		}
*/
		
			res += (int)bitstrings[block]* (long)pow(2, 8 * (bitstrings_size - block - 1));
	}
	return res;

}

/*
 * add
 * @author sangyafei
 * */
static int is_equal_bitstrings(u_char * b1, u_char * b2, int len)
{
	if (!b1 || !b2){
		return -1;
	}
	for (int i = 0; i < len; i++){
		if (b1[i] != b2[i]){
			return 0;
		}
	}
	return 1;
}




/*
 * add
 * @author sangyafei
 * */
int print_bitstrings(u_char * bitstrings, int len){

	if (!bitstrings)
		return -1;
	printf("bitstrings : ");
	for (int i = 0; i < len; i++){
		u_char chs[10];
		memset(chs, 0, 10);
		u_char bitstring = bitstrings[i];
		u_char TMP = 128;
		int j = 0;
		while (j < 8) {
			int tmp = 1;
			if ((bitstring & TMP) == 0) {
				tmp = 0;
			}
			char ch[2];
			memset(ch, 0, 2);
			sprintf(ch, "%d", tmp);
			strcat(chs, ch);
			bitstring = bitstring << 1;
			j++;
		}
		printf("%s ", chs);
	}
	printf("\n");
	return 1;
}

/*
 * add
 * @author sangyafei
 * */
int get_number_of_distinct_string(LST_Node *node, int size){
	int counter = 0;
	int i = 0;
	for (i = 0; i < size; i++){
		u_char bitstring = node->bitstrings[i];
		while (bitstring) {
			if (bitstring & 1){
				counter++;
			}
			bitstring = bitstring >> 1;
		}
	}
	return counter;
}


/*
 * modify
 * @author sangyafei
 * */
static int
alg_find_deepest(LST_Node *node, LST_LCS_Data *data)
{
  LST_NodeIt *it;
  int depth = lst_node_get_string_length(node);
  /*printf("node->bitstrings : \n");
  print_bitstrings(node->bitstrings, node->bitstrings_size);
  printf("data->bitstrings : \n");
  print_bitstrings(data->all_bitstrings, node->bitstrings_size);
*/
  

  if (data->lcs == 1)  // denote longest common substring
    { 
	
	  if ( !is_equal_bitstrings(node->bitstrings, data->all_bitstrings, ceil((double)data->tree->num_strings / 8))){
	  	//printf("bitstrings is not equal....\n");
	  	
		  return 0;
	  } 

    }
  else if (data->lcs == 2) // denote longest k common substring
  {
	  int counter = get_number_of_distinct_string(node, ceil((double)data->tree->num_strings / 8));
	  node->num_distinct_strings = counter;
	  //printf("%d < %d : \n", counter, data->k);
	  if ( counter < data->k){
          	return 0;
	  }

  }
  else
    {
      if (node->num_kids < 1)
       return 0;
    }


//	printf("============\n");
//	print_bitstrings(node->bitstrings, node->bitstrings_size);
//	printf("%s \n",lst_string_print(lst_node_get_string(node, data->max_depth)));



	//printf("if depth : %d \n", depth);
	//printf("if deepest : %d \n", data->deepest);
  if (data->deepest <= data->max_depth)
    {
      if (depth >= data->deepest && depth <= data->max_depth) // modify by syf
	{
	  it = alg_node_it_new(node);
	  
	  if (depth > data->deepest)
	    {
	      data->deepest = depth;
	      data->num_deepest = 0;
	    }
	  
	  data->num_deepest++;
	  TAILQ_INSERT_HEAD(&data->nodes, it, items);

	//printf("depth : %d \n", depth);
	//printf("deepest : %d \n", data->deepest);
	//printf("--------------------\n");
	}
    }

  else if (depth >= data->max_depth)
    {
      it = alg_node_it_new(node);
      data->num_deepest++;
      TAILQ_INSERT_HEAD(&data->nodes, it, items);
    }


  return 1;
}

static LST_StringSet *
alg_longest_substring(LST_STree *tree, u_int min_len, u_int max_len, int lcs, int k, u_int *num_distinct_strings)
{
  LST_StringSet *result = NULL;
  LST_String *string;
  LST_LCS_Data data;
  LST_NodeIt *it;

  if (!tree)
    return NULL;

  memset(&data, 0, sizeof(LST_LCS_Data));
  data.tree = tree;
  data.lcs = lcs;
  //data.tree->root_node->bitstrings =(u_char *)malloc(sizeof(u_char) * ceil((double)(data.tree)->num_strings / 8));
  //memset(data.tree->root_node->bitstrings, (u_char)0, sizeof(u_char) * ceil((double)(data.tree)->num_strings / 8));
  data.all_bitstrings = (u_char *)malloc(sizeof(u_char) * ceil((double)(data.tree)->num_strings / 8));
  memset(data.all_bitstrings, (u_char)0, sizeof(u_char) * ceil((double)(data.tree)->num_strings / 8));
  data.all_bitstrings_size = ceil((double)(data.tree)->num_strings / 8);

  tree->bitstrings = (u_char *)malloc(sizeof(u_char) * ceil((double)(data.tree)->num_strings / 8));
  memset(tree->bitstrings, (u_char)0, sizeof(u_char) * ceil((double)(data.tree)->num_strings / 8));

  if (lcs)
    data.all_bitstrings = lst_alg_set_visitors(tree);

  if (lcs == 2)
	  data.k = k;

  if (max_len > 0)
    data.max_depth = (int) max_len;
  else
    data.max_depth = INT_MAX;

  TAILQ_INIT(&data.nodes);

  /* Now do a DSF finding the node with the largest string-
   * depth that has all strings as visitors.
   */
  LST_STree * lcs_tree = NULL; 
  u_int idx = 0;


  while(data.max_depth >= min_len){

  printf("max_depth : %d\n", data.max_depth);
  lst_alg_dfs(tree, (LST_NodeVisitCB) alg_find_deepest, &data);


  D(("Deepest nodes found -- we have %u longest substring(s) at depth %u.\n",
     data.num_deepest, data.deepest));
  printf("we have %u longest substring(s) at depth %u.\n",data.num_deepest, data.deepest);

  
  /* Now, data.num_deepest tells us how many largest substrings
   * we have, and the first num_deepest items in data.nodes are
   * the end nodes in the suffix tree that define these substrings.
   */  
  while ( (it = data.nodes.tqh_first))
    {
      if (--data.num_deepest >= 0)
	{
	  /* Get our longest common string's length, and if it's
	   * long enough for our requirements, put it in the result
	   * set. We need to allocate that first if we haven't yet
	   * inserted any strings.
	   */
	  if ((u_int) lst_node_get_string_length(it->node) >= min_len)
	    {
	      string = lst_node_get_string(it->node, (int) max_len);

	      if (!result)
		result = lst_stringset_new();
	      
	      // add by syf 
	      if(1 != lst_alg_substring_check(lcs_tree, string)) {
		      lst_stringset_add(result, string);
		      if(num_distinct_strings != NULL)
			      num_distinct_strings[idx++] = it->node->num_distinct_strings;
	      }

	    }
	}
      
      TAILQ_REMOVE(&data.nodes, it, items);
      alg_node_it_free(it);
    }
	if (result)
    {
        printf("result size : %d \n", result->size);
        lst_stringset_foreach(result, str_cb, "\t");
        printf("\n");
	lcs_tree = lst_stree_new(result);
    }

   data.max_depth = data.deepest - 1;
   data.num_deepest = 0;
   data.deepest = 0;

	printf("-------------------------\n");
	}
  return result;
}

/*
 * add
 * @author sangyafei
 * @brief extract first k longest common substring (even, all common substrings) among the given multiple strings
 * */
LST_StringSet *
lst_alg_first_k_longest_common_substring(LST_STree *tree, u_int min_len, u_int max_len, u_int * extension)
{
  return alg_longest_substring(tree, min_len, max_len, 1, 0, extension);
}

LST_StringSet *
lst_alg_longest_common_substring(LST_STree *tree, u_int min_len, u_int max_len, u_int * extension)
{
  return alg_longest_substring(tree, min_len, max_len, 1, 0, extension);
}



/*
 * add
 * @author sangyafei
 * */
LST_StringSet *
lst_alg_k_longest_common_substring(LST_STree *tree, u_int min_len, u_int max_len, int k, u_int  *extension)
{
  return alg_longest_substring(tree, min_len, max_len, 2, k, extension);
}


static int alg_edge_length(LST_Node *node) 
{
	if(lst_node_is_root(node)) {
		return 0;
	}

	if(!node){
		return 0;
	}

	return *(node->up_edge->range.end_index) - node->up_edge->range.start_index + 1;
}

static LST_Node* alg_find_child(LST_Node *node, char ch)
{
 	LST_Edge *edge;
	char *val;
	for (edge = node->kids.lh_first; edge; edge = edge->siblings.le_next) {
		val = (char *) lst_string_get_item(edge->range.string, edge->range.start_index);
		if(*val == ch){
			return edge->dst_node;
		}
	}
	return NULL;

	
}

static int alg_traverse_edge(LST_Edge *edge, LST_String *string, int idx, int start, int end) 
{
	int k = 0;
	/*Traverse the edge with character by character matching*/
	char * edge_ch;
	char * need_match_ch;
	for(k = start; k <= end && idx < lst_string_get_length(string); k++, idx++) {
		char * edge_ch = (char *) lst_string_get_item(edge->range.string, k);
		char * need_match_ch = (char *) lst_string_get_item(string, idx);
		if(*edge_ch != *need_match_ch) {
			return -1;	
		}
	}
	need_match_ch = (char *) lst_string_get_item(string, idx);
	if(*need_match_ch == '\0') {
		return 1; // match
	}

	return 0; // more charachers yet to match

}

static int alg_substring_check(LST_Node *node, LST_String *string, int idx)
{

	int res = -1;
	if(!node){
		return -1; // no match
	}

	/* If node n is not root node, then traverse edge from node n's parent to node n.*/
	if(!lst_node_is_root(node)){
		res = alg_traverse_edge(node->up_edge, string, idx, node->up_edge->range.start_index, *(node->up_edge->range.end_index)); 
		if(res != 0){
			return res; // match (res = 1) or no match (res = -1)
		}
	}
	/* Get the character index to search*/
	idx += lst_edge_get_length(node->up_edge);

	/* If there is an edge from node n going out with current character ch, travrse that edge */
	char *ch = lst_string_get_item(string, idx);
	LST_Node * child_node = NULL;
	if((child_node = alg_find_child(node, *ch)) != NULL) {
		return alg_substring_check(child_node, string, idx);
	} else {
		return -1; // no match
	}

}

int lst_alg_substring_check(LST_STree *tree, LST_String *string)
{

	if(!tree || !string || !(tree->root_node)) {
		return -1;
	}

	return alg_substring_check(tree->root_node, string, 0);
}


LST_StringSet *
lst_alg_longest_repeated_substring(LST_STree *tree, u_int min_len, u_int max_len, u_int *extension)
{
  return alg_longest_substring(tree, min_len, max_len, 0, 0, extension);
}

