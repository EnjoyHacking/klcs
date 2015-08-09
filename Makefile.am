AUTOMAKE_OPTIONS = foreign

CURRENTDIR = $(shell /bin/pwd)

bin_PROGRAMS = lcstext
lcstext_SOURCES = lcstext.c libstree.h lst_algorithms.c lst_algorithms.h lst_debug.c lst_debug.h lst_stree.c lst_stree.h \
		lst_string.h lst_string.c lst_structs.h lst_timestamp.h utils.h utils.c lst_macros.h

lcstext_LDADD = -lc -lm

lcstext_LDFLAGS = -L/lib 


