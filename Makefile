# $Id: Makefile,v 1.3 2014/01/07 20:03:14 luis Exp $
# Author: Luis Colorado <lc@luiscoloradosistemas.com>
# Date: jue nov 21 23:48:42 CET 2013
# Disclaimer: (C) 2013 LUIS COLORADO SISTEMAS S.L.U.
# 		All rights reserved.

targets = tstavl tstavl2

.PHONY: all clean

all: $(targets)
clean:
	$(RM) $(targets) $(tstavl_objs)

tstavl_objs = avl.o tstavl.o stravl.o

tstavl: $(tstavl_objs)
	$(CC) $(LDFLAGS) -o tstavl $(tstavl_objs)

tstavl2_objs = avl.o tstavl2.o intavl.o
tstavl2: $(tstavl2_objs)
	$(CC) $(LDFLAGS) -o tstavl2 $(tstavl2_objs)

avl.o tstavl.o tstavl2.o: avl.h

# $Id: Makefile,v 1.3 2014/01/07 20:03:14 luis Exp $
