#!/usr/bin/env sh

gcc -Wall -O2 \
-o dtc \
-I. -Ilibfdt \
libfdt/fdt.c \
libfdt/fdt_empty_tree.c \
libfdt/fdt_ro.c \
libfdt/fdt_rw.c \
libfdt/fdt_strerror.c \
libfdt/fdt_sw.c \
libfdt/fdt_wip.c \
dtc.c \
flattree.c \
fstree.c \
data.c \
livetree.c \
treesource.c \
srcpos.c \
checks.c \
util.c \
dtc-lexer.lex.c \
dtc-parser.tab.c
