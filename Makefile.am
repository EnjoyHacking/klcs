AUTOMAKE_OPTIONS = foreign

CURRENTDIR = $(shell /bin/pwd)

bin_PROGRAMS = main lcstext test_position_constraints test_kmp_search test_product_distribution_model test_convertion test_merge_common_prefix

main_SOURCES =  main.c libstree.h lst_algorithms.c lst_algorithms.h lst_debug.c lst_debug.h lst_stree.c lst_stree.h \
		lst_string.h lst_string.c lst_structs.h lst_timestamp.h utils.h utils.c lst_macros.h hash-int.h hash-int.c \
		compare-int.c compare-int.h set.h set.c position_constraints.h position_constraints.c trie.h trie.c hash-table.h \
		hash-table.c pattern_search.h pattern_search.c product_distribution_model.h product_distribution_model.c \
		convertion.h convertion.c merge_common_prefix.h merge_common_prefix.c

main_LDADD = -lc -lm

main_LDFLAGS = -L/lib 




lcstext_SOURCES = lcstext.c libstree.h lst_algorithms.c lst_algorithms.h lst_debug.c lst_debug.h lst_stree.c lst_stree.h \
		lst_string.h lst_string.c lst_structs.h lst_timestamp.h utils.h utils.c lst_macros.h hash-int.h hash-int.c \
		compare-int.c compare-int.h set.h set.c
	

lcstext_LDADD = -lc -lm

lcstext_LDFLAGS = -L/lib 


test_position_constraints_SOURCES = test_position_constraints.c position_constraints.h position_constraints.c trie.h trie.c \
		lst_string.h lst_string.c hash-table.h hash-table.c hash-int.h hash-int.c compare-int.h compare-int.c \
		pattern_search.h pattern_search.c



test_kmp_search_SOURCES = test_kmp_search.c pattern_search.h pattern_search.c


test_product_distribution_model_SOURCES = test_product_distribution_model.c product_distribution_model.h product_distribution_model.c \
		lst_string.h lst_string.c hash-table.c hash-table.h hash-int.h hash-int.c compare-int.h compare-int.c trie.h trie.c \
		position_constraints.h position_constraints.c pattern_search.h pattern_search.c

test_convertion_SOURCES = test_convertion.c convertion.h convertion.c lst_string.h lst_string.c trie.h trie.c position_constraints.h \
		position_constraints.c hash-table.h hash-table.c hash-int.h hash-int.c compare-int.h compare-int.c pattern_search.h \
		pattern_search.c

test_merge_common_prefix_SOURCES = test_merge_common_prefix.c merge_common_prefix.c merge_common_prefix.h trie.h trie.c lst_string.h \
		lst_string.c pattern_search.h pattern_search.c

