#ifndef _PATTERN_SEARCH_H
#define _PATTERN_SEARCH_H

void build_lps_array(char *pattern, int M, int *lps);

int* kmp_search(char *pattern, char *target);

#endif
