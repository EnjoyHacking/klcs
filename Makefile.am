AUTOMAKE_OPTIONS = foreign

CURRENTDIR = $(shell /bin/pwd)

bin_PROGRAMS = main lcstext position_constraints

main_SOURCES =  main.c libstree.h lst_algorithms.c lst_algorithms.h lst_debug.c lst_debug.h lst_stree.c lst_stree.h \
		lst_string.h lst_string.c lst_structs.h lst_timestamp.h utils.h utils.c lst_macros.h hash-int.h hash-int.c \
		compare-int.c compare-int.h set.h set.c position_constraints.h position_constraints.c trie.h trie.c hash-table.h \
		hash-table.c pattern_search.h pattern_search.c

main_LDADD = -lc -lm

main_LDFLAGS = -L/lib 



position_constraints_SOURCES = position_constraints.c position_constraints.h pattern_search.c pattern_search.h lst_string.c lst_string.h \
				trie.c trie.h hash-int.h hash-int.c compare-int.h compare-int.c hash-table.h hash-table.c


lcstext_SOURCES = lcstext.c libstree.h lst_algorithms.c lst_algorithms.h lst_debug.c lst_debug.h lst_stree.c lst_stree.h \
		lst_string.h lst_string.c lst_structs.h lst_timestamp.h utils.h utils.c lst_macros.h hash-int.h hash-int.c \
		compare-int.c compare-int.h set.h set.c
	

lcstext_LDADD = -lc -lm

lcstext_LDFLAGS = -L/lib 



position_constraints_SOURCES = position_constraints.c position_constraints.h pattern_search.c pattern_search.h lst_string.c lst_string.h \
				trie.c trie.h hash-int.h hash-int.c compare-int.h compare-int.c hash-table.h hash-table.c
